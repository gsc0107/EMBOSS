/* @source contactcount application
**
** Calculates the relative frequencies (the meaning of the term "relative
**  frequencies" is defined in various ways below in more detail) of
**  residue-residue contacts in domains from domainatrix contact map files 
**
** Outputs EMBOSS format matrix files
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
#include <math.h>

/* residue character constants */
#define ZERO_COUNTS 0
#define TOTAL_RES_TYPES 28
static const ajint cAjIntAsciiOffset   = -65;
static const ajint cAjIntAsterisk      =  91;
static const ajint cAjIntQuestionMark  =  92;

/* DDDDEBUG */
#define DEBUG_LEVEL 0

static AjPFeature write_count(AjPFeattable ajpFeattableCounts,
			      ajint ajIntFeatureResType,
			      ajint ajIntSpecificPairs,
			      ajint ajIntTotalPairs);

static AjBool write_scoring_matrix(AjPInt2d ajpInt2dCounts, AjPFile ajpFileScoringMatrix);

static ajint char_to_BLOSUM_index(char cResType);

static ajint BLOSUM_index_to_char(ajint ajIntBlosumIndex);



/* @prog contactcount ********************************************************
** 
** aggregate residue-specific contacts from contact files in local dir
**
******************************************************************************/

int main( int argc , char **argv )
{

    /* position counters and limits */
    ajint ajIntResTypeCurrent = 0;
    ajint ajIntContactNumber  = 0;
    ajint ajIntNumberOfContactFiles = 0;
    ajint ajIntRow    = 0;
    ajint ajIntColumn = 0;
    ajint ajIntRowMax    = 0;
    ajint ajIntColumnMax = 0;

    /* contact maps */
    AjPStr ajpStrCmapFileName   = NULL; /* name                      */
    AjPFile ajpFileCmapCurrent  = NULL; /* file                      */
    AjPList ajpListCmapFiles    = NULL; /* list of contact map files */
    AjPCmap ajpCmapCurrent      = NULL; /* object                    */
    AjPInt2d ajpInt2dContactMap = NULL; /* Cmap matrix               */
    AjPStr ajpStrChainId        = NULL; /* chain id attribute        */
    AjPStr ajpStrChainSeq       = NULL; /* seq string attribute      */
    AjPSeq ajpSeqChain          = NULL; /* seq AjPStr as seq object  */

    char cFirstResType       = '\0'; /* first residue letter in contact  */
    char cSecondResType      = '\0'; /* second residue letter in contact */
    ajint ajIntFirstResType  = 0;    /* first residue number in contact  */
    ajint ajIntSecondResType = 0;    /* second residue number in contact */

    /* count matrix */
    AjPStr ajpStrNaturalAlphabet   = NULL; /* ordered letters of Eng. lang */
    AjPStr ajpStrResTypeAlphabet   = NULL; /* ordered letters of residues  */
    AjPStr ajpStrFirstResType      = NULL;
    AjPStr ajpStrSecondResType     = NULL;
    AjIStr ajpStrIterFirstResType  = NULL;
    AjIStr ajpStrIterSecondResType = NULL;

    /* scoring matrix */
    ajint ajIntNumberOfResTypes;
    AjPInt2d ajpInt2dCounts = NULL;
    ajint ajIntCount = 0;
    AjPFile ajpFile2dScoringMatrix = NULL;

    /* number of contacts between first and second residue types */
    ajint ajIntSpecificPairs = 0;
    /* total number of contacts of first residue type */
    ajint ajIntTotalPairs = 0;

    /* report objects */
    AjPReport ajpReportCounts       = NULL;
    AjPStr ajpStrReportHead         = NULL;
    AjPFeattable ajpFeattableCounts = NULL;
    AjPFeature ajpFeatCurrent       = NULL;
    AjPStr ajpStrReportTail         = NULL; /* DDDDEBUGGING ONLY */

    embInit( "contactcount", argc, argv );

    /* get contact file directory from ACD */
    ajpListCmapFiles = ajAcdGetDirlist("cmapdir");

    /* get test output file from ACD */
    ajpFile2dScoringMatrix = ajAcdGetOutfile("dummyfile");

    /* get contact count output file from ACD */
    ajpReportCounts = ajAcdGetReport("outfile");

    ajIntNumberOfContactFiles = ajListLength(ajpListCmapFiles);

    /* create a 2-D array (count array) to store the scores */
    ajpStrNaturalAlphabet = ajStrNewC("ABCDEFGHIJKLMNOPQRSTUVWZYZ");
    ajpStrResTypeAlphabet = ajStrNewC("ARNDCQEGHILKMFPSTWYVBZX*");

    ajIntNumberOfResTypes = ajStrLen(ajpStrNaturalAlphabet);
    ajpInt2dCounts = ajInt2dNewL(ajIntNumberOfResTypes);

    /* empty count array */
    for(ajIntRow = 0;ajIntRow < ajIntNumberOfResTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < ajIntNumberOfResTypes;ajIntColumn++)
	{
	    ajInt2dPut(&ajpInt2dCounts, ajIntRow, ajIntColumn, ZERO_COUNTS);
	}
    }

    /* reserve memory for objects in loop */
    ajpSeqChain = ajSeqNewStr(ajpStrChainSeq);
    ajpStrReportHead = ajStrNew();
    ajpFeattableCounts = ajFeattableNewSeq(ajpSeqChain);

    /* loop over contact map files in current directory */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpStrCmapFileName))
    {
	ajpFileCmapCurrent = ajFileNewIn(ajpStrCmapFileName);

	if(!ajpFileCmapCurrent)
	    ajDie("cannot open file %S",ajpStrCmapFileName);

	/* get Cmap object from current Cmap file */
	ajXyzCmapReadI(ajpFileCmapCurrent, 1, 1, &ajpCmapCurrent);

	/* obtain chain name from Cmap object */
	ajpStrChainId = ajpCmapCurrent->Id;

	ajFmtPrint("\n%S\n\n", ajpStrChainId);

	/* obtain chain sequence from Cmap object */
	ajpStrChainSeq = ajpCmapCurrent->Seq;

	ajFmtPrint("\n%S\n\n", ajpStrChainSeq);

	/* obtain sequence object from string */
	ajpStrChainSeq = ajpCmapCurrent->Seq;

	/* obtain contact matrix from Cmap object */
	ajpInt2dContactMap = ajpCmapCurrent->Mat;

	/* get dimensions of contact map */
	ajInt2dLen(ajpInt2dContactMap, &ajIntRowMax, &ajIntColumnMax);

	/* loop through all cells in map */
	for(ajIntRow = 0;ajIntRow < ajIntRowMax;ajIntRow++)
	{
	    cFirstResType = ajStrChar(ajpStrChainSeq, ajIntRow);
	    for(ajIntColumn = 0;ajIntColumn < ajIntColumnMax;ajIntColumn++)
	    {
		cSecondResType = ajStrChar(ajpStrChainSeq, ajIntColumn);
		ajIntFirstResType = char_to_BLOSUM_index(cFirstResType);
		ajIntSecondResType = char_to_BLOSUM_index(cSecondResType);
		if((ajIntFirstResType < 0) || (ajIntFirstResType > 27))
		{
		    ajFmtPrint("\n ======================\n");
		    ajFmtPrint("\n OUT OF RANGE. FIRST RESIDUE = %d", ajIntFirstResType);
		}
		else if((ajIntSecondResType < 0) || (ajIntSecondResType > 27))
		{
		    ajFmtPrint("\n ======================\n");
		    ajFmtPrint("\n OUT OF RANGE. SECOND RESIDUE = %d", ajIntSecondResType);
		}
		else if(ajInt2dGet(ajpInt2dContactMap, ajIntRow, ajIntColumn))
		{
		    ajIntCount = ajInt2dGet(ajpInt2dCounts,
					    ajIntFirstResType,
					    ajIntSecondResType);
		    ajInt2dPut(&ajpInt2dCounts,
			       ajIntFirstResType,
			       ajIntSecondResType,
			       ajIntCount+1);
		}	    
	    }
	}

	ajIntCount = ajInt2dGet(ajpInt2dCounts, 6, 11);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS FIRST:\t%d\n", ajIntCount);
	ajFmtPrint("=======================\n");
	ajIntCount = ajInt2dGet(ajpInt2dCounts, 11, 6);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS AGAIN:\t%d\n", ajIntCount);
	ajFmtPrint("=======================\n");

	/* get dimensions of count array */
	ajInt2dLen(ajpInt2dCounts, &ajIntRowMax, &ajIntColumnMax);

	/* info about chain for head of report */
	ajFmtPrintS(&ajpStrReportHead, "Chain: %S", (ajpStrChainId));
	ajReportSetHeader(ajpReportCounts, ajpStrReportHead);

	/* DDDDEBUG: DUMMY VALUE FOR CONTACT NUMBER */
	ajIntContactNumber= ajSeqLen(ajpSeqChain)-5;

	/* DDDDEBUGGING */
	ajIntSpecificPairs = 4;
	ajIntTotalPairs = 7;

	ajpFeatCurrent = write_count(ajpFeattableCounts,
				     ajIntContactNumber,
				     ajIntSpecificPairs,
				     ajIntTotalPairs);

	/* write the report to the output file */
	ajReportWrite(ajpReportCounts,
		      ajpFeattableCounts,
		      ajpSeqChain);

	/* close the input file */
	ajFileClose(&ajpFileCmapCurrent);
    }

    /* END LOOP OVER CONTACT MAP FILES IN CURRENT DIRECTORY */

    /* DDDDEBUGGING */
    if(DEBUG_LEVEL)
    {
	ajFmtPrint("AFTER AND OUTSIDE LOOP: ajIntNumberOfResTypes:\t%d\n",
		   ajIntNumberOfResTypes);
    }
    
    
    /* read elements in count array */
    for(ajIntRow = 0;ajIntRow < ajIntNumberOfResTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < ajIntNumberOfResTypes;ajIntColumn++)
	{
	    cFirstResType = BLOSUM_index_to_char(ajIntRow);
	    cSecondResType = BLOSUM_index_to_char(ajIntColumn);
	    ajIntCount = ajInt2dGet(ajpInt2dCounts, ajIntRow, ajIntColumn);
	    ajFmtPrint("%d\t", ajIntCount);
	}
	ajFmtPrint("\n");
    }
    
    /* write scores to scoring matrix data file */
    write_scoring_matrix(ajpInt2dCounts, ajpFile2dScoringMatrix);

    /* DDDDEBUG TEST INFO FOR TAIL OF REPORT */
    /*     ajFmtPrintS(&ajpStrReportTail, "This is some tail text"); */
    /*     ajReportSetTail(ajpReportCounts, ajpStrReportTail); */

    /* clear up report objects */
    ajStrDel(&ajpStrReportHead);
    ajFeatDel(&ajpFeatCurrent);
    ajFeattableDel(&ajpFeattableCounts);
    ajReportDel(&ajpReportCounts);

    /*
     * clear up the contact map objects,
     * starting with the file objects
     */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpFileCmapCurrent))
	AJFREE(ajpFileCmapCurrent);
    ajListFree(&ajpListCmapFiles);
    ajSeqDel(&ajpSeqChain);
    ajInt2dDel(&ajpInt2dCounts);

    /* tidy up scoring output objects */
    ajStrIterFree(&ajpStrIterFirstResType);
    ajStrIterFree(&ajpStrIterSecondResType);
    ajStrDel(&ajpStrFirstResType);
    ajStrDel(&ajpStrSecondResType);
    ajStrDel(&ajpStrNaturalAlphabet);
    ajStrDel(&ajpStrResTypeAlphabet);    
    
    ajFileOutClose(&ajpFile2dScoringMatrix);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}




