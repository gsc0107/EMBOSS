/* @source sigscan application
**
** Generates LHF files (ligand hits file) of hits from scanning sequence(s) 
** against a library of ligand-binding signatures.
**
**
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*******************************************************************************
** 
**  SIGSCAN documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Automatic generation and evaluation of sparse protein signatures for 
**  families of protein structural domains. MJ Blades, JC Ison, R Ranasinghe
**  and JBC Findlay. Protein Science. 2005 (accepted)
**  
**  Email Jon Ison (jison@rfcgr.mrc.ac.uk)
**  
**  NOTES
** 
******************************************************************************/





#include "emboss.h"
#include <math.h>







/******************************************************************************
**
** OBJECT DEFINITIONS
**
******************************************************************************/

/* @data AjPLighit ************************************************************
** 
** Lighit object.
**
** Holds data for ligand hit.
**
** AjPLighit is implemented as a pointer to a C data structure.
**
** @alias AjSLighit
** @alias AjOLighit
**
** @attr  ligid [AjPStr]  Ligand id code. 
** @attr  ns    [ajint]   No. of sites achieving >= threshold score. 
** @attr  np    [ajint]   No. of patches achieving >= threshold score. 
** @attr  score [float]   Score of hit. 
** 
** @new    embSigposNew Default Sigdat object constructor
** @delete embSigposDel Default Sigdat object destructor
** @@
******************************************************************************/
typedef struct AjSLighit
{
    AjPStr ligid;
    ajint  ns;
    ajint  np;
    float  score;
} AjOLighit, *AjPLighit;







/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
AjBool sigscanlig_WriteFasta(AjPFile outf, AjPList siglist, AjPList hits);
AjBool sigscanlig_WriteFastaHit(AjPFile outf, AjPList siglist, AjPList hits, ajint n, AjBool DOSEQ);
static AjBool sigscanlig_SignatureAlignWriteBlock(AjPFile outf,
						  AjPList siglist, 
						  AjPList hits);
void sigscanlig_LigHitDel(AjPLighit *obj);
AjPLighit sigscanlig_LighitNew(void);
AjPList sigscanlig_score_ligands_patch(AjPList hits, AjBool DOMAX, ajint maxhits);
AjPList sigscanlig_score_ligands_site(AjPList hits, AjBool DOMAX, ajint maxhits);
ajint sigscanlig_MatchinvScore(const void *hit1, const void *hit2);
void sigscanlig_WriteResults(AjPList results, AjPFile resultsf);





