/******************************************************************************
** @source AJAX REPORT (ajax feature reporting) functions
**
** These functions report AJAX sequence feature data in a variety
** of formats.
**
** @author Copyright (C) 2000 Peter Rice, LION Bioscience Ltd.
** @version 1.0
** @modified Nov 10 First version
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

#include <stddef.h>
#include <stdarg.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#include "ajax.h"




/* @datastatic ReportPFormat **************************************************
**
** Ajax feature report formats 
**
** @attr Name [char*] Format name
** @attr Desc [char*] Format description
** @attr Alias [AjBool] Name is an alias for an identical definition
** @attr Mintags [ajint] Minimum number of special tags needed
** @attr Showseq [AjBool] ajTrue if sequence is to be included
** @attr Nuc [AjBool] ajTrue if format can work with nucleotide sequences
** @attr Prot [AjBool] ajTrue if format can work with protein sequences
** @attr Showheader [AjBool] ajTrue if header appears in output
** @attr Write [(void*)] Function to write report
******************************************************************************/

typedef struct ReportSFormat
{
    char *Name;
    char *Desc;
    AjBool Alias;
    ajint Mintags;
    AjBool Showseq;
    AjBool Nuc;
    AjBool Prot;
    AjBool Showheader;
    void (*Write) (AjPReport outrpt,
		   const AjPFeattable ftable, const AjPSeq seq);
} ReportOFormat;

#define ReportPFormat ReportOFormat*

static AjPRegexp reportTagExp = NULL;



static void reportWriteEmbl(AjPReport outrpt, const AjPFeattable ftable,
			    const AjPSeq seq);
static void reportWriteGenbank(AjPReport outrpt, const AjPFeattable ftable,
			       const AjPSeq seq);
static void reportWriteGff(AjPReport outrpt, const AjPFeattable ftable,
			   const AjPSeq seq);
static void reportWritePir(AjPReport outrpt, const AjPFeattable ftable,
			   const AjPSeq seq);
static void reportWriteSwiss(AjPReport outrpt,const  AjPFeattable ftable,
			     const AjPSeq seq);

static void reportWriteTrace(AjPReport outrpt, const AjPFeattable ftable,
			     const AjPSeq seq);
static void reportWriteListFile(AjPReport outrpt, const AjPFeattable ftable,
				const AjPSeq seq);

static void reportWriteDbMotif(AjPReport outrpt, const AjPFeattable ftable,
			       const AjPSeq seq);
static void reportWriteDiffseq(AjPReport outrpt, const AjPFeattable ftable,
			       const AjPSeq seq);
static void reportWriteDraw(AjPReport outrpt, const AjPFeattable ftable,
			   const AjPSeq seq);
static void reportWriteExcel(AjPReport outrpt, const AjPFeattable ftable,
			     const AjPSeq seq);
static void reportWriteFeatTable(AjPReport outrpt, const AjPFeattable ftable,
				 const AjPSeq seq);
static void reportWriteMotif(AjPReport outrpt, const AjPFeattable ftable,
			     const AjPSeq seq);
static void reportWriteNameTable(AjPReport outrpt, const AjPFeattable ftable,
				 const AjPSeq seq);
static void reportWriteRegions(AjPReport outrpt, const AjPFeattable ftable,
			       const AjPSeq seq);
static void reportWriteSeqTable(AjPReport outrpt, const AjPFeattable ftable,
				const AjPSeq seq);
static void reportWriteSimple(AjPReport outrpt, const AjPFeattable ftable,
			      const AjPSeq seq);
static void reportWriteSrs(AjPReport outrpt, const AjPFeattable ftable,
			   const AjPSeq seq);
static void reportWriteSrsFlags(AjPReport outrpt, const AjPFeattable ftable,
				const AjPSeq seq, AjBool withSeq);
static void reportWriteTable(AjPReport outrpt, const AjPFeattable ftable,
			     const AjPSeq seq);

static void reportWriteTagseq(AjPReport outrpt, const AjPFeattable ftable,
			      const AjPSeq seq);

static char* reportCharname(const AjPReport thys);




/* @funclist reportFormat *****************************************************
**
** Functions to write feature reports
**
******************************************************************************/

static ReportOFormat reportFormat[] =
{
/*   Name         Description */
/*       Alias MinTags Showseq Nuc     Prot     Header   Function */
   /* standard feature formats */
/*    {"unknown",   "Unknown feature format",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJFALSE, AJFALSE, reportWriteSimple},*/
    {"embl",      "EMBL feature format",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJFALSE, AJFALSE, reportWriteEmbl},
    {"genbank",   "Genbank feature format",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJFALSE, AJFALSE, reportWriteGenbank},
    {"gff",       "GFF feature format",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJFALSE, reportWriteGff},
    {"pir",       "PIR feature format",
	 AJFALSE, 0, AJFALSE, AJFALSE, AJTRUE,  AJFALSE, reportWritePir},
    {"swiss",     "Swissprot feature format",
	 AJFALSE, 0, AJFALSE, AJFALSE, AJTRUE,  AJFALSE, reportWriteSwiss},
    /* trace  for debug */
    {"trace",     "Debugging trace of full internal data content",
	 AJFALSE, 0, AJTRUE,  AJTRUE,  AJTRUE,  AJTRUE,  reportWriteTrace},
    /* list file for input to other programs */
    {"listfile",  "EMBOSS list file of sequence USAs with ranges",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteListFile},
    /* feature reports */
    {"dbmotif",   "Motif database hits",
	 AJFALSE, 0, AJTRUE,  AJTRUE,  AJTRUE,  AJTRUE,  reportWriteDbMotif},
    {"diffseq",   "Differences between a pair of sequences",
	 AJFALSE, 0, AJTRUE,  AJTRUE,  AJTRUE,  AJTRUE,  reportWriteDiffseq},
/*    cirdna/lindna input format - looks horrible in those programs */
/*    {"draw",      "",
	 0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteDraw},*/
    {"excel",     "Tab-delimited file for import to Microsoft Excel",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJFALSE, reportWriteExcel},
    {"feattable", "EMBL format feature table with internal tags",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJFALSE, reportWriteFeatTable},
    {"motif",     "Motif report",
	 AJFALSE, 0, AJTRUE,  AJTRUE,  AJTRUE,  AJTRUE,  reportWriteMotif},
    {"nametable", "Simple table with sequence name",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteNameTable},
    {"regions",   "Annotated sequence regions",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteRegions},
    {"seqtable",  "Simple table with sequence on each line",
	 AJFALSE, 0, AJTRUE,  AJTRUE,  AJTRUE,  AJTRUE,  reportWriteSeqTable},
    {"simple",    "Simple report",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteSimple},
    {"srs",       "Simple report format for SRS",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteSrs},
    {"table",     "Simple table",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteTable},
    {"tagseq",    "Sequence with features marked below",
	 AJFALSE, 0, AJFALSE, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteTagseq},
    {NULL, NULL, AJFALSE, 0, AJFALSE, AJFALSE, AJFALSE, AJFALSE, NULL}
};




/* @funcstatic reportWriteTrace ***********************************************
**
** Writes a report in Trace format
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteTrace(AjPReport thys, const AjPFeattable ftable,
			     const AjPSeq seq)
{
    ajReportWriteHeader(thys, ftable, seq);

    ajFmtPrintF(thys->File, "Trace output\n");

    ajReportWriteTail(thys, ftable, seq);

    return;
}




/* @funcstatic reportWriteEmbl ************************************************
**
** Writes a report in EMBL format
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteEmbl(AjPReport thys,
			    const AjPFeattable ftable, const AjPSeq seq)
{
    static AjPStr ftfmt = NULL;

    if(!ftfmt)
	ajStrAssignC(&ftfmt, "embl");

   /*  ajFmtPrintF(thys->File, "#EMBL output\n"); */

    ajFeattabOutDel(&thys->Ftquery);
    thys->Ftquery = ajFeattabOutNewSSF(ftfmt, ajSeqGetNameS(seq),
				       ajStrGetPtr(thys->Type),
				       thys->File);
    if(!ajFeatWrite(thys->Ftquery, ftable))
	ajWarn("ajReportWriteEmbl features output failed format: '%S'",
	       ftfmt);

    return;
}




/* @funcstatic reportWriteGenbank *********************************************
**
** Writes a report in Genbank format
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]const 
** @@
******************************************************************************/

static void reportWriteGenbank(AjPReport thys,
			       const AjPFeattable ftable, const AjPSeq seq)
{
    static AjPStr ftfmt = NULL;

    if(!ftfmt)
	ajStrAssignC(&ftfmt, "genbank");

    /* ajFmtPrintF(thys->File, "#Genbank output\n"); */

    ajFeattabOutDel(&thys->Ftquery);
    thys->Ftquery = ajFeattabOutNewSSF(ftfmt, ajSeqGetNameS(seq),
				       ajStrGetPtr(thys->Type),
				       thys->File);
    if(!ajFeatWrite(thys->Ftquery, ftable))
	ajWarn("ajReportWriteGenbank features output failed format: '%S'",
	       ftfmt);

    return;
}