/* @funcstatic write_count *************************************************
**
** writes frequency features to a feature table and returns new feature  
**
** @param [r] ajpFeattableCounts [AjPFeattable] table to write frequency to
** @param [r] ajIntFeatureResType [ajint] residue type that a count
**                                            belongs to
** @param [r] ajIntSpecificPairs [ajint] contacts specific to both residues
** @param [r] ajIntTotalPairs [ajint] contacts specific to one residue
** @return [AjPFeature] New feature added to feature table
** @@
******************************************************************************/

static AjPFeature write_count(AjPFeattable ajpFeattableCounts,
			      ajint ajIntFeatureResType,
			      ajint ajIntSpecificPairs,
			      ajint ajIntTotalPairs)
{
    AjPFeature ajpFeatCounts = NULL;
    AjPStr ajpStrFeatTemp = NULL;

    ajpStrFeatTemp = ajStrNew();

    /* create feature for count and write per residue and per type frequency */
    ajpFeatCounts = ajFeatNewII(ajpFeattableCounts,
				ajIntFeatureResType,
				ajIntFeatureResType);
    ajFmtPrintS(&ajpStrFeatTemp, "*count1 %d", ajIntSpecificPairs);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);
    ajFmtPrintS(&ajpStrFeatTemp, "*count2 %d", ajIntTotalPairs);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);

    ajStrDel(&ajpStrFeatTemp);
    
    return ajpFeatCounts;
}



