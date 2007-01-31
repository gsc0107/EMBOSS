#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseq_h
#define ajseq_h

#include "ajseqdata.h"
#include "ajax.h"

/* @data AjPSeqCvt ************************************************************
**
** Sequence conversion data. Used to convert a sequence to binary.
**
** @new ajSeqCvtNew Creates from a character string of valid bases.
** @new ajSeqCvtNewText Creates from a character string of valid bases.
** @new ajSeqCvtNewZero Creates from a character string of valid bases.
** @new ajSeqCvtNewZeroS Creates from an array of strings of valid bases.
** @new ajSeqCvtNewZeroSS Creates from an array of strings of valid bases.
** @output ajSeqCvtTrace Reports on contents for debugging
*
** @alias AjSSeqCvt
** @alias AjOSeqCvt
**
** @attr size [ajint] Number of characters in table, usually
**                    all possible characters.
** @attr len [ajint] Number of characters defined
** @attr missing [ajint] Index of the missing character value
** @attr bases [AjPStr] The bases which can be converted
** @attr nrlabels [ajint] Number of row labels
** @attr rlabels [AjPStr*] Row labels 
** @attr nclabels [ajint] Number of column labels
** @attr clabels [AjPStr*] Column labels 
** @attr table [char*] Binary character value for each character in bases
** @@
******************************************************************************/

typedef struct AjSSeqCvt {
  ajint size;
  ajint len;
  ajint missing;
  AjPStr bases;
  ajint nrlabels;
  AjPStr* rlabels;
  ajint nclabels;
  AjPStr* clabels;
  char *table;
} AjOSeqCvt;

#define AjPSeqCvt AjOSeqCvt*


/*
** Prototype definitions
*/

AjBool       ajSeqtestIsAccession (const AjPStr accnum);
const AjPStr ajSeqtestIsSeqversion (const AjPStr sv);

__deprecated AjBool       ajIsAccession (const AjPStr accnum);
__deprecated const AjPStr ajIsSeqversion (const AjPStr sv);

void         ajSeqallDel(AjPSeqall *thys);
const AjPStr ajSeqallGetFilename(const AjPSeqall thys);
const AjPStr ajSeqallGetseqName (const AjPSeqall thys);
ajint        ajSeqallGetseqBegin (const AjPSeqall seq);
ajint        ajSeqallGetseqEnd (const AjPSeqall seq);
ajint        ajSeqallGetseqLen (const AjPSeqall seqall);
const AjPStr ajSeqallGetName(const AjPSeqall seq);
__deprecated ajint        ajSeqallLen (const AjPSeqall seqall);
__deprecated const AjPStr ajSeqallGetNameSeq (const AjPSeqall thys);
ajint        ajSeqallGetseqRange (const AjPSeqall thys, ajint* begin, ajint* end);
const AjPStr ajSeqallGetUsa (const AjPSeqall thys);
AjPSeqall    ajSeqallNew (void);
void         ajSeqallClear (AjPSeqall thys);
AjBool       ajSeqallNext (AjPSeqall seqall, AjPSeq* retseq);
void         ajSeqallSetRange (AjPSeqall seq, ajint ibegin, ajint iend);
void         ajSeqallSetRangeRev (AjPSeqall seq, ajint ibegin, ajint iend);
void         ajSeqAssignAccC(AjPSeq seq, const char* text);
void         ajSeqAssignAccS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignDescC(AjPSeq seq, const char* text);
void         ajSeqAssignDescS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignEntryC(AjPSeq seq, const char* text);
void         ajSeqAssignEntryS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignFileC(AjPSeq seq, const char* text);
void         ajSeqAssignFileS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignFullC(AjPSeq seq, const char* text);
void         ajSeqAssignFullS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignGiC(AjPSeq seq, const char* text);
void         ajSeqAssignGiS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignNameC(AjPSeq seq, const char* txt);
void         ajSeqAssignNameS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignSeqC(AjPSeq seq, const char* text);
void         ajSeqAssignSeqLenC(AjPSeq seq, const char* txt, ajint len);
void         ajSeqAssignSeqS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignSvC(AjPSeq seq, const char* text);
void         ajSeqAssignSvS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignUfoC(AjPSeq seq, const char* text);
void         ajSeqAssignUfoS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignUsaC(AjPSeq seq, const char* text);
void         ajSeqAssignUsaS(AjPSeq seq, const AjPStr str);
ajuint       ajSeqGetBegin (const AjPSeq seq);
const char*  ajSeqGetSeqC (const AjPSeq thys);
const AjPStr ajSeqGetSeqS(const AjPSeq seq);
char*        ajSeqGetSeqCopyC (const AjPSeq seq);
AjPStr       ajSeqGetSeqCopyS(const AjPSeq seq);
ajint        ajSeqCalcCheckgcg (const AjPSeq thys);
void         ajSeqClear (AjPSeq thys);
void         ajSeqComplement (AjPSeq thys);
__deprecated void         ajSeqComplementOnly(AjPSeq pthis);
void         ajSeqstrComplement (AjPStr* thys);
__deprecated void         ajSeqstrComplementOnly (AjPStr* thys);
AjBool       ajSeqstrConvertNum(const AjPStr seq, const AjPSeqCvt cvt,
				AjPStr* Pnumseq);
