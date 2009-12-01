#ifdef __cplusplus
extern "C"
{
#endif

/* @source ajgraph.h
**
** General Plot/Printing routines.
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


#ifndef ajgraph_h
#define ajgraph_h

#include "plplot.h"
#include "ajgraphstruct.h"
#include "ajdefine.h"
#include "ajstr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define GRAPH_XGAP      60
#define GRAPH_YGAP      60
#define GRAPH_TITLE_GAP 60

#define AJGRAPH_X_BOTTOM   0x0001  /* print xaxis bottom line */
#define AJGRAPH_Y_LEFT     0x0002  /* print yaxis left line */
#define AJGRAPH_X_TOP      0x0004  /* print xaxis top line */
#define AJGRAPH_Y_RIGHT    0x0008  /* printf y axis on the right*/
#define AJGRAPH_X_TICK     0x0010  /* add tick marks for x axis (bottom) */
#define AJGRAPH_Y_TICK     0x0020  /* add tick marks dor y axis (left) */
#define AJGRAPH_X_LABEL    0x0040  /* add x axis label */
#define AJGRAPH_Y_LABEL    0x0080  /* add y axis label */
#define AJGRAPH_TITLE      0x0100  /* add title */
#define AJGRAPH_JOINPOINTS 0x0200  /* join the point data by a line */
#define AJGRAPH_OVERLAP    0x0400  /* write plots on top of each other */
#define AJGRAPH_Y_NUMLABEL_LEFT   0x0800
#define AJGRAPH_Y_INVERT_TICK     0x1000
#define AJGRAPH_Y_GRID            0x2000
#define AJGRAPH_X_NUMLABEL_ABOVE  0x4000
#define AJGRAPH_X_INVERT_TICK     0x8000
#define AJGRAPH_X_GRID            0x10000
#define AJGRAPH_CIRCLEPOINTS      0x20000
#define AJGRAPH_SUBTITLE          0x40000
#define AJGRAPH_GAPS              0x80000

#define GRAPH_XY (AJGRAPH_X_BOTTOM + AJGRAPH_Y_LEFT + AJGRAPH_X_TOP + \
		  AJGRAPH_Y_RIGHT + AJGRAPH_X_TICK + AJGRAPH_Y_TICK + \
		  AJGRAPH_Y_LABEL + AJGRAPH_JOINPOINTS + AJGRAPH_X_LABEL + \
		  AJGRAPH_TITLE + AJGRAPH_SUBTITLE + AJGRAPH_OVERLAP)

#define GRAPH_XY_MAIN (AJGRAPH_X_BOTTOM + AJGRAPH_Y_LEFT + AJGRAPH_Y_RIGHT + \
		       AJGRAPH_X_TOP + AJGRAPH_Y_TICK + AJGRAPH_X_LABEL + \
		       AJGRAPH_Y_LABEL + AJGRAPH_JOINPOINTS + AJGRAPH_TITLE + \
		       AJGRAPH_SUBTITLE + AJGRAPH_OVERLAP)



/*
** Prototype definitions
*/

void          ajGraphListDevices (AjPList list);
void          ajGraphxyListDevices (AjPList list);
ajint*        ajGraphicsBasecolourNewNuc(const AjPStr codes);
ajint*        ajGraphicsBasecolourNewProt(const AjPStr codes);
ajint         ajGraphicsCheckColourC(const char* text);
ajint         ajGraphicsCheckColourS(const AjPStr colour);

void          ajGraphicsGetCharsize(float *defheight, float *currentscale);
ajint         ajGraphicsGetFgcolour(void);
ajint         ajGraphicsGetOutfiles(AjPList* files);
void          ajGraphGetParamsPage(const AjPGraph thys,
                                   float *xp,float *yp, ajint *xleng,
                                   ajint *yleng, ajint *xoff, ajint *yoff);
void          ajGraphicsGetParamsPage(float *xp,float *yp, ajint *xleng,
                                      ajint *yleng, ajint *xoff, ajint *yoff);

void          ajGraphicsResetBgcolour(void);
void          ajGraphicsResetFgcolour(void);

