#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajcod_h
#define ajcod_h


/* @data AjPCod ***************************************************************
**
** Ajax codon object.
**
** Holds arrays describing codon usage
** The length is known and held internally.
**
** AjPCod is implemented as a pointer to a C data structure.
**
** @alias AjSCod
** @alias AjOCod
**
** @attr Name [AjPStr] Name of codon file
** @attr Description [AjPStr] Description
** @attr aa [ajint*] Amino acid represented by codon
** @attr num [ajint*] Number of codons
** @attr tcount [double*] Codons per thousand
** @attr fraction [double*] Fraction of amino acids of this type
** @attr back [ajint*] Index of favoured amino acid
** @@
******************************************************************************/

typedef struct AjSCod
{
    AjPStr Name;
    AjPStr Desc;
    ajint *aa;
    ajint *num;
    double *tcount;
    double *fraction;
    ajint *back;
} AjOCod, *AjPCod;


void         ajCodBacktranslate(AjPStr *b, const AjPStr a, const AjPCod thys);
ajint        ajCodBase(ajint c);
double       ajCodCai(const AjPCod cod, const AjPStr str);
double*      ajCodCaiW(const AjPCod cod);
void         ajCodCalcGribskov(AjPCod thys, const AjPStr s);
double       ajCodCalcCai(const AjPCod thys);
double       ajCodCalcNc(const AjPCod thys);
void         ajCodCalculateUsage(AjPCod *thys, ajint c);
void         ajCodClear(AjPCod *thys);
void         ajCodComp(ajint *NA, ajint *NC, ajint *NG, ajint *NT,
		       const char *str);
void         ajCodCountTriplets(AjPCod *thys, const AjPStr s, ajint *c);
void         ajCodDel (AjPCod *thys);
AjPCod       ajCodDup (const AjPCod thys);
const AjPStr ajCodGetDesc(AjPCod thys);
const char*  ajCodGetDescC(AjPCod thys);
const AjPStr ajCodGetName(AjPCod thys);
const char*  ajCodGetNameC(AjPCod thys);
ajint        ajCodIndex(const AjPStr s);
ajint        ajCodIndexC(const char *codon);
AjPCod	     ajCodNew(void);
AjBool       ajCodRead(AjPCod thys, const AjPStr fn);
void         ajCodSetBacktranslate(AjPCod *thys);
char*        ajCodTriplet(ajint idx);
void 	     ajCodWrite( const AjPCod thys, AjPFile outf);

#endif

#ifdef __cplusplus
}
#endif
