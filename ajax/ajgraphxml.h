#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajgraphxml_h
#define ajgraphxml_h

#ifdef GROUT

#include "gdome.h"

/* @data AjPXmlNode ***********************************************************
**
** Xml graph node
**
** @alias AjSXmlNode
** @alias AjOXmlNode
**
** @attr theNode [GdomeNode*] Undocumented
******************************************************************************/

typedef struct AjSXmlNode
{
    GdomeNode *theNode;
} AjOXmlNode, *AjPXmlNode;

/* @data AjPXmlFile ***********************************************************
**
** Xml graph file
**
** @alias AjSXmlFile
** @alias AjOXmlFile
** @alias AjPGraphXml
**
** @attr domimpl [GdomeDOMImplementation*] Undocumented
** @attr doc [GdomeDocument*] Undocumented
** @attr currentGraphic [AjPXmlNode] Undocumented
** @attr currentScene [AjPXmlNode] Undocumented
** @attr colour [double[3]] Undocumented
** @attr nodeTypes [AjPTable] Undocumented
******************************************************************************/

typedef struct AjSXmlFile
{
    GdomeDOMImplementation *domimpl;
    GdomeDocument *doc;
    AjPXmlNode currentGraphic;
    AjPXmlNode currentScene;
    double colour[3];
    AjPTable nodeTypes;
} AjOXmlFile, *AjPXmlFile;

#define AjPGraphXml AjOXmlFile*

AjPXmlNode	ajXmlNodeNew();
AjPGraphXml 	ajXmlCreateNewOutputFile();
AjPGraphXml 	ajXmlFileNew();

void 	ajXmlNodeDel(AjPXmlNode *thys);

void 	ajXmlFileDel(AjPGraphXml *thys);

AjBool 	ajXmlSetMaxMin(AjPGraphXml file, double xMin, double yMin,
			       double xMax, double yMax);
AjBool 	ajXmlWriteFile(AjPGraphXml file, AjPStr filename);
AjBool 	ajXmlWriteStdout(AjPGraphXml file);
void 	ajXmlClearFile(AjPGraphXml file);
AjBool 	ajXmlSetSource(AjPGraphXml file, AjPStr title);
AjBool 	ajXmlAddMainTitleC(AjPGraphXml file, char *title);
AjBool 	ajXmlAddXTitleC (AjPGraphXml file, char *title);
AjBool 	ajXmlAddYTitleC (AjPGraphXml file, char *title);
AjBool 	ajXmlAddMainTitle(AjPGraphXml file, AjPStr title);
AjBool 	ajXmlAddXTitle(AjPGraphXml file, AjPStr title);
AjBool 	ajXmlAddYTitle(AjPGraphXml file, AjPStr title);
void 	ajXmlAddText(AjPGraphXml file, double x, double y,
		     double size, double angle, AjPStr fontFamily,
		     AjPStr fontStyle, AjPStr text);
void 	ajXmlAddTextC(AjPGraphXml file, double x, double y,
		      double size, double angle, char *fontFamily,
		      char *fontStyle, char *text);
void 	ajXmlAddTextCentred(AjPGraphXml file, double x, double y,
			    double size, double angle,
			    AjPStr fontFamily, AjPStr fontStyle,
			    AjPStr text);
void 	ajXmlAddTextWithJustify(AjPGraphXml file, double x, double y,
				double size, double angle,
				AjPStr fontFamily, AjPStr fontStyle,
				AjPStr text, AjBool horizontal,
				AjBool leftToRight,
				AjBool topToBottom,
				AjPStr justifyMajor,
				AjPStr justifyMinor);
void 	ajXmlAddTextWithCJustify(AjPGraphXml file, double x, double y,
				 double size, double angle,
				 AjPStr fontFamily, AjPStr fontStyle,
				 AjPStr text, AjBool horizontal,
				 AjBool leftToRight,
				 AjBool topToBottom,
				 char *justifyMajor,
				 char *justifyMinor);
void 	ajXmlAddTextOnArc(AjPGraphXml file, double xCentre,
			  double yCentre, double startAngle,
			  double endAngle, double radius,
			  double size, AjPStr fontFamily,
			  AjPStr fontStyle, AjPStr text);
void 	ajXmlAddJoinedLineSetEqualGapsF(AjPGraphXml file, float *y,
					int numberOfPoints,
					float startX, float increment);
void 	ajXmlAddJoinedLineSet(AjPGraphXml file, double *x, double *y,
			      int numberOfPoints);