/* @funcstatic write_scoring_matrix ******************************************
**
** writes normalized substitution scores to a scoring matrix file format
**
** @param [r] ajpFileScoringMatrix [AjPFile] file to write scoring table to
** @param [r] ajpInt2dScoringMatrix [AjPInt2d] 2-D matrix of signed integer scores
** @return [AjBool] AJTRUE for success
** @@
******************************************************************************/

static AjBool write_scoring_matrix(AjPInt2d ajpInt2dCounts,
				   AjPFile ajpFileScoringMatrix)
{
    ajint ajIntRow    = 0;
    ajint ajIntColumn = 0;
    ajint ajIntCount  = 0;
    char cRowResType    = '\0';
    char cColumnResType = '\0';
    AjBool ajBoolSuccess = AJFALSE;
    AjPStr ajpStrResTypeAlphabet     = NULL; /* alphabet strings */
    AjPStr ajpStrResTypeAlphabetCopy = NULL;
    AjIStr ajpStrIterRowResType      = NULL;
    AjIStr ajpStrIterColumnResType   = NULL;

    ajIntRow = 0;
    ajIntColumn =0;

    /* write scores to data file */
    ajpStrResTypeAlphabet = ajStrNewC("ARNDCQEGHILKMFPSTWYVBZX*");
    ajpStrResTypeAlphabetCopy = ajStrDup(ajpStrResTypeAlphabet);
    ajpStrIterRowResType  = ajStrIter(ajpStrResTypeAlphabet);
    
    /* iterate over string of BLOSUM residue types */
    do
    {
	cRowResType = ajStrIterGetK(ajpStrIterRowResType);
	ajIntRow = char_to_BLOSUM_index(cRowResType);
	ajpStrIterColumnResType = ajStrIter(ajpStrResTypeAlphabetCopy);

	/* iterate over string of BLOSUM residue types */
	ajFmtPrintF(ajpFileScoringMatrix, "%c ", cRowResType);
	do
	{
	    cColumnResType = ajStrIterGetK(ajpStrIterColumnResType);
	    ajIntColumn = char_to_BLOSUM_index(cColumnResType);
	    ajIntCount = ajInt2dGet(ajpInt2dCounts, ajIntRow, ajIntColumn);
	    /* DDDDEBUG */
	    ajFmtPrint("(%4c, %4c) ", cRowResType, cColumnResType);
	    ajFmtPrint("(%4d, %4d) :\t", ajIntRow, ajIntColumn);
	    ajFmtPrint("%4d", ajIntCount);
	    ajFmtPrintF(ajpFileScoringMatrix, "%4d ", ajIntCount);
	}
	while(ajStrIterNext(ajpStrIterColumnResType));
	ajFmtPrintF(ajpFileScoringMatrix, "\n");
	ajFmtPrint("\n");
	/* KKKK kludge to rewind string iterator */
	ajStrIterFree(&ajpStrIterColumnResType);
    }    
    while(ajStrIterNext(ajpStrIterRowResType));

    ajStrDel(&ajpStrResTypeAlphabet);
    ajStrIterFree(&ajpStrIterRowResType);
  
    return ajBoolSuccess;
}