/* @prog sigscan **************************************************************
**
** Generates LHF files (ligand hits file) of hits from scanning sequence(s) 
** against a library of ligand-binding signatures.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPList      sigin   = NULL;   /* Signature input file names.            */
    AjPStr       signame = NULL;   /* Name of signature file.                */
    AjPFile      sigf    = NULL;   /* Signature input file.                  */
    AjPSignature sig     = NULL;   /* Signature.                             */
    AjPList      siglist = NULL;   /* List of signatures.                    */
    AjIList      sigiter = NULL;   /* Iterator for siglist.                  */
    AjBool       sigok  = ajFalse; /* True if signature processed ok.        */
    
    AjPHit       hit = NULL;      /* Hit to store signature-sequence match.  */
    AjPList      hits = NULL;     /* List of hits */
    ajint        nhits=0;         /* Max. no. of hits to write to output files*/    


    AjPList      ligands = NULL;     /* List of top-scoring ligands. */

    AjPSeqall    database=NULL;   /* Protein sequences to match signature 
				     against.                                */
    AjPSeq       seq = NULL;      /* Current sequence.                       */
    AjPMatrixf   sub  =NULL;      /* Residue substitution matrix.            */
    float        gapo =0.0;       /* Gap insertion penalty.                  */
    float        gape =0.0;       /* Gap extension penalty.                  */

    AjPStr       *nterm=NULL;     /* Holds N-terminal matching options from 
				     acd.                                    */
    ajint         ntermi=0;        /* N-terminal option as int. */

    AjPFile      hitsf =NULL;     /* Hits output file.                       
				     sequence matches.                       */
    AjPDir       hitsdir=NULL;    /* Directory of hits files (output).       */

    AjPFile      alignf =NULL;    /* Alignment output file.                  */
    AjPDir       aligndir=NULL;   /* Directory of alignment files (output).  */

    
    AjPDir     resultsdir=NULL;   /* Directory of results files (output).  */
    AjPFile    resultsf =NULL;   /* Results file (output).  */

    AjBool     domax = ajFalse;
    ajint      maxhits = 0;

    AjPStr *mode         = NULL;  /* Mode, 1: Patch score mode, 
				     2: Site score mode.                       */
    ajint   modei        = 0;     /* Selected mode as integer.                  */



    

    ajNamInit("emboss");
    ajAcdInitP("sigscanlig", argc, argv, "DOMAINATRIX");
    

    /* GET VALUES FROM ACD */
    sigin      = ajAcdGetDirlist("siginfiles");
    database   = ajAcdGetSeqall("dbsequences");
    sub        = ajAcdGetMatrixf("sub");
    gapo       = ajAcdGetFloat("gapo");
    gape       = ajAcdGetFloat("gape");
    nhits      = ajAcdGetInt("nhits");
    nterm      = ajAcdGetList("nterm");
    hitsdir    = ajAcdGetDirectory("hitsdir");
    aligndir   = ajAcdGetDirectory("aligndir"); 
    resultsdir = ajAcdGetDirectory("resultsdir"); 
    domax      = ajAcdGetToggle("domax");
    maxhits    = ajAcdGetInt("maxhits");
    mode        = ajAcdGetList("mode");



    /*Assign N-terminal matching option etc. */
    ajFmtScanS(nterm[0], "%d", &ntermi);
    modei       = (ajint) ajStrChar(*mode,0)-48;



    /* READ & PROCESS SIGNATURES */
    siglist = ajListNew();
    while(ajListPop(sigin, (void **) &signame))
    {
	/* Read signature files, compile signatures and populate list. */
	sigok = ajFalse;
	if((sigf = ajFileNewIn(signame)))
	    if((sig = embSignatureReadNew(sigf)))
		if(embSignatureCompile(&sig, gapo, gape, sub))
		{
		    sigok=ajTrue;
		    ajListPushApp(siglist, sig);
		    /*
		    ajFmtPrint("Id: %S\nDomid: %S\nLigid: %S\nns: %d\nsn: %d\nnp: %d\npn: %d\nminpatch: %d\nmaxgap: %d\n", 
			       sig->Id, sig->Domid, sig->Ligid, sig->ns, sig->sn, sig->np, sig->pn, sig->minpatch, sig->maxgap); */
		    

		}
	if(!sigok)
	{
	    ajWarn("Could not process %S", signame);
	    embSignatureDel(&sig);
	    ajFileClose(&sigf);
	    ajStrDel(&signame);
	    continue;
	}

	ajFileClose(&sigf);
	ajStrDel(&signame);
    }
    ajListDel(&sigin);

    
    
    /* ALIGN EACH QUERY SEQUENCE TO LIST OF SIGNATURE */
    while(ajSeqallNext(database, &seq))
    {
	/* Do sequence-signature alignment and save results */
	hits = ajListNew();
	sigiter = ajListIter(siglist);
	
	while((sig = (AjPSignature) ajListIterNext(sigiter)))
	{
	    if(embSignatureAlignSeq(sig, seq, &hit, ntermi))
	    {
		hit->Sig = sig;
		
		ajListPushApp(hits, hit);
		hit=NULL; /* To force reallocation by embSignatureAlignSeq */
	    }
	}
	
	ajListIterFree(&sigiter);
	

	/* Rank-order the list of hits by score */
	ajListSort(hits, embMatchinvScore);

	
	/* Write ligand hits & alignment files (output)  */	
	hitsf    = ajFileNewOutDir(hitsdir, ajSeqGetName(seq));
	alignf   = ajFileNewOutDir(aligndir, ajSeqGetName(seq));
	resultsf = ajFileNewOutDir(resultsdir, ajSeqGetName(seq));
	

	
	if((!sigscanlig_WriteFasta(hitsf, siglist, hits)))
	    ajFatal("Bad args to sigscanlig_WriteFasta");


    	if((!sigscanlig_SignatureAlignWriteBlock(alignf, siglist, hits)))
	    ajFatal("Bad args to sigscanlig_SignatureAlignWriteBlock");


	/* Sort list of hits by ligand type.  Process list of ligands and print out. */
	ajListSort2(hits, embMatchLigid, embMatchSN);

	if(modei==1)
	    ligands = sigscanlig_score_ligands_patch(hits, domax, maxhits);
	else if(modei==2)
	    ligands = sigscanlig_score_ligands_site(hits, domax, maxhits);
	else 
	    ajFatal("Unrecognised mode");
	

	sigscanlig_WriteResults(ligands, resultsf);	
	

    	ajFileClose(&hitsf);
	ajFileClose(&alignf);
	ajFileClose(&resultsf);


	/* Memory management */
	while(ajListPop(hits, (void **) &hit))
	    embHitDel(&hit);
	ajListDel(&hits);
    }	
    

    /* MEMORY MANAGEMENT */
    while(ajListPop(siglist, (void **) &sig))
	embSignatureDel(&sig);
    ajListDel(&siglist);
    ajSeqallDel(&database);
    ajMatrixfDel(&sub);
	
    AJFREE(nterm);    
    ajDirDel(&hitsdir);
    ajDirDel(&aligndir);
    ajDirDel(&resultsdir);
    AJFREE(mode);


    ajExit();
    return 0;    
}




