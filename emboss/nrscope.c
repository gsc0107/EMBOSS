/* @source nrscope application
**
** Converts redundant embl-format scop file to non-redundant one
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @author: Copyright (C) Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
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
** 
** 
** 
** 
** Operation
** 
** nrscope parses the embl-like format scop classification file generated by 
** the EMBOSS application scope, and writes in the same format a file of 
** non-redundant domains.  The format of these files is explained in the 
** scope documentation.  Domain sequences are extracted from the clean domain 
** coordinate files generated by the EMBOSS application domainer.  

** The current version of nrscope removes redundancy at the level of the scop 
** family, i.e. entries belonging to the same family will be non-redundant.  
** All permutations of pair-wise sequence alignments are calculated for each 
** scop family in turn using the EMBOSS implementation of the Needleman and 
** Wunsch global alignment algorithm.  If a pair of proteins achieve greater 
** than a threshold percentage sequence similarity (specified by the user) the 
** shortest sequence is discarded.  The user must specify gap insertion and 
** extension penalties and a residue substitution matrix for use in the 
** alignments.  % sequence similarity is calculated by using the EMBOSS 
** function embAlignCalcSimilarity.
**
** nrscope generates a log file an excerpt of which is shown (Figure 1). The 
** first two lines describe the level in the scop hierarchy at which redundancy 
** was removed (always 'FAMILIES' for the current implementation) and the value
** of the redundancy threshold.  The file then contains a section for each scop 
** family. Each section contains a line with the record '//' immediately 
** followed by the name of the scop family, and two lines containing 'Retained' 
** and 'Rejected' respectively. Domain identifier codes of domains that were 
** discarded by nrscope are listed under 'Rejected' while those that appear in 
** the output file are listed under 'Retained'.  The text 'WARN  filename not 
** found' is given in cases where a clean domain coordinate file could not be 
** found and 'WARN  Empty family' where no files for an entire family could be 
** found.  'ERROR filename file read error' will be given when an error was 
** encountered during a file read.
**
** Figure 1.  Excerpt from nrscope log file
** FAMILIES are non-redundant
** 95% redundancy threshold
** // Homeodomain
** Retained
** D2HDDA_
** D1AKHA_
** D1MNMC_
** Rejected
** D2HDDB_
** D1ENH__
** D3HDDA_
** WARN  d3hdda_.pxyz not found
** // Di-haem cytohrome c peroxidase
** WARN  ds005__.pxyz not found
** WARN  Empty family
** // Nuclear receptor coactivator Src-1
** Retained
** D2PRGC_
** Rejected
******************************************************************************/






#include "emboss.h"




