/* @source seqnr application
**
** Removes redundant hits from a scop families file.
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
**
**
**
*******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE
*******************************************************************************
**
** Mon May 20 11:43:39 BST 2002
**
** The following documentation is out-of-date and should be disregarded.  It
** will be updated shortly.
**
*******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE
*******************************************************************************
**
** SEQNR DOCUMENTATION seqnr reads a scop families file and a
** validation file generated by the EMBOSS application seqsort and
** writes processed scop families and validation files.  For
** processing, it requires a structure-based sequence alignment file
** ('alignment' or 'alignment file' hereon) for each family in the
** scop families file.
**
** Each family in the scop families file is processed so that
** redundant hits are removed.  Hits derived from sequences in the
** corresponding alignment are considered when calculating redundancy.
** The processed file can then be used to build an extended alignment
** for each family by using the EMBOSS program alignwrap (which
** extends the alignment with sequences from the processed file).
**
** The validation input file will contain hits that could not be
** uniquely assigned to a single scop family.  It is processed so that
** all of the hits from the scop families input file plus hits
** corresponding to sequences from the alignment files are
** incorporated.
**
** Note however that the validation file must only contains hits with
** known accession numbers.  Therefore if an accession number could
** not be assigned to a hit from the alignment file, then that hit
** will NOT appear in the validation file, and a message to the log
** file will be written. Such cases will arise where the pdb code
** corresponding to a hit is not listed in the swissprot:pdb
** equivalence table, Epdbtosp.dat. (The hits in the scop families
** will normally have been found by a search against swissprot and
** therefore should all have accession numbers).
**
** To avoid introducing duplicate hits in the validation file when
** incorporating hits from the alignment, any hits that have an
** identical accession number to an alignment sequence and which
** overlap by at least 10 residues will NOT be written.
**
** Hits in the validation file are flagged as NON_REDUNDANT or
** REDUNDANT as appropriate.  They are then flagged as 'SEED' or left
** as 'HIT' depending on whether the file will be used to validate
** signatures generated from (i) structure-based sequence alignemnts
** (hits corresponding to sequences from the alignment files only are
** marked up as 'SEED') or (ii) extended alignments (all hits in the
** scop families output file are marked up as 'SEED').
**
** Caveat - It should be care must be taken when validating signatures
** from structure-based sequence alignments, as some families in the
** validation file will not contain the same number of SEED's as
** sequences in the alignment.
**
**
**
**Each family is processed in turn:
**
**
**  1    Compile list of Scop objects for domains in alignment file.
**  1.1  Open the corresponding alignment file.
**  1.2  Extract SCOP domain codes
**  1.3  Query scop classification file & retrieve the corresponding
**        scop domains
**  1.4  Construct a list of these domains.
**
**  2.   Mark up hits for this family that should be removed from the
**       scop families
**       file (i.e. those from the alignment file).
**
**  3.   Calculate redundancy and remove unwanted hits
**  3.1  Perform pairwise comparison of all hits in list and identify
**        redundant hits.
**  3.2  Remove the redundant hits.
**  3.3  Remove the hits that are marked up for removal.
**
**  4.   Write new hits file.
**
**PROCESS VALIDATION FILE
**
** Each family in the validation file is processed in turn: Remove
** from the validation file all targetted sequences.  -> Add domain
** sequences back into the validation file.  -> Mark up as "SEED" in
** validation file i) all hits in new his file ii) all domain
** sequences.
******************************************************************************/


#include "emboss.h"

static AjBool seqnr_GetScopFromAlign(AjPScopalg align, AjPScop *scop_arr,
				     ajint dim,
				    AjPList* list);

static AjBool seqnr_ScophitsAccToHitlist(AjPList in, AjPHitlist *out,
					 AjIList *iter);


