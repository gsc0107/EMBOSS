#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajsys_h
#define ajsys_h

#include "ajax.h"
#include <sys/types.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __VMS
#ifndef WIN32
#include <sys/param.h>
#endif
#endif

#include <sys/stat.h>

#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <unistd.h>
#else
#include <winsock2.h>
#include <io.h>
#include <fcntl.h>
#endif

#include <signal.h>
    
/*
** Structure to hold either a UNIX socket or a Windows socket.
** Allows calling functions to use sockets without needing ifdefs.
** Not implemented as an EMBOSS-style typedef as it is really a
** system thing, but could be if desired.
*/

struct AJSOCKET
{
#ifndef WIN32
    ajint sock;
#else
    SOCKET sock;
#endif
};




/*
** Structure for use with alarm timeouts
** UNIX uses SIGALRM, but Windows does not have that signal.
** Windows requires setting up a timer. This structure is primarily
** to allow a handle to the timer to be passed back for use
** with the ajSysTimeoutUnset() timer cancellation function.
** On the UNIX side it holds the sigaction structure to allow
** The action handler to be set to SIG_IGN, for extra safety.
** Not implemented as an EMBOSS-style typedef as it is really a
** system thing, but could be if desired.
*/

struct AJTIMEOUT
{
    ajint seconds;
#ifdef WIN32
    HANDLE thandle;
    LARGE_INTEGER wtime;
#else
    struct sigaction sa;
#endif
};




#ifndef WIN32
#define AJBADSOCK -1
#define SOCKRET int
#else
#define AJBADSOCK INVALID_SOCKET
#define SOCKRET SOCKET
#endif
        


    
/*
** Prototype definitions
*/

AjBool        ajSysArglistBuildC (const char* cmdline,
                                  char** Pname, char*** PParglist);
AjBool        ajSysArglistBuildS (const AjPStr cmdline,
                                  char** Pname, char*** PParglist);
void          ajSysArglistFree (char*** arglist);
void          ajSysCanon(AjBool state);
char          ajSysCastItoc(ajint v);
unsigned char ajSysCastItouc(ajint v);
void          ajSysExit(void);
AjBool        ajSysFileUnlinkC(const char* s);
AjBool        ajSysFileUnlinkS(const AjPStr s);
FILE*         ajSysFdFromSocket(struct AJSOCKET sock, const char *mode);
AjBool        ajSysFileWhich(AjPStr *Pfilename);
AjBool        ajSysFileWhichEnv(AjPStr *Pfilename, char * const env[]);
FILE*         ajSysFuncFdopen(ajint filedes, const char *mode);
char*         ajSysFuncFgets(char *buf, int size, FILE *fp);
FILE*         ajSysFuncFopen(const char *name, const char *flags);
SOCKRET       ajSysFuncSocket(int domain, int type, int protocol);
char*         ajSysFuncStrtokR(const char *s, const char *t,
			       const char **ptrptr,
			       AjPStr *buf);
char*         ajSysFuncStrtok(const char *s, const char *t);
char*         ajSysFuncStrdup(const char *s);
AjBool        ajSysCommandCopyC(const char* filename,
                                const char* newfilename);
AjBool        ajSysCommandCopyS(const AjPStr filename,
                                const AjPStr newfilename);
AjBool        ajSysCommandMakedirC(const char* dirname);
AjBool        ajSysCommandMakedirS(const AjPStr dirname);
AjBool        ajSysCommandRemoveC(const char* filename);
AjBool        ajSysCommandRemoveS(const AjPStr filename);
AjBool        ajSysCommandRemovedirC(const char* dirname);
AjBool        ajSysCommandRemovedirS(const AjPStr dirname);
AjBool        ajSysCommandRenameC(const char* filename,
                                  const char* newfilename);
AjBool        ajSysCommandRenameS(const AjPStr filename,
                                  const AjPStr newfilename);
ajint         ajSysExecC(const char* cl);
void          ajSysExecS(const AjPStr clstr);
void          ajSysExecPathS(const AjPStr clstr);
void          ajSysExecEnvS(const AjPStr clstr, char * const env[]);
void          ajSysExecOutnameS(const AjPStr clstr, const AjPStr outfname);
void          ajSysExecOutnameAppendS(const AjPStr clstr,
                                      const AjPStr outfname);
void          ajSysExecOutnameErrS(const AjPStr clstr, const AjPStr outfname);
void          ajSysExecOutnameAppendErrS(const AjPStr clstr,
                                         const AjPStr outfname);
void          ajSysSocketclose(struct AJSOCKET sock);

int           ajSysTimeoutSet(struct AJTIMEOUT *ts);
int           ajSysTimeoutUnset(struct AJTIMEOUT *ts);
    
/*
** End of prototype definitions
*/


__deprecated void          ajSystem(const AjPStr cl);
__deprecated void          ajSystemEnv(const AjPStr cl, char * const env[]);
__deprecated void          ajSystemOut(const AjPStr cl, const AjPStr outfname);
__deprecated char         *ajSysStrdup(const char *s);
__deprecated char         *ajSysStrtok(const char *s, const char *t);
__deprecated char         *ajSysStrtokR(const char *s, const char *t,
					const char **ptrptr, AjPStr *buf);
__deprecated char         *ajSysFgets(char *buf, int size, FILE *fp);
__deprecated FILE         *ajSysFopen(const char *name, const char *flags);
__deprecated FILE         *ajSysFdopen(ajint filedes, const char *mode);
__deprecated AjBool        ajSysWhich(AjPStr *exefile);
__deprecated AjBool        ajSysWhichEnv(AjPStr *exefile, char * const env[]);
__deprecated void          ajSysBasename(AjPStr *filename);
__deprecated void          ajSysArgListFree (char*** arglist);
__deprecated AjBool        ajSysIsDirectory(const char *s);
__deprecated AjBool        ajSysIsRegular(const char *s);
__deprecated AjBool        ajSysArglist (const AjPStr cmdline,
					 char** Pname, char*** PParglist);
__deprecated AjBool        ajSysArglistBuild(const AjPStr cmdline,
                                             char** Pname, char*** PParglist);
__deprecated char          ajSysItoC(ajint v);
__deprecated unsigned char ajSysItoUC(ajint v);
__deprecated AjBool        ajSysUnlink(const AjPStr s);
__deprecated AjBool        ajSysFileUnlink(const AjPStr s);

__deprecated void          ajSysSystem(const AjPStr cl);
__deprecated void          ajSysSystemEnv(const AjPStr cl, char * const env[]);
__deprecated void          ajSysSystemOut(const AjPStr cl,
                                          const AjPStr outfname);
/*
** S_IFREG is non-ANSI therefore define it here
** At least keeps all the very dirty stuff in one place
*/

#ifndef S_IFREG
#define S_IFREG 0100000
#endif
#ifndef S_IFDIR
#define S_IFDIR 0x4000
#endif

#ifndef WIN32
#define AJ_FILE_REG S_IFREG
#define AJ_FILE_DIR S_IFDIR
#else
#define AJ_FILE_REG _S_IFREG
#define AJ_FILE_DIR _S_IFDIR
#endif

#endif

#ifdef __cplusplus
}
#endif
