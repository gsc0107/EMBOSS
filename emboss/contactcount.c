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

/* DDDDEBUG */
#define DEBUG_LEVEL 0

/* [XXXX ajIntCount1, ajIntCount2 SHOULD BE REPLACED WITH MORE MEANINGFUL NAMES] */
static AjPFeature write_count(AjPFeattable ajpFeattableCounts,
			      ajint ajIntFeatureResType,
			      ajint ajIntCount1,
			      ajint ajIntCount2);




/* @prog contactcount ********************************************************
** 
** aggregate residue-specific contacts from contact files in local dir
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* DDDDEBUG: string for test prints */
    AjPStr ajpStrDebug = NULL;

    AjPStr ajpStrCmapFileName   = NULL;
    AjPList ajpListCmapFiles    = NULL; /* list of contact map files */

    /* position counters and limits */
    ajint ajIntResTypeCurrent = 0;
    ajint ajIntContactNumber      = 0;
    ajint ajIntNumberOfContactFiles = 0;
    ajint ajIntRow = 0;
    ajint ajIntColumn = 0;
    ajint ajIntRowMax = 0;
    ajint ajIntColumnMax = 0;

    /* contact map */
    AjPFile ajpFileCmapCurrent   = NULL; /* file                      */
    AjPCmap ajpCmapCurrent       = NULL; /* object                    */
    AjPInt2d ajpInt2dContactMap  = NULL; /* Cmap matrix               */
    AjPStr ajpStrChainId         = NULL; /* chain id attribute        */
    AjPStr ajpStrChainSeq        = NULL; /* seq string attribute      */
    AjPSeq ajpSeqChain           = NULL; /* seq AjPStr as seq object  */
    char cFirstResType       = '\0'; /* first residue in contact  */
    char cSecondResType      = '\0'; /* second residue in contact */
    ajint ajIntFirstResType  = 0;    /* first residue in contact  */
    ajint ajIntSecondResType = 0;    /* second residue in contact */

    /* count matrix */
    AjPStr ajStrNaturalAlphabet;
    AjPStr ajStrResTypeAlphabet;
    AjPStr ajStrFirstResType;
    AjPStr ajStrSecondResType;
    AjIStr ajStrIterFirstResType;
    AjIStr ajStrIterSecondResType;
    

    /* scoring matrix */
    ajint ajIntNumberOfResTypes = 0;
    AjPInt2d ajpInt2dCounts         = NULL;
    ajint ajIntTempCount            = 0;

    /* number of contacts between first and second residue types */
    ajint ajIntCount1 = 0;
    /* total number of contacts of first residue type */
    ajint ajIntCount2 = 0;

    /* output report file for counts */
    AjPReport ajpReportCounts = NULL;
    AjPStr ajpStrReportHeader = NULL;

    /* DDDDEBUG: string for report footer */
    AjPStr ajpStrReportTail          = NULL;
    AjPFeattable ajpFeattableCounts  = NULL;
    AjPFeature ajpFeatCurrent        = NULL;

    embInit( "contactcount", argc, argv );

    /* get contact file directory from ACD */
    ajpListCmapFiles = ajAcdGetDirlist("cmapdir");

    /* get contact count output file from ACD */
    ajpReportCounts = ajAcdGetReport("outfile");

    ajIntNumberOfContactFiles = ajListLength(ajpListCmapFiles);

    /* create a 2-D array (count array) to store the scores */
    ajStrNaturalAlphabet = ajStrNewC("ABCDEFGHIJKLMNOPQRSTUVWZYZ");
    ajIntNumberOfResTypes = ajStrLen(ajStrNaturalAlphabet);
    ajpInt2dCounts = ajInt2dNewL(ajIntNumberOfResTypes);

    /* empty count array */
    for(ajIntRow = 0;ajIntRow < ajIntNumberOfResTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < ajIntNumberOfResTypes;ajIntColumn++)
	{
	    ajInt2dPut(&ajpInt2dCounts, ajIntRow, ajIntColumn,0);
	}
    }

    /* BEGIN LOOP OVER CONTACT MAP FILES IN CURRENT DIRECTORY */
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
	ajpSeqChain = ajSeqNewStr(ajpStrChainSeq);

	/* obtain contact matrix from Cmap object */
	ajpInt2dContactMap = ajpCmapCurrent->Mat;

	/* get dimensions of contact map */
	ajInt2dLen(ajpInt2dContactMap, &ajIntRowMax, &ajIntColumnMax);

	/* loop through all cells in map */
	for(ajIntRow = 0;ajIntRow<ajIntRowMax;ajIntRow++)
	{
	    cFirstResType = ajStrChar(ajpStrChainSeq, ajIntRow);
	    for(ajIntColumn = 0;ajIntColumn < ajIntColumnMax;ajIntColumn++)
	    {
		cSecondResType = ajStrChar(ajpStrChainSeq, ajIntColumn);
		ajIntFirstResType = ajAZToInt(cFirstResType);
		ajIntSecondResType = ajAZToInt(cSecondResType);
		if(ajInt2dGet(ajpInt2dContactMap, ajIntRow, ajIntColumn))
		{
		    ajIntTempCount = ajInt2dGet(ajpInt2dCounts,
						ajIntFirstResType,
						ajIntSecondResType);
		    ajInt2dPut(&ajpInt2dCounts,
			       ajIntFirstResType,
			       ajIntSecondResType,
			       ajIntTempCount+1);
		}	    
	    }
	}

	ajIntTempCount = ajInt2dGet(ajpInt2dCounts, 6, 11);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS FIRST:\t%d\n", ajIntTempCount);
	ajFmtPrint("=======================\n");
	ajIntTempCount = ajInt2dGet(ajpInt2dCounts, 11, 6);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS AGAIN:\t%d\n", ajIntTempCount);
	ajFmtPrint("=======================\n");

	/* get dimensions of count array */
	ajInt2dLen(ajpInt2dCounts, &ajIntRowMax, &ajIntColumnMax);

	/* chain info for head of report */
	ajFmtPrintS(&ajpStrReportHeader, "Chain: %S", (ajpStrChainId));
	ajReportSetHeader(ajpReportCounts, ajpStrReportHeader);

	/* create feature table for count output */
	ajpFeattableCounts = ajFeattableNewSeq(ajpSeqChain);

	/* DDDDEBUG: DUMMY VALUE FOR CONTACT NUMBER */
	ajIntContactNumber= ajSeqLen(ajpSeqChain)-5;

	/* DDDDEBUGGING */
	ajIntCount1 = 4;
	ajIntCount2 = 7;

	ajpFeatCurrent = write_count(ajpFeattableCounts,
				     ajIntContactNumber,
				     ajIntCount1,
				     ajIntCount2);

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
	    cFirstResType = ajIntToAZ(ajIntRow);
	    cSecondResType = ajIntToAZ(ajIntColumn);
	    ajIntTempCount = ajInt2dGet(ajpInt2dCounts, ajIntRow, ajIntColumn);
	    ajFmtPrint("%d\t", ajIntTempCount);
	}
	ajFmtPrint("\n");
    }

    /* new string iterator over BLOSUM list of residue types */
    ajStrResTypeAlphabet = ajStrNewC("ARNDCQEGHILKMFPSTWYVBZX*");
    ajStrIterFirstResType = ajStrIter(ajStrResTypeAlphabet);

    ajIntRow = 0;
    ajIntColumn =0;
    
    /* write scores to data file */
    while(ajStrIterNext(ajStrIterFirstResType))
    {
	cFirstResType = ajStrIterGetK(ajStrIterFirstResType);
	ajIntRow = ajAZToInt(cFirstResType);
	ajStrIterSecondResType = ajStrIter(ajStrResTypeAlphabet);
	while(ajStrIterNext(ajStrIterSecondResType))
	{
	    cSecondResType = ajStrIterGetK(ajStrIterSecondResType);
	    ajIntColumn = ajAZToInt(cSecondResType);
	    /* DDDDEBUG */
	    ajFmtPrint("(%4c, %4c)-", cFirstResType, cSecondResType);
	    ajFmtPrint("(%4d, %4d) ", ajIntRow, ajIntColumn);
	}
	ajFmtPrint("\n");
	ajStrIterFree(&ajStrIterSecondResType);
    }

    /* DDDDEBUG TEST INFO FOR TAIL OF REPORT */
    /*     ajFmtPrintS(&ajpStrReportTail, "This is some tail text"); */
    /*     ajReportSetTail(ajpReportCounts, ajpStrReportTail); */


    /* close the report file */
    ajReportDel(&ajpReportCounts);

    /* clear up report objects */
    ajFeattableDel(&ajpFeattableCounts);
    
    /* clean up the list iterator */

    /* clean up the list of files */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpFileCmapCurrent))
	AJFREE(ajpFileCmapCurrent);
    ajListFree(&ajpListCmapFiles);
    
    /* clear up the contact map object */
    ajXyzCmapDel(&ajpCmapCurrent);
    
    /* clean up the derived objects */
    ajStrDel(&ajpStrChainId);
    ajSeqDel(&ajpSeqChain);
    ajStrDel(&ajpStrChainSeq);

    /* tidy up other stuff */
    ajStrIterFree(&ajStrIterFirstResType);
    ajStrIterFree(&ajStrIterSecondResType);
    ajStrDel(&ajStrFirstResType);
    ajStrDel(&ajStrSecondResType);
    
    /*  tidy up everything else... */
    ajExit();

    return 0;
}