/* @prog seqnr ****************************************************************
**
** Test
**
******************************************************************************/
int main(int argc, char **argv)
{
    /* Variables */
    /* File input and output */
    AjPFile logf         = NULL;   /* log file pointer */
    AjPFile inf          = NULL;   /* file containing the unprocessed hits */
    AjPFile vinf         = NULL;   /* file pointer for the validation file */
    AjPFile outf         = NULL;   /* the outfile for the processed families */
    AjPStr  voutfname    = NULL;   /* the validation output file name */
    AjPFile voutf        = NULL;   /* the pointer for the validation
                                      output file */
    AjPFile scop_inf     = NULL;   /* file pointer for Escop.dat */
    AjPFile aln_inf      = NULL;   /* file pointer to alignment file */
    AjPStr alignpath     = NULL;   /* path to the seed alignment files */
    AjPStr alignextn     = NULL;   /* extension for seed alignment files */
    AjPStr alignname     = NULL;   /* name of alignment file */


    /* Parameters */
    float gapopen        =0.0;     /* Gap insertion penalty */
    float gapextend      =0.0;     /* Gap extension penalty */
    float thresh         =0.0;     /* Threshold for non-redundancy */
    AjPMatrixf matrix    =NULL;    /* Substitution matrix */


    /* Housekeeping */
    AjPStr    exec       = NULL;   /* The UNIX command line to be executed*/
    ajint     i          = 0;      /* loop counter */
    AjIList   iter       = NULL;   /* Reused list iterator */
    AjPInt    keep       = NULL;   /* 1: This sequence was kept after
				      redundancy removal,
				      0:# it was discarded */
    ajint     nsetnr     = 0;      /* No. proteins in the non-redundant set */
    AjPScop    tmp_scop  = NULL;   /* temp. scop object pointer for
                                      objects popped from scop_align*/
    AjPScophit tmp_hit   = NULL;   /* temp. pointer to a scophit
                                      object from famin_hits */
    AjPSeq     tmp_seq   = NULL;   /* A temp. AjPSeq pointer for
                                      constructing famin_seqs */
    AjPStr     *mode     =NULL;    /* Mode of operation from acd*/
    AjPList    tmp_list  =NULL;    /* Temp. list for freeing memory */
    AjBool    isseed1    =ajFalse; /* Used for targetting overlaps
                                      with alignemnt sequences */
    AjBool    isseed2    =ajFalse; /* Used for targetting overlaps
                                      with alignemnt sequences */
    AjPScophit hit1        = NULL; /* Used for targetting overlaps
                                      with alignemnt sequences */
    AjPScophit hit2        = NULL; /* Used for targetting overlaps
                                      with alignemnt sequences */


    /* Alignment */
    AjPScopalg align     = NULL;   /* Seed alignment corresponding to
                                      the current hitlist */
    AjPList  scop_align  = NULL;   /* list of scop objects from the
                                      alignment file */


    /* SCOP database */
    AjPList   scop_list  = NULL;   /* list of scop objects for entire
                                      Escop.dat */
    AjPScop  *scop_arr   = NULL;   /* Array of Scop objects derived
    from scop_list ordered by Domain Id*/
    ajint     scop_dim   = 0;      /* Size of scop_arr */


    /* Family being processed */
    AjPHitlist famin     = NULL;   /* The family from the scop
                                      families that will be processed */
    AjPList famin_list   = NULL;   /* A list containing famin only */
    AjPList famin_hits   = NULL;   /* A list of Scophits corresponding
				      to famin.  Scophits derived from
				      the alignment file are
				      eventually added to this list,
				      which is then processed. */
    AjPHitlist famout_valid= NULL;  /* A Hitlist corresponding to
				       famin_hits before redundant
				       domains have been removed and
				       hits targetted for removal have
				       been removed.  It is this
				       Hitlist that is written to the
				       validation file */
    AjPList famin_seqs   = NULL;   /* A list of AjPSeq's
				      correspdonding to
				      famin_hits. Used for calculating
				      redundancy removal. */

    AjPList famout_hits  = NULL;   /* A list of Scophit objects that
                                      has been processed */
    AjPHitlist famout    = NULL;   /* The processed family as a
                                      hitlist (from famout_hits) */


    AjPScophit align_hit = NULL;   /* Temp. variable for a Scophit
                                      derived from the alignment file */




    /* Read data from acd */
    embInit("seqnr",argc,argv);

    inf       = ajAcdGetInfile("inf");
    outf      = ajAcdGetOutfile("outf");
    vinf      = ajAcdGetInfile("vinf");
    voutfname = ajAcdGetString("voutfname");
    scop_inf  = ajAcdGetInfile("scopin");
    alignpath = ajAcdGetString("alignpath");
    alignextn = ajAcdGetString("alignextn");
    thresh    = ajAcdGetFloat("thresh");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    mode      = ajAcdGetList("mode");
    logf      = ajAcdGetOutfile("logf");

    alignname = ajStrNew();
    exec     = ajStrNew();





    /* Copy contents of validation input file to create validation
       output file, open the output file.*/
    ajFmtPrintS(&exec, "cp %S %S\n", ajFileGetName(vinf), voutfname);
    system(ajStrStr(exec));
    voutf=ajFileNewApp(voutfname);

    /* Create list of scop objects for entire Escop.dat */
    scop_list  = ajListNew();
    while((ajXyzScopReadC(scop_inf,"*",&tmp_scop)))
        ajListPushApp(scop_list,tmp_scop);

    /* Order the list of Scop objects by Domain Id and create an array */
    ajListSort(scop_list, ajXyzScopCompId);
    scop_dim = ajListToArray(scop_list,(void ***)&scop_arr);


    /* Error handing if Escop.dat was empty */
    if(!scop_dim)
    {
        ajWarn("Empty list in seqnr_GetScopFromAlign\n");
	ajFileClose(&logf);
	ajFileClose(&inf);
	ajFileClose(&outf);
	ajFileClose(&vinf);
	ajFileClose(&voutf);
	ajStrDel(&voutfname);
	ajFileClose(&scop_inf);
	ajStrDel(&alignpath);
	ajStrDel(&alignextn);
	ajMatrixfDel(&matrix);

	ajStrDel(&alignname);

	ajExit();
	return 1;
    }




    /* Read Hitlist for the family from the scop families that will be
       processed, each entry delimited by a "//" is read and processed
       in turn */
    while(ajXyzHitlistRead(inf,"//",&famin))
    {
	ajFmtPrint("Processing %d\n", famin->Sunid_Family);
	ajFmtPrintF(logf, "//\n%d\n", famin->Sunid_Family);

/*JISON        keep        = ajIntNew(); */
        famin_list  = ajListNew();
        famin_hits  = ajListNew();
        famin_seqs  = ajListNew();
        famout_hits = ajListNew();
	tmp_list    = ajListNew();



        /* Open structure-based sequence alignment file corresponding
	   to the family.  If seed alignment is absent no point in
	   continuing */
	ajStrFromInt(&alignname, famin->Sunid_Family);
	ajStrApp(&alignname,alignextn);
        if(!(aln_inf=ajFileNewDF(alignpath,alignname)))
        {
            ajWarn("The alignment file %S%S could not be found\n",
		   alignpath,alignname);
	    ajFmtPrintF(logf, "Alignment file %S%S not found\n",
			alignpath,alignname);
	    ajIntDel(&keep);
	    ajListDel(&famin_list);
	    ajListDel(&famin_hits);
	    ajListDel(&famin_seqs);
	    ajListDel(&famout_hits);
	    ajListDel(&tmp_list);
            continue;
        }

        /* Read seed alignment into scopalg structure */
        ajXyzScopalgRead(aln_inf,&align);


        /* Construct list of scop objects from the alignment file */
        scop_align = ajListNew();

        if(!seqnr_GetScopFromAlign(align,scop_arr,scop_dim,&scop_align))
	    ajFatal("seqnr_GetScopFromAlign");

	/* scop_align must be freed.
	   scop_align points to data in scop_arr so do NOT free nodes
	   */


        /* Push famin onto a list (the list contains this single
	   Hitlist only) No need to pop famin_list and free the single
	   node, so long as famin is freed seperately.  Convert this
	   list to a list of Scophit objects.  Must pop famin_hits and
	   free the nodes that are allocated by ajXyzHitlistToScophits */

        ajListPushApp(famin_list,famin);
        ajXyzHitlistToScophits(famin_list,&famin_hits);


        /* Add the scop objects from the alignment file to the main list */
        while(ajListPop(scop_align,(void **)&tmp_scop))
        {
            align_hit = ajXyzScophitNew();
            ajXyzScopToScophit(tmp_scop,&align_hit);

	    if(ajStrMatchC(align_hit->Acc, "Not_available"))
		ajFmtPrintF(logf,
			    "No swissprot sequence for domain %S "
			    "from alignment "
			    "so no hit will be given in validation file\n",
			    tmp_scop->Entry);



	    /* For structure-based sequence alignemnts, hits
	       corresponding to sequences from the alignment files
	       only are marked up as 'SEED') */
	    if(ajStrChar(*mode, 0) == '1')
	    {
		ajStrAssC(&(align_hit->Typeobj), "SEED");
	    }


            ajXyzScophitTarget(&align_hit);
            ajListPushApp(famin_hits,align_hit);

	    /*The memory here will be freed when famin_hits is popped
              and the nodes are freed */
        }

        /* Delete original Hitlist and the derived list
           We now just have a list of Scophit's called <famin_hits> */
        ajXyzHitlistDel(&famin);
	famin=NULL;
	ajListDel(&famin_list);
	famin_list=NULL;


        /*
	   Sort the list and target hits that are identical or overlapping
	   (the Target2 element will set ajTrue).
	   Overlap will only arise with alignment sequences, other overlaps
	   will have been processed by seqsort.

	   Accession number and start/end point will not be available
	   for some alignment sequences - ajXyzScophitsOverlapAcc will
	   return ajFalse for these.

	   */


	ajListSort(famin_hits,ajXyzScophitCompAcc);


	iter = ajListIter(famin_hits);
	hit1 = (AjPScophit)ajListIterNext(iter);

	while((hit2=(AjPScophit)ajListIterNext(iter)))
	{

	    /*DIAGNOSTICS
	      ajFmtPrint("Comparing %S (%S) to %S (%S)\n",
		       hit1->Acc, hit1->Typeobj,
		       hit2->Acc, hit2->Typeobj); */


	    if(ajXyzScophitsOverlapAcc(hit1,hit2,10))
	    {
		/*target the hit that is not a SEED  */
		isseed1 = ajStrMatchC(hit1->Typeobj, "SEED");
		isseed2 = ajStrMatchC(hit2->Typeobj, "SEED");

		if(isseed1 && isseed2)
		{
		    ajWarn("2 domains in the alignment share the "
			   "same accession "
			   "number and overlap.\n"
			   "Only one will be given in the validation file.");
		    ajFmtPrintF(logf, "Overlap between 2 SEEDs (acc. %S) "
				"from alignment file.\n",
				hit1->Acc);
		    ajXyzScophitTarget2(&hit1);
		}
		else if(!isseed1 && !isseed2)
		    ajFatal("Overlap between two non-SEED's which should "
			    "never happen");
		else if(!isseed1)
		    ajXyzScophitTarget2(&hit1);
		else if(!isseed2)
		    ajXyzScophitTarget2(&hit2);

	    }
	    /* move one node along  */
	    hit1 = hit2;
	}

	ajListIterFree(iter);
	iter=NULL;

	/* DIAGNOSTICS
	printf("\n\n");
	iter = ajListIter(famin_hits);
	while((hit2=(AjPScophit)ajListIterNext(iter)))
	{
	    ajFmtPrint("%S (%B)\n", hit2->Acc, hit2->Target2);
	}
	printf("\n\n");
	ajListIterFree(iter);
	iter=NULL;
	*/


	/* Construct a sequence set from the main list of Scophit's */
        iter = ajListIter(famin_hits);
        while((tmp_hit=(AjPScophit)ajListIterNext(iter)))
        {
	    /* Do not want to include domains from the alignment that
	       duplicate the same accesion number and overlap ! */
	    if(tmp_hit->Target2)
		continue;

            tmp_seq = ajSeqNew();
            ajStrAss(&tmp_seq->Name,tmp_hit->Acc);
            ajStrAss(&tmp_seq->Seq,tmp_hit->Seq);
            ajListPushApp(famin_seqs,tmp_seq);
	    /* famin_seqs must be popped and the nodes freed */
        }
        ajListIterFree(iter);
        iter=NULL;


        /* Remove the redundancy from the sequence set */
/*JISON*/        keep        = ajIntNew();
        embXyzSeqsetNR(famin_seqs, &keep, &nsetnr, matrix, gapopen,
		       gapextend, thresh);

        /* clean up famin_seqs */
        iter=ajListIter(famin_seqs);
        while((tmp_seq=(AjPSeq)ajListIterNext(iter)))
            ajSeqDel(&tmp_seq);
        ajListIterFree(iter);
        ajListDel(&famin_seqs);


        /* Create a list of Scophit objects that has been processed */
        for(iter = ajListIter(famin_hits), i = 0;
	    (tmp_hit = (AjPScophit)ajListIterNext(iter));
	    i++)
        {
	    /* Redundancy will not have been calculated for domains
	       from the alignment that duplicate the same accesion
	       number and overlap ! */
	    if(tmp_hit->Target2)
	    {
		i--;
		/* Push the hit onto tmp_list for later free'ing */
		ajListPushApp(tmp_list,tmp_hit);
		continue;
	    }


	    /* For extended alignemnts, all hits in the scop families output
	       file are marked up as 'SEED' */
	    if(ajStrChar(*mode, 0) == '2')
	    {
		ajStrAssC(&(tmp_hit->Typeobj), "SEED");
	    }

            if(ajIntGet(keep,i))
	    {
		ajStrAssC(&(tmp_hit->Group), "NON_REDUNDANT");
		ajListPushApp(famout_hits,tmp_hit);
	    }
	    else
	    {
		ajStrAssC(&(tmp_hit->Group), "REDUNDANT");

		if(ajStrMatchC(tmp_hit->Acc, "P02213"))
		    printf("*********REDUNDANT written\n");


		/* Push the hit onto tmp_list for later free'ing */
		ajListPushApp(tmp_list,tmp_hit);
	    }


	    /* All the memory for famin_hits is caught by tmp_list or
	       famout_hits.  Both must be popped and the nodes freed */
	}


/*JISON*/        ajIntDel(&keep);

        ajListIterFree(iter);
        iter = NULL;


	/* DIAGNOSTICS
	printf("\n\n");
	iter = ajListIter(famin_hits);
	while((hit2=(AjPScophit)ajListIterNext(iter)))
	{
	    ajFmtPrint("%S (%B)\n", hit2->Acc, hit2->Target2);
	}
	printf("\n\n");
	ajListIterFree(iter);
	iter=NULL;
	*/


	/* Write Hitlist for printing out to validation file.

	   This corresponds to famin_hits before redundant domains and
	   hits targetted for removal have been removed (garbage collection).

	   seqnr_ScophitsAccToHitlist will only write hits to the Hitlist
	   if an accession number is given.

	   Also, any hits that overlap with an alignment sequence with
	   identical accession number will NOT be written
	   (Target2==ajTrue for these).

	   This avoids introducing duplicate hits in the validation file
	   when incorporating hits from the alignment.


	   */

	/* famout_valid and iter must be NULL in this context */
        seqnr_ScophitsAccToHitlist(famin_hits,&famout_valid,&iter);
        ajListDel(&famin_hits);
        ajListIterFree(iter);
        iter = NULL;

	/* Write validation file */
        ajXyzHitlistWrite(voutf,famout_valid);
        ajXyzHitlistDel(&famout_valid);
        famout_valid = NULL;



        /* Remove targeted hits ... i.e. the ones from the alignment*/
        ajListGarbageCollect(famout_hits,ajXyzScophitDelWrap,
			     (const void*)ajXyzScophitCheckTarget);



        /* write a hitlist of the processed scophits */
	/* famout and iter must be NULL in this context */
        ajXyzScophitsToHitlist(famout_hits,&famout,&iter);
	/* famout must be freed */

        ajListIterFree(iter);
        iter = NULL;

        /* free the nodes in famout_hits and the list itself  */
        iter=ajListIter(famout_hits);
        while((tmp_hit=(AjPScophit)ajListIterNext(iter)))
            ajXyzScophitDel(&tmp_hit);
        ajListIterFree(iter);
        ajListDel(&famout_hits);


	/* We are now done with the main list of Scophit's. Memory in tmp_list
	   can also be freed*/
        iter=ajListIter(tmp_list);
        while((tmp_hit=(AjPScophit)ajListIterNext(iter)))
            ajXyzScophitDel(&tmp_hit);
        ajListIterFree(iter);
        ajListDel(&tmp_list);
	iter=NULL;


        /* write the processed family (hitlist) to file in EMBL format */
        ajXyzHitlistWrite(outf,famout);
        ajXyzHitlistDel(&famout);
        famout = NULL;



        /* clean up */
/*JISON        ajIntDel(&keep); */
        ajXyzScopalgDel(&align);
        ajFileClose(&aln_inf);
        ajListDel(&scop_align);

    }

    ajFileClose(&vinf);
    ajFileClose(&voutf);

    ajStrDel(&voutfname);


    /* clean up */
    ajFileClose(&logf);
    ajFileClose(&inf);
    ajFileClose(&outf);
    ajFileClose(&scop_inf);
    ajStrDel(&alignpath);
    ajStrDel(&alignextn);
    ajMatrixfDel(&matrix);

    ajStrDel(&alignname);
    ajStrDel(&exec);


    iter=ajListIter(scop_list);
    while((tmp_scop=(AjPScop)ajListIterNext(iter)))
        ajXyzScopDel(&tmp_scop);
    ajListIterFree(iter);
    ajListDel(&scop_list);
    ajListDel(&tmp_list);

    AJFREE(scop_arr);

    ajStrDel(&mode[0]);
    AJFREE(mode);


    ajExit();
    return 0;

}