void          ajGraphicsSetBgcolourBlack(void);
void          ajGraphicsSetBgcolourWhite(void);
float         ajGraphicsSetCharscale(float scale);
float         ajGraphicsSetCharsize(float size);
float         ajGraphicsSetDefcharsize(float size);
void          ajGraphicsSetDevice(const AjPGraph graph);
ajint         ajGraphicsSetFgcolour(ajint colour);
void          ajGraphicsSetPortrait(AjBool set);
void          ajGraphicsSetPenwidth(float width);
ajint         ajGraphicsSetFillpat (ajint style);
ajint         ajGraphicsSetLinestyle (ajint style);
void          ajGraphicsSetLabelsC(const char *x, const char *y,
                                   const char *title, const char *subtitle);
void          ajGraphicsSetLabelsS(const AjPStr x, const AjPStr y,
                                   const AjPStr title, const AjPStr subtitle);

void          ajGraphPlenv (float xmin, float xmax, float ymin, float ymax,
			    ajint flags);
void          ajGraphPrintType(AjPFile outf, AjBool full);
void          ajGraphicsSetPagesize(ajuint pagewidth, ajuint pageheight);

void          ajGraphClose (void);
void          ajGraphCloseWin (void);

void          ajGraphDumpDevices (void);
void          ajGraphxyDumpDevices (void);

void          ajGraphicsCalcRange(const float *array, ajint npoints, float *min,
				  float *max);

PLFLT         *ajGraphicsCalcCoord(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
                                   PLFLT Angle);
PLFLT         ajGraphicsCalcTextlengthC(const char *text);
PLFLT         ajGraphicsCalcTextlengthS(const AjPStr str);
PLFLT         ajGraphicsCalcTextheight(void);
PLFLT         ajGraphicsCalcDistance(PLFLT xx1, PLFLT yy1,
                                     PLFLT xx2, PLFLT yy2);
PLFLT         ajGraphicsCalcCharsize(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                                     const char *text, PLFLT TextHeight);


void          ajGraphicsDrawarcArc(PLFLT xcentre, PLFLT ycentre, PLFLT radius,
                                   PLFLT startangle, PLFLT endangle);
void          ajGraphicsDrawbarsHoriz(ajuint num, PLFLT *y,
                                      PLFLT *xmin, PLFLT *xmax);
void          ajGraphicsDrawbarsVert(ajuint num, PLFLT *x,
                                     PLFLT *ymin, PLFLT *ymax);
void          ajGraphicsDrawposBox(PLFLT xx0, PLFLT yy0,PLFLT size);
void          ajGraphicsDrawposBoxFill(PLFLT xx0, PLFLT yy0, PLFLT size);
void          ajGraphicsDrawposCircle(PLFLT xcentre, PLFLT ycentre,
                                      float radius);
void          ajGraphicsDrawposDia(PLFLT xx0, PLFLT yy0, PLFLT size);
void          ajGraphicsDrawposDiaFill(PLFLT xx0, PLFLT yy0, PLFLT size);
void          ajGraphicsDrawsetDots(ajuint nhum, PLFLT *xx1,PLFLT *yy1);
void          ajGraphicsDrawposLine(PLFLT xx1,PLFLT yy1,PLFLT xx2,PLFLT yy2);
void          ajGraphicsDrawsetLines(ajuint num, PLFLT *xx,PLFLT *yy,
                                     PLFLT *xx2,PLFLT *yy2);
void          ajGraphicsDrawsetPoly(ajuint num, PLFLT *x, PLFLT *y);
void          ajGraphicsDrawsetPolyFill(ajuint num, PLFLT *x, PLFLT *y);
void          ajGraphicsDrawposRect(PLFLT x, PLFLT y,PLFLT xx1, PLFLT yy1) ;
void          ajGraphicsDrawposRectFill(PLFLT x, PLFLT y,
                                     PLFLT xx1, PLFLT yy1) ;
void          ajGraphicsDrawarcRect(PLFLT xcentre, PLFLT ycentre, PLFLT radius,
                                    PLFLT startangle, PLFLT endangle,
                                    PLFLT height);
void          ajGraphicsDrawarcRectFill(PLFLT xcentre, PLFLT ycentre,
                                        PLFLT radius,
                                        PLFLT startangle, PLFLT endangle,
                                        PLFLT height);
