/********************************************************************
** @source jembossctl
**
** @author Copyright (C) 2002 Alan Bleasby
** @version 1.0 
** @modified Mar 02 2002 ajb First version
** @@
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
********************************************************************/

#ifdef HAVE_JAVA

#include "emboss.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include <sys/file.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>

#define UIDLIMIT 100
#define GIDLIMIT 100


static AjBool jctl_up(char *buf,int *uid,int *gid,AjPStr *home);
static AjBool jctl_do_fork(char *buf, int uid, int gid);
static AjBool jctl_do_directory(char *buf, int uid, int gid);
static AjBool jctl_do_deletefile(char *buf, int uid, int gid);
static AjBool jctl_do_deletedir(char *buf, int uid, int gid);
static AjBool jctl_do_listfiles(char *buf, int uid, int gid, AjPStr *retlist);
static AjBool jctl_do_listdirs(char *buf, int uid, int gid, AjPStr *retlist);
static AjBool jctl_do_getfile(char *buf, int uid, int gid,
			      unsigned char **fbuf, int *size, int sockdes);
static AjBool jctl_do_putfile(char *buf, int uid, int gid, int sockdes);

static char **jctl_make_array(AjPStr str);
static void jctl_tidy_strings(AjPStr *tstr, AjPStr *home, AjPStr *retlist,
			      char *buf);
static void jctl_fork_tidy(AjPStr *cl, AjPStr *prog, AjPStr *enviro,
			   AjPStr *dir, AjPStr *outstd, AjPStr *errstd);



#include <pwd.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <crypt.h>

#ifdef SHADOW
#include <shadow.h>
#endif
#ifdef RSHADOW
#include <shadow.h>
#endif

#ifdef PAM
#include <security/pam_appl.h>
#endif

#ifdef AIX_SHADOW
#include <userpw.h>
#endif

#ifdef HPUX_SHADOW
#include <hpsecurity.h>
#include <prot.h>
#endif

#define R_BUFFER 2048		/* Reentrant buffer size */


static void jctl_empty_core_dump(void);
#ifndef NO_AUTH
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
			      ajint *gid, AjPStr *home);
#endif

#ifdef PAM
static int jctl_pam_conv(int num_msg, struct pam_message **msg,
			 struct pam_response **resp, void *appdata_ptr);
#endif

#define JBUFFLEN 10000