/* @funcstatic sigscanlig_SignatureAlignWriteBlock *******************************
**
** Writes the alignments of a Signature to a list of AjOHit objects to an output 
** file. This is intended for displaying the results from scans of a signature 
** against a protein sequence database. The full sequence / alignment is 
** printed out for each sequence in its own block.
**
** @param [w] outf     [AjPFile] Output file stream
** @param [r] siglist  [AjPList] List of signature objects
** @param [r] hits     [AjPList] List of hits (Hit objects) from scan
**
** @return [AjBool] True if file was written
** @@
******************************************************************************/

static AjBool sigscanlig_SignatureAlignWriteBlock(AjPFile outf,
						  AjPList siglist, 
						  AjPList hits)
{
    /*
    ** A line of the alignment (including accession number, a space and the 
    ** sequence) in the output file is 70 characters long. An index number is 
    ** also printed after this 70 character field.
    */
    ajint  wid1  = 0;    /*Temp. width of Accession Number */
    ajint  mwid1 = 0;    /*
			 ** Max. width of Accession Number or the string
			 ** "Number". 
			 ** This is the field width the accession numbers
			 ** will be  printed into
			 */
    ajint  mwid2 = 0;    /* Width of region to print sequence into */
    ajint  len   = 0;    /* Temp. length of sequence */
    ajint  mlen  = 0;    /* Max. length of sequence */
    const char   *ptrp = NULL; /* Pointer to sequence string */ 
    const char   *ptrs = NULL; /* Pointer to alignment string */ 
    ajint  idx   = 0;    /* Start position for printing */
    ajint  niter = 0;    /*
			 ** No. iterations of loop for printing out
			 ** sequence blocks
			 */
    ajint  fwid1 = 70;   /*
			 ** Including accession number, a space, 7 characters 
			 ** for the first index number, and the sequence
			 */
    ajint  fwid2 = 7;    /* Field width for the first index number */
    ajint  num   = 0;    /* Index number for alignment */
    ajint  y     = 0;    /* Loop counter */

    AjIList iter    = NULL;	
    AjIList itersig = NULL;
    AjPHit hit = NULL;
    ajint hitcnt=0;      /* Counter for current hit */
    AjPStr label = NULL;
    


    /* Check args */
    if(!outf || !hits || !siglist)
	return ajFalse;


    /* Cycle through hits to find longest width of accession number. */
    len=0;
    mlen=0;	
    wid1=0;
    mwid1=0;
    label = ajStrNew();
    
	
    iter = ajListIter(hits);
    while((hit = (AjPHit) ajListIterNext(iter)))
    {
	if((wid1=MAJSTRLEN(hit->Sig->Ligid))>mwid1)
	    mwid1 = wid1; 
	if((len=MAJSTRLEN(hit->Seq))>mlen)
	    mlen = len;
    }
    ajListIterFree(&iter);
    ajListIterFree(&itersig);



    /* Assign field widths and number of iterations for printing. */
    if((wid1 = strlen("SIGNATURE"))>mwid1)
	mwid1 = wid1;
    mwid1++;   /* A space */
    mwid1 += 8;  /* 2 underscores + 2 integers in 3 char spacing each. */
    mwid2 = fwid1-fwid2-mwid1;
    niter = (ajint)ceil( ((double)mlen/(double)mwid2));
    

    /* Print header info and SCOP classification records of signature */
    ajFmtPrintF(outf, "# DE   Alignment of query sequence against library of signatures\n");

    
    /* Main loop for printing alignment. */
    iter = ajListIter(hits);
    while((hit = (AjPHit) ajListIterNext(iter)))
    {
	/* Get pointer to sequence & alignment string. */
	ptrp = ajStrStr(hit->Seq);
	ptrs = ajStrStr(hit->Alg);

	/* Print spacer */
	ajFmtPrintF(outf, "# XX\n");

	ajFmtPrintF(outf, "# ");
	if((!sigscanlig_WriteFastaHit(outf, siglist, hits, hitcnt, ajFalse)))
	    ajFatal("Bad args to sigscanlig_WriteFasta");
	ajFmtPrintF(outf, "\n# XX\n");

	/* Loop for each protein in Hitlist. */
	for(num=0, idx=0, y=0;y<niter;y++)
	{
	    num+=mwid2;


	    ajFmtPrintS(&label, "%S_%d_%d", hit->Sig->Ligid, hit->Sig->sn, hit->Sig->pn);
	    

	    /* There is some of the sequence left to print. */
	    if(idx<MAJSTRLEN(hit->Seq))
	    {
		ajFmtPrintF(outf,"%-*S%-*d%-*.*s %d\n", 
			    mwid1, hit->Acc, fwid2, 
			    (num-mwid2+1), mwid2, mwid2, ptrp+idx, num);
		ajFmtPrintF(outf,"%-*S%-*c%-*.*s\n", 
			    mwid1, label, fwid2, '-', mwid2, 
			    mwid2, ptrs+idx);
	    }	
	    /* Printed all the sequence already. */
	    else
	    {
		ajFmtPrintF(outf,"%-*S%-*d%-*.*s %d\n", 
			    mwid1, hit->Acc, fwid2,  
			    (num-mwid2+1), mwid2, mwid2, ".", num);
		ajFmtPrintF(outf,"%-*S%-*c%-*.*s\n", 
			    mwid1, label, fwid2, '-', mwid2, 
			    mwid2, "." );
	    }
	    idx += mwid2;
	}

	hitcnt++;
    }	 
    ajListIterFree(&iter);
    ajListIterFree(&itersig);
    ajStrDel(&label);
    

    return ajTrue;
}