void          ajGraphicsSetRlabelC(const char *text);
void          ajGraphicsSetRlabelS(const AjPStr str);
void          ajGraphicsDrawsetSymbols(ajuint num, PLFLT *xx,PLFLT *yy,
                                       ajuint symbol);
void          ajGraphicsDrawposTextJustify(PLFLT x, PLFLT y,
                                           const char *txt,
                                           PLFLT just);
void          ajGraphicsDrawarcTextJustify(PLFLT xcentre, PLFLT ycentre,
                                           PLFLT radius, PLFLT startangle,
                                           PLFLT endangle,
                                           const char *txt, PLFLT just);
void          ajGraphicsDrawposTextAtend(PLFLT x, PLFLT y, const char *txt);
void          ajGraphicsDrawposTextAtmid(PLFLT x, PLFLT y, const char *txt);
void          ajGraphicsDrawposTextAtstart(PLFLT x, PLFLT y,
                                           const char *txt);
void          ajGraphicsDrawposTextAtlineJustify(PLFLT x, PLFLT y,
                                                 PLFLT x2, PLFLT y2,
                                                 const char *txt, PLFLT just);
void          ajGraphicsDrawposTri(PLFLT x, PLFLT y, PLFLT x2, PLFLT y2,
                                   PLFLT x3, PLFLT y3);
void          ajGraphicsDrawposTriFill(PLFLT x, PLFLT y, PLFLT x2, PLFLT y2,
                                       PLFLT x3, PLFLT y3);

AjPGraph      ajGraphNew (void);
AjPGraph      ajGraphxyNewI (ajuint numofpoints);

void          ajGraphClear(AjPGraph thys);

const AjPStr  ajGraphGetSubTitle(const AjPGraph thys);
const char*   ajGraphGetSubTitleC(const AjPGraph thys);
const AjPStr  ajGraphGetTitle(const AjPGraph thys);
const char*   ajGraphGetTitleC(const AjPGraph thys);
const AjPStr  ajGraphGetXTitle(const AjPGraph thys);
const char*   ajGraphGetXTitleC(const AjPGraph thys);
const AjPStr  ajGraphGetYTitle(const AjPGraph thys);
const char*   ajGraphGetYTitleC(const AjPGraph thys);

void          ajGraphInitSeq (AjPGraph thys, const AjPSeq seq);
AjBool        ajGraphIsData(const AjPGraph thys);


void          ajGraphNewpage (AjPGraph thys, AjBool resetdefaults);
void          ajGraphAddLine (AjPGraph thys, float xx1, float yy1,
			      float xx2, float yy2, ajint colour);
void          ajGraphAddRect (AjPGraph thys, float xx1, float yy1,
			      float xx2, float yy2, ajint colour,
			      ajint fill);
void          ajGraphAddText (AjPGraph thys, float xx1, float yy1,
				 ajint colour, const char *text);
void          ajGraphAddTextScale (AjPGraph thys, float xx1, float yy1,
				   ajint colour, float scale,
				   const char *text);
void          ajGraphOpen (AjPGraph thys, PLFLT xmin, PLFLT xmax,
			   PLFLT ymin, PLFLT ymax, ajint flags);
void          ajGraphOpenPlot (AjPGraph thys, ajuint numofsets);
void          ajGraphOpenWin  (AjPGraph thys, float xmin, float xmax,
			       float ymin, float ymax);
void          ajGraphOpenWinMm(AjPGraph thys, float * xmm, float *ymm);
AjBool        ajGraphSetDevicetype(AjPGraph thys, const AjPStr type);
void          ajGraphSetMulti (AjPGraph thys, ajuint numsets);
void          ajGraphicsSetFilename (const AjPGraph graph);
void          ajGraphTrace (const AjPGraph thys);
ajint         ajGraphDataAdd (AjPGraph thys, AjPGraphdata graphdata);
ajint         ajGraphDataReplace (AjPGraph thys, AjPGraphdata graphdata);
ajint         ajGraphDataReplaceI (AjPGraph thys, AjPGraphdata graphdata,
				   ajuint num);