/* @prog jembossctl **********************************************************
**
** Slave suid program for Jemboss
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr message=NULL;
    AjPStr sockname=NULL;
    char *pname=NULL;
    struct sockaddr_un there;
    int sockdes;
    int nlen;
    char *buf=NULL;
    int mlen;
    int command=0;
    
    int uid;
    int gid;
    AjPStr home=NULL;
    AjBool ok=ajFalse;
    char c='\0';
    AjPStr tstr = NULL;
    AjPStr retlist=NULL;
    unsigned char *fbuf=NULL;
    int size;
    
    embInit("jembossctl",argc,argv);


    sockname = ajAcdGetString("sock");
    pname = ajStrStr(sockname);


    home = ajStrNew();
    tstr = ajStrNew();
    retlist = ajStrNew();
    
    if(!(buf=(char *)malloc(JBUFFLEN+1)))
    {
	jctl_tidy_strings(&tstr,&home,&retlist,buf);
	fprintf(stderr,"jni buf malloc error (jembossctl)\n");
	exit(-1);
    }
    
    bzero((void*)buf,JBUFFLEN+1);
    
    jctl_empty_core_dump();
    
    
    if((sockdes=socket(AF_UNIX,SOCK_STREAM,0)) == -1)
    {
	jctl_tidy_strings(&tstr,&home,&retlist,buf);
	fprintf(stderr,"jni socket error (jembossctl)\n");
	exit(-1);
    }

    there.sun_family = AF_UNIX;
    strcpy(there.sun_path,pname);
    nlen = ajStrLen(sockname) + sizeof(there.sun_family);
    if(connect(sockdes,(struct sockaddr*)&there,nlen)==-1)
    {
	jctl_tidy_strings(&tstr,&home,&retlist,buf);
	fprintf(stderr,"jni connect error (jembossctl)\n");
	exit(-1);
    }
    


    message = ajStrNewC("OK");
    if(send(sockdes,ajStrStr(message),ajStrLen(message),0)==-1)
    {
	jctl_tidy_strings(&tstr,&home,&retlist,buf);
	ajStrDel(&message);
	fprintf(stderr,"jni send error (jembossctl)\n");
	exit(-1);
    }
    


    /* Wait for a command from jni */
    if((mlen = recv(sockdes,buf,JBUFFLEN,0)) < 0)
    {
	jctl_tidy_strings(&tstr,&home,&retlist,buf);
	ajStrDel(&message);
	fprintf(stderr,"jni recv error (jembossctl)\n");
	exit(-1);
    }

    if(sscanf(buf,"%d",&command)!=1)
    {
	jctl_tidy_strings(&tstr,&home,&retlist,buf);
	ajStrDel(&message);
	fprintf(stderr,"jni sscanf error (jembossctl)\n");
	exit(-1);
    }

    

    switch(command)
    {
    case COMM_AUTH:
	ajStrAssC(&tstr,buf);
	c='\0';
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    c=1;
	if((mlen = send(sockdes,&c,1,0)) < 0)
	{
	    jctl_tidy_strings(&tstr,&home,&retlist,buf);
	    ajStrDel(&message);
	    fprintf(stderr,"jni command send error (auth)\n");
	    exit(-1);
	}
	fprintf(stdout,"%s",ajStrStr(home));
	break;

    case EMBOSS_FORK:
	ajStrAssC(&tstr,buf);
	c='\0';
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);

	if(ok)
	{
	    ok = jctl_do_fork(buf,uid,gid);
	    if(ok)
		c=1;
	}
	break;

    case MAKE_DIRECTORY:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_directory(buf,uid,gid);
	break;

    case DELETE_FILE:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_deletefile(buf,uid,gid);
	break;

    case DELETE_DIR:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_deletedir(buf,uid,gid);
	break;

    case LIST_FILES:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_listfiles(buf,uid,gid,&retlist);
	fprintf(stdout,"%s",ajStrStr(retlist));
	break;

    case LIST_DIRS:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_listdirs(buf,uid,gid,&retlist);
	fprintf(stdout,"%s",ajStrStr(retlist));
	break;

    case GET_FILE:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_getfile(buf,uid,gid,&fbuf,&size,sockdes);

	break;

    case PUT_FILE:
	ajStrAssC(&tstr,buf);
	ok = jctl_up(ajStrStr(tstr),&uid,&gid,&home);
	if(ok)
	    ok = jctl_do_putfile(buf,uid,gid,sockdes);

	break;

    default:
	break;
    }
    

    ajStrDel(&message);
    jctl_tidy_strings(&tstr,&home,&retlist,buf);
    

    ajExit();
    return 0;
}



/* @funcstatic jctl_empty_core_dump ******************************************
**
** Set process coredump size to be zero
**
** @return [void]
** @@
******************************************************************************/

static void jctl_empty_core_dump()
{
    struct rlimit limit;

    limit.rlim_cur = 0;
    limit.rlim_max = 0;

    setrlimit(RLIMIT_CORE,&limit);

    return;
}




#ifdef SHADOW
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home)
{
    struct spwd *shadow = NULL;
    struct passwd *pwd  = NULL;
    char *p = NULL;
    

    shadow = getspnam(ajStrStr(username));
    
    if(!shadow)                 /* No such username */
        return ajFalse;
    

    pwd = getpwnam(ajStrStr(username));
    
    if(!pwd)
        return ajFalse;
    
    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);
    
    p = crypt(ajStrStr(password),shadow->sp_pwdp);

    if(!strcmp(p,shadow->sp_pwdp))
        return ajTrue;

    return ajFalse;
}
#endif



#ifdef RSHADOW
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home)
{
    struct spwd *shadow = NULL;
    struct spwd sresult;
    struct passwd *pwd  = NULL;
    struct passwd *presult;
    char *p = NULL;
    char *sbuf = NULL;
    char *buf  = NULL;

    if(!(buf=(char*)malloc(R_BUFFER)) || !(sbuf=(char*)malloc(R_BUFFER)))
	return ajFalse;

    shadow = getspnam_r(ajStrStr(username),&sresult,sbuf,R_BUFFER);
    
    if(!shadow)                 /* No such username */
    {
	AJFREE(buf);
	AJFREE(sbuf);
        return ajFalse;
    }
    
    

    pwd = getpwnam_r(ajStrStr(username),&result,buf,R_BUFFER);
    
    if(!pwd)
    {
	AJFREE(buf);
	AJFREE(sbuf);
        return ajFalse;
    }
    
    
    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);
    
    p = crypt(ajStrStr(password),shadow->sp_pwdp);

    if(!strcmp(p,shadow->sp_pwdp))
    {
	AJFREE(buf);
	AJFREE(sbuf);
        return ajTrue;
    }

    AJFREE(buf);
    AJFREE(sbuf);

    return ajFalse;
}
#endif