AjPFeattable ajSeqGetFeatCopy (const AjPSeq thys);
void         ajSeqCalcCount (const AjPSeq thys, ajint *b);
__deprecated ajuint       ajSeqCalcCrc(const AjPSeq seq );
ajuint       ajSeqstrCalcCrc(const AjPStr seq );
ajuint       ajSeqstrCountGaps(const AjPStr str);
void         ajSeqcvtDel (AjPSeqCvt* thys);
ajint        ajSeqcvtGetCodeK (const AjPSeqCvt thys, char ch);
ajint        ajSeqcvtGetCodeS (const AjPSeqCvt thys, const AjPStr ch);
ajint        ajSeqcvtGetCodeAsymS (const AjPSeqCvt cvt, const AjPStr str);
ajuint       ajSeqcvtGetLen (const AjPSeqCvt thys);
AjPSeqCvt    ajSeqcvtNewC (const char* bases);
AjPSeqCvt    ajSeqcvtNewNumberC (const char* bases);
AjPSeqCvt    ajSeqcvtNewEndC (const char* bases);
AjPSeqCvt    ajSeqcvtNewStr (const AjPPStr bases, ajint n);
AjPSeqCvt    ajSeqcvtNewStrAsym (const AjPPStr bases, ajint n, 
				const AjPPStr rbases, ajint rn);
void         ajSeqcvtTrace (const AjPSeqCvt cvt);
__deprecated void         ajSeqCvtDel (AjPSeqCvt* thys);
__deprecated ajint        ajSeqCvtK (const AjPSeqCvt thys, char ch);
__deprecated ajint        ajSeqCvtKS (const AjPSeqCvt thys, const AjPStr ch);
__deprecated ajint        ajSeqCvtKSRow (const AjPSeqCvt thys,
					 const AjPStr ch);
__deprecated ajint        ajSeqCvtKSColumn (const AjPSeqCvt thys,
					    const AjPStr ch);
__deprecated ajint        ajSeqCvtLen (const AjPSeqCvt thys);
__deprecated AjPSeqCvt    ajSeqCvtNew (const char* bases);
__deprecated AjPSeqCvt    ajSeqCvtNewText (const char* bases);
__deprecated AjPSeqCvt    ajSeqCvtNewZero (const char* bases);
__deprecated AjPSeqCvt    ajSeqCvtNewZeroS (const AjPPStr bases, ajint n);
__deprecated AjPSeqCvt    ajSeqCvtNewZeroSS (const AjPPStr bases, ajint n, 
				const AjPPStr rbases, ajint rn);
