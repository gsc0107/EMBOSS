/* @source alignrunner application
**
** Runs EMBOSS alignment programs on all sequence pairs in given directory and
**  writes the resulting alignments into a new directory
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.1 $
** @modified $Date: 2004/07/13 15:50:03 $
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
	enumCountZero         =  0,
    };


/* @prog alignrunner.c *******************************************************
** 
** run alignment programs on a directory full of sequence pairs 
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* position counters and limits */
    ajint ajIntCount;
    ajint ajIntNumberOfSeqPairFiles  = enumCountZero;
    AjPSeqin ajpSeqinUnalignedPair   = NULL;
    float fExtensionPenalty;
    float fGapPenalty;
    const AjPSeq ajpSeqQuery         = NULL; /* first unaligned sequence in file as string    */
    const AjPSeq ajpSeqTemplate      = NULL; /* second unaligned sequence in file as string   */
    const AjPStr ajpStrQuerySeq      = NULL; /* first unaligned sequence in file as object    */
    const AjPStr ajpStrTemplateSeq   = NULL; /* second unaligned sequence in file as object   */
    const char *pcFirstCommandLine   = NULL;
    const char *pcSecondCommandLine  = NULL;

    /* alignments */
    AjPStr ajpStrSeqPairFileName     = NULL; /* name of...  */
    AjPFile ajpFileSeqPair           = NULL; /*
					      * ...file containing current
                                              * pair of unaligned input
                                              * sequences
                                              */
    AjPList ajpListSeqPairFiles      = NULL; /* list of alignment files  */
    AjPSeqset ajpSeqsetUnalignedPair = NULL; /* object holding both seqs */

    /* alignment parameters */
    AjPStr ajpStrPathToCommand        = NULL;
    AjPStr ajpStrFirstCommandName     = NULL;
    AjPStr ajpStrSecondCommandName    = NULL;
    AjPStr ajpStrPathToScoringMatrix  = NULL;
    AjPStr ajpStrScoringMatrix        = NULL;
    AjPStr ajpStrFirstPathToOutfile   = NULL; /* directories for aligned files... */
    AjPStr ajpStrSecondPathToOutfile  = NULL; /*   ...from two alignment progs    */
    AjPStr ajpStrFirstOutfileName     = NULL; /* full name of output file of aligned sequences */
    AjPStr ajpStrSecondOutfileName    = NULL; /* full name of output file of aligned sequences */
    AjPStr ajpStrFirstOutfileSuffix   = NULL; /* filename extension for alignment file         */
    AjPStr ajpStrSecondOutfileSuffix  = NULL; /* filename extension for alignment file         */
    AjPStr ajpStrAlignmentFormat      = NULL; /* format for output alignment file              */
    AjPStr ajpStrFirstCommandLine     = NULL; /* command line given to alignment program       */
    AjPStr ajpStrSecondCommandLine    = NULL; /* command line given to alignment program       */

    /* DDDDEBUGGING: TEMPORARY PARAMETER VALUES BELOW */
    ajStrAssC(&ajpStrPathToCommand,
	      "/users/damian/EMBOSS/emboss/emboss/emboss");
    ajStrAssC(&ajpStrFirstCommandName,
	      "needle");
    ajStrAssC(&ajpStrSecondCommandName,
	      "contactalign");
    ajStrAssC(&ajpStrPathToScoringMatrix,
	      "/users/damian/EMBOSS/emboss/emboss/emboss/data");
    ajStrAssC(&ajpStrScoringMatrix,
	      "EBLOSUM62");
    fExtensionPenalty = 0.5;
    fGapPenalty = 10.0;
    ajStrAssC(&ajpStrFirstPathToOutfile,
	      "/users/damian/EMBOSS/emboss/emboss/emboss/contacttest/test_output_alignments/needle");
    ajStrAssC(&ajpStrSecondPathToOutfile,
	      "/users/damian/EMBOSS/emboss/emboss/emboss/contacttest/test_output_alignments/contactalign");
    ajStrAssC(&ajpStrFirstOutfileSuffix,
	      ".needle");
    ajStrAssC(&ajpStrSecondOutfileSuffix,
	      ".contactalign");
    ajStrAssC(&ajpStrAlignmentFormat,
	      "fasta");

    embInit( "alignrunner", argc, argv );

    /*
     * get list unaligned files containing unaligned sequences
     * and directory of aligned sequences directory from ACD
     */
    ajpListSeqPairFiles = ajAcdGetDirlist("seqpairsdir");

    /* count the number of sequence pairs to be aligned */
    ajIntNumberOfSeqPairFiles = ajListLength(ajpListSeqPairFiles);

    /* new string for current input file name */
    ajpStrSeqPairFileName  = ajStrNew();

    /* loop over sequence files in current directory */
    /* new sequence input object */
    ajpSeqinUnalignedPair = ajSeqinNew();
    /* new seqset to store input unaligned sequences */
    ajpSeqsetUnalignedPair = ajSeqsetNew();
    /* new string for alignment command and options to it */
    ajpStrFirstCommandLine = ajStrNew();
    ajpStrSecondCommandLine = ajStrNew();
    ajpSeqQuery = ajSeqNew();    
    ajpSeqTemplate = ajSeqNew();
    ajpStrQuerySeq = ajStrNew();    
    ajpStrTemplateSeq = ajStrNew();
    
    /*     while(ajListPop(ajpListSeqPairFiles, (void **)&ajpStrSeqPairFileName)) */
    /* DDDDEBUG */
    for(ajIntCount = 0;ajIntCount < 10;ajIntCount++)
    {
	/* DDDDEBUG */
	ajListPop(ajpListSeqPairFiles, (void **)&ajpStrSeqPairFileName);
	ajFmtPrint( "%S\n", ajpStrSeqPairFileName);

	ajStrCopy(&ajpStrFirstOutfileName, ajpStrSeqPairFileName);
	ajStrCopy(&ajpStrSecondOutfileName, ajpStrSeqPairFileName);
	ajFileNameTrim(&ajpStrFirstOutfileName);
	ajFileNameTrim(&ajpStrSecondOutfileName);
	ajFileNameShorten(&ajpStrFirstOutfileName);
	ajFileNameShorten(&ajpStrSecondOutfileName);
	ajStrApp(&ajpStrFirstOutfileName, ajpStrFirstOutfileSuffix);
	ajStrApp(&ajpStrSecondOutfileName, ajpStrSecondOutfileSuffix);

	/* read two unaligned sequences into memory */
	ajSeqinUsa(&ajpSeqinUnalignedPair, ajpStrSeqPairFileName);
	ajSeqsetRead(ajpSeqsetUnalignedPair, ajpSeqinUnalignedPair);

	ajpSeqQuery = ajSeqsetGetSeq(ajpSeqsetUnalignedPair, enumQuerySeqIndex);
	ajpSeqTemplate = ajSeqsetGetSeq(ajpSeqsetUnalignedPair, enumQuerySeqIndex);

	ajpStrQuerySeq = ajSeqStr(ajpSeqQuery);
	ajpStrTemplateSeq = ajSeqStr(ajpSeqTemplate);

	/* obtain sequences as modifiable strings */
	ajFmtPrintS(&ajpStrFirstCommandLine,
		    "%S/%S -asequence asis::%S  -bsequence asis::%S -datafile %S/%S -gapopen %2.1f -gapextend %2.1f -outfile %S/%S -aformat3 %S",
		    ajpStrPathToCommand, ajpStrFirstCommandName,
		    ajpStrQuerySeq, ajpStrTemplateSeq,
		    ajpStrPathToScoringMatrix, ajpStrScoringMatrix,
		    fGapPenalty, fExtensionPenalty,
		    ajpStrFirstPathToOutfile, ajpStrFirstOutfileName,
		    ajpStrAlignmentFormat);

	ajFmtPrintS(&ajpStrSecondCommandLine,
		    "%S/%S -down asis::%S -across asis::%S -substitution %S/%S -gapo %2.1f -gape %2.1f -aligned %S/%S",
		    ajpStrPathToCommand, ajpStrSecondCommandName,
		    ajpStrQuerySeq, ajpStrTemplateSeq,
		    ajpStrPathToScoringMatrix, ajpStrScoringMatrix,
		    fGapPenalty, fExtensionPenalty,
		    ajpStrSecondPathToOutfile, ajpStrSecondOutfileName);

	pcFirstCommandLine = ajStrStr(ajpStrFirstCommandLine);
	pcSecondCommandLine = ajStrStr(ajpStrSecondCommandLine);
	system(pcFirstCommandLine);
	system(pcSecondCommandLine);
	printf("\n%s", pcFirstCommandLine);
 	printf("\n%s", pcSecondCommandLine);
   }

    /*
     * clear up the alignment objects,
     * starting with the file objects
     */
    while(ajListPop(ajpListSeqPairFiles,
		    (void **)&ajpFileSeqPair))
	AJFREE(ajpFileSeqPair);
    ajListFree(&ajpListSeqPairFiles);

    ajStrDel(&ajpStrSeqPairFileName);
    ajSeqinDel(&ajpSeqinUnalignedPair);
    ajSeqsetDel(&ajpSeqsetUnalignedPair);
    ajStrDel(&ajpStrFirstCommandLine);
    ajStrDel(&ajpStrSecondCommandLine);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}







