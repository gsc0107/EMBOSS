#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseqreport_h
#define ajseqreport_h

/* @data AjPReport *******************************************************
**
** Ajax Report Output object.
**
** Holds definition of feature report output.
**
** @new ajReportNew Default constructor
** @delete ajReportDel Default destructor
** @set ajReportClear Resets ready for reuse.
** @use ajReportWrite Master sequence output routine
** @use ajReportNewOut Opens an output file for sequence writing.
** @other AjPSeqout Sequence output
** @other AjPFile Input and output files
** @@
******************************************************************************/

typedef struct AjSReport {
  AjPStr Name;
  AjPStr Type;
  AjPStr Formatstr;
  AjEnum Format;
  AjPFeattable Fttable;
  AjPFeattabOut Ftquery;
  AjPStr Filename;
  AjPStr Extension;
  AjPFile File;
  AjPList Tagnames;
  AjPList Tagprints;
  AjPList Tagtypes;
  AjPStr Header;
  AjPStr Tail;
  AjBool Showusa;
  AjBool Multi;
  ajint Mintags;
  ajint Count;
} AjOReport, *AjPReport;

void         ajReportClose (AjPReport pthys);
void         ajReportDel (AjPReport* pthys);
AjBool       ajReportOpen (AjPReport thys, AjPStr name);
AjBool       ajReportFindFormat (AjPStr format, ajint* iformat);
AjBool       ajReportFormatDefault (AjPStr* pformat);
ajint        ajReportLists (AjPReport thys, AjPStr** types, AjPStr** names,
			    AjPStr** prints);
AjPReport    ajReportNew (void);
AjPStr       ajReportSeqName (AjPReport thys, AjPSeq seq);
void         ajReportSetHeader (AjPReport thys, AjPStr header);
void         ajReportSetHeaderC (AjPReport thys, char* header);
AjBool       ajReportSetTags (AjPReport thys, AjPStr taglist, ajint mintags);
void         ajReportSetTail (AjPReport thys, AjPStr tail);
void         ajReportSetTailC (AjPReport thys, char* tail);
void         ajReportSetType (AjPReport thys, AjPFeattable ftable, AjPSeq seq);
void         ajReportWriteTail (AjPReport thys, AjPFeattable ftable,
				AjPSeq seq);
void         ajReportTrace (AjPReport thys);
AjBool       ajReportValid (AjPReport thys);
void         ajReportWrite (AjPReport thys, AjPFeattable ftable, AjPSeq seq);
void         ajReportWriteClose (AjPReport thys);
void         ajReportWriteHeader (AjPReport thys, AjPFeattable ftable,
				  AjPSeq seq);

#endif

#ifdef __cplusplus
}
#endif
