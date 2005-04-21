/* @source seqsearch application
**
** Generate database hits (sequences) to sequences by using PSI-BLAST.
**
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
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
** 
*******************************************************************************
** 
**  SEQSEARCH Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  
******************************************************************************/





#include "emboss.h"





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjPHitlist seqsearch_ReadPsiblastOutput(AjPScopalg scopalg,
					       AjPHitlist hit, 
					       AjPFile psif);

static AjPFile seqsearch_psialigned(AjPStr seqname, 
				    AjPScopalg *scopalg,  
				    AjPStr *psiname, 
				    ajint niter, 
				    ajint maxhits, 
				    float evalue,  
				    AjPStr database);

static AjPFile seqsearch_psisingle(AjPStr seqname, 
				   AjPHitlist *hit,
				   AjPStr *psiname, 
				   ajint niter, 
				   ajint maxhits, 
				   float evalue,
				   AjPStr database);






/* @prog seqsearch ************************************************************
**
** Generate database hits (sequences) for nodes in a DCF file (domain 
** classification file) by using PSI-BLAST.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr   *mode       = NULL;   /* 
				    ** Mode of operation from ACD: 
				    ** 1: Single sequences,
				    ** 2: Sequence sets
				    */

    ajint      modei     = 0;      /* ACD mode as int.                       */

    AjPList    inseqs    = NULL;   /* Directory of sequence files for input. */
    AjPStr     inname    = NULL;   /* Name of alignment file.                */
    AjPFile    inf       = NULL;   /* Alignment file pointer.                */
    
    AjPDir     dhfout    = NULL;   /* Directory of hits files for output.    */
    AjPStr     dhfname   = NULL;   /* Name of hits file.                     */
    AjPStr     singlet   = NULL;   /* sequence of a particular sunid.        */ 

    AjPStr     msg       = NULL;   /* Error message.                         */
    AjPStr     temp      = NULL;   /* Temp string.                           */
    AjPStr     psiname   = NULL;   /* Name of psiblast output file.          */
    AjPStr     database  = NULL;   /* Name of blast-indexed database to search. */
    
    AjPFile    families  = NULL;   /* Pointer to families file for output.   */
    AjPFile    logf      = NULL;   /* Log file pointer.                      */
    AjPFile    psif      = NULL;   /* Pointer to psiblast output file.       */
    
    ajint      maxhits   = 0;      /* Maximum number of hits reported by PSIBLAST. */          
    ajint      niter     = 0;      /* Number of PSIBLAST iterations.               */          
    float      evalue    = 0.0;    /* Threshold E-value for inclusion in family.   */
    
    AjPList    listin     = NULL;  /* A list of hitlist for eleminating the 
				      identical hits.                        */
    AjPList    listout    = NULL;  /* A list of scophit for eleminating the 
				      identical hits.                        */
    
    AjPScopalg scopalg   = NULL;   /* Scop alignment from input file.        */
    AjPHitlist scophit   = NULL;   /* Hitlist of single hit from input file 
				      (singlets).                            */
    
    AjPHitlist tmphitlist = NULL;
    AjPHitlist hitlist   = NULL;   /* Hitlist object for holding results of 
                                      PSIBLAST hits.                         */  
    
    AjIList    iter       = NULL;
    AjPScophit hit        = NULL;  /* For eleminating identical hits. */
    AjPScophit nexthit    = NULL;  /* For eleminating identical hits. */
    



    
    /* Initialise strings etc. */
    inname = ajStrNew();
    dhfname  = ajStrNew();
    msg       = ajStrNew();
    temp      = ajStrNew();
    psiname   = ajStrNew();
    singlet   = ajStrNew();
    


    /* Read data from acd. */
    ajNamInit("emboss");
    ajAcdInitP("seqsearch",argc,argv,"DOMSEARCH"); 
    mode       = ajAcdGetList("mode");
    inseqs     = ajAcdGetDirlist("inseqspath");
    dhfout     = ajAcdGetOutdir("dhfoutdir");
    maxhits    = ajAcdGetInt("maxhits");
    niter      = ajAcdGetInt("niter");
    evalue     = ajAcdGetFloat("evalue");
    logf       = ajAcdGetOutfile("logfile");
    database   = ajAcdGetString("database");    


    /* Housekeeping. */
    modei      = (ajint) ajStrChar(*mode,0)-48;

    
    
    /*Start of main application loop*/   
    while(ajListPop(inseqs,(void **)&inname))
    { 
	listin    = ajListNew();
        listout   = ajListNew();

	ajFmtPrint("PROCESSING %S\n", inname);
	ajFmtPrintF(logf, "//\n%S\n", inname);

        /* Check alignment file can be read. */
        if((inf = ajFileNewIn(inname)) == NULL)
        {
            ajFmtPrintS(&msg, "Could not open for reading %S", 
                        inname);
            ajWarn(ajStrStr(msg));
            ajFmtPrintF(logf, "WARN  Could not open for reading %S\n", 
                        inname);
	    ajListDel(&listin);
	    ajListDel(&listout);
            continue;       
        }
        else
	    ajFileClose(&inf);
	
	/* Single sequences. */
	if(modei==1) 
	    psif = seqsearch_psisingle(inname, &scophit, &psiname,
				       niter,maxhits,evalue,database);
	/* Sequence sets. */
	else if(modei==2) 
	    psif = seqsearch_psialigned(inname, &scopalg,&psiname,
					niter,maxhits,evalue,database);
	else
	    ajFatal("Unknown mode in seqsearch");
	if(!(psif))
	    ajFatal("Error creating psiblast file"); 	    




        /*  Parse the Psi-Blast output file and write a Hitlist object. It
	    is OK for scopalg to be NULL (i.e. if modei ==1) or scophit to be 
	    NULL (i.e. if modei ==2 and sometimes if modei==1). */
        tmphitlist = seqsearch_ReadPsiblastOutput(scopalg, scophit, psif);
        

        /* Delete psiblast output file*/
	ajFmtPrintS(&temp, "rm %S", psiname);
	system(ajStrStr(temp)); 
	


	/* Create hits output file name - the same name as the input file. */
	ajStrAssS(&dhfname, inname);
	ajFileDirExtnTrim(&dhfname);

	
	    
	/* No hits. */   
	if(tmphitlist->N == 0)
            ajFmtPrintF(logf, "WARN  No PSIBLAST hits therefore no output file\n", 
                        inname);
	else
        {
	    /* This list will only ever contain a single hitlist. 
	       Create a list of scophits to eleminate identical hits. 
	       Sort this list by accession number, then by start, then by end. */

	    ajListPushApp(listin,tmphitlist);
	    embDmxHitlistToScophits(listin, &listout);
	    ajListSort3(listout,ajDmxScophitCompAcc, ajDmxScophitCompStart, 
			ajDmxScophitCompEnd);
          
	    /* Eleminate identical hits. */
	    iter=ajListIterRead(listout); 
          
	    /* Get the first node in the list, only once. */
	    hit = (AjPScophit)ajListIterNext(iter);
          
	    /* Loop while we can get another hit. */
	    while((nexthit=(AjPScophit)ajListIterNext(iter)))
	    {
		/* Check if the accession numbers are the same and if the
		   the start and end are identical. */
		if(ajStrMatch(hit->Acc,nexthit->Acc)
		   && (hit->Start == nexthit->Start)
		   && (hit->End == nexthit->End))
		{
		    ajDmxScophitTarget(&hit);
		    hit = nexthit;
		}
            
		else
		    hit = nexthit;
	    }             

	    ajListIterFree(&iter);
	    iter =  NULL;
     
	    /* The end of the list been reached. 
	       Delete hits in the list that are targetted for removal. */
	    ajListGarbageCollect(listout, ajDmxScophitDelWrap, 
				 (const void *) ajDmxScophitCheckTarget);
          
	    /* Recreate the hitlist for printing. */
	    embDmxScophitsToHitlist(listout,&hitlist,&iter);
	    ajListIterFree(&iter);
	    iter =  NULL;


	    if(!(families = ajFileNewOutDir(dhfout, dhfname)))
	    {
		ajFmtPrintS(&msg, "Could not open for writing %S", 
			    dhfname);
		ajWarn(ajStrStr(msg));
		ajFmtPrintF(logf, "WARN  Could not open for writing %S\n", 
			    dhfname);
		embHitlistDel(&hitlist);
		if(modei==2) 
		    ajDmxScopalgDel(&scopalg);  
		if(scophit)
		    embHitlistDel(&scophit);
		ajFileClose(&psif); 

		/* Free listin. */
		iter=ajListIterRead(listin); 
		while((tmphitlist=(AjPHitlist)ajListIterNext(iter)))
		    embHitlistDel(&tmphitlist);
		ajListDel(&listin);
		ajListIterFree(&iter);
		
		/* Free listout. */
		iter=ajListIterRead(listout); 
		while((hit=(AjPScophit)ajListIterNext(iter)))
		    ajDmxScophitDel(&hit);
		ajListDel(&listout);
		ajListIterFree(&iter);

		continue;       
	    }      
        
	    /* Write the Hitlist object to file. */         
	    embHitlistWriteFasta(families, hitlist);

	    /* Close families file. */
	    ajFileClose(&families);

	    /* Free listin. */
	    iter=ajListIterRead(listin); 
	    while((tmphitlist=(AjPHitlist)ajListIterNext(iter)))
		embHitlistDel(&tmphitlist);
	    ajListDel(&listin);
	    ajListIterFree(&iter);
          
	    /* Free listout. */
	    iter=ajListIterRead(listout); 
	    while((hit=(AjPScophit)ajListIterNext(iter)))
		ajDmxScophitDel(&hit);
	    ajListDel(&listout);
	    ajListIterFree(&iter);

	    /* Free memory etc*/
	    embHitlistDel(&hitlist);
	    if(modei==2)
		ajDmxScopalgDel(&scopalg);
	    if(scophit)
		embHitlistDel(&scophit);

	    ajFileClose(&psif);     
	    ajStrDel(&inname);
	}
    }

    
    /*Tidy up and return. */
    ajStrDel(&mode[0]);
    AJFREE(mode);
    ajListDel(&inseqs);
    ajDirDel(&dhfout);
    ajStrDel(&dhfname);
    ajStrDel(&psiname);
    ajStrDel(&database);
    ajStrDel(&msg);
    ajStrDel(&temp);
    ajStrDel(&singlet);
    ajFileClose(&logf);
   


    ajExit();
    return 0;
}








