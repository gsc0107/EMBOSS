/* @source contactalign application
**
** aligns two sequences: seqDownSequence and seqAcrossSequence
**  (eventually using a structure-weighted alignment method)---at the
**   moment implementing Needleman and Wunsch 
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.22 $
** @modified $Date: 2004/06/23 13:19:04 $
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

enum constant
    {
	enumDebugLevel        =  0,
	enumArrayOffset       =  1,
	enumTotalResTypes     = 30,
	enumAsciiOffset       = 65,
	enumMaxCmapLineLen    = 80
    };

#include "emboss.h"
#include <math.h>

/* @prog contactalign ********************************************************
** 
** align two protein sequence of unknown structure to protein sequence of
**  known structure, given residue contact information
**
******************************************************************************/

int main(int argc , char **argv)
{
    /* sequence objects */
    AjPSeq ajpSeqDown = NULL;   /* query sequence---no structure             */
    AjPSeq ajpSeqAcross = NULL; /* template sequence---has structure         */
    AjPSeq ajpSeqDownCopy = NULL;   /* rewritable query sequence             */
    AjPSeq ajpSeqAcrossCopy = NULL; /* rewritable template sequence          */
    /* sequence characters */
    char cTempTraceDown;
    char cTempTraceAcross;
    /* sequence strings */
    char *pcSeqAcross;
    char *pcTraceDown;
    char *pcTraceAcross;
    char *pcUpdatedSeqAcross;
    /* sequence lengths */
    ajint ajIntSeqCount;
    ajint ajIntTraceCount;
    ajint ajIntDownSeqLen;
    ajint ajIntAcrossSeqLen;
    ajint ajIntAlignmentLen;
    
    AjPSeqout ajpSeqoutAligned = NULL; /* output object to write alignment   */

    /* alignment gap extension and gap penalty scores */
    float fExtensionPenalty;
    float fGapPenalty;

    /* prebuilt scoring matrix (e.g. BLOSUM62) */
    AjPMatrixf ajpMatrixfSubstitutionScoring = NULL;
    /* contact-based scoring matrix */
    AjPMatrixf ajpMatrixfContactScoring = NULL;
    /* array for pair scores according to conventional scoring matrix  */
    AjPFloat2d ajpFloat2dPairScores = NULL;
    /* array for pair scores according to contact-based scoring matrix */
    float **floatArrayContactScores = NULL;

    /* array for recursively summed scores in alignment backtrace */
    AjPGotohCell const **ajpGotohCellGotohScores;
    /* stack of backtraced cells */
    AjPList ajpListGotohCellsMaxScoringTrace = NULL;

    /* DDDDEBUG input contact map filename */
    AjPStr ajpStrOriginalCmapFile = NULL;
    AjPFile ajpFileOriginalCmap = NULL;
    /* DDDDEBUG output contact map filename */
    AjPStr ajpStrUpdatedCmapFile = NULL;
    AjPFile ajpFileUpdatedCmap = NULL;
    
    /* contact map components */
    AjPCmapHeader ajpCmapHeader = NULL;
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* has contact map been read successfully? */
    AjBool ajBoolOriginalCmapFileRead = ajFalse;
    /* has contact map been written successfully? */
    AjBool ajBoolUpdatedCmapFileWritten = ajFalse;

    embInit("contactalign", argc, argv);
  
    /* DDDDEBUGGING: DEFAULT GAP AND EXTENSION PENALTIES SET BELOW */

    /* ajAcdGet functions access acd values embInit has put in memory */
    ajpSeqDown   = ajAcdGetSeq("down");
    ajpSeqAcross = ajAcdGetSeq("across");
    ajpMatrixfSubstitutionScoring = ajAcdGetMatrixf("substitutionscoringfile");
    ajpMatrixfContactScoring = ajAcdGetMatrixf("contactscoringfile");
    ajpSeqoutAligned  = ajAcdGetSeqout("aligned");
/*     fGapPenalty       = -10.0; */
/*     fExtensionPenalty =  -0.5; */
    fGapPenalty       = -ajAcdGetFloat("gapopen");
    fExtensionPenalty =  -ajAcdGetFloat("gapextend");

    /* get sequence lengths: no. rows and columns in Gotoh sum array */
    ajIntDownSeqLen = ajSeqLen(ajpSeqDown);
    ajIntAcrossSeqLen = ajSeqLen(ajpSeqAcross);

    /* score the sequences to produce a scoring AjpMatrix */
    ajpFloat2dPairScores = embGotohPairScore(ajpMatrixfSubstitutionScoring,
					     ajpSeqDown,
					     ajpSeqAcross,
					     fExtensionPenalty);
    
    /* initialize Gotoh score array */
    ajpGotohCellGotohScores = embGotohCellGetArray(ajIntDownSeqLen,
						   ajIntAcrossSeqLen);

    embGotohCellCalculateSumScore(ajpFloat2dPairScores,
				  ajpSeqDown,
				  ajpSeqAcross,
				  ajpGotohCellGotohScores,
				  fGapPenalty,
				  fExtensionPenalty);
    
    /* copy original sequences into new sequence objects */
    ajpSeqDownCopy = ajSeqNewS(ajpSeqDown);
    ajpSeqAcrossCopy = ajSeqNewS(ajpSeqAcross);

    /* pile up the backtraced elements onto the stack */
    ajpListGotohCellsMaxScoringTrace = ajListNew();
    ajIntAlignmentLen = embGotohCellBacktrace(ajpGotohCellGotohScores,
					      ajpSeqDownCopy,
					      ajpSeqAcrossCopy,
					      ajpListGotohCellsMaxScoringTrace);
    
    /* read the backtraced elements off the stack */
    embGotohReadOffBacktrace(ajpListGotohCellsMaxScoringTrace,
			     ajpSeqDownCopy,
			     ajpSeqAcrossCopy);

    /* substitute all matches into a new version of the template sequence */
    pcUpdatedSeqAcross = ajSeqCharCopy(ajpSeqAcross);
    pcTraceDown = ajSeqCharCopy(ajpSeqDownCopy);
    pcTraceAcross = ajSeqCharCopy(ajpSeqAcrossCopy);

    ajIntSeqCount = 0;
    
    for(ajIntTraceCount = 0;ajIntTraceCount < ajIntAlignmentLen; ajIntTraceCount++)
    {
	cTempTraceDown = pcTraceDown[ajIntTraceCount];
	cTempTraceAcross = pcTraceAcross[ajIntTraceCount];
	if(cTempTraceAcross == '-')
	{
	    ajIntTraceCount++;
	}
	else
	{
	    ajFmtPrint("%c\t", cTempTraceAcross);
	    ajFmtPrint("%c\t", cTempTraceDown);
	    if( cTempTraceDown != '-')
		pcUpdatedSeqAcross[ajIntSeqCount] = cTempTraceDown;
	    ajFmtPrint("%c\n", pcUpdatedSeqAcross[ajIntSeqCount]);
	}
    }

    /* XXXX EVERYTHING FROM HERE IS TO DO WITH ALIGNMENT MODIFICATION */

    /* DDDD DEBUG DUMMY FILENAME BELOW */
    ajpStrOriginalCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/conts/d1aj3__.con");
    ajpFileOriginalCmap = ajFileNewIn(ajpStrOriginalCmapFile);

    pcSeqAcross = ajSeqCharCopy(ajpSeqAcross);

    /* reserve memory for objects representing contact map */
    ajpInt2dCmapSummary = embGetCmapSummary(pcSeqAcross);
    ajpInt2dCmapResTypes = embGetIntMap(ajIntAcrossSeqLen +
				       enumArrayOffset);
    ajpInt2dCmapPositions = embGetIntMap(ajIntAcrossSeqLen +
					enumArrayOffset);
    ajpCmapHeader = ajCmapHeaderNew();

    /* DDDDEBUGGING */
    if( enumDebugLevel > 2 )
    {
	debug_cmap_summary(&ajpInt2dCmapSummary,
			   ajIntAcrossSeqLen);
    }
    if( enumDebugLevel > 2 )
    {
	debug_int_map(&ajpInt2dCmapSummary,
		      &ajpInt2dCmapResTypes,
		      &ajpInt2dCmapPositions,
		      pcSeqAcross,
		      ajIntAcrossSeqLen);
    }
    
    /* read in original contact map */ 
    ajBoolOriginalCmapFileRead = embReadCmapFile(ajpFileOriginalCmap,
						 ajIntAcrossSeqLen,
						 &ajpInt2dCmapSummary,
						 &ajpCmapHeader,
						 &ajpInt2dCmapResTypes,
						 &ajpInt2dCmapPositions);    
    
    ajFileClose(&ajpFileOriginalCmap);

    /* DDDDEBUGGING DID WE READ THE CMAP FILE? */
    if( enumDebugLevel > 2 )
    {
	debug_cmap_header(&ajpCmapHeader);
	
	debug_cmap_summary(&ajpInt2dCmapSummary,
			  ajIntAcrossSeqLen);
    }

    if( enumDebugLevel > 2 )
    {
	debug_int_map(&ajpInt2dCmapSummary,
		      &ajpInt2dCmapResTypes,
		      &ajpInt2dCmapPositions,
		      pcSeqAcross,
		      ajIntAcrossSeqLen);
    }
   
    /* DDDDEBUG: DUMMY FILENAME IS USED BELOW */
    ajpStrUpdatedCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/conts/test.con");
    ajpFileUpdatedCmap = ajFileNewOut(ajpStrUpdatedCmapFile);


    /* write contact arrays to a new contact map file */
    ajBoolUpdatedCmapFileWritten =
	embWriteCmapFile (ajpFileUpdatedCmap,
			  ajIntAcrossSeqLen,
			  &ajpInt2dCmapSummary,
			  &ajpCmapHeader,
			  &ajpInt2dCmapResTypes,
			  &ajpInt2dCmapPositions);

    /* XXXX LOOK UP PROBABILITY SCORE FOR PAIR */

    /* convert the input float AjpMatrix to a 2D array of scores */
    floatArrayContactScores = ajMatrixfArray(ajpMatrixfContactScoring);

    /* XXXX FIND CORRESPONDING FIRST AND SECOND RESIDUES IN ORIGINAL TEMPLATE SEQUENCE */


    /* XXXX FIND CORRESPONDING FIRST AND SECOND RESIDUES IN ORIGINAL QUERY SEQUENCE */

    /* XXXX COUNT REAL RESIDUES IN FIRST SEQUENCE */

    /* XXXX LOOK UP FIRST CONTACT RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN QUERY SEQUENCE */

    /* XXXX LOOK UP SECOND CONTACT RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN QUERY SEQUENCE */

    /* XXXX LOOK UP FIRST RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN SECOND SEQUENCE */

    /* XXXX LOOK UP PROBABILITY SCORE FOR NEW PAIR */

    /* XXXX FIND RELEVANT CELL IN ALIGNMENT MATRIX AND PENALIZE IT */    

    /* XXXX YOU NEED SOME CODE HERE TO COMPARE THE PROBABILITIES OF EACH PAIR */

    ajCmapHeaderDel(&ajpCmapHeader);

    /* write out "aligned" sequences  */
    ajSeqWrite(ajpSeqoutAligned, ajpSeqDownCopy);
    ajSeqWrite(ajpSeqoutAligned, ajpSeqAcrossCopy);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
