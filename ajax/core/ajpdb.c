/****************************************************************************
**
** @source ajpdb.c 
**
** AJAX low-level functions for handling protein structural data.  
** For use with the Atom, Chain and Pdb objects defined in ajpdb.h
** Also for use with Hetent, Het, Vdwres, Vdwall, Cmap and Pdbtosp objects
** (also defined in ajpdb.h).
** Includes functions for reading and writing ccf (clean coordinate file)
** format.
** 
** @author: Copyright (C) 2004 Jon Ison (jison@hgmp.mrc.ac.uk) 
** @version 1.0 
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
****************************************************************************/

/* ======================================================================= */
/* ============================ include files ============================ */
/* ======================================================================= */

#include "ajax.h"





/* ======================================================================= */
/* ============================ private data ============================= */
/* ======================================================================= */

#define CMAP_MODE_I   1
#define CMAP_MODE_C   2




/* ======================================================================= */
/* ================= Prototypes for private functions ==================== */
/* ======================================================================= */

static ajint  pdbSortPdbtospPdb(const void *ptr1, const void *ptr2);




/* ======================================================================= */
/* ========================== private functions ========================== */
/* ======================================================================= */

/* @funcstatic pdbSortPdbtospPdb **********************************************
**
** Function to sort Pdbtosp objects by Pdb element. Usually called by 
** ajPdbtospReadAllNew.
**
** @param [r] ptr1  [const void*] Pointer to AjOPdbtosp object 1
** @param [r] ptr2  [const void*] Pointer to AjOPdbtosp object 2
**
** @return [ajint] -1 if Pdb1 should sort before Pdb2,
**                 +1 if the Pdb2 should sort first. 
**                  0 if they are identical in length and content. 
** @@
****************************************************************************/

static ajint pdbSortPdbtospPdb(const void *ptr1, const void *ptr2)
{
    AjPPdbtosp p = NULL;
    AjPPdbtosp q = NULL;

    p = (*(AjPPdbtosp*)ptr1);
    q = (*(AjPPdbtosp*)ptr2);
    
    return ajStrCmpO(p->Pdb, q->Pdb);
}





/* ======================================================================= */
/* =========================== constructors ============================== */
/* ======================================================================= */

/* @section Constructors ****************************************************
**
** All constructors return a pointer to a new instance. It is the 
** responsibility of the user to first destroy any previous instance. The 
** target pointer does not need to be initialised to NULL, but it is good 
** programming practice to do so anyway.
**
****************************************************************************/


/* @func ajPdbtospReadAllRawNew ********************************************
**
** Reads the swissprot:pdb equivalence table available at URL (1)
**  (1) http://www.expasy.ch/cgi-bin/lists?pdbtosp.txt
** and returns the data as a list of Pdbtosp objects. 
**
** @param [u] inf [AjPFile] Input file  
**
** @return [AjPList] List of Pdbtosp objects. 
** @@
**
****************************************************************************/

AjPList       ajPdbtospReadAllRawNew(AjPFile inf)
{
    AjPList    ret     =NULL;   /* List of Pdbtosp objects to return */
    AjPPdbtosp tmp     =NULL;   /* Temp. pointer to Pdbtosp object */
    AjPStr     pdb     =NULL;   /* PDB identifier */
    AjPStr     spr     =NULL;   /* Swissprot identifier */
    AjPStr     acc     =NULL;   /* Accession number */
    AjPStr     line    =NULL;   /* Line from file */
    AjPStr     token   =NULL;   /* Token from line */
    AjPStr     subtoken=NULL;   /* Token from line */
    AjPList    acclist =NULL;   /* List of accession numbers */
    AjPList    sprlist =NULL;   /* List of swissprot identifiers */
    ajint      n       =0;      /* No. of accession numbers for current pdb 
				   code */
    AjBool     ok      =ajFalse;/* True if "____  _" has been found and we 
				   can start parsing */
    AjBool     done_1st=ajFalse;/* True if the first line of data has been 
				   parsed*/

    

    /* Memory allocation */
    line    = ajStrNew();
    token   = ajStrNew();
    subtoken= ajStrNew();
    pdb     = ajStrNew();
    acclist = ajListstrNew();
    sprlist = ajListstrNew();
    ret     = ajListNew();
    


    /* Read lines from file */
    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line, "____  _"))
	{
	    ok=ajTrue;
	    continue;
	}
	
	
	if(!ok)
	    continue;

	if(ajStrMatchC(line, ""))
	    break; 
	
	

	/* Read in pdb code first.  Then tokenise by ':', discard the 
	   first token, then tokenise the second token by ',', parsing 
	   out the swisssprot codes and accession numbers from the 
	   subtokens */


	/* Make sure this is a line containing the pdb code */
	if((ajStrFindC(line, ":")!=-1))
	{
	    if(done_1st)
	    {
		tmp = ajPdbtospNew(0);
		ajStrAssS(&tmp->Pdb, pdb);
		tmp->n = n;
		ajListToArray(acclist, (void ***) &tmp->Acc);
		ajListToArray(sprlist, (void ***) &tmp->Spr);
		ajListPushApp(ret, (void *)tmp);
		
		
		ajListstrDel(&acclist);
		ajListstrDel(&sprlist);		
		acclist = ajListstrNew();
		sprlist = ajListstrNew();

		n=0;
	    }	

	    ajFmtScanS(line, "%S", &pdb);

	    ajStrTokC(line, ":");
	    ajStrAssS(&token, ajStrTokC(NULL, ":"));

	    done_1st=ajTrue;
	}
	else 
	{
	    ajStrAssS(&token, line);
	}
	

	spr  = ajStrNew();
	acc  = ajStrNew();
	ajFmtScanS(token, "%S (%S", &spr, &acc);
	

	if(ajStrSuffixC(acc, "),"))
	{
	    ajStrChop(&acc);
	    ajStrChop(&acc);
	}
	else
       	    ajStrChop(&acc);
	
	ajListstrPushApp(acclist, acc);
	ajListstrPushApp(sprlist, spr);
	n++;

	ajStrTokC(token, ",");
	while((subtoken=ajStrTokC(NULL, ",")))
	{
	    spr  = ajStrNew();
	    acc  = ajStrNew();

	    ajFmtScanS(subtoken, "%S (%S", &spr, &acc); 

	    if(ajStrSuffixC(acc, "),"))
	    {
		ajStrChop(&acc);
		ajStrChop(&acc);
	    }
	    else
		ajStrChop(&acc);


	    ajListstrPushApp(acclist, acc);
	    ajListstrPushApp(sprlist, spr);
	    n++;
	}
    }	

    /* Data for last pdb code ! */
    tmp = ajPdbtospNew(0);
    ajStrAssS(&tmp->Pdb, pdb);
    tmp->n = n;


    ajListToArray(acclist, (void ***) &tmp->Acc);
    ajListToArray(sprlist, (void ***) &tmp->Spr);	  
    ajListPushApp(ret, (void *)tmp);
    ajListstrDel(&acclist);
    ajListstrDel(&sprlist);		



    /* Tidy up */
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&subtoken);
    ajStrDel(&pdb);
    ajListstrDel(&acclist);	
    ajListstrDel(&sprlist);	


    return ret;
}




/* @func ajPdbtospReadNew **************************************************
**
** Read a Pdbtosp object from a file in embl-like format (see documentation  
** for DOMAINATRIX "pdbtosp" application).
**
** @param [u] inf [AjPFile] Input file stream
** @param [r] entry [const AjPStr] Pdb id
**
** @return [AjPPdbtosp] True on success
** @@
****************************************************************************/

AjPPdbtosp  ajPdbtospReadNew(AjPFile inf, const AjPStr entry) 
{
    AjPPdbtosp ret = NULL;
    
    ret = ajPdbtospReadCNew(inf,ajStrStr(entry));

    return ret;
}





/* @func ajPdbtospReadCNew *************************************************
**
** Read a Pdbtosp object from a file in embl-like format.  Memory for the
** object is allocated.
**
** @param [u] inf   [AjPFile] Input file stream
** @param [r] entry [const char*]   Pdb id
**
** @return [AjPPdbtosp] True on success
** @@
****************************************************************************/

AjPPdbtosp ajPdbtospReadCNew(AjPFile inf, const char *entry)
{
    AjPPdbtosp ret = NULL;

    static AjPStr line   = NULL;
    static AjPStr tentry = NULL;	
    static AjPStr pdb    = NULL;	
    AjBool ok            = ajFalse;
    ajint  n             = 0;
    ajint  i             = 0;
    

    /* Only initialise strings if this is called for the first time */
    if(!line)
    {
	line    = ajStrNew();
	tentry  = ajStrNew();
	pdb     = ajStrNew();
    }


    ajStrAssC(&tentry,entry);
    ajStrToUpper(&tentry);
    
    while((ok=ajFileReadLine(inf,&line)))
    {
	if(!ajStrPrefixC(line,"EN   "))
	    continue;
	ajFmtScanS(line, "%*S %S", &pdb);
	if(ajStrMatchWild(pdb,tentry))
	    break;
    }
    if(!ok)
	return NULL;

    while(ok && !ajStrPrefixC(line,"//"))
    {
	if(ajStrPrefixC(line,"XX"))
	{
	    ok = ajFileReadLine(inf,&line);
	    continue;
	}
	else if(ajStrPrefixC(line,"NE"))
	{
	    ajFmtScanS(line, "%*S %d", &n);
	    (ret) = ajPdbtospNew(n);
	    ajStrAssS(&(ret)->Pdb, pdb);
	}
	else if(ajStrPrefixC(line,"IN"))
	{
	    ajFmtScanS(line, "%*S %S %*S %S", &(ret)->Spr[i],
		       &(ret)->Acc[i]);
	    i++;
	}
	
	ok = ajFileReadLine(inf,&line);
    }
    
    return ret;
}





/* @func ajPdbtospReadAllNew ***********************************************
**
** Read all the Pdbtosp objects in a file in embl-like format (see 
** documentation for DOMAINATRIX "pdbtosp" application) and writes a list of 
** these objects. It then sorts the list by PDB id.
**
** @param [u] inf [AjPFile] Input file stream
**
** @return [AjPList] List of Pdbtosp objects.
** @@
****************************************************************************/

AjPList  ajPdbtospReadAllNew(AjPFile inf)
{
    AjPList ret = NULL;
    AjPPdbtosp ptr = NULL;
    
    /* Check args and allocate list if necessary */
    if(!inf)
	return NULL;

    if(!(ret))
	ret = ajListNew();
    

    while((ptr = ajPdbtospReadCNew(inf, "*")))
	ajListPush(ret, (void *) ptr);

    ajListSort(ret, pdbSortPdbtospPdb);
    
    return ret;
}





/* @func ajCmapReadINew ****************************************************
**
** Read a Cmap object from a file in CON  format (see 
** documentation for DOMAINATRIX "contacts" application). Takes the chain 
** identifier as an integer. If the arguments mod and chn are both 0, the 
** function will read the next Cmap in the file.
** 
** @param [u] inf     [AjPFile]  Input file stream
** @param [r] chn     [ajint]    Chain number
** @param [r] mod     [ajint]    Model number
**
** @return [AjPCmap] Pointer to new Cmap object.
** @category new [AjPCmap] Cmap constructor from reading file in CON
**           format (see documentation for the EMBASSY DOMAINATRIX package).
** @@
****************************************************************************/

 AjPCmap  ajCmapReadINew(AjPFile inf, ajint chn, ajint mod)
{
    AjPCmap ret = NULL;
    
    if(!(ret=ajCmapReadNew(inf, CMAP_MODE_I, chn, mod)))
	return NULL;

    return ret;
}





/* @func ajCmapReadCNew ****************************************************
**
** Read a Cmap object from a file in CON format (see 
** documentation for DOMAINATRIX "contacts" application). Takes the chain 
** identifier as a character.  
** 
** @param [u] inf     [AjPFile]  Input file stream
** @param [r] chn     [char]     Chain number
** @param [r] mod     [ajint]    Model number
**
** @return [AjPCmap]   Pointer to new Cmap object.
** @category new [AjPCmap] Cmap constructor from reading file in CON
**              format (see documentation for the EMBASSY DOMAINATRIX package).
** @@
****************************************************************************/

AjPCmap ajCmapReadCNew(AjPFile inf, char chn, ajint mod)
{
    AjPCmap ret = NULL;

    if(!(ret = ajCmapReadNew(inf, CMAP_MODE_C, (ajint)chn, mod)))
	return NULL;

    return ret;
}





/* @func ajCmapReadAllNew ***************************************************
**
** Read every Cmap object from a file in CON format (see 
** documentation for DOMAINATRIX "contacts" application) and returns a list
** of these objects. 
** 
** @param [u] inf     [AjPFile]  Input file stream
**
** @return [AjPList]   List of Cmap objects.
** @category new [AjPCmap] Cmap constructor from reading file in CON
**              format (see documentation for the EMBASSY DOMAINATRIX package).
** @@
****************************************************************************/

AjPList ajCmapReadAllNew(AjPFile inf)
{
  AjPList ret  = NULL;
  AjPCmap cmap = NULL;

  ret = ajListNew();

  while((cmap = ajCmapReadNew(inf, CMAP_MODE_I, 0, 0)))
    ajListPushApp(ret, cmap);

  return ret;
}