#ifdef AIX_SHADOW
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home)
{
    struct userpw *shadow = NULL;
    struct passwd *pwd  = NULL;
    char *p = NULL;

    shadow = getuserpw(ajStrStr(username));
    if(!shadow)	
	return ajFalse;

    pwd = getpwnam(ajStrStr(username));
    if(!pwd)
	return ajFalse;

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),shadow->upw_passwd);

    if(!strcmp(p,shadow->upw_passwd))
	return ajTrue;

    return ajFalse;
}
#endif



#ifdef HPUX_SHADOW
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
		      ajint *gid, AjPStr *home)
{
    struct pr_passwd *shadow = NULL;
    struct passwd pwd;
    
    char *p = NULL;

    shadow = getprpwnam(ajStrStr(username));
    if(!shadow)	
	return ajFalse;

    *uid = shadow->ufld.fd_uid;

    
    pwd = getpwnam(ajStrStr(username));
    if(!pwd)
	return ajFalse;
    
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),shadow->ufld.fd_encrypt);

    if(!strcmp(p,shadow->ufld.fd_encrypt))
	return ajTrue;

    return ajFalse;
}
#endif


#ifdef NO_SHADOW
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
		      ajint *gid, AjPStr *home)
{
    struct passwd *pwd  = NULL;
    char *p = NULL;
    
    pwd = getpwnam(ajStrStr(username));
    if(!pwd)		 /* No such username */
	return ajFalse;

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),pwd->pw_passwd);

    if(!strcmp(p,pwd->pw_passwd))
	return ajTrue;

    return ajFalse;
}
#endif



#ifdef RNO_SHADOW
static AjBool jctl_check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home)
{
    struct passwd *pwd  = NULL;
    char *p = NULL;
    struct passwd result;
    char *buf=NULL;

    if(!(buf=(char *)malloc(R_BUFFER)))
	return ajFalse;
    
    pwd = getpwnam_r(ajStrStr(username),&result,buf,R_BUFFER);
    if(!pwd)		 /* No such username */
    {
	AJFREE(buf);
	return ajFalse;
    }

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),pwd->pw_passwd);

    if(!strcmp(p,pwd->pw_passwd))
    {
	AJFREE(buf);
	return ajTrue;
    }

    AJFREE(buf);

    return ajFalse;
}
#endif



#ifdef PAM

struct ad_user
{
    char *username;
    char *password;
};


static int jctl_pam_conv(int num_msg, struct pam_message **msg,
		    struct pam_response **resp, void *appdata_ptr)
{
    struct ad_user *user=(struct ad_user *)appdata_ptr;
    struct pam_response *response;
    int i;

    if (msg == NULL || resp == NULL || user == NULL)
    	return PAM_CONV_ERR;
    
    response= (struct pam_response *)
    	malloc(num_msg * sizeof(struct pam_response));
    
    for(i=0;i<num_msg;++i)
    {
	response[i].resp_retcode = 0;
	response[i].resp = NULL;

	switch(msg[i]->msg_style)
	{
	case PAM_PROMPT_ECHO_ON:
	    /* Store the login as the response */
	    response[i].resp = appdata_ptr ?
		(char *)strdup(user->username) : NULL;
	    break;

	case PAM_PROMPT_ECHO_OFF:
	    /* Store the password as the response */
	    response[i].resp = appdata_ptr ?
		(char *)strdup(user->password) : NULL;
	    break;

	case PAM_TEXT_INFO:
	case PAM_ERROR_MSG:
	    break;

	default:
	    if(response)
		free(response);
	    return PAM_CONV_ERR;
	}
    }

    /* On success, return the response structure */
    *resp= response;
    return PAM_SUCCESS;
}