/* @funcstatic sigscanlig_WriteFasta ************************************************
**
** Write a list of Hit objects to an output file in LHF (ligand hits file) format
** (see documentation for the DOMAINATRIX "sigscanlig" application).  The list of 
** Hit objects corresponds to a search of signatures against a sequence database.
** There is one hit per search and the list of correponding signatures must be 
** provided. 
** 
** @param [u] outf [AjPFile] Output file stream
** @param [r] hits [const AjPList] List of hit objects.
** @param [r] siglist [const AjPList] List of signnature objects.
**
** @return [AjBool] True on success
** @@
******************************************************************************/

AjBool sigscanlig_WriteFasta(AjPFile outf, AjPList siglist, AjPList hits)
{
    ajint x = 0;
    
    AjPHit hit       = NULL;
    AjPSignature sig = NULL;
    
    AjPSignature *sigarr = NULL;
    AjPHit *hitarr = NULL;
    ajint  sizarr=0;
    
        
    if(!outf || !siglist || !hits)
	return ajFalse;

    
    sizarr = ajListToArray(siglist, (void ***) &sigarr);
    if(sizarr != ajListToArray(hits, (void ***) &hitarr))
	ajFatal("Arrays are different sizes");
        

    for(x=0; x<sizarr; x++)
    {
	hit = hitarr[x];
	sig = sigarr[x];
	
	ajFmtPrintF(outf, "> ");
	
	if(MAJSTRLEN(hit->Acc))
	    ajFmtPrintF(outf, "%S^", hit->Acc);
	else
	    ajFmtPrintF(outf, ".^");

	if(MAJSTRLEN(hit->Spr))
	    ajFmtPrintF(outf, "%S^", hit->Spr);
	else
	    ajFmtPrintF(outf, ".^");

	ajFmtPrintF(outf, "%d^%d^", hit->Start, hit->End);
	
	ajFmtPrintF(outf, "LIGAND^");
	
	if(MAJSTRLEN(sig->Id))
	    ajFmtPrintF(outf, "%S^", sig->Id);
	else
	    ajFmtPrintF(outf, ".^");

	if(MAJSTRLEN(sig->Domid))
	    ajFmtPrintF(outf, "%S^", sig->Domid);
	else
	    ajFmtPrintF(outf, ".^");

	if(MAJSTRLEN(sig->Ligid))
	    ajFmtPrintF(outf, "%S^", sig->Ligid);
	else
	    ajFmtPrintF(outf, ".^");

	ajFmtPrintF(outf,"%d^", sig->sn);
	ajFmtPrintF(outf,"%d^", sig->ns);
	ajFmtPrintF(outf,"%d^", sig->pn);
	ajFmtPrintF(outf,"%d^", sig->np);
	
	if(sig->Typesig == aj1D)
	    ajFmtPrintF(outf,"1D");
	else if(sig->Typesig == aj3D)
	    ajFmtPrintF(outf,"3D");
	else
	    ajFatal("Signature type unknown in sigscanlig_WriteFasta");
	
	if(sig->np)
	    ajFmtPrintF(outf,"P^");
	else
	    ajFmtPrintF(outf,"F^");

	ajFmtPrintF(outf, "%.2f^", hit->Score);

	ajFmtPrintF(outf, "%.3e^", hit->Pval);

	ajFmtPrintF(outf, "%.3e", hit->Eval);

	ajFmtPrintF(outf, "\n");
	ajFmtPrintF(outf, "%S\n", hit->Seq);
    }
    
    AJFREE(sigarr);
    AJFREE(hitarr);
    
    
    return ajTrue;
}