void          ajGraphAppendTitleC(AjPGraph thys, const char* txt);
void          ajGraphAppendTitleS(AjPGraph thys, const AjPStr str);
void          ajGraphxyDel (AjPGraph* pmult);
void          ajGraphxyDisplay (AjPGraph thys, AjBool closeit );
void          ajGraphxyCheckMaxMin (AjPGraph thys);
AjBool        ajGraphxySetDevicetype (AjPGraph thys, const AjPStr devicetype);
void          ajGraphxySetCirclePoints (AjPGraph thys, AjBool set);
void          ajGraphSetFlag (AjPGraph thys, ajint flag, AjBool istrue);
void          ajGraphxySetJoinPoints (AjPGraph thys, AjBool set);
void          ajGraphxySetMaxMin(AjPGraph thys,float xmin,float xmax,
				 float ymin,float ymax);
void          ajGraphSetOutfileS (AjPGraph thys, const AjPStr txt);
void          ajGraphSetOutfileC (AjPGraph thys, const char *txt);
void          ajGraphxySetOverLap (AjPGraph thys, AjBool overlap);

void          ajGraphShowSubtitle (AjPGraph thys, AjBool set);
void          ajGraphShowTitle (AjPGraph thys, AjBool set);

void          ajGraphSetDescC(AjPGraph thys, const char* txt);
void          ajGraphSetDescS(AjPGraph thys, const AjPStr str);

void          ajGraphSetOutdirC(AjPGraph thys, const char* txt);
void          ajGraphSetOutdirS(AjPGraph thys, const AjPStr str);
void          ajGraphSetTitleC(AjPGraph thys, const char *title);
void          ajGraphSetTitleS(AjPGraph thys, const AjPStr title);
void          ajGraphSetSubtitleC(AjPGraph thys, const char *title);
void          ajGraphSetSubtitleS(AjPGraph thys, const AjPStr title);
void          ajGraphSetXlabelC(AjPGraph thys, const char *title);
void          ajGraphSetXlabelS(AjPGraph thys, const AjPStr title);
void          ajGraphSetYlabelC(AjPGraph thys, const char *title);
void          ajGraphSetYlabelS(AjPGraph thys, const AjPStr title);

void          ajGraphxySetGaps(AjPGraph thys, AjBool overlap);

void          ajGraphxyShowRaxis(AjPGraph thys, AjBool set);
void          ajGraphxyShowUaxis(AjPGraph thys, AjBool set);
void          ajGraphxyShowUnum(AjPGraph thys, AjBool set);
void          ajGraphxyShowXaxis(AjPGraph thys, AjBool set);
void          ajGraphxyShowXlabel(AjPGraph thys, AjBool set);
void          ajGraphxyShowXtick(AjPGraph thys, AjBool set);
void          ajGraphxyShowYaxis(AjPGraph thys, AjBool set);
void          ajGraphxyShowYlabel(AjPGraph thys, AjBool set);
void          ajGraphxyShowYnum(AjPGraph thys, AjBool set);
void          ajGraphxyShowYtick(AjPGraph thys, AjBool set);

void          ajGraphxySetXendF(AjPGraph thys, float val);
void          ajGraphxySetXgrid(AjPGraph thys, AjBool set);
void          ajGraphxySetXinvert(AjPGraph thys, AjBool set);
void          ajGraphxySetXrangeII(AjPGraph thys, ajint start, ajint end);
void          ajGraphxySetXstartF(AjPGraph thys, float val);
void          ajGraphxySetYendF(AjPGraph thys, float val);
void          ajGraphxySetYgrid(AjPGraph thys, AjBool set);
void          ajGraphxySetYinvert(AjPGraph thys, AjBool set);
void          ajGraphxySetYrangeII(AjPGraph thys, ajint start, ajint end);
void          ajGraphxySetYstartF(AjPGraph thys, float val);

AjPGraphdata  ajGraphdataNew (void);
AjPGraphdata  ajGraphdataNewI (ajuint numsets);

void          ajGraphdataDel(AjPGraphdata *thys);
void          ajGraphdataAddLine (AjPGraphdata graphs,
				     float xx1, float yy1,
				     float xx2, float yy2, ajint colour);