/* @funcstatic jctl_check_pass ********************************************
**
** Check username/password. Return uid/gid/homedir
**
** @param [r] username [AjPStr] username
** @param [r] password [AjPStr] password
** @param [w] uid [ajint*] uid
** @param [w] gid [ajint*] gid
** @param [w] home [AjPStr*] home
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_check_pass(AjPStr username,AjPStr password,ajint *uid,
			      ajint *gid,AjPStr *home)
{
    struct ad_user user_info;

    struct pam_cv
    {
	int (*cv)(int,struct pam_message **,struct pam_response **,void *);
	void *userinfo;
    };

    struct pam_cv conv;
    pam_handle_t *pamh = NULL;
    int retval;

    struct passwd *pwd = NULL;

    user_info.username = ajStrStr(username);
    user_info.password = ajStrStr(password);

    conv.cv = jctl_pam_conv;
    conv.userinfo = (void *)&user_info;
    
    pwd = getpwnam(ajStrStr(username));
    if(!pwd)		 /* No such username */
	return ajFalse;

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    retval = pam_start("emboss_auth",ajStrStr(username),
		       (struct pam_conv*)&conv,&pamh);
    
    if (retval == PAM_SUCCESS)
	retval= pam_authenticate(pamh,PAM_SILENT);
    
    if(retval==PAM_SUCCESS)
	retval = pam_acct_mgmt(pamh,0);
    
    if(pam_end(pamh,retval)!=PAM_SUCCESS)
    {
	pamh = NULL;
	return ajFalse;
    }

    if(retval==PAM_SUCCESS)
	return ajTrue;

    return ajFalse;
}
#endif


/* @funcstatic jctl_up ********************************************
**
** Primary username/password check. Return uid/gid/homedir
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [ajint*] uid
** @param [w] gid [ajint*] gid
** @param [w] home [AjPStr*] home
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_up(char *buf, int *uid, int *gid, AjPStr *home)
{
    AjPStr username=NULL;
    AjPStr password=NULL;
    AjPStr cstr=NULL;
    ajint command;
    AjBool ok=ajFalse;

    username = ajStrNew();
    password = ajStrNew();
    cstr     = ajStrNew();

    ajStrAssC(&cstr,buf);
    ajFmtScanS(cstr,"%d%S%S",&command,&username,&password);

    
#ifndef NO_AUTH
    ok = jctl_check_pass(username,password,uid,gid,home);
#endif


    bzero((void*)ajStrStr(username),ajStrLen(username));
    bzero((void*)ajStrStr(password),ajStrLen(password));
    bzero((void *)buf,ajStrLen(password)+ajStrLen(username)+4);


    ajStrDel(&username);
    ajStrDel(&password);
    ajStrDel(&cstr);

    if(*uid<UIDLIMIT || *gid<GIDLIMIT)
	ok = ajFalse;
    if(!*uid || !*gid)
	ok = ajFalse;
    
    if(ok)
	return ajTrue;

    return ajFalse;
}



/* @funcstatic jctl_do_fork **************************************************
**
** Fork emboss program
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [ajint*] uid
** @param [w] gid [ajint*] gid
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_fork(char *buf, int uid, int gid)
{
    AjPStr cl     = NULL;
    AjPStr prog   = NULL;
    AjPStr enviro = NULL;
    AjPStr dir    = NULL;
    AjPStrTok handle=NULL;

    char *p=NULL;


    /* Fork stuff */
    char **argp=NULL;
    char **envp=NULL;
    int  pid;
    int  status = 0;
    int  i=0;

    int  outpipe[2];
    int  errpipe[2];
    
    fd_set rec;
    struct timeval t;
    int nread=0;

    AjPStr outstd=NULL;
    AjPStr errstd=NULL;
    int retval=0;

    outstd = ajStrNew();
    errstd = ajStrNew();

    cl     = ajStrNew();
    prog   = ajStrNew();
    enviro = ajStrNew();
    dir    = ajStrNew();


    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve command line, environment and directory */
    ajStrAssC(&cl,p);
    while(*p)
	++p;
    ++p;

    ajStrAssC(&enviro,p);
    while(*p)
	++p;
    ++p;

    ajStrAssC(&dir,p);

    handle = ajStrTokenInit(cl," \t\n");
    ajStrToken(&prog,&handle,NULL);
    ajStrTokenClear(&handle);


    argp = jctl_make_array(cl);
    envp = jctl_make_array(enviro);

    if(!ajSysWhichEnv(&prog,envp))
    {
	jctl_fork_tidy(&cl,&prog,&enviro,&dir,&outstd,&errstd);
	return ajFalse;
    }
    

    pipe(outpipe);
    pipe(errpipe);



    pid = fork();
    if(pid == -1)
    {
	close(errpipe[0]);
	close(errpipe[1]);
	close(outpipe[0]);
	close(outpipe[1]);
	jctl_fork_tidy(&cl,&prog,&enviro,&dir,&outstd,&errstd);
	return ajFalse;
    }
    
    
    if(!pid)			/* Child */
    {
	dup2(outpipe[1],1);
	dup2(errpipe[1],2);
	if(setgid(gid)==-1)
	{
	    fprintf(stderr,"setgid failure");
	    exit(-1);
	}
	if(setuid(uid)==-1)
	{
	    fprintf(stderr,"setuid failure");
	    exit(-1);
	}
	if(chdir(ajStrStr(dir))==-1)
	{
	    fprintf(stderr,"chdir failure");
	    exit(-1);
	}
	if(execve(ajStrStr(prog),argp,envp) == -1)
	{
	    fprintf(stderr,"execve failure");
	    exit(-1);
	}
    }


    while((retval=waitpid(pid,&status,WNOHANG))!=pid && !retval)
    {
	FD_ZERO(&rec);
	FD_SET(outpipe[0],&rec);
	t.tv_sec = 0;
	t.tv_usec = 0;
	select(outpipe[0]+1,&rec,NULL,NULL,&t);
	if(FD_ISSET(outpipe[0],&rec))
	{
	    nread = read(outpipe[0],(void *)buf,JBUFFLEN);
	    buf[nread]='\0';
	    ajStrAppC(&outstd,buf);
	}

	FD_ZERO(&rec);
	FD_SET(errpipe[0],&rec);
	t.tv_sec = 0;
	t.tv_usec = 0;
	select(errpipe[0]+1,&rec,NULL,NULL,&t);
	if(FD_ISSET(errpipe[0],&rec))
	{
	    nread = read(errpipe[0],(void *)buf,JBUFFLEN);
	    buf[nread]='\0';
	    ajStrAppC(&errstd,buf);
	}


    }


    FD_ZERO(&rec);
    FD_SET(outpipe[0],&rec);
    t.tv_sec = 0;
    t.tv_usec = 0;
    select(outpipe[0]+1,&rec,NULL,NULL,&t);
    if(FD_ISSET(outpipe[0],&rec))
    {
	nread = read(outpipe[0],(void *)buf,JBUFFLEN);
	buf[nread]='\0';
	ajStrAppC(&outstd,buf);
    }


    FD_ZERO(&rec);
    FD_SET(errpipe[0],&rec);
    t.tv_sec = 0;
    t.tv_usec = 0;
    select(errpipe[0]+1,&rec,NULL,NULL,&t);
    if(FD_ISSET(errpipe[0],&rec))
    {
	nread = read(errpipe[0],(void *)buf,JBUFFLEN);
	buf[nread]='\0';
	ajStrAppC(&errstd,buf);
    }

    fprintf(stdout,"%s",ajStrStr(outstd));
    fprintf(stderr,"%s",ajStrStr(errstd));
    

    close(errpipe[0]);
    close(errpipe[1]);
    close(outpipe[0]);
    close(outpipe[1]);

    i = 0;
    while(argp[i])
	AJFREE(argp[i]);
    AJFREE(argp);

    i = 0;
    while(envp[i])
	AJFREE(envp[i]);
    AJFREE(envp);

    jctl_fork_tidy(&cl,&prog,&enviro,&dir,&outstd,&errstd);

    return ajTrue;
}