void 	ajXmlAddJoinedLineSetF(AjPGraphXml file, float *x, float *y,
			       int numberOfPoints);
void 	ajXmlAddLine(AjPGraphXml file,
		     double x1, double y1, double x2, double y2);
void 	ajXmlAddLineF(AjPGraphXml file, float x1, float y1,
		      float x2, float y2);
void 	ajXmlAddPoint(AjPGraphXml file, double x1, double y1);
void 	ajXmlAddRectangle(AjPGraphXml file, double x1, double y1, double x2,
			  double y2, AjBool fill);
void 	ajXmlAddHistogramEqualGapsF(AjPGraphXml file, float *y, int numPoints,
				    float startX, float xGap);
void 	ajXmlAddRectangleSet(AjPGraphXml file, double *x1, double *y1,
			     double *x2, double *y2, int numPoints,
			     AjBool fill);
void 	ajXmlAddCylinder(AjPGraphXml file, double x1, double y1, double x2,
			 double y2, double width);
AjBool 	ajXmlAddPointLabelCircle(AjPGraphXml file, double angle,
				 double xCentre, double yCentre,
				 double radius, double length, double size,
				 AjPStr fontFamily, AjPStr fontStyle,
				 AjPStr text);
AjBool 	ajXmlAddSectionLabelCircle(AjPGraphXml file, double startAngle,
				   double endAngle, double xCentre,
				   double yCentre, double radius,
				   double width, double labelArmAngle,
				   AjPStr labelStyle, double textPosition,
				   double size, AjPStr fontFamily,
				   AjPStr fontStyle, AjPStr text);
AjBool 	ajXmlAddPointLabelLinear(AjPGraphXml file, double angle, double xPoint,
				 double yPoint, double length,
				 AjBool textParallelToLine, double size,
				 AjPStr fontFamily, AjPStr fontStyle,
				 AjPStr text);
AjBool 	ajXmlAddSectionLabelLinear(AjPGraphXml file, double xStart,
				   double yStart, double xEnd, double yEnd,
				   double width, double labelArmLength,
				   AjPStr labelStyle, double textPosition,
				   double size, AjPStr fontFamily,
				   AjPStr fontStyle, AjPStr text);
AjBool 	ajXmlAddSquareResidueLinear(AjPGraphXml file, char residue, float x,
				    float y);
AjBool 	ajXmlAddOctagonalResidueLinear(AjPGraphXml file, char residue, float x,
				       float y);
AjBool 	ajXmlAddDiamondResidueLinear(AjPGraphXml file, char residue, float x,
				     float y);
AjBool 	ajXmlAddNakedResidueLinear(AjPGraphXml file, char residue, float x,
				   float y);
AjBool 	ajXmlAddSquareResidue(AjPGraphXml file, char residue, double radius,
			      double angle);
AjBool 	ajXmlAddOctagonalResidue(AjPGraphXml file, char residue, double radius,
				 double angle);
AjBool 	ajXmlAddDiamondResidue(AjPGraphXml file, char residue, double radius,
			       double angle);
AjBool 	ajXmlAddNakedResidue(AjPGraphXml file, char residue, double radius,
			     double angle);
float 	ajXmlFitTextOnLine(float x1, float y1, float x2, float y2,
			   AjPStr text);
void 	ajXmlGetColour(AjPGraphXml file, double r, double g, double b);
void 	ajXmlSetColour(AjPGraphXml file, double r, double g, double b);
void 	ajXmlSetColourFromCode(AjPGraphXml file, ajint colour);
void 	ajXmlAddGraphic(AjPGraphXml file, AjPStr type);
void 	ajXmlAddGraphicC(AjPGraphXml file, char *type);

void 	ajXmlAddCircle(AjPGraphXml file, double xCentre, double yCentre,
		       double radius);
void 	ajXmlAddCircleF(AjPGraphXml file, float xCentre, float yCentre,
			float radius);
void 	ajXmlAddArc(AjPGraphXml file, double xCentre, double yCentre,
		    double startAngle, double endAngle, double radius);
void 	ajXmlAddGroutOption(AjPGraphXml file, AjPStr name, AjPStr value);
void 	ajXmlAddGroutOptionC(AjPGraphXml file, char *name, char *value);


#else

/* dummy for plplot-only graphics */

#define AjPGraphXml void*

#endif

#endif

#ifdef __cplusplus
}
#endif