/* @funcstatic seqsearch_psialigned *******************************************
**
** Reads a Scopalg object and the corresponding alignment file. Calls psiblast
** to do a search for the SCOP family over a specified database. The psiblast 
** output file is created and a pointer to it provided.
**
** @param [r] seqname,   [AjPStr]      Name of file with sequence set
** @param [w] scopalg    [AjPScopalg*] Domain alignment object
** @param [r] psiname    [AjPStr *]    Name of psiblast output file (output)
** @param [r] niter      [ajint]       No. psiblast iterations
** @param [r] maxhits    [ajint]       Maximum number of hits to generate
** @param [r] evalue     [float]       Threshold E-value for psiblast
** @param [r] database   [AjPStr]      Database name
**
** @return [AjPFile] Pointer to  psiblast output file for reading or NULL for error.
** @@
** 
** Note
** When the library code function ScopalgWrite is written, we will no longer
** need to pass in a pointer to the alignment file itself. Write ScopalgWrite
** and modify this function accordingly - not urgent.
**
******************************************************************************/
static AjPFile seqsearch_psialigned(AjPStr seqname,
				    AjPScopalg *scopalg,  
				    AjPStr *psiname, 
				    ajint niter, 
				    ajint maxhits, 
				    float evalue,  
				    AjPStr database)
{
    AjPStr    line    = NULL;  /* Temp string for reading alignment file.    */
    AjPStr    name    = NULL;  /* Base name of STAMP temp files              */
    AjPStr    temp    = NULL;  /* Temp. string                               */
    AjPStr    seq_in  = NULL;  /* Name of temp. file for PSIBLAST input file 
				  (single sequence in FASTA format from 
				  alignment.                                 */
    AjPStr    seqs_in = NULL;  /* Name of temp. file for PSIBLAST input file 
				  (sequences alignment w/o scop records, 
				  'Post_similar' or 'Number' lines.          */

    AjPFile   seqsinf = NULL;  /* File pointer for  PSIBLAST input file 
				  (multiple sequences).                      */
    AjPFile   seqinf  = NULL;  /* File pointer for  PSIBLAST input file 
				  (single sequence).                         */
    AjPFile   psif    = NULL;  /* Pointer to psiblast output file.           */
    
    ajint     nseqs   = 0;     /* No. of sequences in alignment.             */
    ajint     x       = 0;     /* Loop counter.                              */
    
    AjPSeqset seqset  = NULL;
    AjPSeqin  seqin   = NULL;
    AjPFile   alignf  = NULL;
    AjPStr    degap   = NULL;



    /* Allocate strings. */
    line      = ajStrNew();
    name      = ajStrNew();
    temp      = ajStrNew();
    seq_in    = ajStrNew();
    seqs_in   = ajStrNew();
    degap     = ajStrNew();
    


    /* Open alignment file. */
    alignf = ajFileNewIn(seqname);
    
    /* Read domain alignment if appropriate. */
    if(!(ajDmxScopalgRead(alignf, scopalg)))
    {
	ajWarn("ajDmxScopalgRead call failed in seqsearch_psialigned");
	
	/* Read sequence set instead. */ 
	seqset = ajSeqsetNew();
	seqin  = ajSeqinNew();
	/* Set the filename via the USA. ajSeqsetRead interprets it to find 
	   the filename.. */
	ajSeqinUsa(&seqin, seqname);
	
        if(!(ajSeqsetRead(seqset, seqin)))
	    ajFatal("SeqsetRead failed in seqsearch_psialigned");
    }
    


    /* Initialise random number generator for naming of temp. files
       and create  psiblast input files. */
    ajRandomSeed();
    ajStrAssC(&name, ajFileTempName(NULL));
    ajStrAss(&seqs_in, name);
    ajStrAppC(&seqs_in, ".seqsin");
    ajStrAss(&seq_in, name);
    ajStrAppC(&seq_in, ".seqin");
    ajStrAss(psiname, name);
    ajStrAppC(psiname, ".psiout");


    seqsinf = ajFileNewOut(seqs_in);
    seqinf  = ajFileNewOut(seq_in);
    


    /* Write psiblast multiple sequence input file. */
    if((*scopalg))
	nseqs = (*scopalg)->N;
    else
	nseqs = ajSeqsetSize(seqset);

    ajFmtPrintF(seqsinf,"\n");
    for(x=0;x<nseqs;x++)
	if((*scopalg))
	    ajFmtPrintF(seqsinf,"%S %S\n", (*scopalg)->Codes[x], 
			(*scopalg)->Seqs[x]);
	else
	    ajFmtPrintF(seqsinf,"%S %s\n", ajSeqsetName(seqset, x), 
			ajSeqsetSeq(seqset, x));
    

    
    /* Write psiblast single sequence input file. */    
    if((*scopalg))
    {
	ajStrAssS(&degap, (*scopalg)->Seqs[0]);
	ajStrDegap(&degap);
	ajFmtPrintF(seqinf,"> %S\n%S\n", (*scopalg)->Codes[0], degap);
    }
    
    else
    {
	ajStrAssC(&degap, ajSeqsetSeq(seqset, 0));
	ajStrDegap(&degap);
	ajFmtPrintF(seqinf,"> %S\n%s\n", ajSeqsetName(seqset, 0), degap);
    }
    
    

    /* Close psiblast input files before psiblast opens them. */
    ajFileClose(&seqinf);
    ajFileClose(&seqsinf);

    
    /* Run PSI-BLAST. */
    ajFmtPrintS(&temp,"blastpgp -i %S -B %S -j %d -e %f -b %d -v %d -d %S > %S\n",
                seq_in, seqs_in, niter,evalue, maxhits, maxhits, database, *psiname);
    ajFmtPrint("%S\n", temp);
    system(ajStrStr(temp));
    

    /* Remove temp. files. */
    ajFmtPrintS(&temp, "rm %S", seq_in);
    system(ajStrStr(temp));
    ajFmtPrintS(&temp, "rm %S", seqs_in);
    system(ajStrStr(temp)); 


    /* Tidy up. */
    ajStrDel(&line);
    ajStrDel(&name);
    ajStrDel(&temp);
    ajStrDel(&seq_in);   
    ajStrDel(&seqs_in);
    ajStrDel(&degap);
    if(seqset)
	ajSeqsetDel(&seqset);
    if(seqin)
	ajSeqinDel(&seqin);
    ajFileClose(&alignf);
    

    /* Open psiblast output file and return pointer. */
    psif = ajFileNewIn(*psiname); 

    return psif;
}





