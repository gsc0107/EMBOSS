/* @source alignwrap application
**
** Aligns single sequences to an existing seed alignment
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
*******************************************************************************
**
**
**
**
**
**
** Operation
**
** alignwrap parses a stamp alignment annotated with SCOP
** classification records (Figure 1 - generated by the EMBOSS
** applications scopalign) and a file of database hits (Figure 2 -
** generated by the EMBOSS application psiblasts or swissparse and
** possibly processed by seqsort), and extends the structure-based
** alignment with sequences from the hits file one sequence at a time
** by calling clustalw.  A multiple sequence alignment in clustal
** format is generated (Figure 3).
**
**
** Figure 1   Input file (output from scopalign)
** (The format of the scopalign input file is explained in scopalign.c)
**
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  Number               10        20        30        40        50
**  d1vrea_              LSAAQRQVVASTWKDIAgsdngAGVGKECFTKFLSAHHDMAAV f gFS
**  d3sdhb_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  d3hbia_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  d3sdha_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  Post_similar --------11111111111111111-00-1111111111111111111111-0-111
**
**  Number        60        70        80        90       100       110
**  d1vrea_      GAS   dpGVADLGAKVLAQIGVAVSHLgDEGKMVAEMKAVGVRHKgygnkhIKAEY
**  d3sdhb_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKFAVNHI  t rkISAAE
**  d3hbia_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKLAVNHI  t rkISAAE
**  d3sdha_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKFAVNHI  t rkISAAE
**  Post_similar 111---0011111111111111111111011111111111111111--0-0011111
**
**  Number          120       130       140       150       160
**  d1vrea_      FEPlGASL LSAMEhriggkMNAAAKDAWAAAYADisgalisglqs
**  d3sdhb_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al
**  d3hbia_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al
**  d3sdha_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al
**  Post_similar 111-1111-11111-0-0-1111111111111111100-00-----
**
**
**  Figure 2  Excerpt from file of database hits
** (The format of the file of database hits (scop families file) is explained
** in psiblasts.c)
**
**  DE   Members of scop families
**  XX
**  EX   THRESH 0.001; MATRIX BLOSUM62;
**  XX
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  NS   1
**  XX
**  NN   [1]
**  XX
**  AC   HBDEX1
**  XX
**  CL   PSIBLAST
**  XX
**  RA   2 START; 79 END;
**  XX
**  SQ   SEQUENCE   141 AA;  15127 MW;  5EC7DB1E CRC32;
**       VLSPADKTNV KAAWGKVGAH AGEYGAEALE RMFLSFPTTK TYFPHFDLSH GSAQVKGHGK
**       KVADALTNAV AHVDDMPNAL SALSDLHAHK LRVDPVNFKL LSHCLLVTLA AHLPAEFTPA
**       VHASLDKFLA SVSTVLTSKY R
**  XX
**  NN   [2]
**  XX
**  AC   HBDEX2
**  XX
**  CL   PSIBLAST
**  XX
**  RA   2 START; 79 END;
**  XX
**  SQ   SEQUENCE   141 AA;  15127 MW;  5EC7DB1E CRC32;
**       VLSPADKTNV KAAWGKVGAH AGEYGAEALE RMFLSFPTTK TYFPHFDLSH GSAQVKGHGK
**       KVADALTNAV AHVDDMPNAL SALSDLHAHK LRVDPVNFKL LSHCLLVTLA AHLPAEFTPA
**       VHASLDKFLA SVSTVLTSKY R
**  XX
**  //
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Phycocyanins
**  XX
**
**  Figure 3  Excerpt of output file
**  (Standard clustal format, except no conservation or numbering lines are
**  given).
**
**  CLUSTALW
**
**  d1vrea_              LSAAQRQVVASTWKDIAgsdngAGVGKECFTKFLSAHHDMAAV f gFS
**  d3sdhb_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  d3hbia_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  d3sdha_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**
**  d1vrea_      GAS   dpGVADLGAKVLAQIGVAVSHLgDEGKMVAEMKAVGVRHKgygnkhIKAEY
**  d3sdhb_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKFAVNHI  t rkISAAE
**  d3hbia_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKLAVNHI  t rkISAAE
**  d3sdha_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKFAVNHI  t rkISAAE
**
**  d1vrea_      FEPlGASL LSAMEhriggkMNAAAKDAWAAAYADisgalisglqs
**  d3sdhb_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al
**  d3hbia_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al
**  d3sdha_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al



**  Important Note
**
**  WHEN RUNNING ALIGNWRAP AT THE HGMP IT IS ESSENTIAL THAT THE COMMAND
**  'use clustal' (which runs the script /packages/menu/USE/clustal) IS GIVEN
**  BEFORE IT IS RUN.
**
**  If run elsewhere, alignwrap requires a working version of clustalw
**/