void          ajGraphdataAddRect (AjPGraphdata graphs,
				     float xx1, float yy1,
				     float xx2, float yy2,
				     ajint colour, ajint fill);
void          ajGraphdataAddText (AjPGraphdata graphs,
				     float xx1, float yy1,
				     ajint colour, const char *text);
void          ajGraphdataAddTextScale (AjPGraphdata graphs,
					  float xx1, float yy1,
					  ajint colour, float scale,
					  const char *text);
void          ajGraphdataSetMaxMin (AjPGraphdata graphdata, float xmin,
				      float xmax, float ymin, float ymax);

void          ajGraphdataSetMaxima(AjPGraphdata graphdata, float xmin,
				     float xmax, float ymin, float ymax);
void          ajGraphdataSetTypeC(AjPGraphdata graphdata,
				     const char *type);

void          ajGraphdataCalcXY (AjPGraphdata graphdata,
                                 ajuint numofpoints,
				    float start, float incr,
				    const float* y);
void          ajGraphdataSetXY (AjPGraphdata graphdata,
				   const float *x, const float *y);
void          ajGraphdataSetSubTitle  (AjPGraphdata graphdata,
					  const AjPStr title);
void          ajGraphdataSetSubTitleC (AjPGraphdata graphdata,
					  const char *title);
void          ajGraphdataSetTitle  (AjPGraphdata graphdata,
				       const AjPStr title);
void          ajGraphdataSetTitleC (AjPGraphdata graphdata,
				       const char *title);
void          ajGraphdataSetXTitle  (AjPGraphdata graphdata,
				       const AjPStr title);
void          ajGraphdataSetXTitleC (AjPGraphdata graphdata,
				       const char *title);
void          ajGraphdataSetYTitle  (AjPGraphdata graphdata,
				       const AjPStr title);
void          ajGraphdataSetYTitleC (AjPGraphdata graphdata,
				       const char *title);
void          ajGraphdataSetColour (AjPGraphdata graphdata,
				       ajint colour);
void          ajGraphdataSetLineType (AjPGraphdata graphdata,
					 ajint type);
void          ajGraphdataTrace (const AjPGraphdata thys);

void          ajGraphUnused(void);

/*
** End of prototype definitions
*/


/*
**  These prototypes do not have any corresponding .c code so cannot
**  go into the block above, otherwise the VC compilation will fail 
*/
void          ajGraphTraceInt (FILE* outf);

__deprecated PLFLT         ajGraphTextLength(PLFLT xx1, PLFLT yy1,
                                             PLFLT xx2, PLFLT yy2,
                                             const char *text);
__deprecated PLFLT         ajGraphTextHeight(PLFLT xx1, PLFLT xx2,
                                             PLFLT yy1, PLFLT yy2);
__deprecated PLFLT         ajGraphFitTextAtline(PLFLT xx1, PLFLT yy1,
                                                PLFLT xx2, PLFLT yy2,
                                                const char *text,
                                                PLFLT TextHeight);
__deprecated PLFLT         ajGraphDistPts(PLFLT xx1, PLFLT yy1,
                                          PLFLT xx2, PLFLT yy2);
__deprecated void          ajGraphArrayMaxMin(const float *array,
                                              ajint npoints,
                                              float *min, float *max);
__deprecated PLFLT         *ajComputeCoord(PLFLT xcentre, PLFLT ycentre,
                                           PLFLT Radius,  PLFLT Angle);
__deprecated ajint         ajGraphicsGetColourFore(void);
__deprecated void          ajGraphicsDrawRlabelC(const char *text);
__deprecated void          ajGraphicsDrawRlabelS(const AjPStr str);
__deprecated void          ajGraphNewPage (AjPGraph thys, AjBool resetdefaults);
__deprecated void          ajGraphSetTitlePlus(AjPGraph thys,
                                               const AjPStr title);
__deprecated AjBool        ajGraphxySet (AjPGraph thys, const AjPStr type);
__deprecated void          ajGraphSetOutputDirS(AjPGraph thys,
                                                const AjPStr str);
