/******************************************************************************
** @source NUCLEUS contact functions for alignment
**
** functions used to handle contact map files for refining sequence-structure
**                                                                 alignment
**
**
** @author Copyright (C) 2004 Damian Counsell
** @version $Revision: 1.5 $
** @modified $Date: 2005/01/19 18:09:24 $
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/




/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ajax.h"
#include "embcontact.h"

/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

enum constant
    {
	enumDebugLevel        =  3,
	enumFirstResType      =  0,
	enumFirstResTypeIndex =  0,
	enumNoCmapLine        =  0,
	enumZeroContacts      =  0,
	enumArrayOffset       =  1,
	enumHeaderCmapLine    =  1,
	enumLastContactIndex  =  1,
	enumBlankCmapLine     =  2,
	enumContactCmapLine   =  3,
	enumSeqCmapLine       =  4,
	enumMaxContactsPerRes = 20,
	enumTotalResTypes     = 30,
	enumNoResTypeMatch    = 29,
 	enumAsciiOffset       = 65,
	enumAsciiAsterisk     = 91,
	enumAsciiQuestionMark = 92
    };

/* look-up array of three-letter codes */
/* XXXX THE PENULTIMATE THREE TLCs ARE DUMMIES */
static char *cArrayThreeLetterCodes[enumTotalResTypes] =
    {
	"ALA\0","ASX\0","CYS\0","ASP\0","GLU\0","PHE\0","GLY\0","HIS\0",
	"ILE\0","---\0","LYS\0","LEU\0","MET\0","ASN\0","---\0","PRO\0",
	"GLN\0","ARG\0","SER\0","THR\0","SEC\0","VAL\0","TRP\0","XAA\0",
	"TYR\0","GLX\0","XXX\0","YYY\0","ZZZ\0","NO MATCH\0"
    };
/* static char cOneLetterCodes[enumTotalResTypes] = */
/*     { */
/* 	'A','B','C','D','E','F','G','H', */
/* 	'I','\0','K','L','M','N','\0','P', */
/* 	'Q','R','S','T','U','V','W','X', */
/* 	'Y','Z','\0','\0','\0','\0' */
/*     }; */

/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

/* ==================================================================== */
/* ========================= constructors ============================= */
/* ==================================================================== */

/* @section EmbPContact Constructors ******************************************
**
** All constructors return a new EmbPContact by pointer. It is the
** responsibility of the user to first destroy any previous cell. The target
** pointer does not need to be initialised to NULL, but it is good programming
** practice to do so anyway.
**
******************************************************************************/

/* @func embContactNew *****************************************************
**
** Default constructor for NUCLEUS Cmap contact objects
**
** @return [EmbPContact] pointer to an EmbPContact
** @category new [EmbPContact] default constructor
** @@
******************************************************************************/

EmbPContact embContactNew(void)
{
    EmbPContact embpContactReturned = NULL;

    AJNEW0(embpContactReturned);

    return embpContactReturned;
}


/* @section EmbPHeader Constructors ******************************************
**
** All constructors return a new EmbPCmapHeader by pointer. It is the
** responsibility of the user to first destroy any previous contact map file
** header object. The target pointer does not need to be initialised to NULL,
** but it is good programming practice to do so anyway.
**
******************************************************************************/



/* @func embCmapHeaderNew *****************************************************
**
** Default constructor for a contact map header object
**
** @return [EmbPCmapHeader] Pointer to an EmbPCmapHeader
** @@
******************************************************************************/

EmbPCmapHeader embCmapHeaderNew(void)
{
    EmbPCmapHeader embpCmapHeaderReturned = NULL;

    AJNEW0(embpCmapHeaderReturned);

    return embpCmapHeaderReturned;
}

/* ==================================================================== */
/* =========================== destructor ============================= */
/* ==================================================================== */


/* @section Contact Destructors **********************************************
**
** Destruction is achieved by deleting the pointer to the contact object and
**  freeing the associated memory
**
******************************************************************************/

/* @func embContactDel *******************************************************
**
** Default destructor for NUCLEUS Cmap contact objects
**
** If the given pointer is NULL, or a NULL pointer, simply returns.
**
** @param  [d] pthis [EmbPContact*] Pointer to the object to be deleted.
**         The pointer is always deleted.
** @return [void]
** @category delete [EmbPContact] default destructor
** @@
******************************************************************************/

void embContactDel(EmbPContact* pthis)
{
    EmbPContact thys = NULL;

    thys = pthis ? *pthis :0;
 
    if(!pthis)
	return;

    thys->ajIntFirstPosition = 0;
    thys->ajIntSecondPosition = 0;
    thys->ajpStrFirstResType = NULL; 
    thys->ajpStrSecondResType = NULL;    

    AJFREE(thys);
    *pthis = NULL;

    return;
}


/* @section Cmap header Destructors ******************************************
**
** Destruction is achieved by deleting the pointer to the contact map header
**  object and freeing the associated memory
**
******************************************************************************/

/* @func embCmapHeaderDel ************************************************
**
** Default destructor for a contact map header object
**
** @param [d] pthis [EmbPCmapHeader*] header to be deleted
** @return [void]
** @@
******************************************************************************/

void embCmapHeaderDel (EmbPCmapHeader* pthis)
{
    EmbPCmapHeader thys = NULL;

    thys = pthis ? *pthis :0;
 
    if(!pthis)
	return;

    thys->ajpStrCmapId = NULL; 
    thys->ajpStrCmapDe = NULL;    
    thys->ajpStrCmapEx = NULL; 
    thys->ajpStrCmapMo = NULL;    
    thys->ajpStrCmapCn = NULL; 
    thys->ajpStrCmapIn = NULL;    
    thys->ajpStrCmapSq = NULL;    

    AJFREE(thys);
    *pthis = NULL;

    return;
}






/* @func embContactWriteScoringMatrix ****************************************
**
** writes normalized substitution scores to a scoring matrix file format
**
** @param [r] ajpInt2dCounts [const AjPInt2d] 2-D matrix of signed integer
**                                            scores
** @param [w] ajpFileScoringMatrix [AjPFile] file to write scoring table to
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

AjBool embContactWriteScoringMatrix(const AjPInt2d ajpInt2dCounts,
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

    /* write header */
    
    /*
     * write scores to data file
     * iterating over string of
     * scoring_matrix residue types
     */

    /* start by writing top line */
    ajpStrResTypeAlphabet = ajStrNewC("ARNDCQEGHILKMFPSTWYVBZX*");
    ajpStrResTypeAlphabetCopy = ajStrDup(ajpStrResTypeAlphabet);

    ajFmtPrintF(ajpFileScoringMatrix, " ");

    ajpStrIterColumnResType = ajStrIter(ajpStrResTypeAlphabetCopy);
    do
    {
	cColumnResType = ajStrIterGetK(ajpStrIterColumnResType);
	ajFmtPrintF(ajpFileScoringMatrix, "    %c", cColumnResType);
    }    
    while(ajStrIterNext(ajpStrIterColumnResType));

    /* rewind string iterator */
    ajStrIterBegin(ajpStrIterColumnResType);

    ajFmtPrintF(ajpFileScoringMatrix, "\n");

    /* write rest of score block */
    ajpStrIterRowResType = ajStrIter(ajpStrResTypeAlphabet);
   do
    {
	cRowResType = ajStrIterGetK(ajpStrIterRowResType);
	ajIntRow = embCharToScoringMatrixIndex(cRowResType);

	/* iterate over string of scoring_matrix residue types */
	ajFmtPrintF(ajpFileScoringMatrix, "%c ", cRowResType);
	do
	{
	    cColumnResType = ajStrIterGetK(ajpStrIterColumnResType);
	    ajIntColumn = embCharToScoringMatrixIndex(cColumnResType);
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
	/* rewind string iterator */
	ajStrIterBegin(ajpStrIterColumnResType);
    }    
    while(ajStrIterNext(ajpStrIterRowResType));

    ajStrIterFree(&ajpStrIterRowResType);
    ajStrIterFree(&ajpStrIterColumnResType);
    ajStrDel(&ajpStrResTypeAlphabet);
  
    return ajBoolSuccess;
}