/* @funcstatic sigscanlig_LighitNew *************************************************
**
** Constructor for Lighit object. 
** 
** @param [r] ligid [AjPStr] Ligand ID. 
** @param [r] ns    [ajint]  List of signnature objects.
** @param [r] np    [ajint]  Number of hit to generate.
** @param [r] score [float]  True if sequence is to be printed. 
**
** @return [AjBool] True on success
** @@
******************************************************************************/
AjPLighit sigscanlig_LighitNew(void)
{
    AjPLighit ret = NULL;
    
    AJNEW0(ret);
    
    ret->ligid = ajStrNew();
    
    return ret;
}







/* @funcstatic sigscanlig_LigHitDel *************************************************
**
** Destructor for Lighit object. 
** 
** @param [r] obj [AjPLighit *]  
**
** @return [AjBool] True on success
** @@
******************************************************************************/
void sigscanlig_LigHitDel(AjPLighit *obj)
{
    if((!obj) || (!(*obj)))
	return;

    ajStrDel(&(*obj)->ligid);
    
    AJFREE(*obj);
    *obj = NULL;
    
    return;
}







/* @funcstatic sigscanlig_WriteFastaHit *********************************************
**
** Write a Hit from a Hitlist object to an output file in embl-like format
** (see documentation for the DOMAINATRIX "seqsearch" application).
** Text for Class, Fold, Superfamily and Family is only written if the text
** is available.
** 
** @param [u] outf [AjPFile] Output file stream
** @param [r] hits [const AjPList] List of hit objects.
** @param [r] siglist [const AjPList] List of signnature objects.
** @param [r] n  [ajint] Number of hit to generate.
** @param [r] DOSEQ  [AjBool] True if sequence is to be printed. 
**
** @return [AjBool] True on success
** @@
******************************************************************************/