/* @funcstatic jctl_make_array ********************************************
**
** Construct argv and env arrays for Ajax.fork
**
** @param [r] str [AjPStr] space separated tokens
**
** @return [char**] env or argv array
******************************************************************************/

static char **jctl_make_array(AjPStr str)
{
    int n;
    char **ptr=NULL;
    AjPStrTok handle;
    AjPStr token;
    
    token = ajStrNew();
    
    n = ajStrTokenCount(&str," \t\n");

    AJCNEW0(ptr,n+1);

    ptr[n] = NULL;

    n = 0;
    
    handle = ajStrTokenInit(str," \t\n");
    while(ajStrToken(&token,&handle,NULL))
	ptr[n++] = ajCharNew(token);

    ajStrTokenClear(&handle);
    ajStrDel(&token);
    
    return ptr;
}


/* @funcstatic jctl_do_directory *********************************************
**
** Make user directory
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int] uid
** @param [w] gid [int] gid
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_directory(char *buf, int uid, int gid)
{
    AjPStr dir    = NULL;
    char *p=NULL;


    dir    = ajStrNew();


    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve directory */
    ajStrAssC(&dir,p);


    if(setgid(gid)==-1)
    {
	ajStrDel(&dir);
	fprintf(stderr,"setgid error (mkdir)\n");
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	ajStrDel(&dir);
	fprintf(stderr,"setuid error (mkdir)\n");
	return ajFalse;
    }
    

    if(mkdir(ajStrStr(dir),0711)==-1)
    {
	ajStrDel(&dir);
	fprintf(stderr,"mkdir error (mkdir)\n");
	return ajFalse;
    }


    ajStrDel(&dir);
    
    return ajTrue;
}