/* @funcstatic reportWriteGff *************************************************
**
** Writes a report in GFF format
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteGff(AjPReport thys,
			   const AjPFeattable ftable, const AjPSeq seq)
{
    AjPStr ftfmt = NULL;

    if(!ftfmt)
	ajStrAssignC(&ftfmt, "gff");

    ajFeattabOutDel(&thys->Ftquery);
    thys->Ftquery = ajFeattabOutNewSSF(ftfmt, ajSeqGetNameS(seq),
				       ajStrGetPtr(thys->Type),
				       thys->File);
    if(!ajFeatWrite(thys->Ftquery, ftable))
	ajWarn("ajReportWriteGff features output failed format: '%S'",
	       ftfmt);
    ajStrDel(&ftfmt);

    return;
}




/* @funcstatic reportWritePir *************************************************
**
** Writes a report in PIR format
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWritePir(AjPReport thys,
			   const AjPFeattable ftable, const AjPSeq seq)
{
    static AjPStr ftfmt = NULL;

    if(!ftfmt)
	ajStrAssignC(&ftfmt, "pir");

    ajFeattabOutDel(&thys->Ftquery);
    thys->Ftquery = ajFeattabOutNewSSF(ftfmt, ajSeqGetNameS(seq),
				       ajStrGetPtr(thys->Type),
				       thys->File);
    if(!ajFeatWrite(thys->Ftquery, ftable))
	ajWarn("ajReportWritePir features output failed format: '%S'",
	       ftfmt);

    return;
}




/* @funcstatic reportWriteSwiss ***********************************************
**
** Writes a report in SwissProt format
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteSwiss(AjPReport thys,
			     const AjPFeattable ftable, const AjPSeq seq)
{
    static AjPStr ftfmt = NULL;

    if(!ftfmt)
	ajStrAssignC(&ftfmt, "swissprot");

    ajFeattabOutDel(&thys->Ftquery);
    thys->Ftquery = ajFeattabOutNewSSF(ftfmt, ajSeqGetNameS(seq),
				       ajStrGetPtr(thys->Type),
				       thys->File);
    if(!ajFeatWrite(thys->Ftquery, ftable))
	ajWarn("ajReportWriteSwiss features output failed format: '%S'",
	       ftfmt);

    return;
}




/* @funcstatic reportWriteDbMotif *********************************************
**
** Writes a report in DbMotif format
**
** Format:<br>
** Length = [length] <br>
** Start = position [start] of sequence <br>
** End = position [start] of sequence <br>
** ... other tags ... <br>
** [sequence] <br>
** [start and end numbered below sequence] <br>
**
** Data reported: Length, Start, End, Sequence (5 bases around feature)
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: <br>
**   all tags reported as name = value
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteDbMotif(AjPReport thys,
			       const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    ajint ilen    = 0;
    AjPStr subseq = NULL;
    AjPStr tmpstr = NULL;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;
    ajint jstart;
    ajint jend;

    outf = thys->File;
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart  = feature->Start;
	iend    = feature->End;
	ilen    = iend - istart + 1;
	
	jstart = AJMAX(0, istart-6);
	jend   = AJMIN(ajSeqGetLen(seq), iend+4);
	
	ajStrAssignResC(&tmpstr, ilen+10, "");
	for(j=istart+1; j<iend; j++)
	    ajStrAppendK(&tmpstr, ' ');
	
	ajStrAssignSubS(&subseq, ajSeqStr(seq), jstart, jend);
	/* ajStrFmtUpper(&subseq); */
	ajFmtPrintF(outf, "Length = %d\n", ilen);
	ajFmtPrintF(outf, "Start = position %d of sequence\n", istart);
	ajFmtPrintF(outf, "End = position %d of sequence\n\n", iend);

	for(j=0; j < ntags; j++)
	    if(ajFeatGetNote(feature, tagnames[j], &tagval))
		ajFmtPrintF(outf, "%S = %S\n", tagprints[j], tagval);

	ajFmtPrintF(outf, "\n");
	
	ajFmtPrintF(outf, "%S\n", subseq);
	if(istart == iend)
	{
	    ajFmtPrintF(outf, "%*s|\n",(istart-jstart-1), " ");
	    ajFmtPrintF(outf, "%*d\n\n", AJMIN(6, istart), istart);
	}
	else
	{
	    ajFmtPrintF(outf, "%*s|%S|\n", (istart-jstart-1), " ", tmpstr);
	    ajFmtPrintF(outf, "%*d%S%d\n\n",
			AJMIN(6, istart),
			istart, tmpstr, iend);
	}
	ajStrDel(&tmpstr);
    }
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);
    
    ajReportWriteTail(thys, ftable, seq);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteDiffseq *********************************************
**
** Writes a report in Diffseq format, based on the output from the
** diffseq application. The report describes matches, usually short,
** between two sequences and features which overlap them.
**
** A number of tags are used. The rpeort makes little sense without them.
** These tags are used to replicate features in a second sequence.
**
** Format:<br>
**   [Name] [start]-[end] Length: [length] <br>
**   Feature: [special first_feature tag] <br>
**   Sequence: [sequence] <br>
**   Sequence: [special sequence tag] <br>
**   Feature: [special second_feature tag] <br>
**   [as first line, using special tags name, start, end, length] <br>
**
** Data reported: Name, Start, End, Length, Sequence
**
** Tags required: None
**
** Tags used: <br>
**   start : start position in second sequence <br>
**   end : end in second sequence <br>
**   length : length of match in second sequence <br>
**   name : name of second sequence (set by ajReportSeqName) <br>
**   sequence : sequence of match in second sequence <br>
**   first_feature : feature(s) in first sequence <br>
**   second_feature : feature(s) in second sequence <br>
**
** Tags reported: None
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteDiffseq(AjPReport thys,
			       const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;

    ajint istart = 0;
    ajint jstart = 0;
    ajint iend   = 0;
    ajint jend   = 0;
    ajint ilen   = 0;
    ajint jlen   = 0;

    AjPStr subseq = NULL;
    ajint i       = 0;
    ajint ifeat   = 0;
    AjPStr tmpstr = NULL;
    AjPStr jname  = NULL;

    AjPStr firstfeat  = NULL;
    AjPStr secondfeat = NULL;
    AjPStr tagval     = NULL;
    
    outf = thys->File;
    
    ajReportWriteHeader(thys, ftable, seq);
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart  = feature->Start;
	iend    = feature->End;
	ilen    = iend - istart + 1;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	i++;
	
	if(!ajFeatGetNoteC(feature, "start", &tagval))
	    jstart = 0;
	else
	    ajStrToInt(tagval, &jstart);

	
	if(!ajFeatGetNoteC(feature, "end", &tagval))
	    jend = 0;
	else
	    ajStrToInt(tagval, &jend);
	
	if(!ajFeatGetNoteC(feature, "length", &tagval))
	    jlen = 0;
	else
	    ajStrToInt(tagval, &jlen);
	
	if(!ajFeatGetNoteC(feature, "name", &jname))
	    ajStrAssignC(&jname, "");
	
	if(ilen > 0)
	{
	    ajFmtPrintF(outf, "\n%S %d-%d Length: %d\n",
			ajReportSeqName(thys, seq), istart, iend, ilen);
	    ifeat = 1;
	    while(ajFeatGetNoteCI(feature, "first_feature",
				  ifeat++, &firstfeat))
		ajFmtPrintF(outf, "Feature: %S\n", firstfeat);

	    ajFmtPrintF(outf, "Sequence: %S\n", subseq);
	}
	else
	{
	    ajFmtPrintF(outf, "\n%S %d Length: %d\n",
			ajReportSeqName(thys, seq), istart, ilen);
	    ifeat = 1;
	    while(ajFeatGetNoteCI(feature, "first_feature",
				  ifeat++, &firstfeat))
		ajFmtPrintF(outf, "Feature: %S\n", firstfeat);
	    ajFmtPrintF(outf, "Sequence: \n");
	}
	
	if(!ajFeatGetNoteC(feature, "sequence", &subseq))
	    ajStrAssignC(&subseq, "");
	
	if(jlen > 0)
	{
	    ajFmtPrintF(outf, "Sequence: %S\n", subseq);
	    ifeat = 1;
	    while(ajFeatGetNoteCI(feature, "second_feature",
				  ifeat++, &secondfeat))
		ajFmtPrintF(outf, "Feature: %S\n", secondfeat);

	    ajFmtPrintF(outf, "%S %d-%d Length: %d\n",
			jname, jstart, jend, jlen);
	}
	else
	{
	    ajFmtPrintF(outf, "Sequence: \n");
	    ifeat = 1;
	    while(ajFeatGetNoteCI(feature, "second_feature",
				  ifeat++, &secondfeat))
		ajFmtPrintF(outf, "Feature: %S\n", secondfeat);
	    ajFmtPrintF(outf, "%S %d Length: %d\n",
			jname, jstart, jlen);
	}
	
	ajStrDelStatic(&tmpstr);
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tmpstr);
    ajStrDel(&tagval);
    ajStrDel(&jname);
    
    ajListIterFree(&iterft);

    return;
}