__deprecated void         ajSeqCvtTrace (const AjPSeqCvt cvt);
void         ajSeqSetName(AjPSeq thys, const AjPStr setname);
void         ajSeqSetNameMulti(AjPSeq thys, const AjPStr setname);
__deprecated void         ajSeqDefName(AjPSeq thys, const AjPStr setname,
				       AjBool multi);
void         ajSeqDel (AjPSeq* pthis);
ajuint       ajSeqGetEnd (const AjPSeq seq);
void         ajSeqExit(void);
void         ajSeqGapFill (AjPSeq seq, ajuint len);
__deprecated ajint        ajSeqFill (AjPSeq seq, ajint len);
ajuint       ajSeqCountGaps(const AjPSeq seq);
__deprecated ajint        ajSeqGapCount (const AjPSeq thys);
__deprecated ajint        ajSeqGapCountS (const AjPStr str);
void         ajSeqGapStandard (AjPSeq thys, char gapch);
__deprecated void         ajSeqGapStandardS(AjPStr thys, char gapch);
const char*  ajSeqGetAccC (const AjPSeq thys);
const AjPStr ajSeqGetAccS (const AjPSeq thys);
const char*  ajSeqGetDescC (const AjPSeq thys);
const AjPStr ajSeqGetDescS (const AjPSeq thys);
const char*  ajSeqGetEntryC (const AjPSeq thys);
const AjPStr ajSeqGetEntryS (const AjPSeq thys);
const AjPFeattable ajSeqGetFeat (const AjPSeq thys);
const char*  ajSeqGetGiC (const AjPSeq thys);
const AjPStr ajSeqGetGiS (const AjPSeq thys);
const char*  ajSeqGetNameC (const AjPSeq thys);
const AjPStr ajSeqGetNameS (const AjPSeq thys);
ajuint       ajSeqGetOffend(const AjPSeq seq);
ajuint       ajSeqGetOffset(const AjPSeq seq);
ajuint       ajSeqGetRange (const AjPSeq thys, ajint* begin, ajint* end);
AjBool       ajSeqGetRev (const AjPSeq thys);
const char*  ajSeqGetSvC (const AjPSeq thys);
const AjPStr ajSeqGetSvS (const AjPSeq thys);
const char*  ajSeqGetTaxC (const AjPSeq thys);
const AjPStr ajSeqGetTaxS (const AjPSeq thys);
const char*  ajSeqGetUsaC (const AjPSeq thys);
const AjPStr ajSeqGetUsaS (const AjPSeq thys);
AjBool       ajSeqIsNuc (const AjPSeq thys);
AjBool       ajSeqIsProt (const AjPSeq thys);
ajuint       ajSeqGetLen (const AjPSeq seq);
void         ajSeqSetUnique(AjPSeq seq);
__deprecated void         ajSeqMod (AjPSeq thys);
float        ajSeqCalcMolwt (const AjPSeq seq);
float        ajSeqstrCalcMolwt (const AjPStr seq);
AjPSeq       ajSeqNew (void);
AjPSeq       ajSeqNewNameC(const char* txt, const char* name);
AjPSeq       ajSeqNewNameS(const AjPStr str, const AjPStr name);
AjPSeq       ajSeqNewRes (size_t size);
AjPSeq       ajSeqNewSeq (const AjPSeq seq);
AjPSeq       ajSeqNewRangeC(const char* txt,
			   ajint offset, ajint offend, AjBool rev);
AjPSeq       ajSeqNewRangeS(const AjPStr str,
			   ajint offset, ajint offend, AjBool rev);
AjBool       ajSeqConvertNum (const AjPSeq thys, const AjPSeqCvt cvt,
		       AjPStr *numseq);
__deprecated AjBool       ajSeqNum (const AjPSeq thys, const AjPSeqCvt cvt,
		       AjPStr *numseq);
__deprecated AjBool       ajSeqNumS (const AjPStr thys, const AjPSeqCvt cvt,
			AjPStr *numseq);