/* @func embAjintToScoringMatrixIndex *****************************************
**
** converts residue type integer to scoring_matrix index
**
** @param [r] ajIntResType [ajint] numerical residue type
** @return [ajint] index to corresponding character in data dir scoring matrix
** @@
******************************************************************************/

ajint embAjintToScoringMatrixIndex(ajint ajIntResType)
{
    ajint ajIntScoringMatrixIndex;
    ajint ajIntLookUpArrayIndex;

    /* look up array for residue type order
     *  found in scoring matrix data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const ajint ajIntArrayLookUp[enumTotalResTypes] =
	{0,20,4,3,6,13,7,8,9,24,11,10,12,2,25,14,5,1,15,16,26,19,17,22,18,21,23,27};

    /* default to out-of-range value */
    if ((ajIntResType < 0) || (ajIntResType > 27))
	ajIntScoringMatrixIndex = enumNoResTypeMatch;
    else
    {
	ajIntLookUpArrayIndex = (ajIntResType);
	ajIntScoringMatrixIndex = ajIntArrayLookUp[ajIntLookUpArrayIndex] + enumArrayOffset;
    }

    if( enumDebugLevel )
	ajFmtPrint( "\n ajIntResType: %d, ajIntScoringMatrixIndex: %d"
		    , ajIntResType, ajIntScoringMatrixIndex );
    
    return ajIntScoringMatrixIndex;
}



/* @func embCharToAjint1 *****************************************************
**
** converts residue type character to integer
**
** @param [r] cResType [char] original single-letter residue type
** @return [ajint] integer corresponding to character in one-letter code list
** @@
******************************************************************************/

ajint embCharToAjint1(char cResType)
{
    ajint ajIntOneLetterCode;
    ajint ajIntCount;

    /* look up array for permitted one-letter codes */
    static char cOneLetterCodes[enumTotalResTypes] =
	{
	    'A','B','C','D','E','F','G','H',
	    'I','\0','K','L','M','N','\0','P',
	    'Q','R','S','T','U','V','W','X',
	    'Y','Z','\0','\0','\0','\0'
	};

    if (cResType == '*')
	cResType = enumAsciiAsterisk;
    if (cResType == '?')
	cResType = enumAsciiQuestionMark;

    /* default to out-of-range value */
    ajIntOneLetterCode = 28;
    for(ajIntCount = 0; ajIntCount < enumTotalResTypes; ajIntCount++)
    {
	if(cResType == cOneLetterCodes[ajIntCount])
	{
	    ajIntOneLetterCode = ajIntCount;
	}
    }
    
    return ajIntOneLetterCode;
}



/* @func embCharToScoringMatrixIndex *****************************************
**
** converts residue type character to scoring_matrix index
**
** @param [r] cResType [char] original single-letter residue type
** @return [ajint] index to corresponding character in data dir scoring matrix
** @@
******************************************************************************/

ajint embCharToScoringMatrixIndex(char cResType)
{
    ajint ajIntScoringMatrixIndex;

    /* look up array for residue type order
     *  found in scoring matrix data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const ajint ajIntArrayLookUp[enumTotalResTypes] =
	{0,20,4,3,6,13,7,8,9,24,11,10,12,2,25,14,5,1,15,16,26,19,17,22,18,21,23,27};
    if (cResType == '*')
	cResType = enumAsciiAsterisk;
    if (cResType == '?')
	cResType = enumAsciiQuestionMark;
  
    cResType = cResType - enumAsciiOffset;
    
    /* default to out-of-range value */
    if ((cResType < 0) || (cResType > 27))
	ajIntScoringMatrixIndex = 28;
    else
	ajIntScoringMatrixIndex = ajIntArrayLookUp[(ajint) cResType];

    return ajIntScoringMatrixIndex;
}



/* @func embScoringMatrixIndexToChar *****************************************
**
** converts scoring_matrix index number to char
**
** @param [r] ajIntScoringMatrixIndex [ajint] BLOSUM index number
** @return [ajint]  original single-letter residue type
** @@
******************************************************************************/

ajint embScoringMatrixIndexToChar(ajint ajIntScoringMatrixIndex)
{
    char cResType = '\0';

    /* look up array for residue type order
     *  found in scoring matrix data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const char cArrayLookUp[enumTotalResTypes] =
	{'A','R','N','D','C','Q','E','G','H','I','L','K','M','F','P','S','T','W','Y','V','B','Z','X','*','J','O','U','?'};
  
    /* XXXX SHOULD THIS BE AN OFFICIAL AMBIGUITY CODE INSTEAD? */
    /* default to out-of-range value */
    if ((cResType < 0) || (cResType > 27))
	cResType = '?';
    else
	cResType = cArrayLookUp[ajIntScoringMatrixIndex];

    return cResType;
}






/* @func embAjint1ToString3 **************************************************
**
** gives three-letter amino acid residue code for one-letter ASCII number
**
** @param [r] ajIntCode [ajint] one digit ASCII string A=0 29=NO_MATCH
** @return [AjPStr] code for three-letter equivalent amino acid residue
** @@
******************************************************************************/

AjPStr embAjint1ToString3 (ajint ajIntCode)
{
    AjPStr ajpStrThreeLetterCode = NULL;
    
    if(ajIntCode < 0 || ajIntCode > 29)
    {
	ajpStrThreeLetterCode = NULL;
    }
    else
    {
	ajpStrThreeLetterCode = 
	    ajStrNewC(cArrayThreeLetterCodes[ajIntCode]);
    }

    return ajpStrThreeLetterCode;
}



/* @func embReadAndReviseCmapFile *********************************************
**
** reads ajpdb contact map file into three arrays of ints and modifies residue
**  values according to alignment provided 
**
** @param [r] ajpFileCmap [AjPFile] input file stream of current cmap
** @param [r] pcTraceDown [char *] trace of query sequence
** @param [r] pcTraceAcross [char *] trace of template sequence
** @param [r] ajIntSeqLen [ajint] Sequence length
** @param [r] pEmbpCmapHeader [EmbPCmapHeader*] Contact map header
** @param [r] pEmbpInt2dCmapSummary [AjPInt2d*] contacts summary
** @param [r] pEmbpInt2dCmapResTypes [AjPInt2d*] contacts as residue types
** @param [r] pEmbpInt2dCmapPositions [AjPInt2d*] contacts as positions
**                                               in chain
** @return [AjBool] ajTrue if file successfully read
** @@
******************************************************************************/