/* @funcstatic reportWriteDraw ************************************************
**
** Writes a report in Draw format, for use as input to cirdna or lindna
**
** Format:<br>
**   group<br>
**<br>
**   label<br>
**   tick  [tagvalue] [start] 8<br>
**   endlabel<br>
**   label<br>
**   tick  [tagvalue] [end] 3<br>
**   endlabel<br>
**<br>
**   endgroup<br>
**
** Data reported:
**
** Tags required: Enzyme_name, 5prime, 3primem 5primerev, 3primerev
**
** Tags used:
**
** Tags reported:
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteDraw(AjPReport thys,
			    const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    float score   = 0.0;
    AjPStr subseq = NULL;
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;
    ajint jenz = -1;
    ajint j5 = -1;
    ajint j3 = -1;
    ajint j5rev = -1;
    ajint j3rev = -1;
    ajint jstart;
    ajint jend;
    outf = thys->File;
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    for(j=0; j < ntags; j++)
    {
	if(ajStrMatchCaseC(tagnames[j], "enzyme"))
	    jenz = j;
	if(ajStrMatchCaseC(tagnames[j], "5prime"))
	    j5 = j;
	if(ajStrMatchCaseC(tagnames[j], "3prime"))
	    j3 = j;
	if(ajStrMatchCaseC(tagnames[j], "5prime_rev"))
	    j5rev = j;
	if(ajStrMatchCaseC(tagnames[j], "3prime_rev"))
	    j3rev = j;
    }

    ajFmtPrintF(outf, "Start %d\n", 
 		ajSeqGetBegin(seq) + ajSeqGetOffset(seq));
    ajFmtPrintF(outf, "End   %d\n", 
 		ajSeqGetEnd(seq) + ajSeqGetOffset(seq));

    ajFmtPrintF(outf, "\n"); 
    ajFmtPrintF(outf, "group\n");
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);

	istart = feature->Start;
	iend = feature->End;

	score = feature->Score;

	ajFmtPrintF(outf, "label\n");
	if (j5 >= 0)
	{
	    ajFeatGetNote(feature, tagnames[j5], &tagval);
	    ajStrToInt(tagval, &jstart);
	    ajFmtPrintF(outf, "Tick %d 8\n", jstart);
	    if (jenz >= 0)
	    {
		ajFeatGetNote(feature, tagnames[jenz], &tagval);
		ajFmtPrintF(outf, "%S\n", tagval);
	    }
	    else
		ajFmtPrintF(outf, "Enz\n");
	    ajFmtPrintF(outf, "endlabel\n");
	}
	if (j3 >= 0)
	{
	    ajFmtPrintF(outf, "label\n");
	    ajFeatGetNote(feature, tagnames[j3], &tagval);
	    ajStrToInt(tagval, &jend);
	    ajFmtPrintF(outf, "Tick %d 3\n", jend);
	    if (jenz >= 0)
	    {
		ajFeatGetNote(feature, tagnames[jenz], &tagval);
		ajFmtPrintF(outf, "%S\n", tagval);
	    }
	    else
		ajFmtPrintF(outf, "Enz\n");
	    ajFmtPrintF(outf, "endlabel\n");
	}

	ajFmtPrintF(outf, "\n");
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteExcel ***********************************************
**
** Writes a report in Excel (tab delimited) format. Name, start, end
** score and strand are always reported. Other tags in the report definition
** are added as extra columns.
**
** All values are (for now) unquoted. Missing values are reported as '.'
**
** Format:<br>
**   "SeqName Start End Score" (tab delimited) <br>
**   [extra tag names added to first line] <br>
**   Name Start End Score [extra tag values] (tab delimited) <br>
**
** Data reported: Name Start End Score Strand
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All defined tags
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteExcel(AjPReport thys,
			     const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    float score   = 0.0;
    AjPStr subseq = NULL;
    AjPStr tmpstr = NULL;
    ajint i = 0;
    ajint j = 0;
    char strand;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;

    AjPStr tagval = NULL;

    outf = thys->File;
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    if(thys->Showscore)
	ajFmtPrintF(outf, "SeqName\tStart\tEnd\tScore\tStrand");
    else
	ajFmtPrintF(outf, "SeqName\tStart\tEnd\tStrand");
    
    /* then extra tags */
    for(j=0; j < ntags; j++)
	ajFmtPrintF(outf, "\t%S", tagprints[j]);
    ajFmtPrintF(outf, "\n");
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart  = feature->Start;
	iend    = feature->End;
	score   = feature->Score;
	strand = feature->Strand;
	if(strand != '-')
	    strand = '+';
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	i++;
	if(thys->Showscore)
	    ajFmtPrintF(outf, "%S\t%d\t%d\t%.*f\t%c",
			ajReportSeqName(thys, seq),
			istart, iend, thys->Precision,
			score, strand);
	else
	    ajFmtPrintF(outf, "%S\t%d\t%d\t%c",
			ajReportSeqName(thys, seq),
			istart, iend, strand);
	
	for(j=0; j < ntags; j++)
	{				/* then extra tags */
	    if(ajFeatGetNote(feature, tagnames[j], &tagval))
		ajFmtPrintF(outf, "\t%S", tagval);
	    else
		ajFmtPrintF(outf, "\t."); /* missing value '.' for now */
	}
	ajFmtPrintF(outf, "\n");
	ajStrDelStatic(&tmpstr);
    }
    
    ajStrDel(&subseq);
    ajStrDel(&tmpstr);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteFeatTable *******************************************
**
** Writes a report in FeatTable format. The report is an EMBL feature
** table using only the tags in the report definition. There is no
** requirement for tag names to match standards for the EMBL feature
** table.
**
** The original EMBOSS application for this format was cpgreport.
**
** Format:<br>
**   FT [type] [start]..[end] <br>
**                            /[tagname]=[tagvalue] <br>
**
** Data reported: Type, Start, End
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteFeatTable(AjPReport thys,const  AjPFeattable ftable,
				 const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    AjPStr subseq = NULL;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;

    outf = thys->File;
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	if(feature->Strand == '-')
	    ajFmtPrintF(outf, "FT   %-15.15S complement(%d..%d)\n",
			ajFeatGetType(feature),
			istart, iend);
	else
	    ajFmtPrintF(outf, "FT   %-15.15S %d..%d\n",
			ajFeatGetType(feature),
			istart, iend);

	for(j=0; j < ntags; j++)
	{
	    if(ajFeatGetNote(feature, tagnames[j], &tagval))
		ajFmtPrintF(outf, "FT                   /%S=\"%S\"\n",
			    tagprints[j], tagval);
	    else
	    {
		/* skip the missing ones for now */
		/*ajFmtPrintF(outf,
		  "FT                   /%S=\"<unknown>\"\n",
		  tagprints[j]);*/
	    }
	}
	ajFmtPrintF(outf, "\n");
    }
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteListFile ********************************************
**
** Writes a report in ListFile format for use as input to another application.
**
** Format:<br>
**   Name[start:end] <br>
**
** Data reported: Name, Start, End, Strand
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: None
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteListFile(AjPReport thys,
				const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart = 0;
    ajint iend   = 0;
    AjPStr subseq = NULL;
    ajint i = 0;
    AjPStr tmpstr = NULL;

    outf = thys->File;
    thys->Showusa = ajTrue;		/* so we get a usable USA */

    ajReportWriteHeader(thys, ftable, seq);

    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	i++;

	ajFmtPrintS(&tmpstr, "[");

	if(istart)
	{
	    if(feature->Strand == '-')
		ajFmtPrintAppS(&tmpstr, "%d", ajSeqGetLen(seq) - istart + 1);
	    else
		ajFmtPrintAppS(&tmpstr, "%d", istart);
	}
	ajFmtPrintAppS(&tmpstr, ":");

	if(iend)
	{
	    if(feature->Strand == '-')
		ajFmtPrintAppS(&tmpstr, "%d", ajSeqGetLen(seq) - iend + 1);
	    else
		ajFmtPrintAppS(&tmpstr, "%d", iend);
	}

	if(feature->Strand == '-')
	    ajFmtPrintAppS(&tmpstr, ":r");
	ajFmtPrintAppS(&tmpstr, "]");

	if(ajStrGetLen(tmpstr) > 3)
	    ajFmtPrintF(outf, "%S%S\n",
			ajReportSeqName(thys, seq), tmpstr);
	else
	    ajFmtPrintF(outf, "%S\n",
			ajReportSeqName(thys, seq));

	ajStrDelStatic(&tmpstr);
    }

    ajReportWriteTail(thys, ftable, seq);

    ajListIterFree(&iterft);
    ajStrDel(&subseq);
    ajStrDel(&tmpstr);

    return;
}