/* @funcstatic write_count *************************************************
**
** writes frequency features to a feature table and returns new feature  
**
** @param [r] ajpFeattableCounts [AjPFeattable] table to write frequency to
** @param [r] ajIntFeatureResType [ajint] residue type that count
**                                            belongs to
** @param [r] ajIntCount1 [ajint] phi torsion angle for residue
** @param [r] ajIntCount2 [ajint] psi torsion angle for residue
** @return [AjPFeature] New feature added to feature table
** @@
******************************************************************************/

static AjPFeature write_count (AjPFeattable ajpFeattableCounts,
			       ajint ajIntFeatureResType,
			       ajint ajIntCount1,
			       ajint ajIntCount2)
{
    AjPFeature ajpFeatCounts;
    AjPStr ajpStrFeatTemp;

    ajpStrFeatTemp = ajStrNew();

    /* create feature for count and write per residue and per type frequency */
    ajpFeatCounts = ajFeatNewII(ajpFeattableCounts,
				ajIntFeatureResType,
				ajIntFeatureResType);
    ajFmtPrintS(&ajpStrFeatTemp, "*count1 %d", ajIntCount1);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);
    ajFmtPrintS(&ajpStrFeatTemp, "*count2 %d", ajIntCount2);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);

    ajStrDel(&ajpStrFeatTemp);
    
    return ajpFeatCounts;
}