AjBool embReadAndReviseCmapFile (AjPFile ajpFileCmap,
				 char *pcUpdatedSeqAcross,
				 ajint ajIntSeqLen,
				 AjPInt2d *pAjpInt2dCmapSummary,
				 EmbPCmapHeader *pEmbpCmapHeader,
				 AjPInt2d *pAjpInt2dCmapResTypes,
				 AjPInt2d *pAjpInt2dCmapPositions)
{
    AjBool ajBoolCmapFileRead; /* has the file been read? */
    /* DDDD DEBUG: ARE NULL HEADERS BEING RETURNED? */
    AjBool ajBoolCmapHeaderLoaded; /* has the Cmap header been read? */

    ajint ajIntColumnCount;

    /* what kind of contact map line has been read in? */
    ajint ajIntCmapLineType;

    /* to store contact attributes from contact map */
    ajint ajIntFirstResType;
    ajint ajIntSecondResType;
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;
    ajint ajIntLastContact;

     /* object to hold single contact */
    EmbPContact embpContactTemp = NULL;

    /* structure to hold header text */
    EmbPCmapHeader embpCmapHeader = NULL;

    /* residue in seq updated by alignment */
    char cTempUpdatedRes;

    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* string to store that contact map line */
    AjPStr ajpStrCmapLine = NULL;

    /* default to no line */
    ajIntCmapLineType = enumNoCmapLine;

    ajBoolCmapFileRead = ajFalse;

    /* DDDD DEBUG: ARE NULL HEADERS BEING RETURNED? */
    ajBoolCmapHeaderLoaded = ajFalse;

    /* check file passed to function is usable */	
    if(!ajpFileCmap)
    {	
	ajWarn("function embReadCmapFile cannot open passed filestream");	
	return ajFalse;
    }

    /* there are no contacts in any of the columns of the contact arrays yet */
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
	ajInt2dPut(pAjpInt2dCmapSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   enumZeroContacts);

    /* reserve memory for current contact string */
    ajpStrCmapLine = ajStrNew();
    
    /* reserve memory for current contact object */
    embpContactTemp = embContactNew();

    /* dereference pointers to passed object pointers */
    embpCmapHeader = *pEmbpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* read through contact map file until there are no more contacts */
    while( ( ajpStrCmapLine = embReadCmapLine(ajpFileCmap) ) )
    {
	ajIntCmapLineType = embTypeCmapLine(ajpStrCmapLine);

	ajIntLastContact = enumZeroContacts;
	
	if(ajIntCmapLineType == enumContactCmapLine)
	{
	    embLoadContactLine(ajpStrCmapLine, embpContactTemp);

	    /* load up temporary contact object with values read from line in Cmap file */ 
	    ajIntFirstResType = 
		embString3ToAjint1(embpContactTemp->ajpStrFirstResType);
	    ajIntSecondResType = 
		embString3ToAjint1(embpContactTemp->ajpStrSecondResType);
	    ajIntFirstPosition = 
		embpContactTemp->ajIntFirstPosition;
	    ajIntSecondPosition = 
		embpContactTemp->ajIntSecondPosition;

	    /* change residue type if necessary */
	    cTempUpdatedRes = pcUpdatedSeqAcross[ajIntFirstPosition];
	    ajFmtPrint( "\n�� cTempUpdatedRes: %c integer conversion: %d ��", cTempUpdatedRes,
			embCharToAjint1(cTempUpdatedRes));
	    
	    cTempUpdatedRes = pcUpdatedSeqAcross[ajIntSecondPosition];
	    ajFmtPrint( "\n�� cTempUpdatedRes: %c integer conversion: %d �\n", cTempUpdatedRes,
			embCharToAjint1(cTempUpdatedRes));
	    
	    /* get position of last contact in column */
	    ajIntLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntFirstPosition);
	    
	    /*
	     * insert type of first residue in contact into
	     * column of ints in summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumFirstResType,
		       ajIntFirstPosition,
		       ajIntFirstResType);

	    /*
	     * insert position of second residue in contact into
	     * column of ints in array of second residue positions
	     */
	    ajInt2dPut(pAjpInt2dCmapPositions,
		       ajIntLastContact,
		       ajIntFirstPosition,
		       ajIntSecondPosition);
	    
	    /*
	     * insert type of second residue in contact into
	     * column of ints in array of second residues
	     */
	    ajInt2dPut(pAjpInt2dCmapResTypes,
		       ajIntLastContact,
		       ajIntFirstPosition,
		       ajIntSecondResType);
	    
	    /* increment contact counter at zeroth position in column */
	    ajIntLastContact++;
	    
	    /*
	     * insert incremented contact counter
	     * into summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumLastContactIndex,
		       ajIntFirstPosition,
		       ajIntLastContact);	    

	}
	/*
	 * XXXX IDEALLY THERE SHOULD BE A LOOP HERE SO THAT, IF THE
	 *  LINE IS THE FIRST SEQUENCE ("SQ") LINE, THEN ALL THE
	 *  ALL THE LINES OF SEQUENCE BEFORE THE NEXT TRUE BLANK ("XX")
	 *  LINE ARE READ IN AS A BLOCK
	 */

	else if(ajIntCmapLineType == enumHeaderCmapLine ||
		ajIntCmapLineType == enumSeqCmapLine)
	{
	    /* DDDDEBUG: DEBUGGGING VERSION OF THIS CLAUSE USED HERE */
	    ajBoolCmapHeaderLoaded = embLoadHeaderLine(ajpStrCmapLine,
						       embpCmapHeader);
	    if(!ajBoolCmapHeaderLoaded)
		    {	
			ajWarn("HEADER NOT READ!");	
			return ajFalse;
		    }
	}
    }

    /* free contact */
    embContactDel(&embpContactTemp);
    
    return ajBoolCmapFileRead;
}



/* @func embReadCmapFile ******************************************************
**
** reads ajpdb contact map file into three arrays of ints  
**
** @param [u] ajpFileCmap [AjPFile] input file stream of current cmap
** @param [r] ajIntSeqLen [ajint] Sequence length
** @param [w] pAjpInt2dCmapSummary [AjPInt2d*] contacts summary
** @param [w] pEmbpCmapHeader [EmbPCmapHeader*] Contact map header
** @param [w] pAjpInt2dCmapResTypes [AjPInt2d*] contacts as residue types
** @param [w] pAjpInt2dCmapPositions [AjPInt2d*] contacts as positions
**                                               in chain
** @return [AjBool] ajTrue if file successfully read
** @@
******************************************************************************/