/* @funcstatic jctl_do_deletefile *******************************************
**
** Delete a user file
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int] uid
** @param [w] gid [int] gid
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_deletefile(char *buf, int uid, int gid)
{
    AjPStr ufile = NULL;
    char *p=NULL;


    ufile    = ajStrNew();


    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve user file */
    ajStrAssC(&ufile,p);


    if(setgid(gid)==-1)
    {
	fprintf(stderr,"setgid error (delete file)\n");
	ajStrDel(&ufile);
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	fprintf(stderr,"setuid error (delete file)\n");
	ajStrDel(&ufile);
	return ajFalse;
    }
    

    if(unlink(ajStrStr(ufile))==-1)
    {
	fprintf(stderr,"unlink error (delete file)\n");
	ajStrDel(&ufile);
	return ajFalse;
    }

    ajStrDel(&ufile);    
    return ajTrue;
}



/* @funcstatic jctl_do_deletedir *******************************************
**
** Recursively delete a user directory
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int] uid
** @param [w] gid [int] gid
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_deletedir(char *buf, int uid, int gid)
{
    AjPStr dir  = NULL;
    AjPStr cmnd = NULL;
    char *p=NULL;


    dir    = ajStrNew();


    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve user file */
    ajStrAssC(&dir,p);


    if(setgid(gid)==-1)
    {
	fprintf(stderr,"setgid error (delete file)\n");
	ajStrDel(&dir);
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	fprintf(stderr,"setuid error (delete file)\n");
	ajStrDel(&dir);
	return ajFalse;
    }
    

    cmnd = ajStrNew();
    ajFmtPrintS(&cmnd,"rm -rf %S",dir);
    

    if(system(ajStrStr(cmnd))==-1)
    {
	fprintf(stderr,"system error (delete directory)\n");
	ajStrDel(&cmnd);
	ajStrDel(&dir);
	return ajFalse;
    }
    
    ajStrDel(&cmnd);
    ajStrDel(&dir);
    return ajTrue;
}


/* @funcstatic jctl_do_listfiles *********************************************
**
** Return regular files in a directory
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int] uid
** @param [w] gid [int] gid
** @param [w] retlist [AjPStr*] file list
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_listfiles(char *buf, int uid, int gid, AjPStr *retlist)
{
    AjPStr dir     = NULL;
    AjPStr full    = NULL;
    
    char *p=NULL;
    DIR  *dirp;
    struct dirent *dp;
    struct stat sbuf;
    

    dir     = ajStrNew();
    full    = ajStrNew();

    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve user file */
    ajStrAssC(&dir,p);


    if(setgid(gid)==-1)
    {
	fprintf(stderr,"setgid error (list files)\n");
	ajStrDel(&dir);
	ajStrDel(&full);
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	fprintf(stderr,"setuid error (list files)\n");
	ajStrDel(&dir);
	ajStrDel(&full);
	return ajFalse;
    }
    

    if(!(dirp=opendir(ajStrStr(dir))))
    {
	fprintf(stderr,"opendir error (list files)\n");
	ajStrDel(&dir);
	ajStrDel(&full);
	return ajFalse;
    }

    ajFileDirFix(&dir);

    for(dp=readdir(dirp);dp;dp=readdir(dirp))
    {
	if(*(dp->d_name)=='.')
	    continue;
	ajFmtPrintS(&full,"%S%s",dir,dp->d_name);
	if(stat(ajStrStr(full),&sbuf)==-1)
	    continue;

	if(sbuf.st_mode & S_IFREG)
	{
	    ajStrAppC(retlist,dp->d_name);
	    ajStrAppC(retlist,"\n");
	}
    }
    
    ajStrDel(&full);
    ajStrDel(&dir);
    return ajTrue;
}