#include "emboss.h"

static AjBool alignwrap_WriteScopout(AjPFile in, AjPFile out, AjPStr fam,
				     AjPStr sfam, AjPStr fold, AjPStr class);


/* @prog alignwrap ************************************************************
**
** Aligns a set of sequences to a seed alignment
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr  syscmd      = NULL;  /* command line arguments */
    AjPStr  inpath      = NULL;  /* path to the directory that contain
                                    the alignment files*/
    AjPStr  outpath     = NULL;  /* the path to the output directory */
    AjPStr  extn        = NULL;  /* the file extention of the
                                    alignment files */
    AjPStr  outextn     = NULL;  /* the file extention of the extended
                                    alignment files */
    AjPStr  tmp_str     = NULL;  /* temporary string */
    AjPStr  tmp_name    = NULL;  /* string object for holding randomly
                                    generated file names */

    AjPStr  filename    = NULL;  /* the name of the present working file */
    AjPStr  outfilename = NULL;  /* the name of the current output file */
    AjPStr  clustinf1   = NULL;  /* the name of the input alignment
                                    file for clustalw */
    AjPStr  clustinf2   = NULL;  /* the name of the input sequence
                                    file for clustalw */
    AjPStr  treefile    = NULL;  /* the name of the .dnd file that
                                    will be created */

    AjPFile scopin     = NULL;  /* scop families file (for reading)*/
    AjPFile scopout    = NULL;  /* scop families file (for writing)*/
    AjPStr scopoutname  = NULL;   /* name of the new scop families
                                     file (for writing)*/

    AjPStr fam          = NULL;   /* family name */
    AjPStr sfam         = NULL;   /* superfamily name */
    AjPStr fold         = NULL;   /* fold name */
    AjPStr class        = NULL;   /* class name */
    AjPStr msg          = NULL;   /* Message string */


    AjPFile alnf       	= NULL;   /* the seed alignment file with the
                                     scop info and the stamp info */
    AjPFile outf1       = NULL;   /* the outfile for the parsed
                                     database sequences in CLUSTAL
                                     format */
    AjPFile outf2       = NULL;   /* the outfile for the parsed seed
                                     alignment in CLUSTAL format */