AjBool embReadCmapFile (AjPFile ajpFileCmap,
			ajint ajIntSeqLen,
			AjPInt2d *pAjpInt2dCmapSummary,
			EmbPCmapHeader *pEmbpCmapHeader,
			AjPInt2d *pAjpInt2dCmapResTypes,
			AjPInt2d *pAjpInt2dCmapPositions)
{
    AjBool ajBoolCmapFileRead; /* has the file been read? */
    /* DDDD DEBUG: ARE NULL HEADERS BEING RETURNED? */
    AjBool ajBoolCmapHeaderLoaded; /* has the Cmap header been read? */

    ajint ajIntColumnCount;

    /* what kind of contact map line has been read in? */
    ajint ajIntCmapLineType;

    /* to store contact attributes from contact map */
    ajint ajIntFirstResType;
    ajint ajIntSecondResType;
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;
    ajint ajIntLastContact;

     /* object to hold single contact */
    EmbPContact embpContactTemp = NULL;

    /* structure to hold header text */
    EmbPCmapHeader embpCmapHeader = NULL;

    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* string to store that contact map line */
    AjPStr ajpStrCmapLine = NULL;

    /* default to no line */
    ajIntCmapLineType = enumNoCmapLine;
 
    ajBoolCmapFileRead = ajFalse;
    /* DDDD DEBUG: ARE NULL HEADERS BEING RETURNED? */
    ajBoolCmapHeaderLoaded = ajFalse;

    /* check file passed to function is usable */	
    if(!ajpFileCmap)
    {	
	ajWarn("function embReadCmapFile cannot open passed filestream");	
	return ajFalse;
    }

    /* there are no contacts in any of the columns of the contact arrays yet */
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
	ajInt2dPut(pAjpInt2dCmapSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   enumZeroContacts);

    /* reserve memory for current contact string */
    ajpStrCmapLine = ajStrNew();
    
    /* reserve memory for current contact object */
    embpContactTemp = embContactNew();

    /* dereference pointers to passed object pointers */
    embpCmapHeader = *pEmbpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* read through contact map file until there are no more contacts */
    while( ( ajpStrCmapLine = embReadCmapLine(ajpFileCmap) ) )
    {
	ajIntCmapLineType = embTypeCmapLine(ajpStrCmapLine);

	ajIntLastContact = enumZeroContacts;
	
	if(ajIntCmapLineType == enumContactCmapLine)
	{
	    embLoadContactLine(ajpStrCmapLine, embpContactTemp);

	    /* load up temporary contact object with values read from line in Cmap file */ 
	    ajIntFirstResType = 
		embString3ToAjint1(embpContactTemp->ajpStrFirstResType);
	    ajIntSecondResType = 
		embString3ToAjint1(embpContactTemp->ajpStrSecondResType);
	    ajIntFirstPosition = 
		embpContactTemp->ajIntFirstPosition;
	    ajIntSecondPosition = 
		embpContactTemp->ajIntSecondPosition;
	    
	    /* get position of last contact in column */
	    ajIntLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntFirstPosition);
	    /* XXXX DEBUG */
	    if( enumDebugLevel > 2 )
	    {
		ajFmtPrint("ajIntFirstPosition (col.): %d\t",
			   ajIntFirstPosition);
		ajFmtPrint("ajIntSecondPosition: %d\t",
			   ajIntSecondPosition);
		ajFmtPrint("ajIntLastContact: %d\n",
			   ajIntLastContact);
		ajFmtPrint("# ajpStrFirstResType: %S\tajIntFirstResType: %d\tand back to type: %S #\n",
			   embpContactTemp->ajpStrFirstResType,
			   ajIntFirstResType, embAjint1ToString3(ajIntFirstResType));
		ajFmtPrint("# ajIntSecondResType: %S\tajIntSecondResType: %d\tand back to type: %S #\n",
			   embpContactTemp->ajpStrSecondResType,
			   ajIntSecondResType,
			   embAjint1ToString3(ajIntSecondResType));
	    }
	    
	    /*
	     * insert type of first residue in contact into
	     * column of ints in summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumFirstResType,
		       ajIntFirstPosition,
		       ajIntFirstResType);

	    /*
	     * insert position of second residue in contact into
	     * column of ints in array of second residue positions
	     */
	    ajInt2dPut(pAjpInt2dCmapPositions,
		       ajIntLastContact,
		       ajIntFirstPosition,
		       ajIntSecondPosition);
	    
	    /*
	     * insert type of second residue in contact into
	     * column of ints in array of second residues
	     */
	    ajInt2dPut(pAjpInt2dCmapResTypes,
		       ajIntLastContact,
		       ajIntFirstPosition,
		       ajIntSecondResType);
	    
	    /* increment contact counter at zeroth position in column */
	    ajIntLastContact++;
	    
	    /*
	     * insert incremented contact counter
	     * into summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumLastContactIndex,
		       ajIntFirstPosition,
		       ajIntLastContact);	    

	}
	/*
	 * XXXX IDEALLY THERE SHOULD BE A LOOP HERE SO THAT, IF THE
	 *  LINE IS THE FIRST SEQUENCE ("SQ") LINE, THEN ALL THE
	 *  ALL THE LINES OF SEQUENCE BEFORE THE NEXT TRUE BLANK ("XX")
	 *  LINE ARE READ IN AS A BLOCK
	 */

	else if(ajIntCmapLineType == enumHeaderCmapLine ||
		ajIntCmapLineType == enumSeqCmapLine)
	{
	    /* DDDDEBUG: DEBUGGGING VERSION OF THIS CLAUSE USED HERE */
	    ajBoolCmapHeaderLoaded = embLoadHeaderLine(ajpStrCmapLine,
						       embpCmapHeader);
	    if(!ajBoolCmapHeaderLoaded)
		    {	
			ajWarn("HEADER NOT READ!");	
			return ajFalse;
		    }
	}
    }

    /* free contact */
    embContactDel(&embpContactTemp);
    
    return ajBoolCmapFileRead;
}



/* @func embReadCmapLine *****************************************************
**
** reads single line from ajpdb contact map file and returns it as a string
**
** @param [u] ajpFileCmap [AjPFile]  input file stream of current cmap
** @return [AjPStr] contents of line read
** @@
******************************************************************************/

AjPStr embReadCmapLine (AjPFile ajpFileCmap)
{
    AjPStr ajpStrCmapLine = NULL;

    ajFileReadLine(ajpFileCmap, &ajpStrCmapLine);

    return ajpStrCmapLine;
}



/* @func embTypeCmapLine ******************************************************
**
** based on first two chars, returns type of line from contact map file:
**
** @param [r] ajpStrCmapLine [const AjPStr] Contact map file line  
** @return [ajint] Enumerated line code
** @@
******************************************************************************/