/* @func ajCmapReadNew *****************************************************
**
** Read a Cmap object from a file in CON format (see 
** documentation for DOMAINATRIX "contacts" application). This is not 
** usually called by the user, who uses ajCmapReadINew or ajCmapReadCNew 
** instead.  If mode==CMAP_MODE_I, chn==0 and  mod==0, the function will 
** read the next Cmap in the file.
** 
** @param [u] inf     [AjPFile]  Input file stream.
** @param [r] mode    [ajint]    Mode, either CMAP_MODE_I (treat chn arg as  
**                               an integer) or CMAP_MODE_C (treat chn arg as
**                               a character).
** @param [r] chn     [ajint]    Chain identifier / number.
** @param [r] mod     [ajint]    Model number.
**
** @return [AjPCmap] True on success (an object read)
** @category new [AjPCmap] Cmap constructor from reading file in CON
**              format (see documentation for the EMBASSY DOMAINATRIX package).
** @@
****************************************************************************/

AjPCmap ajCmapReadNew(AjPFile inf, ajint mode, ajint chn, ajint mod)
	       
{	
    AjPCmap  ret = NULL;
    static   AjPStr line       = NULL;   /* Line of text     */
    static   AjPStr temp_id    = NULL;   /* Temp. protein id */
    static   AjPStr temp_domid = NULL;   /* Temp. domain id  */
    static   AjPStr temp_ligid = NULL;   /* Temp. ligand id  */
    static   AjPStr type       = NULL;   /* Type of contact  */
    static   AjPStr desc       = NULL;   /* Ligand description, SITES output only */
    AjPStr   token             = NULL;   /* For parsing      */
        
    ajint    smcon     = 0;      /* No. of SM contacts       */	
    ajint    licon     = 0;      /* No. of LI contacts       */	
    ajint    x         = 0;      /* No. of first residue making contact */
    ajint    y         = 0;      /* No. of second residue making contact */
    ajint    md        = -1;     /* Model number   */
    ajint    cn1       = -1;     /* Chain number 1 */
    ajint    cn2       = -1;     /* Chain number 2 */
    char     id1       = -1;     /* Chain id 1     */
    char     id2       = -1;     /* Chain id 2     */
    ajint    nres1     = 0;      /* No. of residues in domain / chain 1 */	
    ajint    nres2     = 0;      /* No. of residues in domain / chain 2 */	
    AjPStr   seq1      = NULL;   /* Sequence 1 */
    AjPStr   seq2      = NULL;   /* Sequence 2 */
           
    AjBool   idok      = ajFalse; /* If the required chain has been found */

    ajint     ns;                /* No. of sites, SITES output only */
    ajint     sn;                /* Site number, SITES output only */
    
    /* Check args */	
    if(!inf)
    {	
	ajWarn("Invalid args to ajCmapReadNew");	
	return NULL;
    }


    /* Convert '_' chain identifiers to '.' if necessary */
    if(mode==CMAP_MODE_C)
	if(chn=='_')
	    chn='.';



    
    /* Initialise strings */
    if(!line)
    {
	line       = ajStrNew();
	temp_id    = ajStrNew();
	temp_domid = ajStrNew();
	temp_ligid = ajStrNew();
	desc       = ajStrNew();
    }
    

    /* Start of main loop */
    while((ajFileReadLine(inf, &line)))
    {
        /* // */
	if(ajStrPrefixC(line, "//"))
	{
        /* If the delimiter between entries is found and ret is non-NULL, i.e.
           has been allocated, the function should return. */
	  ajStrDel(&seq1);
	  ajStrDel(&seq2);
	  return ret;	
	}


        /* SI */
	else if(ajStrPrefixC(line, "SI"))
	{ 
	  token = ajStrTokC(line, ";");
	  ajFmtScanS(token, "%*s %*s %d", &sn);

	  token = ajStrTokC(NULL, ";");
	  ajFmtScanS(token, "%*s %d", &ns);
        }

	/* TY */
	else if(ajStrPrefixC(line, "TY"))
	{
	    ajFmtScanS(line, "%*s %S", &type);
	    ajStrClear(&seq1);
	    ajStrClear(&seq2);
	    id1 = '.';
	    id2 = '.';
	    cn1=0;
	    cn2=0;
	    nres1=0;
	    nres2=0;
	}

	/* EX, NE and EN records are not parsed */

	/* ID */
	else if(ajStrPrefixC(line, "ID"))
	{
	    token = ajStrTokC(line, ";");
	    ajFmtScanS(token, "%*s %*s %S", &temp_id);
	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %S", &temp_domid);
	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %S", &temp_ligid);
	}

	/* DE records are not parsed (SITES output) */
	else if(ajStrPrefixC(line, "DE"))
	{
	  ajStrAssSub(&desc, line, 4, -1);
	}

	/* CN */
	else if(ajStrPrefixC(line, "CN"))
	{
	    token = ajStrTokC(line, ";");
	    ajFmtScanS(token, "%*s %*s %d", &md);
	    if(md == '.')
		md = 0;	    

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %d", &cn1);

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %d", &cn2);

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %c", &id1);

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %c", &id2);

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %d", &nres1);
	    if(nres1 == '.')
		nres1 = 0;

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %d", &nres2);
	    if(nres2 == '.')
		nres2 = 0;
	}

	/* S1 */
	else if(ajStrPrefixC(line, "S1"))
	{    
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&seq1,ajStrStr(line));
	    ajStrCleanWhite(&seq1);
	}

	/* S2 */
	else if(ajStrPrefixC(line, "S2"))
	{    
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&seq2,ajStrStr(line));
	    ajStrCleanWhite(&seq2);
	}
	/* NC */	    
	else if((ajStrPrefixC(line, "NC")) && 
		((md==mod) || ((chn==0)&&(mod==0)&&(mode==CMAP_MODE_I))))
	{
	    token = ajStrTokC(line, ";");
	    ajFmtScanS(token, "%*s %*s %d", &smcon);

	    token = ajStrTokC(NULL, ";");
	    ajFmtScanS(token, "%*s %d", &licon);
	    

	    /*
	    ** The fourth conditional is to capture those few domains which are 
	    ** made up from more than one chain.  For these, the chain 
	    ** character passed in might be an A or a B (e.g. the character
	    ** extracted from the scop domain code) whereas the chain id given
	    ** in the contact map file will be a '.' - because of how scopparse 
	    ** copes with these cases. (A '.' is also in the contact maps for
	    ** where a chain id was not specified in the original pdb file).
	    */

	    if( ((cn1==chn)&&(mode==CMAP_MODE_I))                       ||
		((chn==0)&&(mod==0)&&(mode==CMAP_MODE_I))               ||
		((toupper((int)id1)==toupper(chn))&&(mode==CMAP_MODE_C))||
		((toupper((int)id1)=='.') && (toupper(chn)!='.') &&
		(mode==CMAP_MODE_C))

		)
	    {
		idok=ajTrue;
		
		/* Allocate contact map and write values */
		if(ajStrMatchC(type, "INTER"))
		{
		    if(nres1>nres2)
		      (ret) = ajCmapNew(nres1);
		    else
		      (ret) = ajCmapNew(nres2);
		}
		else
		  (ret) = ajCmapNew(nres1);

		ajStrAssS(&(ret)->Id, temp_id);
		ajStrAssS(&(ret)->Domid, temp_domid);
		ajStrAssS(&(ret)->Ligid, temp_ligid);
		
		if(ajStrMatchC(type, "INTRA"))
		{
		    ret->Type = ajINTRA;
		    (ret)->Ncon = smcon;
		}
		else if(ajStrMatchC(type, "INTER"))
		{
		    ret->Type = ajINTER;
		    (ret)->Ncon = smcon;
		}
		else if(ajStrMatchC(type, "LIGAND"))
		  {
		    ret->Type = ajLIGAND;
		    (ret)->Ncon = licon;
		    ret->ns = ns;
		    ret->sn = sn;
		    ajStrAssS(&ret->Desc, desc);
		}
		else
		    ajFatal("Unrecognised contact type");

		ret->Chn1  = cn1;
		ret->Chn2  = cn2;
		ret->Chid1 = id1;
		ret->Chid2 = id2;
		ret->Nres1 = nres1;
		ret->Nres2 = nres2;
		
		ajStrAssS(&ret->Seq1, seq1);
		ajStrAssS(&ret->Seq2, seq2);		
	    }
	}

	/* SM */
	else if((ajStrPrefixC(line, "SM")) && 
		((md==mod) || ((chn==0)&&(mod==0)&&(mode==CMAP_MODE_I)))
		&& (idok))
	{
	    ajFmtScanS(line, "%*s %*s %d %*c %*s %d", &x, &y);

	    /* Check residue number is in range */
	    if((x>(ret)->Dim) || (y>(ret)->Dim))
		ajFatal("Fatal attempt to write bad data in "
			"ajCmapReadNew\nEmail culprit: "
			"jison@hgmp.mrc.ac.uk\n");
	    
	    /* Enter '1' in matrix to indicate contact */
	    ajInt2dPut(&(ret)->Mat, x-1, y-1, 1);
	    ajInt2dPut(&(ret)->Mat, y-1, x-1, 1);
	}

	/* LI */
	else if((ajStrPrefixC(line, "LI")) && 
		((md==mod) || ((chn==0)&&(mod==0)&&(mode==CMAP_MODE_I)))
		&& (idok))
	{
	    ajFmtScanS(line, "%*s %*s %d", &x);

	    /* Check residue number is in range */
	    if((x>(ret)->Dim))
		ajFatal("Fatal attempt to write bad data in "
			"ajCmapReadNew\nEmail culprit: "
			"jison@hgmp.mrc.ac.uk\n");
	    
	    /* Enter '1' in matrix to indicate contact.  For ligand contacts, 
	       the first row / column only is used. */
	    ajInt2dPut(&(ret)->Mat, x-1, 0, 1);
	    ajInt2dPut(&(ret)->Mat, 0, x-1, 1);
	}
    }

    
    ajStrDel(&seq1);
    ajStrDel(&seq2);
    return ret;	
}	




/* @func ajVdwallReadNew ***************************************************
**
** Read a Vdwall object from a file in embl-like format (see documentation
** for the EMBASSY DOMAINATRIX package).
** 
** @param [u] inf     [AjPFile]  Input file stream
**
** @return [AjPVdwall] Pointer to Vdwall object.
** @category new [AjPVdwall] Vdwall constructor from reading file in embl-like
**              format (see documentation for the EMBASSY DOMAINATRIX package).
** @@
****************************************************************************/

AjPVdwall  ajVdwallReadNew(AjPFile inf)
{
    AjPVdwall ret = NULL;
    AjPStr line = NULL;   /* Line of text */
    ajint nres  = 0;      /* No. residues */
    ajint natm  = 0;      /* No. atoms */
    ajint rcnt  = 0;      /* Residue count */
    ajint acnt  = 0;      /* Atom count */
    char id1    = '\0';             /* Residue 1 char id code */
    AjPStr id3  = NULL;   /* Residue 3 char id code */
    
    
    /* Allocate strings */
    line = ajStrNew();
    id3  = ajStrNew();


    /* Start of main loop */
    while((ajFileReadLine(inf, &line)))
    {
	/* Parse NR line */
	if(ajStrPrefixC(line, "NR"))
	{	
	    ajFmtScanS(line, "%*s %d", &nres);
		
	    /* Allocate Vdwall object */
	    (ret) = ajVdwallNew(nres);
		
	}
	/* Parse residue id 3 char */
	else if(ajStrPrefixC(line, "AA"))
	{	
	    rcnt++;
	    acnt = 0;
	    ajFmtScanS(line, "%*s %S", &id3);
	}
	/* Parse residue id 1 char */
	else if(ajStrPrefixC(line, "ID"))
	    ajFmtScanS(line, "%*s %c", &id1);
	/* Parse number of atoms */
	else if(ajStrPrefixC(line, "NN"))
	{
	    ajFmtScanS(line, "%*s %d", &natm);
	    
	    /* Allocate next Vdwres object */
	    (ret)->Res[rcnt-1]=ajVdwresNew(natm);
	    
	    /* Write members of Vdwres object */
	    (ret)->Res[rcnt-1]->Id1 = id1;
	    ajStrAssS(&(ret)->Res[rcnt-1]->Id3, id3);
	    
	}
	/* Parse atom line */
	else if(ajStrPrefixC(line, "AT"))
	{
	    acnt++;
	    ajFmtScanS(line, "%*s %S %*c %f", 
		       &(ret)->Res[rcnt-1]->Atm[acnt-1], 
		       &(ret)->Res[rcnt-1]->Rad[acnt-1]);	
	}
    }	


    ajStrDel(&line);
    ajStrDel(&id3);
    
    return ret;
}




/* @func ajHetReadNew ******************************************************
**
** Read heterogen dictionary, the Het object is allocated.
** 
** @param [u] inf [AjPFile]    Pointer to Het file
** 
** @return [AjPHet] Het object.
** @category new [AjPHet] Het constructor from reading dictionary of
**                         heterogen groups in clean format (see documentation
**                         for the EMBASSY DOMAINATRIX package).
** @@
****************************************************************************/