/* @funcstatic seqsearch_psisingle*********************************************
**
** Calls psiblast to do a search for the SCOP family over a specified 
** database. The psiblast output file is created and a pointer to it provided.
**
** @param [r] seqname    [AjPStr]      Full name of sequence file 
** @param [r] hit        [AjPHitlist]  Hitlist of single hit
** @param [r] psiname    [AjPStr *]    Name of psiblast output file created
** @param [r] niter      [ajint]       No. psiblast iterations
** @param [r] maxhits    [ajint]       Maximum number of hits to generate
** @param [r] evalue     [float]       Threshold E-value for psiblast
** @param [r] database   [AjPStr]      Database name
**
** @return [AjPFile] Pointer to  psiblast output file for reading or NULL for 
** error.
** @@
** 
** Note
** When the library code function ScopalgWrite is written, we will no longer
** need to pass in a pointer to the alignment file itself. Write ScopalgWrite
** and modify this function accordingly - not urgent.
******************************************************************************/
static AjPFile seqsearch_psisingle(AjPStr seqname, 
				   AjPHitlist *hit, 
				   AjPStr *psiname, 
				   ajint niter, 
				   ajint maxhits, 
				   float evalue,  
				   AjPStr database)
{
    AjPStr    name      = NULL;  /* Base name of STAMP temp files.           */
    AjPStr    temp      = NULL;  /* Temp. string. */
    AjPStr    seq_in    = NULL;  /* Name of temp. file for PSIBLAST input file 
                                    (single sequence in FASTA format from 
				    alignment.                               */

    AjPFile   seqinf    = NULL;  /* File pointer for  PSIBLAST input file 
				    (single sequence).                       */
    AjPFile   psif      = NULL;  /* Pointer to psiblast output file.         */

    AjPFile   dhfin     = NULL;  /* File pointer for reading DHF input file 
				    (if provided) otherwise the sequence is 
				    read via USA.                            */
    AjPSeq    seq       = NULL;
    AjPSeqin  seqin     = NULL;


    /* Open dhf (or other format) sequence file. */
    dhfin = ajFileNewIn(seqname);

    /* Read dhf file if appropriate. */
    if(!(*hit = embHitlistReadFasta(dhfin)))
    {   
	ajWarn("embHitlistReadFasta call failed in seqsearch_psisingle");
	
	/* Read sequence instead. */ 
	/* Set the filename via USA. ajSeqRead interprets it to find the 
	   filename.. */
	seq    = ajSeqNew();
	seqin  = ajSeqinNew();
	ajSeqinUsa(&seqin, seqname);
	ajSeqRead(seq, seqin);
    }

    /* Allocate strings. */
    name      = ajStrNew();
    temp      = ajStrNew();
    seq_in    = ajStrNew();

    /* Initialise random number generator for naming of temp. files
       and create  psiblast input files. */
    ajStrAssC(&name, ajFileTempName(NULL));
    ajStrAss(&seq_in, name);
    ajStrAppC(&seq_in, ".seqin");
    ajStrAss(psiname, name);
    ajStrAppC(psiname, ".psiout");

    /* Create output file for psi-blast input file. */
    seqinf = ajFileNewOut(seq_in);
    
    /* Write psiblast input file of single sequence. */    
    if((*hit))
	ajFmtPrintF(seqinf,">\n%S\n",(*hit)->hits[0]->Seq);
    else
	ajFmtPrintF(seqinf,">\n%S\n",ajSeqStr(seq));
    
    /* Close psiblast input file before psiblast opens them. */
    ajFileClose(&seqinf);

    /* Run PSI-BLAST. */
    ajFmtPrintS(&temp,"blastpgp -i %S -j %d -e %f -b %d -v %d -d %S > %S\n",
                seq_in, niter,evalue, maxhits, maxhits, database, *psiname);
    ajFmtPrint("%S\n", temp);
    system(ajStrStr(temp));

    /* Remove temp. files. */
    ajFmtPrintS(&temp, "rm %S", seq_in);
    system(ajStrStr(temp)); 

    /* Tidy up. */
    ajFileClose(&dhfin);
    ajStrDel(&name);
    ajStrDel(&temp);
    ajStrDel(&seq_in);   
    if(seq)
	ajSeqDel(&seq);
    if(seqin)
	ajSeqinDel(&seqin);


    /* Open psiblast output file and return pointer. */
    psif = ajFileNewIn(*psiname);
    return psif;
}