/* @funcstatic seqnr_GetScopFromAlign *****************************************
**
** Takes a Scopalg object (scop alignment) and an array of AjPScop objects
** from the Escop database.
** Extracts the scop domain codes from the alignment and compiles a list of
** corresponding Scop objects from the scop database (EScop.dat).
**
** @param [r] align     [AjPScopalg]   Contains a seed alignment.
** @param [r] scop_arr     [AjPScop*]   Array of AjPScop objects
** @param [r] scop_dim       [ajint]   Size of array
** @param [w] list      [AjPList*]      List of Scop objects.
**
** @return [AjBool] A populated list has been returned (a file has
** been written) @@
******************************************************************************/
static AjBool seqnr_GetScopFromAlign(AjPScopalg align, AjPScop *scop_arr,
				     ajint scop_dim,
				     AjPList* list)
{
    AjPStr   entry_up = NULL;  /* Current entry, upper case */
    ajint  idx        = 0;     /* Index into array for the Pdb code */
    ajint  i          = 0;     /* Simple loop counter */


    entry_up  = ajStrNew();


    /* check for bad arguments */
    if(!align)
    {
        ajWarn("Bad args passed to seqnr_GetScopFromAlign");
	ajStrDel(&entry_up);
        return ajFalse;
    }



    /* write to list the scop structures matching a particular family
       of domains */
    for(i=0;i<align->N;i++)
    {
	ajStrAssS(&entry_up, align->Codes[i]);
	ajStrToUpper(&entry_up);


        if((idx = ajXyzScopBinSearch(entry_up,scop_arr,scop_dim))==-1)
        {
	    ajStrDel(&entry_up);
	    return ajFalse;
	}
	else
	{
	    /* DIAGNOSTICS
	    ajFmtPrint("Pushing %d (%S)\n", scop_arr[idx]->Sunid_Family,
		       scop_arr[idx]->Acc); */

            ajListPushApp(*list,scop_arr[idx]);
	}

    }

    /* clean up */
    ajStrDel(&entry_up);


    return ajTrue;
}