AjBool sigscanlig_WriteFastaHit(AjPFile outf, AjPList siglist, AjPList hits, ajint n, AjBool DOSEQ)
{
    AjPHit hit       = NULL;
    AjPSignature sig = NULL;
    
    AjPSignature *sigarr = NULL;
    AjPHit *hitarr = NULL;
    ajint  sizarr=0;
    
        
    if(!outf || !siglist || !hits)
	return ajFalse;

    
    sizarr = ajListToArray(siglist, (void ***) &sigarr);
    if(sizarr != ajListToArray(hits, (void ***) &hitarr))
	ajFatal("Arrays are different sizes");
        
    if(n>=sizarr)
	ajFatal("Requested hit out of range in sigscanlig_WriteFastaHit");
    
    
    
    hit = hitarr[n];
    sig = sigarr[n];
    
    ajFmtPrintF(outf, "> ");
    
    if(MAJSTRLEN(hit->Acc))
	ajFmtPrintF(outf, "%S^", hit->Acc);
    else
	ajFmtPrintF(outf, ".^");
    
    if(MAJSTRLEN(hit->Spr))
	ajFmtPrintF(outf, "%S^", hit->Spr);
    else
	ajFmtPrintF(outf, ".^");
    
    ajFmtPrintF(outf, "%d^%d^", hit->Start, hit->End);
    
    ajFmtPrintF(outf, "LIGAND^");
    
    if(MAJSTRLEN(sig->Id))
	ajFmtPrintF(outf, "%S^", sig->Id);
    else
	ajFmtPrintF(outf, ".^");
    
    if(MAJSTRLEN(sig->Domid))
	ajFmtPrintF(outf, "%S^", sig->Domid);
    else
	ajFmtPrintF(outf, ".^");
    
    if(MAJSTRLEN(sig->Ligid))
	ajFmtPrintF(outf, "%S^", sig->Ligid);
    else
	ajFmtPrintF(outf, ".^");
    
    ajFmtPrintF(outf,"%d^", sig->sn);
    ajFmtPrintF(outf,"%d^", sig->ns);
    ajFmtPrintF(outf,"%d^", sig->pn);
    ajFmtPrintF(outf,"%d^", sig->np);
    
    if(sig->Typesig == aj1D)
	ajFmtPrintF(outf,"1D");
    else if(sig->Typesig == aj3D)
	ajFmtPrintF(outf,"3D");
    else
	ajFatal("Signature type unknown in sigscanlig_WriteFasta");
    
    if(sig->np)
	ajFmtPrintF(outf,"P^");
    else
	ajFmtPrintF(outf,"F^");
    
    ajFmtPrintF(outf, "%.2f^", hit->Score);
    
    ajFmtPrintF(outf, "%.3e^", hit->Pval);
    
    ajFmtPrintF(outf, "%.3e", hit->Eval);
    
    if(DOSEQ)
    {
	ajFmtPrintF(outf, "\n");
	ajFmtPrintF(outf, "%S\n", hit->Seq);
    }
    
    AJFREE(sigarr);
    AJFREE(hitarr);
    
    
    return ajTrue;
}




/* @funcstatic sigscanlig_score_ligands_patch *********************************
**
** Write a Hit from a Hitlist object to an output file in embl-like format
** (see documentation for the DOMAINATRIX "seqsearch" application).
** Text for Class, Fold, Superfamily and Family is only written if the text
** is available.
** 
** @param [r] hits    [const AjPList] List of hit objects.
** @param [r] MAXHITS [AjBool] Whether a max. number of top-scoring hits applies.
** @param [r] maxhits [ajint] Max. number of top-scoring hits that are considered
** for each ligand.
** @return [AjBool] True on success
** @@
******************************************************************************/