AjPHet  ajHetReadNew(AjPFile inf)
{
    AjPHet hetdic   = NULL;
    AjPStr line     = NULL;		/* current line */
    AjPHetent entry = NULL;		/* current entry */
    AjPList list    = NULL;		/* List of entries */
    AjPStr temp     = NULL;		/* Temporary string */
  
  
    /*Check args */
    if((!inf))
    {
	ajWarn("Bad args passed to ajHetReadNew\n");
	return NULL;
    }

    /* Create Het object if necessary */
    if(!(hetdic))
	hetdic = ajHetNew(0);
    
    /* Create string and list objects */
  
    line = ajStrNew();
    temp = ajStrNew();
    list = ajListNew();
  
    /* Read lines from file */
    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line, "ID   "))
	{
	    entry=ajHetentNew();
	    ajFmtScanS(line, "%*s %S", &entry->abv);
	}	
	else if(ajStrPrefixC(line, "DE   "))
	{  	/* NEED TO ACCOUNT FOR MULTIPLE LINES */
	    ajStrAssSub(&temp, line, 5, -1);
	    if(ajStrLen(entry->ful))
		ajStrApp(&entry->ful, temp);
	    else
		ajStrAssS(&entry->ful, temp);
	}	
	else if(ajStrPrefixC(line, "SY   "))
	{
	    /*	  ajFmtScanS(line, "%*s %S", &entry->syn); */
	    ajStrAssSub(&temp, line, 5, -1);
	    if(ajStrLen(entry->syn))
		ajStrApp(&entry->syn, temp);
	    else
		ajStrAssS(&entry->syn, temp);
	}
	else if(ajStrPrefixC(line, "NN   "))
	    ajFmtScanS(line, "%*s %S", &entry->cnt);
	else if(ajStrPrefixC(line, "//"))
	    ajListPush(list, (AjPHetent) entry);
    }

    (hetdic)->n=ajListToArray(list, (void ***) &((hetdic)->entries));
  
    ajStrDel(&line);
    ajStrDel(&temp);
    ajListDel(&list);

    return hetdic;
}




/* @func ajHetReadRawNew ***************************************************
**
** Reads a dictionary of heterogen groups available at 
** http://pdb.rutgers.edu/het_dictionary.txt and writes a Het object.
**
** @param [u] inf [AjPFile]    Pointer to dictionary
**
** @return [AjPHet] True on success
** @category new [AjPHet] Het constructor from reading dictionary of
**                        heterogen groups in raw format.
** @@
****************************************************************************/

AjPHet ajHetReadRawNew(AjPFile inf)
{
    AjPHet ret       = NULL;
    AjPStr line      = NULL;  /* A line from the file */
    AjPHetent entry  = NULL;  /* The current entry */
    AjPHetent tmp    = NULL;  /* Temp. pointer */
    AjPList list     = NULL;  /* List of entries */
    ajint het_cnt    = 0;     /* Count of number of HET records in file */
    ajint formul_cnt = 0;     /* Count of number of FORMUL records in file */
    

    /* Check arg's */
    if((!inf))
    {
	ajWarn("Bad args passed to ajHetReadRawNew\n");
	return NULL;
    }
    
    /* Create strings etc */
    line = ajStrNew();
    list = ajListNew();

    
    /* Read lines from file */
    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line,"HET "))
	{
	    het_cnt++;
	    
	    entry=ajHetentNew();
	    ajFmtScanS(line, "%*s %S", &entry->abv);
	}
	else if(ajStrPrefixC(line,"HETNAM"))
	{
	    ajStrAppC(&entry->ful, &line->Ptr[15]);
	}
	else if(ajStrPrefixC(line,"HETSYN"))
	{
	    ajStrAppC(&entry->syn, &line->Ptr[15]);
	}
	else if(ajStrPrefixC(line,"FORMUL"))
	{
	    formul_cnt++;

	    /* In cases where HETSYN or FORMUL were not
	       specified, assign a value of '.' */
	    if(MAJSTRLEN(entry->ful)==0)
		ajStrAssC(&entry->ful, ".");

	    if(MAJSTRLEN(entry->syn)==0)
		ajStrAssC(&entry->syn, ".");
	    

	    /* Push entry onto list */
	    ajListPush(list, (AjPHetent) entry);
	}
    }

    if(het_cnt != formul_cnt)
    {	
	while(ajListPop(list, (void **) &tmp))
	    ajHetentDel(&tmp);
	
	ajListDel(&list);	    
	ajStrDel(&line);

	ajFatal("Fatal discrepancy in count of HET and FORMUL records\n"
		"Email wawan@hgmp.mrc.ac.uk\n");
    }	
    
    ret = ajHetNew(0);
    ret->n = ajListToArray(list, (void ***) &((ret)->entries));
    
   
    ajStrDel(&line);
    ajListDel(&list);

    return ret;
}





/* @func ajPdbReadFirstModelNew ********************************************
**
** Reads a clean coordinate file file (see documentation for DOMAINATRIX 
** "pdbparse" application) and writes a filled Pdb object. Data for the first
** model only is read in.
**
** @param [u] inf  [AjPFile] Pointer to clean coordinate file
**
** @return [AjPPdb] Pointer to Pdb object.
** @category new [AjPPdb] Pdb constructor from reading ccf format file
**                         (retrive data for 1st model only).
** @@
****************************************************************************/

AjPPdb ajPdbReadFirstModelNew(AjPFile inf) 
{
    AjPPdb ret = NULL;
    
    ajint nmod = 0;
    ajint ncha = 0;
    ajint ngrp = 0;
    ajint nc   = 0;
    ajint mod  = 0;
    ajint chn  = 0;
    ajint gpn  = 0;

    float reso = 0.0;

    AjPStr line      = NULL;
    AjPStr token     = NULL;
    AjPStr idstr     = NULL;
    AjPStr destr     = NULL;
    AjPStr osstr     = NULL;
    AjPStr xstr      = NULL;
    AjPStrTok handle = NULL;
    
    AjPAtom atom     = NULL;

    /* Intitialise strings */
    line  = ajStrNew();
    token = ajStrNew();
    idstr = ajStrNew();
    destr = ajStrNew();
    osstr = ajStrNew();
    xstr  = ajStrNew();

    /* Start of main application loop */
    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"XX"))
	    continue;

	/* Parse ID */
	if(ajStrPrefixC(line,"ID"))
	{
	    ajStrTokenAss(&handle,line," \n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&idstr,&handle,NULL);
	    continue;
	}

	
	/* Parse number of chains */
	if(ajStrPrefixC(line,"CN"))
	{
	    ajStrTokenAss(&handle,line," []\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&nc);
	    continue;
	}
	

	/* Parse description text */
	if(ajStrPrefixC(line,"DE"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'DE' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* desc */
	    if (ajStrLen(destr))
	    {
		ajStrAppC (&destr, " ");
		ajStrApp (&destr, token);
	    }
	    else
		ajStrAssS(&destr, token);
	    continue;
	}


	/* Parse source text */
	if(ajStrPrefixC(line,"OS"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'OS' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* source */
	    if (ajStrLen(osstr))
	    {
		ajStrAppC (&osstr, " ");
		ajStrApp (&osstr, token);
	    }
	    else
		ajStrAssS(&osstr, token);
	    continue;
	}
	

	/* Parse experimental line */
	if(ajStrPrefixC(line,"EX"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&xstr,&handle,NULL); /* method */
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* reso */
	    ajStrToFloat(token,&reso);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* nmod */
	    ajStrToInt(token,&nmod);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* ncha */
	    ajStrToInt(token,&ncha);

	    ajStrToken(&token,&handle,NULL); /* nlig */
	    ajStrToInt(token,&ngrp);

	    ret = ajPdbNew(ncha);

	    ajStrAssS(&(ret)->Pdb,idstr);
	    ajStrAssS(&(ret)->Compnd,destr);
	    ajStrAssS(&(ret)->Source,osstr);
	    if(ajStrMatchC(xstr,"xray"))
		(ret)->Method = ajXRAY;
	    else
		(ret)->Method = ajNMR;

	    (ret)->Reso = reso;
	    /* (ret)->Nmod = nmod; */

	    /*
	    ** Number of models is hard-coded to 1 as only the 
	    **  data for the first model is read in
	    */
	    (ret)->Nmod = 1;
	    (ret)->Nchn = ncha;
	    (ret)->Ngp  = ngrp;
	}
	

	/* Parse information line */
	if(ajStrPrefixC(line,"IN"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* id value */
	    (ret)->Chains[nc-1]->Id=*ajStrStr(token);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* residues */
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nres);
	    ajStrToken(&token,&handle,NULL);
	    /* hetatm */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nlig);
	    /* helices */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numHelices);
	    /* strands */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numStrands);
	    /* sheets */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numSheets);
	    */
	    /* turns */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numTurns);
	    */
	    continue;
	}
  

	/* Parse sequence line */
	if(ajStrPrefixC(line,"SQ"))
	{
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(ret)->Chains[nc-1]->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(ret)->Chains[nc-1]->Seq);
	    continue;
	}


	/* Parse coordinate line */
	if(ajStrPrefixC(line,"CO"))
	{
	    mod = chn = gpn = 0;
	    
	    ajStrTokenAss(&handle,line," \t\n\r");
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&mod);

	    if(mod!=1)
		break;

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&chn);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&gpn);
	    
	    /* AJNEW0(atom); */
	    atom = ajAtomNew();

	    atom->Mod = mod;
	    atom->Chn = chn;
	    atom->Gpn = gpn;
	    

	    ajStrToken(&token,&handle,NULL);
	    atom->Type = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Idx);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Pdb,token);

	    ajStrToken(&token,&handle,NULL);
	    atom->eType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eNum);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->eId,token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eClass);

	    ajStrToken(&token,&handle,NULL);
	    atom->eStrideType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eStrideNum);

	    ajStrToken(&token,&handle,NULL);
	    atom->Id1 = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Id3,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Atm,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->X);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Y);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Z);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->O);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->B);

	    ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Phi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Psi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Area);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_rel);

	    /* Check for coordinates for water or groups that could not
	    ** be uniquely assigned to a chain
	    */
	    if(chn==0)
	    {
		/* Heterogen */
		if(atom->Type == 'H')
		    ajListPushApp((ret)->Groups,(void *)atom);
		else if(atom->Type == 'W')
		    ajListPushApp((ret)->Water,(void *)atom);
		else
		    ajFatal("Unexpected parse error in "
			    "ajPdbReadFirstModelNew. Email "
			    "jison@hgmp.mrc.ac.uk");
	    }
	    else
		ajListPushApp((ret)->Chains[chn-1]->Atoms,(void *)atom);
	}
    }
    /* End of main application loop */
    
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&idstr);
    ajStrDel(&destr);
    ajStrDel(&osstr);
    ajStrDel(&xstr);

    return ret;
}





/* @func ajPdbReadNew ******************************************************
**
** Reads a clean coordinate file (see documentation for DOMAINATRIX "pdbparse" 
** application) and writes a filled Pdb object.
**
** @param [u] inf  [AjPFile] Pointer to clean coordinate file
**
** @return [AjPPdb] Pointer to Pdb object.
** @category new [AjPPdb] Pdb constructor from reading ccf format file.
** @@
****************************************************************************/

AjPPdb ajPdbReadNew(AjPFile inf)
{
    AjPPdb ret = NULL;
    
    ajint nmod = 0;
    ajint ncha = 0;
    ajint ngrp = 0;
    ajint nc   = 0;
    ajint mod  = 0;
    ajint chn  = 0;
    ajint gpn  = 0;

    float reso = 0.0;

    AjPStr line      = NULL;
    AjPStr token     = NULL;
    AjPStr idstr     = NULL;
    AjPStr destr     = NULL;
    AjPStr osstr     = NULL;
    AjPStr xstr      = NULL;
    AjPStrTok handle = NULL;
    
    AjPAtom atom     = NULL;

    /* Intitialise strings */
    line  = ajStrNew();
    token = ajStrNew();
    idstr = ajStrNew();
    destr = ajStrNew();
    osstr = ajStrNew();
    xstr  = ajStrNew();

    /* Start of main application loop */
    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"XX"))
	    continue;

	/* Parse ID */
	if(ajStrPrefixC(line,"ID"))
	{
	    ajStrTokenAss(&handle,line," \n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&idstr,&handle,NULL);
	    continue;
	}

	
	/* Parse number of chains */
	if(ajStrPrefixC(line,"CN"))
	{
	    ajStrTokenAss(&handle,line," []\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&nc);
	    continue;
	}
	

	/* Parse description text */
	if(ajStrPrefixC(line,"DE"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'DE' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* desc */
	    if (ajStrLen(destr))
	    {
		ajStrAppC (&destr, " ");
		ajStrApp (&destr, token);
	    }
	    else
		ajStrAssS(&destr, token);
	    continue;
	}


	/* Parse source text */
	if(ajStrPrefixC(line,"OS"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'OS' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* source */
	    if (ajStrLen(osstr))
	    {
		ajStrAppC (&osstr, " ");
		ajStrApp (&osstr, token);
	    }
	    else
		ajStrAssS(&osstr, token);
	    continue;
	}
	

	/* Parse experimental line */
	if(ajStrPrefixC(line,"EX"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&xstr,&handle,NULL); /* method */
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* reso */
	    ajStrToFloat(token,&reso);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* nmod */
	    ajStrToInt(token,&nmod);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* nchn */
	    ajStrToInt(token,&ncha);

	    ajStrToken(&token,&handle,NULL); /* nlig */
	    ajStrToInt(token,&ngrp);

	    ret = ajPdbNew(ncha);

	    ajStrAssS(&(ret)->Pdb,idstr);
	    ajStrAssS(&(ret)->Compnd,destr);
	    ajStrAssS(&(ret)->Source,osstr);
	    if(ajStrMatchC(xstr,"xray"))
		(ret)->Method = ajXRAY;
	    else
		(ret)->Method = ajNMR;

	    (ret)->Reso = reso;
	    (ret)->Nmod = nmod;
	    (ret)->Nchn = ncha;
	    (ret)->Ngp  = ngrp;
	}
	

	/* Parse information line */
	if(ajStrPrefixC(line,"IN"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* id value */
	    (ret)->Chains[nc-1]->Id=*ajStrStr(token);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* residues */
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nres);
	    ajStrToken(&token,&handle,NULL);
	    /* hetatm */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nlig);
	    /* helices */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numHelices);
	    /* strands */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numStrands);
	    /* sheets */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numSheets);
	    */
	    /* turns */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numTurns);
	    */
	    continue;
	}
  

	/* Parse sequence line */
	if(ajStrPrefixC(line,"SQ"))
	{
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(ret)->Chains[nc-1]->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(ret)->Chains[nc-1]->Seq);
	    continue;
	}


	/* Parse coordinate line */
	if(ajStrPrefixC(line,"CO"))
	{
	    mod = chn = gpn = 0;
	    
	    ajStrTokenAss(&handle,line," \t\n\r");
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&mod);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&chn);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&gpn);
	    
	    /* AJNEW0(atom); */
	    atom = ajAtomNew();
	    
	    atom->Mod = mod;
	    atom->Chn = chn;
	    atom->Gpn = gpn;
	    

	    ajStrToken(&token,&handle,NULL);
	    atom->Type = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Idx);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Pdb,token);

	    ajStrToken(&token,&handle,NULL);
	    atom->eType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eNum);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->eId,token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eClass);

	    ajStrToken(&token,&handle,NULL);
	    atom->eStrideType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eStrideNum);

	    ajStrToken(&token,&handle,NULL);
	    atom->Id1 = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Id3,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Atm,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->X);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Y);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Z);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->O);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->B);

	    ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Phi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Psi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Area);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_rel);


	    /* Check for coordinates for water or groups that could not
	       be uniquely assigned to a chain */
	    if(chn==0)
	    {
		/* Heterogen */
		if(atom->Type == 'H')
		    ajListPushApp((ret)->Groups,(void *)atom);
		else if(atom->Type == 'W')
		    ajListPushApp((ret)->Water,(void *)atom);
		else
		    ajFatal("Unexpected parse error in ajPdbRead. "
			    "Email jison@hgmp.mrc.ac.uk");
	    }
	    else
		ajListPushApp((ret)->Chains[chn-1]->Atoms,(void *)atom);
	}
    }
    /* End of main application loop */
    

    ajStrTokenClear(&handle);
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&idstr);
    ajStrDel(&destr);
    ajStrDel(&osstr);
    ajStrDel(&xstr);

    return ret;
}