ajint embTypeCmapLine (const AjPStr ajpStrCmapLine)
{
    ajint ajIntLineCode = enumNoCmapLine;

    if(!ajpStrCmapLine)
	ajWarn("NULL string passed to embTypeCmapLine");

    /* flag contact line as such */
    if(ajStrPrefixC(ajpStrCmapLine, "SM"))
    {
	ajIntLineCode = enumContactCmapLine;
    }
    /* flag blank Cmap lines */
    else if(ajStrPrefixC(ajpStrCmapLine, "XX"))
    {
	ajIntLineCode = enumBlankCmapLine;
    }
    /* flag Cmap header lines */
    else if( ajStrPrefixC(ajpStrCmapLine, "ID") ||
	     ajStrPrefixC(ajpStrCmapLine, "DE") ||
	     ajStrPrefixC(ajpStrCmapLine, "EX") ||
	     ajStrPrefixC(ajpStrCmapLine, "MO") ||
	     ajStrPrefixC(ajpStrCmapLine, "CN") ||
	     ajStrPrefixC(ajpStrCmapLine, "IN") )
    {
	ajIntLineCode = enumHeaderCmapLine;	
    }     
    else if(ajStrPrefixC(ajpStrCmapLine, "SQ"))
    {
	ajIntLineCode = enumSeqCmapLine;
    }
    else 
    {
	ajIntLineCode = enumNoCmapLine;
    }

    return ajIntLineCode;
}



/* @func embLoadContactLine ***************************************************
**
** loads single contact from single line of ajpdb contact map file
**
** @param [r] ajpStrCmapLine [const AjPStr] to contact line from Cmap file
** @param [w] embpContactLoaded [EmbPContact] to contact object from line
** @return [AjBool] ajTrue if contact successfully parsed
** @@
******************************************************************************/

AjBool embLoadContactLine (const AjPStr ajpStrCmapLine,
			   EmbPContact embpContactLoaded)
{
    AjPStr ajpStrPrefix = NULL;  /* first two characters on current line */
    char cPunctuation  = '\0';  /* separator on current line */
    ajint ajIntFirstPosition = 0; /* position of 1st residue in contact */
    ajint ajIntSecondPosition = 0;/* position of 2nd residue in contact */
    ajint ajIntConversionsDone = 0; /* n(successful conversions) by
				       AjFmtScansS */
    AjPStr ajpStrFirstResType = NULL; /* 3-letter code for 1st residue
					 in contact */
    AjPStr ajpStrSecondResType = NULL; /* 3-letter code for 2nd
					  residue in contact */
    AjBool ajBoolContactLoaded = ajFalse;

    if(!ajpStrFirstResType)
	ajpStrFirstResType = ajStrNew();
    if(!ajpStrSecondResType)
	ajpStrSecondResType = ajStrNew();
    if(!ajpStrPrefix)
	ajpStrPrefix = ajStrNew();

    /* check string passed to function is usable */	
    if(!ajpStrCmapLine)
    {	
	ajWarn("NULL string passed to load_cmap_line");	
	return ajFalse;
    }

    ajIntConversionsDone = ajFmtScanS(ajpStrCmapLine, "%S %S %d %c %S %d",
				      &ajpStrPrefix,
				      &ajpStrFirstResType, &ajIntFirstPosition,
				      &cPunctuation,
				      &ajpStrSecondResType,
				      &ajIntSecondPosition);

    if(ajIntConversionsDone == 6)
    {
	embpContactLoaded->ajpStrFirstResType = ajpStrFirstResType;
	embpContactLoaded->ajIntFirstPosition = ajIntFirstPosition;
	embpContactLoaded->ajpStrSecondResType = ajpStrSecondResType;
	embpContactLoaded->ajIntSecondPosition = ajIntSecondPosition;
	ajBoolContactLoaded = ajTrue;
    }

    ajStrDel(&ajpStrPrefix);
    
    return ajBoolContactLoaded;
}



/* @func embLoadHeaderLine ****************************************************
**
** loads single line from ajpdb contact map file into header object
**
** @param [r] ajpStrCmapLine [const AjPStr] to header line from Cmap file
** @param [w] embpCmapHeaderToLoad [EmbPCmapHeader] to header object from
**                                                    line
** @return [AjBool] ajTrue if line successfully loaded
** @@
******************************************************************************/