AjPList sigscanlig_score_ligands_patch(AjPList hits, AjBool DOMAX, ajint maxhits)
{ 
    AjPList ret         = NULL;
    AjIList iter        = NULL;   /* Iterator. */
    AjPHit  hit         = NULL;   
    AjPStr  prev_ligand = NULL;
    AjPLighit lighit    = NULL;
    float  score        = 0.0;
    ajint  hit_cnt      = 0;      /* No. of hits for the current ligand. */
    AjPInt snarr        = NULL;
    ajint  snarr_siz    = 0;
    ajint  x = 0;
    AjBool ok = ajTrue;
    

    

    ret = ajListNew();
    prev_ligand = ajStrNew();
    snarr=ajIntNew();
  
    iter = ajListIter(hits);

    while((hit = (AjPHit) ajListIterNext(iter)))
    {
	if((!ajStrMatch(hit->Sig->Ligid, prev_ligand)) || 
	   (DOMAX && ((hit_cnt+1) == maxhits)))
	{
	    if(hit_cnt)
		score /= hit_cnt;
	    
	    if(lighit)
	    {
		lighit->ns = snarr_siz;
		lighit->score =  score;
		ajStrAssS(&lighit->ligid, prev_ligand);
		ajListPushApp(ret, lighit);

		ajIntDel(&snarr);
		snarr=ajIntNew();
		snarr_siz = 0;
	    }
	    
	    
	    /* Disregard remaining hits for this ligand */
	    if( DOMAX && ((hit_cnt+1) == maxhits))
		while((hit = (AjPHit) ajListIterNext(iter)))
		    if((!ajStrMatch(hit->Sig->Ligid, prev_ligand)))
			break;

	    lighit = sigscanlig_LighitNew();

	    hit_cnt=0;
	    score = 0;
	}
	
	for(ok=ajTrue, x=0; x<snarr_siz; x++)
	    if(hit->Sig->sn == ajIntGet(snarr, x))
	    {
		ok=ajFalse;
		break;
	    }
	if(ok)
	{
	    ajIntPut(&snarr, snarr_siz, hit->Sig->sn);
	    snarr_siz++;
	}
	
	

	hit_cnt++;
	score+= hit->Score;
	ajStrAssS(&prev_ligand, hit->Sig->Ligid);
    }
    
    /* Process last hit */
    if(hit_cnt)
	score /= hit_cnt;
	    
    if(lighit)
    {
	lighit->ns = snarr_siz;
	lighit->score = score;
	ajStrAssS(&lighit->ligid, prev_ligand);
	ajListPushApp(ret, lighit);
    }
	

    ajListSort(ret, sigscanlig_MatchinvScore);
    

    ajListIterFree(&iter);
    ajStrDel(&prev_ligand);
    ajIntDel(&snarr);
    

    return ret;
}






/* @funcstatic sigscanlig_score_ligands_site *********************************
**
** Write a Hit from a Hitlist object to an output file in embl-like format
** (see documentation for the DOMAINATRIX "seqsearch" application).
** Text for Class, Fold, Superfamily and Family is only written if the text
** is available.
** 
** @param [r] hits    [const AjPList] List of hit objects.
** @param [r] MAXHITS [AjBool] Whether a max. number of top-scoring hits applies.
** @param [r] maxhits [ajint] Max. number of top-scoring hits that are considered
** for each ligand.
** @return [AjBool] True on success
** @@
******************************************************************************/