/* @func ajAtomNew *******************************************************
**
** Atom object constructor.
** This is normally called by the ajChainNew function.
**
** @return [AjPAtom] Pointer to an atom object
** @category new [AjPAtom] Default Atom constructor.
** @@
****************************************************************************/

AjPAtom ajAtomNew(void)
{
    AjPAtom ret = NULL;

    AJNEW0(ret);
    
    ret->Id3   = ajStrNew();
    ret->Atm   = ajStrNew();
    ret->Pdb   = ajStrNew();
    ret->eId   = ajStrNew();

    ret->Id1   = '.';
    ret->eType = '.';
    ajStrAssC(&ret->eId, ".");
    ret->eStrideType = '.';
    

    return ret;
}





/* @func ajChainNew *********************************************************
**
** Chain object constructor. 
** This is normally called by the ajPdbNew function
**
** @return [AjPChain] Pointer to a chain object
** @@
** @category new [AjPChain] Default Chain constructor.
****************************************************************************/

AjPChain ajChainNew(void)
{
    AjPChain ret = NULL;
  
    AJNEW0(ret);

    ret->Seq    = ajStrNewC("");
    ret->Atoms  = ajListNew();

    return ret;
}





/* @func ajPdbNew ***********************************************************
**
** Pdb object constructor. Fore-knowledge of the number of chains 
** is required. This is normally called by the functions that read PDB 
** files or clean coordinate files (see embpdb.c & embpdbraw.c).
**
** @param [r] n [ajint] Number of chains in this pdb file
**
** @return [AjPPdb] Pointer to a pdb object
** @category new [AjPPdb] Default Pdb constructor.
** @@
****************************************************************************/

AjPPdb ajPdbNew(ajint n)
{
    AjPPdb ret = NULL;
    ajint i;
    
    AJNEW0(ret);
  

    ret->Pdb    = ajStrNew();
    ret->Compnd = ajStrNew();
    ret->Source = ajStrNew();
    ret->Groups = ajListNew();
    ret->Water  = ajListNew();
    ret->gpid   = ajChararrNew();
    

    if(n)
    {	
	AJCNEW0(ret->Chains,n);
	for(i=0;i<n;++i)
	    ret->Chains[i] = ajChainNew();
    }

    return ret;
}


/* @func ajHetentNew ********************************************************
**
** Hetent object constructor. 
**
** @return [AjPHetent] Pointer to a Hetent object
** @category new [AjPHetent] Default Hetent constructor.
** @@
****************************************************************************/

AjPHetent ajHetentNew(void)
{
    AjPHetent ret = NULL;
    
    AJNEW0(ret);
    
    /* Create strings */
    ret->abv = ajStrNew();
    ret->syn = ajStrNew();
    ret->ful = ajStrNew();
    
    return ret;
}





/* @func ajHetNew ***********************************************************
**
** Het object constructor. 
**
** @param [r] n [ajint] Number of entries in dictionary.
** 
** @return [AjPHet] Pointer to a Het object
** @category new [AjPHet] Default Het constructor.
** @@
****************************************************************************/

AjPHet ajHetNew(ajint n)
{
    ajint i    = 0;
    AjPHet ret = NULL;
    
    AJNEW0(ret);

    if(n)
    {
	ret->n = n;
	AJCNEW0(ret->entries, n);
	for(i=0;i<n;i++)
	    ret->entries[i]=ajHetentNew();
    }
    else
    {
	ret->n = 0;
	ret->entries = NULL;
    }
    

    return ret;
}





/* @func ajVdwallNew ********************************************************
**
** Vdwall object constructor. This is normally called by the ajVdwallReadNew
** function. Fore-knowledge of the number of residues is required.
**
** @param  [r] n [ajint]  Number of residues
**
** @return [AjPVdwall] Pointer to a Vdwall object
** @category new [AjPVdwall] Default Vdwall constructor.
** @@
****************************************************************************/

AjPVdwall ajVdwallNew(ajint n)
{
    AjPVdwall ret = NULL;
    
    AJNEW0(ret);

    ret->N = n;

    if(n)
	AJCNEW0(ret->Res, n);


    return ret;
}





/* @func ajVdwresNew ********************************************************
**
** Vdwres object constructor. This is normally called by the ajVdwallReadNew
** function. Fore-knowledge of the number of atoms is required.
**
** @param  [r] n [ajint]  Number of atoms
**
** @return [AjPVdwres] Pointer to a Vdwres object
** @category new [AjPVdwres] Default Vdwres constructor.
** @@
****************************************************************************/

AjPVdwres  ajVdwresNew(ajint n)
{
    ajint x;
    AjPVdwres ret = NULL;
    
    AJNEW0(ret);

    ret->Id3 = ajStrNew();    
    ret->N   = n;

    if(n)
    {
	AJCNEW0(ret->Atm, n);
	for(x=0;x<n;++x)
	    ret->Atm[x]=ajStrNew();

	AJCNEW0(ret->Rad, n);
    }

    return ret;
}





/* @func ajCmapNew **********************************************************
**
** Cmap object constructor. This is normally called by the ajCmapReadNew
** function. Fore-knowledge of the dimension (number of residues) for the 
** contact map is required.
**
** @param  [r] n [ajint]   Dimenion of contact map
**
** @return [AjPCmap] Pointer to a Cmap object
** 
** @category new [AjPCmap] Default Cmap constructor.
** @@
****************************************************************************/

AjPCmap ajCmapNew(ajint n)
{
    AjPCmap ret = NULL;
    ajint z = 0;
    

    AJNEW0(ret);

    ret->Id    = ajStrNew();    	
    ret->Domid = ajStrNew();    	
    ret->Ligid = ajStrNew();    	
    ret->Seq1  = ajStrNew();
    ret->Seq2  = ajStrNew();
    ret->Desc  = ajStrNew();
    ret->Chid1 = '.';
    ret->Chid2 = '.';
       

    if(n)
    {
	/* Create the SQUARE contact map */
	ret->Mat = ajInt2dNewL((ajint)n);
	for(z=0;z<n;++z)
	    ajInt2dPut(&ret->Mat, z, n-1, (ajint) 0);
    }

    ret->Dim  = n;
    ret->Ncon = 0;

    return ret;
}





/* @func ajPdbtospNew *******************************************************
**
** Pdbtosp object constructor. Fore-knowledge of the number of entries is 
** required. This is normally called by the ajPdbtospReadCNew / 
** ajPdbtospReadNew functions.
**
** @param [r] n [ajint] Number of entries
**
** @return [AjPPdbtosp] Pointer to a Pdbtosp object
** @category new [AjPPdbtosp] Default Pdbtosp constructor.
** @@
****************************************************************************/

AjPPdbtosp ajPdbtospNew(ajint n)
{

    AjPPdbtosp ret = NULL;
    ajint i;

    AJNEW0(ret);

    ret->Pdb  = ajStrNew();

    if(n)
    {
	AJCNEW0(ret->Acc,n);
	AJCNEW0(ret->Spr,n);
	for(i=0; i<n; i++)
	{
	    ret->Acc[i]=ajStrNew();
	    ret->Spr[i]=ajStrNew();
	}
    }

    ret->n = n;

    return ret;
}





/* ======================================================================= */
/* =========================== destructors =============================== */
/* ======================================================================= */

/* @section Destructors *****************************************************
**
** All destructor functions receive the address of the instance to be
** deleted.  The original pointer is set to NULL so is ready for re-use.
**
****************************************************************************/


/* @func ajAtomDel **********************************************************
**
** Destructor for atom object.
**
** @param [d] ptr [AjPAtom*] Atom object pointer
**
** @return [void]
** @category delete [AjPAtom] Default Atom destructor.
** @@
****************************************************************************/