__deprecated void          ajGraphSetOut (AjPGraph thys, const AjPStr txt);
__deprecated void          ajGraphSetOutC (AjPGraph thys, const char *txt);
__deprecated void          ajGraphSetName (const AjPGraph thys);
__deprecated void          ajGraphSetPage(ajuint width, ajuint height);
__deprecated AjBool        ajGraphSet (AjPGraph thys, const AjPStr type);
__deprecated void          ajGraphSetPenWidth(float width);
__deprecated ajint         ajGraphSetFillPat (ajint style);
__deprecated ajint         ajGraphSetLineStyle (ajint style);

__deprecated void          ajGraphSetDevice(const AjPGraph thys);
__deprecated ajint         ajGraphInfo(AjPList* files);
__deprecated void          ajGraphGetOut (float *xp,float *yp,
                                          ajint *xleng, ajint *yleng,
                                          ajint *xoff, ajint *yoff);
__deprecated ajint         ajGraphGetColour(void);
__deprecated ajint*        ajGraphGetBaseColour (void);
__deprecated ajint*        ajGraphGetBaseColourNuc(const AjPStr codes);
__deprecated ajint*        ajGraphGetBaseColourProt(const AjPStr codes);
__deprecated float         ajGraphSetCharScale (float scale);
__deprecated float         ajGraphSetCharSize(float size);
__deprecated void          ajGraphGetCharSize(float *defheight,
                                              float *currentscale);