AjPList sigscanlig_score_ligands_site(AjPList hits, AjBool DOMAX, ajint maxhits)
{ 
    AjPList ret         = NULL;
    AjIList iter        = NULL;   /* Iterator. */
    AjPHit  hit         = NULL;   
    AjPStr  prev_ligand = NULL;
    AjPLighit lighit    = NULL;
    float  score_site   = 0.0;    /* Score for this site. */
    float  score        = 0.0;    /* Overall score */
    ajint  hit_cnt      = 0;      /* No. of hits for the current ligand. */
    ajint  patch_cnt    = 0;
    


    AjPInt snarr        = NULL;
    ajint  snarr_siz    = 0;
    ajint  x = 0;
    AjBool ok = ajTrue;
    
    ajint  prevsn      = -1;        /* Previous site number */
    ajint  site_cnt    = 0;    
    
    

    ret = ajListNew();
    prev_ligand = ajStrNew();
    snarr=ajIntNew();
  
    iter = ajListIter(hits);

    while((hit = (AjPHit) ajListIterNext(iter)))
    {
	if(hit->Sig->sn != prevsn)
	{
	    if(patch_cnt)
		score_site /= patch_cnt;
	    score += score_site;
	    patch_cnt = 0;
	    score_site = 0;
	}

	if((!ajStrMatch(hit->Sig->Ligid, prev_ligand)) || 
	   (DOMAX && ((hit_cnt+1) == maxhits)))
	{
	    if(site_cnt)
		score /= site_cnt;
	    
	    if(lighit)
	    {
		lighit->ns = snarr_siz;
		lighit->score =  score;
		ajStrAssS(&lighit->ligid, prev_ligand);
		ajListPushApp(ret, lighit);

		ajIntDel(&snarr);
		snarr=ajIntNew();
		snarr_siz = 0;
		site_cnt = 0;
		prevsn = -1;
	    }
	    
	    
	    /* Disregard remaining hits for this ligand */
	    if( DOMAX && ((hit_cnt+1) == maxhits))
		while((hit = (AjPHit) ajListIterNext(iter)))
		    if((!ajStrMatch(hit->Sig->Ligid, prev_ligand)))
			break;

	    lighit = sigscanlig_LighitNew();

	    hit_cnt=0;
	    score = 0;
	}
	
	for(ok=ajTrue, x=0; x<snarr_siz; x++)
	    if(hit->Sig->sn == ajIntGet(snarr, x))
	    {
		ok=ajFalse;
		break;
	    }
	if(ok)
	{
	    ajIntPut(&snarr, snarr_siz, hit->Sig->sn);
	    snarr_siz++;
	}
	
	if(hit->Sig->sn != prevsn)
	    site_cnt++;
	
	hit_cnt++;
	patch_cnt++;
	score_site += hit->Score;
	ajStrAssS(&prev_ligand, hit->Sig->Ligid);
	prevsn = hit->Sig->sn;
    }
    
    /* Process last hit */
    if(hit_cnt)
	score /= hit_cnt;
	    
    if(lighit)
    {
	lighit->ns = snarr_siz;
	lighit->score = score;
	ajStrAssS(&lighit->ligid, prev_ligand);
	ajListPushApp(ret, lighit);
    }
	

    ajListSort(ret, sigscanlig_MatchinvScore);
    

    ajListIterFree(&iter);
    ajStrDel(&prev_ligand);
    ajIntDel(&snarr);
    

    return ret;
}








/* @funcstatic sigscanlig_MatchinvScore ***************************************************
**
** Function to sort Lighit objects by score record. Usually called by 
** ajListSort.  The sorting order is inverted - i.e. it returns -1 if score1 
** > score2.
**
** @param [r] hit1  [const void*] Pointer to Hit object 1
** @param [r] hit2  [const void*] Pointer to Hit object 2
**
** @return [ajint] 1 if score1<score2, 0 if score1==score2, else -1.
** @@
****************************************************************************/

ajint sigscanlig_MatchinvScore(const void *hit1, const void *hit2)
{
    AjPLighit p = NULL;
    AjPLighit q = NULL;

    p = (*(AjPLighit*)hit1);
    q = (*(AjPLighit*)hit2);
    
    if(p->score > q->score)
	return -1;
    else if (p->score == q->score)
	return 0;

    return 1;
}




/* @funcstatic sigscanlig_WriteResults ****************************************
**
** Function to write SIGSCANLIG results file. 
**
** @param [r] results  [AjPList] List of Lighit objects. 
** @param [r] resultsf  [AjPFile] Results file.
**
** @return [ajint] 1 if score1<score2, 0 if score1==score2, else -1.
** @@
******************************************************************************/
void sigscanlig_WriteResults(AjPList results, AjPFile resultsf)
{
    AjIList   iter      = NULL;   /* Iterator. */
    AjPLighit lighit    = NULL;
    
    iter = ajListIter(results);

    ajFmtPrintF(resultsf, "%-10s%-10s%-10s%-10s\n", "LIGID", "PATCHES", "SITES", "SCORE");
    
    while((lighit = (AjPLighit) ajListIterNext(iter)))
	ajFmtPrintF(resultsf, "%-10S%-10d%-10d%-10.2f\n", lighit->ligid, lighit->np, lighit->ns, lighit->score);


    ajListIterFree(&iter);

    return;
}