AjBool embLoadHeaderLine (const AjPStr ajpStrCmapLine,
			  EmbPCmapHeader embpCmapHeaderToLoad)
{
    AjBool ajBoolCmapHeaderToLoad;

    ajBoolCmapHeaderToLoad = ajFalse;

    if(!ajpStrCmapLine)
    {	
	ajWarn("NULL string passed to embReadCmapLine()");	
	return ajFalse;
    }

    if(ajStrPrefixC(ajpStrCmapLine, "ID"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapId, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    }
    /* read in other types of line */
    else if(ajStrPrefixC(ajpStrCmapLine, "DE"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapDe, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    }
    else if(ajStrPrefixC(ajpStrCmapLine, "EX"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapEx, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "MO"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapMo, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "CN"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapCn, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "IN"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapIn, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "SQ"))
    {
	ajStrAssS(&embpCmapHeaderToLoad->ajpStrCmapSq, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else 
    {
	ajBoolCmapHeaderToLoad = ajFalse;
    }

    return ajBoolCmapHeaderToLoad;
}





/* @func embString3ToAjint1 ***************************************************
**
** gives one-letter ASCII number for three-letter amino acid residue code
**
** @param [r] ajpStrThreeLetterCode [const AjPStr] three-letter code string
** @return [ajint] code for one-letter equivalent A=0 29=NO MATCH
** @@
******************************************************************************/

ajint embString3ToAjint1 (const AjPStr ajpStrThreeLetterCode)
{
    ajint ajIntCount;
    ajint ajIntCode = enumNoResTypeMatch;

    for(ajIntCount = 0; ajIntCount < enumTotalResTypes; ajIntCount++)
	if(!ajStrCmpC(ajpStrThreeLetterCode,
		      cArrayThreeLetterCodes[ajIntCount]))
	{
	    ajIntCode = ajIntCount;
	}

    return ajIntCode;
}





/* @func embGetCmapSummary ***************************************************
**
** reserves memory for a 2-D array of ints to summarise a contact map 
**
** @param [r] pcSeq [const char*] Contact map
** @return [AjPInt2d] array of ajints containing number of contacts and
** types of first residue in each contact
** @@
******************************************************************************/

AjPInt2d embGetCmapSummary (const char *pcSeq)
{
    ajint ajIntAcrossSeqLen;
    ajint ajIntColumnCount;
    ajint ajIntResType;
    
    AjPInt2d ajpInt2dSummary = NULL;

    ajIntAcrossSeqLen = strlen(pcSeq);

    ajpInt2dSummary = ajInt2dNewL(ajIntAcrossSeqLen);

    for(ajIntColumnCount = 0;ajIntColumnCount < ajIntAcrossSeqLen;ajIntColumnCount++)
    {
	ajIntResType = enumNoResTypeMatch;

	ajInt2dPut(&ajpInt2dSummary,
		   enumFirstResTypeIndex,
		   ajIntColumnCount,
		   ajIntResType);

	ajInt2dPut(&ajpInt2dSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   enumZeroContacts);
    }
	
    return ajpInt2dSummary;
}




/* @func embGetIntMap ********************************************************
**
** reserves memory for a 2-D array of ints to store a contact map 
**
** @param [r] ajIntAcrossSeqLen [ajint] number of residues in template
** @return [AjPInt2d] array of ajints containing position IDs for each contact
** @@
******************************************************************************/

AjPInt2d embGetIntMap (ajint ajIntAcrossSeqLen)
{
    ajint ajIntRowCount;
    ajint ajIntColumnCount;
    
    AjPInt2d ajpInt2dContacts = NULL;

    ajpInt2dContacts = ajInt2dNewL(ajIntAcrossSeqLen);

    for(ajIntRowCount = 0;ajIntRowCount < enumMaxContactsPerRes;ajIntRowCount++)
    {
	for(ajIntColumnCount = 0;ajIntColumnCount < ajIntAcrossSeqLen;ajIntColumnCount++)
	{
	    ajInt2dPut(&ajpInt2dContacts,
		       ajIntRowCount,
		       ajIntColumnCount,
		       enumZeroContacts);
	}
    }

    return ajpInt2dContacts;
}



/* @func embWriteCmapFile ****************************************************
**
** writes ajpdb contact map file from header object and two arrays of ints  
**
** @param [u] ajpFileCmap [AjPFile] output file stream of current cmap
** @param [r] ajIntSeqLen [ajint] number of residues in chain sequence
** @param [w] pAjpInt2dCmapSummary [AjPInt2d*] summary of contact map
** @param [w] pEmbpCmapHeader [EmbPCmapHeader*] Contact map header
** @param [w] pAjpInt2dCmapResTypes [AjPInt2d*] contacts as residue types
** @param [w] pAjpInt2dCmapPositions [AjPInt2d*] contacts as positions in
**                                               chain
** @return [AjBool] ajTrue if file successfully written
** @@
******************************************************************************/

AjBool embWriteCmapFile (AjPFile ajpFileCmap,
			ajint ajIntSeqLen,
			AjPInt2d *pAjpInt2dCmapSummary,
			EmbPCmapHeader *pEmbpCmapHeader,
			AjPInt2d *pAjpInt2dCmapResTypes,
			AjPInt2d *pAjpInt2dCmapPositions)
{
    AjBool ajBoolCmapFileWritten = ajFalse; /* has the file been written? */

    /* counters and limits */
    ajint ajIntRow;
    ajint ajIntColumn;
    ajint ajIntColumnLen;

    /* to store contact attributes from contact map */
    ajint ajIntTempFirstResType;
    ajint ajIntTempSecondResType;
    AjPStr ajpStrTempFirstResType;
    AjPStr ajpStrTempSecondResType;
    ajint ajIntTempFirstPosition;
    ajint ajIntTempSecondPosition;

     /* object to hold single contact */
    EmbPContact embpContactTemp = NULL;
    /* structure to hold header text */
    EmbPCmapHeader embpCmapHeader = NULL;

    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* dereference pointers to original pointers */
    embpCmapHeader = *pEmbpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* check file passed to function is usable */	
    if(!ajpFileCmap)
    {	
	ajWarn("function embWriteCmapFile cannot open passed filestream");	
	return ajFalse;
    }

    /* write header to file */
    ajBoolCmapFileWritten = embWriteCmapHeader(ajpFileCmap,
						     embpCmapHeader);

    /* reserve memory for current contact object */
    embpContactTemp = embContactNew();

    /* XXXX OUTER LOOP OVER FIRST RESIDUES IN CONTACTS */
    for(ajIntColumn = enumArrayOffset;ajIntColumn < ajIntSeqLen;ajIntColumn++)
    {
	/* find end of each column */
	ajIntColumnLen = ajInt2dGet(ajpInt2dCmapSummary,
				    enumLastContactIndex,
				    ajIntColumn);

/* 	ajFmtPrint("��������������������������������\n"); */
/* 	ajFmtPrint("������ COLUMN LENGTH: %d ������\n", ajIntColumnLen); */
/* 	ajFmtPrint("��������������������������������\n\n"); */

	ajIntTempFirstResType = ajInt2dGet(ajpInt2dCmapSummary,
					   enumFirstResTypeIndex,
					   ajIntColumn);
	ajpStrTempFirstResType = embAjint1ToString3(ajIntTempFirstResType);

	if (ajIntColumnLen > enumZeroContacts)
	{

	    /* XXXX INNER LOOP OVER CONTACTS IN COLUMN */
	    for(ajIntRow = 0;ajIntRow < ajIntColumnLen;ajIntRow++)
	    {
		ajIntTempSecondResType = ajInt2dGet(ajpInt2dCmapResTypes,
						    ajIntRow,
						    ajIntColumn);	    

		/* load up temporary contact object with values from residue type array */ 
		ajpStrTempSecondResType = embAjint1ToString3(ajIntTempSecondResType);

		if( enumDebugLevel > 1 )
		{
		    
		    ajFmtPrint("��������������������������������\n");
		    ajFmtPrint("���� first residue type: %S ����\n",
			       ajpStrTempFirstResType);
		    ajFmtPrint("��� second residue type: %S ����\n",
			       ajpStrTempSecondResType);
		    ajFmtPrint("��������������������������������\n\n");
		}
		
		/* read temporary contact object with values from position array */
		ajIntTempFirstPosition = ajIntColumn;
		ajIntTempSecondPosition = ajInt2dGet(ajpInt2dCmapPositions,
						     ajIntRow,
						     ajIntColumn);

		/* load up temporary contact object with values from residue position array */
		embpContactTemp->ajIntFirstPosition = ajIntTempFirstPosition;
		embpContactTemp->ajIntSecondPosition = ajIntTempSecondPosition;
		embpContactTemp->ajpStrFirstResType = ajpStrTempFirstResType;
		embpContactTemp->ajpStrSecondResType = ajpStrTempSecondResType;
		
		embWriteContact(ajpFileCmap,
				embpContactTemp);
	    }
	}
    }

    /* free contact */
    embContactDel(&embpContactTemp);
    
    return ajBoolCmapFileWritten;
}



/* @func embWriteUpdatedCmapFile *********************************************
**
** writes ajpdb contact map file from header two arrays plus contact scores
**  for each pair of residues
**
** @param [u] ajpFileUpdatedCmap [AjPFile] output file stream of current cmap
** @param [r] ajIntSeqLen [ajint] number of residues in chain sequence
** @param [w] pAjpInt2dCmapSummary [AjPInt2d*] summary of contact map
** @param [w] pEmbpCmapHeader [EmbPCmapHeader*] Contact map header
** @param [w] pAjpInt2dCmapResTypes [AjPInt2d*] contacts as residue types
** @param [w] pAjpInt2dCmapPositions [AjPInt2d*] contacts as positions in
**                                               chain
** @param [r] ajpMatrixfContactScoring [const AjPMatrixf] scoring matrix
** @return [AjBool] ajTrue if file successfully written
** @@
******************************************************************************/

AjBool embWriteUpdatedCmapFile (AjPFile ajpFileUpdatedCmap,
				ajint ajIntSeqLen,
				AjPInt2d *pAjpInt2dCmapSummary,
				EmbPCmapHeader *pEmbpCmapHeader,
				AjPInt2d *pAjpInt2dCmapResTypes,
				AjPInt2d *pAjpInt2dCmapPositions,
				const AjPMatrixf ajpMatrixfContactScoring)
{
    AjBool ajBoolUpdatedCmapFileWritten = ajFalse; /* has the file been written? */

    /* counters and limits */
    ajint ajIntRow;
    ajint ajIntColumn;
    ajint ajIntColumnLen;

    /* to store contact attributes from contact map */
    ajint ajIntFirstResType;
    ajint ajIntSecondResType;
    ajint ajIntScoreRow;
    ajint ajIntScoreColumn;
    AjPStr ajpStrFirstResType;
    AjPStr ajpStrSecondResType;
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;

     /* object to hold single contact */
    EmbPContact embpContact = NULL;
    /* structure to hold header text */
    EmbPCmapHeader embpCmapHeader = NULL;

    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* array from contact scoring matrix */
    float** floatArray2dScoring = NULL;
    float fContactScore;

    /* dereference pointers to original pointers */
    embpCmapHeader = *pEmbpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;

    /* convert the input float AjpMatrix to a 2D array of scores */ 
    floatArray2dScoring = ajMatrixfArray(ajpMatrixfContactScoring);

    /* check file passed to function is usable */	
    if(!ajpFileUpdatedCmap)
    {	
	ajWarn("function embWriteCmapFile cannot open passed filestream");
	return ajFalse;
    }

    /* write header to file */
    ajBoolUpdatedCmapFileWritten = embWriteCmapHeader(ajpFileUpdatedCmap,
						      embpCmapHeader);

    /* reserve memory for current contact object */
    embpContact = embContactNew();

    fContactScore = 0.0;

    /* XXXX OUTER LOOP OVER FIRST RESIDUES IN CONTACTS */
    for(ajIntColumn = enumArrayOffset;ajIntColumn < ajIntSeqLen;ajIntColumn++)
    {
	/* find end of each column */
	ajIntColumnLen = ajInt2dGet(ajpInt2dCmapSummary,
				    enumLastContactIndex,
				    ajIntColumn);

/* 	ajFmtPrint("��������������������������������\n"); */
/* 	ajFmtPrint("������ COLUMN LENGTH: %d ������\n", ajIntColumnLen); */
/* 	ajFmtPrint("��������������������������������\n\n"); */

	ajIntFirstResType = ajInt2dGet(ajpInt2dCmapSummary,
				       enumFirstResTypeIndex,
				       ajIntColumn);
	ajpStrFirstResType = embAjint1ToString3(ajIntFirstResType);

	if (ajIntColumnLen > enumZeroContacts)
	{

	    /* XXXX INNER LOOP OVER CONTACTS IN COLUMN */
	    for(ajIntRow = 0;ajIntRow < ajIntColumnLen;ajIntRow++)
	    {
		ajIntSecondResType = ajInt2dGet(ajpInt2dCmapResTypes,
						ajIntRow,
						ajIntColumn);	    

		/* load up temporary contact object with values from
                   residue type array */
		ajpStrSecondResType = embAjint1ToString3(ajIntSecondResType);

		if( enumDebugLevel > 1 )
		{
		    
		    ajFmtPrint("��������������������������������\n");
		    ajFmtPrint("���� first residue type: %S ����\n",
			       ajpStrFirstResType);
		    ajFmtPrint("��� second residue type: %S ����\n",
			       ajpStrSecondResType);
		    ajFmtPrint("��������������������������������\n\n");
		}
		
		/* read temporary contact object with values from
                   position array */
		ajIntFirstPosition = ajIntColumn;
		ajIntSecondPosition = ajInt2dGet(ajpInt2dCmapPositions,
						 ajIntRow,
						 ajIntColumn);

		/*
		 * load up temporary contact object with
		 * values from residue position array
		 */
		embpContact->ajIntFirstPosition  = ajIntFirstPosition;
		embpContact->ajIntSecondPosition = ajIntSecondPosition;
		embpContact->ajpStrFirstResType  = ajpStrFirstResType;
		embpContact->ajpStrSecondResType = ajpStrSecondResType;

		/* get probability of two residues contact each other */
		ajIntScoreRow = embAjintToScoringMatrixIndex(ajIntFirstResType);
		if (ajIntScoreRow == enumNoResTypeMatch )
		{	
		    ajDie("non-existent residue row value!");	
		    return ajFalse;
		}
		
		ajIntScoreColumn = embAjintToScoringMatrixIndex(ajIntSecondResType);
		if (ajIntScoreColumn == enumNoResTypeMatch )
		{	
		    ajDie("non-existent residue column value!");	
		    return ajFalse;
		}

		fContactScore =
		    (float)(floatArray2dScoring[ajIntScoreRow][ajIntScoreColumn]);
		
		embWriteUpdatedContact(ajpFileUpdatedCmap,
				       embpContact,
				       fContactScore);
	    }
	}
    }

    /* free contact */
    embContactDel(&embpContact);
    
    return ajBoolUpdatedCmapFileWritten;
}



/* @func embWriteContact ******************************************************
**
** writes contact object to open contact map file
**
** @param [w] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] embpContactToWrite [const EmbPContact] contact to be written
** @return [void]
** @@
******************************************************************************/

void embWriteContact (AjPFile ajpFileUpdatedCmap,
		      const EmbPContact embpContactToWrite)
{
    /* write contact object attributes in EMBL-like format */
    ajFmtPrintF(ajpFileUpdatedCmap,
		"SM   %S %d ; %S %d\n",
		embpContactToWrite->ajpStrFirstResType,
		embpContactToWrite->ajIntFirstPosition,
		embpContactToWrite->ajpStrSecondResType,
		embpContactToWrite->ajIntSecondPosition);
}



/* @func embWriteUpdatedContact **********************************************
**
** writes updated contact object to open contact map file
**
** @param [w] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] embpContactToWrite [const EmbPContact] contact to be written
** @param [r] fContactScore [float] probability score of contact pair
** @return [void]
** @@
******************************************************************************/

void embWriteUpdatedContact (AjPFile ajpFileUpdatedCmap,
			     const EmbPContact embpContactToWrite,
			     float fContactScore)
{
    /* write contact object attributes in EMBL-like format */
    ajFmtPrintF(ajpFileUpdatedCmap,
		"SM   %S %d ; %S %d\t%f\n",
		embpContactToWrite->ajpStrFirstResType,
		embpContactToWrite->ajIntFirstPosition,
		embpContactToWrite->ajpStrSecondResType,
		embpContactToWrite->ajIntSecondPosition,
		fContactScore);
}



/* @func embWriteCmapHeader *************************************************
**
** writes header object to open contact map file
**
** @param [w] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] embpHeaderToWrite [const EmbPCmapHeader] contact to be written
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

AjBool embWriteCmapHeader (AjPFile ajpFileUpdatedCmap,
			   const EmbPCmapHeader embpHeaderToWrite)
{
    AjBool ajBoolSuccess;
    AjPStr ajpStrBlankLine = NULL;
    AjPStr ajpStrTemp = NULL;

    ajBoolSuccess = ajFalse;

    ajpStrBlankLine = ajStrNewC("XX");
    
    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    embpHeaderToWrite->ajpStrCmapId);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    ajpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    embpHeaderToWrite->ajpStrCmapDe);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    ajpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    embpHeaderToWrite->ajpStrCmapEx);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    ajpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    embpHeaderToWrite->ajpStrCmapMo);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    ajpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    embpHeaderToWrite->ajpStrCmapIn);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    ajpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    embpHeaderToWrite->ajpStrCmapSq);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    ajpStrBlankLine);

    ajStrDel(&ajpStrTemp);
    ajStrDel(&ajpStrBlankLine);
    

    return ajBoolSuccess;

}



/* @func embWriteCmapLine ****************************************
**
** writes single AjPStr to open contact map file
**
** @param [u] ajpFileUpdatedCmap [AjPFile] file to write AjPStr to
** @param [r] ajpStrLineToWrite [const AjPStr] line to be written
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

AjBool embWriteCmapLine (AjPFile ajpFileUpdatedCmap,
			 const AjPStr ajpStrLineToWrite)
{
    AjBool ajBoolSuccess;
    AjPStr ajpStrTemp = NULL;
    ajint ajIntStrTempLen;

    ajBoolSuccess = ajFalse;

    ajStrAssS(&ajpStrTemp, ajpStrLineToWrite);
    ajStrAppK(&ajpStrTemp, '\n');

    ajIntStrTempLen = ajStrLen(ajpStrTemp);

    ajBoolSuccess = ajFileWriteStr(ajpFileUpdatedCmap,
				   ajpStrTemp,
				   ajIntStrTempLen);
    ajStrDel(&ajpStrTemp);

    return ajBoolSuccess;

}








/* DEBUGGING FUNCTIONS ONLY FROM HERE DOWNWARDS*/


/* @func debug_cmap_header ***************************************************
**
** prints out a contact map header
**
** @param [r] embpCmapHeaderToPrint [const EmbPCmapHeader] header to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

AjBool debug_cmap_header (const EmbPCmapHeader embpCmapHeaderToPrint)
{

    AjBool ajBoolSuccess; /* did we print it? */

    ajBoolSuccess = ajFalse;

    /* print out each line in contact map header in order */
    ajFmtPrint("ID   %S\n", embpCmapHeaderToPrint->ajpStrCmapId);
    ajFmtPrint("XX\n");
    ajFmtPrint("DE   %S\n", embpCmapHeaderToPrint->ajpStrCmapDe);
    ajFmtPrint("XX\n");
    ajFmtPrint("MO   %S\n", embpCmapHeaderToPrint->ajpStrCmapMo);
    ajFmtPrint("XX\n");
    ajFmtPrint("CN   %S\n", embpCmapHeaderToPrint->ajpStrCmapCn);
    ajFmtPrint("XX\n");
    ajFmtPrint("IN   %S\n", embpCmapHeaderToPrint->ajpStrCmapIn);
    ajFmtPrint("XX\n");
    ajFmtPrint("SQ   %S\n", embpCmapHeaderToPrint->ajpStrCmapSq);
    ajFmtPrint("XX\n");
	
    ajBoolSuccess = ajTrue;
    
    return ajBoolSuccess; 
}




/* @func embPrintContact ******************************************************
**
** prints out contact object 
**
** @param [r] embpContactToPrint [const EmbPContact] contact to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

AjBool embPrintContact (const EmbPContact embpContactToPrint)
{
    AjBool ajBoolSuccess;
    ajBoolSuccess= ajFalse;

    /* print contact object attributes in EMBL-like format */
    ajFmtPrint("SM   ");
    ajFmtPrint("%S %d ; %S %d\n",
	       embpContactToPrint->ajpStrFirstResType,
	       embpContactToPrint->ajIntFirstPosition,
	       embpContactToPrint->ajpStrSecondResType,
	       embpContactToPrint->ajIntSecondPosition);

    ajBoolSuccess= ajTrue;
    
    return ajBoolSuccess;
}



/* 17Mar04              debug_int_map()                  Damian Counsell  */
/*                                                                        */
/* checks that array of strings of one-letter contacts is properly built  */

void debug_int_map(AjPInt2d *pAjpInt2dCmapSummary,
			  AjPInt2d *pAjpInt2dCmapResTypes,
			  AjPInt2d *pAjpInt2dCmapPositions,
			  char *pcSeq,
			  ajint ajIntSeqLen)
{
    AjPStr ajpStrResTypeTemp;
    
    ajint ajIntRowCount;
    ajint ajIntColumnCount;
    ajint ajIntTempLastContact;
    ajint ajIntTempFirstPosition;
    ajint ajIntTempSecondPosition;
    
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    
    /* dereference pointers */
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;

    ajFmtPrint("====================================================================================================\n");
    ajFmtPrint("====================================================================================================\n");
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
    {
	ajIntTempLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntColumnCount);
	ajFmtPrint("current position:\t%4d current residue type: %c\tnumber of contacts: %4d",
		   ajIntColumnCount,
		   pcSeq[ajIntColumnCount],
		   ajIntTempLastContact);
	
	ajpStrResTypeTemp = embAjint1ToString3( ajInt2dGet(ajpInt2dCmapSummary,
							  enumFirstResTypeIndex,
							  ajIntColumnCount));

	ajFmtPrint("\tcurrent int residue type: %S:  ", ajpStrResTypeTemp);
	
	ajIntTempFirstPosition = ajInt2dGet(ajpInt2dCmapSummary,
					    enumFirstResTypeIndex,
					    ajIntColumnCount);
	
	if(ajIntTempLastContact)
	    for (ajIntRowCount = 0;ajIntRowCount < ajIntTempLastContact;ajIntRowCount++)
	    {
		ajIntTempSecondPosition = ajInt2dGet(ajpInt2dCmapPositions,
						     ajIntRowCount,
						     ajIntColumnCount);
		ajpStrResTypeTemp = embAjint1ToString3( ajInt2dGet(ajpInt2dCmapResTypes,
								  ajIntRowCount,
								  ajIntColumnCount));
		ajFmtPrint("%S", ajpStrResTypeTemp);
		ajFmtPrint("%3d  ", ajIntTempSecondPosition);
	    }
	ajFmtPrint("\n");
	
    }

  ajFmtPrint("====================================================================================================\n");
  ajFmtPrint("====================================================================================================\n");

  return;
}

/* 24Mar04              debug_cmap_summary()                  Damian Counsell  */
/*                                                                            */
/* checks that contact summary array is properly built                        */

void debug_cmap_summary(AjPInt2d *pAjpInt2dCmapSummary,
			       ajint ajIntSeqLen)
{
    ajint ajIntColumn;
    ajint ajIntFirstResType;
    ajint ajIntLastContact;
    
    AjPStr ajpStrFirstResType;

    AjPInt2d ajpInt2dCmapSummary = NULL;
    
    /* dereference pointer to summary array */
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;

    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");

    for(ajIntColumn = 0; ajIntColumn < ajIntSeqLen; ajIntColumn++)
    {
	    ajIntFirstResType = ajInt2dGet(ajpInt2dCmapSummary,
					   enumFirstResTypeIndex,
					   ajIntColumn);
	    
	    ajIntLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntColumn);

	    ajpStrFirstResType = embAjint1ToString3(ajIntFirstResType);

	    ajFmtPrint("COLUMN %d TYPE OF FIRST RESIDUE IN CONTACT: %S\t LAST CONTACT NUMBER: %d\n",
		       ajIntColumn,
		       ajpStrFirstResType,
		       ajIntLastContact);
    }

    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");

    return;
}