/*    AjPFile scopf       = NULL; */  /* the scop families file */

    AjPList list        = NULL;   /* a list of relavant file names */
    AjPList tmp_list    = NULL;   /* temp list */

    AjPScopalg align    = NULL;   /* a scop alignment structure */

    AjPHitlist temphl   = NULL;   /* Temp. pointer to Hitlist object */

    ajint   posdash     = 0;      /* Position of last '/' in filenames
                                     from 'list' */
    ajint   posdot      = 0;      /* Position of last '.' in filenames
                                     from 'list' */




    msg        = ajStrNew();
    outfilename= ajStrNew();
    syscmd     = ajStrNew();
    inpath     = ajStrNew();
    outpath    = ajStrNew();
    extn       = ajStrNew();
    outextn    = ajStrNew();
    tmp_str    = ajStrNew();
    tmp_name   = ajStrNew();
    clustinf1  = ajStrNew();
    clustinf2  = ajStrNew();
    treefile   = ajStrNew();

    fam        = ajStrNew();
    sfam       = ajStrNew();
    fold       = ajStrNew();
    class      = ajStrNew();

    list       = ajListNew();

    embInit("alignwrap",argc,argv);

    inpath    = ajAcdGetString("inpath");
    extn      = ajAcdGetString("extn");
    outextn   = ajAcdGetString("outextn");
    scopin   = ajAcdGetInfile("scopin");
    scopoutname   = ajAcdGetString("scopoutname");
    outpath   = ajAcdGetString("outpath");

    /* Check directories */
    if((!ajFileDir(&inpath)) || (!(extn)))
	ajFatal("Could not open seed alignment directory or "
		"file extension NULL");

    if((!ajFileDir(&outpath)) || (!(outextn)))
	ajFatal("Could not open extended alignment directory or "
		"file extension NULL");




    /* Add a '.' to outextn if one does not already exist */
    if((ajStrChar(outextn, 0)!='.')) 	/* checks if the file
                                           extension starts with "."  */
	ajStrInsertC(&outextn, 0, ".");



    /* Create list of files in the path */
    ajStrAssC(&tmp_str, "*");  		/* assign a wildcard to tmp_str */

    if((ajStrChar(extn, 0)=='.')) 	/* checks if the file
                                           extension starts with "."  */
	ajStrApp(&tmp_str, extn);    	/* assign the acd input file
                                           extension to tmp */
    /* this picks up situations where the user has specified an
       extension without a "." */
    else
    {
	ajStrAppC(&tmp_str, ".");       /* assign "." to tmp */
	ajStrApp(&tmp_str, extn);       /* append tmp with a user
                                           specified extension */
    }

    /* all files containing seed alignments will be in a list */
    ajFileScan(inpath, tmp_str, &list, ajFalse, ajFalse, NULL, NULL,
	       ajFalse, NULL);


    /*Initialise random number generator for naming of tmp_name. files
      and create clustalw input files */
    ajRandomSeed();
    ajStrAssC(&tmp_name,ajFileTempName(NULL));


    /* read each psiblast file and create a list of Scophit structures */
    while(ajListPop(list,(void **)&filename))
    {
	/* open a scop seed alignment file */
	if((alnf = ajFileNewIn(filename)) == NULL)
	{
	    ajFmtPrintS(&msg, "Could not open seed alignment file %S",
			filename);
	    ajWarn(ajStrStr(msg));
	}
	else
	{
	    /* set up the random file for the input alignment for clustal */
	    ajStrAss(&clustinf1,tmp_name);
	    ajStrAppC(&clustinf1,".aln");

	    /* set up the random file for the input sequences for clustal */
	    ajStrAss(&clustinf2,tmp_name);
	    ajStrAppC(&clustinf2,".seqs");

	    /* setup the .dnd tree file to delete later */
	    ajStrAss(&treefile,tmp_name);
	    ajStrAppC(&treefile,".dnd");

	    /* read the scop seed alignment file into a scopalg object */
	    ajXyzScopalgRead(alnf,&align);

	    /* extract the scop information */
	    ajStrAss(&fam,align->Family);
	    ajStrAss(&sfam,align->Superfamily);
	    ajStrAss(&fold,align->Fold);
	    ajStrAss(&class,align->Class);

	    /* write out the seed alignment (from the scopalg object)
               in CLUSTAL format */
	    outf1 = ajFileNewOut(clustinf1);
	    ajXyzScopalgWriteClustal(align,&outf1);
	    ajFileClose(&outf1);

	    /* Delete Scopalg structure */
	    ajXyzScopalgDel(&align);

	    /*open the scopfamilies file and pass a file pointer into
              each of the functions */
	    /* scopf = ajFileNewIn(scopfam); */
	    /* extract the relavent family or superfamily or
               etc.. into a list of Hitlist objects. */
	    tmp_list = ajListNew();
	    ajXyzHitlistReadNode(scopin,&tmp_list, fam, sfam, fold, class);

	    /* Rewind the scop families input file, write scop
	       families output file.  The Typeobj element of the hits
	       used in the alignment are given as SEED */
	    ajFileSeek(scopin,0,SEEK_SET);
	    scopout = ajFileNewOut(scopoutname);

	    alignwrap_WriteScopout(scopin, scopout, fam, sfam, fold, class);


	    ajFileClose(&scopin);
	    ajFileClose(&scopout);



	    /* write out the sequences in the list of Hitlist
               structures in CLUSTAL format. */
	    outf2 = ajFileNewOut(clustinf2);
	    ajXyzHitlistsWriteFasta(&tmp_list,&outf2);
	    ajFileClose(&alnf);

	    ajFileClose(&outf2);

	    while(ajListPop(tmp_list,(void **)&temphl))
		ajXyzHitlistDel(&temphl);
	    ajListDel(&tmp_list);


	    /* Create the name of the output file */
	    posdash = ajStrRFindC(filename, "/");
	    posdot  = ajStrRFindC(filename, ".");
	    if(posdash >= posdot)
		ajFatal("Could not create filename. "
			"Email rranasin@hgmp.mrc.ac.uk");
	    else
	    {
		ajStrAssSub(&outfilename, filename, posdash+1, posdot-1);
	    }


	    /* run clustalw in profile to sequence mode */
	    ajFmtPrintS(&syscmd,"clustalw -type=protein -profile1=%S "
			"-sequences -profile2=%S -outfile=%S%S%S\n",
			clustinf1,clustinf2,outpath, outfilename, outextn);
	    system(ajStrStr(syscmd));

	    /* clean up directory */
	    ajSysUnlink(&clustinf1);
	    ajSysUnlink(&clustinf2);
	    ajSysUnlink(&treefile);
	}
	/* Free the nodes ! */
	ajStrDel(&filename);
    }


    /* clean up */
    ajStrDel(&msg);
    ajStrDel(&syscmd);
    ajStrDel(&fam);
    ajStrDel(&sfam);
    ajStrDel(&fold);
    ajStrDel(&class);
    ajStrDel(&inpath);
    ajStrDel(&outpath);
    ajStrDel(&extn);
    ajStrDel(&outextn);
    ajStrDel(&tmp_str);
    ajStrDel(&tmp_name);
    ajStrDel(&clustinf1);
    ajStrDel(&clustinf2);
    ajStrDel(&treefile);
    ajStrDel(&scopoutname);
    ajStrDel(&outfilename);
    ajListDel(&list);

    ajExit();
    return 0;

}