void ajAtomDel(AjPAtom *ptr)
{
    AjPAtom pthis;

    pthis = *ptr;

    if(!ptr || !pthis)
	return;

    ajStrDel(&pthis->Id3);
    ajStrDel(&pthis->Atm);
    ajStrDel(&pthis->Pdb);
    ajStrDel(&pthis->eId);

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajChainDel *********************************************************
**
** Destructor for chain object.
**
** @param [d] ptr [AjPChain*] Chain object pointer
**
** @return [void]
** @category delete [AjPChain] Default Chain destructor.
** @@
****************************************************************************/

void ajChainDel(AjPChain *ptr)
{
    AjPChain pthis;
    AjPAtom atm = NULL;

    pthis = *ptr;

    if(!ptr || !pthis)
	return;
    
    while(ajListPop(pthis->Atoms,(void **)&atm))
	ajAtomDel(&atm);

    ajStrDel(&pthis->Seq);
    ajListDel(&pthis->Atoms);

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajPdbDel ***********************************************************
**
** Destructor for pdb object.
**
** @param [d] ptr [AjPPdb*] Pdb object pointer
**
** @return [void]
** @category delete [AjPPdb] Default Pdb destructor.
** @@
****************************************************************************/

void ajPdbDel(AjPPdb *ptr)
{
    AjPPdb pthis;
    AjPAtom atm = NULL;

    ajint nc = 0;
    ajint i  = 0;

    pthis = *ptr;

    if(!pthis || !ptr)
	return;
    
    nc = pthis->Nchn;

    ajStrDel(&pthis->Pdb);
    ajStrDel(&pthis->Compnd);
    ajStrDel(&pthis->Source);

    ajChararrDel(&pthis->gpid);
    
    
    while(ajListPop(pthis->Water,(void **)&atm))
	ajAtomDel(&atm);
    ajListDel(&pthis->Water);

    while(ajListPop(pthis->Groups,(void **)&atm))
	ajAtomDel(&atm);
    ajListDel(&pthis->Groups);
    
    
    for(i=0;i<nc;++i)
	ajChainDel(&pthis->Chains[i]);
    AJFREE(pthis->Chains);

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajHetentDel ********************************************************
**
** Destructor for Hetent object.
**
** @param [d] ptr [AjPHetent*] Hetent object pointer
**
** @return [void]
** @category delete [AjPHetent] Default Hetent destructor.
** @@
****************************************************************************/

void ajHetentDel(AjPHetent *ptr)
{
    /* Check arg's */
    if(*ptr==NULL)
	return;

   
    if((*ptr)->abv)
	ajStrDel( &((*ptr)->abv)); 

    if((*ptr)->syn)
	ajStrDel( &((*ptr)->syn)); 

    if((*ptr)->ful)
	ajStrDel( &((*ptr)->ful)); 

    AJFREE(*ptr);
    *ptr = NULL;
    
    return;
}





/* @func ajHetDel ***********************************************************
**
** Destructor for Het object.
**
** @param [d] ptr [AjPHet*] Het object pointer
**
** @return [void]
** @category delete [AjPHet] Default Het destructor.
** @@
****************************************************************************/

void ajHetDel(AjPHet *ptr)
{
    ajint i = 0;
    
    /* Check arg's */
    if(*ptr==NULL)
	return;

    if((*ptr)->entries)
    {
        for(i=0;i<(*ptr)->n;i++)
	{
	    if((*ptr)->entries[i])
		ajHetentDel(&((*ptr)->entries[i]));
	}	
	
	AJFREE((*ptr)->entries);
    }
    AJFREE(*ptr);
    *ptr = NULL;

    return;
}




/* @func ajVdwallDel ********************************************************
**
** Destructor for Vdwall object.
**
** @param [d] ptr [AjPVdwall*] Vdwall object pointer
**
** @return [void]
** @category delete [AjPVdwall] Default Vdwall destructor.
** @@
****************************************************************************/

void ajVdwallDel(AjPVdwall *ptr)
{
    ajint x = 0;
    
    for(x=0;x<(*ptr)->N; ++x)
	ajVdwresDel(&(*ptr)->Res[x]);
    
    AJFREE((*ptr)->Res);
    AJFREE(*ptr);    
    *ptr = NULL;

    return;
}	





/* @func ajVdwresDel ********************************************************
**
** Destructor for Vdwres object.
**
** @param [d] ptr [AjPVdwres*] Vdwres object pointer
**
** @return [void]
** @category delete [AjPVdwres] Default Vdwres destructor.
** @@
****************************************************************************/

void ajVdwresDel(AjPVdwres *ptr)
{
    ajint x = 0;
    
    ajStrDel(&(*ptr)->Id3);
    
    for(x=0;x<(*ptr)->N; ++x)
	ajStrDel(&(*ptr)->Atm[x]);
    
    AJFREE((*ptr)->Atm);
    AJFREE((*ptr)->Rad);
    AJFREE(*ptr);    
    *ptr = NULL;

    return;
}	





/* @func ajCmapDel **********************************************************
**
** Destructor for Cmap object.
**
** @param [d] ptr [AjPCmap*] Cmap object pointer
**
** @return [void]
** @category delete [AjPCmap] Default Cmap destructor.
** @@
****************************************************************************/

void ajCmapDel(AjPCmap *ptr)
{
    if((*ptr)->Id)
	ajStrDel(&(*ptr)->Id);

    if((*ptr)->Domid)
	ajStrDel(&(*ptr)->Domid);

    if((*ptr)->Ligid)
	ajStrDel(&(*ptr)->Ligid);

    if((*ptr)->Seq1)
	ajStrDel(&(*ptr)->Seq1);

    if((*ptr)->Seq2)
	ajStrDel(&(*ptr)->Seq2);

    if((*ptr)->Desc)
        ajStrDel(&(*ptr)->Desc);

    if((*ptr)->Mat)
	ajInt2dDel(&(*ptr)->Mat);

    if((*ptr))
	AJFREE(*ptr);    

    *ptr = NULL;

    return;
}	





/* @func ajPdbtospDel *******************************************************
**
** Destructor for Pdbtosp object.
**
** @param [d] ptr [AjPPdbtosp*] Pdbtosp object pointer
**
** @return [void]
** @category delete [AjPPdbtosp] Default Pdbtosp destructor.
** @@
****************************************************************************/

void ajPdbtospDel(AjPPdbtosp *ptr)
{
    AjPPdbtosp pthis;
    ajint i;

    pthis = *ptr;

    if(!pthis || !ptr)
	return;

    ajStrDel(&pthis->Pdb);

    if(pthis->n)
    {
	for(i=0; i<pthis->n; i++)
	{
	    ajStrDel(&pthis->Acc[i]);
	    ajStrDel(&pthis->Spr[i]);
	}
	AJFREE(pthis->Acc);
	AJFREE(pthis->Spr);
    }

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* ======================================================================= */
/* ============================ Assignments ============================== */
/* ======================================================================= */

/* @section Assignments *****************************************************
**
** These functions overwrite the instance provided as the first argument
** A NULL value is always acceptable so these functions are often used to
** create a new instance by assignment.
**
****************************************************************************/

/* @func ajAtomCopy *********************************************************
**
** Copies the data from an Atom object to an Atom object; the new object
** is created if needed. 
** 
** IMPORTANT - THIS DOES NOT COPY THE eNum & eType ELEMENTS, WHICH ARE SET 
** TO ZERO and '.' INSTEAD.
** 
** @param [w] to   [AjPAtom*]  Atom object pointer
** @param [r] from [const AjPAtom]   Atom object pointer
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool ajAtomCopy(AjPAtom *to, const AjPAtom from)
{
    if(!to)
    {
	ajWarn("Bad arg (NULL) passed to ajAtomCopy");
	return ajFalse;
    }

    if(!(*to))
	*to = ajAtomNew();

    (*to)->Mod   = from->Mod;
    (*to)->Chn   = from->Chn;    
    (*to)->Gpn   = from->Gpn;
    (*to)->Type  = from->Type;
    (*to)->Idx   = from->Idx;
    ajStrAssS(&((*to)->Pdb), from->Pdb);
    (*to)->Id1   = from->Id1;
    ajStrAssS(&((*to)->Id3), from->Id3);
    ajStrAssS(&((*to)->Atm), from->Atm);
    (*to)->X     = from->X;
    (*to)->Y     = from->Y;
    (*to)->Z     = from->Z;
    (*to)->O     = from->O;
    (*to)->B     = from->B;

    (*to)->eNum  = from->eNum; 
    ajStrAssS(&((*to)->eId), from->eId);
    (*to)->eType = from->eType;
    (*to)->eClass= from->eClass; 

    (*to)->eStrideNum  = from->eStrideNum; 
    (*to)->eStrideType = from->eStrideType; 

    (*to)->all_abs  = from->all_abs; 
    (*to)->all_rel  = from->all_rel; 
    (*to)->side_abs = from->side_abs; 
    (*to)->side_rel = from->side_rel; 
    (*to)->main_abs = from->main_abs;     
    (*to)->main_rel = from->main_rel; 
    (*to)->npol_abs = from->npol_abs; 
    (*to)->npol_rel = from->npol_rel; 
    (*to)->pol_abs  = from->pol_abs; 
    (*to)->pol_rel  = from->pol_rel; 


    return ajTrue;
}





/* @func ajAtomListCopy *****************************************************
**
** Read a list of Atom structures and writes a copy of the list.  The 
** data are copied and the list is created if necessary.
** 
** @param [w] to       [AjPList *] List of Atom objects to write
** @param [r] from     [const AjPList]   List of Atom objects to read
**
** @return [AjBool] True if list was copied ok.
** @@
****************************************************************************/

AjBool ajAtomListCopy(AjPList *to, const AjPList from)
{
/* AjPList ret  = NULL; */
   AjIList iter = NULL;
   AjPAtom hit  = NULL;
   AjPAtom new  = NULL;

   /* Check arg's */
   if(!from || !to)
   {
       ajWarn("Bad arg's passed to ajAtomListCopy\n");
       return ajFalse;
   }
    
   /* Allocate the new list, if necessary */
   if(!(*to))
       *to=ajListNew();
    
   /* Initialise the iterator */
   iter=ajListIterRead(from);
    
   /* Iterate through the list of Atom objects */
   while((hit=(AjPAtom)ajListIterNext(iter)))
   {
       new=ajAtomNew();
	
       ajAtomCopy(&new, hit);

       /* Push scophit onto list */
       ajListPushApp(*to, new);
   }


   ajListIterFree(&iter);
   return ajTrue; 
}





/* @func ajPdbCopy **********************************************************
**
** Copies data from one Pdb object to another; the new object is 
** always created. 
** 
** 
** @param [w] to   [AjPPdb*] Pdb object pointer
** @param [r] from [const AjPPdb]  Pdb object pointer
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool ajPdbCopy(AjPPdb *to, const AjPPdb from)
{
    ajint x = 0;
    

    if(!from)
    {
	ajWarn("NULL arg passed to ajPdbCopy");
	return ajFalse;
    }

    if((*to))
    {
	ajWarn("Pointer passed to ajPdbCopy should be NULL but isn't !");
	return ajFalse;
    }	


    /* Copy data in Pdb object */
    (*to) = ajPdbNew(from->Nchn);
    ajStrAssS(&((*to)->Pdb), from->Pdb);
    ajStrAssS(&((*to)->Compnd), from->Compnd);
    ajStrAssS(&((*to)->Source), from->Source);
    (*to)->Method = from->Method;
    (*to)->Reso   = from->Reso;
    (*to)->Nmod   = from->Nmod;    
    (*to)->Nchn   = from->Nchn;    
    (*to)->Ngp    = from->Ngp;

    for(x=0;x<from->Ngp;x++)
	ajChararrPut(&((*to)->gpid), x, ajChararrGet(from->gpid, x));
       
    ajListDel(&((*to)->Groups));
    ajListDel(&((*to)->Water));

/*    (*to)->Groups = ajAtomListCopy(from->Groups);
    (*to)->Water  = ajAtomListCopy(from->Water); */

    if(!ajAtomListCopy(&(*to)->Groups, from->Groups))
	ajFatal("Error copying Groups list");
    
    if(!ajAtomListCopy(&(*to)->Water, from->Water))
	ajFatal("Error copying Water list");
    
    /* Copy data in Chain objects */
    for(x=0;x<from->Nchn;x++)
    {
	ajListDel(&((*to)->Chains[x]->Atoms));
	
	(*to)->Chains[x]->Id         = from->Chains[x]->Id;
	(*to)->Chains[x]->Nres       = from->Chains[x]->Nres;
	(*to)->Chains[x]->Nlig       = from->Chains[x]->Nlig;
	(*to)->Chains[x]->numHelices = from->Chains[x]->numHelices;
	(*to)->Chains[x]->numStrands = from->Chains[x]->numStrands;
	ajStrAssS(&((*to)->Chains[x]->Seq), from->Chains[x]->Seq);
/*	(*to)->Chains[x]->Atoms = ajAtomListCopy(from->Chains[x]->Atoms); */
	if(!ajAtomListCopy(&(*to)->Chains[x]->Atoms, from->Chains[x]->Atoms))
	    ajFatal("Error copying Atoms list");	    
    }
    

    return ajTrue;
}





/* ======================================================================= */
/* ============================= Modifiers =============================== */
/* ======================================================================= */

/* @section Modifiers *******************************************************
**
** These functions use the contents of an instance and update them.
**
****************************************************************************/





/* ======================================================================= */
/* ========================== Operators ===================================*/
/* ======================================================================= */

/* @section Operators *******************************************************
**
** These functions use the contents of an instance but do not make any 
** changes.
**
****************************************************************************/



/* @func ajAtomSSEnv **********************************************************
** Assigns secondary structure environment of a residue
** @param  [r] atom [AjPAtom]    Atom object
** @param  [w] SEnv [char] Character for the Secondary structure environment
** @return [AjBool]
** @@
******************************************************************************/
/*Function to assign environment class*/
AjBool   ajAtomSSEnv(AjPAtom atom, char *SEnv, AjPFile logf)
{
    *SEnv='\0';
    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f\n", atom->Id1, atom->Idx, 
		atom->eStrideType, atom->side_rel);
  
    if(atom->eStrideType == 'H' || atom->eStrideType == 'G')
	*SEnv='H';
    else if(atom->eStrideType == 'E' || atom->eStrideType == 'B' || atom->eStrideType == 'b')
	*SEnv='S';
    else if(atom->eStrideType == 'T' || atom->eStrideType == 'C' || atom->eStrideType == 'I')
	*SEnv='C';
    else if(atom->eStrideType == '.')	/*If no stride assignment, get pdb assignment*/
    {
	if(atom->eType == 'H')
	    *SEnv='H';
	else if(atom->eType == 'E')
	    *SEnv='S';
	else if(atom->eType == 'C' || atom->eType == 'T')
	    *SEnv='C';
	else if(atom->eType == '.')
	{
	    /* ajFmtPrintF(logf, "SEnv unassigned for residue %d\n", atom->Idx); */
	    *SEnv='\0';
	    return ajFalse;
	}
    }

    return ajTrue;
}







/* @func ajAtomEnv1 ***********************************************************
** Assigns environment based only of side chain accessibility and secondary
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]                   Number of environments
** @@
******************************************************************************/
ajint   ajAtomEnv1(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    if(SEnv=='\0')
    {
	ajStrClear(OEnv);
	return 0;
    }
  
    if((atom->side_rel <= 15) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((atom->side_rel <= 15) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((atom->side_rel <= 15) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((atom->side_rel > 15) && (atom->side_rel <= 30) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD"); 
    else if((atom->side_rel > 15) && (atom->side_rel <= 30) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((atom->side_rel > 15) && (atom->side_rel <= 30) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((atom->side_rel > 30) && (atom->side_rel <= 45) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((atom->side_rel > 30) && (atom->side_rel <= 45) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((atom->side_rel > 30) && (atom->side_rel <= 45) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((atom->side_rel > 45) && (atom->side_rel <= 60) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((atom->side_rel > 45) && (atom->side_rel <= 60) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((atom->side_rel > 45) && (atom->side_rel <= 60) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL");
    else if((atom->side_rel > 60) && (atom->side_rel <= 75) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((atom->side_rel > 60) && (atom->side_rel <= 75) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");  
    else if((atom->side_rel > 60) && (atom->side_rel <= 75) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO"); 
    else if((atom->side_rel > 75) && (atom->side_rel <= 90) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((atom->side_rel > 75) && (atom->side_rel <= 90) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((atom->side_rel > 75) && (atom->side_rel <= 90) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((atom->side_rel > 90) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");  
    else if((atom->side_rel > 90) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((atom->side_rel > 90) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else
    {
	ajStrClear(OEnv);
	/* ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx); */
	return ajFalse;
    }

    /*The total number of environments*/
    return 21;
}







/* @func ajAtomEnv2 ***********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv2(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=40;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=114;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float HLimit=45;			/*Upper limit for the fraction polar measure of a Hydrophobic residue*/
    float MPLimit=67;			/*Upper limit for the fraction polar measure of a Moderately oplar residue*/
  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return ajFalse;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/

    /*Buried, Hydrophobic*/
    if((atom->side_rel <= BLimit) && (atom->pol_rel <= HLimit))
	ajStrAssC(&BEnv, "B1");  
    /*buried, moderately polar*/
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > HLimit) && (atom->pol_rel <= MPLimit)) 
	ajStrAssC(&BEnv, "B2");
    /*buried, polar*/
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > MPLimit))
	ajStrAssC(&BEnv, "B3");
    /*Partially buried, moderately Polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) &&  (atom->pol_rel <= MPLimit))
	ajStrAssC(&BEnv, "P1");
    /*Partially buried, Polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > MPLimit))
	ajStrAssC(&BEnv, "P2");
    /*Exposed*/
    else if(atom->side_rel > PBLimit)
	ajStrAssC(&BEnv, "E");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return ajFalse;
    }

    ajStrDel(&BEnv);

    return 18;

}







/* @func ajAtomEnv3 ***********************************************************
** Assigns environment based on side chain accessibility and secondary
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv3(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=30;
    float PolLimit3=50;
    float PolLimit4=80;

  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/

    /*Buried, Hydrophobic*/
    if((atom->side_rel <= BLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "B1");  
    /*buried, moderately polar*/
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2)) 
	ajStrAssC(&BEnv, "B2");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3)) 
	ajStrAssC(&BEnv, "B3");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4)) 
	ajStrAssC(&BEnv, "B4");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit4)) 
	ajStrAssC(&BEnv, "B5");
    /*Partially buried, moderately Polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) &&  (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "P1");
    /*Partially buried, Polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit3))
	ajStrAssC(&BEnv, "P2");
    /*Exposed*/
    else if(atom->side_rel > PBLimit)
	ajStrAssC(&BEnv, "E");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return ajFalse;
    }

    ajStrDel(&BEnv);

    return 24;

}

/* @func ajAtomEnv4 ***********************************************************
** Assigns environment based only of side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv4(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    ajStrClear(OEnv);
    if(SEnv=='\0')
    {
	ajStrClear(OEnv);
	return 0;
    }
  
    if((atom->side_rel <= 5) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((atom->side_rel <= 5) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((atom->side_rel <= 5) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((atom->side_rel > 5) && (atom->side_rel <= 25) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((atom->side_rel > 5) && (atom->side_rel <= 25) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((atom->side_rel > 5) && (atom->side_rel <= 25) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((atom->side_rel > 25) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((atom->side_rel > 25) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((atom->side_rel > 25) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else
    {
	ajStrClear(OEnv);
	/* ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx); */
	return 0;
    }

    return 9;

}







/* @func ajAtomEnv5 ***********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv5(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=30;
    float PolLimit3=50;
    float PolLimit4=80;

  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/

    /*Buried*/
    if(atom->side_rel <= BLimit)
	ajStrAssC(&BEnv, "B");  
    /*partially buried, hydrophobic*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "P1");  
    /*partially buried, moderately polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2)) 
	ajStrAssC(&BEnv, "P2");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3)) 
	ajStrAssC(&BEnv, "P3");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4)) 
	ajStrAssC(&BEnv, "P4");
    /*partially buried, polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit4)) 
	ajStrAssC(&BEnv, "P5");
    /*Exposed, moderately Polar*/
    else if((atom->side_rel > PBLimit) &&  (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "E1");
    /*Exposed, Polar*/
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit3))
	ajStrAssC(&BEnv, "E2");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "P4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "P4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "P4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "P5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "P5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "P5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return ajFalse;
    }

    ajStrDel(&BEnv);

    return 24;

}








/* @func ajAtomEnv6 ***********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv6(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=30;
    float PolLimit3=50;
    float PolLimit4=80;

  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/

    /*Buried*/
    if(atom->side_rel <= BLimit)
	ajStrAssC(&BEnv, "B");  
    /*Partially buried, hydrophobic*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) &&  (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "P1");
    /*Partially buried, Polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit3))
	ajStrAssC(&BEnv, "P2");
    /*partially buried, hydrophobic*/
    else if((atom->side_rel > PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "E1");  
    /*partially buried, moderately polar*/
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2)) 
	ajStrAssC(&BEnv, "E2");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3)) 
	ajStrAssC(&BEnv, "E3");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4)) 
	ajStrAssC(&BEnv, "E4");
    /*partially buried, polar*/
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit4)) 
	ajStrAssC(&BEnv, "E5");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return ajFalse;
    }

    ajStrDel(&BEnv);

    return 24;

}