/* @funcstatic jctl_do_listdirs *********************************************
**
** Return directoriy files within a directory
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int] uid
** @param [w] gid [int] gid
** @param [w] retlist [AjPStr*] file list
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_listdirs(char *buf, int uid, int gid, AjPStr *retlist)
{
    AjPStr dir     = NULL;
    AjPStr full    = NULL;
    
    char *p=NULL;
    DIR  *dirp;
    struct dirent *dp;
    struct stat sbuf;
    

    dir     = ajStrNew();
    full    = ajStrNew();

    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve directory file */
    ajStrAssC(&dir,p);


    if(setgid(gid)==-1)
    {
	fprintf(stderr,"setgid error (list dirs)\n");
	ajStrDel(&dir);
	ajStrDel(&full);
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	fprintf(stderr,"setuid error (list dirs)\n");
	ajStrDel(&dir);
	ajStrDel(&full);
	return ajFalse;
    }
    

    if(!(dirp=opendir(ajStrStr(dir))))
    {
	fprintf(stderr,"opendir error (list dirs)\n");
	ajStrDel(&dir);
	ajStrDel(&full);
	return ajFalse;
    }

    ajFileDirFix(&dir);

    for(dp=readdir(dirp);dp;dp=readdir(dirp))
    {
	if(*(dp->d_name)=='.')
	    continue;
	ajFmtPrintS(&full,"%S%s",dir,dp->d_name);
	if(stat(ajStrStr(full),&sbuf)==-1)
	    continue;

	if(sbuf.st_mode & S_IFDIR)
	{
	    ajStrAppC(retlist,dp->d_name);
	    ajStrAppC(retlist,"\n");
	}
    }
    
    ajStrDel(&full);
    ajStrDel(&dir);
    return ajTrue;
}