/* @funcstatic char_to_BLOSUM_index ******************************************
**
** converts residue type character to BLOSUM index
**
** @param [r] cResType [char] original single-letter residue type
** @return [ajint] index to corresponding character in data dir scoring matrix
** @@
******************************************************************************/

static ajint char_to_BLOSUM_index(char cResType)
{
    ajint ajIntBlosumIndex;

    /* look up array for residue type order
     *  found in EBLOSUM data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const ajint ajIntArrayLookUp[TOTAL_RES_TYPES] =
	{0,20,4,3,6,13,7,8,9,24,11,10,12,2,25,14,5,1,15,16,26,19,17,22,18,21,23,27};
    if (cResType == '*')
	cResType = cAjIntAsterisk;
    if (cResType == '?')
	cResType = cAjIntQuestionMark;
  
    cResType = cResType + cAjIntAsciiOffset;
    
    /* default to out-of-range value */
    if ((cResType < 0) || (cResType > 27))
	ajIntBlosumIndex = 28;
    else
	ajIntBlosumIndex = ajIntArrayLookUp[(ajint) cResType];

    return ajIntBlosumIndex;
}



/* @funcstatic BLOSUM_index_to_char ******************************************
**
** converts BLOSUM index number to char
**
** @param [r] cResType [char] index to residue type in data dir scoring matrix
** @return [char]  original single-letter residue type
** @@
******************************************************************************/

static ajint BLOSUM_index_to_char(ajint ajIntBlosumIndex)
{
    char cResType;

    /* look up array for residue type order
     *  found in EBLOSUM data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const char cArrayLookUp[TOTAL_RES_TYPES] =
	{'A','R','N','D','C','Q','E','G','H','I','L','K','M','F','P','S','T','W','Y','V','B','Z','X','*','J','O','U','?'};
  
    /* XXXX SHOULD THIS BE AN OFFICIAL AMBIGUITY CODE INSTEAD? */
    /* default to out-of-range value */
    if ((cResType < 0) || (cResType > 27))
	cResType = '?';
    else
	cResType = cArrayLookUp[ajIntBlosumIndex];

    return cResType;
}