/* @func ajAtomEnv7 ***********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [AjBool]
** @@
******************************************************************************/
ajint   ajAtomEnv7(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=50;
    float PolLimit3=90;
  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/

    /*Buried*/
    if((atom->side_rel <= BLimit) && (atom->pol_rel <=PolLimit1))
	ajStrAssC(&BEnv, "B1");  
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "B2"); 
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit3))
	ajStrAssC(&BEnv, "B3");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit))
	ajStrAssC(&BEnv, "P");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "E1");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2))
	ajStrAssC(&BEnv, "E2");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "E3");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit3))
	ajStrAssC(&BEnv, "E4");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);

    return 24;

}






/* @func ajAtomEnv8 ***********************************************************
** Assigns environment based only of side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv8(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    ajStrClear(OEnv);
    if(SEnv=='\0')
    {
	ajStrClear(OEnv);
	return 0;
    }
  
    if((atom->pol_rel <= 15) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((atom->pol_rel <= 15) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((atom->pol_rel <= 15) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((atom->pol_rel > 15) && (atom->pol_rel <= 30) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((atom->pol_rel > 15) && (atom->pol_rel <= 30) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((atom->pol_rel > 15) && (atom->pol_rel <= 30) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((atom->pol_rel > 30) && (atom->pol_rel <= 45) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((atom->pol_rel > 30) && (atom->pol_rel <= 45) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((atom->pol_rel > 30) && (atom->pol_rel <= 45) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((atom->pol_rel > 45) && (atom->pol_rel <= 60) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((atom->pol_rel > 45) && (atom->pol_rel <= 60) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((atom->pol_rel > 45) && (atom->pol_rel <= 60) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL");
    else if((atom->pol_rel > 60) && (atom->pol_rel <= 75) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((atom->pol_rel > 60) && (atom->pol_rel <= 75) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");  
    else if((atom->pol_rel > 60) && (atom->pol_rel <= 75) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO"); 
    else if((atom->pol_rel > 75) && (atom->pol_rel <= 90) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((atom->pol_rel > 75) && (atom->pol_rel <= 90) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((atom->pol_rel > 75) && (atom->pol_rel <= 90) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((atom->pol_rel > 90) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");  
    else if((atom->pol_rel > 90)&& (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((atom->pol_rel > 90)&& (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else
    {
	ajStrClear(OEnv);
	/* ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx); */
	return 0;
    }

    return 21;

}






/* @func ajAtomEnv9 ***********************************************************
** Assigns environment based only of side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv9(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    ajStrClear(OEnv);
    if(SEnv=='\0')
    {
	ajStrClear(OEnv);
	return 0;
    }
  
    if((atom->pol_rel <= 5) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((atom->pol_rel <= 5) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((atom->pol_rel <= 5) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((atom->pol_rel > 5) && (atom->pol_rel <= 25) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((atom->pol_rel > 5) && (atom->pol_rel <= 25) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((atom->pol_rel > 5) && (atom->pol_rel <= 25) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((atom->pol_rel > 25) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((atom->pol_rel > 25) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((atom->pol_rel > 25) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else
    {
	ajStrClear(OEnv);
	/* ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx); */
	return 0;
    }

    return 9;

}






/* @func ajAtomEnv10 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv10(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float HLimit=5;			/*Upper limit for the fraction polar measure of a Hydrophobic residue*/
    float MPLimit=25;			/*Upper limit for the fraction polar measure of a Moderately oplar residue*/
  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/
  
    /*Buried, Hydrophobic*/
    if((atom->side_rel <= BLimit) && (atom->pol_rel <= HLimit))
	ajStrAssC(&BEnv, "B1");  
    /*buried, moderately polar*/
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > HLimit) && (atom->pol_rel <= MPLimit)) 
	ajStrAssC(&BEnv, "B2");
    /*buried, polar*/
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > MPLimit))
	ajStrAssC(&BEnv, "B3");
    /*Partially buried, moderately hydrophobic*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel <= HLimit))
	ajStrAssC(&BEnv, "P1");
    /*Partially buried, moderately polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > HLimit) &&(atom->pol_rel <= MPLimit))
	ajStrAssC(&BEnv, "P2");
    /*Partially buried, polar*/
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > MPLimit))
	ajStrAssC(&BEnv, "P3");
    /*Exposed, moderately polar*/
    else if((atom->side_rel > PBLimit) && (atom->pol_rel <= MPLimit))
	ajStrAssC(&BEnv, "E1");
    /*Exposed, polar*/
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > MPLimit))
	ajStrAssC(&BEnv, "E2");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);
  
    return 24;

}










/* @func ajAtomEnv11 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajing]
** @@
******************************************************************************/
ajint   ajAtomEnv11(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=30;
    float PolLimit3=50;
    float PolLimit4=70;
    float PolLimit5=90;

  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/
  
    if((atom->side_rel <= BLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "B1");    
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2)) 
	ajStrAssC(&BEnv, "B2");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3)) 
	ajStrAssC(&BEnv, "B3");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4)) 
	ajStrAssC(&BEnv, "B4");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit4) && (atom->pol_rel <= PolLimit5)) 
	ajStrAssC(&BEnv, "B5");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit5)) 
	ajStrAssC(&BEnv, "B6");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit))
	ajStrAssC(&BEnv, "P");
    else if(atom->side_rel > PBLimit)
	ajStrAssC(&BEnv, "E");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);

    return 24;

} 







/* @func ajAtomEnv12 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv12(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=30;
    float PolLimit3=50;
    float PolLimit4=70;
    float PolLimit5=90;

  
    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/
  
    if((atom->side_rel <= BLimit))
	ajStrAssC(&BEnv, "B");    
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "P1");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2))
	ajStrAssC(&BEnv, "P2");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "P3");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4))
	ajStrAssC(&BEnv, "P4");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit4) && (atom->pol_rel <= PolLimit5))
	ajStrAssC(&BEnv, "P5");
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit5))
	ajStrAssC(&BEnv, "P6");
    else if(atom->side_rel > PBLimit)
	ajStrAssC(&BEnv, "E"); 
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "P1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "P2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "P3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "P4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "P4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "P4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "P5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "P5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "P5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "P6")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "P6")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "P6")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);

    return 24;

} 









/* @func ajAtomEnv13 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv13(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=10;
    float PolLimit2=30;
    float PolLimit3=50;
    float PolLimit4=70;
    float PolLimit5=90;

    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);
  
    /*Assign the basic classes*/
    if((atom->side_rel <= BLimit))
	ajStrAssC(&BEnv, "B");    
    else if((atom->side_rel > BLimit) && (atom->side_rel <= PBLimit))
	ajStrAssC(&BEnv, "P");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "E1");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2))
	ajStrAssC(&BEnv, "E2");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "E3");
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4))
	ajStrAssC(&BEnv, "E4"); 
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit4) && (atom->pol_rel <= PolLimit5))
	ajStrAssC(&BEnv, "E5"); 
    else if((atom->side_rel > PBLimit) && (atom->pol_rel > PolLimit5))
	ajStrAssC(&BEnv, "E6");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }
  
    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "P")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "E1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "E2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "E3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "E4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "E5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "E5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "E5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E6")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E6")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E6")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);

    return 24;

} 








/* @func ajAtomEnv14 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]         AtomStride object
** @param [w] OEnv [char]            Character for the overall environment class 
** @return [AjBool]
** @@
******************************************************************************/
ajint   ajAtomEnv14(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=5;
    float PolLimit2=15;
    float PolLimit3=25;
    float PolLimit4=40;
    float PolLimit5=60;
    float PolLimit6=80;
 

    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);

    if((atom->side_rel <= PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "B1");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2))
	ajStrAssC(&BEnv, "B2");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "B3");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4))
	ajStrAssC(&BEnv, "B4");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit4) && (atom->pol_rel <= PolLimit5))
	ajStrAssC(&BEnv, "B5");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit5) && (atom->pol_rel <= PolLimit6))
	ajStrAssC(&BEnv, "B6");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit6))
	ajStrAssC(&BEnv, "B7");
    else if((atom->side_rel > PBLimit))
	ajStrAssC(&BEnv, "E");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);
    return 24;

} 







/* @func ajAtomEnv15 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]      AtomStride object
** @param [w] OEnv [char]         Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv15(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float PBLimit=25;			/*Upper limit for the relative solvent accessible area for a Parially buried residue*/
  
    float PolLimit1=5;
    float PolLimit2=15;
    float PolLimit3=25;
    float PolLimit4=35;
    float PolLimit5=45;
    float PolLimit6=75;
 

    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, atom->eType, atom->side_rel, atom->pol_rel);

    if((atom->side_rel <= PBLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "B1");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2))
	ajStrAssC(&BEnv, "B2");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "B3");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4))
	ajStrAssC(&BEnv, "B4");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit4) && (atom->pol_rel <= PolLimit5))
	ajStrAssC(&BEnv, "B5");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit5) && (atom->pol_rel <= PolLimit6))
	ajStrAssC(&BEnv, "B6");
    else if((atom->side_rel <= PBLimit) && (atom->pol_rel > PolLimit6))
	ajStrAssC(&BEnv, "B7");
    else if((atom->side_rel > PBLimit))
	ajStrAssC(&BEnv, "E");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);

    return 24;
} 