/* @funcstatic reportWriteMotif ***********************************************
**
** Writes a report in Motif format.  Based on the original output
** format of antigenic, helixturnhelix and sigcleave.
**
** Format:<br>
**   (1) Score [score] length [length] at [name] [start->[end] <br>
**               *  (marked at position pos) <br>
**             [sequence] <br>
**             |        | <br>
**       [start]        [end] <br>
**   [tagname]: tagvalue
**
** Data reported: Name, Start, End, Length, Score, Sequence
**
** Tags required: None
**
** Tags used: <br>
**   pos (integer, maximum score position)
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteMotif(AjPReport thys,
			     const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    float score   = 0.0;
    ajint ilen    = 0;
    AjPStr subseq = NULL;
    ajint i = 0;
    AjPStr tmpstr = NULL;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;
    
    ajint jpos = 0;
    ajint jmax = -1;
    
    outf = thys->File;
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    for(j=0; j < ntags; j++)
	if(ajStrMatchCaseC(tagnames[j], "pos"))
	{
	    jmax = j;
	    ajFmtPrintF(outf, "%S at \"*\"\n\n", tagprints[jmax]);
	    break;
	}
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	score = feature->Score;
	ilen = iend - istart + 1;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	i++;
	if(thys->Showscore)
	    ajFmtPrintF(outf, "(%d) Score %.*f length %d at %s %d->%d\n",
			i, thys->Precision, score,
			ilen, reportCharname(thys), istart, iend);
	else
	    ajFmtPrintF(outf, "(%d) length %d at %s %d->%d\n",
			i, ilen, reportCharname(thys), istart, iend);
	if(jmax >= 0)
	{
	    if(ajFeatGetNote(feature, tagnames[jmax], &tagval))
		ajStrToInt(tagval, &jpos);
	    else
		jpos = iend+1;

	    ajStrAssignResC(&tmpstr, ilen,"");
	    for(j=istart; j<jpos; j++)
		ajStrAppendK(&tmpstr, ' ');

	    ajFmtPrintF(outf, "           %S*\n", tmpstr);
	}
	
	ajFmtPrintF(outf, " Sequence: %S\n", subseq);
	ajStrAssignResC(&tmpstr, ilen,"");
	for(j=istart+1; j<iend; j++)
	    ajStrAppendK(&tmpstr, ' ');

	if(istart == iend)
	{
	    ajFmtPrintF(outf, "           |\n");
	    ajFmtPrintF(outf, "%12d\n", istart);
	}
	else
	{
	    ajFmtPrintF(outf, "           |%S|\n", tmpstr);
	    ajFmtPrintF(outf, "%12d%S%d\n", istart, tmpstr, iend);
	}

	if(ntags)
	    for(j=0; j < ntags; j++)
	    {
		if(j == jmax)
		    continue;
		if(ajFeatGetNote(feature, tagnames[j], &tagval))
		    ajFmtPrintF(outf, " %S: %S\n", tagprints[j], tagval);
	    }
	
	ajFmtPrintF(outf, "\n");
	ajStrDelStatic(&tmpstr);
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tmpstr);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteNameTable *******************************************
**
** Writes a report in NameTable format. See reportWriteSeqTable for a version
** with the sequence. See reportWriteTable for a version without the name,
** as this already appears in the header.
**
** Missing tag values are reported as '.'
** The column width is 6, or longer if the name is longer.
**
** Format:<br>
**   USA    Start   End   Score   [tagnames]
**   [name] [start] [end] [score] [tagvalues]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported:
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteNameTable(AjPReport thys, const AjPFeattable ftable,
				 const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    float score   = 0.0;
    AjPStr subseq = NULL;
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;
    ajint jwid = 6;
    ajint jmin = 6;	 /* minimum width for printing special tags */
    static AjPStr strstr = NULL;
    
    outf = thys->File;

    if(!strstr)
	ajStrAssignC(&strstr, "str");
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    if(thys->Showscore)
	ajFmtPrintF(outf,
		    "%-15s %7s %7s %7s", "USA", "Start", "End", "Score");
    else
	ajFmtPrintF(outf, "%-15s %7s %7s", "USA", "Start", "End");
    
    for(j=0; j < ntags; j++)
    {
	jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	if(ajStrMatchS(tagtypes[j], strstr))
	    ajFmtPrintF(outf, " %-*S", jwid, tagprints[j]);
	else
	    ajFmtPrintF(outf, " %*S", jwid, tagprints[j]);
    }
    ajFmtPrintF(outf, "\n");
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature =(AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	score = feature->Score;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	
	if(thys->Showscore)
	    ajFmtPrintF(outf, "%-15.15S %7d %7d %7.*f",
			ajReportSeqName(thys, seq),
			istart, iend, thys->Precision, score);
	else
	    ajFmtPrintF(outf, "%-15.15S %7d %7d",
			ajReportSeqName(thys, seq),
			istart, iend);
	for(j=0; j < ntags; j++)
	{
	    jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	    if(!ajFeatGetNote(feature, tagnames[j], &tagval))
		ajStrAssignC(&tagval, ".");
	    if(ajStrMatchS(tagtypes[j], strstr))
		ajFmtPrintF(outf, " %-*S", jwid, tagval);
	    else
		ajFmtPrintF(outf, " %*S", jwid, tagval);
	}
	ajFmtPrintF(outf, "\n");
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteRegions *********************************************
**
** Writes a report in Regions format. The report (unusually for the current
** report formats) includes the feature type.
**
** Format: <br>
**   [type] from [start] to [end] ([length] [name]) <br>
**   ([tagname]: [tagvalue], [tagname]: [tagvalue]  ...) <br>
**
** Data reported: Type, Start, End, Length, Name
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteRegions(AjPReport thys, const AjPFeattable ftable,
			       const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    float score   = 0.0;
    ajint ilen    = 0;
    AjPStr subseq = NULL;
    AjPStr tagstr = NULL;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;

    outf = thys->File;
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	score = feature->Score;
	ilen = iend - istart + 1;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */
	ajFmtPrintF(outf, "%S from %d to %d (%d %s)\n",
		    ajFeatGetType(feature), istart, iend, ilen,
		    reportCharname(thys));
	if(thys->Showscore)
	    ajFmtPrintF(outf, "   Max score: %.*f", thys->Precision, score);
	if(ntags)
	{
	    ajFmtPrintF(outf, " (");
	    for(j=0; j < ntags; j++)
		if(ajFeatGetNote(feature, tagnames[j], &tagval))
		{
		    if(j)
			ajFmtPrintF(outf, ", ");
		    ajFmtPrintF(outf, "%S: %S", tagprints[j], tagval);
		}
	    
	    ajFmtPrintF(outf, ")");	    
	}
	ajFmtPrintF(outf, "\n\n");
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tagstr);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteSeqTable ********************************************
**
** Writes a report in SeqTable format.
**
** This is a simple table format that
** includes the feature sequence. See reportWriteTable for a version
** without the sequence. Missing tag values are reported as '.'
** The column width is 6, or longer if the name is longer.
**
** Format:<br>
**   Start   End   [tagnames]  Sequence
**   [start] [end] [tagvalues] [sequence]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteSeqTable(AjPReport thys, const AjPFeattable ftable,
				const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    AjPStr subseq = NULL;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j=0;
    AjPStr tagval = NULL;
    ajint jwid = 6;
    ajint jmin = 6;	 /* minimum width for printing special tags */
    AjPStr strstr = NULL;
    
    outf = thys->File;

    if(!strstr)
	ajStrAssignC(&strstr, "str");
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    ajFmtPrintF(outf, "%7s %7s", "Start", "End");
    for(j=0; j < ntags; j++)
    {
	jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	if(ajStrMatchS(tagtypes[j], strstr))
	    ajFmtPrintF(outf, " %-*S", jwid, tagprints[j]);
	else
	    ajFmtPrintF(outf, " %*S", jwid, tagprints[j]);
    }
    ajFmtPrintF(outf, " Sequence\n");
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	if(feature->Strand == '-')
	{
	    ajStrAssignSubS(&subseq, ajSeqStr(seq), iend-1, istart-1);
	    ajSeqstrReverse(&subseq);
	}
	else
	{
	    ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	}
	/* ajStrFmtUpper(&subseq); */
	
	ajDebug("reportWriteSeqTable subseq %d seq %d %d..%d\n",
		ajStrGetLen(subseq), ajSeqGetLen(seq), istart, iend);
	
	ajFmtPrintF(outf, "%7d %7d", istart, iend);

	for(j=0; j < ntags; j++)
	{
	    jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	    if(!ajFeatGetNote(feature, tagnames[j], &tagval))
		ajStrAssignC(&tagval, ".");
	    ajDebug("reportWriteSeqTable jwid %d jmin %d tagval '%S'\n",
		    jwid, jmin, tagval);
	    if(ajStrMatchS(tagtypes[j], strstr))
		ajFmtPrintF(outf, " %-*S", jwid, tagval);
	    else
		ajFmtPrintF(outf, " %*S", jwid, tagval);
	}
	ajFmtPrintF(outf, " %S\n", subseq);
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&strstr);
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteSimple **********************************************
**
** Writes a report in SRS simple format This is a simple parsable format that
** does not include the feature sequence (see also SRS format)
** for applicatins where features can be large.
** Missing tag values are reported as '.'
**
** Format:<br>
**   Start   End   [tagnames]  Sequence
**   [start] [end] [tagvalues] [sequence]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteSimple(AjPReport thys,
			      const AjPFeattable ftable, const AjPSeq seq)
{
    static AjBool withSeq = AJFALSE;

    reportWriteSrsFlags(thys, ftable, seq, withSeq);

    return;
}




/* @funcstatic reportWriteSrs *************************************************
**
** Writes a report in SRS format This is a simple parsable format that
** includes the feature sequence.
** Missing tag values are reported as '.'
**
** Format:<br>
**   Start   End   [tagnames]  Sequence
**   [start] [end] [tagvalues] [sequence]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteSrs(AjPReport thys,
			   const AjPFeattable ftable, const AjPSeq seq)
{
    static AjBool withSeq = AJTRUE;

    reportWriteSrsFlags(thys, ftable, seq, withSeq);

    return;
}




/* @funcstatic reportWriteSrsFlags ********************************************
**
** Writes a report in SRS format.
** A flag controls whether to include the sequence.
** Missing tag values are reported as '.'
**
** Format:<br>
**   Start   End   [tagnames]  Sequence
**   [start] [end] [tagvalues] [sequence]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @param [r] withSeq [AjBool] If ajTrue, includes the sequence in the output.
** @return [void]
** @@
******************************************************************************/

static void reportWriteSrsFlags(AjPReport thys, const AjPFeattable ftable,
				const AjPSeq seq, AjBool withSeq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    ajint ilen    = 0;
    float score   = 0.0;
    AjPStr subseq = NULL;
    ajint ift     = 0;
    
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;
    
    outf = thys->File;

    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart  = feature->Start;
	iend    = feature->End;
	ilen    = iend - istart + 1;
	score   = feature->Score;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */

	/* blank line before each feature */
	/* don't write at the end, because tail always starts with a
           blank line */

	if(ift)
	    ajFmtPrintF(outf, "\n");

	ift++;

	ajFmtPrintF(outf, "Feature: %d\n", ift);
	ajFmtPrintF(outf, "Name: %S\n", ajReportSeqName(thys, seq));
	ajFmtPrintF(outf, "Start: %d\n", istart);
	ajFmtPrintF(outf, "End: %d\n", iend);
	ajFmtPrintF(outf, "Length: %d\n", ilen);
	if(withSeq)
	    ajFmtPrintF(outf, "Sequence: %S\n", subseq);

	/* We always write the score - ignore thys->Showscore */

	ajFmtPrintF(outf, "Score: %.*f\n",thys->Precision,  score);
	if(feature->Strand == '+' || feature->Strand == '-')
	    ajFmtPrintF(outf, "Strand: %c\n", feature->Strand);
	if(feature->Frame)
	    ajFmtPrintF(outf, "Frame: %d\n", feature->Frame);

	for(j=0; j < ntags; j++)
	{
	    if(ajFeatGetNote(feature, tagnames[j], &tagval))
		ajFmtPrintF(outf, "%S: %S\n", tagprints[j], tagval);
	    else
		ajFmtPrintF(outf, "%S: .\n",
			    tagprints[j]); /* missing value '.' for now */
	}
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteTable ***********************************************
**
** Writes a report in Table format. See reportWriteSeqTable for a version
** with the sequence. See reportWriteNameTable for a version with
** the name, which was the earlier format for reportWriteTable.
** The name already appears in the sequence header
**
** Missing tag values are reported as '.'
** The column width is 6, or longer if the name is longer.
**
** Format:<br>
**   USA    Start   End   Score   [tagnames]
**   [name] [start] [end] [score] [tagvalues]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported:
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteTable(AjPReport thys,
			     const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    float score   = 0.0;
    AjPStr subseq = NULL;
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    AjPStr tagval = NULL;
    ajint jwid = 6;
    ajint jmin = 6;	 /* minimum width for printing special tags */
    AjPStr strstr = NULL;

    outf = thys->File;
    
    if(!strstr)
	ajStrAssignC(&strstr, "str");
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    
    if(thys->Showscore)
	ajFmtPrintF(outf, "%7s %7s %7s", "Start", "End", "Score");
    else
	ajFmtPrintF(outf, "%7s %7s", "Start", "End");
    
    for(j=0; j < ntags; j++)
    {
	jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	if(ajStrMatchS(tagtypes[j], strstr))
	    ajFmtPrintF(outf, " %-*S", jwid, tagprints[j]);
	else
	    ajFmtPrintF(outf, " %*S", jwid, tagprints[j]);
    }
    ajFmtPrintF(outf, "\n");
    
    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);

	istart = feature->Start;
	iend = feature->End;
	score = feature->Score;
	ajStrAssignSubS(&subseq, ajSeqStr(seq), istart-1, iend-1);
	/* ajStrFmtUpper(&subseq); */

	if(thys->Showscore)
	    ajFmtPrintF(outf, "%7d %7d %7.*f",
			istart, iend, thys->Precision, score);
	else
	    ajFmtPrintF(outf, "%7d %7d",
			istart, iend);

	for(j=0; j < ntags; j++)
	{
	    jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	    if(!ajFeatGetNote(feature, tagnames[j], &tagval))
		ajStrAssignC(&tagval, ".");
	    if(ajStrMatchS(tagtypes[j], strstr))
		ajFmtPrintF(outf, " %-*S", jwid, tagval);
	    else
		ajFmtPrintF(outf, " %*S", jwid, tagval);
	}
	ajFmtPrintF(outf, "\n");
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    ajStrDel(&strstr);

    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @funcstatic reportWriteTagseq **********************************************
**
** Writes a report in Tagseq format. Features are marked up below the sequence.
** Originally developed for the garnier application, but has general uses.
**
** Format:<br>
**   Sequence (50 residues)<br>
**   tagname        ++++++++++++    +++++++++<br>
**
** If the tag value is a 1 letter code, use this instead of '+'
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported:
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteTagseq(AjPReport thys,
			      const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjIList iterft     = NULL;
    AjPFeature feature = NULL;
    ajint istart  = 0;
    ajint iend    = 0;
    ajint ilen    = 0;
    AjPStr subseq = NULL;
    AjPStr substr = NULL;
    ajint ntags;
    static AjPStr* tagtypes = NULL;
    static AjPStr* tagnames = NULL;
    static AjPStr* tagprints = NULL;
    static ajint*  tagsizes = NULL;
    ajint j = 0;
    ajint i = 0;
    AjPStr tagval = NULL;
    ajint jwid = 6;
    ajint jmin = 6;	 /* minimum width for printing special tags */
    AjPStr* seqmarkup;
    AjPStr seqnumber = NULL;
    ajint seqbeg;
    ajint seqend;
    ajint seqlen;
    ajint ilast;
    ajint jlast;

    outf = thys->File;

    seqlen = ajSeqGetLen(seq);
    seqbeg = ajSeqGetBegin(seq) + ajSeqGetOffset(seq);
    seqend = ajSeqGetEnd(seq) + ajSeqGetOffset(seq);
    
    ajReportWriteHeader(thys, ftable, seq);
    
    ntags = ajReportLists(thys, &tagtypes, &tagnames, &tagprints, &tagsizes);
    AJCNEW0(seqmarkup, ntags);
    
    for(j=0; j < ntags; j++)
    {
	jwid = AJMAX(jmin, ajStrGetLen(tagprints[j]));
	ajStrAppendCountK(&seqmarkup[j], ' ', seqlen);
    }
    
    ajFmtPrintAppS(&seqnumber, "    .%5d", 10);
    for(i=10; i < seqend-9; i+=10)
	ajFmtPrintAppS(&seqnumber, "    .%5d", i+10);

    iterft = ajListIterRead(ftable->Features);
    while(ajListIterMore(iterft))
    {
	feature = (AjPFeature)ajListIterNext(iterft);
	istart = feature->Start;
	iend = feature->End;
	ilen = iend - istart + 1;
	for(j=0; j < ntags; j++)
	    if(ajFeatGetNote(feature, tagnames[j], &tagval))
	    {
		if(ajStrGetLen(tagval))
		    ajStrPasteCountK(&seqmarkup[j], istart-1,
				  ajStrGetCharFirst(tagval), ilen);
		else
		    ajStrPasteCountK(&seqmarkup[j], istart-1, '+', ilen);
	    }
    }
    
    for(i=seqbeg-1; i < seqend; i+=50)
    {
	ilast = AJMIN(i+50-1, seqend-1);
	jlast = AJMIN(i+50-1, ajStrGetLen(seqnumber)-1);

	ajStrAssignSubS(&substr, seqnumber, i, jlast);
	ajFmtPrintF(outf, "      %S\n", substr);

	ajStrAssignSubS(&subseq, ajSeqStr(seq), i, ilast);
	/* ajStrFmtUpper(&subseq); */

	ajFmtPrintF(outf, "      %S\n", subseq);
	for(j=0; j < ntags; j++)
	{
	    ajStrAssignSubS(&substr, seqmarkup[j], i, ilast);
	    ajFmtPrintF(outf, "%5S %S\n", tagprints[j], substr);
	}
    }
    
    ajReportWriteTail(thys, ftable, seq);
    
    ajStrDel(&subseq);
    ajStrDel(&tagval);
    
    for(i=0;i<ntags;++i)
	ajStrDel(&seqmarkup[i]);
    AJFREE(seqmarkup);
    
    ajStrDel(&substr);
    ajStrDel(&seqnumber);
    ajListIterFree(&iterft);

    AJFREE(tagtypes);
    AJFREE(tagnames);
    AJFREE(tagprints);
    AJFREE(tagsizes);

    return;
}




/* @func ajReportDel **********************************************************
**
** Destructor for report objects
**
** @param [d] pthys [AjPReport*] Report object reference
** @return [void]
** @category delete [AjPReport] Default destructor
** @@
******************************************************************************/

void ajReportDel(AjPReport* pthys)
{
    AjPReport thys;
    AjPStr str = NULL;

    thys = *pthys;
    if(!thys)
	return;

    ajStrDel(&thys->Name);
    ajStrDel(&thys->Type);
    ajStrDel(&thys->Formatstr);
    ajStrDel(&thys->Extension);

    while(ajListPop(thys->FileTypes,(void **)&str))
	ajStrDel(&str);
    ajListDel(&thys->FileTypes);
    while(ajListPop(thys->FileNames,(void **)&str))
	ajStrDel(&str);
    ajListDel(&thys->FileNames);

    while(ajListPop(thys->Tagnames,(void **)&str))
	ajStrDel(&str);
    ajListDel(&thys->Tagnames);
    while(ajListPop(thys->Tagprints,(void **)&str))
	ajStrDel(&str);
    ajListDel(&thys->Tagprints);
    while(ajListPop(thys->Tagtypes,(void **)&str))
	ajStrDel(&str);
    ajListDel(&thys->Tagtypes);

    ajStrDel(&thys->Header);
    ajStrDel(&thys->SubHeader);
    ajStrDel(&thys->Tail);
    ajStrDel(&thys->SubTail);

    ajFeattableDel(&thys->Fttable);
    ajFeattabOutDel(&thys->Ftquery);

    ajFileClose(&thys->File);

    AJFREE(*pthys);

    return;
}




/* @func ajReportOpen *********************************************************
**
** Opens a new report file
**
** @param [u] thys [AjPReport] Report object
** @param [r] name [const AjPStr] File name
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajReportOpen(AjPReport thys, const AjPStr name)
{
    if(!ajReportValid(thys))
	return ajFalse;

    thys->File = ajFileNewOut(name);
    if(thys->File)
	return ajTrue;

    return ajFalse;
}




/* @func ajReportFormatDefault ************************************************
**
** Sets the default format for a feature report
**
** @param [w] pformat [AjPStr*] Default format returned
** @return [AjBool] ajTrue is format was returned
** @@
******************************************************************************/

AjBool ajReportFormatDefault(AjPStr* pformat)
{
    if(ajStrGetLen(*pformat))
	ajDebug("... output format '%S'\n", *pformat);
    else
    {
	/* ajStrAssignEmptyC(pformat, reportFormat[0].Name);*/
	ajStrAssignEmptyC(pformat, "gff");	/* use the real name */
	ajDebug("... output format not set, default to '%S'\n", *pformat);
    }

    return ajTrue;
}




/* @func ajReportFindFormat ***************************************************
**
** Looks for the specified report format in the internal definitions and
** returns the index.
**
** @param [r] format [const AjPStr] Format required.
** @param [w] iformat [ajint*] Index
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajReportFindFormat(const AjPStr format, ajint* iformat)
{
    AjPStr tmpformat = NULL;
    ajint i = 0;

    if(!ajStrGetLen(format))
	return ajFalse;

    ajStrAssignS(&tmpformat, format);
    ajStrFmtLower(&tmpformat);

    while(reportFormat[i].Name)
    {
	if(ajStrMatchCaseC(tmpformat, reportFormat[i].Name))
	{
	    *iformat = i;
	    ajStrDel(&tmpformat);
	    return ajTrue;
	}
	i++;
    }

    ajStrDel(&tmpformat);

    return ajFalse;
}




/* @func ajReportSetTags ******************************************************
**
** Sets the tag list for a report
**
** @param [u] thys [AjPReport] Report object
** @param [r] taglist [const AjPStr] Tag names list
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajReportSetTags(AjPReport thys, const AjPStr taglist)
{
    AjPStr tmplist   = NULL;
    AjPStr tmpstr    = NULL;
    AjPStr tagtype  = NULL;
    AjPStr tagname  = NULL;
    AjPStr tagprint = NULL;

    /*
     ** assume the tags are a simple list in this format:
     ** type: name[=printname]
     **
     ** spaces are not allowed in names (for ease of parsing the results)
     */

    if(!reportTagExp)
	reportTagExp = ajRegCompC("^ *([^:]+):([^= ]+)(=([^ ]+))?");

    ajStrAssignS(&tmplist, taglist);
    while(ajRegExec(reportTagExp, tmplist))
    {
	tagtype = NULL;
	tagname = NULL;
	tagprint = NULL;
	ajRegSubI(reportTagExp, 1, &tagtype);
	ajRegSubI(reportTagExp, 2, &tagname);
	ajRegSubI(reportTagExp, 4, &tagprint);
	if(!ajStrGetLen(tagprint))
	    ajStrAssignS(&tagprint, tagname);

	ajDebug("Tag '%S' : '%S' print '%S'\n", tagtype, tagname, tagprint);
	ajRegPost(reportTagExp, &tmpstr);
	ajStrAssignS(&tmplist, tmpstr);

	if(!ajListLength(thys->Tagtypes))
	{
	    thys->Tagtypes  = ajListNew();
	    thys->Tagnames  = ajListNew();
	    thys->Tagprints = ajListNew();
	}

	ajListPushApp(thys->Tagtypes,  tagtype);
	ajListPushApp(thys->Tagnames,  tagname);
	ajListPushApp(thys->Tagprints, tagprint);
    }

    if(ajStrGetLen(tmplist))
    {				      /* test acdc-reportbadtaglist */
	ajErr("Bad report taglist at '%S'", tmplist);
	return ajFalse;
    }

    ajStrDel(&tmpstr);
    ajStrDel(&tmplist);

    return ajTrue;
}




/* @func ajReportValid ********************************************************
**
** Test for a report object.
**
** Checks the format works with the number of tags.
** Checks the format works with the type (protein or nucleotide).
** Sets the format if not already defined.
**
** @param [u] thys [AjPReport] Report object
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajReportValid(AjPReport thys)
{
    if(!thys->Format)
	/* test acdc-reportbadformat */
	if(!ajReportFindFormat(thys->Formatstr, &thys->Format))
	{
	    ajErr("Unknown report format '%S'", thys->Formatstr);
	    return ajFalse;
	}

    /* so far, no format has mintags non-zero */
    if( reportFormat[thys->Format].Mintags > ajListLength(thys->Tagnames))
    {
	ajErr("Report format '%S' needs %d tags, has only %d",
	      reportFormat[thys->Format].Mintags,
	      ajListLength(thys->Tagnames));
	return ajFalse;
    }

    return ajTrue;
}