__deprecated float         ajGraphSetDefCharSize (float size);
__deprecated void          ajGraphxySetXBottom (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXLabelTop (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXTop (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXEnd (AjPGraph thys, float val);
__deprecated void          ajGraphxySetXGrid (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXInvTicks (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXLabel (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXRangeII (AjPGraph thys,
                                                 ajint start, ajint end);
__deprecated void          ajGraphxySetXStart (AjPGraph thys, float val);
__deprecated void          ajGraphxySetYEnd (AjPGraph thys, float val);
__deprecated void          ajGraphxySetYGrid (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYInvTicks (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYStart (AjPGraph thys, float val);
__deprecated void          ajGraphxySetYLeft (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYLabel (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXTick (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYLabelLeft (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYRangeII (AjPGraph thys,
                                                 ajint start, ajint end);
__deprecated void          ajGraphxySetYTick (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYRight(AjPGraph thys, AjBool set);
__deprecated void          ajGraphSetDir(AjPGraph thys, const AjPStr txt);
__deprecated void          ajGraphSetDesc(AjPGraph thys, const AjPStr title);
__deprecated void          ajGraphSetSubTitle (AjPGraph thys,
                                               const AjPStr title);
__deprecated void          ajGraphSetSubTitleC (AjPGraph thys,
                                                const char *title);
__deprecated void          ajGraphSetXTitle  (AjPGraph thys,
                                              const AjPStr title);
__deprecated void          ajGraphSetXTitleC (AjPGraph thys,
                                              const char *title);
__deprecated void          ajGraphSetYTitle  (AjPGraph thys,
                                              const AjPStr title);
__deprecated void          ajGraphSetYTitleC (AjPGraph thys,
                                              const char *title);
__deprecated void          ajGraphSetSubTitleDo (AjPGraph thys, AjBool set);
__deprecated void          ajGraphSetTitleDo (AjPGraph thys, AjBool set);
__deprecated void          ajGraphSetTitle (AjPGraph thys, const AjPStr title);
__deprecated void          ajGraphLabel (const char *x, const char *y,
                                         const char *title,
                                         const char *subtitle);
__deprecated void          ajGraphDrawTextOnCurve(PLFLT xcentre, PLFLT ycentre,
                                                  PLFLT Radius,
                                                  PLFLT StartAngle,
                                                  PLFLT EndAngle,
                                                  const char *Text, PLFLT just);
__deprecated void          ajGraphFillRectangleOnCurve(PLFLT xcentre,
                                                       PLFLT ycentre,
                                                       PLFLT Radius,
                                                       PLFLT BoxHeight,
                                                       PLFLT StartAngle,
                                                       PLFLT EndAngle);

__deprecated void          ajGraphRectangleOnCurve(PLFLT xcentre, PLFLT ycentre,
                                                   PLFLT Radius,
                                                   PLFLT BoxHeight,
                                                   PLFLT StartAngle,
                                                   PLFLT EndAngle);
__deprecated void          ajGraphPartCircle(PLFLT xcentre, PLFLT ycentre,
                                             PLFLT Radius,
                                             PLFLT StartAngle, PLFLT EndAngle);
__deprecated void          ajGraphBox (PLFLT xx0, PLFLT yy0,PLFLT size);
__deprecated void          ajGraphBoxFill (PLFLT xx0, PLFLT yy0, PLFLT size);
__deprecated void          ajGraphRectFill (PLFLT xx0, PLFLT yy0,
                                            PLFLT xx1, PLFLT yy1) ;
__deprecated void          ajGraphRect (PLFLT xx0, PLFLT yy0,
                                        PLFLT xx1, PLFLT yy1) ;
__deprecated void          ajGraphLine (PLFLT xx1,PLFLT yy1,
                                        PLFLT xx2,PLFLT yy2);
__deprecated void          ajGraphCircle (PLFLT xcentre, PLFLT ycentre,
                                          float radius);
__deprecated void          ajGraphDia (PLFLT xx0, PLFLT yy0, PLFLT size);
__deprecated void          ajGraphDiaFill (PLFLT xx0, PLFLT yy0, PLFLT size);
__deprecated void          ajGraphPoly (ajuint n, PLFLT *x, PLFLT *y);
__deprecated void          ajGraphPolyFill (ajuint n, PLFLT *x, PLFLT *y);
__deprecated void          ajGraphLines (PLFLT *xx1,PLFLT *yy1,
                                         PLFLT *xx2,PLFLT *yy2,
                                         ajuint numoflines);
__deprecated void          ajGraphTextLine (PLFLT xx1, PLFLT yy1,
                                            PLFLT xx2, PLFLT yy2,
                                            const char *text, PLFLT just);
__deprecated void          ajGraphTri (PLFLT xx1, PLFLT yy1,
                                       PLFLT xx2, PLFLT yy2,
                                       PLFLT x3, PLFLT y3);
__deprecated void          ajGraphTriFill (PLFLT xx1, PLFLT yy1,
                                           PLFLT xx2, PLFLT yy2,
                                           PLFLT x3, PLFLT y3);
__deprecated void          ajGraphDots (PLFLT *xx1,PLFLT *yy1, ajuint numofdots);
__deprecated void          ajGraphSymbols( ajuint numofdots,
                                           PLFLT *xx1,PLFLT *yy1,
                                           ajuint symbol);
__deprecated void          ajGraphText (PLFLT xx1, PLFLT yy1,
                                        const char *text, PLFLT just);
__deprecated void          ajGraphTextEnd (PLFLT xx1, PLFLT yy1,
                                           const char *text);
__deprecated void          ajGraphTextMid (PLFLT xx1, PLFLT yy1,
                                           const char *text);
__deprecated void          ajGraphTextStart (PLFLT xx1, PLFLT yy1,
                                             const char *text);

__deprecated void          ajGraphHoriBars (ajuint numofpoints, PLFLT *y,
                                            PLFLT *xmin, PLFLT *xmax);
__deprecated void          ajGraphVertBars (ajuint numofpoints, PLFLT *x,
                                            PLFLT *ymin, PLFLT *ymax);

__deprecated void          ajGraphSetBackBlack (void);
__deprecated void          ajGraphSetBackWhite (void);
__deprecated void          ajGraphColourBack (void);
__deprecated void          ajGraphColourFore(void);
__deprecated ajint         ajGraphSetFore (ajint colour);

__deprecated void          ajGraphInit (const char *pgm,
                                        ajint argc, char * const argv[]);
__deprecated void          ajGraphInitPV(const char *pgm,
                                         ajint argc, char * const argv[],
                                         const char *package,
                                         const char *packversion);
__deprecated void          ajGraphLabelYRight (const char *text);

__deprecated void          ajGraphSetOri(ajint ori);

__deprecated ajint         ajGraphCheckColour (const AjPStr colour);

/* End of prototypes without C code */



#endif /* ajgraph_h */



#ifdef __cplusplus
}
#endif