/* @func ajAtomEnv16 **********************************************************
** Assigns environment based on side chain accessibility and secondary 
** structure.
** @param [r] atom [AjPAtom]    AtomStride object
** @param [w] OEnv [char]       Character for the overall environment class 
** @return [ajint]
** @@
******************************************************************************/
ajint   ajAtomEnv16(AjPAtom atom, char SEnv, AjPStr *OEnv, AjPFile logf)
{
    float BLimit=5;			/*Upper limit for the relative solvent accessible area for a buried residue*/
  
    float PolLimit1=5;
    float PolLimit2=15;
    float PolLimit3=25;
    float PolLimit4=35;
    float PolLimit5=45;
    float PolLimit6=75;
 

    AjPStr   BEnv=NULL;

    if(!atom)
    {
	ajWarn("No atom to ajAtomEnv");
	return 0;
    }
  
    ajStrClear(OEnv);
    BEnv=ajStrNew();

    ajFmtPrintF(logf, "R:%c-%d S:%c A:%.2f f:%.2f\n", atom->Id1, atom->Idx, 
		atom->eType, atom->side_rel, atom->pol_rel);

    if((atom->side_rel <= BLimit) && (atom->pol_rel <= PolLimit1))
	ajStrAssC(&BEnv, "B1");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit1) && (atom->pol_rel <= PolLimit2))
	ajStrAssC(&BEnv, "B2");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit2) && (atom->pol_rel <= PolLimit3))
	ajStrAssC(&BEnv, "B3");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit3) && (atom->pol_rel <= PolLimit4))
	ajStrAssC(&BEnv, "B4");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit4) && (atom->pol_rel <= PolLimit5))
	ajStrAssC(&BEnv, "B5");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit5) && (atom->pol_rel <= PolLimit6))
	ajStrAssC(&BEnv, "B6");
    else if((atom->side_rel <= BLimit) && (atom->pol_rel > PolLimit6))
	ajStrAssC(&BEnv, "B7");
    else if((atom->side_rel > BLimit))
	ajStrAssC(&BEnv, "E");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "BEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    /*Assign overall environment class*/
    if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AA");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AB");
    else if((ajStrMatchC(BEnv, "B1")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AC");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AD");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AE");
    else if((ajStrMatchC(BEnv, "B2")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AF");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AG");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AH");
    else if((ajStrMatchC(BEnv, "B3")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AI");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AJ");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AK");
    else if((ajStrMatchC(BEnv, "B4")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AL"); 
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AM");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AN");
    else if((ajStrMatchC(BEnv, "B5")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AO");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AP");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AQ");
    else if((ajStrMatchC(BEnv, "B6")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AR");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AS");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AT");
    else if((ajStrMatchC(BEnv, "B7")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AU");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'H'))
	ajStrAssC(OEnv,"AV");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'S'))
	ajStrAssC(OEnv,"AW");
    else if((ajStrMatchC(BEnv, "E")) && (SEnv == 'C'))
	ajStrAssC(OEnv,"AX");
    else
    {
	ajStrClear(OEnv);
	ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
	return 0;
    }

    ajStrDel(&BEnv);

    return 24;

} 






/* @func ajPdbGetEStrideType ************************************************
**
** Reads a Pdb object and writes a string with the secondary structure. The
** string that is written is the same length as the full-length chain 
** (regardless of whether coordinates for a residue were available or not) 
** therefore it can be indexed into using residue numbers.  The string is 
** allocated if necessary.  If secondary structure assignment was not available
** for a residue a '.' is given in the string.
**
** @param [r] obj [const AjPPdb]  Pdb object
** @param [r] chn [ajint]   Chain number
** @param [w] EStrideType [AjPStr *] String to hold secondary structure
**
** @return [ajint] Length (residues) of array that was written or -1 (for 
**                 an error)
** @@
****************************************************************************/

ajint  ajPdbGetEStrideType(const AjPPdb obj, ajint chn, AjPStr *EStrideType)
{
    AjPAtom tmp    = NULL;
    AjIList iter   = NULL;
    ajint idx      = 0;
    ajint last_res = -10000;
    

    if(!obj || !EStrideType || (chn<1))
    {
	ajWarn("Bad args passed to ajPdbGetEStrideType");
	return -1;
    }
    if(chn > obj->Nchn)
    {
	ajWarn("chn arg in ajPdbGetEStrideType exceeds no. chains");
	return -1;
    }
    else 
	idx = chn-1;
    
    /* +1 is for the NULL */
    if(!(*EStrideType))
	*EStrideType = ajStrNewL(obj->Chains[idx]->Nres+1);
    else
    {
	ajStrDel(EStrideType);
	*EStrideType = ajStrNewL(obj->Chains[idx]->Nres+1);
    }
    
    /* Set all positions to . */
    ajStrAppKI(EStrideType,  (const char) ".", obj->Chains[idx]->Nres);   

    iter=ajListIterRead(obj->Chains[idx]->Atoms);
    while((tmp=(AjPAtom)ajListIterNext(iter)))
    {
	/* New residue */
	if(tmp->Idx != last_res)
	{
	    (*EStrideType)->Ptr[tmp->Idx-1] = tmp->eStrideType;
	    last_res = tmp->Idx;
	}
	else
	    continue;
    }
    

    ajListIterFree(&iter);
    return obj->Chains[idx]->Nres;
}





/* ======================================================================= */
/* ============================== Casts ===================================*/
/* ======================================================================= */

/* @section Casts ***********************************************************
**
** These functions examine the contents of an instance and return some
** derived information. Some of them provide access to the internal
** components of an instance. They are provided for programming convenience
** but should be used with caution.
**
****************************************************************************/





/* ======================================================================= */
/* =========================== Reporters ==================================*/
/* ======================================================================= */

/* @section Reporters *******************************************************
**
** These functions return the contents of an instance but do not make any 
** changes.
**
****************************************************************************/

/* @func ajPdbChnidToNum ****************************************************
**
** Finds the chain number for a given chain identifier in a pdb structure
**
** @param [r] id  [char]    Chain identifier
** @param [r] pdb [const AjPPdb]  Pdb object
** @param [w] chn [ajint *] Chain number
**
** @return [AjBool] True on succcess
** @@
****************************************************************************/

AjBool ajPdbChnidToNum(char id, const AjPPdb pdb, ajint *chn)
{
    ajint a;
    
    for(a=0;a<pdb->Nchn;a++)
    {
	if(toupper((int)pdb->Chains[a]->Id) == toupper((int)id))
	{
	    *chn=a+1;
	    return ajTrue;
	}

	/*
	** Cope with chain id's of ' ' (which might be given as '.' in 
	**the Pdb object)
	*/
	if((id==' ')&&(pdb->Chains[a]->Id=='.'))
	{
	    *chn=a+1;
	    return ajTrue;
	}
    }
    
    /*
    ** A '.' may be given as the id for domains comprising more than one
    ** chain
    */
    if(id=='.')
    {
	*chn=1;
	return ajTrue;
    }
	
    return ajFalse;
}





/* @func ajPdbtospArrFindPdbid **********************************************
**
** Performs a binary search for a PDB code over an array of Pdbtosp
** structures (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] arr [const AjPPdbtosp*] Array of AjOPdbtosp objects
** @param [r] siz [ajint] Size of array
** @param [r] id  [const AjPStr]      Search term
**
** @return [ajint] Index of first Pdbtosp object found with an PDB code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/

ajint ajPdbtospArrFindPdbid(const AjPPdbtosp *arr, ajint siz, const AjPStr id)
{
    int l;
    int m;
    int h;
    int c;


    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m=(l+h)>>1;

        if((c=ajStrCmpCase(id, arr[m]->Pdb)) < 0) 
	    h=m-1;
        else if(c>0) 
	    l=m+1;
        else 
	    return m;
    }

    return -1;
}





/* ======================================================================= */
/* ========================== Input & Output ============================= */
/* ======================================================================= */

/* @section Input & output **************************************************
**
** These functions are used for formatted input and output to file.    
**
****************************************************************************/

/* @func ajPdbWriteAll *****************************************************
**
** Writes a clean coordinate file for a protein.
**
** @param [w] outf [AjPFile] Output file stream
** @param [r] obj  [const AjPPdb]  Pdb object
**
** @return [AjBool] True on success
** @category output [AjPPdb] Writes a ccf-format file for a protein.
** @@
****************************************************************************/

AjBool ajPdbWriteAll(AjPFile outf, const AjPPdb obj)
{
    ajint x      = 0;
    ajint y      = 0;
    AjIList iter = NULL;
    AjPAtom tmp  = NULL;
    
    /* Write the header information */

    ajFmtPrintF(outf, "%-5s%S\n", "ID", obj->Pdb);
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintSplit(outf,obj->Compnd,"DE   ",75," \t\r\n");
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintSplit(outf,obj->Source,"OS   ",75," \t\r\n");
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintF(outf, "%-5sMETHOD ", "EX");
    if(obj->Method == ajXRAY)
	ajFmtPrintF(outf, "xray; ");	
    else
	ajFmtPrintF(outf, "nmr_or_model; ");		
    ajFmtPrintF(outf, "RESO %.2f; NMOD %d; NCHN %d; NGRP %d;\n", obj->Reso,
		obj->Nmod, obj->Nchn, obj->Ngp);


    /* Write chain-specific information */
    for(x=0;x<obj->Nchn;x++)
    { 
	ajFmtPrintF(outf, "XX\n");	
	ajFmtPrintF(outf, "%-5s[%d]\n", 
		    "CN", 
		    x+1);	
	ajFmtPrintF(outf, "XX\n");	
 
	ajFmtPrintF(outf, "%-5s", "IN");

	if(obj->Chains[x]->Id == ' ')
	    ajFmtPrintF(outf, "ID %c; ", '.');
	else
	    ajFmtPrintF(outf, "ID %c; ", obj->Chains[x]->Id);

	
	/*
	ajFmtPrintF(outf, "NR %d; NL %d; NH %d; NE %d; NS %d; NT %d;\n", 
		    obj->Chains[x]->Nres,
		    obj->Chains[x]->Nlig,
		    obj->Chains[x]->numHelices, 
		    obj->Chains[x]->numStrands, 
		    obj->Chains[x]->numSheets, 
		    obj->Chains[x]->numTurns);
		    */

	ajFmtPrintF(outf, "NR %d; NL %d; NH %d; NE %d;\n", 
		    obj->Chains[x]->Nres,
		    obj->Chains[x]->Nlig,
		    obj->Chains[x]->numHelices, 
		    obj->Chains[x]->numStrands);

	/*
	ajFmtPrintF(outf, "%-5sID %c; NR %d; NH %d; NW %d;\n", 
		    "IN", 
		    obj->Chains[x]->Id,
		    obj->Chains[x]->Nres,
		    obj->Chains[x]->Nhet,
		    obj->Chains[x]->Nwat);
		    */
	ajFmtPrintF(outf, "XX\n");	
	ajSeqWriteXyz(outf, obj->Chains[x]->Seq, "SQ");
    }
    ajFmtPrintF(outf, "XX\n");	

    /* printf("NCHN: %d   NMOD: %d\n", obj->Nchn, obj->Nmod); */
    
    

    /* Write coordinate list */
    for(x=1;x<=obj->Nmod;x++)
    {
	for(y=0;y<obj->Nchn;y++)
	{
	    /* Print out chain-specific coordinates */
	    iter=ajListIterRead(obj->Chains[y]->Atoms);
	    while(ajListIterMore(iter))
	    {
		tmp=(AjPAtom)ajListIterNext(iter);
		if(tmp->Mod>x)
		    break;
		else if(tmp->Mod!=x)
		    continue;
		else	
		{
		    if(tmp->Type=='H')
			ajFmtPrintF(outf, "%-5s%-5d%-5d%-5d%-5c%-6c%-6S%-5c"
				    "%-5c%-5c%-5c%-5c%-5c%-2c"
				    "%6S    %-4S"
				    "%8.3f%9.3f%9.3f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f%8.2f%8.2f%8.2f\n", 
				    "CO", 
				    tmp->Mod, 
				    tmp->Chn, 
				    tmp->Gpn, 
				    tmp->Type, 
				    '.',
				    tmp->Pdb, 
				    '.', 
				    '.', 
				    '.', 
				    '.', 
				    '.', 
				    '.', 
				    tmp->Id1,
				    tmp->Id3,
				    tmp->Atm, 
				    tmp->X, 
				    tmp->Y, 
				    tmp->Z,
				    tmp->O,
				    tmp->B, 
				    tmp->Phi,
				    tmp->Psi,
				    tmp->Area, 
				    tmp->all_abs, 
				    tmp->all_rel, 
				    tmp->side_abs, 
				    tmp->side_rel, 
				    tmp->main_abs, 
				    tmp->main_rel, 
				    tmp->npol_abs, 
				    tmp->npol_rel, 
				    tmp->pol_abs, 
				    tmp->pol_rel);
		    else
		    {
			ajFmtPrintF(outf, "%-5s%-5d%-5d%-5c%-5c%-6d%-6S%-5c", 
				    "CO", 
				    tmp->Mod, 
				    tmp->Chn, 
				    '.',
				    tmp->Type, 
				    tmp->Idx, 
				    tmp->Pdb,
				    tmp->eType);
			
			if(tmp->eNum != 0)
			    ajFmtPrintF(outf, "%-5d", tmp->eNum);
			else
			    ajFmtPrintF(outf, "%-5c", '.');


			ajFmtPrintF(outf, "%-5S", tmp->eId);

			if(tmp->eType == 'H')
			    ajFmtPrintF(outf, "%-5d", tmp->eClass);
			else
			    ajFmtPrintF(outf, "%-5c", '.');


			ajFmtPrintF(outf, "%-5c", tmp->eStrideType);
			if(tmp->eStrideNum != 0)
			    ajFmtPrintF(outf, "%-5d", tmp->eStrideNum);
			else
			    ajFmtPrintF(outf, "%-5c", '.');



			ajFmtPrintF(outf, "%-2c%6S    %-4S%8.3f%9.3f%9.3f"
				    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f\n", 
				    tmp->Id1, 
				    tmp->Id3,
				    tmp->Atm, 
				    tmp->X, 
				    tmp->Y, 
				    tmp->Z,
				    tmp->O,
				    tmp->B,
				    tmp->Phi,
				    tmp->Psi,
				    tmp->Area, 
				    tmp->all_abs, 
				    tmp->all_rel, 
				    tmp->side_abs, 
				    tmp->side_rel, 
				    tmp->main_abs, 
				    tmp->main_rel, 
				    tmp->npol_abs, 
				    tmp->npol_rel, 
				    tmp->pol_abs, 
				    tmp->pol_rel);
		    }
		}
	    }
	    ajListIterFree(&iter);			
	} 	

	/* Print out group-specific coordinates for this model */
	iter=ajListIterRead(obj->Groups);
	while(ajListIterMore(iter))
	{
	    tmp=(AjPAtom)ajListIterNext(iter);
	    if(tmp->Mod>x)
		break;
	    else if(tmp->Mod!=x)
		continue;
	    else	
	    {
		ajFmtPrintF(outf, "%-5s%-5d%-5c%-5d%-5c%-6c%-6S%-5c%-5c%-5c"
			    "%-5c%-5c%-5c%-2c"
			    "%6S    %-4S"
			    "%8.3f%9.3f%9.3f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
			    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n", 
			    "CO", 
			    tmp->Mod, 
			    '.',
			    tmp->Gpn, 
			    tmp->Type, 
			    '.', 
			    tmp->Pdb, 
			    '.', 
			    '.', 
			    '.', 
			    '.', 
			    '.', 
			    '.',
			    tmp->Id1,
			    tmp->Id3,
			    tmp->Atm, 
			    tmp->X, 
			    tmp->Y, 
			    tmp->Z,
			    tmp->O,
			    tmp->B,
			    tmp->Phi,
			    tmp->Psi,
			    tmp->Area, 
			    tmp->all_abs, 
			    tmp->all_rel, 
			    tmp->side_abs, 
			    tmp->side_rel, 
			    tmp->main_abs, 
			    tmp->main_rel, 
			    tmp->npol_abs, 
			    tmp->npol_rel, 
			    tmp->pol_abs, 
			    tmp->pol_rel);

	    }
	}
	ajListIterFree(&iter);			


	/* Print out water-specific coordinates for this model */
	iter=ajListIterRead(obj->Water);
	while(ajListIterMore(iter))
	{
	    tmp=(AjPAtom)ajListIterNext(iter);
	    if(tmp->Mod>x)
		break;
	    else if(tmp->Mod!=x)
		continue;
	    else	
	    {
		ajFmtPrintF(outf, "%-5s%-5d%-5c%-5c%-5c%-6c%-6S%-5c%-5c"
			    "%-5c%-5c%-5c%-5c%-2c"
			    "%6S    %-4S"
			    "%8.3f%9.3f%9.3f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
			    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
			    "CO", 
			    tmp->Mod, 
			    '.', 
			    '.', 
			    tmp->Type, 
			    '.', 
			    tmp->Pdb, 
			    '.', 
			    '.', 
			    '.', 
			    '.',
			    '.', 
			    '.',
			    tmp->Id1,
			    tmp->Id3,
			    tmp->Atm, 
			    tmp->X, 
			    tmp->Y, 
			    tmp->Z,
			    tmp->O,
			    tmp->B, 
			    tmp->Phi,
			    tmp->Psi,
			    tmp->Area, 
			    tmp->all_abs, 
			    tmp->all_rel, 
			    tmp->side_abs, 
			    tmp->side_rel, 
			    tmp->main_abs, 
			    tmp->main_rel, 
			    tmp->npol_abs, 
			    tmp->npol_rel, 
			    tmp->pol_abs, 
			    tmp->pol_rel);
	    }
	}
	ajListIterFree(&iter);			
    }
    ajFmtPrintF(outf, "//\n");    


    return ajTrue;
}






/* @func ajPbdWriteSegment *************************************************
**
** Writes a clean coordinate file for a segment, e.g. a domain. The segment 
** corresponds to a sequence that is passed to the function.
** In the clean coordinate file, the coordinates are presented as belonging
** to a single chain.  Coordinates for heterogens are NOT written to file.
**
** @param [w] outf    [AjPFile] Output file stream
** @param [r] pdb     [const AjPPdb]  Pdb object
** @param [r] segment [const AjPStr]  Sequence of segment to print out.
** @param [r] chnid   [char]    Chain id of segment
** @param [r] domain  [const AjPStr]  Domain code for segment
** @param [w] errf    [AjPFile] Output file stream for error messages
**
** @return [AjBool] True on success
** @@
** 
****************************************************************************/
AjBool ajPbdWriteSegment(AjPFile outf, const AjPPdb pdb, const AjPStr segment, 
			  char chnid, const AjPStr domain, AjPFile errf)
{
    ajint chn;
    ajint start     = 0;
    ajint end       = 0;
    char  id;
    
    AjIList  iter        = NULL;
    AjPAtom  atm         = NULL;
    AjPAtom  atm2        = NULL;
    AjBool   found_start = ajFalse;
    AjBool   found_end   = ajFalse;    


   
    /* Check for unknown or zero-length chain */
    if(!ajPdbChnidToNum(chnid, pdb, &chn))
    {
	ajWarn("Chain incompatibility error in "
	       "ajPbdWriteSegment");			
		
	ajFmtPrintF(errf, "//\n%S\nERROR Chain incompatibility "
		    "error in ajPbdWriteDomain\n", domain);
	return ajFalse;
    }
    else if(pdb->Chains[chn-1]->Nres==0)
    {		
	ajWarn("Chain length zero");			
	    
	ajFmtPrintF(errf, "//\n%S\nERROR Chain length zero\n", 
		    domain);
	return ajFalse;
    }
    
    /* Check if segment exists in this chain */
    if((start = ajStrFind(pdb->Chains[chn-1]->Seq, segment)) == -1)
    {
	ajWarn("Domain not found in ajPbdWriteSegment");
	ajFmtPrintF(errf, "//\n%S\nERROR Domain not found "
		    "in ajPbdWriteSegment\n", domain);
	return ajFalse;
    }
    else	
    {
	/* Residue numbers start at 1 ! */
	start++;
	end = start + MAJSTRLEN(segment) - 1;
    }  	  
    
    
    /* Write header info. to domain coordinate file */
    ajFmtPrintF(outf, "%-5s%S\n", "ID", domain);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sCo-ordinates for domain %S\n", 
		"DE", domain);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sDomain defined from sequence segment\n", 
		"OS");
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sMETHOD ", "EX");
    if(pdb->Method == ajXRAY)
	ajFmtPrintF(outf, "xray; ");	
    else
	ajFmtPrintF(outf, "nmr_or_model; ");		
    /* The NCHN and NMOD are hard-coded to 1 for domain files */
    ajFmtPrintF(outf, "RESO %.2f; NMOD 1; NCHN 1; NGRP 0;\n", 
		pdb->Reso);
    

    id = pdb->Chains[chn-1]->Id;
    if(id == ' ')
	id = '.';

    
    /* Write sequence to domain coordinate file */
    ajFmtPrintF(outf, "XX\n");	
    ajFmtPrintF(outf, "%-5s[1]\n", "CN");	
    ajFmtPrintF(outf, "XX\n");	
    
    ajFmtPrintF(outf, "%-5sID %c; NR %d; NL 0; NH 0; NE 0;\n", 
		"IN", 
		id,
		MAJSTRLEN(segment));
    ajFmtPrintF(outf, "XX\n");	
    ajSeqWriteXyz(outf, segment, "SQ");
    ajFmtPrintF(outf, "XX\n");	
    
    
    /* Write co-ordinates list to domain coordinate file */        
    ajPdbChnidToNum(chnid, pdb, &chn);
    
    /* Initialise the iterator */
    iter = ajListIterRead(pdb->Chains[chn-1]->Atoms);
    
    
    /* Iterate through the list of atoms */
    while((atm=(AjPAtom)ajListIterNext(iter)))
    {
	if(atm->Mod!=1)
	    break;
	if(atm->Type!='P')
	    continue;
	if(!found_start)
	{
	    if(atm->Idx == start)
		found_start = ajTrue;	
	    else		
		continue;
	}	
	if(!found_end)
	{
	    if(atm->Idx == end)
		found_end = ajTrue;     
	}
	/*  The end position has been found, and the current atom no longer
	 ** belongs to this final residue.
	 */
	else if(atm->Idx != end && found_end)
	    break;
	    
	    
	/* Print out coordinate line */
	ajFmtPrintF(outf, "%-5s%-5d%-5d%-5c%-5c%-6d%-6S%-5c",
		    "CO", 
		    atm->Mod,		/* It will always be 1 */
		    1,			/* chn number is always given as 1 */
		    '.',
		    atm->Type, 
		    atm->Idx-start+1, 
		    atm->Pdb, 
		    atm->eType);
	if(atm->eNum != 0)
	    ajFmtPrintF(outf, "%-5d", atm->eNum);
	else
	    ajFmtPrintF(outf, "%-5c", '.');
	ajFmtPrintF(outf, "%-5S", atm->eId);

	if(atm->eType == 'H')
	    ajFmtPrintF(outf, "%-5d", atm->eClass);
	else
	    ajFmtPrintF(outf, "%-5c", '.');


	ajFmtPrintF(outf, "%-5c", atm->eStrideType);
	if(atm->eStrideNum != 0)
	    ajFmtPrintF(outf, "%-5d", atm->eStrideNum);
	else
	    ajFmtPrintF(outf, "%-5c", '.');

	ajFmtPrintF(outf, "%-2c%6S    %-4S%8.3f%9.3f%9.3f%8.2f%8.2f"
		    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
		    "%8.2f%8.2f%8.2f%8.2f\n", 
		    atm->Id1, 
		    atm->Id3,
		    atm->Atm, 
		    atm->X, 
		    atm->Y, 
		    atm->Z, 
		    atm->O, 
		    atm->B,
		    atm->Phi,
		    atm->Psi,
		    atm->Area, 
		    atm->all_abs, 
		    atm->all_rel, 
		    atm->side_abs, 
		    atm->side_rel, 
		    atm->main_abs, 
		    atm->main_rel, 
		    atm->npol_abs, 
		    atm->npol_rel, 
		    atm->pol_abs, 
		    atm->pol_rel);
	    
	    
	/* Assign pointer for this chain */
	atm2 = atm;
    }
    
    ajListIterFree(&iter);			
    
    
    
    /* Write last line in file */
    ajFmtPrintF(outf, "//\n");    
    
    
    return ajTrue;
}







/* @func ajHetWrite ********************************************************
**
** Writes the contents of a Het object to file. 
**
** @param [w] outf    [AjPFile]   Output file
** @param [r] obj     [const AjPHet]    Het object
** @param [r] dogrep  [AjBool]    Flag (True if we are to write <cnt>
**                                element of the Het object to file).
**
** @return [AjBool] True on success
** @category output [AjPHet] Write Het object to file in clean
** @@
****************************************************************************/

AjBool ajHetWrite(AjPFile outf, const AjPHet obj, AjBool dogrep)
{
    ajint i = 0;
    
    /* Check arg's */
    if(!outf || !obj)
	return ajFalse;
    
    
    for(i=0;i<obj->n; i++)
    {
	ajFmtPrintF(outf, "ID   %S\n", obj->entries[i]->abv);
	ajFmtPrintSplit(outf, obj->entries[i]->ful, "DE   ", 70, " \t\n\r");
	ajFmtPrintSplit(outf, obj->entries[i]->syn, "SY   ", 70, " \t\n\r");
	if(dogrep)
	    ajFmtPrintF(outf, "NN   %d\n", obj->entries[i]->cnt);
	ajFmtPrintF(outf, "//\n");
    }

    return ajTrue;
}









/* @func ajPdbtospWrite ****************************************************
**
** Reads a list of Pdbtosp objects, e.g. taken from the swissprot:pdb 
** equivalence table available at URL:
**  (1) http://www.expasy.ch/cgi-bin/lists?pdbtosp.txt)
** and writes the list out to file in embl-like format (see 
** documentation for DOMAINATRIX "pdbtosp" application).
**
** @param [w] outf  [AjPFile] Output file   
** @param [r] list  [const AjPList] List of Pdbtosp objects   
**
** @return [AjBool] True of file was written ok.
** @@
**
****************************************************************************/

AjBool       ajPdbtospWrite(AjPFile outf, const AjPList list)
{
    AjIList    iter = NULL;
    AjPPdbtosp tmp  = NULL;
    ajint        x  = 0;
    

    if(!outf || !list)
    {
	ajWarn("Bad args passed to ajPdbtospWrte");
	return ajFalse;
    }
    
    iter = ajListIterRead(list);
    
    while((tmp=(AjPPdbtosp)ajListIterNext(iter)))
    {
	ajFmtPrintF(outf, "%-5s%S\nXX\n%-5s%d\nXX\n", 
		    "EN", tmp->Pdb, "NE", tmp->n);	

	for(x=0; x<tmp->n; x++)
	    ajFmtPrintF(outf, "%-5s%S ID; %S ACC;\n", 
			"IN", tmp->Spr[x], tmp->Acc[x]);
	ajFmtPrintF(outf, "XX\n//\n");
    }

    return ajTrue;
}






/* ======================================================================= */
/* ======================== Miscellaneous =================================*/
/* ======================================================================= */
/* @section Miscellaneous ***************************************************
**
** These functions may have diverse functions that do not fit into the other
** categories. 
**
****************************************************************************/