/* @func ajReportNew **********************************************************
**
** Constructor for a report object
**
** @return [AjPReport] New report object
** @category new [AjPReport] Default constructor
** @@
******************************************************************************/

AjPReport ajReportNew(void)
{
    AjPReport pthis;

    AJNEW0(pthis);

    pthis->CountSeq  = 0;
    pthis->CountHit  = 0;
    pthis->Name      = ajStrNew();
    pthis->Formatstr = ajStrNew();
    pthis->Format    = 0;
    pthis->Fttable   = NULL;
    pthis->Ftquery   = ajFeattabOutNew();
    pthis->Extension = ajStrNew();
    pthis->Precision = 3;
    pthis->File      = NULL;
    pthis->Showscore = ajTrue;

    return pthis;
}




/* @func ajReportWrite ********************************************************
**
** Writes a feature report
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [AjBool] True if data was written
**                  False is maximum output has already been reached.
** @category output [AjPReport] Master sequence output routine
** @@
******************************************************************************/

AjBool ajReportWrite(AjPReport thys,
		     const AjPFeattable ftable, const AjPSeq seq)
{
    ajint maxreport = 0;
    AjPFeattable ftcopy = NULL;
    AjPStr tmpstr = NULL;

    ajDebug("ajReportWrite\n");		/* add ftable name and size */

    if(!thys->Format)
	if(!ajReportFindFormat(thys->Formatstr, &thys->Format))
	    ajDie("unknown report format '%S'", thys->Formatstr);

    ajDebug("ajReportWrite %d '%s' %d\n",
	    thys->Format, reportFormat[thys->Format].Name,
	    ajFeattableSize(ftable));

    if(thys->MaxHitAll)
    {
	maxreport = thys->MaxHitAll - thys->CountHit;
	if(maxreport <= 0)
	{
	    return ajFalse;
	}
    }

    if(thys->MaxHitSeq)
    {
	if(thys->MaxHitAll)
	    maxreport = AJMIN(maxreport, thys->MaxHitSeq);
	else
	    maxreport = thys->MaxHitSeq;
    }

    ajDebug("ajReportWrite MaxHitSeq: %d MaxHitAll: %d "
	    "CountHit: %d FeattableSize: %d maxreport: %d\n",
	    thys->MaxHitSeq, thys->MaxHitAll, thys->CountHit,
	    ajFeattableSize(ftable), maxreport);

    ajReportSetType(thys, ftable, seq);

    /* Calling funclist reportFormat() */

    if(maxreport && maxreport < ajFeattableSize(ftable))
    {
	ajFmtPrintS(&tmpstr, "HitLimit: %d/%d",
		    maxreport, ajFeattableSize(ftable));
	ajReportAppendSubTail(thys, tmpstr);
	ajDebug("new subtail '%S'\n", tmpstr);
	ajStrDel(&tmpstr);
	ftcopy = ajFeattableCopyLimit(ftable, maxreport);
	reportFormat[thys->Format].Write(thys, ftcopy, seq);
	ajFeattableDel(&ftcopy);
	thys->CountHit += maxreport;
    }
    else
    {
	reportFormat[thys->Format].Write(thys, ftable, seq);
	thys->CountHit += ajFeattableSize(ftable);
    }

    ++thys->CountSeq;

    return ajTrue;
}