/* @funcstatic seqsearch_ReadPsiblastOutput ***********************************
**
** Reads a psiblast output file and writes a Hitlist object containing the 
** hits.
**
** @param [r] scopalg   [AjPScopalg]  Alignment    
** @param [r] scophit   [AjPHitlist]  Hit 
** @param [r] psif      [AjPFile]     psiblast output file 
**
** @return [AjPHitlist] Pointer to Hitlist object (or NULL if 0 hits were found)
** @@
** 
******************************************************************************/
static AjPHitlist seqsearch_ReadPsiblastOutput(AjPScopalg scopalg, 
					       AjPHitlist scophit, 
					       AjPFile psif)
{
    /* 
     ** The hits are organised into blocks, each block contains hits to 
     ** a protein with a unique accession number.  
     ** Each hit in a block mnay be spread over multiple lines. nlines 
     ** is the number of the line (sequence fragment) we are currently on. 
     */
    
    
    AjPStr  line       = NULL;  /* Temp string. */ 
    AjPStr  acc        = NULL;  /* Acc. number of current hit*/ 
    AjPStr  prevacc    = NULL;  /* Acc. number of previous hit*/ 
    AjPStr  fragseq    = NULL;  /* Sequence fragment. */ 
    AjPStr  fullseq    = NULL;  /* Sequence of entire hit. */ 
    
    ajint   start      = 0;     /* Start of hit. */
    ajint   fragstart  = 0;     /* Start of sequence fragment. */
    ajint   fragend    = 0;     /* End of sequence fragment. */
    ajint   hitn       = 0;     /* The number of the hit we are on. */
    ajint   nhits      = 0;     /* No. of hits in alignment file. */
    ajint   fseekr     = 0;

    float   offset     = 0;
    float   eval       = 0;
    float   score      = 0;

    AjPHitlist hitlist = NULL;  /* Hitlist object for holding results 
                                   of PSIBLAST hits*/
    
    /* Allocate strings etc. */
    line      = ajStrNew();
    acc       = ajStrNew();
    prevacc   = ajStrNew();
    fragseq   = ajStrNew();
    fullseq   = ajStrNew();
    
    /* Calculate the number of hits. */
    while(ajFileReadLine(psif,&line))
        if(ajStrFindCaseC(line,"score = ")>=0)
            nhits++;
    fseekr = ajFileSeek(psif,offset,SEEK_SET);
    
    
    /* Allocate memory for Hitlist object. */
    if(!(hitlist = embHitlistNew(nhits)))
        ajFatal("Could not allocate hitlist in seqsearch_ReadPsiblastOutput");
    
    
    
    /* Copy domain classification records.
       Assign domain classification records from hitlist structure. */
    if(scopalg)
    {    
	hitlist->Type = scopalg->Type;
	ajStrAssS(&hitlist->Class, scopalg->Class);
	ajStrAssS(&hitlist->Fold, scopalg->Fold);
	ajStrAssS(&hitlist->Superfamily, scopalg->Superfamily);
	ajStrAssS(&hitlist->Family, scopalg->Family);
	hitlist->Sunid_Family = scopalg->Sunid_Family;
    }
    else if(scophit)
    {    	
	hitlist->Type = scophit->Type;
	ajStrAssS(&hitlist->Class, scophit->Class);
	ajStrAssS(&hitlist->Fold, scophit->Fold);
	ajStrAssS(&hitlist->Superfamily, scophit->Superfamily);
	ajStrAssS(&hitlist->Family, scophit->Family);
	hitlist->Sunid_Family = scophit->Sunid_Family;
    }
    /* 
     ** It is ok that these records are not written if there is no
     ** scopalg or hit object (i.e. single sequences or sequences sets 
     ** were used as input), they just will not appear in the seqsearch output 
     ** file. 
     */
    

    /* Loop for whole of input file. */
    while(ajFileReadLine(psif,&line))
    {
        /* We've found a line beginning with > i.e. the start of a block of 
	   hits to a single protein. */
        if(ajStrPrefixC(line,">SW:"))
        {
            /* Parse the accession number. */
            ajFmtScanS(line, "%*s %S", &prevacc);
        }
        /* We've found a line beginning with ' Score = ' i.e. the start of data 
	   for a hit. */
        else if(ajStrPrefixC(line," Score = "))
        {
            /* Write hit structure, we've parsed . */
            if(hitn!=0)
            {
                hitlist->hits[hitn-1]->Start   = start;
                hitlist->hits[hitn-1]->End     = fragend;
                ajStrAss(&hitlist->hits[hitn-1]->Acc, acc);
                ajStrAss(&hitlist->hits[hitn-1]->Seq, fullseq);
                ajStrDegap(&hitlist->hits[hitn-1]->Seq);
                ajStrAssC(&hitlist->hits[hitn-1]->Model, "PSIBLAST");
                hitlist->hits[hitn-1]->Score = score;
		hitlist->hits[hitn-1]->Eval  = eval;
	    }

	    ajFmtScanS(line, "%*s %*s %f %*s %*s %*s %*s %f", 
		       &score, &eval);
	    
	   
            /* Reset the sequence of the full hit. */
            ajStrAssC(&fullseq, "");

            /* Incremenet hit counter. */
            hitn++;

            /* Copy accession number. */
            ajStrAss(&acc, prevacc);

        }
        /* Line containing sequence segment of the hit. */
        else if(ajStrPrefixC(line,"Sbjct: "))
        {
            /* Parse the start, end and sequence of the fragment. */
            ajFmtScanS(line, "%*s %d %S %d", &fragstart, &fragseq, &fragend);

            /* If this is the first fragment, get the start point. */
            if(!ajStrCmpC(fullseq, ""))
                start=fragstart;
   
            /* Add fragment to end of sequence of full hit. */
            ajStrApp(&fullseq, fragseq);
        }
    }

    /* Write hit structure for last hit. */
    if(hitn!=0)
    {
        hitlist->hits[hitn-1]->Start = start;
        hitlist->hits[hitn-1]->End = fragend;
        ajStrAss(&hitlist->hits[hitn-1]->Acc, acc);
        ajStrAss(&hitlist->hits[hitn-1]->Seq, fullseq);
        ajStrDegap(&hitlist->hits[hitn-1]->Seq);
	ajStrAssC(&hitlist->hits[hitn-1]->Model, "PSIBLAST");
	hitlist->hits[hitn-1]->Score = score;
	hitlist->hits[hitn-1]->Eval  = eval;
    }

    /*Tidy up and return. */
    ajStrDel(&line);
    ajStrDel(&acc);
    ajStrDel(&prevacc);
    ajStrDel(&fragseq);
    ajStrDel(&fullseq);

    return hitlist;
}