AjBool       ajSeqIsGarbage(const AjPSeq thys);
AjBool       ajSeqIsReversedTrue(const AjPSeq seq);
AjBool       ajSeqIsReversed (const AjPSeq thys);
AjBool       ajSeqIsTrimmed (const AjPSeq thys);
void         ajSeqReverseDo (AjPSeq thys);
void         ajSeqReverseForce (AjPSeq thys);
void         ajSeqstrReverse (AjPStr* thys);
void         ajSeqReverseOnly (AjPSeq thys);
ajuint       ajSeqsetGetBegin (const AjPSeqset seq);
__deprecated ajint        ajSeqsetBegin (const AjPSeqset seq);
void         ajSeqsetDel(AjPSeqset *thys);
void         ajSeqsetDelarray(AjPSeqset **thys);
ajuint       ajSeqsetGetEnd (const AjPSeqset seq);
__deprecated ajint        ajSeqsetEnd (const AjPSeqset seq);
ajint        ajSeqsetFill (AjPSeqset seq);
const AjPStr ajSeqsetGetFormat (const AjPSeqset thys);
const char*  ajSeqsetGetNameC (const AjPSeqset thys);
const AjPStr ajSeqsetGetNameS (const AjPSeqset thys);
__deprecated const AjPStr ajSeqsetGetName (const AjPSeqset thys);
ajint        ajSeqsetGetOffend(const AjPSeqset seq);
ajint        ajSeqsetGetOffset(const AjPSeqset seq);
ajint        ajSeqsetGetRange (const AjPSeqset thys, ajint* begin, ajint* end);
const AjPSeq ajSeqsetGetseqSeq (const AjPSeqset thys, ajuint i);
const char*  ajSeqsetGetseqSeqC (const AjPSeqset thys, ajuint i);
const AjPStr ajSeqsetGetseqSeqS (const AjPSeqset thys, ajuint i);
__deprecated const AjPSeq ajSeqsetGetSeq (const AjPSeqset thys, ajint i);
__deprecated AjPSeq*      ajSeqsetGetSeqArray(const AjPSeqset thys);
AjPSeq*      ajSeqsetGetSeqarray(const AjPSeqset thys);
const AjPStr ajSeqsetGetUsa (const AjPSeqset thys);
const AjPStr ajSeqsetGetFilename(const AjPSeqset thys);
AjBool       ajSeqsetIsDna (const AjPSeqset thys);
AjBool       ajSeqsetIsNuc (const AjPSeqset thys);
AjBool       ajSeqsetIsProt (const AjPSeqset thys);
AjBool       ajSeqsetIsRna (const AjPSeqset thys);
ajuint       ajSeqsetGetLen (const AjPSeqset seq);
__deprecated ajint        ajSeqsetLen (const AjPSeqset seq);
const char*  ajSeqsetGetseqAccC (const AjPSeqset seq, ajuint i);
const AjPStr ajSeqsetGetseqAccS (const AjPSeqset seq, ajuint i);
const char*  ajSeqsetGetseqNameC (const AjPSeqset seq, ajuint i);
const AjPStr ajSeqsetGetseqNameS (const AjPSeqset seq, ajuint i);
__deprecated const AjPStr ajSeqsetName (const AjPSeqset seq, ajint i);
__deprecated const AjPStr ajSeqsetAcc (const AjPSeqset seq, ajint i);
AjPSeqset    ajSeqsetNew (void);
void         ajSeqSetOffsets(AjPSeq seq, ajint ioff, ajint ioriglen);
void         ajSeqSetRange (AjPSeq seq, ajint ibegin, ajint iend);
void         ajSeqSetRangeRev (AjPSeq seq, ajint ibegin, ajint iend);
void         ajSeqsetReverse (AjPSeqset thys);
__deprecated const char*  ajSeqsetSeq (const AjPSeqset seq, ajint i);
void         ajSeqsetSetRange (AjPSeqset seq, ajint ibegin, ajint iend);
ajuint       ajSeqsetGetSize (const AjPSeqset seq);
__deprecated ajint        ajSeqsetSize (const AjPSeqset seq);
void         ajSeqsetFmtLower (AjPSeqset seq);
void         ajSeqsetFmtUpper (AjPSeqset seq);
float        ajSeqsetGetTotweight (const AjPSeqset seq);
__deprecated float        ajSeqsetTotweight (const AjPSeqset seq);
void         ajSeqsetTrim(AjPSeqset thys);
float        ajSeqsetGetseqWeight (const AjPSeqset seq, ajuint i) ;
__deprecated float        ajSeqsetWeight (const AjPSeqset seq, ajint i) ;
void         ajSeqFmtLower (AjPSeq thys);
void         ajSeqFmtUpper (AjPSeq thys);
void         ajSeqTrace (const AjPSeq seq);
void         ajSeqTraceTitle (const AjPSeq seq, const char* title);
__deprecated void         ajSeqTraceT (const AjPSeq seq, const char* title);
AjBool       ajSeqFindOutFormat (const AjPStr format, ajint* iformat);
void         ajSeqTrim(AjPSeq thys);
ajuint       ajSeqGetBeginTrue (const AjPSeq seq);
ajuint       ajSeqGetEndTrue (const AjPSeq seq);
ajuint       ajSeqGetLenTrue (const AjPSeq seq);
ajint        ajSeqCalcTruepos (const AjPSeq thys, ajint ipos);
ajint        ajSeqCalcTrueposMin (const AjPSeq thys, ajint imin, ajint ipos);