/* @funcstatic alignwrap_WriteScopout *****************************************
**
** Reads a scop families file, classifies those hits with the specified
** classification as SEED proteins (the Typeobj of the hit is written), and
** writes out the new scop families file.
**
** @param [r] in        [AjPFile]       Input scop families file
** @param [r] out       [AjPFile]       Output scop families file
** @param [r] fam       [AjPStr]       Family
** @param [r] sfam      [AjPStr]       Superfamily
** @param [r] fold      [AjPStr]       Fold
** @param [r] class     [AjPStr]       Class
**
** @return [AjBool] True on success (a file has been written)
** @@
******************************************************************************/
static AjBool alignwrap_WriteScopout(AjPFile in, AjPFile out, AjPStr fam,
				     AjPStr sfam, AjPStr fold, AjPStr class)
{
    AjPHitlist hitlist = NULL;
    ajint x=0;

    /* if family is specified then the other fields also have to be
       specified. */
    if(fam)
    {
	if(!sfam || !fold || !class)
	{
	    ajWarn("Bad arguments passed to alignwrap_WriteScopout\n");
	    return ajFalse;
	}
	else
	{
	    while(ajXyzHitlistRead(in,"//",&hitlist))
	    {
		if(ajStrMatch(fam,hitlist->Family) &&
		   ajStrMatch(sfam,hitlist->Superfamily) &&
		   ajStrMatch(fold,hitlist->Fold) &&
		   ajStrMatch(class,hitlist->Class))
		{
		    for(x=0; x<hitlist->N; x++)
			ajStrAssC(&hitlist->hits[x]->Typeobj, "SEED");
		}
		ajXyzHitlistWrite(out, hitlist);
		ajXyzHitlistDel(&hitlist);
		hitlist=NULL;
	    }

	}
    }

    /* if superfamily is specified then the other fields also have to
       be specified. */
    else if(sfam)
    {
	if(!fold || !class)
	{
	    ajWarn("Bad arguments passed to alignwrap_WriteScopout\n");
	    return ajFalse;
	}
	else
	{
	    while(ajXyzHitlistRead(in,"//",&hitlist))
	    {
		if(ajStrMatch(sfam,hitlist->Superfamily) &&
		   ajStrMatch(fold,hitlist->Fold) &&
		   ajStrMatch(class,hitlist->Class))
		{
		    for(x=0; x<hitlist->N; x++)
			ajStrAssC(&hitlist->hits[x]->Typeobj, "SEED");
		}
		ajXyzHitlistWrite(out, hitlist);
		ajXyzHitlistDel(&hitlist);
		hitlist=NULL;
	    }

	}
    }

    /* if fold is specified then the other fields also have to be
       specified. */
    else if(fold)
    {
	if(!class)
	{
	    ajWarn("Bad arguments passed to alignwrap_WriteScopout\n");
	    return ajFalse;
	}
	else
	{
	    while(ajXyzHitlistRead(in,"//",&hitlist))
	    {
		if(ajStrMatch(fold,hitlist->Fold) &&
		   ajStrMatch(class,hitlist->Class))
		{
		    for(x=0; x<hitlist->N; x++)
			ajStrAssC(&hitlist->hits[x]->Typeobj, "SEED");
		}
		ajXyzHitlistWrite(out, hitlist);
		ajXyzHitlistDel(&hitlist);
		hitlist=NULL;
	    }

	}
    }

    else
    {
	ajWarn("Bad arguments passed to alignwrap_WriteScopout\n");
	return ajFalse;
    }

    return ajTrue;
}