/* @func ajReportClose ********************************************************
**
** Closes a feature report
**
** @param [u] thys [AjPReport] Report object
** @return [void]
** @@
******************************************************************************/

void ajReportClose(AjPReport thys)
{
    ajDebug("ajReportClose '%F' CountSeq %d CountHit %d\n",
	    thys->File, thys->CountSeq, thys->CountHit);

    if(!thys->File)	     /* already closed, nothing to write to */
	return;

    if(!thys->Format)
	if(!ajReportFindFormat(thys->Formatstr, &thys->Format))
	    ajDie("unknown report format '%S'", thys->Formatstr);

    if(reportFormat[thys->Format].Showheader) {
	if(!thys->CountSeq)
	    ajReportWriteHeader(thys, NULL, NULL);
	ajReportWriteTail(thys, NULL, NULL);
    }

    ajFileClose(&thys->File);

    return;
}




/* @func ajReportLists ********************************************************
**
** Converts a report tagtypes definition (ACD taglist attribute)
** into arrays of tag types, names and printnames.
**
** @param [r] thys [const AjPReport] Report object
** @param [w] types [AjPStr**] Address of array of types generated
** @param [w] names [AjPStr**] Address of array of names generated
** @param [w] prints [AjPStr**] Address of array of print names generated
** @param [w] sizes [ajint**] Width needed to print heading
** @return [ajint] Number of tagtypes (size of arrays created)
******************************************************************************/

