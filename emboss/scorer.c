/* @source scorer application
**
** Takes a structural alignment between a "query" (first) sequence and a
**  "template" sequence, substitutes corresponding residues from the query
**  into the template and compares this "gold standard" substituted alignment
**  with that generated by another (sequence-based) alignment method
**
** Outputs plain text files containing basic accuracy scores
**
** @author: Copyright (C) Damian Counsell
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
******************************************************************************/

#include "emboss.h"

enum constant
    {
	enumDebugLevel        =  0,
	enumQuerySeqIndex     =  0,
	enumTemplateSeqIndex  =  1,
	enumTotalResTypes     = 28,
	enumZeroCounts        =  0,
    };


/* @prog scorer.c ************************************************************
** 
** compare alignments with gold standard and score
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* position counters and limits */
    ajint ajIntCount;
    ajint ajIntResTypeCurrent = 0;
    ajint ajIntPosition  = 0;
    ajint ajIntNumberOfAlignmentFiles = 0;

    /* alignments */
    AjPStr ajpStrAlignmentFileName      = NULL; /* name                    */
    AjPFile ajpFileAlignmentCurrent     = NULL; /* file                    */
    AjPList ajpListAlignmentFiles       = NULL; /* list of alignment files */
    AjPSeqset ajpSeqsetAlignmentCurrent = NULL; /* object                  */
    const AjPSeq ajpSeqQuery;                   /* first sequence in alignment pair */
    const AjPSeq ajpSeqTemplate;                /* second sequence in pair          */
    const AjPSeq ajpSeqGold             = NULL; /* ideal substituted sequence       */
    const AjPSeq ajpSeqScored           = NULL; /* substituted sequence for testing */

    char cGoldResType        = '\0'; /* residue letter in gold standard substituted sequence */
    char cScoredResType      = '\0'; /* residue letter in tested substituted sequence */

    AjPStr ajpStrResTypeAlphabet   = NULL; /* ordered letters of residues */

    /* sequence string iterators */
    AjIStr ajpStrIterGoldResType   = NULL; /* to iterate through gold standard sequence */
    AjIStr ajpStrIterScoredResType = NULL; /* to iterate through scored sequence */

    /* report objects */
    AjPReport ajpReportScores       = NULL;
    AjPStr ajpStrReportHead         = NULL;
    AjPFeattable ajpFeattableScores = NULL;
    AjPFeature ajpFeatCurrent       = NULL;
    AjPStr ajpStrReportTail         = NULL; /* DDDDEBUGGING ONLY */

    /* plain output file (as opposed to report format o/p file) */
    AjPFile ajpFileScores = NULL;

    /* DDDDEBUG for testing sequence reading */
    AjPSeqout ajpSeqoutAligned = NULL; /* output object to write alignment */

    embInit( "scorer", argc, argv );

    /* get test output file from ACD */
    ajpFileScores = ajAcdGetOutfile("dummyfile");

    /* get score report output file from ACD */
    ajpReportScores = ajAcdGetReport("outfile");

    /* DDDDEBUGGING open file for temporary check writes of sequences */
    ajpSeqoutAligned  = ajAcdGetSeqout("aligned");

    ajIntNumberOfAlignmentFiles = ajListLength(ajpListAlignmentFiles);

    /* reserve memory for objects in loop */
    ajpStrReportHead = ajStrNew();

    /* loop over contact map files in current directory */
    /*     while(ajListPop(ajpListAlignmentFiles, (void **)&ajpStrAlignmentFileName)) */
    /* DDDDEBUG */
    for(ajIntCount = 0;ajIntCount < 10;ajIntCount++)
    {
	/* DDDDEBUG */
	ajListPop(ajpListAlignmentFiles, (void **)&ajpStrAlignmentFileName);
	ajFmtPrint( "%S\n", ajpStrAlignmentFileName);
	
	ajpFileAlignmentCurrent = ajFileNewIn(ajpStrAlignmentFileName);

	if(!ajpFileAlignmentCurrent)
	    ajDie("cannot open file %S",ajpStrAlignmentFileName);

	/* reserve memory for new seqset */
	ajpSeqsetAlignmentCurrent = ajSeqsetNew();

	/* get sequence objects from current alignment file */
	ajSeqsetGetFromUsa(ajpStrAlignmentFileName,
			   &ajpSeqsetAlignmentCurrent);

	/* if there aren't two sequences in alignment, bail */ 
	if(ajpSeqsetAlignmentCurrent->Size !=2)
	    ajFatal("alignment does not contain pair of alignments!\n");

	/* read current seqset into sequences */
	ajpSeqQuery = ajSeqsetGetSeq(ajpSeqsetAlignmentCurrent,
				     enumQuerySeqIndex);
	ajpSeqTemplate = ajSeqsetGetSeq(ajpSeqsetAlignmentCurrent,
					enumTemplateSeqIndex);

	/* write out "aligned" sequences */
	ajSeqWrite(ajpSeqoutAligned, ajpSeqQuery);
	ajSeqWrite(ajpSeqoutAligned, ajpSeqTemplate);

	/* clean up seqset */
	ajSeqsetDel(&ajpSeqsetAlignmentCurrent);

	/* close current file */
	ajFileClose(&ajpFileAlignmentCurrent);
    }

    /*
     * clear up the alignment objects,
     * starting with the file objects
     */
    while(ajListPop(ajpListAlignmentFiles,
		    (void **)&ajpFileAlignmentCurrent))
	AJFREE(ajpFileAlignmentCurrent);
    ajListFree(&ajpListAlignmentFiles);

    /* tidy up scoring output objects */
    ajStrIterFree(&ajpStrIterGoldResType);
    ajStrIterFree(&ajpStrIterScoredResType);
    ajStrDel(&ajpStrReportHead);
    ajStrDel(&ajpStrResTypeAlphabet);
    
    ajFileClose(&ajpFileScores);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}