/* @funcstatic jctl_do_getfile *********************************************
**
** Get a user file
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int*] uid
** @param [w] gid [int*] gid
** @param [w] fbuf [unsigned char**] file
** @param [w] size [int*] uid
** @param [r] sockdes [int] socket
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_getfile(char *buf, int uid, int gid,
			      unsigned char **fbuf,
			      int *size, int sockdes)
{
    AjPStr file    = NULL;
    AjPStr message = NULL;
    
    char *p=NULL;
    struct stat sbuf;
    int n=0;
    int sofar=0;
    int pos=0;
    int fd;
    
    file     = ajStrNew();

    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve file name */
    ajStrAssC(&file,p);


    if(setgid(gid)==-1)
    {
	fprintf(stderr,"setgid error (get file)\n");
	ajStrDel(&file);
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	fprintf(stderr,"setuid error (get file)\n");
	ajStrDel(&file);
	return ajFalse;
    }


    if(stat(ajStrStr(file),&sbuf)==-1)
    {
	fprintf(stderr,"stat error (get file)\n");
	ajStrDel(&file);
	return ajFalse;
    }

    n = *size = sbuf.st_size;


    message = ajStrNew();
    ajFmtPrintS(&message,"%d",n);
    if(send(sockdes,ajStrStr(message),ajStrLen(message)+1,0)==-1)
    {
	ajStrDel(&file);
	ajStrDel(&message);
	fprintf(stderr,"get file send error\n");
	exit(-1);
    }


    
    if(!n)
    {
	ajStrDel(&message);
	ajStrDel(&file);
	return ajTrue;
    }
    
    if(!(*fbuf=(unsigned char*)malloc(n)))
    {
	fprintf(stderr,"malloc error (get file)\n");
	ajStrDel(&message);
	ajStrDel(&file);
	return ajFalse;
    }


    if((fd=open(ajStrStr(file),O_RDONLY))==-1)
    {
	fprintf(stderr,"open error (get file)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    if((sofar=read(fd,(void *)*fbuf,n))!=n)
    {
	fprintf(stderr,"read loop required error (get file)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    if(close(fd)==-1)
    {
	fprintf(stderr,"close error (get file)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }


    while(pos+JBUFFLEN < n)
    {
	fwrite((void*)&(*fbuf)[pos],1,JBUFFLEN,stdout);
	pos += JBUFFLEN;
    }
    if(n)
	if(n-pos)
	    fwrite((void *)&(*fbuf)[pos],1,n-pos,stdout);
	
    ajStrDel(&file);
    ajStrDel(&message);


    return ajTrue;
}



/* @funcstatic jctl_do_putfile *********************************************
**
** Put a user file
**
** @param [w] buf [char*] socket buffer
** @param [w] uid [int*] uid
** @param [w] gid [int*] gid
** @param [r] sockdes [int] socket
**
** @return [AjBool] true if success
******************************************************************************/

static AjBool jctl_do_putfile(char *buf, int uid, int gid, int sockdes)
{
    int sofar=0;
    int size;
    char *p=NULL;
    int mlen;
    int fd;
    unsigned char *fbuf=NULL;
    AjPStr file;
    struct timeval tv;
    long then;
    long now;
    AjPStr message = ajStrNewC("OK");
    

    gettimeofday(&tv,NULL);
    then = tv.tv_sec;
    
    file     = ajStrNew();

    /* Skip over authentication stuff */
    p=buf;
    while(*p)
	++p;
    ++p;

    /* retrieve file name */
    ajStrAssC(&file,p);


    if(send(sockdes,ajStrStr(message),2,0)==-1)
    {
	fprintf(stderr,"jni OK1 error (jctl_do_putfile)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }



    if(recv(sockdes,buf,JBUFFLEN,0) < 0)
    {
	fprintf(stderr,"jni recv error (jctl_do_putfile)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    if(sscanf(buf,"%d",&size)!=1)
    {
	fprintf(stderr,"jni file size read  error (jctl_do_putfile)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }


    if(send(sockdes,ajStrStr(message),2,0)==-1)
    {
	fprintf(stderr,"jni OK2 error (jctl_do_putfile)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }



    if(size)
    {
	if(!(fbuf=(unsigned char *)malloc(size)))
	{
	    fprintf(stderr,"jni malloc error (jctl_do_putfile)\n");
	    ajStrDel(&message);
	    ajStrDel(&file);
	    return ajFalse;
	}
    }


    while(sofar != size)
    {
	gettimeofday(&tv,NULL);
	now = tv.tv_sec;
	if(now-then>120)
	{
	    fprintf(stderr,"jni timeout error (jctl_do_putfile)\n");
	    ajStrDel(&file);
	    ajStrDel(&message);
	    return ajFalse;
	}
	
	

	if((mlen=recv(sockdes,buf,JBUFFLEN,0))<0)
	{
	    fprintf(stderr,"jni recv error (jctl_do_putfile)\n");
	    ajStrDel(&file);
	    ajStrDel(&message);
	    return ajFalse;
	}
	memcpy((void *)&fbuf[sofar],(const void *)buf,mlen);
	sofar += mlen;
    }





    if(setgid(gid)==-1)
    {
	fprintf(stderr,"setgid error (get file)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }
    if(setuid(uid)==-1)
    {
	fprintf(stderr,"setuid error (get file)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    


    if((fd=open(ajStrStr(file),O_CREAT|O_WRONLY|O_TRUNC,0600))<0)
    {
	fprintf(stderr,"jni open error (jctl_do_putfile)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    if(write(fd,(void *)fbuf,size)<0)
    {
	fprintf(stderr,"jni write error %d %d(jctl_do_putfile)\n",size,fd);
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    if(close(fd)<0)
    {
	fprintf(stderr,"jni close error (jctl_do_putfile)\n");
	ajStrDel(&file);
	ajStrDel(&message);
	return ajFalse;
    }

    if(size)
	AJFREE(fbuf);
    ajStrDel(&file);
    ajStrDel(&message);

    return ajTrue;
}



/* @funcstatic jctl_tidy_strings *********************************************
**
** Deallocate memory
**
** @param [w] tstr [AjPStr*] temp string
** @param [w] home [AjPStr*] home directory
** @param [w] retlist [AjPStr*] filename list
** @param [w] buf [char*] socket buffer
**
** @return [void]
******************************************************************************/

static void jctl_tidy_strings(AjPStr *tstr, AjPStr *home, AjPStr *retlist,
			     char *buf)
{
    
    ajStrDel(tstr);
    ajStrDel(home);
    ajStrDel(retlist);
    AJFREE(buf);
    
    return;
}



/* @funcstatic jctl_fork_tidy *********************************************
**
** Deallocate fork memory
**
** @param [w] cl [AjPStr*] command line
** @param [w] prog [AjPStr*] program name
** @param [w] enviro [AjPStr*] environment
** @param [w] dir [AjPStr*] directory
** @param [w] outstd [AjPStr*] stdout
** @param [w] errstd [AjPStr*] stderr
**
** @return [void]
******************************************************************************/

static void jctl_fork_tidy(AjPStr *cl, AjPStr *prog, AjPStr *enviro,
		      AjPStr *dir, AjPStr *outstd, AjPStr *errstd)
{
    
    ajStrDel(cl);
    ajStrDel(prog);
    ajStrDel(enviro);
    ajStrDel(dir);
    ajStrDel(outstd);
    ajStrDel(errstd);
    
    return;
}

#else
int main()
{
    fprintf(stderr,"Error: To be used only by the Jemboss GUI (RTFM)\n");
    return 0;
}
#endif