ajint ajReportLists(const AjPReport thys, AjPStr** types, AjPStr** names,
		     AjPStr** prints, ajint** sizes)
{
    ajint ntags;
    static ajint jmin = 6;
    ajint i;

    if(!ajListLength(thys->Tagtypes))
	return 0;

    ntags = ajListToArray(thys->Tagnames, (void***) names);
    ntags = ajListToArray(thys->Tagprints, (void***) prints);
    ntags = ajListToArray(thys->Tagtypes,  (void***) types);

    if(ntags)
    {
	AJCRESIZE(*sizes, ntags);
	for(i=0; i < ntags; i++)
	    (*sizes)[i] = AJMIN(jmin, ajStrGetLen((*prints)[i]));
    }
    else
	AJFREE(sizes);

    return ntags;
}




/* @func ajReportWriteHeader **************************************************
**
** Writes a feature report header and updates internal counters.
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportWriteHeader(AjPReport thys,
			 const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjPStr tmpstr    = NULL;
    AjPStr tmpname   = NULL;
    AjPStr tmptype   = NULL;
    AjIList itername = NULL;
    AjIList itertype = NULL;
    AjBool doSingle  =ajFalse; /* turned off for now - always multi format */
    int i;
    AjPTime today = NULL;
    
    outf = thys->File;

    today =  ajTimeTodayF("report");
    
    /* Header for the top of the file (first call for report only) */
    
    if(!thys->CountSeq)
    {
	ajFmtPrintF(outf, "########################################\n");
	ajFmtPrintF(outf, "# Program: %S\n", ajAcdGetProgram());
	ajFmtPrintF(outf, "# Rundate: %D\n", today);
	ajFmtPrintF(outf, "# Commandline: %S\n", ajAcdGetProgram());
	ajStrAssignS(&tmpstr, ajAcdGetCmdline());
	if(ajStrGetLen(tmpstr))
	{
	    ajStrExchangeCC(&tmpstr, "\n", "\1#    ");
	    ajStrExchangeCC(&tmpstr, "\1", "\n");
	    ajFmtPrintF(outf, "#    %S\n", tmpstr);
	}
	ajStrAssignS(&tmpstr, ajAcdGetInputs());
	if(ajStrGetLen(tmpstr))
	{
	    ajStrExchangeCC(&tmpstr, "\n", "\1#    ");
	    ajStrExchangeCC(&tmpstr, "\1", "\n");
	    ajFmtPrintF(outf, "#    %S\n", tmpstr);
	}
	if(thys->MaxHitAll)
	    ajFmtPrintF(outf, "# Max_hits_total: %d\n",
			thys->MaxHitAll);
	if(thys->MaxHitSeq)
	    ajFmtPrintF(outf, "# Max_hits_sequence: %d\n",
			thys->MaxHitSeq);
	ajFmtPrintF(outf, "# Report_format: %S\n", thys->Formatstr);
	ajFmtPrintF(outf, "# Report_file: %F\n", outf);
	if(ajListLength(thys->FileNames))
	{
	    i = 0;
	    itername = ajListIterRead(thys->FileNames);
	    itertype = ajListIterRead(thys->FileTypes);
	    ajFmtPrintF(outf, "# Additional_files: %d\n",
			ajListLength(thys->FileNames));
	    while(ajListIterMore(itername) && ajListIterMore(itertype))
	    {
		tmpname = (AjPStr)ajListIterNext(itername);
		tmptype = (AjPStr)ajListIterNext(itertype);
		ajFmtPrintF(outf, "# %d: %S (%S)\n", ++i, tmpname, tmptype);
	    }
	    ajListIterFree(&itername);
	    ajListIterFree(&itertype);
	}

	if(!doSingle || thys->Multi)
	    ajFmtPrintF(outf, "########################################\n\n");
	else
	    ajFmtPrintF(outf, "#\n");
    }

    /* Sequence header (can be part of top header) */
    
    if(!doSingle || thys->Multi)
	ajFmtPrintF(outf, "#=======================================\n#\n");
 
    if(seq)
    {
	ajFmtPrintF(outf, "# Sequence: %S     from: %d   to: %d\n",
		    ajReportSeqName(thys, seq),
		    ajSeqGetBegin(seq) + ajSeqGetOffset(seq),
		    ajSeqGetEnd(seq) + ajSeqGetOffset(seq));
    
	if(thys->Showacc)
	    ajFmtPrintF(outf, "# Accession: %S\n", ajSeqGetAccS(seq));
	if(thys->Showdes)
	    ajFmtPrintF(outf, "# Description: %S\n", ajSeqGetDescS(seq));
    }

    if(ftable)
    {
	ajFmtPrintF(outf, "# HitCount: %d\n",
		    ajFeattableSize(ftable));
    }

    if(ajStrGetLen(thys->Header))
    {
	ajStrAssignS(&tmpstr, thys->Header);
	ajStrExchangeCC(&tmpstr, "\n", "\1# ");
	ajStrExchangeCC(&tmpstr, "\1", "\n");
	ajStrTrimEndC(&tmpstr, " ");
	ajFmtPrintF(outf, "#\n");
	ajFmtPrintF(outf, "# %S", tmpstr);
	if(!ajStrSuffixC(tmpstr, "\n#"))
	    ajFmtPrintF(outf, "\n#");
	ajFmtPrintF(outf, "\n");
    }
    
    if(ajStrGetLen(thys->SubHeader))
    {
	ajStrAssignS(&tmpstr, thys->SubHeader);
	ajStrExchangeCC(&tmpstr, "\n", "\1# ");
	ajStrExchangeCC(&tmpstr, "\1", "\n");
	ajStrTrimEndC(&tmpstr, " ");
	ajFmtPrintF(outf, "#\n");
	ajFmtPrintF(outf, "# %S", tmpstr);
	if(!ajStrSuffixC(tmpstr, "\n#"))
	    ajFmtPrintF(outf, "\n#");
	ajFmtPrintF(outf, "\n");
	ajStrDel(&thys->SubHeader);
    }
    
    if(!doSingle || thys->Multi)
	ajFmtPrintF(outf, "#=======================================\n\n");
    else
	ajFmtPrintF(outf, "########################################\n\n");
    
    ajStrDel(&tmpstr);
    AJFREE(today);
    
    return;
}




/* @func ajReportWriteTail ****************************************************
**
** Writes (and clears) a feature report tail
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportWriteTail(AjPReport thys,
		       const AjPFeattable ftable, const AjPSeq seq)
{
    AjPFile outf;
    AjPStr tmpstr   = NULL;
    AjBool doSingle = ajFalse; /* turned off for now - always multi format */

    outf = thys->File;

    if(!doSingle || thys->Multi)
	ajFmtPrintF(outf, "\n#---------------------------------------\n");
    else
	ajFmtPrintF(outf, "\n########################################\n");

    if(ftable)
    {
	if(ajStrGetLen(thys->SubTail))
	{
	    ajStrAssignS(&tmpstr, thys->SubTail);
	    ajStrExchangeCC(&tmpstr, "\n", "\1# ");
	    ajStrExchangeCC(&tmpstr, "\1", "\n");
	    ajStrTrimEndC(&tmpstr, " ");
	    ajFmtPrintF(outf, "#\n");
	    ajFmtPrintF(outf, "# %S", tmpstr);
	    if(!ajStrSuffixC(tmpstr, "\n#"))
		ajFmtPrintF(outf, "\n#");
	    ajFmtPrintF(outf, "\n");
	    ajStrDel(&thys->SubTail);
	}
	if(ajStrGetLen(thys->Tail))
	{
	    ajStrAssignS(&tmpstr, thys->Tail);
	    ajStrExchangeCC(&tmpstr, "\n", "\1# ");
	    ajStrExchangeCC(&tmpstr, "\1", "\n");
	    ajStrTrimEndC(&tmpstr, " ");
	    ajFmtPrintF(outf, "#\n");
	    ajFmtPrintF(outf, "# %S", tmpstr);
	    if(!ajStrSuffixC(tmpstr, "\n#"))
		ajFmtPrintF(outf, "\n#");
	    ajFmtPrintF(outf, "\n");
	}
    }

    if(!ftable)
    {
	ajFmtPrintF(outf, "# Total_sequences: %d\n", thys->CountSeq);
	ajFmtPrintF(outf, "# Total_hitcount: %d\n", thys->CountHit);
	if(thys->MaxHitAll)
	ajFmtPrintF(outf, "# Max_hitcount: %d\n", thys->MaxHitAll);
    }

    if(!doSingle || thys->Multi)
	ajFmtPrintF(outf, "#---------------------------------------\n");
    else
	ajFmtPrintF(outf, "########################################\n");

    ajStrDel(&tmpstr);

    return;
}




