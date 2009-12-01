/************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 ************************************************************/

/* hmmconvert.c
 * SRE, Thu Oct 30 08:56:22 1997; St. Louis
 * 
 * main() for converting between HMM file formats, and
 * for converting HMMs to other software formats like GCG profiles.
 * 
 * Modified for EMBOSS by Alan Bleasby (ISMB 2001)
 */

#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"		/* data structures, macros, #define's   */
#include "config.h"		/* compile-time configuration constants */
#include "funcs.h"		/* function declarations                */
#include "globals.h"		/* alphabet global variables            */
#include "squid.h"		/* general sequence analysis library    */

#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif


int main(int argc, char **argv)
{
    const char *infile;		/* name of input HMM file                   */
    const char *outfile;	/* name of output HMM file                  */
    HMMFILE *infp;		/* input HMM file ptr                       */
    FILE    *outfp;		/* output HMM file ptr                      */
    char    *mode=NULL;		/* mode to open file in                     */
    struct plan7_s *hmm;	/* a profile HMM structure                  */
    int      nhmm;		/* number of HMMs converted                 */

    int   do_append;		/* TRUE to append to existing outfile       */
    int   do_force;		/* TRUE to allow overwriting */
    enum hmmfmt_e
    {
	P7ASCII, P7BINARY, GCGPROFILE, BICPROFILE
    } 
    outfmt;			/* output format */
    AjPStr ajfmt;
    AjBool ajappend=ajFalse;
    AjBool ajforce=ajFalse;
    AjPStr instr;
    AjPStr outstr;
    AjPFile inf;
    AjPFile outf;
  
    char ajc='\0';
  


#ifdef MEMDEBUG
    unsigned long histid1, histid2, orig_size, current_size;
    orig_size = malloc_inuse(&histid1);
    fprintf(stderr, "[... memory debugging is ON ...]\n");
#endif

    /*********************************************** 
     * Parse command line
     ***********************************************/

    embInitPV("ohmmconvert",argc,argv,"HMMER",VERSION);

    outfmt    = P7ASCII; 
    do_append = FALSE;
    do_force  = FALSE;

    ajfmt    = ajAcdGetListSingle("format");
    ajappend = ajAcdGetBoolean("append");
    ajforce  = ajAcdGetBoolean("force");
    inf      = ajAcdGetInfile("infile");
    outf     = ajAcdGetOutfile("outfile");
  
    if(ajappend)
	do_append=TRUE;
    else
	do_append=FALSE;
    if(ajforce)
	do_force=TRUE;
    else
	do_force=FALSE;

    ajc = ajStrGetCharFirst(ajfmt);
    if(ajc=='A')
	outfmt = P7ASCII;
    else if(ajc=='B')
	outfmt = P7BINARY;
    else if(ajc=='G')
	outfmt = GCGPROFILE;
    else
	outfmt = BICPROFILE;

    instr  = ajStrNewC((char *)ajFileGetNameC(inf));
    outstr = ajStrNewC((char *)ajFileGetNameC(outf));
    if(*ajStrGetPtr(outstr)>31)
    {
	ajFileClose(&outf);
	if(!ajFilenameGetSize(outstr))
	    unlink(ajStrGetPtr(outstr));
    }

    ajFileClose(&inf);


    infile  = ajStrGetPtr(instr);
    outfile = ajStrGetPtr(outstr);



  
    /*********************************************** 
     * Open input HMM database (might be in HMMERDB or current directory)
     ***********************************************/

    if ((infp = HMMFileOpen(infile, "HMMERDB")) == NULL)
	ajFatal("Failed to open HMM database %s\n", infile);

    /*********************************************** 
     * Open output HMM file
     ***********************************************/
  
    if (do_append) 
    {	/* If we're appending to a file, it needs to be Plan7 format */
	HMMFILE *test;

	if (FileExists(outfile))
	{ 
	    test = HMMFileOpen(outfile, NULL);
	    if (test == NULL) 
		ajFatal("%s not an HMM file; I refuse to append to it; "
			"using stdout instead", outfile);
	    HMMFileClose(test);
	}
	switch (outfmt)
	{
	case P7ASCII:    mode = "a";  break;
	case P7BINARY:   mode = "ab"; break;
	case GCGPROFILE: ajFatal("You cannot append GCG profiles");
	case BICPROFILE: ajFatal("You cannot append Compugen extended "
				 "profiles");
	default:         ajFatal("unexpected format");
	}
    }
    else
    {					/* else, we're writing a new file */
	if (! do_force && FileExists(outfile))
	    ajFatal("Output HMM file %s already exists. Please rename "
		    "or delete it.", outfile); 
	switch (outfmt)
	{
	case P7ASCII:    mode = "w";  break;
	case P7BINARY:   mode = "wb"; break;
	case GCGPROFILE: mode = "w";  break;
	case BICPROFILE: mode = "w";  break;
	default:         ajFatal("unexpected format");
	}
    }

    if(*outfile < 32)
	outfp=stdout;
    else if ((outfp = fopen(outfile, mode)) == NULL) 
	ajFatal("Failed to open output file %s for writing", outfile);

    /*********************************************** 
     * Show the banner
     ***********************************************/
    /*
       Banner(stdout, banner);
       ajUser(   "Input HMM file:           %s\n", infile);
       ajUser(   "Output HMM file:          %s\n", outfile);
       fprintf(stdout,   "Converting to:            ");
       switch (outfmt)
       {
       case P7ASCII:    puts("HMMER Plan7 ASCII");      break;
       case P7BINARY:   puts("HMMER Plan7 binary");     break;
       case GCGPROFILE: puts("GCG Profile .prf");       break;
       case BICPROFILE: puts("Compugen .eprf profile"); break;
       default:         ajFatal("unexpected fault");
       }
       printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - -"
       " - - - -\n\n");
       */
    /*********************************************** 
     * Do the conversion
     ***********************************************/

    nhmm = 0;
    while (HMMFileRead(infp, &hmm))
    {
	if (hmm == NULL) 
	    ajFatal("HMM file %s may be corrupt or in incorrect format; "
		    "parse failed", infile);

	switch(outfmt)
	{
	case P7ASCII:    WriteAscHMM(outfp, hmm);         break;
	case P7BINARY:   WriteBinHMM(outfp, hmm);         break;
	case GCGPROFILE: WriteProfile(outfp, hmm, FALSE); break;
	case BICPROFILE: WriteProfile(outfp, hmm, TRUE);  break;
	default:         ajFatal("unexpected format");
	}

	printf(" - converted %s\n", hmm->name);
	FreePlan7(hmm);
	nhmm++;
    }
    printf("\n%d HMM(s) converted and written to %s\n", nhmm, outfile);

    /*********************************************** 
     * Clean-up and exit.
     ***********************************************/

    HMMFileClose(infp);
    fclose(outfp);
    SqdClean();
#ifdef MEMDEBUG
    current_size = malloc_inuse(&histid2);
    if (current_size != orig_size)
	malloc_list(2, histid1, histid2);
    else
	fprintf(stderr, "[No memory leaks.]\n");
#endif

    ajStrDel(&instr);
    ajStrDel(&outstr);
    ajStrDel(&ajfmt);

    embExit();
  
    return 0;
}