/* @prog nrscope **************************************************************
**
** Converts redundant EMBL-format SCOP file to non-redundant one
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint        x         = 0;
    ajint        nset      = 0;	 /*No. proteins in our  set*/
    ajint        nsetnr    = 0;	 /*No. proteins in the non-redundant set */
    ajint        nfam      = 0;	 /* Counter of SCOP families */

    float      gapopen;		 /* Gap insertion penalty*/
    float      gapextend;	 /* Gap extension penalty*/
    float      thresh;		 /* Threshold for non-redundancy*/

    AjPStr    last_fam   = NULL; /* Last family that was processed*/
    AjPStr    msg        = NULL; /* String used for messages */
    AjPStr    dpdb_path  = NULL; /* Path of dpdb (clean domain) files */
    AjPStr    dpdb_name  = NULL; /* Name of dpdb (clean domain) file */
    AjPStr    dpdb_extn  = NULL; /* Extension of dpdb (clean domain) file */

    AjPFile   dpdb_inf   = NULL; /* File pointer for clean domain files */
    AjPFile   scop_inf   = NULL; /* File pointer for original scop file */
    AjPFile   scop_outf  = NULL; /* File pointer for scop output file */
    AjPFile   errf       = NULL;

    AjPList    list_seqs = NULL; /* List of dpdb sequences for a family*/
    AjPList    list_scop = NULL; /* List of scop structures for a family*/
    AjIList    iter      = NULL; /* Iterator for scop list */    
    AjPInt     keep      = NULL; /*1: This sequence was kept after */
                                 /*redundancy removal, 0: it was discarded*/
    AjPSeq     seq       = NULL; /* A temporary pointer*/
    AjPMatrixf matrix;		 /* Substitution matrix*/
    AjPScop   scop       = NULL; /* Pointer to scop structure */
    AjPScop   scop_tmp   = NULL; /* Pointer to scop structure */
    AjPPdb    pdb        = NULL; /* Pointer to pdb structure */

    



    
    /* Initialise strings etc */
    msg       = ajStrNew();
    dpdb_path = ajStrNew();
    dpdb_name = ajStrNew();
    dpdb_extn = ajStrNew();
    last_fam  = ajStrNew();
    keep = ajIntNew();  	    


    /* Initialise last_fam with a value that is not in SCOP */
    ajStrAssC(&last_fam,"!!!!!");

    
    /* Read data from acd */
    embInit("nrscope",argc,argv);
    scop_inf  = ajAcdGetInfile("scopin");
    scop_outf = ajAcdGetOutfile("scopout");
    dpdb_path = ajAcdGetString("dpdb");
    dpdb_extn = ajAcdGetString("extn");
    thresh    = ajAcdGetFloat("thresh");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    errf      = ajAcdGetOutfile("errf");


    /* Check directory*/
    if(!ajFileDir(&dpdb_path))
	ajFatal("Could not open dpdb directory");


    /* Write header of log file */
    /* JCI Replace hard-coded 'FAMILIES' with appropriate string when */
    /* redundancy removal is implemented for any node in SCOP */
    ajFmtPrintF(errf, "FAMILIES are non-redundant\n"
		"%.0f%% redundancy threshold\n", thresh); 


    


    
    /* Start of main application loop */
    while((ajXyzScopReadC(scop_inf, "*", &scop)))
    {
	/* If we are on to a new family*/
	if(ajStrMatch(last_fam, scop->Family)==ajFalse)
	{
	    /* If we have done the first family*/
	    if(nfam)
	    {
		/* If the family does not have any members*/
		if(!nset)
		{
		    /* Write diagnostic */
        	    ajFmtPrint("WARN  Empty family\n"); 
		    ajFmtPrintF(errf, "WARN  Empty family\n");
		}
		/* Last family had members, so remove redundancy*/
		else
		{
		    /* Remove redundancy from list_seqs*/
		    if(!embXyzSeqsetNR(list_seqs, &keep, &nsetnr, matrix, gapopen, 
				 gapextend,thresh))
		    {
			ajStrDel(&last_fam);
			ajStrDel(&msg);
			ajStrDel(&dpdb_path);
			ajStrDel(&dpdb_name);
			ajStrDel(&dpdb_extn);
			ajIntDel(&keep);	
			ajMatrixfDel(&matrix);
			ajFileClose(&errf);    
			ajFileClose(&scop_outf);	    
			ajFatal("Unexpected embXyzSeqsetNR error");
		    }
		    

		    /* Write file with SCOP entries that are retained*/
		    for(iter=ajListIter(list_scop), x=0;
			(scop_tmp=(AjPScop)ajListIterNext(iter));
			x++)
			if(ajIntGet(keep,x))
			    ajXyzScopWrite(scop_outf, scop_tmp);
		    ajListIterFree(iter);	


		    /* Write diagnostic */
 		    ajFmtPrintF(errf, "Retained\n");
		    for(iter=ajListIter(list_scop), x=0;
			(scop_tmp=(AjPScop)ajListIterNext(iter));
			x++)
			if(ajIntGet(keep,x))
			    ajFmtPrintF(errf, "%S\n", scop_tmp->Entry);
		    ajListIterFree(iter);
		    ajFmtPrintF(errf, "Rejected\n");
		    for(iter=ajListIter(list_scop), x=0;
			(scop_tmp=(AjPScop)ajListIterNext(iter));
			x++)
			if(!(ajIntGet(keep,x)))
			    ajFmtPrintF(errf, "%S\n", scop_tmp->Entry);
		    ajListIterFree(iter);	

		    
		    /* Write diagnostic */
		    ajFmtPrintF(errf, "// %S\n", scop->Family);
		    ajFmtPrint("// %S\n", scop->Family);
		}
		/* Intitiliase counter for number in family*/
		nset=0;


		/* Free up the scop list and create a new one*/
		iter=ajListIter(list_scop);
		while((scop_tmp=(AjPScop)ajListIterNext(iter)))
		    ajXyzScopDel(&scop_tmp);
		ajListIterFree(iter);	
		ajListDel(&list_scop);	    


		/* Free up the seqs list and create a new one*/
		iter=ajListIter(list_seqs);
		while((seq=(AjPSeq)ajListIterNext(iter)))
		    ajSeqDel(&seq);
		ajListIterFree(iter);	
		ajListDel(&list_seqs);	    
	    }
	    else
	    {
		/* We have not done the first family*/
		/* Write diagnostic */
		ajFmtPrintF(errf, "// %S\n", scop->Family);
		ajFmtPrint("// %S\n", scop->Family);
	    }
	    
	    
	    /* Write the current family to last_fam*/
	    ajStrAss(&last_fam, scop->Family);

	    /*Create new list for scop and seqs*/
	    list_seqs = ajListNew();
	    list_scop = ajListNew();

	    
	    /* Increment counter for number of families*/
	    nfam++;
	}
	/* Write diagnostic */
	ajFmtPrint("%S\n", scop->Entry);

		
	/*Open the coordinate file for the domain*/
	ajStrAss(&dpdb_name, scop->Entry);
	ajStrToLower(&dpdb_name);
	ajStrApp(&dpdb_name, dpdb_extn);
	if(!(dpdb_inf=ajFileNewDF(dpdb_path, dpdb_name)))
	{
	    ajFmtPrintF(errf, "WARN  %S not found\n", dpdb_name);
	    ajFmtPrintS(&msg, "Could not open dpdb file %S", dpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFileClose(&dpdb_inf); 
	    ajXyzScopDel(&scop);
	    continue;
	}


	/*Read the coordinate file for the domain*/
	if(!ajXyzCpdbRead(dpdb_inf, &pdb))
	{
	    ajFmtPrintF(errf, "ERROR %S file read error\n", dpdb_name);
	    ajFmtPrintS(&msg, "Error reading dpdb file %S", dpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFileClose(&dpdb_inf);
	    ajXyzScopDel(&scop);
	    ajXyzPdbDel(&pdb);
	    continue;
	}
	else
	    ajFileClose(&dpdb_inf);	

	
	
	/*Add a new sequence to the list - only ever one chain (chain 0) 
	  for domain coordinate files */
	nset++;
	seq=ajSeqNew();
	ajStrAss(&seq->Seq, pdb->Chains[0]->Seq);
	ajStrAss(&seq->Name, pdb->Pdb);
	ajListPushApp(list_seqs,seq);	


	/* Free the pdb structure */
	ajXyzPdbDel(&pdb);
	

	/* Add the current scop structure to the list */
	ajListPushApp(list_scop,scop);	
    }
    /* End of main application loop */






    /*Close the scop input file*/
    ajFileClose(&scop_inf);	
    
    
    /* Process the last family*/
    /* If the family does not have any members*/
    if(!nset)
    {
	/* Write diagnostic */
	ajFmtPrint("WARN  Empty family\n");
	ajFmtPrintF(errf, "WARN  Empty family\n");
    }
    /* Last family had members, so remove redundancy*/
    else
    {
	/* Remove redundancy from list_seqs*/
	embXyzSeqsetNR(list_seqs, &keep, &nsetnr, matrix, gapopen, gapextend,thresh);		


	/* Write file with SCOP entries that are retained*/
	for(iter=ajListIter(list_scop), x=0;
	    (scop_tmp=(AjPScop)ajListIterNext(iter));
	    x++)
	    if(ajIntGet(keep,x))
		ajXyzScopWrite(scop_outf, scop_tmp);
	ajListIterFree(iter);	
    


	/* Write diagnostic */
	ajFmtPrintF(errf, "Retained\n");
	for(iter=ajListIter(list_scop), x=0;
	    (scop_tmp=(AjPScop)ajListIterNext(iter));
	    x++)
	    if(ajIntGet(keep,x))
		ajFmtPrintF(errf, "%S\n", scop_tmp->Entry);
	ajListIterFree(iter);	
	ajFmtPrintF(errf, "Rejected\n");
	for(iter=ajListIter(list_scop), x=0;
	    (scop_tmp=(AjPScop)ajListIterNext(iter));
	    x++)
	    if(!(ajIntGet(keep,x)))
		ajFmtPrintF(errf, "%S\n", scop_tmp->Entry);
	ajListIterFree(iter);	
    }

    
    /* Free up the scop list */
    iter=ajListIter(list_scop);
    while((scop_tmp=(AjPScop)ajListIterNext(iter)))
	ajXyzScopDel(&scop_tmp);
    ajListIterFree(iter);	
    ajListDel(&list_scop);	    
    
    
    /* Free up the seqs list */
    iter=ajListIter(list_seqs);
    while((seq=(AjPSeq)ajListIterNext(iter)))
	ajSeqDel(&seq);
    ajListIterFree(iter);	
    ajListDel(&list_seqs);	    
    
    
    /* Tidy up*/
    ajMatrixfDel(&matrix);
    ajIntDel(&keep);	
    ajStrDel(&last_fam);
    ajStrDel(&msg);
    ajStrDel(&dpdb_path);
    ajStrDel(&dpdb_name);
    ajStrDel(&dpdb_extn);
    ajFileClose(&errf);    
    ajFileClose(&scop_outf);	    
    

    /* Bye Bye */
    ajExit();
    return 0;
}