/* @func ajReportSetHeader ****************************************************
**
** Defines a feature report header
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const AjPStr] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetHeader(AjPReport thys, const AjPStr header)
{
    ajStrAssignS(&thys->Header, header);

    return;
}




/* @func ajReportSetHeaderC ***************************************************
**
** Defines a feature report header
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const char*] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetHeaderC(AjPReport thys, const char* header)
{
    ajStrAssignC(&thys->Header, header);

    return;
}




/* @func ajReportAppendHeader *************************************************
**
** Defines a feature report header
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const AjPStr] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendHeader(AjPReport thys, const AjPStr header)
{
    if(ajStrGetLen(thys->Header))
    {
	if(ajStrGetCharLast(thys->Header) != '\n')
	    ajStrAppendK(&thys->Header, '\n');
    }
    ajStrAppendS(&thys->Header, header);

    return;
}




/* @func ajReportAppendHeaderC ************************************************
**
** Appends to a feature report header
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const char*] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendHeaderC(AjPReport thys, const char* header)
{
    if(ajStrGetLen(thys->Header))
    {
	if(ajStrGetCharLast(thys->Header) != '\n')
	    ajStrAppendK(&thys->Header, '\n');
    }
    ajStrAppendC(&thys->Header, header);

    return;
}




/* @func ajReportSetSubHeader *************************************************
**
** Defines a feature report subheader
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const AjPStr] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetSubHeader(AjPReport thys, const AjPStr header)
{
    ajStrAssignS(&thys->SubHeader, header);

    return;
}




/* @func ajReportSetSubHeaderC ************************************************
**
** Appends to a feature report subheader
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const char*] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetSubHeaderC(AjPReport thys, const char* header)
{
    ajStrAssignC(&thys->SubHeader, header);

    return;
}



/* @func ajReportAppendSubHeader **********************************************
**
** Defines a feature report subheader
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const AjPStr] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendSubHeader(AjPReport thys, const AjPStr header)
{
    if(ajStrGetLen(thys->SubHeader))
    {
	if(ajStrGetCharLast(thys->SubHeader) != '\n')
	    ajStrAppendK(&thys->SubHeader, '\n');
    }
    ajStrAppendS(&thys->SubHeader, header);

    return;
}




/* @func ajReportAppendSubHeaderC *********************************************
**
** Appends to a feature report subheader
**
** @param [u] thys [AjPReport] Report object
** @param [r] header [const char*] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendSubHeaderC(AjPReport thys, const char* header)
{
    if(ajStrGetLen(thys->SubHeader))
    {
	if(ajStrGetCharLast(thys->SubHeader) != '\n')
	    ajStrAppendK(&thys->SubHeader, '\n');
    }
    ajStrAppendC(&thys->SubHeader, header);

    return;
}





/* @func ajReportSetTail ******************************************************
**
** Defines a feature report tail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const AjPStr] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetTail(AjPReport thys, const AjPStr tail)
{
    ajStrAssignS(&thys->Tail, tail);

    return;
}




/* @func ajReportSetTailC *****************************************************
**
** Defines a feature report tail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const char*] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetTailC(AjPReport thys, const char* tail)
{
    ajStrAssignC(&thys->Tail, tail);

    return;
}




/* @func ajReportAppendTail ***************************************************
**
** Appends to a feature report tail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const AjPStr] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendTail(AjPReport thys, const AjPStr tail)
{
    if(ajStrGetLen(thys->Tail))
    {
	if(ajStrGetCharLast(thys->Tail) != '\n')
	    ajStrAppendK(&thys->Tail, '\n');
    }
    ajStrAppendS(&thys->Tail, tail);

    return;
}




/* @func ajReportAppendTailC **************************************************
**
** Appends to a feature report tail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const char*] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendTailC(AjPReport thys, const char* tail)
{
    if(ajStrGetLen(thys->Tail))
    {
	if(ajStrGetCharLast(thys->Tail) != '\n')
	    ajStrAppendK(&thys->Tail, '\n');
    }
    ajStrAppendC(&thys->Tail, tail);

    return;
}




/* @func ajReportSetSubTail ***************************************************
**
** Defines a feature report subtail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const AjPStr] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetSubTail(AjPReport thys, const AjPStr tail)
{
    ajStrAssignS(&thys->SubTail, tail);

    return;
}




/* @func ajReportSetSubTailC **************************************************
**
** Defines a feature report subtail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const char*] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetSubTailC(AjPReport thys, const char* tail)
{
    ajStrAssignC(&thys->SubTail, tail);

    return;
}




/* @func ajReportAppendSubTail ************************************************
**
** Appends to a feature report subtail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const AjPStr] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendSubTail(AjPReport thys, const AjPStr tail)
{
    if(ajStrGetLen(thys->SubTail))
    {
	if(ajStrGetCharLast(thys->SubTail) != '\n')
	    ajStrAppendK(&thys->SubTail, '\n');
    }
    ajStrAppendS(&thys->SubTail, tail);

    return;
}




/* @func ajReportAppendSubTailC ***********************************************
**
** Appends to a feature report subtail
**
** @param [u] thys [AjPReport] Report object
** @param [r] tail [const char*] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportAppendSubTailC(AjPReport thys, const char* tail)
{
    if(ajStrGetLen(thys->SubTail))
    {
	if(ajStrGetCharLast(thys->SubTail) != '\n')
	    ajStrAppendK(&thys->SubTail, '\n');
    }
    ajStrAppendC(&thys->SubTail, tail);

    return;
}




/* @func ajReportSetType ******************************************************
**
** Sets the report type (if it is not set already)
**
** @param [u] thys [AjPReport] Report object
** @param [r] ftable [const AjPFeattable] Feature table object
** @param [r] seq [const AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportSetType(AjPReport thys,
		      const AjPFeattable ftable, const AjPSeq seq)
{
    ajDebug("ajReportSetType '%S' ft: '%S' sq: '%S'\n",
	    thys->Type, ftable->Type, seq->Type);

    if(ajStrGetLen(thys->Type))
	return;

    if(ajStrGetLen(ftable->Type))
    {
	ajStrAssignS(&thys->Type, ftable->Type);
	return;
    }

    if(seq && ajStrGetLen(seq->Type))
    {
	ajStrAssignS(&thys->Type, seq->Type);
	return;
    }

    return;
}




/* @funcstatic reportCharname *************************************************
**
** Returns 'residues' for a protein report, 'bases' for a nucleotide report.
**
** @param [r] thys [const AjPReport] Report object
** @return [char*] String to print the sequence character type
******************************************************************************/

static char* reportCharname(const AjPReport thys)
{
    static char* protstr = "residues";
    static char* nucstr = "bases";

    if(!ajStrGetLen(thys->Type))
	return protstr;

    switch(ajStrGetCharFirst(thys->Type))
    {
    case 'n':
    case 'N':
	return nucstr;
    default:
	break;
    }

    return protstr;
}




/* @func ajReportSeqName ******************************************************
**
** Returns the sequence name or USA depending on the setting in the
** report object (derived from the ACD and command line -rusa option)
**
** @param [r] thys [const AjPReport] Report object
** @param [r] seq [const AjPSeq] Sequence object
** @return [const AjPStr] Sequence name for this report
******************************************************************************/

const AjPStr ajReportSeqName(const AjPReport thys, const AjPSeq seq)
{
    if(thys->Showusa)
	return ajSeqGetUsa(seq);

    return ajSeqGetNameS(seq);
}




/* @func ajReportFileAdd ******************************************************
**
** Adds an extra file name and description to the report
**
** @param [u] thys [AjPReport] Report object
** @param [u] file [AjPFile] File
** @param [r] type [const AjPStr] Type (simple text description)
** @return [void]
******************************************************************************/

void ajReportFileAdd(AjPReport thys, AjPFile file, const AjPStr type)
{
    AjPStr tmpname = NULL;
    AjPStr tmptype = NULL;

    if(!thys->FileTypes)
	thys->FileTypes = ajListstrNew();
    if(!thys->FileNames)
	thys->FileNames = ajListstrNew();

    ajStrAssignS(&tmptype, type);
    ajListstrPushApp(thys->FileTypes, tmptype);

    ajFmtPrintS(&tmpname, "%F", file);
    ajListstrPushApp(thys->FileNames, tmpname);

    return;
}




/* @func ajReportPrintFormat **************************************************
**
** Reports the internal data structures
**
** @param [u] outf [AjPFile] Output file
** @param [r] full [AjBool] Full report (usually ajFalse)
** @return [void]
** @@
******************************************************************************/

void ajReportPrintFormat(AjPFile outf, AjBool full)
{
    ajint i = 0;

    ajFmtPrintF(outf, "\n");
    ajFmtPrintF(outf, "# report output formats\n");
    ajFmtPrintF(outf, "# Name    Format name (or alias)\n");
    ajFmtPrintF(outf, "# Alias   Alias name\n");
    ajFmtPrintF(outf, "# Nuc     Valid for nucleotide sequences\n");
    ajFmtPrintF(outf, "# Pro     Valid for protein sequences\n");
    ajFmtPrintF(outf, "# Mintags Minimum number of tags to be specified (0 for all)\n");
    ajFmtPrintF(outf, "# Showseq Includes sequence\n");
    ajFmtPrintF(outf, "# Desc    Format description\n");
    ajFmtPrintF(outf, "# Name          Alias Nuc Pro Mintags Showseq "
		"Description\n");
    ajFmtPrintF(outf, "\n");
    ajFmtPrintF(outf, "RFormat {\n");
    for(i=0; reportFormat[i].Name; i++)
    {
	if(full || !reportFormat[i].Alias)
	    ajFmtPrintF(outf, "  %-12s %5B %3B %3B %7d %7B \"%s\"\n",
			reportFormat[i].Name,
			reportFormat[i].Alias,
			reportFormat[i].Nuc,
			reportFormat[i].Prot,
			reportFormat[i].Mintags,
			reportFormat[i].Showseq,
			reportFormat[i].Desc);
    }
    ajFmtPrintF(outf, "}\n\n");

    return;
}
/* @func ajReportDummyFunction ************************************************
**
** Dummy function to catch all unused functions defined in the ajreport
** source file.
**
** @return [void]
**
******************************************************************************/

void ajReportDummyFunction(void)
{
    AjPReport report = NULL;
    AjPFeattable ftable = NULL;
    AjPSeq seq = NULL;

    reportWriteDraw(report, ftable, seq);
}



/* @func ajReportExit *********************************************************
**
** Cleans up report processing internal memory
**
** @return [void]
** @@
******************************************************************************/

void ajReportExit(void)
{
    ajRegFree(&reportTagExp);

    return;
}