__deprecated ajint ajSeqallBegin(const AjPSeqall seq);
__deprecated ajint ajSeqallEnd(const AjPSeqall seq);
__deprecated ajint ajSeqallGetRange(const AjPSeqall seq,
				    ajint* begin, ajint* end);
__deprecated void ajSeqsetToLower(AjPSeqset seqset);
__deprecated void ajSeqsetToUpper(AjPSeqset seqset);

__deprecated AjPSeq       ajSeqNewC (const char* seq, const char* name);
__deprecated AjPSeq       ajSeqNewStr (const AjPStr str);
__deprecated AjPSeq       ajSeqNewRange(const AjPStr seq,
					ajint offset, ajint offend,
					AjBool rev);
__deprecated AjPSeq       ajSeqNewRangeCI(const char* seq, ajint len,
					  ajint offset, ajint offend,
					  AjBool rev);
__deprecated AjPSeq       ajSeqNewL (size_t size);
__deprecated AjPSeq       ajSeqNewS  (const AjPSeq seq);

__deprecated void         ajSeqAssAcc (AjPSeq thys, const AjPStr str);
__deprecated void         ajSeqAssAccC (AjPSeq thys, const char* text);
__deprecated void         ajSeqAssDesc (AjPSeq thys, const AjPStr str);
__deprecated void         ajSeqAssDescC (AjPSeq thys, const char* text);
__deprecated void         ajSeqAssEntry (AjPSeq thys, const AjPStr str);
__deprecated void         ajSeqAssEntryC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssFile (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssFileC(AjPSeq thys, const char* text);
__deprecated void          ajSeqAssFull (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssFullC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssGi (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssGiC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssName (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssNameC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssSeq(AjPSeq seq, const AjPStr str);
__deprecated void          ajSeqAssSeqC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssSeqCI (AjPSeq thys, const char* text,
					 ajint ilen);
__deprecated void          ajSeqAssSvC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssSv (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssUfo (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssUfoC (AjPSeq thys, const char* text);
__deprecated void          ajSeqAssUsa (AjPSeq thys, const AjPStr str);
__deprecated void          ajSeqAssUsaC (AjPSeq thys, const char* text);

__deprecated void          ajSeqSetRangeDir (AjPSeq seq,
					    ajint ibegin, ajint iend,
					    AjBool rev);
__deprecated void          ajSeqReplace (AjPSeq thys, const AjPStr seq);
__deprecated void          ajSeqReplaceC (AjPSeq thys, const char* seq);
__deprecated void          ajSeqMakeUsa (AjPSeq thys, const AjPSeqin seqin);
__deprecated void          ajSeqMakeUsaS(const AjPSeq thys,
					const AjPSeqin seqin, AjPStr* usa);
__deprecated void          ajSeqCompOnly (AjPSeq thys);
__deprecated void          ajSeqToLower (AjPSeq thys);
__deprecated void          ajSeqToUpper (AjPSeq thys);
__deprecated void          ajSeqRevOnly (AjPSeq thys);
__deprecated AjBool        ajSeqReverse (AjPSeq thys);

__deprecated const AjPStr  ajSeqGetAcc (const AjPSeq thys);
__deprecated ajint         ajSeqBegin (const AjPSeq seq);
__deprecated ajint         ajSeqTrueBegin (const AjPSeq seq);
__deprecated const AjPStr  ajSeqGetDesc (const AjPSeq thys);
__deprecated ajint         ajSeqEnd (const AjPSeq seq);
__deprecated ajint         ajSeqTrueEnd (const AjPSeq seq);
__deprecated const AjPStr  ajSeqGetEntry (const AjPSeq thys);
__deprecated AjPFeattable  ajSeqCopyFeat (const AjPSeq thys);
__deprecated const AjPStr  ajSeqGetGi (const AjPSeq thys);
__deprecated ajint         ajSeqLen (const AjPSeq seq);
__deprecated ajint         ajSeqTrueLen (const AjPSeq seq);
__deprecated const char*   ajSeqName (const AjPSeq seq);
__deprecated const AjPStr  ajSeqGetName (const AjPSeq thys);
__deprecated ajint         ajSeqOffend (const AjPSeq seq);
__deprecated ajint         ajSeqOffset (const AjPSeq seq);
__deprecated AjBool        ajSeqGetReverse (const AjPSeq thys);
__deprecated AjBool        ajSeqGetReversed (const AjPSeq thys);

__deprecated const AjPStr  ajSeqStr (const AjPSeq thys);
__deprecated const char*   ajSeqChar (const AjPSeq thys);
__deprecated AjPStr        ajSeqStrCopy (const AjPSeq thys);
__deprecated char*         ajSeqCharCopy (const AjPSeq seq);
__deprecated char*         ajSeqCharCopyL (const AjPSeq seq, size_t size);
__deprecated const AjPStr  ajSeqGetSv (const AjPSeq thys);
__deprecated const AjPStr  ajSeqGetTax (const AjPSeq thys);
__deprecated const AjPStr  ajSeqGetUsa (const AjPSeq thys);

__deprecated AjBool        ajSeqRev (const AjPSeq thys);
__deprecated ajint         ajSeqCheckGcg (const AjPSeq thys);
__deprecated void          ajSeqCount (const AjPSeq thys, ajint *b);

__deprecated ajint         ajSeqPos (const AjPSeq thys, ajint ipos);
__deprecated ajint         ajSeqPosI (const AjPSeq thys,
				     ajint imin, ajint ipos);
__deprecated ajint         ajSeqPosII (ajint ilen, ajint imin, ajint ipos);

__deprecated ajint         ajSeqTruePos (const AjPSeq thys, ajint ipos);
__deprecated ajint         ajSeqTruePosI (const AjPSeq thys, ajint imin,
					 ajint ipos);
__deprecated ajint         ajSeqTruePosII (ajint ilen, ajint imin, ajint ipos);

__deprecated void          ajSeqallReverse (AjPSeqall thys);
__deprecated void          ajSeqallToLower (AjPSeqall seqall);
__deprecated void          ajSeqallToUpper (AjPSeqall seqall);
__deprecated void          ajSeqReverseStr (AjPStr* thys);
__deprecated void          ajSeqCompOnlyStr (AjPStr* thys);

__deprecated float         ajSeqMW (const AjPStr seq);
__deprecated ajuint        ajSeqCrc(const AjPStr seq );

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