/* @funcstatic seqnr_ScophitsAccToHitlist *************************************
**
** Reads from a list of Scophit objects and writes a Hitlist object
** with the next block of hits with identical SCOP classification. A
** Hit is only written to the Hitlist if an accession number is given.
**
** Also, only one of any pair of duplicate hits (overlapping hits with
** identical accession) will be written to the Hitlist. An 'overlap'
** is defined as a shared region of 10 or more residues.  To check for
** these the list is first be sorted by Accession number.
**
** If the iterator passed in is NULL it will read from the start of the list,
** otherwise it will read from the current position. Memory for the Hitlist
** will be allocated if necessary and must be freed by the user.
**
** @param [r] in      [AjPList]     List of pointers to Scophit objects
** @param [w] out     [AjPHitlist*] Pointer to Hitlist object
** @param [r] iter    [AjIList*]    Pointer to iterator for list.
**
** @return [AjBool] True on success (lists were processed ok)
** @@
******************************************************************************/
static AjBool seqnr_ScophitsAccToHitlist(AjPList in, AjPHitlist *out,
					 AjIList *iter)
{
    AjPScophit scoptmp=NULL;        /* Temp. pointer to Scophit object */
    AjPHit     tmp=NULL;            /* Temp. pointer to Hit object */
    AjPList    list=NULL;           /* Temp. list of Hit objects */
    AjBool     do_fam=ajFalse;
    AjBool     do_sfam=ajFalse;
    AjBool     do_fold=ajFalse;
    AjBool     do_class=ajFalse;
    AjPStr     fam=NULL;
    AjPStr     sfam=NULL;
    AjPStr     fold=NULL;
    AjPStr     class=NULL;
    ajint Sunid_Family=0;



    /* Check args and allocate memory */
    if(!in || !iter)
    {
	ajWarn("NULL arg passed to seqnr_ScophitsAccToHitlist");
	return ajFalse;
    }


    /* If the iterator passed in is NULL it will read from the start of the
       list, otherwise it will read from the current position.*/
    if(!(*iter))
	*iter=ajListIter(in);


    if(!((scoptmp=(AjPScophit)ajListIterNext(*iter))))
    {
	ajWarn("Empty list in ajXyzScophitsToHitlist");
	ajListIterFree(*iter);
	*iter=NULL;
	return ajFalse;
    }
    /* Find the first Scophit which has an accession number
       if necessary */
    if((ajStrMatchC(scoptmp->Acc,"Not_available")) ||
       (MAJSTRLEN(scoptmp->Acc)==0))
    {
	while((scoptmp=(AjPScophit)ajListIterNext(*iter)))
	{
	    if((ajStrMatchC(scoptmp->Acc,"Not_available") == ajFalse) &&
	       (MAJSTRLEN(scoptmp->Acc)!=0))
		break;
	}
	if(!scoptmp)
	{
	    ajWarn("List with no Scophits with Acc in "
		   "seqnr_ScophitsAccToHitlist");
	    ajListIterFree(*iter);
	    *iter=NULL;
	    return ajFalse;
	}
    }



    if(!(*out))
	*out = ajXyzHitlistNew(0);

    fam   = ajStrNew();
    sfam  = ajStrNew();
    fold  = ajStrNew();
    class = ajStrNew();

    list = ajListNew();


    Sunid_Family=scoptmp->Sunid_Family;


    if(scoptmp->Class)
    {
	do_class = ajTrue;
	ajStrAssS(&class, scoptmp->Class);
    }
    if(scoptmp->Fold)
    {
	do_fold= ajTrue;
	ajStrAssS(&fold, scoptmp->Fold);
    }
    if(scoptmp->Superfamily)
    {
	do_sfam = ajTrue;
	ajStrAssS(&sfam, scoptmp->Superfamily);
    }
    if(scoptmp->Family)
    {
	do_fam = ajTrue;
	ajStrAssS(&fam, scoptmp->Family);
    }

    /* Only want to push the hit if it is not targetted */
    if(!scoptmp->Target2)
    {
	ajXyzScophitToHit(&tmp, scoptmp);
	ajListPush(list, (AjPHit) tmp);
	tmp=NULL;
    }



    while((scoptmp=(AjPScophit)ajListIterNext(*iter)))
    {
	if(do_class)
	    if(!ajStrMatch(scoptmp->Class, class))
		break;
	if(do_fold)
	    if(!ajStrMatch(scoptmp->Fold, fold))
		break;
	if(do_sfam)
	    if(!ajStrMatch(scoptmp->Superfamily, sfam))
		break;
	if(do_fam)
	    if(!ajStrMatch(scoptmp->Family, fam))
		break;

	if((ajStrMatchC(scoptmp->Acc,"Not_available")) ||
	   (MAJSTRLEN(scoptmp->Acc)==0))
	    continue;

	if(scoptmp->Target2)
	    continue;

	ajXyzScophitToHit(&tmp, scoptmp);
	ajListPush(list, (AjPHit) tmp);
	tmp=NULL;

	continue;
    }
    ajStrAssS(&(*out)->Class, class);
    ajStrAssS(&(*out)->Fold, fold);
    ajStrAssS(&(*out)->Superfamily, sfam);
    ajStrAssS(&(*out)->Family, fam);
    (*out)->Sunid_Family = Sunid_Family;


    /* Copy temp. list to Hitlist */
    (*out)->N = ajListToArray(list, (void ***) &((*out)->hits));

    /* Tidy up and return */
    ajStrDel(&fam);
    ajStrDel(&sfam);
    ajStrDel(&fold);
    ajStrDel(&class);
    ajListDel(&list);
    return ajTrue;
}
