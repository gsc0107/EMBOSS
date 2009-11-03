/******************************************************************************
** @source AJAX GRAPH (ajax graphics) functions
** @author Ian Longden
** These functions control all aspects of AJAX graphics.
**
** @version 1.0
** @modified 1988-11-12 pmr First version
** @modified 1999 ajb ANSI
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

#define GRAPHMAIN 1

#include "ajax.h"
#include "ajgraph.h"

#include <math.h>
#include <limits.h>
#include <float.h>
#define AZ 28


#include "plplotP.h"

/*
** Andre Blavier:
** The following function defs come from the old version of PLPLOT (the one
** used in EMBOSS 2.6.0).  I have changed some of them slightly, despite
** my poor understanding of their purpose and of the meaning of their
** disappearance in PLPLOT 5.1.0
**
** Peter Rice:
** Updating to PLPLOT 5.7.2 we still need these to find out how wide a text
** string is
*/


static void     GraphArray(ajint numofpoints,
			   float *x, float *y);
static void     GraphArrayGaps(ajint numofpoints,
			       float *x, float *y);
static void     GraphArrayGapsI(ajint numofpoints, ajint *x, ajint *y);
static void     GraphCharScale(float size);
static void     GraphCheckFlags(ajint flags);
static void     GraphCheckPoints(ajint n, const PLFLT *x, const PLFLT *y);
static void     GraphClose(void);
static void     GraphColourBack(void);
static void     GraphColourFore(void);
static void     GraphDatafileNext(void);
static void     GraphDataDraw(const AjPGraphdata graphdata);
static void     GraphDataPrint(const AjPGraphdata graphdata);
static void     GraphDefCharSize(float size);
static void     GraphDrawLines( ajint numoflines,PLFLT *xx1, PLFLT *yy1);
static void     GraphFill(ajint numofpoints, float *x, float *y);
static void     GraphFillPat(ajint pat);
static void     GraphInit(AjPGraph thys);
static void     GraphLabelTitle(const char *title, const char *subtitle);
static void     GraphLineStyle(ajint style);
static void     GraphNewPlplot(AjPGraph graph);
static void     GraphDraw(const AjPGraph thys);
static void     GraphPrint(const AjPGraph thys);
static void     GraphOpenData(AjPGraph thys, const char *ext);
static void     GraphOpenFile(AjPGraph thys, const char *ext);
static void     GraphOpenSimple(AjPGraph thys, const char *ext);
#ifndef X_DISPLAY_MISSING
static void     GraphOpenXwin(AjPGraph thys, const char *ext);
#endif
static void     GraphPen(ajint pen, ajint red, ajint green, ajint blue);
static void     GraphSetName(const AjPGraph thys,
			     const AjPStr txt, const char *ext);
static void     GraphSetNumSubPage(ajint numofsubpages);
static void     GraphSetPen(ajint colour);
static void     GraphSetWin(float xmin, float xmax, float ymin, float ymax);
static void     GraphSetWin2(float xmin, float xmax, float ymin, float ymax);
static void     GraphSubPage(ajint page);
static void     GraphSymbols(float *xx1, float *yy1, ajint numofdots,
			     ajint symbol);
static void     GraphText(float xx1, float yy1, float xx2, float yy2,
			  float just, const char *text);
static void     GraphWind(float xmin, float xmax, float ymin, float ymax);
static void     GraphxyDisplayToDas(AjPGraph thys, AjBool closeit,
				     const char *ext);
static void     GraphxyDisplayToData(AjPGraph thys, AjBool closeit,
				     const char *ext);
static void     GraphxyDisplayToFile(AjPGraph thys, AjBool closeit,
				     const char *ext);
static void     GraphxyDisplayXwin(AjPGraph thys, AjBool closeit,
				   const char *ext);
static void     GraphxyGeneral(AjPGraph thys, AjBool closeit);
static void     GraphdataInit(AjPGraphdata graphdata);
static void     GraphxyNewPlplot(AjPGraph thys);
static void     GraphDrawTextOncurve(PLFLT xcentre, PLFLT ycentre,
                                     PLFLT Radius, PLFLT Angle, PLFLT pos,
                                     const char *Text, PLFLT just);

/* PLPLOT CALLS *START* */

static PLFLT plstrlW(PLFLT dx, PLFLT dy, const char *text);
static PLFLT plgchrW(PLFLT dx, PLFLT dy);
static void plxswin(const char* window);
static int plfileinfo(char* tmp);

/* @funcstatic plstrlW ********************************************************
**
** Old PLPLOT version function. Obsolete. Needed for EMBOSSWIN compilation
**
** @param [r] dx [PLFLT] Undocumented
** @param [r] dy [PLFLT] Undocumented
** @param [r] text [const char*] Undocumented
** @return [PLFLT] Undocumented
** @@
** Andre Blavier:
** The following function defs come from the old version of PLPLOT (the one
** used in EMBOSS 2.6.0).  I have changed some of them slightly, despite
** my poor understanding of their purpose and of the meaning of their
** disappearance in PLPLOT 5.1.0
******************************************************************************/

static PLFLT plstrlW(PLFLT dx, PLFLT dy, const char *text)
{
    PLFLT diag;
    /*
     ** set an arbitrary char default size; don't know why this is not set yet
     ** nor how it should be appropriately done
     */
    if (plsc->chrdef == 0.0)
	c_plschr(2.0, 1.0);

    if (dx == 0.0 && dy !=0.0)
	diag = plstrl(text)/plsc->wmyscl;
    else if (dy == 0.0 && dx !=0.0)
	diag = plstrl(text)/plsc->wmxscl;
    else
	diag = (PLFLT) sqrt((plstrl(text)/plsc->wmxscl) *
			    (plstrl(text)/plsc->wmxscl) + 
			    (plstrl(text)/plsc->wmyscl) *
			    (plstrl(text)/plsc->wmyscl));

    return diag;
}




/* @funcstatic plgchrW ********************************************************
**
** Old PLPLOT version function. Obsolete. Needed for EMBOSSWIN compilation
**
** @param [r] dx [PLFLT] Undocumented
** @param [r] dy [PLFLT] Undocumented
** @return [PLFLT] Undocumented
** @@
** Andre Blavier:
** The following function defs come from the old version of PLPLOT (the one
** used in EMBOSS 2.6.0).  I have changed some of them slightly, despite
** my poor understanding of their purpose and of the meaning of their
** disappearance in PLPOT 5.1.0
******************************************************************************/

static PLFLT plgchrW(PLFLT dx, PLFLT dy)
{
    PLFLT diag;
    /*
     ** set an arbitrary char default size; don't know why this is not set yet
     ** nor how it should be appropriately done
     */

    if (plsc->chrdef == 0.0)
	c_plschr(2.0, 1.0);

    if (dx == 0.0 && dy !=0.0)
	diag = plsc->chrdef/plsc->wmxscl;
    else if(dy == 0.0 && dx !=0.0)
	diag = plsc->chrdef/plsc->wmyscl;
    else
	diag = (PLFLT) sqrt((plsc->chrdef/plsc->wmxscl) *
			    (plsc->chrdef/plsc->wmxscl) +
			    (plsc->chrdef/plsc->wmyscl) *
			    (plsc->chrdef/plsc->wmyscl));

    return diag;
}





/* @funcstatic  plxswin *******************************************************
**
** Old PLPLOT version function. Obsolete. Needed for EMBOSSWIN compilation
**
** @param [r] window [const char*] Undocumented
** @return [void]
** @@
** Andre Blavier:
** The following function defs come from the old version of PLPLOT (the one
** used in EMBOSS 2.6.0).  I have changed some of them slightly, despite
** my poor understanding of their purpose and of the meaning of their
** disappearance in PLPOT 5.1.0
******************************************************************************/


static void plxswin(const char* window)
{
    if (plsc->plwindow != NULL)
	free((void *) plsc->plwindow);

    plsc->plwindow = (char *) malloc(1 + strlen(window));

    strcpy(plsc->plwindow, window);

    return;
}




/* @funcstatic plfileinfo *****************************************************
**
** Old PLPLOT version function. Obsolete. Needed for EMBOSSWIN compilation
**
** @param [w] tmp [char*] Undocumented
** @return [int] Undocumented
** @@
** Andre Blavier:
** The following function defs come from the old version of PLPLOT (the one
** used in EMBOSS 2.6.0).  I have changed some of them slightly, despite
** my poor understanding of their purpose and of the meaning of their
** disappearance in PLPOT 5.1.0
******************************************************************************/


static int plfileinfo (char* tmp)
{
    
    if (plsc->family && plsc->BaseName != NULL)
    {					/* family names*/
	(void) sprintf(tmp, "%s.%%0%1dd%s",
		       plsc->BaseName, (int) plsc->fflen,plsc->Ext);

	return plsc->member;
    }
    
    if (plsc->FileName == NULL)
    {
	*tmp='\0';

	return 0;
    }
    
    /* simple filenames */
    sprintf(tmp, "%s", plsc->FileName);

    return -1;
}


/* PLPLOT CALLS *END* */




enum AjEGraphObjectTypes { RECTANGLE, RECTANGLEFILL, TEXT, LINE};

static const char *graphColourName[] =
{
    "BLACK", "RED", "YELLOW", "GREEN",
    "AQUAMARINE", "PINK", "WHEAT",
    "GREY", "BROWN", "BLUE", "BLUEVIOLET",
    "CYAN", "TURQUOISE", "MAGENTA",
    "SALMON", "WHITE"
};

struct graphSData
{
    AjPFile  File;
    AjPStr   FName;
    AjPStr   Ext;
    ajint    Num;
    ajint    Lines;
    float    xmin;
    float    ymin;
    float    xmax;
    float    ymax;
    AjPList  List;
} *graphData=NULL;



/* @filesection ajgraph *******************************************************
**
** @nam1rule aj Function belongs to the AJAX library.
**
*/






/* @datastatic GraphPType *****************************************************
**
** Graph type definition structure
**
** @alias GraphSType
** @alias GraphOType
**
** @attr Name [const char*] Name used by Ajax
** @attr Device [const char*] Name used by plplot library
** @attr Ext [const char*] File extension for output file if any
** @attr Width [ajint] Default width within the driver, 0 if not a plot
** @attr Height [ajint] Default height within the driver, 0 if not a plot
** @attr Plplot [AjBool] true if using PlPlot data and library calls
** @attr Alias [AjBool] ajTrue if this name is a user alias for another
**                      entry with the same Device name.
** @attr Fixedsize [AjBool] true if plplot fixes the plot window size,
**                          ignoring user settings
** @attr Interactive[AjBool] true if plplot can interact with the device
** @attr XYDisplay [(void*)] Function to display an XY graph
** @attr GOpen [(void*)] Function to display a general graph
** @attr Desc [const char*] Description
** @@
******************************************************************************/

typedef struct GraphSType
{
    const char* Name;
    const char* Device;
    const char* Ext;
    ajint Width;
    ajint Height;
    AjBool Plplot;
    AjBool Alias;
    AjBool Fixedsize;
    AjBool Interactive;
    void (*XYDisplay) (AjPGraph thys, AjBool closeit, const char *ext);
    void (*GOpen) (AjPGraph thys, const char *ext);
    const char* Desc;
} GraphOType;
#define GraphPType GraphOType*




/* @funclist graphType ********************************************************
**
** Graph functions to open and display a graph with the PLPLOT
** device drivers
**
******************************************************************************/

static GraphOType graphType[] =
{
    /*Name         Device     Extension Width Height
     PlPlot   Alias    Fixedsize Interactive
     DisplayFunction       OpenFunction
     Description*/
    {"ps",         "ps",      ".ps",    720,  540,
     AJTRUE,  AJFALSE, AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "Postscript"},
    {"postscript", "ps",      ".ps",    720,  540,
     AJTRUE,  AJTRUE,  AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "Postscript"},

#ifndef WIN32
    {"hpgl",       "lj_hpgl", ".hpgl",  11000,7200,
     AJTRUE,  AJFALSE,  AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "HPGL general"},

    {"hp7470",     "hp7470",  ".hpgl",  10300,7650,
     AJTRUE,  AJFALSE,  AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "HPGL 7470 pen plotter"},

    {"hp7580",     "hp7580",  ".hpgl",  9000, 5580,
     AJTRUE,  AJFALSE,  AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "HPGL 7580 pen plotter"},
#endif

    {"meta",       "plmeta",  ".meta",  0,    0,
     AJTRUE,  AJFALSE,  AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "Metafile"},

    {"cps",        "psc",     ".ps",    720,  540,
     AJTRUE,  AJFALSE, AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "Colour postscript"},
    {"colourps",   "psc",     ".ps",    720,  540,
     AJTRUE,  AJTRUE,  AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "Colour postscript"},

#ifndef WIN32
#ifndef X_DISPLAY_MISSING /* X11 is  available */
    {"x11",        "xwin",    "null",   1440, 900,
     AJTRUE,  AJFALSE, AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenXwin,
     "X11 in new window"},
    {"xwindows",   "xwin",    "null",   1440, 900,
     AJTRUE,  AJTRUE,  AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenXwin,
     "X11 in new window"},
#endif

    {"tek",        "tek4107t","null",   1024, 780,
     AJTRUE,  AJFALSE, AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "Tektronix screen graphics"},
    {"tek4107t",   "tek4107t","null",   1024, 780,
     AJTRUE,  AJTRUE,  AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "Tektronix model 4107 screen graphics"},
    {"tekt",       "tekt",    "null",   1024, 780,
     AJTRUE,  AJFALSE, AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "Tektronix screen graphics"},
    {"tektronics", "tekt",    "null",   1024, 780,
     AJTRUE,  AJTRUE,  AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "Tektronix screen graphics"},

#endif

    {"none",       "null",    "null",  0,    0,
     AJTRUE,  AJFALSE, AJFALSE,  AJFALSE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "No output"},
    {"null",       "null",    "null",  0,    0,
     AJTRUE,  AJTRUE,  AJFALSE,  AJFALSE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "No output"},
    {"text",       "null",    "null",  0,    0,
     AJTRUE,  AJTRUE,  AJFALSE,  AJFALSE,
     GraphxyDisplayXwin,   GraphOpenSimple,
     "Text file"},
/* data file output */
  
    {"data",       "data",    ".dat",  960,  768,
     AJTRUE,  AJFALSE, AJTRUE,  AJFALSE,
     GraphxyDisplayToData, GraphOpenData,
     "Data file for Staden package"},

    {"das",        "das",     ".das",  0,    0,
     AJTRUE,  AJFALSE, AJFALSE,  AJFALSE,
     GraphxyDisplayToDas,  NULL,
     "Data file for DAS server"},

#ifndef WIN32
#ifndef X_DISPLAY_MISSING /* X11 is available */
    {"xterm",      "xterm",   "null",   1024, 780,
     AJTRUE,  AJFALSE, AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,   GraphOpenXwin,
     "Xterm screen graphics"},
#endif
#endif

#ifdef PLD_png          /* if png/gd/zlib libraries available for png driver */
    {"png",        "png",     ".png",   640,  480,
     AJTRUE,  AJFALSE, AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "PNG graphics files"},
#ifndef WIN32
    {"gif",        "gif",     ".gif",   640,  480,
     AJTRUE,  AJFALSE, AJFALSE,  AJFALSE,
     GraphxyDisplayToFile, GraphOpenFile,
     "GIF graphics files"},
#endif
#endif

#ifdef WIN32
    {"win3",        "win3",     "null", 720,  540,
     AJTRUE, AJFALSE, AJFALSE,  AJTRUE,
     GraphxyDisplayXwin,     GraphOpenXwin,
     "Windows screen graphics"},
#endif

    {NULL, NULL, NULL, 0, 0,
     AJFALSE, AJFALSE, AJFALSE, AJFALSE,
     NULL, NULL,
     NULL}
};


#ifndef NO_PLOT




static ajuint currentfgcolour = BLACK;/* Change with ajGraphicsSetColourFore */
static AjBool currentbgwhite = AJTRUE;      /* By default white background */
static PLFLT  currentcharsize   = 4.0; /* Use ajGraphSetCharSize to change */
static PLFLT  currentcharscale  = 1.0; /* Use ajGraphSetCharccale to change */
static PLINT  currentlinestyle = 1;     /* Use ajGraphSetLineStyle to change*/
static PLINT  currentfillpattern = 1;     /* Use ajGraphSetLineStyle to change*/



/* @datasection [none] Graphics internals **************************************
**
** Function is for controlling the graphics internals without using
** an AjPGraph object
**
** @nam2rule Graphics
*/


/******************************************************************************
plplot calls should only be made once from now on so all calls to plplot
should be made via ajGraph calls. So why bother? This is to make it easier
to change to another graph package as all the plplot calls should only be
listed once hence only a few routines should need to be changed?? Well in
theory anyway.
******************************************************************************/

/******************************************************************************
PLPLOT CALLS *START*
******************************************************************************/




/* @func ajGraphicsSetBgcolourBlack ********************************************
**
** Set the background colour to black
**
** @return [void]
** @@
******************************************************************************/

void ajGraphicsSetBgcolourBlack(void)
{
    ajDebug("ajGraphicsSetBgcolourBlack currentbgwhite: %B\n", currentbgwhite);

    if(currentbgwhite)
    {
	currentbgwhite = ajFalse;

	GraphPen(0, 0, 0, 0);
	GraphPen(MAXCOL, 255, 255, 255);

	graphColourName[0] = "BLACK";
	graphColourName[MAXCOL] = "WHITE";

	ajDebug("ajGraphicsSetBgcolourBlack pen 0 '%s' pen %d '%s'\n",
		graphColourName[0], MAXCOL, graphColourName[MAXCOL]);
    }

    return;
}


/* @obsolete ajGraphSetBackBlack
** @rename ajGraphicsSetBgcolourBlack
*/

__deprecated void ajGraphSetBackBlack(void)
{
    ajGraphicsSetBgcolourBlack();
    return;
}


/* @func ajGraphicsSetColourReset **********************************************
**
** Reset the foreground plotting colour using current stored colour.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphicsSetColourReset(void)
{
    GraphColourFore();
    return;
}

/* @obsolete ajGraphColourFore
** @rename ajGraphicsSetColourReset
*/

__deprecated void ajGraphColourFore(void)
{
    GraphColourFore();
    return;
}


/* @func ajGraphicsSetBgcolourWhite ********************************************
**
** Set the background colour to white (the default colour)
**
** @return [void]
** @@
******************************************************************************/

void ajGraphicsSetBgcolourWhite(void)
{
    ajDebug("ajGraphicsSetBgcolourWhite currentbgwhite: %B\n", currentbgwhite);

    if(!currentbgwhite)
    {
	currentbgwhite = ajTrue;

	GraphPen(0, 255, 255, 255);
	GraphPen(MAXCOL, 0, 0, 0);

	graphColourName[0] = "WHITE";
	graphColourName[MAXCOL] = "BLACK";

	ajDebug("ajGraphicsSetBgcolourWhite pen 0 '%s' pen %d '%s'\n",
		graphColourName[0], MAXCOL, graphColourName[MAXCOL]);
    }

    return;
}




/* @obsolete ajGraphSetBackWhite
** @rename ajGraphicsSetBgcolourkWhite
*/

__deprecated void ajGraphSetBackWhite(void)
{
    ajGraphicsSetBgcolourWhite();
    return;
}

/* @func ajGraphicsSetColourFore ***********************************************
**
** Set the foreground plotting colour
**
** @param [r] colour [ajint]  colour to set drawing to.
** @return [ajint] the previous colour.
** @@
******************************************************************************/

ajint ajGraphicsSetColourFore(ajint colour)
{
    ajint oldcolour;
    ajint col;

    col = colour;

    /* in case of looping through colours */
    /* start at the begining once past end */
    while(col > MAXCOL)
	col -= (MAXCOL+1);

    if(col < 0)
	ajDebug("ajGraphicsSetColourFore give up and use "
                "currentfgcolour %d '%s'\n",
		 currentfgcolour, graphColourName[currentfgcolour]);
    if(col < 0)
	return currentfgcolour;

    oldcolour = currentfgcolour;
    currentfgcolour = col;

    if(currentbgwhite)          /* swap black and white foreground */
    {
	if(col == 0)
	    col = MAXCOL;
	else if(col == MAXCOL)
	    col = 0;
    }

    ajDebug("ajGraphicsSetColourFore (%d '%s') currentbgwhite: %B, "
	    "oldcolour: %d '%s'\n",
	    colour, graphColourName[colour], currentbgwhite,
	    oldcolour, graphColourName[oldcolour]);

    GraphColourFore();

    return oldcolour;
}


/* @obsolete ajGraphSetFore
** @rename ajGraphicsSetColourFore
*/

__deprecated ajint ajGraphSetFore(ajint colour)
{

    return ajGraphicsSetColourFore(colour);
}


/* @funcstatic GraphColourFore ************************************************
**
** Set the foreground plotting colour using current stored colour.
**
** @return [void]
** @@
******************************************************************************/

static void GraphColourFore(void)
{
    ajint colour;

    colour = currentfgcolour;

    if(currentbgwhite)	 /* OKAY!! when we swap backgrounds we also */
    {
	if(colour == 0)	/* swap pens. User does not know this so switch */
	    colour = MAXCOL;		/* for them */
	else if(colour == MAXCOL)
	    colour = 0;
    }

    GraphSetPen(colour);
    ajDebug("GraphColourFore currentbgwhite: %B '%s' "
	    "currentfgcolour: %d (%s) => %d (%s)\n",
	    currentbgwhite, graphColourName[0],
	    currentfgcolour, graphColourName[currentfgcolour],
	    colour, graphColourName[colour]);

    return;
}




/* @func ajGraphicsSetCharscale ************************************************
**
** Set the character scale factor
**
** @param [r] scale [float]  character scale.
** @return [float] the previous character scale factor.
** @@
******************************************************************************/

float ajGraphicsSetCharscale(float scale)
{
    float oldscale;

    oldscale = currentcharscale;

    if(scale)
    {
	currentcharscale = scale;
	GraphCharScale(currentcharscale);
    }

    return oldscale;
}



/* @obsolete ajGraphSetCharScale
** @rename ajGraphicsSetCharscale
*/

__deprecated float ajGraphSetCharScale(float scale)
{
    return ajGraphicsSetCharscale(scale);
}


/* @func ajGraphicsSetCharsize *************************************************
**
** Set the character size factor
**
** @param [r] size [float]  character size.
** @return [float] the previous character size factor.
** @@
******************************************************************************/

float ajGraphicsSetCharsize(float size)
{
    float oldsize;

    oldsize = currentcharsize;

    if(size)
    {
	currentcharsize = size;
        currentcharscale = 1.0;
	plschr(currentcharsize, currentcharscale);
    }

    return oldsize;
}


/* @obsolete ajGraphSetCharSize
** @rename ajGraphicsSetCharsize
*/

__deprecated float ajGraphSetCharSize(float size)
{
    return ajGraphicsSetCharsize(size);
}

/* @func ajGraphicsSetDefcharsize **********************************************
**
** Set the default character size in mm.
**
** @param [r] size [float]  character size in mm.
** @return [float] the previous character size in mm.
** @@
******************************************************************************/

float ajGraphicsSetDefcharsize(float size)
{
    float oldsize;
    float oldscale;

    ajGraphicsGetCharsize(&oldsize, &oldscale);
    GraphDefCharSize((PLFLT)size);

    return oldsize;
}


/* @obsolete ajGraphSetDefCharSize
** @rename ajGraphicsSetDefcharsize
*/

__deprecated float ajGraphSetDefCharSize(float size)
{
    return ajGraphicsSetDefcharsize(size);
}


/* @datasection [AjPGraph] Graph object ****************************************
**
** Function is for manipulating an AjPGraph object
**
** @nam2rule Graph
*/


/* @func ajGraphSetDevice **************************************************
**
** Set graph device
**
** @param [r] thys [const AjPGraph] Graph object with displaytype set
**
** @return [void]
** @@
******************************************************************************/

void ajGraphSetDevice(const AjPGraph thys)
{
    ajDebug("=g= plsdev ('%s') [graphType[%d].Device] ready: %B\n",
            graphType[thys->displaytype].Device,
            thys->displaytype, thys->ready);

    if(!thys->ready)
        plsdev(graphType[thys->displaytype].Device);

    return;
}




/* @func ajGraphSetFillPat ****************************************************
**
** Set the Fill Pattern type.
**
** @param [r] style [ajint]  line style to set drawing to.
** @return [ajint] the previous line style.
** @@
******************************************************************************/

ajint ajGraphSetFillPat(ajint style)
{
    ajint oldstyle;

    if(style < 0 || style > 8)
	style = 1;

    oldstyle = currentfillpattern;
    currentfillpattern = style;
    GraphFillPat((PLINT)style);

    return oldstyle;
}




/* @func ajGraphSetLineStyle **************************************************
**
** Set the line style.
**
** @param [r] style [ajint]  line style to set drawing to.
** @return [ajint] the previous line style.
** @@
******************************************************************************/

ajint ajGraphSetLineStyle(ajint style)
{
    ajint oldstyle;

    if(style < 1 || style > 8)
	style = 1;

    oldstyle = currentlinestyle;
    currentlinestyle = style;
    GraphLineStyle((PLINT)style);

    return oldstyle;
}




/* @func ajGraphicsSetPortrait *************************************************
**
** Set graph orientation
**
** @param [r] set [AjBool] portrait orientation if true, else landscape
**
** @return [void]
**@@
******************************************************************************/

void ajGraphicsSetPortrait(AjBool set)
{
    ajDebug("=g= plsori(%d) [ori]\n", set);

    if(set)
        plsori(1);
    else
        plsori(0);

    return;
}

/* @obsolete ajGraphSetOri
** @rename ajGraphicsSetPortrait
*/

__deprecated void ajGraphSetOri(ajint ori)
{
    ajGraphicsSetPortrait(ori);
    return;
}


/* @func ajGraphSetPenWidth ***************************************************
**
** Set the current pen width.
**
** @param [r] width [float] width for the pen.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetPenWidth(float width)
{
    ajDebug("=g= plwid(%.2f) [width]\n", width);
    plwid((PLINT)width);

    return;
}


/* @funcstatic GraphDefCharSize ***********************************************
**
** Set the default char size in mm.
**
** @param [r] size [float] Character size in mm. See PLPLOT.
**
** @return [void]
** @@
******************************************************************************/

static void GraphDefCharSize(float size)
{
    currentcharscale = 1.0;
    currentcharsize = size;
    plschr(currentcharsize, currentcharscale);

    return;
}




/* @funcstatic GraphTextLength ************************************************
**
** Compute the length of a string in user coordinates.
**
** @param [r] xx1 [float] Start of text box on x axis
** @param [r] yy1 [float] Start of text box on y axis
** @param [r] xx2 [float] End of text box on x axis
** @param [r] yy2 [float] End of text box on y axis
** @param [r] text [const char*] Text
**
** @return [float] The length of the string in user coordinates
** @@
******************************************************************************/

static float GraphTextLength(float xx1, float yy1, float xx2, float yy2,
			     const char *text)
{
    return plstrlW(xx2-xx1, yy2-yy1, text);
}




/* @funcstatic GraphTextHeight ************************************************
**
** Compute the height of a character in user coordinates.
**
** @param [r] xx1 [float] Start of text box on x axis
** @param [r] xx2 [float] End of text box on x axis
** @param [r] yy1 [float] Start of text box on y axis
** @param [r] yy2 [float] End of text box on y axis
**
** @return [float] The height of the character in user coordinates
** @@
******************************************************************************/

static float GraphTextHeight(float xx1, float xx2, float yy1, float yy2)
{
    return plgchrW(xx2-xx1, yy2-yy1);
}




/* @funcstatic GraphFillPat ***************************************************
**
** Set the pattern to fill with.
**
** @param [r] pat [ajint] Pattern code. See PLPLOT.
**
** @return [void]
** @@
******************************************************************************/

static void GraphFillPat(ajint pat)
{
    ajDebug("=g= plpsty(%d) [pattern style]\n", pat);
    plpsty((PLINT)pat);

    return;
}




/* @funcstatic GraphLineStyle *************************************************
**
** Set the Line style. i.e. dots dashes unbroken.
**
** @param [r] style [ajint] Line style. See PLPLOT.
**
** @return [void]
** @@
******************************************************************************/

static void GraphLineStyle(ajint style)
{
    ajDebug("=g= pllsty(%d) [line style]\n", style);
    pllsty((PLINT)style);

    return;
}




/* @funcstatic GraphCharScale *************************************************
**
** Set the char size.
**
** @param [r] scale [float] Character scale factor. See PLPLOT.
**
** @return [void]
** @@
******************************************************************************/

static void GraphCharScale(float scale)
{
    ajDebug("=g= plschr(0.0, %.2f) [0.0 charscale]\n", scale);
    plschr(0.0,(PLFLT)scale);		/* use current size */

    return;
}





/* @func ajGraphSetPage *******************************************************
**
** Set the width and height of the plot page.
**
** Ignored by most plplot drivers, but wil be used by PNG
**
** @param [r] width [ajuint] Page width in pixels
** @param [r] height [ajuint] Page height in pixels
**
******************************************************************************/

void ajGraphSetPage(ajuint width, ajuint height)
{
    plspage(0.0, 0.0, width, height, 0, 0);

    return;
}




/* @funcstatic GraphSetPen ****************************************************
**
** Set the pen to the colour specified.
**
** @param [r] colour [ajint] Pen colour (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

static void GraphSetPen(ajint colour)
{
    ajDebug("=g= plcol(%d '%s') [colour]\n", colour, graphColourName[colour]);
    plcol((PLINT)colour);

    return;
}



/* @funcstatic GraphSetNumSubPage *********************************************
**
** Sets the number of sub pages for a page.
**
** @param [r] numofsubpages [ajint] Number of subpages
**
** @return [void]
** @@
******************************************************************************/

static void GraphSetNumSubPage(ajint numofsubpages)
{
    ajDebug("=g= plssub (1, %d) [numofsubpages]\n", numofsubpages);
    plssub(1,numofsubpages);

    return;
}




/* @func ajGraphSet ***********************************************************
**
** Initialize options for a graph object
**
** @param [u] thys [AjPGraph] Graph object
** @param [r] type [const AjPStr] Graph type
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajGraphSet(AjPGraph thys, const AjPStr type)
{
    ajint i;
    ajint j=-1;
    ajint k;
    AjPStr aliases = NULL;

    ajDebug("ajGraphSet '%S'\n", type);

    for(i=0;graphType[i].Name;i++)
    {
        if(!graphType[i].GOpen)
            continue;

        if(ajStrMatchCaseC(type, graphType[i].Name))
	{
	    j = i;
	    break;
	}

	if(ajCharPrefixCaseS(graphType[i].Name, type))
	{
	    if(j < 0)
		j = i;
	    else
	    {
		for(k=0;graphType[k].Name;k++)
		{
		    if(ajCharPrefixCaseS(graphType[k].Name, type))
		    {
			if(ajStrGetLen(aliases))
			    ajStrAppendC(&aliases, ", ");
			ajStrAppendC(&aliases, graphType[k].Name);
		    }
		}
		    
		ajErr("Ambiguous graph device name '%S' (%S)",
		       type, aliases);
		ajStrDel(&aliases);

		return ajFalse;
	    }
	}
    }

    if(j<0)
	return ajFalse;

    GraphNewPlplot(thys);

    thys->displaytype = j;

    ajDebug("ajGraphSet plplot type '%S' displaytype %d '%s'\n",
	    type, j, graphType[j].Name);

    return ajTrue;
}



/* @func ajGraphSetDescS *******************************************************
**
** Set the graph description.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const AjPStr]  Title.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetDescS(AjPGraph thys, const AjPStr title)
{
    ajDebug("ajGraphSetDesc '%S'\n", title);

    ajStrAssignS(&thys->desc,title);

    return;
}


/* @obsolete ajGraphSetDesc
** @rename ajGraphSetDescS
*/

__deprecated void ajGraphSetDesc(AjPGraph thys, const AjPStr title)
{
    ajGraphSetDescS(thys, title);

    return;
}



/* @func ajGraphSetFlag *****************************************************
**
** Set the flags for the graph to add or subract "flag" depending on istrue
**
** @param [u] thys [AjPGraph] graph to have flags altered.
** @param [r] flag   [ajint]      flag to be set.
** @param [r] istrue [AjBool]   whether to set the flag or remove it.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetFlag(AjPGraph thys, ajint flag, AjBool istrue)
{
    if (!thys)
	return;

    if(thys->flags & flag)
    {
	if(!istrue)
	    thys->flags -= flag;
    }
    else
    {
	if(istrue)
	    thys->flags += flag;
    }

    return;
}




/* @func ajGraphSetName *******************************************************
**
** set BaseName and extension.
**
** @param [r] thys [const AjPGraph] Graph object.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphSetName(const AjPGraph thys)
{
    if(!ajCharMatchCaseC(graphType[thys->displaytype].Ext, "null"))
        GraphSetName(thys, thys->outputfile,
                     graphType[thys->displaytype].Ext);

    return;
}



/* @funcstatic GraphSetName ***************************************************
**
** Set BaseName and extension.
**
** @param [r] thys [const AjPGraph] Graph object.
** @param [r] txt [const AjPStr] base name for files
** @param [r] ext [const char*] extension for files
**
** @return [void]
** @@
******************************************************************************/

static void GraphSetName(const AjPGraph thys,
			 const AjPStr txt, const char *ext)
{
    if(!thys->ready)
    {
        ajDebug("=g= plxsfnam ('%S', '%s')\n", txt, ext);
        plxsfnam(ajStrGetPtr(txt), ext);
    }

    return;
}




/* @func ajGraphSetOutputDirS **************************************************
**
** Set the directory of the output file. Only used later if the device
** plotter is capable of postscript output. ps and cps.
**
** @param [u] thys [AjPGraph] Graph structure to write file name too.
** @param [r] txt [const AjPStr] Name of the file.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetOutputDirS(AjPGraph thys, const AjPStr txt)
{
    if(!ajStrGetLen(txt))
	return;

    if (!thys)
	return;

    if(ajStrGetCharLast(txt) != '/')
	ajStrInsertC(&thys->outputfile, 0, "/");
    ajStrInsertS(&thys->outputfile, 0, txt);

    return;
}


/* @obsolete ajGraphSetDir
** @rename ajGraphSetOutputDirS
*/

__deprecated void ajGraphSetDir(AjPGraph thys, const AjPStr txt)
{
    ajGraphSetOutputDirS(thys, txt);
    return;
}


/* @func ajGraphSetOutC *****************************************************
**
** Set the name of the output file. Only used later if the device
** plotter is capable of postscript output. ps and cps.
**
** @param [u] thys [AjPGraph] Graph structure to write file name to.
** @param [r] txt [const char*] Name of the file.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetOutC(AjPGraph thys, const char* txt)
{
    if(!strlen(txt))
	return;

    if (!thys)
	return;

    ajStrAssignC(&thys->outputfile, txt);

    return;
}




/* @func ajGraphSetOut ******************************************************
**
** Set the name of the output file. Only used later if the device
** plotter is capable of postscript output. ps and cps.
**
** @param [u] thys [AjPGraph] Graph structure to write file name too.
** @param [r] txt [const AjPStr] Name of the file.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetOut(AjPGraph thys, const AjPStr txt)
{
    if(!ajStrGetLen(txt))
	return;

    if (!thys)
	return;

    ajStrAssignS(&thys->outputfile, txt);

    return;
}




/* @func ajGraphSetShowSubtitle ***********************************************
**
** Set whether the graph is to display the subtitle.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetShowSubtitle(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_SUBTITLE, set);

    return;
}



/* @obsolete ajGraphSetSubTitleDo
** @rename ajGraphSetShowSubtitle
*/

__deprecated void ajGraphSetSubTitleDo(AjPGraph thys, AjBool set)
{
    ajGraphSetShowSubtitle(thys, set);
    return;
}


/* @func ajGraphSetShowTitle **************************************************
**
** Set whether the graph is to display the title.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetShowTitle(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_TITLE, set);

    return;
}


/* @obsolete ajGraphSetTitleDo
** @rename ajGraphSetShowTitle
*/

__deprecated void ajGraphSetTitleDo(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_TITLE, set);

    return;
}


/* @func ajGraphSetSubtitleC **************************************************
**
** Set the graph subtitle.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const char*]  subtitle.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetSubtitleC(AjPGraph thys, const char* title)
{
    ajStrAssignC(&thys->subtitle,title);

    return;
}


/* @obsolete ajGraphSetSubTitleC
** @rename ajGraphSetSubtitleC
*/

__deprecated void ajGraphSetSubTitleC(AjPGraph thys, const char* title)
{
    ajGraphSetSubtitleC(thys, title);
    return;
}


/* @func ajGraphSetSubtitleS ***************************************************
**
**  Set the title for the Y axis.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const AjPStr] Subtitle
** @return [void]
** @@
******************************************************************************/

void ajGraphSetSubtitleS(AjPGraph thys, const AjPStr title)
{
    ajStrAssignS(&thys->subtitle,title);

    return;
}


/* @obsolete ajGraphSetSubTitle
** @rename ajGraphSetSubtitleS
*/

__deprecated void ajGraphSetSubTitle(AjPGraph thys, const AjPStr title)
{
    ajGraphSetSubtitleS(thys, title);
    return;
}


/* @func ajGraphSetTitleC ***************************************************
**
** Set the graph Title.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const char*]  title.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetTitleC(AjPGraph thys, const char* title)
{
    ajDebug("ajGraphSetTitleC '%s'\n", title);

    ajStrAssignC(&thys->title,title);

    return;
}




/* @func ajGraphSetTitleS ******************************************************
**
** Set the graph title.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const AjPStr]  Title.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetTitleS(AjPGraph thys, const AjPStr title)
{
    ajDebug("ajGraphSetTitleS '%S'\n", title);

    ajStrAssignS(&thys->title,title);

    return;
}


/* @obsolete ajGraphSetTitle
** @rename ajGraphSetTitleS
*/

__deprecated void ajGraphSetTitle(AjPGraph thys, const AjPStr title)
{
    ajGraphSetTitleS(thys, title);
    return;
}


/* @func ajGraphSetTitlePlus **************************************************
**
** Set an empty graph title from the description or program name and extra text
** 'of ...' supplied by the caller.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const AjPStr]  Title.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetTitlePlus(AjPGraph thys, const AjPStr title)
{
    ajDebug("ajGraphSetTitlePlus '%S'\n", title);

    if (!thys)
	return;

    if(ajStrGetLen(thys->title))
	return;

    if(ajStrGetLen(thys->desc))
	ajFmtPrintS(&thys->title, "%S of %S",
                    thys->desc, title);
    else
	ajFmtPrintS(&thys->title, "%S of %S",
                    ajUtilGetProgram(), title);

    return;
}




/* @func ajGraphSetXlabelC ****************************************************
**
** Set the title for the X axis for multiple plot on one graph.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const char*] title for the x axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetXlabelC(AjPGraph thys, const char* title)
{
    ajStrAssignC(&thys->xaxis,title);

    return;
}




/* @obsolete ajGraphSetXTitleC
** @rename ajGraphSetXlabelC
*/

__deprecated void ajGraphSetXTitleC(AjPGraph thys, const char* title)
{
    ajGraphSetXlabelC(thys, title);
    return;
}


/* @func ajGraphSetXlabelS *****************************************************
**
** Set the title for the X axis for multiple plots on one graph.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] title [const AjPStr] title for the x axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetXlabelS(AjPGraph thys, const AjPStr title)
{
    ajStrAssignS(&thys->xaxis,title);

    return;
}



/* @obsolete ajGraphSetXTitle
** @rename ajGraphSetXlabelS
*/

__deprecated void ajGraphSetXTitle(AjPGraph thys, const AjPStr title)
{
    ajGraphSetXlabelS(thys, title);
    return;
}


/* @func ajGraphSetYlabelC ****************************************************
**
** Set the title for the Y axis for multiple plots on one graph.
**
** @param [u] thys  [AjPGraph] Graph structure to store info in.
** @param [r] title [const char*] title for the y axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetYlabelC(AjPGraph thys, const char* title)
{
    ajStrAssignC(&thys->yaxis, title);

    return;
}


/* @obsolete ajGraphSetYTitleC
** @rename ajGraphSetYlabelC
*/

__deprecated void ajGraphSetYTitleC(AjPGraph thys, const char* title)
{
    ajGraphSetYlabelC(thys, title);
    return;
}


/* @func ajGraphSetYlabelS *****************************************************
**
** Set the title for the Y axis for multiple plots on one graph.
**
** @param [u] thys  [AjPGraph] Graph structure to store info in.
** @param [r] title [const AjPStr] title for the y axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetYlabelS(AjPGraph thys, const AjPStr title)
{
    ajStrAssignS(&thys->yaxis, title);

    return;
}



/* @obsolete ajGraphSetYTitle
** @rename ajGraphSetYlabelS
*/

__deprecated void ajGraphSetYTitle(AjPGraph thys, const AjPStr title)
{
    ajGraphSetYlabelS(thys, title);
    return;
}



/* @func ajGraphxySet *********************************************************
**
** Initialize options for a graphxy object
**
** @param [u] thys [AjPGraph] Graph object
** @param [r] type [const AjPStr] Graph type
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajGraphxySet(AjPGraph thys, const AjPStr type)
{
    ajint i;
    ajint j=-1;
    ajint k;
    AjPStr aliases = NULL;

    ajDebug("ajGraphxySet '%S'\n", type);

    for(i=0;graphType[i].Name;i++)
    {
        if(!graphType[i].XYDisplay)
            continue;

        if(ajStrMatchCaseC(type, graphType[i].Name))
	{
	    j = i;
	    break;
	}

	if(ajCharPrefixCaseS(graphType[i].Name, type))
	{
	    if(j < 0)
		j = i;
	    else
	    {
		for(k=0;graphType[k].Name;k++)
		{
		    if(ajCharPrefixCaseS(graphType[k].Name, type))
		    {
			if(ajStrGetLen(aliases))
			    ajStrAppendC(&aliases, ", ");
			ajStrAppendC(&aliases, graphType[k].Name);
		    }
		}
		    
		ajErr("Ambiguous graph device name '%S' (%S)",
		       type, aliases);
		ajStrDel(&aliases);

		return ajFalse;
	    }
	}
    }

    if(j<0)
	return ajFalse;

    GraphxyNewPlplot(thys);

    /* Calling funclist graphType() */

    thys->displaytype = j;

    ajDebug("ajGraphxySet type '%S' displaytype %d '%s'\n",
	    type, j, graphType[j].Name);

    return ajTrue;

}




/* @func ajGraphxySetGaps *****************************************************
**
** Set whether the graphs should enable gaps.
**
** @param [u] thys [AjPGraph] Multiple graph object
** @param [r] overlap [AjBool] if true allowgaps else do not.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetGaps(AjPGraph thys, AjBool overlap)
{
    if (!thys)
	return;

    if(thys->flags & AJGRAPH_GAPS)
    {
	if(!overlap)
	    thys->flags -= AJGRAPH_GAPS;
    }
    else
    {
	if(overlap)
	    thys->flags += AJGRAPH_GAPS;
    }

    return;
}




/* @func ajGraphxySetMaxMin ***************************************************
**
** Set the max and min of the data points for all graphs.
**
** @param [u] thys [AjPGraph] multiple graph structure.
** @param [r] xmin [float]  x min.
** @param [r] xmax [float]  x max.
** @param [r] ymin [float]  y min.
** @param [r] ymax [float]  y max.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetMaxMin(AjPGraph thys,float xmin,float xmax,
			float ymin,float ymax)
{
    AjPGraphdata graphdata;
    ajint i;

    if (!thys)
	return;

    thys->minX = xmin;
    thys->minY = ymin;
    thys->maxX = xmax;
    thys->maxY = ymax;

    for(i = 0 ; i < thys->numofgraphs ; i++)
    {
	graphdata = (thys->graphs)[i];
	graphdata->minX = xmin;
	graphdata->minY = ymin;
	graphdata->maxX = xmax;
	graphdata->maxY = ymax;
    }

    return;
}




/* @func ajGraphxySetOverLap **************************************************
**
** Set whether the graphs should lay on top of each other.
**
** @param [u] thys [AjPGraph] Multiple graph object
** @param [r] overlap [AjBool] if true overlap else do not.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetOverLap(AjPGraph thys, AjBool overlap)
{
    if (!thys)
	return;

    if(thys->flags & AJGRAPH_OVERLAP)
    {
	if(!overlap)
	    thys->flags -= AJGRAPH_OVERLAP;
    }
    else
    {
	if(overlap)
	    thys->flags += AJGRAPH_OVERLAP;
    }

    return;
}




/* @func ajGraphxySetShowXaxis *************************************************
**
** Set whether the graph is to display a bottom x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowXaxis(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_BOTTOM, set);

    return;
}


/* @obsolete ajGraphxySetXBottom
** @rename ajGraphxySetShowXaxis
*/

__deprecated void ajGraphxySetXBottom(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowXaxis(thys,set);
    return;
}


/* @func ajGraphxySetXgrid ****************************************************
**
** Set whether the graph is to grid the tick marks on the x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetXgrid(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_GRID, set);

    return;
}



/* @obsolete ajGraphxySetXGrid
** @rename ajGraphxySetXgrid
*/

__deprecated void ajGraphxySetXGrid(AjPGraph thys, AjBool set)
{
    ajGraphxySetXgrid(thys, set);
    return;
}

/* @func ajGraphxySetShowXlabel ************************************************
**
** Set whether the graph is to label the x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowXlabel(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_LABEL, set);

    return;
}


/* @obsolete ajGraphxySetXLabel
** @rename 
    ajGraphxySetShowXlabel
*/

__deprecated void ajGraphxySetXLabel(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowXlabel(thys, set);
    return;
}


/* @func ajGraphxySetCirclePoints *********************************************
**
** Set the graph to draw circles at the points.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetCirclePoints(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_CIRCLEPOINTS, set);

    return;
}




/* @func ajGraphxySetJoinPoints ***********************************************
**
** Set the graph to draw lines between the points.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetJoinPoints(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_JOINPOINTS, set);

    return;
}




/* @func ajGraphxySetXinvert ***************************************************
**
** Set whether the graph is to display the tick marks inside the plot on
** the x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetXinvert(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_INVERT_TICK, set);

    return;
}


/* @obsolete ajGraphxySetXInvTicks
** @rename ajGraphxySetXinvert
*/

__deprecated void ajGraphxySetXInvTicks(AjPGraph thys, AjBool set)
{
    ajGraphxySetXinvert(thys, set);
    return;
}


/* @func ajGraphxySetShowUnum **************************************************
**
** Set whether the graph is to display the labels on the upper x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowUnum(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_NUMLABEL_ABOVE, set);

    return;
}


/* @obsolete ajGraphxySetXLabelTop
** @rename ajGraphxySetShowUnum
*/

__deprecated void ajGraphxySetXLabelTop(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowUnum(thys, set);
    return;
}


/* @func ajGraphxySetXrangeII *************************************************
**
** Sets the X axis range with integers
**
** @param [u] thys [AjPGraph] Graph
** @param [r] start [ajint] start position
** @param [r] end [ajint] end position
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetXrangeII(AjPGraph thys, ajint start, ajint end)
{
    if (thys)
    {
	thys->xstart = (float) start;
	thys->xend = (float) end;
    }

    return;
}



/* @obsolete ajGraphxySetXRangeII
** @rename ajGraphxySetXrangeII
*/

__deprecated void ajGraphxySetXRangeII(AjPGraph thys, ajint start, ajint end)
{
    ajGraphxySetXrangeII(thys, start, end);

    return;
}


/* @func ajGraphxySetShowXtick *************************************************
**
** Set whether the graph is to display tick marks on the x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowXtick(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_TICK, set);

    return;
}



/* @obsolete ajGraphxySetXTick
** @rename ajGraphxySetShowXtick
*/

__deprecated void ajGraphxySetXTick(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowXtick(thys, set);
    return;
}
    
/* @func ajGraphxySetShowUaxis *************************************************
**
** Set whether the graph is to display the left X axis at the top.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowUaxis(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_X_TOP, set);

    return;
}



/* @obsolete ajGraphxySetXTop
** @rename ajGraphxySetShowTaxis
*/

__deprecated void ajGraphxySetXTop(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowUaxis(thys,set);
    return;
}

/* @func ajGraphxySetYgrid ****************************************************
**
** Set whether the graph is to grid the tick marks on the x axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetYgrid(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_GRID, set);

    return;
}


/* @obsolete ajGraphxySetYGrid
** @rename ajGraphxySetYgrid
*/

__deprecated void ajGraphxySetYGrid(AjPGraph thys, AjBool set)
{
    ajGraphxySetYgrid(thys, set);
    return;
}


/* @func ajGraphxySetYinvert ***************************************************
**
** Set whether the graph is to display the tick marks inside the plot
** on the y axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetYinvert(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_INVERT_TICK, set);

    return;
}


/* @obsolete ajGraphxySetYInvTicks
** @rename ajGraphxySetYinvert
*/

__deprecated void ajGraphxySetYInvTicks(AjPGraph thys, AjBool set)
{
    ajGraphxySetYinvert(thys,set);
    return;
}


/* @func ajGraphxySetShowYlabel ***********************************************
**
** Set whether the graph is to label the y axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowYlabel(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_LABEL, set);

    return;
}



/* @obsolete ajGraphxySetYLabel
** @rename ajGraphxySetShowYlabel
*/

__deprecated void ajGraphxySetYLabel(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowYlabel(thys, set);
    return;
}


/* @func ajGraphxySetShowYnum *************************************************
**
** Set whether the graph is to display the numbers on the left Y axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowYnum(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_NUMLABEL_LEFT, set);

    return;
}


/* @obsolete ajGraphxySetYLabelLeft
** @rename ajGraphxySetShowYnum
*/

__deprecated void ajGraphxySetYLabelLeft(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowYnum(thys, set);
    return;
}


/* @func ajGraphxySetShowYaxis *************************************************
**
** Set whether the graph is to display the left Y axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowYaxis(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_LEFT, set);

    return;
}

void ajGraphxySetYLeft(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowYaxis(thys, set);
    return;
}



/* @func ajGraphxySetYrangeII *************************************************
**
** Sets the Y axis range with integers
**
** @param [u] thys [AjPGraph] Graph
** @param [r] start [ajint] start position
** @param [r] end [ajint] end position
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetYrangeII(AjPGraph thys, ajint start, ajint end)
{
    if (thys)
    {
	thys->ystart = (float) start;
	thys->yend = (float) end;
    }

    return;
}



/* @obsolete ajGraphxySetYRangeII
** @rename ajGraphxySetYrangeII
*/

__deprecated void ajGraphxySetYRangeII(AjPGraph thys, ajint start, ajint end)

{
    ajGraphxySetYrangeII(thys, start, end);
    return;
}


/* @func ajGraphxySetShowRaxis *************************************************
**
** Set the graph is to display a right hand Y axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowRaxis(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_RIGHT, set);

    return;
}


/* @obsolete ajGraphxySetYRight
** @rename ajGraphxySetShowRaxis
*/

__deprecated void ajGraphxySetYRight(AjPGraph thys, AjBool set)
{
    ajGraphxySetShowRaxis(thys, set);
    return;
}


/* @func ajGraphxySetShowYtick *************************************************
**
** Set the graph is to display tick marks on the y axis.
**
** @param [u] thys [AjPGraph] graph to have flag altered
** @param [r]  set    [AjBool]   whether to set or turn off.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetShowYtick(AjPGraph thys, AjBool set)
{
    ajGraphSetFlag(thys, AJGRAPH_Y_TICK, set);

    return;
}


/* @obsolete ajGraphxySetYTick
** @rename ajGrapxyhSetShowYtick
*/

__deprecated void ajGraphxySetYTick(AjPGraph thys, AjBool set)
{

    ajGraphxySetShowYtick(thys, set);
    return;
}


/* @datasection [AjPGraphdata] Graph dataobject ********************************
**
** Function is for manipulating an AjPGraphdata object
**
** @nam2rule Graphdata
*/


/* @func ajGraphdataSetColour **********************************************
**
** Set the colour for the plot on one graph.
**
** @param [u] graphdata [AjPGraphdata] Graph structure to store info in.
** @param [r] colour [ajint] colour for this plot.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetColour(AjPGraphdata graphdata, ajint colour)
{
    graphdata->colour = colour;

    return;
}




/* @func ajGraphdataSetLineType ********************************************
**
** Set the line type for this graph.
**
** @param [u] graphdata [AjPGraphdata] Graph structure to store info in.
** @param [r] type [ajint] Set the line type.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetLineType(AjPGraphdata graphdata, ajint type)
{
    graphdata->lineType = type;

    return;
}


/* @datasection [AjPGraph] Graph object ****************************************
**
** Function is for manipulating an AjPGraph object for an XY graph
**
** @nam2rule Graphxy
*/



/* @func ajGraphxySetXendF *****************************************************
**
** Set the end position for X in the graph.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] val  [float]  The end value for x graph coord
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetXendF(AjPGraph thys, float val)
{
    if (thys)
    {
	thys->xend = val;
    }

    return;
}


/* @obsolete ajGraphxySetXEnd
** @rename ajGraphxySetXendF
*/

__deprecated void ajGraphxySetXEnd(AjPGraph thys, float val)
{
    ajGraphxySetXendF(thys, val);
    return;
}


/* @func ajGraphxySetXstartF ***************************************************
**
** Set the start position for X in the graph.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] val  [float] The start value for x graph coord.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetXstartF(AjPGraph thys, float val)
{
    if (thys)
    {
	thys->xstart = val;
    }

    return;
}



/* @obsolete ajGraphxySetXStart
** @rename ajGraphxySetXstartF
*/

__deprecated void ajGraphxySetXStart(AjPGraph thys, float val)
{
    ajGraphxySetXstartF(thys, val);
    return;
}


/* @func ajGraphxySetYendF *****************************************************
**
** Set the end position for Y in the graph.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] val  [float] The start value for y graph coord.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetYendF(AjPGraph thys, float val)
{
    if (thys)
    {
	thys->yend = val;
    }

    return;
}


/* @obsolete ajGraphxySetYEnd
** @rename ajGraphxySetYendF
*/

__deprecated void ajGraphxySetYEnd(AjPGraph thys, float val)
{
    ajGraphxySetYendF(thys, val);
    return;
}


/* @func ajGraphxySetYstartF ***************************************************
**
** Set the start position for Y in the graph.
**
** @param [u] thys [AjPGraph] Graph structure to store info in.
** @param [r] val  [float] The end value for y graph coord.
** @return [void]
** @@
******************************************************************************/

void ajGraphxySetYstartF(AjPGraph thys, float val)
{
    if (thys)
    {
	thys->ystart = val;
    }

    return;
}


/* @obsolete ajGraphxySetYStart
** @rename ajGraphxySetYstartF
*/

__deprecated void ajGraphxySetYStart(AjPGraph thys, float val)
{
    ajGraphxySetYstartF(thys, val);
    return;
}




/* @func ajGraphdataSetMaxMin **********************************************
**
** Set the max and min of the data points you wish to display.
**
** @param [u] graphdata [AjPGraphdata] multiple graph structure.
** @param [r] xmin [float]  x min.
** @param [r] xmax [float]  x max.
** @param [r] ymin [float]  y min.
** @param [r] ymax [float]  y max.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetMaxMin(AjPGraphdata graphdata, float xmin, float xmax,
			    float ymin, float ymax)
{
    graphdata->minX = xmin;
    graphdata->minY = ymin;
    graphdata->maxX = xmax;
    graphdata->maxY = ymax;

    return;
}




/* @func ajGraphdataSetMaxima **********************************************
**
** Set the scale max and min of the data points you wish to display.
**
** @param [u] graphdata [AjPGraphdata] multiple graph structure.
** @param [r] xmin [float]  true x min.
** @param [r] xmax [float]  true x max.
** @param [r] ymin [float]  true y min.
** @param [r] ymax [float]  true y max.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetMaxima(AjPGraphdata graphdata, float xmin, float xmax,
			    float ymin, float ymax)
{
    graphdata->tminX = xmin;
    graphdata->tminY = ymin;
    graphdata->tmaxX = xmax;
    graphdata->tmaxY = ymax;

    return;
}




/* @func ajGraphdataSetXTitleC *********************************************
**
** Set the title for the X axis.
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const char*] title for the X axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetXTitleC(AjPGraphdata graphdata, const char* title)
{
    ajStrAssignEmptyC(&graphdata->xaxis,title);

    return;
}




/* @func ajGraphdataSetTypeC ***********************************************
**
** Set the type of the graph for data output.
**
** @param [u] graphdata [AjPGraphdata] multiple graph structure.
** @param [r] type [const char*]  Type e.g. "2D Plot", "Histogram".
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetTypeC(AjPGraphdata graphdata,const  char* type)
{
    ajStrAssignC(&graphdata->gtype,type);

    return;
}




/* @func ajGraphdataSetXTitle **********************************************
**
** Set the title for the X axis.
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const AjPStr] title for the X axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetXTitle(AjPGraphdata graphdata, const AjPStr title)
{
    ajStrAssignEmptyS(&graphdata->xaxis,title);

    return;
}




/* @func ajGraphdataSetYTitleC *********************************************
**
** Set the title for the Y axis.
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const char*] title for the Y axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetYTitleC(AjPGraphdata graphdata, const char* title)
{
    ajStrAssignEmptyC(&graphdata->yaxis,title);

    return;
}




/* @func ajGraphdataSetYTitle **********************************************
**
** Set the title for the Y axis.
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const AjPStr] title for the Y axis.
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetYTitle(AjPGraphdata graphdata, const AjPStr title)
{
    ajStrAssignEmptyS(&graphdata->yaxis,title);

    return;
}




/* @funcstatic GraphInit ******************************************************
**
** calls plinit.
**
** @param [w] thys [AjPGraph] Graph object.
** @return [void]
** @@
******************************************************************************/

static void GraphInit(AjPGraph thys)
{
    float fold;

    if (thys)
    {
	if(!thys->ready)
	{
	    ajDebug("=g= plinit ()\n");
	    plinit();
	}

	thys->ready = ajTrue;
    }

    fold = ajGraphicsSetCharscale(0.0);
    fold = ajGraphicsSetCharscale(fold);

    return;
}




/* @funcstatic GraphSubPage ***************************************************
**
** start new graph page.
**
** @param [r] page [ajint] Page number
**
** @return [void]
** @@
******************************************************************************/

static void GraphSubPage(ajint page)
{
    ajDebug("=g= pladv (%d) [page]\n", page);
    pladv(page);

    return;
}




/* @funcstatic GraphWind ******************************************************
**
** Calls plwind.
**
** @param [r] xmin [float] Minimum x axis value
** @param [r] xmax [float] Maximum x axis value
** @param [r] ymin [float] Minimum y axis value
** @param [r] ymax [float] Maximum y axis value
**
** @return [void]
** @@
******************************************************************************/

static void GraphWind(float xmin, float xmax, float ymin, float ymax)
{
    ajDebug("=g= plwind (%.2f, %.2f, %.2f, %.2f) [xmin/max ymin/max]\n",
	     xmin, xmax, ymin, ymax);
    plwind(xmin, xmax, ymin, ymax);

    return;
}




/* @funcstatic GraphSetWin ****************************************************
**
** Creates a window using the ranges.
**
** @param [r] xmin [float] Minimum x axis value
** @param [r] xmax [float] Maximum x axis value
** @param [r] ymin [float] Minimum y axis value
** @param [r] ymax [float] Maximum y axis value
**
** @return [void]
** @@
******************************************************************************/

static void GraphSetWin(float xmin, float xmax, float ymin, float ymax)
{
    ajDebug("=g= plvpor (0.0,1.0,0.0,1.0) [whole screen]\n");
    ajDebug("GraphSetWin(%.3f, %.3f, %.3f, %.3f)\n",
	   xmin, xmax, ymin, ymax);

    if(graphData)
    {
	graphData->xmin = xmin;
	graphData->ymin = ymin;
	graphData->xmax = xmax;
	graphData->ymax = ymax;

	if(graphData->Num == 1)	/* else GraphDatafileNext does it */
	{
	    ajFmtPrintF(graphData->File,
			"##Graphic\n##Screen x1 %f y1 %f x2 %f y2 %f\n",
			graphData->xmin, graphData->ymin,
			graphData->xmax, graphData->ymax);
	    graphData->Lines++;
	    graphData->Lines++;
	}
    }
    else
    {
	/*
	** Use the whole screen. User may add boundaries by
	** modifying xmin, xmax et
	*/
	plvpor(0.0,1.0,0.0,1.0);
	GraphWind(xmin, xmax, ymin, ymax);
    }

    return;
}




/* @funcstatic GraphSetWin2 ***************************************************
**
** Creates a window using the ranges.
**
** @param [r] xmin [float] Minimum x axis value
** @param [r] xmax [float] Maximum x axis value
** @param [r] ymin [float] Minimum y axis value
** @param [r] ymax [float] Maximum y axis value
**
** @return [void]
** @@
******************************************************************************/

static void GraphSetWin2(float xmin, float xmax, float ymin, float ymax)
{
    ajDebug("=g= plvsta()\n");
    plvsta();
    /* use the whole screen */
    GraphWind(xmin, xmax, ymin, ymax);

    return;
}




/* @funcstatic GraphArray *****************************************************
**
** Draw lines from the array of x and y values.
**
** @param [r] numofpoints [ajint] Number of data points
** @param [r] x [CONST float*] Array of x axis values
** @param [r] y [CONST float*] Array of y axis values
**
** @return [void]
** @@
******************************************************************************/

static void GraphArray(ajint numofpoints, float *x, float *y)
{
    ajint i;
    float xlast;
    float ylast;

    if(numofpoints)
	ajDebug("=g= plline( %d, %.2f .. %.2f, %.2f .. %.2f) "
		"[num x..x y..y]\n",
		numofpoints, x[0], x[numofpoints-1], y[0], y[numofpoints-1] );
    else
	ajDebug("=g= plline( %d, <> .. <>, <> .. <>) [num x..x y..y]\n",
		numofpoints );

    if(graphData)
    {
	if(numofpoints < 2)
	    return;

	xlast = *x;
	ylast = *y;

	for(i=1; i<numofpoints; i++)
	{
	    ajGraphicsDrawLine(xlast, ylast, x[i], y[i]);
	    xlast = x[i];
	    ylast = y[i];
	}
    }
    else
	plline(numofpoints, x,y);

    return;
}




/* @funcstatic GraphArrayGaps *************************************************
**
** Draw lines for an array of floats with gaps.
** Gaps are declared by having values of FLT_MIN.
**
** @param [r] numofpoints [ajint] Number of data points
** @param [r] x [CONST float*] Array of x axis values
** @param [r] y [CONST float*] Array of y axis values
**
** @return [void]
** @@
******************************************************************************/

static void GraphArrayGaps(ajint numofpoints, float *x, float *y)
{
    ajint i;
    float *xx1,*xx2;
    float *yy1,*yy2;

    xx1 = xx2 = x;
    yy1 = yy2 = y;
    xx2++;
    yy2++;

    for(i=1;i<numofpoints;i++)
    {
	if(*xx2 != FLT_MIN && *xx1 != FLT_MIN &&
	   *yy2 != FLT_MIN && *yy1 != FLT_MIN)
	{
	    /*ajDebug("=g= pljoin(%.2f, %.2f, %.2f, %.2f) [ xy xy]\n",
		    *xx1, *yy1, *xx2, *yy2);*/
            pljoin(*xx1,*yy1,*xx2,*yy2);
	}

	xx1++; yy1++;
	xx2++; yy2++;
    }

    return;
}




/* @funcstatic GraphArrayGapsI ************************************************
**
** Draw lines for an array of integers with gaps.
** Gaps are declared by having values of INT_MIN.
**
** @param [r] numofpoints [ajint] Number of data points
** @param [r] x [CONST ajint*] Array of x axis values
** @param [r] y [CONST ajint*] Array of y axis values
**
** @return [void]
** @@
******************************************************************************/

static void GraphArrayGapsI(ajint numofpoints, ajint *x, ajint *y)
{
    ajint i;
    ajint *xx1,*xx2;
    ajint *yy1,*yy2;

    xx1 = xx2 = x;
    yy1 = yy2 = y;
    xx2++;
    yy2++;

    for(i=0;i<numofpoints-1;i++)
    {
	if(*xx2 != INT_MIN && *xx1 != INT_MIN &&
	   *yy2 != INT_MIN && *yy1 != INT_MIN)
	{
	    /*ajDebug("=g= pljoin(%.2f, %.2f, %.2f, %.2f) "
		    "[ xy xy] [ajint xy xy]\n",
		    (float)*xx1, (float)*xx2, (float)*yy1, (float)*yy2);*/
	    pljoin((float)*xx1,(float)*yy1,(float)*xx2,(float)*yy2);
	}

	xx1++; yy1++;
	xx2++; yy2++;
    }

    return;
}




/* @funcstatic GraphFill ******************************************************
**
** Polygon fill a set of points.
**
** @param [r] numofpoints [ajint] Number of data points
** @param [r] x [CONST float*] Array of x axis values
** @param [r] y [CONST float*] Array of y axis values
**
** @return [void]
** @@
******************************************************************************/

static void GraphFill(ajint numofpoints, float *x, float *y)
{
    if(numofpoints)
	ajDebug("=g= plfill( %d, %.2f .. %.2f, %.2f .. %.2f) "
		"[num x..x y..y]\n",
		numofpoints, x[0], x[numofpoints-1], y[0], y[numofpoints-1] );
    else
	ajDebug("=g= plfill( %d, <> .. <>, <> .. <>) [num x..x y..y]\n",
		numofpoints );

    plfill(numofpoints, x, y);

    return;
}




/* @funcstatic GraphPen *******************************************************
**
** Change the actual colour of a pen.
**
** @param [r] pen [ajint] Pen colour number
** @param [r] red [ajint] Red value (see PLPLOT)
** @param [r] green [ajint] Green value (see PLPLOT)
** @param [r] blue [ajint] Blue value (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

static void GraphPen(ajint pen, ajint red, ajint green, ajint blue)
{
    ajDebug("=g= plscol0(%d, %d, %d, %d) [pen RGB]\n",
	     pen, red, green, blue);
    plscol0(pen,red,green,blue);

    return;
}




/* @funcstatic GraphClose *****************************************************
**
** Close the graph with the plplot plend command.
**
** @return [void]
** @@
******************************************************************************/

static void GraphClose(void)
{
    AjPList files = NULL;
    AjPStr tmpStr = NULL;

    ajDebug("GraphClose\n");

    if(graphData)
	files = graphData->List;
    else
	ajGraphicsGetOutfiles(&files);

    while(ajListstrGetLength(files))
    {
	ajListstrPop(files, &tmpStr);
	ajDebug("GraphInfo file '%S'\n", tmpStr);
	ajFmtPrint("Created %S\n", tmpStr);
	ajStrDel(&tmpStr);
    }

    if(graphData)
    {
	ajDebug("GraphClose deleting graphData '%F' '%S'.'%S'\n",
		graphData->File, graphData->FName, graphData->Ext);
	ajListstrFree(&graphData->List);
	ajFileClose(&graphData->File);
	ajStrDel(&graphData->FName);
	ajStrDel(&graphData->Ext);
	AJFREE(graphData);
	ajDebug("=g= plend()\n");
 	plend();
    }
    else
    {
	ajListstrFree(&files);

	ajDebug("=g= plend()\n");
	plend();
    }

    return;
}




/* @func ajGraphicsSetLabelsC **************************************************
**
** Label current Plot.
**
** @param [r] x [const char*]        text for x axis labelling.
** @param [r] y [const char*]        text for y axis labelling.
** @param [r] title [const char*]    text for title of plot.
** @param [r] subtitle [const char*] text for subtitle of plot.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsSetLabelsC(const char *x, const char *y,
                          const char *title, const char *subtitle)
{
    float fold;

    ajDebug("=g= plmtex('t', 2.5, 0.5, 0.5, '%s') [title]\n", title);
    plmtex("t", (PLFLT) 2.5, (PLFLT) 0.5, (PLFLT) 0.5, title);
    ajDebug("=g= plmtex('b', 3.2, 0.5, 0.5, '%s') [x-title]\n", x);
    plmtex("b", (PLFLT) 3.2, (PLFLT) 0.5, (PLFLT) 0.5, x);
    ajDebug("=g= plmtex('l', 5.0, 0.5, 0.5, '%s') [y-title]\n", y);
    plmtex("l", (PLFLT) 5.0, (PLFLT) 0.5, (PLFLT) 0.5, y);

    fold = ajGraphicsSetCharscale(0.0);
    ajGraphicsSetCharscale(fold/(float)2.0);

    ajDebug("=g= plmtex('t', 1.5, 0.5, 0.5, '%s') [subtitle]\n", subtitle);
    plmtex("t", (PLFLT) 1.5, (PLFLT) 0.5, (PLFLT) 0.5, subtitle);

    fold = ajGraphicsSetCharscale(fold);

    /*  pllab(x,y,title);*/

    return;
}



/* @func ajGraphicsSetLabelsS **************************************************
**
** Label current Plot.
**
** @param [r] x [const AjPStr]        text for x axis labelling.
** @param [r] y [const AjPStr]        text for y axis labelling.
** @param [r] title [const AjPStr]    text for title of plot.
** @param [r] subtitle [const AjPStr] text for subtitle of plot.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsSetLabelsS(const AjPStr x, const AjPStr y,
                          const AjPStr title, const AjPStr subtitle)
{
    float fold;

    ajDebug("=g= plmtex('t', 2.5, 0.5, 0.5, '%S') [title]\n", title);
    plmtex("t", (PLFLT) 2.5, (PLFLT) 0.5, (PLFLT) 0.5, MAJSTRGETPTR(title));
    ajDebug("=g= plmtex('b', 3.2, 0.5, 0.5, '%S') [x-title]\n", x);
    plmtex("b", (PLFLT) 3.2, (PLFLT) 0.5, (PLFLT) 0.5,  MAJSTRGETPTR(x));
    ajDebug("=g= plmtex('l', 5.0, 0.5, 0.5, '%S') [y-title]\n", y);
    plmtex("l", (PLFLT) 5.0, (PLFLT) 0.5, (PLFLT) 0.5,  MAJSTRGETPTR(y));

    fold = ajGraphicsSetCharscale(0.0);
    ajGraphicsSetCharscale(fold/(float)2.0);

    ajDebug("=g= plmtex('t', 1.5, 0.5, 0.5, '%S') [subtitle]\n", subtitle);
    plmtex("t", (PLFLT) 1.5, (PLFLT) 0.5, (PLFLT) 0.5,  MAJSTRGETPTR(subtitle));

    fold = ajGraphicsSetCharscale(fold);

    /*  pllab(x,y,title);*/

    return;
}



/* @obsolete ajGraphLabel
** @rename ajGraphicsSetLabelsC
*/

__deprecated void ajGraphLabel(const char *x, const char *y,
                               const char *title, const char *subtitle)
{
    ajGraphicsSetLabelsC(x, y, title, subtitle);
    return;
}




/* @funcstatic GraphLabelTitle ************************************************
**
** Label current Plot with title and subtitle
**
** @param [r] title [const char*]    text for title of plot.
** @param [r] subtitle [const char*] text for subtitle of plot.
** @return [void]
** @@
******************************************************************************/

static void GraphLabelTitle(const char *title, const char *subtitle)
{
    float fold;
    AjPStr tmpstr = NULL;

    tmpstr = ajStrNewC(title);
    if(ajStrGetLen(tmpstr) > 55)
    {
	ajStrKeepRange(&tmpstr, 0, 55);
	ajStrAppendC(&tmpstr, "...");
    }

    ajDebug("=g= plmtex('t', 2.5, 0.5, 0.5, '%S') [title]\n", tmpstr);

    plvsta();          /* set standard viewport for standard titles */

    if(ajStrGetLen(tmpstr)) {
	fold = ajGraphicsSetCharscale(1.0);
	ajDebug("GraphLabelTitle title fold: %.3f size: %.3f scale %.3f\n",
		fold, currentcharsize, currentcharscale);
	plmtex("t", (PLFLT) 4.0, (PLFLT) 0.5, (PLFLT) 0.5,
	       ajStrGetPtr(tmpstr));
	fold = ajGraphicsSetCharscale(fold);
    }

    ajStrAssignC(&tmpstr, subtitle);

    if(ajStrGetLen(tmpstr) > 95)
    {
	ajStrKeepRange(&tmpstr, 0, 95);
	ajStrAppendC(&tmpstr, "...");
    }

    ajDebug("=g= plmtex('t', 1.5, 0.5, 0.5, '%S') [subtitle]\n", tmpstr);

    if(ajStrGetLen(tmpstr))
    {
	fold = ajGraphicsSetCharscale(1.0);
	ajGraphicsSetCharscale(0.5);
	ajDebug("GraphLabelTitle subtitle fold: %.3f size: %.3f scale %.3f\n",
		fold, currentcharsize, currentcharscale);
	plmtex("t", (PLFLT) 5.0, (PLFLT) 0.5, (PLFLT) 0.5,
	       ajStrGetPtr(tmpstr));
	fold = ajGraphicsSetCharscale(fold);
    }

    if (graphData)
    {
        ajFmtPrintF(graphData->File,"##Maintitle %s\n",title);
        graphData->Lines++;

        if(subtitle)
        {
            ajFmtPrintF(graphData->File,"##Subtitle %s\n",subtitle);
            graphData->Lines++;
        }
    }


    /*  pllab(x,y,title);*/

    ajStrDel(&tmpstr);
    return;
}





/******************************************************************************
PLPLOT CALLS *END*
******************************************************************************/




/* @func ajGraphOpenPlot ******************************************************
**
** Open a window.
**
** @param [w] thys [AjPGraph] Graph object.
** @param [r] numofsets [ajint] number of plots in set.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphOpenPlot(AjPGraph thys, ajint numofsets)
{
    ajGraphSetDevice(thys);
    GraphSetNumSubPage(numofsets);
    GraphColourBack();
    GraphInit(thys);
    GraphColourFore();

    return;
}




/* @func ajGraphOpenWin *******************************************************
**
** Open a window whose view is defined by x and y's min and max.
**
** @param [w] thys [AjPGraph] Graph object
** @param [r] xmin [float] minimum x value.(user coordinates)
** @param [r] xmax [float] maximum x value.
** @param [r] ymin [float] minimum y value.
** @param [r] ymax [float] maximum y value.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphOpenWin(AjPGraph thys, float xmin, float xmax,
		    float ymin, float ymax)
{
    AjPTime ajtime;

    ajGraphSetDevice(thys);

    if (thys)
    {
	/* Calling funclist graphType() */
	graphType[thys->displaytype].GOpen(thys,
			     graphType[thys->displaytype].Ext);

	if(!ajStrGetLen(thys->title))
	{
	    ajFmtPrintAppS(&thys->title,"%S",
			   ajUtilGetProgram());
	}

	if(!ajStrGetLen(thys->subtitle))
	{
	    ajtime = ajTimeNewTodayFmt("report");
	    ajFmtPrintAppS(&thys->subtitle,"%D",
			   ajtime);
	    ajTimeDel(&ajtime);
	}
    }
    GraphColourBack();
    GraphInit(thys);
    GraphColourFore();
    GraphSubPage(0);

    GraphLabelTitle(((thys->flags & AJGRAPH_TITLE) ?
                     ajStrGetPtr(thys->title) : ""),
                    ((thys->flags & AJGRAPH_SUBTITLE) ?
                     ajStrGetPtr(thys->subtitle) : ""));
    thys->xstart = xmin;
    thys->xend = xmax;
    thys->ystart = ymin;
    thys->yend = ymax;
    thys->windowset = ajTrue;

    GraphSetWin(xmin, xmax, ymin, ymax);

    return;
}




/* @func ajGraphNewPage *******************************************************
**
** Clear Screen if (X) or new page if plotter/postscript. Also pass a boolean
** to state whether you want the current pen colour character sizes,
** plot title and subtitle, etc to be reset or stay the same for the next page.
**
** @param [w] thys [AjPGraph] Graph object
** @param [r] resetdefaults [AjBool] reset page setting?
** @return [void]
** @@
******************************************************************************/

void ajGraphNewPage(AjPGraph thys, AjBool resetdefaults)
{
    ajint old;
    ajint cold;
    float fold;


    ajDebug("ajGraphNewPage reset:%B\n", resetdefaults);

    if(graphData)
    {
	GraphDatafileNext();
	return;
    }

    GraphSubPage(0);

    if(resetdefaults)
    {
	ajGraphicsSetColourFore(BLACK);
	ajGraphicsSetCharscale(1.0);
	ajGraphSetLineStyle(0);
    }
    else
    {
	/* pladv resets every thing so need */
	/* to get the old copies */
	cold = ajGraphicsSetColourFore(BLACK);
	fold = ajGraphicsSetCharscale(0.0);
	old = ajGraphSetLineStyle(0);
	ajGraphTrace(thys);

	GraphLabelTitle(((thys->flags & AJGRAPH_TITLE) ?
                         ajStrGetPtr(thys->title) : " "),
                        ((thys->flags & AJGRAPH_SUBTITLE) ?
                         ajStrGetPtr(thys->subtitle) : " "));

        if(thys->windowset)
            GraphSetWin(thys->xstart, thys->xend,
                        thys->ystart, thys->yend);
        else
            GraphSetWin(0.0, 480.0,
                        0.0, 640.0);

	/* then set it again */
	ajGraphicsSetColourFore(cold);
	ajGraphicsSetCharscale(fold);
	ajGraphSetLineStyle(old);
    }

    return;
}




/* @func ajGraphCloseWin ******************************************************
**
** Close current window.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphCloseWin(void)
{
    ajDebug("ajGraphCloseWin\n");
    GraphClose();

    return;
}




/* @func ajGraphOpen **********************************************************
**
** Open a window whose view is defined by the x's and y's min and max
** values.
**
** @param [w] thys [AjPGraph] Graph object
** @param [r] xmin [PLFLT] minimum x value.(user coordinates)
** @param [r] xmax [PLFLT] maximum x value.
** @param [r] ymin [PLFLT] minimum y value.
** @param [r] ymax [PLFLT] maximum y value.
** @param [r] flags [ajint] flag bit settings
**
** @return [void]
** @@
******************************************************************************/

void ajGraphOpen(AjPGraph thys, PLFLT xmin, PLFLT xmax,
		  PLFLT ymin, PLFLT ymax, ajint flags)
{
    AjPTime ajtime;

    ajGraphSetDevice(thys);

    /* Calling funclist graphType() */
    graphType[thys->displaytype].GOpen(thys,
                                       graphType[thys->displaytype].Ext);

    if( ajStrGetLen(thys->title) <=1)
    {
        ajtime = ajTimeNewToday();
        ajStrAppendC(&thys->title,
                     ajFmtString("%S (%D)",ajUtilGetProgram(),ajtime));
        ajTimeDel(&ajtime);
    }

    GraphSetName(thys, thys->outputfile,
                 graphType[thys->displaytype].Ext);
    thys->xstart = xmin;
    thys->xend = xmax;
    thys->ystart = ymin;
    thys->yend = ymax;

    GraphColourBack();
    GraphInit(thys);
    GraphColourFore();
    ajGraphPlenv(xmin, xmax , ymin, ymax , flags);

    return;
}




/* @func ajGraphClose *********************************************************
**
** Close current Plot.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphClose(void)
{
    ajDebug("ajGraphClose\n");
    GraphClose();

    return;
}




/* @func ajGraphDumpDevices ***************************************************
**
** Dump device options for a graph object
**
** @return [void]
** @@
******************************************************************************/

void ajGraphDumpDevices(void)
{
    ajint i;
    ajint j;
    AjPStr aliases = NULL;

    ajUserDumpC("Devices allowed (with alternative names) are:-");

    for(i=0;graphType[i].Name;i++)
    {
	if(!graphType[i].GOpen)
            continue;

	if(!graphType[i].Alias)
	{
	    ajStrAssignClear(&aliases);

	    for(j=0;graphType[j].Name;j++)
	    {
		if(graphType[j].Alias &&
		   ajCharMatchC(graphType[i].Device, graphType[j].Device))
		{
		    if(ajStrGetLen(aliases))
			ajStrAppendC(&aliases, ", ");
		    ajStrAppendC(&aliases, graphType[j].Name);
		}
	    }

	    if(ajStrGetLen(aliases))
		ajUser("%s (%S)",graphType[i].Name, aliases);
	    else
		ajUserDumpC(graphType[i].Name);
	}
    }

    ajStrDel(&aliases);

    return;
}





/* @func ajGraphxyDumpDevices *************************************************
**
** Dump device options for a graph object
**
** @return [void]
** @@
******************************************************************************/

void ajGraphxyDumpDevices(void)
{
    ajint i;
    ajint j;
    AjPStr aliases = NULL;

    ajUserDumpC("Devices allowed (with alternative names) are:-");

    for(i=0;graphType[i].Name;i++)
    {
	if(!graphType[i].XYDisplay)
            continue;

	if(!graphType[i].Alias)
	{
	    ajStrAssignClear(&aliases);

	    for(j=0;graphType[j].Name;j++)
	    {
		if(graphType[j].Alias &&
		   ajCharMatchC(graphType[i].Device, graphType[j].Device))
		{
		    if(ajStrGetLen(aliases))
			ajStrAppendC(&aliases, ", ");
		    ajStrAppendC(&aliases, graphType[j].Name);
		}
	    }

	    if(ajStrGetLen(aliases))
		ajUser("%s (%S)",graphType[i].Name, aliases);
	    else
		ajUserDumpC(graphType[i].Name);
	}
    }

    ajStrDel(&aliases);

    return;
}





/* @func ajGraphListDevices ***************************************************
**
** Store device names for a graph object in a list
**
** @param [w] list [AjPList] List to write device names to.
** @return [void]
** @@
******************************************************************************/

void ajGraphListDevices (AjPList list)
{
    ajint i;
    AjPStr devname;

    for(i=0;graphType[i].Name;i++)
    {
	if(!graphType[i].GOpen)
            continue;

	if(!graphType[i].Alias)
	{
	    devname = ajStrNewC(graphType[i].Name);
	    ajListstrPushAppend(list, devname);
	}
    }

    return;
}





/* @func ajGraphxyListDevices *************************************************
**
** Store device names for a graph object in a list
**
** @param [w] list [AjPList] List to write device names to.
** @return [void]
** @@
******************************************************************************/

void ajGraphxyListDevices (AjPList list)
{
    ajint i;
    AjPStr devname;

    for(i=0;graphType[i].Name;i++)
    {
	if(!graphType[i].XYDisplay)
            continue;

	if(!graphType[i].Alias)
	{
	    devname = ajStrNewC(graphType[i].Name);
	    ajListstrPushAppend(list, devname);
	}
    }

    return;
}




/* @func ajGraphTrace *********************************************************
**
** Writes debug messages to trace the contents of a graph object.
**
** @param [r] thys [const AjPGraph] Graph object
** @return [void]
** @@
******************************************************************************/

void ajGraphTrace(const AjPGraph thys)
{
    ajDebug("Graph trace\n");

    ajDebug("\n(a) True booleans\n");

    ajDebug("minmaxcalc %B\n", thys->minmaxcalc);


    ajDebug("\n(b) Strings with values\n");

    ajDebug("Desc '%S'\n", thys->desc);
    ajDebug("Title '%S'\n", thys->title);
    ajDebug("Subtitle '%S'\n", thys->subtitle);
    ajDebug("Xaxis '%S'\n", thys->xaxis);
    ajDebug("Yaxis '%S'\n", thys->yaxis);
    ajDebug("outputfile '%S'\n", thys->outputfile);

    ajDebug("\n(c) Other values\n");
    ajDebug("flags %x\n", thys->flags);
    ajDebug("numofgraphs %d\n", thys->numofgraphs);
    ajDebug("numofgraphsmax %d\n", thys->numofgraphsmax);
    ajDebug("minX   %7.3f maxX   %7.3f\n",
	    thys->minX,
	    thys->maxX);
    ajDebug("minY   %7.3f maxY   %7.3f\n",
	    thys->minY,
	    thys->maxY);
    ajDebug("xstart %7.3f xend   %7.3f\n",
	    thys->xstart,
	    thys->xend);
    ajDebug("ystart %7.3f yend   %7.3f\n",
	    thys->ystart,
	    thys->yend);
    ajDebug("displaytype %d '%s'\n", thys->displaytype,
	    graphType[thys->displaytype].Device);

    return;
}




/* @func ajGraphdataTrace **************************************************
**
** Writes debug messages to trace the contents of a graphdata object.
**
** @param [r] thys [const AjPGraphdata] Graphdata object
** @return [void]
** @@
******************************************************************************/

void ajGraphdataTrace(const AjPGraphdata thys)
{
    ajint i = 0;
    AjPGraphobj obj;

    ajDebug("GraphData trace\n");
    ajDebug("\n(a) True booleans\n");

    if(thys->xcalc)
	ajDebug("xcalc %B\n", thys->xcalc);

    if(thys->ycalc)
	ajDebug("ycalc %B\n", thys->ycalc);

    ajDebug("\n(b) Strings with values\n");

    ajDebug("Title '%S'\n", thys->title);
    ajDebug("Subtitle '%S'\n", thys->subtitle);
    ajDebug("Xaxis '%S'\n", thys->xaxis);
    ajDebug("Yaxis '%S'\n", thys->yaxis);
    ajDebug("gtype '%S'\n", thys->gtype);

    ajDebug("\n(c) Other values\n");
    ajDebug("numofpoints %d\n", thys->numofpoints);
    ajDebug("numofobjects %d\n", thys->numofobjects);
    ajDebug("minX   %7.3f maxX   %7.3f\n", thys->minX, thys->maxX);
    ajDebug("minY   %7.3f maxY   %7.3f\n", thys->minY, thys->maxY);
    ajDebug("tminX   %7.3f tmaxX   %7.3f\n", thys->tminX, thys->tmaxX);
    ajDebug("tminY   %7.3f tmaxY   %7.3f\n", thys->tminY, thys->tmaxY);
    ajDebug("colour %d\n", thys->colour);
    ajDebug("lineType %d\n", thys->lineType);

    ajDebug("obj list: %x\n", thys->Dataobj);

    if(thys->Dataobj)
    {
	obj=thys->Dataobj;

	while(obj->next)
	{
	    i++;
	    obj = obj->next;
	}
    }

    ajDebug("obj list length: %d/%d\n",
	     i, thys->numofobjects);
 
    return;
}




/* @func ajGraphicsDrawArc ****************************************************
**
** Draw a portion of a circle (an arc).
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] Radius  [PLFLT] radius of the circle.
** @param  [r] StartAngle [PLFLT] angle of the start of the arc.
** @param  [r] EndAngle [PLFLT] angle of the end of the arc.
** @return [void]
** @@
**
** NOTE: Due to x and y not the same length this produces an oval!!
**       This will have to do for now. But I am aware that the code
**       is slow and not quite right.
******************************************************************************/

void ajGraphicsDrawArc(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
                       PLFLT StartAngle, PLFLT EndAngle)
{
    PLFLT angle;
    ajint i;
    PLFLT x[361];
    PLFLT y[361];
    ajint numofpoints;

    x[0] = xcentre + ( Radius*(float)cos(ajDegToRad(StartAngle)) );
    y[0] = ycentre + ( Radius*(float)sin(ajDegToRad(StartAngle)) );

    for(i=1, angle=StartAngle+1; angle<EndAngle; angle++, i++)
    {
	x[i] = xcentre + ( Radius*(float)cos(ajDegToRad(angle)) );
	y[i] = ycentre + ( Radius*(float)sin(ajDegToRad(angle)) );
    }

    x[i] = xcentre + ( Radius*(float)cos(ajDegToRad(EndAngle)) );
    y[i] = ycentre + ( Radius*(float)sin(ajDegToRad(EndAngle)) );
    numofpoints = i+1;

    GraphDrawLines(numofpoints, x, y);

    return;
}


/* @obsolete ajGraphPartCircle
** @rename ajGraphicsDrawArc
*/

__deprecated void ajGraphPartCircle(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
		       PLFLT StartAngle, PLFLT EndAngle)
{
    
    ajGraphicsDrawArc(xcentre, ycentre, Radius, StartAngle, EndAngle);
    return;
}





/* @func ajGraphicsDrawBarsHoriz ***********************************************
**
** Draw Horizontal Error Bars.
**
** @param [r] numofpoints [ajint] number of error bars to be drawn.
** @param [r] y [CONST PLFLT*] y positions to draw at.
** @param [r] xmin [CONST PLFLT*] x positions to start at.
** @param [r] xmax [CONST PLFLT*] x positions to end at.
** @return [void]
**
** @@
******************************************************************************/
void ajGraphicsDrawBarsHoriz(ajint numofpoints,
                             PLFLT *y, PLFLT *xmin, PLFLT *xmax)
{
    if(numofpoints)
	ajDebug("=g= plerrx(%d %.2f..%.2f, %.2f, %.2f) "
		 "[num,y..y,xmin,xmax]\n",
		 numofpoints, y[0], y[numofpoints-1], xmin, xmax);
    else
	ajDebug("=g= plerrx(%d <>..<>, %.2f, %.2f) [num,y..y,xmin,xmax]\n",
		 numofpoints, xmin, xmax);

    plerrx(numofpoints,y,xmin,xmax);

    return;
}


/* @obsolete ajGraphHoriBars
** @rename ajGraphicsDrawBarsHoriz
*/

__deprecated void ajGraphHoriBars(ajint numofpoints,
                                  PLFLT *y, PLFLT *xmin, PLFLT *xmax)
{
    ajGraphicsDrawBarsHoriz(numofpoints,y, xmin, xmax);
    return;
}



/* @func ajGraphicsDrawBarsVert ************************************************
**
** Draw vertical Error Bars.
**
** @param [r] numofpoints [ajint] number of error bars to be drawn.
** @param [r] x [CONST PLFLT*] x positions to draw at.
** @param [r] ymin [CONST PLFLT*] y positions to start at.
** @param [r] ymax [CONST PLFLT*] y positions to end at.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawBarsVert(ajint numofpoints,
                            PLFLT *x, PLFLT *ymin, PLFLT *ymax)
{
    if(numofpoints)
	ajDebug("=g= plerry(%d %.2f..%.2f, %.2f, %.2f) "
		 "[num,x..x,ymin,ymax]\n",
		 numofpoints, x[0], x[numofpoints-1], ymin, ymax);
    else
	ajDebug("=g= plerry(%d <>..<>, %.2f, %.2f) [num,x..x,ymin,ymax]\n",
		 numofpoints, ymin, ymax);

    plerry(numofpoints,x,ymin,ymax);

    return;
}



/* @obsolete ajGraphVertBars
** @rename ajGraphicsDrawBarsVert
*/

__deprecated void ajGraphVertBars(ajint numofpoints,
                                  PLFLT *x, PLFLT *ymin, PLFLT *ymax)
{
    ajGraphicsDrawBarsVert(numofpoints, x, ymin, ymax);
    return;
}


/* @func ajGraphicsDrawBox *****************************************************
**
** Draw a box to the plotter device at point xx0,yy0 size big.
**
** @param [r] xx0 [PLFLT] x position to draw the box.
** @param [r] yy0 [PLFLT] y position to draw the box.
** @param [r] size [PLFLT]  how big to draw the box.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawBox(PLFLT xx0, PLFLT yy0,PLFLT size)
{
    PLFLT x[5];
    PLFLT y[5];

    if(graphData)
    {
	ajFmtPrintF(graphData->File,"Rectangle x1 %f y1 %f x2 %f"
		    " y2 %f colour %d\n",
		    xx0, yy0, xx0+size, yy0+size, currentfgcolour);
	graphData->Lines++;
    }
    else
    {
	x[0] = xx0;
	y[0] = yy0;
	x[1] = xx0;
	y[1] = yy0 + size;
	x[2] = xx0 + size;
	y[2] = yy0 + size;
	x[3] = xx0 + size;
	y[3] = yy0;
	x[4] = x[0];
	y[4] = y[0];
	GraphArray(5, x, y);
    }

    return;
}


/* @obsolete ajGraphBox
** @rename ajGraphicsDrawBox
*/

__deprecated void ajGraphBox(PLFLT xx0, PLFLT yy0,PLFLT size)
{
    ajGraphicsDrawBox(xx0, yy0, size);
    return;
}


/* @func ajGraphicsDrawBoxFill *************************************************
**
** Draw a box to the plotter device at point xx0,yy0 size big and
** fill it in.
**
** @param [r] xx0 [PLFLT] x position to draw the box.
** @param [r] yy0 [PLFLT] y position to draw the box.
** @param [r] size [PLFLT]  how big to draw the box.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawBoxFill(PLFLT xx0, PLFLT yy0, PLFLT size)
{
    PLFLT x[4];
    PLFLT y[4];

    if(graphData)
    {
	ajFmtPrintF(graphData->File,"Shaded Rectangle x1 %f y1 %f x2 %f"
		    " y2 %f colour %d\n",
		    xx0, yy0, xx0+size, yy0+size, currentfgcolour);
	graphData->Lines++;
    }
    else
    {
	x[0] = xx0;
	y[0] = yy0;
	x[1] = xx0;
	y[1] = yy0 + size;
	x[2] = xx0 + size;
	y[2] = yy0 + size;
	x[3] = xx0 + size;
	y[3] = yy0;
	GraphFill(4, x, y);
    }

    return;
}



/* @obsolete ajGraphBoxFill
** @rename ajGraphicsDrawBoxFill
*/

__deprecated void ajGraphBoxFill(PLFLT xx0, PLFLT yy0, PLFLT size)
{
    ajGraphicsDrawBoxFill(xx0, yy0, size);
    return;
}




/* @func ajGraphicsDrawCircle *************************************************
**
** Draw a circle.
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] radius  [float] radius of the circle.
** @return [void]
** @@
**
** NOTE: Due to x and y not the same length this produces an oval!!
**       This will have to do for now. But i (il@sanger) am aware that the code
**       is slow and not quite right.
******************************************************************************/

void ajGraphicsDrawCircle(PLFLT xcentre, PLFLT ycentre, float radius)
{
    PLFLT x[361];
    PLFLT y[361];
    
    ajint i;

    /* graphData : uses GraphArray */

    for(i=0;i<360;i++)
    {
	x[i] = xcentre + (radius * (float)cos(ajDegToRad((float)i)));
	y[i] = ycentre + (radius * (float)sin(ajDegToRad((float)i)));
    }

    x[360] = x[0];
    y[360] = y[0];

    GraphArray(361, x,y);

    return;
}



/* @obsolete ajGraphCircle
** @rename ajGraphicsDrawCircle
*/

__deprecated void ajGraphCircle(PLFLT xcentre, PLFLT ycentre, float radius)
{
    ajGraphicsDrawCircle(xcentre, ycentre, radius);
    return;
}


/* @func ajGraphicsDrawDia *****************************************************
**
** Draw a diamond to the plotter device at point xx0,yy0 size big.
**
** @param [r] xx0 [PLFLT] x position to draw the diamond.
** @param [r] yy0 [PLFLT] y position to draw the diamond.
** @param [r] size [PLFLT]  how big to draw the diamond.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawDia(PLFLT xx0, PLFLT yy0, PLFLT size)
{
    PLFLT x[5];
    PLFLT y[5];
    PLFLT incr;

    incr = size*(float)0.5;

    /* graphData : uses GraphArray */

    x[0] = xx0;
    y[0] = yy0 + incr;
    x[1] = xx0 + incr;
    y[1] = yy0 + size;
    x[2] = xx0 + size;
    y[2] = yy0 + incr;
    x[3] = xx0 + incr;
    y[3] = yy0;
    x[4] = x[0];
    y[4] = y[0];
    GraphArray(5, x, y);

    return;
}



/* @obsolete ajGraphDia
** @rename ajGraphicsDrawDia
*/

__deprecated void ajGraphDia(PLFLT xx0, PLFLT yy0, PLFLT size)
{
    ajGraphicsDrawDia(xx0, yy0, size);
    return;
}


/* @func ajGraphicsDrawDiaFill *************************************************
**
** Draw a diamond to the plotter device at point xx0,yy0 size big and
** fill it in.
**
** @param [r] xx0 [PLFLT] x position to draw the diamond.
** @param [r] yy0 [PLFLT] y position to draw the diamond.
** @param [r] size [PLFLT]  how big to draw the diamond.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawDiaFill(PLFLT xx0, PLFLT yy0, PLFLT size)
{
    PLFLT x[4];
    PLFLT y[4];
    PLFLT incr;

    incr = size*(float)0.5;

    x[0] = xx0;
    y[0] = yy0 + incr;
    x[1] = xx0 + incr;
    y[1] = yy0 + size;
    x[2] = xx0 + size;
    y[2] = yy0 + incr;
    x[3] = xx0 + incr;
    y[3] = yy0;

    GraphFill(4, x, y);

    return;
}


/* @obsolete ajGraphDiaFill
** @rename ajGraphicsDrawDiaFill
*/

__deprecated void ajGraphDiaFill(PLFLT xx0, PLFLT yy0, PLFLT size)
{
    ajGraphicsDrawDiaFill(xx0, yy0, size);
    return;
}


/* @func ajGraphicsDrawDots ****************************************************
**
** Draw a set of dots.
**
** @param [r] xx1 [CONST PLFLT*] xx1 coord.
** @param [r] yy1 [CONST PLFLT*] yy1 coord.
** @param [r] numofdots [ajint] The number of dots to be drawn.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawDots(PLFLT *xx1,PLFLT *yy1, ajint numofdots)
{
    GraphSymbols(xx1, yy1, numofdots, 17);

    /* see xx06c in examples for codes e.g. 17 is a dot*/
    return;
}





/* @obsolete ajGraphDots
** @rename ajGraphicsDrawDots
*/

__deprecated void ajGraphDots(PLFLT *xx1,PLFLT *yy1, ajint numofdots)
{
    ajGraphicsDrawDots(xx1,yy1,numofdots);
    return;
}



/* @func ajGraphicsDrawLine ****************************************************
**
** Draw line between 2 points.
**
** @param [r] xx1 [PLFLT]  x start position.
** @param [r] yy1 [PLFLT]  y start position.
** @param [r] xx2 [PLFLT]  x end position.
** @param [r] yy2 [PLFLT]  y end position.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawLine(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2)
{
    if(graphData)
    {
	ajFmtPrintF(graphData->File,
		    "Line x1 %f y1 %f x2 %f y2 %f colour %d\n",
		    xx1, yy1, xx2, yy2, currentfgcolour);
	graphData->Lines++;
    }
    else
    {
	/*ajDebug("=g= pljoin(%.2f, %.2f, %.2f, %.2f) [xy xy]\n",
		 xx1, yy1, xx2, yy2);*/
	pljoin(xx1,yy1,xx2,yy2);
    }

    return;
}



/* @obsolete ajGraphLine
** @rename ajGraphicsDrawLine
*/

__deprecated void ajGraphLine(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2)
{
    ajGraphicsDrawLine(xx1, yy1, xx2, yy2);
    return;
}


/* @funcstatic GraphDrawLines **********************************************
**
** Draw a number of lines. The points are stored in two array.
**
** @param [r] numoflines [ajint] number of line to plot.
** @param [r] xx1         [CONST PLFLT*] pointer to x coordinates.
** @param [r] yy1         [CONST PLFLT*] pointer to y coordinates.
** @return [void]
** @@
******************************************************************************/

static void GraphDrawLines( ajint numoflines,PLFLT *xx1, PLFLT *yy1)
{
    ajint i;
    PLFLT xx0;
    PLFLT yy0;

    /* graphdata : calls ajGraphicsDrawLine */

    for(i=0;i<numoflines-1;i++)
    {
	xx0 = *xx1;
	yy0 = *yy1;
	xx1++;
	yy1++;
	ajGraphicsDrawLine(xx0,yy0,*xx1,*yy1);
    }

    return;
}




/* @func ajGraphicsDrawLines ***************************************************
**
** Draw a set of lines.
**
** @param [u] xx1 [PLFLT*] xx1 coord.
** @param [u] yy1 [PLFLT*] yy1 coord.
** @param [u] xx2 [PLFLT*] xx2 coord.
** @param [u] yy2 [PLFLT*] yy2 coord.
** @param [r] numoflines [ajint] The number of lines to be drawn.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawLines(PLFLT *xx1, PLFLT *yy1, PLFLT *xx2, PLFLT *yy2,
		  ajint numoflines)
{
    ajint i = 0;

    /* graphdata : calls ajGraphLine */

    ajDebug("=g= ajGraphicsDrawLines(... %d) [xx1,xx2,yy1,yy2,numoflines]\n",
	    numoflines);

    for(i=0;i<numoflines;i++)
    {
	ajGraphicsDrawLine(*xx1,*yy1,*xx2,*yy2);
	xx1++; yy1++; xx2++; yy2++;
    }

    return;
}


/* @obsolete ajGraphLines
** @rename ajGraphicsDrawLines
*/

__deprecated void ajGraphLines(PLFLT *xx1, PLFLT *yy1, PLFLT *xx2, PLFLT *yy2,
                               ajint numoflines)

{
    ajGraphicsDrawLines(xx1, yy1, xx2, yy2, numoflines);
    return;
}


/* @func ajGraphicsDrawPoly ****************************************************
**
** Draw a polygon.
**
** @param [r] n [ajint] number of points
** @param [r] x [CONST PLFLT *] x coord of points
** @param [r] y [CONST PLFLT *] y coord of points
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawPoly(ajint n, PLFLT *x, PLFLT *y)
{
    /* graphData : uses GraphArray */

    GraphArray(n, x, y);

    return;
}


/* @obsolete ajGraphPoly
** @rename ajGraphicsDrawPoly
*/

__deprecated void ajGraphPoly(ajint n, PLFLT *x, PLFLT *y)
{
    ajGraphicsDrawPoly(n, x, y);
    return;
}


/* @func ajGraphicsDrawPolyFill ************************************************
**
** Draw a polygon and fill it in.
**
** @param [r] n [ajint] number of points
** @param [r] x [CONST PLFLT *] x coord of points
** @param [r] y [CONST PLFLT *] y coord of points
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawPolyFill(ajint n, PLFLT *x, PLFLT *y)
{
    GraphFill(n, x, y);

    return;
}



/* @obsolete ajGraphPolyFill
** @rename ajGraphicsDrawPolyFill
*/

__deprecated void ajGraphPolyFill(ajint n, PLFLT *x, PLFLT *y)
{
    ajGraphicsDrawPolyFill(n, x, y);
    return;
}


/* @func ajGraphicsDrawRect ****************************************************
**
** Draw a rectangle with the current pen colour/style.
**
** @param [r] xx0 [PLFLT] xx0 coor.
** @param [r] yy0 [PLFLT] yy0 coor.
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawRect(PLFLT xx0, PLFLT yy0,PLFLT xx1, PLFLT yy1)
{
    PLFLT x[5];
    PLFLT y[5];

    if(graphData)
    {
	ajFmtPrintF(graphData->File,"Rectangle x1 %f y1 %f x2 %f"
		    " y2 %f colour %d\n",
		    xx0, yy0, xx1, yy1, currentfgcolour);
	graphData->Lines++;
    }
    else
    {
	x[0] = xx0;
	y[0] = yy0;
	x[1] = xx0;
	y[1] = yy1;
	x[2] = xx1;
	y[2] = yy1;
	x[3] = xx1;
	y[3] = yy0;
	x[4] = x[0];
	y[4] = y[0];
	GraphArray(5, x, y);
    }

    return;
}



/* @obsolete ajGraphRect
** @rename ajGraphicsDrawRect
*/

__deprecated void ajGraphRect(PLFLT xx0, PLFLT yy0,PLFLT xx1, PLFLT yy1)
{
    ajGraphicsDrawRect(xx0, yy0, xx1, yy1);
    return;
}




/* @func ajGraphicsDrawRectFill ************************************************
**
** Draw a rectangle and fill it with the current pen colour/style.
**
** @param [r] xx0 [PLFLT] xx0 coor.
** @param [r] yy0 [PLFLT] yy0 coor.
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawRectFill(PLFLT xx0, PLFLT yy0, PLFLT xx1, PLFLT yy1)
{
    PLFLT x[4];
    PLFLT y[4];

    if(graphData)
    {
	ajFmtPrintF(graphData->File,"Shaded Rectangle x1 %f y1 %f x2 %f"
		    " y2 %f colour %d\n",
		    xx0, yy0, xx1, yy1, currentfgcolour);
	graphData->Lines++;
    }
    else
    {
	x[0] = xx0;
	y[0] = yy0;
	x[1] = xx0;
	y[1] = yy1;
	x[2] = xx1;
	y[2] = yy1;
	x[3] = xx1;
	y[3] = yy0;
	GraphFill(4, x, y);
    }

    return;
}



/* @obsolete ajGraphRectFill
** @rename ajGraphicsDrawRectFill
*/

__deprecated void ajGraphRectFill(PLFLT xx0, PLFLT yy0, PLFLT xx1, PLFLT yy1)
{
    ajGraphicsDrawRectFill(xx0, yy0, xx1, yy1);
    return;
}



/* @func ajGraphicsDrawRectOncurve ********************************************
**
** Draw a rectangle along a curve with the current pen colour/style.
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] Radius  [PLFLT] radius of the circle.
** @param  [r] BoxHeight [PLFLT] Height of the rectangle in user coordinates.
** @param  [r] StartAngle [PLFLT] angle of the start of the rectangle.
** @param  [r] EndAngle [PLFLT] angle of the end of the rectangle.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawRectOncurve(PLFLT xcentre, PLFLT ycentre,
                               PLFLT Radius, PLFLT BoxHeight,
                               PLFLT StartAngle, PLFLT EndAngle)
{
    PLFLT *xyy1;
    PLFLT *xyy2;
    PLFLT r1Blocks;
    PLFLT r2Blocks;

    r1Blocks = Radius;
    r2Blocks = r1Blocks+BoxHeight;

    ajGraphicsDrawArc(xcentre, ycentre, r1Blocks, StartAngle, EndAngle);
    ajGraphicsDrawArc(xcentre, ycentre, r2Blocks, StartAngle, EndAngle);

    xyy1 = ajComputeCoord(xcentre, ycentre, r1Blocks, StartAngle);
    xyy2 = ajComputeCoord(xcentre, ycentre, r2Blocks, StartAngle);
    ajGraphicsDrawLine( xyy1[0], xyy1[1], xyy2[0], xyy2[1] );
    AJFREE(xyy1);
    AJFREE(xyy2);
    xyy1 = ajComputeCoord(xcentre, ycentre, r1Blocks, EndAngle);
    xyy2 = ajComputeCoord(xcentre, ycentre, r2Blocks, EndAngle);
    ajGraphicsDrawLine( xyy1[0], xyy1[1], xyy2[0], xyy2[1] );
    AJFREE(xyy1);
    AJFREE(xyy2);

    return;
}



/* @obsolete ajGraphRectangleOnCurve
** @rename ajGraphicsDrawRectOncurve
*/

__deprecated void ajGraphRectangleOnCurve(PLFLT xcentre, PLFLT ycentre,
                                          PLFLT Radius, PLFLT BoxHeight,
                                          PLFLT StartAngle, PLFLT EndAngle)
{

    ajGraphicsDrawRectOncurve(xcentre, ycentre, Radius,
                              BoxHeight, StartAngle, EndAngle);
    return;
}


/* @func ajGraphicsDrawRectOncurveFill *****************************************
**
** Draw a rectangle along a curve and fill it with the current pen
** colour/style.
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] Radius  [PLFLT] radius of the circle.
** @param  [r] BoxHeight [PLFLT] Height of the rectangle in user coordinates.
** @param  [r] StartAngle [PLFLT] angle of the start of the rectangle.
** @param  [r] EndAngle [PLFLT] angle of the end of the rectangle.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawRectOncurveFill(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
                                   PLFLT BoxHeight, PLFLT StartAngle,
                                   PLFLT EndAngle)
{
    PLFLT angle;
    ajint i;
    PLFLT x[4];
    PLFLT y[4];
    ajint numofpoints;
    PLFLT r1Blocks;
    PLFLT r2Blocks;

    r1Blocks = Radius;
    r2Blocks = r1Blocks+BoxHeight;

    for(i=0, angle=StartAngle; angle<EndAngle; angle++, i++)
    {
	x[0]=xcentre + ( r1Blocks*(float)cos(ajDegToRad(angle)) );
	y[0]=ycentre + ( r1Blocks*(float)sin(ajDegToRad(angle)) );
	x[1]=xcentre + ( r2Blocks*(float)cos(ajDegToRad(angle)) );
	y[1]=ycentre + ( r2Blocks*(float)sin(ajDegToRad(angle)) );
	x[2]=xcentre + ( r2Blocks*(float)cos(ajDegToRad(angle+1)) );
	y[2]=ycentre + ( r2Blocks*(float)sin(ajDegToRad(angle+1)) );
	x[3]=xcentre + ( r1Blocks*(float)cos(ajDegToRad(angle+1)) );
	y[3]=ycentre + ( r1Blocks*(float)sin(ajDegToRad(angle+1)) );
	numofpoints = 4;
	ajGraphicsDrawPolyFill(numofpoints, x, y);
    }

    return;
}




/* @obsolete ajGraphFillRectangleOnCurve
** @rename ajGraphicsDrawRectOncurveFill
*/

__deprecated void ajGraphFillRectangleOnCurve(PLFLT xcentre, PLFLT ycentre,
                                              PLFLT Radius,
                                              PLFLT BoxHeight, PLFLT StartAngle,
                                              PLFLT EndAngle)
{

    ajGraphicsDrawRectOncurveFill(xcentre, ycentre, Radius,
                                  BoxHeight, StartAngle, EndAngle);
    return;
}



/* @func ajGraphicsDrawRlabelC *************************************************
**
** Label the right hand y axis.
**
** @param [r] text [const char*] text for label of right y axis.
** @return [void]
** @@
******************************************************************************/
void ajGraphicsDrawRlabelC(const char *text)
{
    ajDebug("=g= plmtex('r', 2.0, 0.5, 0.5, '%s') [ajGraphicsDrawRlabelC]\n",
            text);
    plmtex("r",2.0,0.5,0.5,text);

    return;
}


/* @func ajGraphicsDrawRlabelS *************************************************
**
** Label the right hand y axis.
**
** @param [r] str [const AjPStr] text for label of right y axis.
** @return [void]
** @@
******************************************************************************/
void ajGraphicsDrawRlabelS(const AjPStr str)
{
    ajDebug("=g= plmtex('r', 2.0, 0.5, 0.5, '%S') [ajGraphicsDrawRlabelS]\n",
            str);
    plmtex("r",2.0,0.5,0.5,MAJSTRGETPTR(str));

    return;
}

/* @obsolete ajGraphLabelYRight
** @rename ajGraphicsDrawRlabelC
*/

__deprecated void ajGraphLabelYRight(const char *text)
{
    ajGraphicsDrawRlabelC(text);
    return;
}


/* @func ajGraphicsDrawSymbols *************************************************
**
** Draw a set of dots.
**
** @param [r] numofdots [ajint] Number of coordinates in xx1 and xx2.
** @param [r] xx1 [CONST PLFLT*] xx1 coord.
** @param [r] yy1 [CONST PLFLT*] yy1 coord.
** @param [r] symbol [ajint] Symbol code.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawSymbols( ajint numofdots, PLFLT *xx1,PLFLT *yy1,
                            ajint symbol)
{
    GraphSymbols(xx1, yy1, numofdots, symbol);

    /* see xx06c in examples for codes e.g. 17 is a dot*/
    return;
}




/* @obsolete ajGraphSymbols
** @rename ajGraphicsDrawSymbols
*/

__deprecated void ajGraphSymbols( ajint numofdots, PLFLT *xx1,PLFLT *yy1,
                                  ajint symbol)
{
    ajGraphicsDrawSymbols(numofdots,xx1,yy1,symbol);
    return;
}



/* @func ajGraphicsDrawText ****************************************************
**
** Draw text, positioning with respect to (xx1,yy1) by justified as
** defined by just.
**
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @param [r] text [const char*] The text to be displayed.
** @param [r] just [PLFLT] justification of the string.
**                         (0=left,1=right,0.5=middle etc)
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawText(PLFLT xx1, PLFLT yy1, const char *text, PLFLT just)
{
    if(graphData)
    {
	ajFmtPrintF(graphData->File,
		    "Text1 x1 %f y1 %f colour %d size %f %s\n",
		    xx1,yy1,currentfgcolour, currentcharscale,text);
	graphData->Lines++;
    }
    else
	GraphText(xx1,yy1,1.0,0.0,just, text);

    return;
}


/* @obsolete ajGraphText
** @rename ajGraphicsDrawText
*/

__deprecated void ajGraphText(PLFLT xx1, PLFLT yy1,
                              const char *text, PLFLT just)
{
    ajGraphicsDrawText(xx1,yy1,text,just);
    return;
}


/* @func ajGraphicsDrawTextAtend ***********************************************
**
** Draw text ending at position (xx1,yy1)
**
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @param [r] text [const char*] The text to be displayed.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawTextAtend(PLFLT xx1, PLFLT yy1, const char *text)
{
    if(graphData)
    {
	ajFmtPrintF(graphData->File,
		    "Text3 x1 %f y1 %f colour %d size %f %s\n",
		    xx1,yy1,
		    currentfgcolour, currentcharscale,text);
	graphData->Lines++;
    }
    else
	GraphText(xx1,yy1,1.0,0.0,1.0, text);

    return;
}


/* @obsolete ajGraphTextEnd
** @rename ajGraphicsDrawTextAtend
*/

__deprecated void ajGraphTextEnd(PLFLT xx1, PLFLT yy1, const char *text)
{
    ajGraphicsDrawTextAtend(xx1,yy1,text);
    return;
}


/* @func ajGraphicsDrawTextAtmid ***********************************************
**
** Draw text with Mid point of text at (xx1,yy1).
**
** For graph data type, writes directly.
**
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @param [r] text [const char*] The text to be displayed.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawTextAtmid(PLFLT xx1, PLFLT yy1, const char *text)
{
    if(graphData)
    {
	ajFmtPrintF(graphData->File,
		    "Text2 x1 %f y1 %f colour %d size %f %s\n",
		    xx1, yy1,
		    currentfgcolour, currentcharscale, text);
	graphData->Lines++;
    }
    else
	GraphText(xx1,yy1,1.0,0.0,0.5, text);

    return;
}


/* @obsolete ajGraphTextMid
** @rename ajGraphicsDrawTextAtmid
*/

__deprecated void ajGraphTextMid(PLFLT xx1, PLFLT yy1, const char *text)
{
    ajGraphicsDrawTextAtmid(xx1,yy1,text);
    return;
}


/* @func ajGraphicsDrawTextAtstart *********************************************
**
** Draw text starting at position (xx1,yy1)
**
** @param [r] xx1 [PLFLT] xx1 coordinate
** @param [r] yy1 [PLFLT] yy1 coordinate
** @param [r] text [const char*] The text to be displayed.
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawTextAtstart(PLFLT xx1, PLFLT yy1, const char *text)
{
    if(graphData)
    {
	ajFmtPrintF(graphData->File,
		    "Text1 x1 %f y1 %f colour %d size %f %s\n",
		    xx1,yy1,
		    currentfgcolour, currentcharscale,text);
	graphData->Lines++;
    }
    else
	GraphText(xx1,yy1,1.0,0.0,0.0, text);

    return;
}



/* @obsolete ajGraphTextStart
** @rename ajGraphicsDrawTextAtstart
*/

__deprecated void ajGraphTextStart(PLFLT xx1, PLFLT yy1, const char *text)
{
    ajGraphicsDrawTextAtstart(xx1,yy1,text);
    return;
}


/* @func ajGraphicsDrawTextOncurve *********************************************
**
** Draw text along a curve (i.e., an arc of a circle).  The text is
** written character by character, forwards or backwards depending o n
** the angle.
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] Radius  [PLFLT] radius of the circle.
** @param  [r] StartAngle [PLFLT] angle of the start of the arc (in deg).
** @param  [r] EndAngle [PLFLT] angle of the end of the arc (in deg).
** @param  [r] Text [const char*] The text to be displayed.
** @param  [r] just [PLFLT] justification of the string.
**                          (0=left,1=right,0.5=middle etc)
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawTextOncurve(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
			    PLFLT StartAngle,
			    PLFLT EndAngle, const char *Text, PLFLT just)
{
    ajint numchar;
    PLFLT pos;

    numchar = strlen(Text);
    pos = (EndAngle-StartAngle)/numchar;

    if( ((StartAngle>180.0 && StartAngle<=360.0) &&
	 (EndAngle>180.0 && EndAngle<=360.0)) ||
       ((StartAngle>540.0 && StartAngle<=720.0)
	&& (EndAngle>540.0 && EndAngle<=720.0)) )
	GraphDrawTextOncurve(xcentre, ycentre, Radius, StartAngle,
			     +1*pos, Text, just);
    else GraphDrawTextOncurve(xcentre, ycentre, Radius, EndAngle,
			      -1*pos, Text, just);

    return;
}



/* @obsolete ajGraphDrawTextOnCurve
** @rename ajGraphicsDrawTextOncurve
*/

__deprecated void ajGraphDrawTextOnCurve(PLFLT xcentre, PLFLT ycentre,
                                         PLFLT Radius, PLFLT StartAngle,
                                         PLFLT EndAngle,
                                         const char *Text, PLFLT just)

{
    ajGraphicsDrawTextOncurve(xcentre, ycentre, Radius,
                              StartAngle, EndAngle, Text, just);
    return;
}


/* @func ajGraphicsDrawTextAtline **********************************************
**
** Draw text along a line.
**
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @param [r] xx2 [PLFLT] xx2 coor.
** @param [r] yy2 [PLFLT] yy2 coor.
** @param [r] text [const char*] The text to be displayed.
** @param [r] just [PLFLT] justification of the string.
**                         (0=left,1=right,0.5=middle etc)
** @return [void]
**
** @@
******************************************************************************/

void ajGraphicsDrawTextAtline(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
		     const char *text, PLFLT just)
{
    if(graphData)
    {
	ajFmtPrintF(graphData->File,
		    "Textline x1 %f y1 %f x2 %f y2 %f colour %d size %f %s\n",
		    xx1,yy1,xx2, yy2,
		    currentfgcolour, currentcharscale,text);
	graphData->Lines++;
    }
    else
	GraphText(xx1,yy1,xx2-xx1,yy2-yy1,just, text);

    return;
}



/* @obsolete ajGraphTextLine
** @rename ajGraphicsDrawTextAtline
*/

__deprecated void ajGraphTextLine(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
		     const char *text, PLFLT just)
{
    ajGraphicsDrawTextAtline(xx1, yy1, xx2, yy2, text, just);
    return;
}


/* @func ajGraphicsDrawTri *****************************************************
**
** Draw a Triangle.
**
** @param [r] xx1 [PLFLT] xx1 coord of point 1.
** @param [r] yy1 [PLFLT] yy1 coord of point 1.
** @param [r] xx2 [PLFLT] xx2 coord of point 2.
** @param [r] yy2 [PLFLT] yy2 coord of point 2.
** @param [r] x3 [PLFLT] x3 coord of point 3.
** @param [r] y3 [PLFLT] y3 coord of point 3.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawTri(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                    PLFLT x3, PLFLT y3)
{
    PLFLT x[4];
    PLFLT y[4];
    
    x[0]=xx1; x[1]=xx2; x[2]=x3; x[3]=xx1;
    y[0]=yy1; y[1]=yy2; y[2]=y3; y[3]=yy1;

    ajGraphicsDrawPoly(4, x, y);

    return;
}



/* @obsolete ajGraphTri
** @rename ajGraphicsDrawTri
*/

__deprecated void ajGraphTri(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
		PLFLT x3, PLFLT y3)
{
    ajGraphicsDrawTri(xx1,yy1,xx2,yy2,x3,y3);
    return;
}



/* @func ajGraphicsDrawTriFill *************************************************
**
** Draw a Triangle and fill it in.
**
** @param [r] xx1 [PLFLT] xx1 coord of point 1.
** @param [r] yy1 [PLFLT] yy1 coord of point 1.
** @param [r] xx2 [PLFLT] xx2 coord of point 2.
** @param [r] yy2 [PLFLT] yy2 coord of point 2.
** @param [r] x3 [PLFLT] x3 coord of point 3.
** @param [r] y3 [PLFLT] y3 coord of point 3.
** @return [void]
** @@
******************************************************************************/

void ajGraphicsDrawTriFill(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                           PLFLT x3, PLFLT y3)
{
    PLFLT x[3];
    PLFLT y[3];
    
    x[0]=xx1; x[1]=xx2; x[2]=x3;
    y[0]=yy1; y[1]=yy2; y[2]=y3;

    ajGraphicsDrawPolyFill(3, x, y);

    return;
}





/* @obsolete ajGraphTriFill
** @rename ajGraphicsDrawTriFill
*/

__deprecated void ajGraphTriFill(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                                 PLFLT x3, PLFLT y3)
{
    ajGraphicsDrawTriFill(xx1,yy1,xx2,yy2,x3,y3);
    return;
}

/* @func ajGraphicsCheckColourC ************************************************
**
** Find if the colour is on the list
**
** @param [r] text [const char*] colour name.
** @return [ajint] the colour number if found else -1.
** @@
******************************************************************************/

ajint ajGraphicsCheckColourC(const char* text)
{
    ajint i;

    for(i=0;i<=MAXCOL;i++)
	if(ajCharMatchCaseC(text,graphColourName[i]))
	    return i;

    return -1;
}


/* @func ajGraphicsCheckColourS ************************************************
**
** Find if the colour is on the list
**
** @param [r] colour [const AjPStr]  colour name.
** @return [ajint] the colour number if found else -1.
** @@
******************************************************************************/

ajint ajGraphicsCheckColourS(const AjPStr colour)
{
    ajint i;

    for(i=0;i<=MAXCOL;i++)
	if(ajStrMatchCaseC(colour,graphColourName[i]))
	    return i;

    return -1;
}


/* @obsolete ajGraphCheckColour
** @rename ajGraphicsCheckColourS
*/

__deprecated ajint ajGraphCheckColour(const AjPStr colour)
{
    return ajGraphicsCheckColourS(colour);
}



/* @obsolete ajGraphGetBaseColour
** @remove Use ajGraphicsBasecolourNewNuc or ajGraphicsBasecolourNewProt
*/

__deprecated ajint* ajGraphGetBaseColour(void)
{
    ajint *ret;

    ret = (ajint *) AJALLOC0(sizeof(ajint)*AZ);

    ret[0] = BLACK;			/* A */
    ret[1] = BLACK;			/* B */
    ret[2] = YELLOW;			/* C */
    ret[3] = RED;			/* D */
    ret[4] = RED;			/* E */
    ret[5] = WHEAT;			/* F */
    ret[6] = GREY;			/* G */
    ret[7] = BLUE;			/* H */
    ret[8] = BLACK;			/* I */
    ret[9] = BLACK;			/* J */
    ret[10] = BLUE;			/* K */
    ret[11] = BLACK;			/* L */
    ret[12] = YELLOW;			/* M */
    ret[13] = GREEN;			/* N */
    ret[14] = BLACK;			/* O */
    ret[15] = BLUEVIOLET;		/* P */
    ret[16] = GREEN;			/* Q */
    ret[17] = BLUE;			/* R */
    ret[18] = CYAN;			/* S */
    ret[19] = CYAN;			/* T */
    ret[20] = BLACK;			/* U */
    ret[21] = BLACK;			/* V */
    ret[22] = WHEAT;			/* W */
    ret[23] = BLACK;			/* X */
    ret[24] = WHEAT;			/* Y */
    ret[25] = BLACK;			/* Z */
    ret[26] = BLACK;			/* ? */
    ret[27] = BLACK;			/* ?*/

    return ret;
}



/* @func ajGraphicsBasecolourNewProt *******************************************
**
** Initialize a base colours array for a string of protein sequence characters
** according to the following colour scheme designed by Toby Gibson (EMBL
** Heidelberg) to show the chemical propoerties important in sequence
** and structure alignment and to be clear to those with common forms
** of colour blindness.
**
** DE: Red (Acidic)
** HKR: Blue (Basic)
** NQ: Green (Amide)
** AILV: Black (Hydrophobic Small)
** FWY: Wheat (Hydrophobic Large)
** ST: Cyan (Hydroxyl group)
** CM: Yellow (Sulphur chemistry)
** G: Grey (helix breaker I)
** P: Violet (helix breaker II)
**
** @param [r] codes [const AjPStr] Residue codes for each numbered position
** @return [ajint*] Array of colours (see PLPLOT)
** @@
******************************************************************************/

ajint* ajGraphicsBasecolourNewProt(const AjPStr codes)
{
    ajint *ret;
    const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ajint colours[] =
        {
	BLACK,BLACK,YELLOW,RED,RED,WHEAT, /* A-F */
	GREY,BLUE,BLACK,BLACK,BLUE,BLACK, /* G-L */
	YELLOW,GREEN,BLACK,BLUEVIOLET,GREEN,BLUE, /* M-R */
	CYAN,CYAN,BLACK,BLACK,WHEAT,BLACK, /* S-X */
	WHEAT,BLACK,BLACK,BLACK};	/* Y-Z plus 2 more */
    const char* cp;
    const char* cq;
    ajint i;
    ajint j;

    ret = (ajint *) AJALLOC0(sizeof(ajint)*AZ);	/* BLACK is zero */

    ajDebug("ajGraphGetBaseColourProt '%S'\n", codes);

    cp = ajStrGetPtr(codes);
    i = 1;

    while(*cp)
    {
	cq = strchr(alphabet, *cp);

	if(cq)
	{
	    j = cq - alphabet;
	    ret[i] = colours[j];
	    ajDebug("ColourProt %d: '%c' %d -> %d %s\n",
		    i, *cp, j, colours[j], graphColourName[colours[j]]);
	}

	i++;
	cp++;
    }

    return ret;
}



/* @obsolete ajGraphGetBaseColourProt
** @rename ajGraphicsBasecolourNewProt
*/

__deprecated ajint* ajGraphGetBaseColourProt(const AjPStr codes)
{
    return ajGraphicsBasecolourNewProt(codes);
}

/* @func ajGraphicsBasecolourNewNuc ********************************************
**
** Initialize a base colours array for a string of nucleotide
** sequence characters
**
** @param [r] codes [const AjPStr] Residue codes for each numbered position
** @return [ajint*] Array of colours (see PLPLOT)
** @@
******************************************************************************/

ajint* ajGraphicsBasecolourNewNuc(const AjPStr codes)
{
    ajint *ret;
    const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    ajint colours[] =
        {
	GREEN,YELLOW,BLUE,YELLOW,YELLOW,YELLOW,
	BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,
	YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,
	YELLOW,RED,RED,YELLOW,YELLOW,YELLOW,
	YELLOW,YELLOW,YELLOW
    };
    const char* cp;
    const char* cq;
    ajint i;
    ajint j;

    ret = (ajint *) AJALLOC0(sizeof(ajint)*AZ);	/* BLACK is zero */

    ajDebug("ajGraphGetBaseColourProt '%S'\n", codes);

    cp = ajStrGetPtr(codes);
    i = 1;

    while(*cp)
    {
	cq = strchr(alphabet, *cp);

	if(cq)
	{
	    j = cq - alphabet;
	    ret[i] = colours[j];
	    ajDebug("ColourProt %d: '%c' %d -> %d %s\n",
		    i, *cp, j, colours[j], graphColourName[colours[j]]);
	}

	i++;
	cp++;
    }

    return ret;
}




/* @obsolete ajGraphGetBaseColourNuc
** @rename ajGraphicsBasecolourNewNuc
*/

__deprecated ajint* ajGraphGetBaseColourNuc(const AjPStr codes)
{
    return ajGraphicsBasecolourNewNuc(codes);
}

/* @func ajGraphicsGetCharsize *************************************************
**
** Get the char size.
**
** @param [u] defheight [float *] where to store the default character height
** @param [u] currentscale [float *] where to store the current (scaled)
**                                    character height
**
** @return [void]
** @@
******************************************************************************/

void ajGraphicsGetCharsize(float *defheight, float *currentscale)
{
    if(graphData)
    {
	*defheight = currentcharsize;
	*currentscale = currentcharscale;
	ajDebug("=g= plgchr(&f &f) [defht:%f, scale:%f] copy\n",
		*defheight, *currentscale);
    }
    else
    {
	plgchr(defheight,currentscale);
	ajDebug("=g= plgchr(&f &f) [defht:%f, scale:%f] plplot\n",
		*defheight, *currentscale);
    }

    return;
}



/* @obsolete ajGraphGetCharSize
** @rename ajGraphicsGetCharsize
*/

__deprecated void ajGraphGetCharSize(float *defheight, float *currentheight)
{
    ajGraphicsGetCharsize(defheight, currentheight);
    return;
}


/* @func ajGraphGetParamsPage **************************************************
**
** Get the output page parameters
**
** For graph data type, sets to zero as these are not applicable.
**
** @param [r] thys [const AjPGraph] Graph object.
** @param [u] xp [float *] where to store the x position
** @param [u] yp [float *] where to store the y position
** @param [u] xleng [ajint *] where to store the x length
** @param [u] yleng [ajint *] where to the y length
** @param [u] xoff [ajint *] where to store the x offset
** @param [u] yoff [ajint *] where to sore the y offset
**
** @return [void]
** @@
******************************************************************************/

void ajGraphGetParamsPage(const AjPGraph thys,
                          float *xp,float *yp, ajint *xleng, ajint *yleng,
                          ajint *xoff, ajint *yoff)
{
    ajDebug("=g= plgpage(&f &f) [&xp, &yp, &xleng, &yleng, &xoff, &yoff]\n");

    if(graphData)
    {
	*xp=0.0;
	*yp=0.0;
	*xleng=graphType[thys->displaytype].Width;
	*yleng=graphType[thys->displaytype].Height;
	*xoff=0;
	*yoff=0;
    }
    else
    {
	plgpage(xp,yp,xleng,yleng,xoff,yoff);
        if(!*xleng)
            *xleng = graphType[thys->displaytype].Width;
        if(!*yleng)
            *yleng = graphType[thys->displaytype].Height;
	ajDebug("returns xp:%.3f yp:%.3f xleng:%d yleng:%d xoff:%d yoff:%d\n",
		 *xp, *yp, *xleng, *yleng, *xoff, *yoff);
    }

    return;
}


/* @func ajGraphicsGetParamsPage ***********************************************
**
** Get the output page parameters
**
** For graph data type, sets to zero as these are not applicable.
**
** @param [u] xp [float *] where to store the x position
** @param [u] yp [float *] where to store the y position
** @param [u] xleng [ajint *] where to store the x length
** @param [u] yleng [ajint *] where to the y length
** @param [u] xoff [ajint *] where to store the x offset
** @param [u] yoff [ajint *] where to sore the y offset
**
** @return [void]
** @@
******************************************************************************/

void ajGraphicsGetParamsPage(float *xp,float *yp, ajint *xleng, ajint *yleng,
                             ajint *xoff, ajint *yoff)
{
    ajDebug("=g= plgpage(&f &f) [&xp, &yp, &xleng, &yleng, &xoff, &yoff]\n");

    if(graphData)
    {
	*xp=0.0;
	*yp=0.0;
	*xleng=0;
	*yleng=0;
	*xoff=0;
	*yoff=0;
    }
    else
    {
	plgpage(xp,yp,xleng,yleng,xoff,yoff);
	ajDebug("returns xp:%.3f yp:%.3f xleng:%d yleng:%d xoff:%d yoff:%d\n",
		 *xp, *yp, *xleng, *yleng, *xoff, *yoff);
    }

    return;
}


/* @obsolete ajGraphGetOut
** @rename ajGraphicsGetParamsPage
*/

__deprecated void ajGraphGetOut(float *xp,float *yp, ajint *xleng, ajint *yleng,
		   ajint *xoff, ajint *yoff)

{

    ajGraphicsGetParamsPage(xp, yp, xleng, yleng, xoff, yoff);
    return;
}



/* @func ajGraphPlenv *********************************************************
**
** Defines a plot environment. i.e. tells plplot whether the graph is boxed,
** whether it has tick marks, whether it has labels etc. These should already
** be set in the flags.
**
** @param [r] xmin [float] X axis start
** @param [r] xmax [float] X axis end
** @param [r] ymin [float] Y axis start
** @param [r] ymax [float] Y axis end
** @param [r] flags [ajint] flag bit settings
** @return [void]
** @@
******************************************************************************/

void ajGraphPlenv(float xmin, float xmax, float ymin, float ymax,
		  ajint flags)
{
    char xopt[15] = " ";
    char yopt[15] = " ";
    ajuint i = 0;
    ajuint j = 0;

    /* ignore 'a' axis at zero value */
    /* ignore 'd' label plot as date/time */
    /* ignore 'f' fixed point labels */
    /* ignore 'h' minor interval grid */
    /* ignore 'l' logarithmic axis */

    ajDebug("ajGraphPlenv(%.3f, %.3f, %.3f, %.3f, flags:%x)\n",
	     xmin, xmax, ymin, ymax, flags);
    GraphSubPage(0);
    GraphSetWin2(xmin,xmax,ymin,ymax);
    
    if(flags & AJGRAPH_X_BOTTOM)
	xopt[i++] = 'b';
    
    if(flags & AJGRAPH_Y_LEFT)
	yopt[j++] = 'b';
    
    if(flags & AJGRAPH_X_TOP)
	xopt[i++] = 'c';
    
    if(flags & AJGRAPH_Y_RIGHT)
	yopt[j++] = 'c';
    
    if(flags & AJGRAPH_X_TICK)
    {
	xopt[i++] = 't';		/* do ticks */
	xopt[i++] = 's';		/* do subticks */

	if(flags & AJGRAPH_X_INVERT_TICK)
	    xopt[i++] = 'i';

	if(flags & AJGRAPH_X_NUMLABEL_ABOVE)
	    xopt[i++] = 'm';
	else
	    xopt[i++] = 'n';		/* write numeric labels */
    }
    
    if(flags & AJGRAPH_Y_TICK)
    {
	yopt[j++] = 't';
	yopt[j++] = 's';

	if(flags & AJGRAPH_Y_INVERT_TICK)
	    yopt[j++] = 'i';

	if(flags & AJGRAPH_Y_NUMLABEL_LEFT)
	    yopt[j++] = 'm';
	else
	    yopt[j++] = 'n';
    }
    
    if(flags & AJGRAPH_X_GRID)
	xopt[i++] = 'g';
    
    if(flags & AJGRAPH_Y_GRID)
	yopt[j++] = 'g';

    /* ignore minor interval grid option 'h' */

    xopt[i] = '\0';
    yopt[j] = '\0';

    ajDebug("=g= plbox('%s', 0.0, 0, '%s', 0.0, 0) "
	     "[xopt, 0.0, 0, yopt, 0.0, 0]\n", xopt, yopt);
    plbox(xopt, 0.0, 0, yopt, 0.0, 0);
    
    return;
}




/* @funcstatic GraphCheckFlags ************************************************
**
** Prints the flags defined by bits in the input integer value.
**
** @param [r] flags [ajint] flag bits
** @return [void]
** @@
******************************************************************************/

static void GraphCheckFlags(ajint flags)
{
    ajFmtPrint("flag = %d\n",flags);
    if(flags & AJGRAPH_X_BOTTOM)
	ajFmtPrint("AJGRAPH_X_BOTTOM \n");

    if(flags & AJGRAPH_Y_LEFT)
	ajFmtPrint("AJGRAPH_Y_LEFT \n");

    if(flags & AJGRAPH_X_TOP)
	ajFmtPrint("AJGRAPH_X_TOP \n");

    if(flags & AJGRAPH_Y_RIGHT)
	ajFmtPrint(" AJGRAPH_Y_RIGHT\n");

    if(flags & AJGRAPH_X_TICK)
	ajFmtPrint("AJGRAPH_X_TICK \n");

    if(flags & AJGRAPH_Y_TICK)
	ajFmtPrint("AJGRAPH_Y_TICK \n");

    if(flags & AJGRAPH_X_LABEL)
	ajFmtPrint("AJGRAPH_X_LABEL \n");

    if(flags & AJGRAPH_Y_LABEL)
	ajFmtPrint("AJGRAPH_Y_LABEL \n");

    if(flags & AJGRAPH_TITLE)
	ajFmtPrint("AJGRAPH_TITLE \n");

    if(flags & AJGRAPH_JOINPOINTS)
	ajFmtPrint("AJGRAPH_JOINPOINTS \n");

    if(flags & AJGRAPH_OVERLAP)
	ajFmtPrint("AJGRAPH_OVERLAP \n");

    if(flags & AJGRAPH_Y_NUMLABEL_LEFT)
	ajFmtPrint("AJGRAPH_Y_NUMLABEL_LEFT");

    if(flags & AJGRAPH_Y_INVERT_TICK)
	ajFmtPrint("AJGRAPH_Y_INVERT_TICK");

    if(flags & AJGRAPH_Y_GRID)
	ajFmtPrint("AJGRAPH_Y_GRID");

    if(flags & AJGRAPH_X_NUMLABEL_ABOVE)
	ajFmtPrint("AJGRAPH_X_NUMLABEL_ABOVE");

    if(flags & AJGRAPH_X_INVERT_TICK)
	ajFmtPrint("AJGRAPH_X_INVERT_TICK");

    if(flags & AJGRAPH_X_GRID)
	ajFmtPrint("AJGRAPH_X_GRID");
    
    return;
}




/* @func ajGraphicsGetColourFore ***********************************************
**
** Return current foreground colour
**
** @return [ajint] colour.
** @@
******************************************************************************/

ajint ajGraphicsGetColourFore(void)
{
    return currentfgcolour;
}





/* @obsolete ajGraphGetColour
** @rename ajGraphicsGetColourFore
*/

__deprecated ajint ajGraphGetColour(void)
{
    return ajGraphicsGetColourFore();
}




/* @func ajGraphGetSubTitle **************************************************
**
** Return plot subtitle
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const AjPStr] Subtitle
** @@
******************************************************************************/

const AjPStr ajGraphGetSubTitle(const AjPGraph thys)
{
    return thys->subtitle;

}




/* @func ajGraphGetSubTitleC **************************************************
**
** Return plot subtitle
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const char*] Subtitle
** @@
******************************************************************************/

const char* ajGraphGetSubTitleC(const AjPGraph thys)
{
    if (thys->subtitle)
	    return ajStrGetPtr(thys->subtitle);

    return "";
}




/* @func ajGraphGetTitle *****************************************************
**
** Return plot title
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const AjPStr] Title
** @@
******************************************************************************/

const AjPStr ajGraphGetTitle(const AjPGraph thys)
{
    return thys->title;
}




/* @func ajGraphGetTitleC *****************************************************
**
** Return plot title
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const char*] Title
** @@
******************************************************************************/

const char* ajGraphGetTitleC(const AjPGraph thys)
{
    if(thys->title)
        return ajStrGetPtr(thys->title);

    return "";
}




/* @func ajGraphGetXTitle *****************************************************
**
** Return plot x-axis title
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const AjPStr] Title of x-axis
** @@
******************************************************************************/

const AjPStr ajGraphGetXTitle(const AjPGraph thys)
{
    return thys->xaxis;
}




/* @func ajGraphGetXTitleC ****************************************************
**
** Return plot x-axis title
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const char*] Title of x-axis
** @@
******************************************************************************/

const char* ajGraphGetXTitleC(const AjPGraph thys)
{
    if (thys->xaxis)
        return ajStrGetPtr(thys->xaxis);

    return "";
}




/* @func ajGraphGetYTitle *****************************************************
**
** Return plot y-axis title
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const AjPStr] Title of y-axis
** @@
******************************************************************************/

const AjPStr ajGraphGetYTitle(const AjPGraph thys)
{
    return thys->yaxis;
}




/* @func ajGraphGetYTitleC ************************************************
**
** Return plot y-axis title
**
** @param [r] thys [const AjPGraph] Graph object.
** @return [const char*] Title of y-axis
** @@
******************************************************************************/

const char* ajGraphGetYTitleC(const AjPGraph thys)
{
    if (thys->yaxis)
        return ajStrGetPtr(thys->yaxis);

    return "";
}





/* @funcstatic GraphCheckPoints ***********************************************
**
** Prints a list of data points from two floating point arrays.
**
** @param [r] n [ajint] Number of points to print
** @param [r] x [const PLFLT*] X coordinates
** @param [r] y [const PLFLT*] Y coordinates
** @return [void]
** @@
******************************************************************************/

static void GraphCheckPoints(ajint n, const PLFLT *x, const PLFLT *y)
{
    ajint i;

    for(i=0;i<n;i++)
	ajFmtPrint("%d %f %f\n",i,x[i],y[i]);

    return;
}




/* @func ajGraphInitSeq *******************************************************
**
** Initialises a graph using default values based on a sequence.
**
** Existing titles and other data are unchanged
**
** @param [u] thys [AjPGraph] Graph
** @param [r] seq [const AjPSeq] Sequence
** @return [void]
** @@
******************************************************************************/

void ajGraphInitSeq(AjPGraph thys, const AjPSeq seq)
{
    if(!ajStrGetLen(thys->title))
        ajFmtPrintS(&thys->title, "%S of %S",
                    ajUtilGetProgram(), ajSeqGetNameS(seq));

    ajGraphxySetXrangeII(thys, 1, ajSeqGetLen(seq));

    return;
}




/* @funcstatic GraphOpenFile **************************************************
**
** A general routine for setting BaseName and extension in plplot.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphOpenFile(AjPGraph thys, const char *ext)
{
    ajDebug("GraphOpenFile '%S'\n", thys->outputfile);

    GraphSetName(thys, thys->outputfile,ext);

    return;
}




/* @funcstatic GraphOpenData **************************************************
**
** A general routine for setting BaseName and extension in plplot
** for -graph data output.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphOpenData(AjPGraph thys, const char *ext)
{
    ajDebug("GraphOpenData '%S' '%s\n", thys->outputfile, ext);

    if(!graphData)
    {
        AJNEW0(graphData);
        graphData->List = ajListstrNew();
    }

    GraphSetName(thys, thys->outputfile,ext);
    thys->isdata = ajTrue;

    ajStrAssignS(&graphData->FName, thys->outputfile);
    ajStrAssignC(&graphData->Ext, ext);

    GraphDatafileNext();

    return;
}




/* @funcstatic GraphOpenSimple ************************************************
**
** A general routine for drawing graphs. Does nothing.
**
** This is called when initialised. Graph files are opened when
** each graph is written.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphOpenSimple(AjPGraph thys, const char *ext )
{
    (void) thys;			/* make it used */
    (void) ext;				/* make it used */

    return;
}




#ifndef X_DISPLAY_MISSING
/* @funcstatic GraphOpenXwin **************************************************
**
** A general routine for drawing graphs to an xwin. Only creates the window.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphOpenXwin(AjPGraph thys, const char *ext )
{
    const char* programname = NULL;

    (void) thys;			/* make it used */
    (void) ext;				/* make it used */

    programname = ajStrGetPtr(ajUtilGetProgram());
    ajDebug("=g= plxswin('%s') ajUtilGetProgram\n", programname);
    plxswin(programname);

/*
    GraphLabelTitle(((thys->flags & AJGRAPH_TITLE) ?
		     ajStrGetPtr(thys->title) : " "),
		    ((thys->lags & AJGRAPH_SUBTITLE) ?
		     ajStrGetPtr(thys->subtitle) : " "));
	
*/

    return;
}
#endif





/* @funcstatic GraphxyDisplayToFile *******************************************
**
** A general routine for drawing graphs to a file.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] closeit [AjBool] Close file if true
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphxyDisplayToFile(AjPGraph thys, AjBool closeit,
				 const char *ext)
{
    	ajDebug("GraphxyDisplayToFile '%S'\n", thys->outputfile);

	GraphSetName(thys, thys->outputfile,ext);
	ajGraphxyCheckMaxMin(thys);
	GraphxyGeneral(thys, closeit);

    return;
}




/* @funcstatic GraphxyDisplayToDas ********************************************
**
** A general routine for drawing graphs to DAS output file as points.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] closeit [AjBool] Close file if true
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphxyDisplayToDas(AjPGraph thys, AjBool closeit,
                                const char *ext)
{
    AjPFile outf    = NULL;
    AjPGraphdata graphdata  = NULL;
    AjPStr temp;
    AjPTime ajtime;
    ajint i,j;
    float minxa = 64000.;
    float minya = 64000.;
    float maxxa = -64000.;
    float maxya = -64000.;
    ajint nfeat = 0;
    ajint istart;
    ajint iend;

    /*
    ** Things to do:
    **
    ** Test this output
    ** Check how it handles multiple sequences
    ** Try to catch the sequnce information
    **
    ** Do we want end=start or end=start+1
    **
    */

     ajDebug("GraphxyDisplayToDas '%S' '%s' %d graphs\n",
	     thys->outputfile, ext, thys->numofgraphs);
        
    if(!graphData)
    {
	AJNEW0(graphData);
	graphData->List = ajListstrNew();
    }

    /* Calculate maxima and minima */
    for(i=0;i<thys->numofgraphs;i++)
    {
	graphdata = (thys->graphs)[i];
	minxa = (minxa<graphdata->minX) ? minxa : graphdata->minX;
	minya = (minya<graphdata->minY) ? minya : graphdata->minY;
	maxxa = (maxxa>graphdata->maxX) ? maxxa : graphdata->maxX;
	maxya = (maxya>graphdata->maxY) ? maxya : graphdata->maxY;
    }
    
    ajFmtPrintF(outf,"<DASGFF>\n");
    ajFmtPrintF(outf,"  <GFF version=\"1.0\" href=\"url\">\n");
    ajFmtPrintF(outf,"<!DOCTYPE DASGFF SYSTEM \"http://www.biodas.org/dtd/dasgff.dtd\">\n");
    ajFmtPrintF(outf,"<DASGFF>\n");
    ajFmtPrintF(outf,"  <GFF version=\"1.0\" href=\"url\">\n");

    ajFmtPrintF(outf,"<!-- Title %S -->\n", thys->title);
    ajFmtPrintF(outf,"<!-- Graphs %d -->\n",thys->numofgraphs);
    ajFmtPrintF(outf,"<!-- Number %d -->\n",i+1);
    ajFmtPrintF(outf,"<!-- Points %d -->\n",graphdata->numofpoints);
	
    for(i=0;i<thys->numofgraphs;i++)
    {
	graphdata = (thys->graphs)[i];
	
	/* open a file for dumping the data points */
	temp = ajFmtStr("%S%d%s",thys->outputfile,i+1,ext);
	outf = ajFileNewOutNameS(temp);
	ajListstrPushAppend(graphData->List, temp);
	if(!outf)
	{
	    ajErr("Could not open graph file %S\n",temp);
	    return;
	}
	else
	    ajDebug("Writing graph %d data to %S\n",i+1,temp);

	if( ajStrGetLen(thys->title) <=1)
	{
	    ajFmtPrintAppS(&thys->title,"%S",
			   ajUtilGetProgram());
	}
	if( ajStrGetLen(thys->subtitle) <=1)
	{
	    ajtime = ajTimeNewTodayFmt("report");
	    ajFmtPrintAppS(&thys->subtitle,"%D",
			   ajtime);
	    ajTimeDel(&ajtime);
	}

        istart = (ajint) graphdata->minX;
        if(istart < 1)
            istart = 1;
        iend = (ajint) graphdata->maxX;
        if(iend < istart)
            iend = istart;
	ajFmtPrintF(outf,"    <SEGMENT id=\"%s\" start=\"%d\" stop=\"%d\"\n",
                    "graphid",
                    istart,
                    iend);
        ajFmtPrintF(outf,
                    "                version=\"%s\">\n",
                    "0.0");

	
	/* Dump out the data points */
	for(j=0;j<graphdata->numofpoints;j++)
        {
            nfeat++;
	    ajFmtPrintF(outf,"      <FEATURE id=\"%s.%d\">\n",
                        "featid", nfeat);
            ajFmtPrintF(outf,"        <START>%d</START>\n",
                        (ajint) graphdata->x[j]);
            ajFmtPrintF(outf,"        <END>%d</END>\n",
                        (ajint) graphdata->x[j]);
            ajFmtPrintF(outf,"        <SCORE>%f</SCORE>\n",
                        graphdata->y[j]);
	    ajFmtPrintF(outf,"      </FEATURE>\n");
	}
        
	
	ajFmtPrintF(outf,"    <SEGMENT>\n");
    }
    
    ajFmtPrintF(outf,
                "  </GFF>\n");
    ajFmtPrintF(outf,
                "</DASGFF>\n");

    ajFileClose(&outf);
    if(closeit)
	GraphClose();

    return;
}




/* @funcstatic GraphxyDisplayToData *******************************************
**
** A general routine for drawing graphs to file as points.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] closeit [AjBool] Close file if true
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphxyDisplayToData(AjPGraph thys, AjBool closeit,
				 const char *ext)
{
    AjPFile outf    = NULL;
    AjPGraphdata graphdata  = NULL;
    AjPGraphobj ptr = NULL;
    AjPStr temp;
    AjPTime ajtime;
    ajint i,j;
    float minxa = 64000.;
    float minya = 64000.;
    float maxxa = -64000.;
    float maxya = -64000.;
    ajint type  = 0;
    
    ajDebug("GraphxyDisplayToData '%S' '%s' %d graphs\n",
	     thys->outputfile, ext, thys->numofgraphs);
        
    if(!graphData)
    {
	AJNEW0(graphData);
	graphData->List = ajListstrNew();
    }

    /* Calculate maxima and minima */
    for(i=0;i<thys->numofgraphs;i++)
    {
	graphdata = (thys->graphs)[i];
	minxa = (minxa<graphdata->minX) ? minxa : graphdata->minX;
	minya = (minya<graphdata->minY) ? minya : graphdata->minY;
	maxxa = (maxxa>graphdata->maxX) ? maxxa : graphdata->maxX;
	maxya = (maxya>graphdata->maxY) ? maxya : graphdata->maxY;
    }
    
    for(i=0;i<thys->numofgraphs;i++)
    {
	graphdata = (thys->graphs)[i];
	
	/* open a file for dumping the data points */
	temp = ajFmtStr("%S%d%s",thys->outputfile,i+1,ext);
	outf = ajFileNewOutNameS(temp);
	ajListstrPushAppend(graphData->List, temp);

	if(!outf)
	{
	    ajErr("Could not open graph file %S\n",temp);

	    return;
	}
	else
	    ajDebug("Writing graph %d data to %S\n",i+1,temp);

	if( ajStrGetLen(thys->title) <=1)
	{
	    ajFmtPrintAppS(&thys->title,"%S",
			   ajUtilGetProgram());
	}

	if( ajStrGetLen(thys->subtitle) <=1)
	{
	    ajtime = ajTimeNewTodayFmt("report");
	    ajFmtPrintAppS(&thys->subtitle,"%D",
			   ajtime);
	    ajTimeDel(&ajtime);
	}

	ajFmtPrintF(outf,"##%S\n",graphdata->gtype);
	ajFmtPrintF(outf,"##Title %S\n",thys->title);
	ajFmtPrintF(outf,"##Graphs %d\n",thys->numofgraphs);
	ajFmtPrintF(outf,"##Number %d\n",i+1);
	ajFmtPrintF(outf,"##Points %d\n",graphdata->numofpoints);
	
	
	ajFmtPrintF(outf,"##XminA %f XmaxA %f YminA %f YmaxA %f\n",
			   minxa,maxxa,minya,maxya);
	ajFmtPrintF(outf,"##Xmin %f Xmax %f Ymin %f Ymax %f\n",
		    graphdata->tminX,graphdata->tmaxX,
		    graphdata->tminY,graphdata->tmaxY);
	ajFmtPrintF(outf,"##ScaleXmin %f ScaleXmax %f ScaleYmin %f "
			   "ScaleYmax %f\n",
		    graphdata->minX,graphdata->maxX,
		    graphdata->minY,graphdata->maxY);
	
	ajFmtPrintF(outf,"##Maintitle %S\n",graphdata->title);
	
	if(thys->numofgraphs == 1)
	{
	    ajFmtPrintF(outf,"##Xtitle %S\n",thys->xaxis);
	    ajFmtPrintF(outf,"##Ytitle %S\n",thys->yaxis);
	}
	else
	{
	    if(graphdata->subtitle)
	        ajFmtPrintF(outf,"##Subtitle %S\n",graphdata->subtitle);
	    ajFmtPrintF(outf,"##Xtitle %S\n",graphdata->xaxis);
	    ajFmtPrintF(outf,"##Ytitle %S\n",graphdata->yaxis);
	}
	
	/* Dump out the data points */
	for(j=0;j<graphdata->numofpoints;j++)
	    ajFmtPrintF(outf,"%f\t%f\n",graphdata->x[j],graphdata->y[j]);
	
	
	/* Now for the Data graphobjs */
	ajFmtPrintF(outf,"##DataObjects\n##Number %d\n",
		    graphdata->numofobjects);
	
	if(graphdata->numofobjects)
	{
	    ptr = graphdata->Dataobj;

	    for(j=0;j<graphdata->numofobjects;++j)
	    {
		type = ptr->type;

		if(type==LINE || type==RECTANGLE || type==RECTANGLEFILL)
		{
		    if(type==LINE)
			ajFmtPrintF(outf,"Line ");
		    else if(type==RECTANGLE)
			ajFmtPrintF(outf,"Rectangle ");
		    else
			ajFmtPrintF(outf,"Filled Rectangle ");
		    ajFmtPrintF(outf,"x1 %f y1 %f x2 %f y2 %f colour %d\n",
				ptr->xx1,ptr->yy1,
				ptr->xx2,ptr->yy2,ptr->colour);
		}
		else if(type==TEXT)
		{
		    ajFmtPrintF(outf,"Textline ");
		    ajFmtPrintF(outf,"x1 %f y1 %f x2 %f y2 %f colour %d "
				"size 1.0 %S\n",
				ptr->xx1,ptr->yy1,ptr->xx2,
				ptr->yy2,ptr->colour,
				ptr->text);
		}

		ptr = ptr->next;
	    }
	}
	
	
	/* Now for the Graph graphobjs */
	ajFmtPrintF(outf,"##GraphObjects\n##Number %d\n",
		    thys->numofobjects);
	
	if(thys->numofobjects)
	{
	    ptr = thys->Mainobj;

	    for(j=0;j<thys->numofobjects;++j)
	    {
		type = ptr->type;

		if(type==LINE || type==RECTANGLE || type==RECTANGLEFILL)
		{
		    if(type==LINE)
			ajFmtPrintF(outf,"Line ");
		    else if(type==RECTANGLE)
			ajFmtPrintF(outf,"Rectangle ");
		    else
			ajFmtPrintF(outf,"Filled Rectangle ");

		    ajFmtPrintF(outf,"x1 %f y1 %f x2 %f y2 %f colour %d\n",
				ptr->xx1,ptr->yy1,
				ptr->xx2,ptr->yy2,ptr->colour);
		}
		else if(type==TEXT)
		{
		    ajFmtPrintF(outf,"Textline ");
		    ajFmtPrintF(outf,"x1 %f y1 %f x2 %f y2 %f colour %d "
				"size 1.0 %S\n",
				ptr->xx1,ptr->yy1,
				ptr->xx2,ptr->yy2,ptr->colour,
				ptr->text);
		}

		ptr = ptr->next;
	    }
	}
	
	
	ajFileClose(&outf);
    }
    
    if(closeit)
	GraphClose();

    return;
}




/* @funcstatic GraphxyDisplayXwin *********************************************
**
** A general routine for drawing graphs to an xwin.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] closeit [AjBool] Close file if true
** @param [r] ext [const char*] file extension
** @return [void]
** @@
******************************************************************************/

static void GraphxyDisplayXwin(AjPGraph thys, AjBool closeit, const char *ext )
{
    (void) ext;				/* make it used */

    ajGraphxyCheckMaxMin(thys);
    GraphxyGeneral(thys, closeit);

    return;
}





/* @func ajGraphxyDel *********************************************************
**
** Destructor for a graph object
**
** @param [w] pthis [AjPGraph*] Graph structure to store info in.
** @return [void]
** @@
******************************************************************************/

void ajGraphxyDel(AjPGraph* pthis)
{
    AjPGraphdata graphdata;
    AjPGraph thys;
    ajint i;

    if(!pthis)
	return;

    if(!*pthis)
	return;

    thys = *pthis;

    ajDebug("ajGraphxyDel numofgraphs:%d\n",
            thys->numofgraphs);

    for(i = 0 ; i < thys->numofgraphs ; i++)
    {
        graphdata = (thys->graphs)[i];

        if (graphdata)
        {
            ajDebug("ajGraphxyDel graphs[%d] xcalc:%B ycalc:%B x:%x y:%x\n",
                    i, graphdata->xcalc, graphdata->ycalc,
                    graphdata->x, graphdata->y);

            if(!graphdata->xcalc)
                AJFREE(graphdata->x);
            if(!graphdata->ycalc)
                AJFREE(graphdata->y);
            if(!graphdata->gtype)
                ajStrDel(&graphdata->gtype);
            ajGraphdataDel(&graphdata);
        }
    }

    ajStrDel(&thys->desc);
    ajStrDel(&thys->title);
    ajStrDel(&thys->subtitle);
    ajStrDel(&thys->xaxis);
    ajStrDel(&thys->yaxis);
    ajStrDel(&thys->outputfile);

    ajGraphClear(thys);

    AJFREE(thys->graphs);

    AJFREE(thys);

    *pthis = NULL;

    return;
}




/* @funcstatic GraphdataInit ***********************************************
**
** Initialise the contents of a graph object
**
** @param [w] graphdata  [AjPGraphdata] Graph structure to store info in.
** @return [void]
** @@
******************************************************************************/

static void GraphdataInit(AjPGraphdata graphdata)
{
    ajDebug("GraphdataInit title: %x subtitle: %x xaxis: %x yaxis: %x\n",
	    graphdata->title, graphdata->subtitle,
	    graphdata->xaxis, graphdata->yaxis);

    graphdata->x = NULL;
    graphdata->y = NULL;
    graphdata->xcalc = ajTrue;
    graphdata->ycalc = ajTrue;
    ajStrAssignEmptyC(&graphdata->title,"");
    ajStrAssignEmptyC(&graphdata->subtitle,"");
    ajStrAssignEmptyC(&graphdata->xaxis,"");
    ajStrAssignEmptyC(&graphdata->yaxis,"");
    ajStrAssignEmptyC(&graphdata->gtype,"");
    graphdata->minX = graphdata->maxX = 0;
    graphdata->minY = graphdata->maxY = 0;
    graphdata->lineType = 1;
    graphdata->colour = BLACK;
    graphdata->Dataobj = 0;

    return;
}




/* @func ajGraphdataNew ****************************************************
**
** Creates a new empty graph
**
** @return [AjPGraphdata] New empty graph
** @@
******************************************************************************/

AjPGraphdata ajGraphdataNew(void)
{
    static AjPGraphdata graphdata;

    AJNEW0(graphdata);
    graphdata->numofpoints = 0;

    return graphdata;
}




/* @func ajGraphdataSetXY ***********************************************
**
** Adds (x,y) data points defined in two floating point arrays
**
** @param [u] graphdata [AjPGraphdata] Graph object
** @param [r] x [const float*] X coordinates
** @param [r] y [const float*] Y coordinates
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetXY(AjPGraphdata graphdata,
			 const float *x,const float *y)
{
    ajint i;

    for(i=0;i<graphdata->numofpoints; i++)
    {
	graphdata->x[i] = x[i];
	graphdata->y[i] = y[i];
    }

    return;
}




/* @func ajGraphdataCalcXY **********************************************
**
** Adds (x,y) data points defined by an x-axis start and increment
** and a floating point array of y-axis values.
**
** @param [u] graphdata [AjPGraphdata] Graph object
** @param [r] numofpoints [ajint] Number of points in array
** @param [r] start [float] Start position
** @param [r] incr [float] Increment
** @param [r] y [const float*] Y coordinates
** @return [void]
** @@
******************************************************************************/

void ajGraphdataCalcXY(AjPGraphdata graphdata, ajint numofpoints,
			      float start, float incr, const float* y)
{
    ajint i;
    PLFLT x = 0.0;

    if(!graphdata->xcalc)
	AJFREE(graphdata->x);

    if(!graphdata->ycalc)
	AJFREE(graphdata->y);

    graphdata->xcalc = ajFalse;	 /* i.e. OK to delete at the end as it */
    graphdata->ycalc = ajFalse;	 /* is our own copy */

    AJCNEW(graphdata->x, numofpoints);
    AJCNEW(graphdata->y, numofpoints);

    for(i=0;i<numofpoints; i++)
    {
	graphdata->x[i] = (start+x);
	graphdata->y[i] = y[i];
	x += incr;
    }

    graphdata->numofpoints = numofpoints;

    return;
}




/* @func ajGraphdataNewI ***************************************************
**
** Create and initialise the data structure for the graph with a defined
** number of data points.
**
** @param [r] numofpoints [ajint] Number of points
** @return [AjPGraphdata] Pointer to new graph structure.
** @@
******************************************************************************/

AjPGraphdata ajGraphdataNewI(ajint numofpoints)
{
    AjPGraphdata graphdata;

    AJNEW0(graphdata);

    GraphdataInit(graphdata);

    graphdata->numofpoints = numofpoints;

    graphdata->xcalc = ajFalse;
    AJCNEW0(graphdata->x, numofpoints);

    graphdata->ycalc = ajFalse;
    AJCNEW0(graphdata->y, numofpoints);

    return graphdata;
}




/* @func ajGraphDataAdd ****************************************************
**
** Add another graph structure to the multiple graph structure.
**
** The graphdata now belongs to the graph - do not delete it while
** the graph is using it.
**
** @param [u] thys [AjPGraph] multiple graph structure.
** @param [u] graphdata [AjPGraphdata] graph to be added.
** @return [ajint] 1 if graph added successfully else 0;
** @@
******************************************************************************/

ajint ajGraphDataAdd(AjPGraph thys, AjPGraphdata graphdata)
{
    if(thys->numofgraphs)
    {
        ajDebug("ajGraphDataAdd multi \n");
    }

    if(thys->numofgraphs < thys->numofgraphsmax)
    {
        (thys->graphs)[thys->numofgraphs++] = graphdata;
        return 1;
    }

    ajErr("Too many multiple graphs - expected %d graphs",
          thys->numofgraphsmax);

    return 0;
}




/* @func ajGraphDataReplace ************************************************
**
** Replace graph structure into the multiple graph structure.
**
** The graphdata now belongs to the graph - do not delete it while
** the graph is using it.
**
** @param [u] thys [AjPGraph] multiple graph structure.
** @param [u] graphdata [AjPGraphdata] graph to be added.
** @return [ajint] 1 if graph added successfully else 0;
** @@
******************************************************************************/

ajint ajGraphDataReplace(AjPGraph thys, AjPGraphdata graphdata)
{
    ajGraphdataDel(&(thys->graphs)[0]);
    (thys->graphs)[0] = graphdata;
    thys->numofgraphs=1;

    thys->minmaxcalc = 0;

    return 1;
}




/* @func ajGraphDataReplaceI ************************************************
**
** Replace one of the graph structures in the multiple graph structure.
**
** The graphdata now belongs to the graph - do not delete it while
** the graph is using it.
**
** @param [u] thys [AjPGraph] multiple graph structure.
** @param [u] graphdata [AjPGraphdata] graph to be added.
** @param [r] num [ajint] number within multiple graph.
** @return [ajint] 1 if graph added successfully else 0;
** @@
******************************************************************************/

ajint ajGraphDataReplaceI(AjPGraph thys, AjPGraphdata graphdata, ajint num)
{
    if(thys->numofgraphs > num) {
        ajGraphdataDel(&(thys->graphs)[num]);
        (thys->graphs)[num] = graphdata;
    }
    else if(thys->numofgraphs < thys->numofgraphsmax)
    {
        (thys->graphs)[thys->numofgraphs++] = graphdata;
    }


    thys->minmaxcalc = 0;

    return 1;
}




/* @func ajGraphNew ***********************************************************
**
** Create a structure to hold a general graph.
**
** @return [AjPGraph] multiple graph structure.
** @@
******************************************************************************/

AjPGraph ajGraphNew(void)
{
    AjPGraph graph;

    AJNEW0(graph);
    ajDebug("ajGraphNew - need to call ajGraphSet\n");

    return graph;
}




/* @funcstatic GraphNewPlplot *************************************************
**
** Populate a plplot structure to hold a general graph.
**
** @param [w] graph [AjPGraph] multiple graph structure.
** @return [void]
** @@
******************************************************************************/

static void GraphNewPlplot(AjPGraph graph)
{
    AJCNEW0(graph->graphs,1);

    ajDebug("GraphNewPlplot\n");

    graph->numofgraphs    = 0;
    graph->numofgraphsmax = 1;
    graph->flags          = GRAPH_XY;
    graph->minmaxcalc     = 0;
    ajFmtPrintS(&graph->outputfile,"%S", ajUtilGetProgram());

    return;
}




/* @func ajGraphxyNewI ********************************************************
**
** Create a structure to hold a number of graphs.
**
** @param [r] numsets [ajint] maximum number of graphs that can stored.
** @return [AjPGraph] multiple graph structure.
** @@
******************************************************************************/

AjPGraph ajGraphxyNewI(ajint numsets)
{
    AjPGraph ret;

    AJNEW0(ret);
    ret->numsets = numsets;

    ajDebug("ajGraphxyNewI numsets: %d\n", numsets);

    return ret;
}




/* @funcstatic GraphxyNewPlplot ***********************************************
**
** Create a structure to hold a number of graphs.
**
** @param [w] thys [AjPGraph] Empty graph object
** @return [void]
** @@
******************************************************************************/

static void GraphxyNewPlplot(AjPGraph thys)
{
    AJCNEW0(thys->graphs,thys->numsets);

    ajDebug("GraphxyNewPlplot numsets: %d\n", thys->numsets);

    thys->numofgraphs       = 0;
    thys->numofgraphsmax    = thys->numsets;
    thys->minX = thys->minY = 64000;
    thys->maxX = thys->maxY = -64000;
    thys->flags             = GRAPH_XY;
    thys->minmaxcalc          = 0;
    thys->xstart = thys->xend = 0;
    thys->ystart = thys->yend = 0;
    thys->Mainobj             = NULL;
    ajFmtPrintS(&thys->outputfile,"%S", ajUtilGetProgram());

    return;
}




/* @func ajGraphSetMulti ******************************************************
**
** Create a structure to hold a number of graphs.
**
** @param [w] thys [AjPGraph] Graph structure to store info in.
** @param [r] numsets [ajint] maximum number of graphs that can stored.
** @return [void]
** @@
******************************************************************************/

void ajGraphSetMulti(AjPGraph thys, ajint numsets)
{
    if (!thys)
	return;

    if (thys->graphs)
        AJFREE(thys->graphs);

    AJCNEW0(thys->graphs,numsets);

    ajDebug("ajGraphSetMulti numsets: %d\n", numsets);

    thys->numofgraphs    = 0;
    thys->numofgraphsmax = numsets;
    thys->minmaxcalc     = 0;
    thys->flags          = GRAPH_XY;

    return;
}




/* @func ajGraphdataSetTitle ***********************************************
**
**  Set the title.
**
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const AjPStr] Title
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetTitle(AjPGraphdata graphdata, const AjPStr title)
{
    ajStrAssignEmptyS(&graphdata->title,title);

    return;
}




/* @func ajGraphdataSetTitleC **********************************************
**
**  Set the title.
**
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const char*] Title
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetTitleC(AjPGraphdata graphdata, const char *title)
{
    ajStrAssignEmptyC(&graphdata->title,title);

    return;
}




/* @func ajGraphdataSetSubTitle ********************************************
**
**  Set the Subtitle.
**
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const AjPStr] Sub Title
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetSubTitle(AjPGraphdata graphdata, const AjPStr title)
{
    ajStrAssignEmptyS(&graphdata->subtitle,title);

    return;
}




/* @func ajGraphdataSetSubTitleC *******************************************
**
**  Set the subtitle.
**
** @param [u] graphdata  [AjPGraphdata] Graph structure to store info in.
** @param [r] title [const char*] Sub Title
** @return [void]
** @@
******************************************************************************/

void ajGraphdataSetSubTitleC(AjPGraphdata graphdata, const char *title)
{
    ajStrAssignEmptyC(&graphdata->subtitle,title);

    return;
}




/* @func ajGraphArrayMaxMin **************************************************
**
** Get the max and min of the data points you wish to display.
**
** @param [r] array [const float*] array
** @param [r] npoints [ajint] Number of data points
** @param [w] min [float*]  min.
** @param [w] max [float*]  max.
** @return [void]
** @@
******************************************************************************/

void ajGraphArrayMaxMin(const float *array,
			ajint npoints, float *min, float *max)
{
    ajint i;

    *min = 64000.;
    *max = -64000.;

    for(i=0;i<npoints;++i)
    {
	*min = (*min < array[i]) ? *min : array[i];
	*max = (*max > array[i]) ? *max : array[i];
    }

    return;
}




/* @func ajGraphxyCheckMaxMin *************************************************
**
** Calculate the max and min of the data points and store them.
**
** @param [u] thys [AjPGraph] multiple graph structure.
** @return [void]
** @@
******************************************************************************/

void ajGraphxyCheckMaxMin(AjPGraph thys)
{
    AjPGraphdata graphdata = NULL;
    ajint i;
    ajint j;

    for(i = 0 ; i < thys->numofgraphs ; i++)
    {
	graphdata = (thys->graphs)[i];

	if(graphdata->minX == graphdata->maxX ||
	   graphdata->minY == graphdata->maxY)
	{
	    graphdata->minX = graphdata->minY =  64000;
	    graphdata->maxX = graphdata->maxY =  -64000;

	    for( j = 0 ; j < graphdata->numofpoints; j++)
	    {
		if(graphdata->maxX < (graphdata->x)[j])
		    graphdata->maxX = (graphdata->x)[j];
		if(graphdata->maxY < (graphdata->y)[j])
		    graphdata->maxY = (graphdata->y)[j];
		if(graphdata->minX > (graphdata->x)[j])
		    graphdata->minX = (graphdata->x)[j];
		if(graphdata->minY > (graphdata->y)[j])
		    graphdata->minY = (graphdata->y)[j];
	    }
	}

	if (thys->minX > graphdata->minX)
	    thys->minX = graphdata->minX;

	if (thys->minY > graphdata->minY)
	    thys->minY = graphdata->minY;

	if (thys->maxX < graphdata->maxX)
	    thys->maxX = graphdata->maxX;

	if (thys->maxY < graphdata->maxY)
	    thys->maxY = graphdata->maxY;
    }

    if(!thys->minmaxcalc)
    {
	thys->xstart     = thys->minX;
	thys->xend       = thys->maxX;
	thys->ystart     = thys->minY;
	thys->yend       = thys->maxY;
	thys->minmaxcalc = ajTrue;
    }

    return;
}




/* @funcstatic GraphxyGeneral *************************************************
**
** A general routine for drawing graphs.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] closeit [AjBool] Close at end if true.
** @return [void]
** @@
******************************************************************************/

static void GraphxyGeneral(AjPGraph thys, AjBool closeit)
{
    AjPGraphdata graphdata;
    ajint i;
    ajint old;
    ajint old2;
    AjPTime ajtime;

    ajGraphSetDevice(thys);
    
    ajDebug("GraphxyGeneral flags %x\n", thys->flags);
    ajDebug("... title '%S'\n", thys->title);
    
    if(thys->flags & AJGRAPH_OVERLAP)
    {
	ajDebug("... AJGRAPH_OVERLAP\n");
	GraphColourBack();
	GraphInit(thys);
	GraphColourFore();
	/*    GraphSubPage(0);         Done in ajGraphplenv*/
	
	if(thys->numofgraphs)
	{
	    graphdata = (thys->graphs)[0];
	    thys->xstart = graphdata->minX;
	    thys->xend   = graphdata->maxX;
	    thys->ystart = graphdata->minY;
	    thys->yend   = graphdata->maxY;
	}

	ajGraphPlenv(thys->xstart, thys->xend,
		     thys->ystart, thys->yend,
		     thys->flags);
	
	if((thys->flags & AJGRAPH_TITLE) &&
	   ajStrGetLen(thys->title) <=1)
	{
	    ajtime = ajTimeNewToday();
	    ajStrAppendC(&thys->title,
			     ajFmtString("%S (%D)",
					 ajUtilGetProgram(),
					 ajtime));
	    ajTimeDel(&ajtime);
	}

	ajGraphicsSetLabelsS(((thys->flags & AJGRAPH_X_LABEL) ?
                              thys->xaxis : NULL),
                             ((thys->flags & AJGRAPH_Y_LABEL) ?
                              thys->yaxis : NULL),
                             ((thys->flags & AJGRAPH_TITLE) ?
                              thys->title : NULL),
                             (thys->flags & AJGRAPH_SUBTITLE) ?
                             thys->subtitle : NULL);
	
	for(i=0;i<thys->numofgraphs;i++)
	{
	    graphdata = (thys->graphs)[i];

	    /* Draw the line through the data */
	    old = ajGraphicsSetColourFore(graphdata->colour);

	    if(thys->flags & AJGRAPH_CIRCLEPOINTS)
		ajGraphicsDrawSymbols(graphdata->numofpoints,
                                   (graphdata->x), (graphdata->y), 9);

	    old2 = ajGraphSetLineStyle(graphdata->lineType);

	    if(thys->flags & AJGRAPH_JOINPOINTS)
	    {
		if(thys->flags & AJGRAPH_GAPS)
		    GraphArrayGaps(graphdata->numofpoints,
				   (graphdata->x),
				   (graphdata->y));
		else
		    GraphArray(graphdata->numofpoints,
			       (graphdata->x), (graphdata->y));
	    }

	    ajGraphSetLineStyle(old2);
	    ajGraphicsSetColourFore(old);
	}

	GraphDraw(thys);	
    }
    else
    {
	ajDebug("... else not AJGRAPH_OVERLAP\n");
	GraphSetNumSubPage(thys->numofgraphs);
	GraphColourBack();
	GraphInit(thys);
	GraphColourFore();

	/* first do the main plot title */
	if((thys->flags & AJGRAPH_TITLE) &&
	   ajStrGetLen(thys->title) <=1)
	{
	    ajtime = ajTimeNewToday();
	    ajStrAppendC(&thys->title,
			     ajFmtString("%S (%D)",
					 ajUtilGetProgram(),
					 ajtime));
	    ajTimeDel(&ajtime);
	}

	for(i=0;i<thys->numofgraphs;i++)
	{
	    graphdata = (thys->graphs)[i];
	    ajGraphPlenv(graphdata->minX, graphdata->maxX,
			 graphdata->minY, graphdata->maxY,
			 thys->flags);

	    if(!ajStrGetLen(graphdata->title) &&
	       (thys->flags & AJGRAPH_TITLE))
		ajStrAssignS(&graphdata->title, thys->title);

	    if(!ajStrGetLen(graphdata->subtitle) &&
	       (thys->flags & AJGRAPH_SUBTITLE))
		ajStrAssignS(&graphdata->subtitle, thys->subtitle);

	    ajGraphicsSetLabelsS(((thys->flags & AJGRAPH_X_LABEL) ?
                                  graphdata->xaxis : NULL),
                                 ((thys->flags & AJGRAPH_Y_LABEL) ?
                                  graphdata->yaxis : NULL),
                                 ((thys->flags & AJGRAPH_TITLE) ?
                                  graphdata->title : NULL),
                                 (thys->flags & AJGRAPH_SUBTITLE) ?
                                 graphdata->subtitle : NULL);
	    old = ajGraphicsSetColourFore(graphdata->colour);

	    if(thys->flags & AJGRAPH_CIRCLEPOINTS)
		ajGraphicsDrawSymbols(graphdata->numofpoints,
                                      (graphdata->x), (graphdata->y), 9);
	    
	    /* Draw the line through the data */
	    /*    old = ajGraphicsSetColourFore(i+2);*/
	    old2 = ajGraphSetLineStyle(graphdata->lineType);

	    if(thys->flags & AJGRAPH_JOINPOINTS)
	    {
		if(thys->flags & AJGRAPH_GAPS)
		    GraphArrayGaps(graphdata->numofpoints,
				   (graphdata->x), (graphdata->y));
		else
		    GraphArray(graphdata->numofpoints,
			       (graphdata->x), (graphdata->y));
	    }

	    ajGraphSetLineStyle(old2);
	    ajGraphicsSetColourFore(old);
	    GraphDataDraw(graphdata);
	}	
    }

    if(closeit)
	GraphClose();

    return;
}




/* @func ajGraphxyDisplay *****************************************************
**
** A general routine for drawing graphs.
**
** @param [u] thys [AjPGraph] Multiple graph pointer.
** @param [r] closeit [AjBool]   Whether to close graph at the end.
** @return [void]
** @@
******************************************************************************/

void ajGraphxyDisplay(AjPGraph thys, AjBool closeit)
{
    /* Calling funclist graphType() */
    (graphType[thys->displaytype].XYDisplay)
	(thys, closeit, graphType[thys->displaytype].Ext);

    return;
}




/* @func ajGraphAddRect ****************************************************
**
** Add a Rectangle to be drawn when the graph is plotted, fill states whether
** the rectangle should be filled in.
**
** @param [u] thys [AjPGraph] Graph object
** @param [r] xx1 [float] Start x coordinate
** @param [r] yy1 [float] Start y coordinate
** @param [r] xx2 [float] End x coordinate
** @param [r] yy2 [float] End y coordinate
** @param [r] colour [ajint] Colour code (see PLPLOT)
** @param [r] fill [ajint] Fill code (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

void ajGraphAddRect(AjPGraph thys, float xx1, float yy1,
		       float xx2, float yy2, ajint colour, ajint fill)
{
    AjPGraphobj Obj;

    if(!thys->Mainobj)
    {
	AJNEW(thys->Mainobj);
	Obj = thys->Mainobj;
    }
    else
    {
	Obj = thys->Mainobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++thys->numofobjects;


    if(fill)
	Obj->type = RECTANGLEFILL;
    else
	Obj->type = RECTANGLE;

    Obj->text = 0;
    Obj->xx1 = xx1;
    Obj->xx2 = xx2;
    Obj->yy1 = yy1;
    Obj->yy2 = yy2;
    Obj->colour = colour;
    Obj->next = 0;

    return;
}




/* @func ajGraphAddText ****************************************************
**
** Add text to be drawn when the graph is plotted.
**
** @param [u] thys [AjPGraph] Graph object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
** @param [r] text [const char*] Text
**
** @return [void]
** @@
******************************************************************************/

void ajGraphAddText(AjPGraph thys, float xx1, float yy1,
		       ajint colour, const char *text)
{
    AjPGraphobj Obj;

    if(!thys->Mainobj)
    {
	AJNEW(thys->Mainobj);
	Obj = thys->Mainobj;
    }
    else
    {
	Obj = thys->Mainobj;
	while(Obj->next)
	    Obj = Obj->next;
	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++thys->numofobjects;


    Obj->type = TEXT;
    Obj->text = ajStrNewC(text);
    Obj->xx1 = xx1;
    Obj->xx2 = 0.0;
    Obj->yy1 = yy1;
    Obj->yy2 = 0.0;
    Obj->colour = colour;
    Obj->scale = 0.0;
    Obj->next = 0;

    return;
}




/* @func ajGraphAddTextScale **************************************************
**
** Add text to be drawn when the graph is plotted.
**
** @param [u] thys [AjPGraph] Graph object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
** @param [r] scale [float] Character scale (0.0 to use the default)
** @param [r] text [const char*] Text
**
** @return [void]
** @@
******************************************************************************/

void ajGraphAddTextScale(AjPGraph thys, float xx1, float yy1,
			 ajint colour, float scale, const char *text)
{
    AjPGraphobj Obj;

    if(!thys->Mainobj)
    {
	AJNEW(thys->Mainobj);
	Obj = thys->Mainobj;
    }
    else
    {
	Obj = thys->Mainobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++thys->numofobjects;


    Obj->type = TEXT;
    Obj->text = ajStrNewC(text);
    Obj->xx1 = xx1;
    Obj->xx2 = 0.0;
    Obj->yy1 = yy1;
    Obj->yy2 = 0.0;
    Obj->colour = colour;
    Obj->scale = scale;
    Obj->next = 0;

    return;
}




/* @func ajGraphAddLine ****************************************************
**
** Add a line to be drawn when the graph is plotted.
**
** @param [u] thys [AjPGraph] Graph object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] xx2 [float] End x position
** @param [r] yy2 [float] End y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

void ajGraphAddLine(AjPGraph thys, float xx1, float yy1,
		       float xx2, float yy2, ajint colour)
{
    AjPGraphobj Obj;

    if(!thys->Mainobj)
    {
	AJNEW(thys->Mainobj);
	Obj = thys->Mainobj;
    }
    else
    {
	Obj = thys->Mainobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++thys->numofobjects;

    Obj->type = LINE;
    Obj->text = 0;
    Obj->xx1 = xx1;
    Obj->xx2 = xx2;
    Obj->yy1 = yy1;
    Obj->yy2 = yy2;
    Obj->colour = colour;
    Obj->next = 0;

    return;
}




/* @func ajGraphdataDel ****************************************************
**
** Destructor for a graph data object
**
** @param [d] pthys [AjPGraphdata*] Graph data object
** @return [void]
** @@
******************************************************************************/

void ajGraphdataDel(AjPGraphdata *pthys)
{
    AjPGraphobj here = NULL;
    AjPGraphobj p    = NULL;
    AjPGraphdata thys;

    thys = *pthys;

    if (!thys)
	return;

    ajDebug("ajGraphdataDel objects:%d\n", thys->numofobjects);

    here = p = thys->Dataobj;

    while(p)
    {
	p = here->next;
	ajStrDel(&here->text);
	AJFREE(here);
	here = p;
    }

    thys->Dataobj = NULL;

    ajStrDel(&thys->title);
    ajStrDel(&thys->subtitle);
    ajStrDel(&thys->xaxis);
    ajStrDel(&thys->yaxis);
    ajStrDel(&thys->gtype);
    AJFREE(thys->x);
    AJFREE(thys->y);
    thys->numofobjects = 0;

    AJFREE(*pthys);

    return;
}






/* #func ajGraphdataDel ****************************************************
**
** Delete a graph data object.
**
** #param [w] thys [AjPGraphdata*] Graph data object
**
** #return [void]
** ##
******************************************************************************/

/*
//void ajGraphdataDel(AjPGraphdata *thys)
//{
//    AjPGraphdata this;
//
//    this = *thys;
//
//    if (!thys)
//	return;
//
//    AJFREE(this->x);
//    AJFREE(this->y);
//    ajStrDel(&this->title);
//    ajStrDel(&this->subtitle);
//    ajStrDel(&this->xaxis);
//    ajStrDel(&this->yaxis);
//    ajStrDel(&this->gtype);
//
//    ajGraphdataDel(thys);
//
//    AJFREE(*thys);
//
//    return;
//}
*/




/* @funcstatic GraphPrint **************************************************
**
** Print all the drawable objects in readable form.
**
** @param [r] thys [const AjPGraph] Graph object
**
** @return [void]
** @@
******************************************************************************/

static void GraphPrint(const AjPGraph thys)
{
    AjPGraphobj Obj;

    if(!thys->Mainobj)
    {
	ajUserDumpC("No Objects");

	return;
    }
    else			   /* cycle through till NULL found */
    {
	Obj = thys->Mainobj;

	while(Obj)
	{
	    if(Obj->type == RECTANGLE )
		ajUser("type = RECTANGLE, %f %f %f %f col= %d",
		       Obj->xx1, Obj->yy1,
		       Obj->xx2,Obj->yy2,Obj->colour);
	    else if(Obj->type == RECTANGLEFILL )
		ajUser("type = RECTANGLEFILL, %f %f %f %f col= %d",
		       Obj->xx1, Obj->yy1,
		       Obj->xx2,Obj->yy2,Obj->colour);
	    else if(Obj->type == TEXT)
		ajUser("type = TEXT, %f %f col= %d %S",
		       Obj->xx1, Obj->yy1,Obj->colour,
		       Obj->text);
	    else if(Obj->type == LINE )
		ajUser("type = LINE, %f %f %f %f col= %d",
		       Obj->xx1, Obj->yy1,
		       Obj->xx2,Obj->yy2,Obj->colour);

	    Obj = Obj->next;
	}
    }

    return;
}




/* @funcstatic GraphDraw ***************************************************
**
** Display the drawable objects connected to this graph.
**
** @param [r] thys [const AjPGraph] Graph object
**
** @return [void]
** @@
******************************************************************************/

static void GraphDraw(const AjPGraph thys)
{
    AjPGraphobj Obj;
    ajint temp;
    float cold = 1.0;

    /* graphdata : calls ajGraphLine etc. */

    if(!thys->Mainobj)
	return;
    else
    {
	Obj = thys->Mainobj;

	while(Obj)
	{
	    if(Obj->type == RECTANGLE)
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawRect(Obj->xx1, Obj->yy1,Obj->xx2,
                                   Obj->yy2);
		ajGraphicsSetColourFore(temp);
	    }
	    else if(Obj->type == RECTANGLEFILL)
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawRectFill(Obj->xx1, Obj->yy1,Obj->xx2,
                                       Obj->yy2);
		ajGraphicsSetColourFore(temp);
	    }
	    else if(Obj->type == TEXT )
	    {
		if(Obj->scale)
		    cold = ajGraphicsSetCharscale(Obj->scale);
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawTextAtstart(Obj->xx1, Obj->yy1,
                                          ajStrGetPtr(Obj->text));
		ajGraphicsSetColourFore(temp);
		if(Obj->scale)
		    ajGraphicsSetCharscale(cold);
	    }
	    else if(Obj->type == LINE)
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawLine(Obj->xx1, Obj->yy1,Obj->xx2,
                                   Obj->yy2);
		ajGraphicsSetColourFore(temp);
	    }
	    else
		ajUserDumpC("UNDEFINED OBJECT TYPE USED");

	    Obj = Obj->next;
	}
    }

    return;
}




/* @func ajGraphClear **************************************************
**
**  Delete all the drawable objects connected to the graph object.
**
** @param [u] thys [AjPGraph] Graph object
**
** @return [void]
** @@
******************************************************************************/

void ajGraphClear(AjPGraph thys)
{
    AjPGraphobj Obj;
    AjPGraphobj anoth;

    if(!thys->Mainobj)
	return;
    else
    {
	Obj = thys->Mainobj;

	while(Obj)
	{
	    anoth = Obj->next;
	    ajStrDel(&Obj->text);
	    AJFREE(Obj);
	    Obj = anoth;
	}
    }

    thys->Mainobj = NULL;
    thys->numofobjects = 0;

    return;
}




/* @func ajGraphdataAddRect ************************************************
**
** Add a Rectangle to be drawn when the graph is plotted, fill states whether
** the rectangle should be filled in.
**
** @param [u] graphdata [AjPGraphdata] Graph data object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] xx2 [float] End x position
** @param [r] yy2 [float] End y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
** @param [r] fill [ajint] Fill code (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

void ajGraphdataAddRect(AjPGraphdata graphdata,
			   float xx1, float yy1,
			   float xx2, float yy2,
			   ajint colour, ajint fill)
{
    AjPGraphobj Obj;

    if(!graphdata->Dataobj)
    {
	AJNEW((graphdata->Dataobj));
	Obj = graphdata->Dataobj;
    }
    else
    {
	Obj = graphdata->Dataobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++graphdata->numofobjects;


    if(fill)
	Obj->type = RECTANGLEFILL;
    else
	Obj->type = RECTANGLE;

    Obj->text = 0;
    Obj->xx1 = xx1;
    Obj->xx2 = xx2;
    Obj->yy1 = yy1;
    Obj->yy2 = yy2;
    Obj->colour = colour;
    Obj->next = 0;

    return;
}




/* @func ajGraphdataAddText ************************************************
**
** Add Text to be drawn when the graph is plotted.
**
** @param [u] graphdata [AjPGraphdata] Graph data object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
** @param [r] text [const char*] Text to add
**
** @return [void]
** @@
******************************************************************************/

void ajGraphdataAddText(AjPGraphdata graphdata, float xx1, float yy1,
			   ajint colour, const char *text)
{
    AjPGraphobj Obj;

    if(!graphdata->Dataobj)
    {
	AJNEW((graphdata->Dataobj));
	Obj = graphdata->Dataobj;
    }
    else			   /* cycle through till NULL found */
    {
	Obj = graphdata->Dataobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++graphdata->numofobjects;


    Obj->type = TEXT;
    Obj->text = 0;
    ajStrAssignEmptyC(&Obj->text,text);
    Obj->xx1 = xx1;
    Obj->xx2 = 0.0;
    Obj->yy1 = yy1;
    Obj->yy2 = 0.0;
    Obj->colour = colour;
    Obj->scale = 0.0;
    Obj->next = 0;

    return;
}




/* @func ajGraphdataAddTextScale *******************************************
**
** Add Text to be drawn when the graph is plotted.
**
** @param [u] graphdata [AjPGraphdata] Graph data object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
** @param [r] scale [float] Character scale (0.0 to use the default)
** @param [r] text [const char*] Text to add
**
** @return [void]
** @@
******************************************************************************/

void ajGraphdataAddTextScale(AjPGraphdata graphdata,
				float xx1, float yy1,
				ajint colour, float scale, const char *text)
{
    AjPGraphobj Obj;

    if(!graphdata->Dataobj)
    {
	AJNEW((graphdata->Dataobj));
	Obj = graphdata->Dataobj;
    }
    else			   /* cycle through till NULL found */
    {
	Obj = graphdata->Dataobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++graphdata->numofobjects;


    Obj->type = TEXT;
    Obj->text = 0;
    ajStrAssignEmptyC(&Obj->text,text);
    Obj->xx1 = xx1;
    Obj->xx2 = 0.0;
    Obj->yy1 = yy1;
    Obj->yy2 = 0.0;
    Obj->colour = colour;
    Obj->scale = scale;
    Obj->next = 0;

    return;
}




/* @func ajGraphdataAddLine ************************************************
**
** Add a line to be drawn when the graph is plotted.
**
** @param [u] graphdata [AjPGraphdata] Graph data object
** @param [r] xx1 [float] Start x position
** @param [r] yy1 [float] Start y position
** @param [r] xx2 [float] End x position
** @param [r] yy2 [float] End y position
** @param [r] colour [ajint] Colour code (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

void ajGraphdataAddLine(AjPGraphdata graphdata, float xx1, float yy1,
			   float xx2, float yy2, ajint colour)
{
    AjPGraphobj Obj;

    if(!graphdata->Dataobj)
    {
	AJNEW((graphdata->Dataobj));
	Obj = graphdata->Dataobj;
    }
    else
    {
	Obj = graphdata->Dataobj;

	while(Obj->next)
	    Obj = Obj->next;

	AJNEW(Obj->next);
	Obj = Obj->next;
    }

    ++graphdata->numofobjects;

    Obj->type = LINE;
    Obj->text = 0;
    Obj->xx1 = xx1;
    Obj->xx2 = xx2;
    Obj->yy1 = yy1;
    Obj->yy2 = yy2;
    Obj->colour = colour;
    Obj->next = 0;

    return;
}




/* @funcstatic GraphDataPrint **********************************************
**
** Print all the drawable objects in readable form.
**
** @param [r] graphdata [const AjPGraphdata] Graph data object
**
** @return [void]
** @@
******************************************************************************/

static void GraphDataPrint(const AjPGraphdata graphdata)
{
    AjPGraphobj Obj;

    if(!graphdata->Dataobj)
    {
	ajUserDumpC("No Objects");

	return;
    }
    else
    {
	Obj = graphdata->Dataobj;

	while(Obj)
	{
	    if(Obj->type == RECTANGLE )
		ajUser("type = RECTANGLE, %f %f %f %f col= %d",
		       Obj->xx1,Obj->yy1,
		       Obj->xx2,Obj->yy2,Obj->colour);
	    else if(Obj->type == RECTANGLEFILL )
		ajUser("type = RECTANGLEFILL, %f %f %f %f col= %d",
		       Obj->xx1, Obj->yy1,
		       Obj->xx2,Obj->yy2,Obj->colour);
	    else if(Obj->type == TEXT)
		ajUser("type = TEXT, %f %f col= %d %S",Obj->xx1,
		       Obj->yy1,Obj->colour,
		       Obj->text);
	    else if(Obj->type == LINE )
		ajUser("type = LINE, %f %f %f %f col= %d",Obj->xx1,
		       Obj->yy1,
		       Obj->xx2,Obj->yy2,Obj->colour);

	    Obj = Obj->next;
	}
    }

    return;
}




/* @funcstatic GraphDataDraw ***********************************************
**
** Display the drawable objects connected to this graph.
**
** @param [r] graphdata [const AjPGraphdata] Graph data object
**
** @return [void]
** @@
******************************************************************************/

static void GraphDataDraw(const AjPGraphdata graphdata)
{
    AjPGraphobj Obj;
    ajint temp;

    /* graphdata : calls ajGraphLine */

    if(!graphdata->Dataobj)
	return;
    else
    {
	Obj = graphdata->Dataobj;
	while(Obj)
	{
	    if(Obj->type == RECTANGLE)
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawRect(Obj->xx1, Obj->yy1,
                                Obj->xx2,Obj->yy2);
		ajGraphicsSetColourFore(temp);
	    }
	    else if(Obj->type == RECTANGLEFILL)
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawRectFill(Obj->xx1, Obj->yy1,
				Obj->xx2,Obj->yy2);
		ajGraphicsSetColourFore(temp);
	    }
	    else if(Obj->type == TEXT )
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawTextAtstart(Obj->xx1, Obj->yy1,
				 ajStrGetPtr(Obj->text));
		ajGraphicsSetColourFore(temp);
	    }
	    else if(Obj->type == LINE)
	    {
		temp = ajGraphicsSetColourFore(Obj->colour);
		ajGraphicsDrawLine(Obj->xx1, Obj->yy1,
			    Obj->xx2,Obj->yy2);
		ajGraphicsSetColourFore(temp);
	    }
	    else
		ajUserDumpC("UNDEFINED OBJECT TYPE USED");

	    Obj = Obj->next;
	}
    }

    return;
}





/* @func ajGraphPrintType *****************************************************
**
** Print graph types
**
** @param [u] outf [AjPFile] Output file
** @param [r] full [AjBool] Full report (usually ajFalse)
** @return [void]
** @@
******************************************************************************/
void ajGraphPrintType(AjPFile outf, AjBool full)
{
    GraphPType gt;
    ajint i;

    ajFmtPrintF(outf, "\n");
    ajFmtPrintF(outf, "# Graphics Devices\n");
    ajFmtPrintF(outf, "# Name     Name\n");
    ajFmtPrintF(outf, "# Alias    Alias name\n");
    ajFmtPrintF(outf, "# Device   Device name\n");
    ajFmtPrintF(outf, "# Extension Filename extension (null if no file "
                "created)\n");
    ajFmtPrintF(outf, "# Description\n");
    ajFmtPrintF(outf, "# Name       Alias Device     Extension Description\n");
    ajFmtPrintF(outf, "GraphType {\n");

    for(i=0; graphType[i].Name; i++)
    {
	gt = &graphType[i];

	if(full || !gt->Alias)
        {
	    ajFmtPrintF(outf, "  %-10s", gt->Name);
	    ajFmtPrintF(outf, " %5B", gt->Alias);
	    ajFmtPrintF(outf, " %-10s", gt->Device);
	    ajFmtPrintF(outf, " %-10s", gt->Ext);
	    ajFmtPrintF(outf, " \"%s\"", gt->Desc);
	    ajFmtPrintF(outf, "\n");
	}
    }

    ajFmtPrintF(outf, "}\n");

    return;
}




/* @funcstatic GraphDistPts ***************************************************
**
** Compute the distance between 2 points in user coordinates.
**
** @param [r] xx1 [float] x coord of point 1
** @param [r] yy1 [float] y coord of point 1
** @param [r] xx2 [float] x coord of point 2
** @param [r] yy2 [float] y coord of point 2
**
** @return [float] The distance between the 2 points in user coordinates
** @@
******************************************************************************/

static float GraphDistPts(float xx1, float yy1, float xx2, float yy2)
{
    PLFLT diag;

    diag = (PLFLT) sqrt( (xx2-xx1) * (xx2-xx1) + (yy2-yy1) * (yy2-yy1) );

    return diag;
}




/* @func ajGraphTextLength ****************************************************
**
** Compute the length of a string in user coordinates.
**
** @param [r] xx1 [PLFLT] Start of text box on x axis
** @param [r] yy1 [PLFLT] Start of text box on y axis
** @param [r] xx2 [PLFLT] End of text box on x axis
** @param [r] yy2 [PLFLT] End of text box on y axis
** @param [r] text [const char*] Text
**
** @return [PLFLT] The length of the string in user coordinates.
** @@
******************************************************************************/

PLFLT ajGraphTextLength(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
			const char *text)
{
    return GraphTextLength(xx1, yy1, xx2, yy2, text);
}




/* @func ajGraphTextHeight ****************************************************
**
** Compute the height of a character in user coordinates.
**
** @param [r] xx1 [PLFLT] Start of text box on x axis
** @param [r] xx2 [PLFLT] End of text box on x axis
** @param [r] yy1 [PLFLT] Start of text box on y axis
** @param [r] yy2 [PLFLT] End of text box on y axis
**
** @return [PLFLT] The height of the character in user coordinates.
** @@
******************************************************************************/

PLFLT ajGraphTextHeight(PLFLT xx1, PLFLT xx2, PLFLT yy1, PLFLT yy2)
{
    return GraphTextHeight(xx1, yy1, xx2, yy2);
}




/* @func ajGraphDistPts *******************************************************
**
** Compute the distance between 2 points in user coordinates.
**
** @param [r] xx1 [PLFLT] x coord of point 1
** @param [r] yy1 [PLFLT] y coord of point 1
** @param [r] xx2 [PLFLT] x coord of point 2
** @param [r] yy2 [PLFLT] y coord of point 2
**
** @return [PLFLT] The distance between the 2 points in user coordinates.
** @@
******************************************************************************/

PLFLT ajGraphDistPts(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2)
{
    return GraphDistPts(xx1, yy1, xx2, yy2);
}




/* @func ajGraphFitTextAtline *************************************************
**
** Computes the character size (in mm) needed to write a text string
** with specified height and length (in user coord). The length of the
** string is the distance between (xx1,yy1) and (xx2,yy2); its height is
** TextHeight.  If the default size is too large, characters are
** shrunk. If it is too small, characters are enlarged.
**
** @param [r] xx1 [PLFLT] xx1 coor.
** @param [r] yy1 [PLFLT] yy1 coor.
** @param [r] xx2 [PLFLT] xx2 coor.
** @param [r] yy2 [PLFLT] yy2 coor.
** @param [r] text [const char*] The text to be displayed.
** @param [r] TextHeight [PLFLT] The height of the text (in user coord).
** @return [PLFLT] The character size (in mm) that fits the specified
**                 height and length.
**
** @@
******************************************************************************/

PLFLT ajGraphFitTextAtline(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
			   const char *text,
			   PLFLT TextHeight)
{
    PLFLT i;
    PLFLT stringHeight;
    PLFLT stringLength;
    PLFLT distpts;
    PLFLT oldcharsize;
    PLFLT origcharsize;
    PLFLT origcharscale;
    PLFLT charsize;
    
    /* adjust character height */
    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);
    ajGraphicsGetCharsize(&origcharsize,&origcharscale);

    ajDebug("ajGraphFitTextAtline (higha) %.1f,%.1f %.1f,%.1f '%s' %f (%f)\n",
	    xx1, yy1, xx2, yy2, text, TextHeight, stringHeight);

    /* start at origcharsize */

    if( stringHeight<TextHeight )
    {
	for(i=origcharsize; i<100.0; i+=(PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);
	    if( stringHeight>TextHeight )
	    {
		i -= (PLFLT)0.1;
		break;
	    }
	}

	ajDebug("ajGraphFitTextAtline (higha) larger %f\n", stringHeight);
    }
    else
    {
	for(i=origcharsize; i>0.0; i-=(PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);

	    if( stringHeight<TextHeight )
                break;
	}

	ajDebug("ajGraphFitTextAtline (higha) smaller %f\n", stringHeight);
    }

    charsize = i;
    ajGraphicsSetDefcharsize(charsize);

    /* repeat starting with new charsize */

    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);
    oldcharsize = ajGraphicsSetDefcharsize(0.0);
    
    ajDebug("ajGraphFitTextAtline (highb) %.1f,%.1f %.1f,%.1f '%s' %f (%f)\n",
	    xx1, yy1, xx2, yy2, text, TextHeight, stringHeight);

    if( stringHeight<TextHeight )
    {
	for(i=oldcharsize; i<100.0; i+=(PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);

	    if( stringHeight>TextHeight )
	    {
		i -= (PLFLT)0.1;
		break;
	    }
	}

	ajDebug("ajGraphFitTextAtline (highb) larger %f\n", stringHeight);
    }
    else
    {
	for(i=oldcharsize; i>0.0; i -= (PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);
	    if( stringHeight<TextHeight )
                break;
	}

	ajDebug("ajGraphFitTextAtline (highb) smaller %f\n", stringHeight);
    }

    charsize = i;
    ajGraphicsSetDefcharsize(charsize);
    
    /* repeat again starting with new charsize */

    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);
    oldcharsize = ajGraphicsSetDefcharsize(0.0);
    
    ajDebug("ajGraphFitTextAtline (highc) %.1f,%.1f %.1f,%.1f '%s' %f (%f)\n",
	    xx1, yy1, xx2, yy2, text, TextHeight, stringHeight);

    if(stringHeight<TextHeight)
    {
	for(i=oldcharsize; i<100.0; i += (PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);

	    if(stringHeight>TextHeight)
	    {
		i -= (PLFLT)0.1;
		break;
	    }
	}

	ajDebug("ajGraphFitTextAtline (highc) larger %f\n", stringHeight);
    }
    else
    {
	for(i=oldcharsize; i>0.0; i -= (PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringHeight = ajGraphTextHeight(xx1, yy1, xx2, yy2);
	    if( stringHeight<TextHeight ) break;
	}

	ajDebug("ajGraphFitTextAtline (highc) smaller %f\n", stringHeight);
    }

    charsize = i;
    ajGraphicsSetDefcharsize(charsize);
    
    /* adjust character width */
    distpts = ajGraphDistPts(xx1, yy1, xx2, yy2);
    stringLength = ajGraphTextLength(xx1, yy1, xx2, yy2, text);
    
    ajDebug("ajGraphFitTextAtline (width) %.1f,%.1f %.1f,%.1f '%s' %f (%f)\n",
	    xx1, yy1, xx2, yy2, text, distpts, stringLength);

    if(stringLength<distpts)
    {
	for(i=charsize; i<100.0; i += (PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringLength = ajGraphTextLength(xx1, yy1, xx2, yy2, text);

	    if(stringLength>distpts)
	    {
		i -= (PLFLT)0.1;
		break;
	    }
	}

	ajDebug("ajGraphFitTextAtline (width) larger %f\n", stringLength);
    }
    else
    {
	for(i=charsize; i>0.0; i -= (PLFLT)0.1)
	{
	    ajGraphicsSetDefcharsize(i);
	    stringLength = ajGraphTextLength(xx1, yy1, xx2, yy2, text);
	    if( stringLength<distpts ) break;
	}

	ajDebug("ajGraphFitTextAtline (width) smaller %f\n", stringLength);
    }

    charsize = i;
    ajGraphicsSetDefcharsize(origcharsize);
    ajDebug("ajGraphFitTextAtline returns charsize %f\n", charsize);

    return charsize;
}




/* @func ajComputeCoord *******************************************************
**
** compute the coordinates of a point on a circle knowing the angle.
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] Radius  [PLFLT] Radius of the circle.
** @param  [r] Angle [PLFLT] angle at which the point is.
** @return [PLFLT*] The x and y coordinates of the point.
** @@
******************************************************************************/

PLFLT* ajComputeCoord(PLFLT xcentre, PLFLT ycentre, PLFLT Radius, PLFLT Angle)
{
    PLFLT *xy;

    xy = (float *)AJALLOC( 2*sizeof(float) );
    xy[0] = xcentre + ( Radius*(float)cos(ajDegToRad(Angle)) );
    xy[1] = ycentre + ( Radius*(float)sin(ajDegToRad(Angle)) );

    return xy;
}




/* @funcstatic GraphDrawTextOncurve *******************************************
**
** Draw text along a curve (i.e., an arc of a circle).
** The text is written character by character.
**
** @param  [r] xcentre [PLFLT] x coor for centre.
** @param  [r] ycentre [PLFLT] y coor for centre.
** @param  [r] Radius  [PLFLT] Radius of the circle.
** @param  [r] Angle [PLFLT] angle at which a particular character
**                           will be written (in deg).
** @param  [r] pos [PLFLT] index for incrementing the angle for the
**                         next character in the text.
** @param  [r] Text [const char*] The text to be displayed.
** @param  [r] just [PLFLT] justification of the string.
**                          (0=left,1=right,0.5=middle etc)
** @return [void]
** @@
******************************************************************************/

static void GraphDrawTextOncurve(PLFLT xcentre, PLFLT ycentre,
				 PLFLT Radius, PLFLT Angle, PLFLT pos,
				 const char *Text, PLFLT just)
{
    ajint i;
    ajint numchar;
    PLFLT *xyy1;
    PLFLT *xyy2;
    char *text;

    text = (char *)AJALLOC( 1000*sizeof(char) );

    numchar = strlen(Text);

    for(i=0; i<numchar; i++)
    {
	xyy1 = ajComputeCoord(xcentre, ycentre, Radius, Angle+pos*(PLFLT)i+
			     (PLFLT)0.5*pos);
	xyy2 = ajComputeCoord(xcentre, ycentre, Radius, Angle+pos*(PLFLT)i+
			     (PLFLT)1.5*pos);
	strcpy(text, Text);
	text[i+1] = '\0';
	ajGraphicsDrawTextAtline(xyy1[0], xyy1[1], xyy2[0], xyy2[1], &text[i],
                                 just);
	AJFREE(xyy1);
	AJFREE(xyy2);
    }

    AJFREE(text);

    return;
}





/* @funcstatic GraphSymbols ***************************************************
**
** Draw a symbol from the font list.
**
** @param [r] xx1 [CONST float*] Array of x axis values
** @param [r] yy1 [CONST float*] Array of y axis values
** @param [r] numofdots [ajint] Number of data points
** @param [r] symbol [ajint] Symbol number (see PLPLOT)
**
** @return [void]
** @@
******************************************************************************/

static void GraphSymbols(float *xx1, float *yy1, ajint numofdots,
			 ajint symbol)
{
    ajDebug("=g= plpoin(%d, %.2f .. %.2f, %.2f .. %.2f, %d) "
	     "[size, x..x y..y sym ]\n",
	     numofdots,xx1[0], xx1[numofdots-1], yy1[0], yy1[numofdots-1],
            symbol);
    plpoin(numofdots,xx1,yy1, symbol);

    return;
}




/* @funcstatic GraphText ******************************************************
**
** Display text.
**
** @param [r] xx1 [float] Start of text box on x axis
** @param [r] yy1 [float] Start of text box on y axis
** @param [r] xx2 [float] End of text box on x axis
** @param [r] yy2 [float] End of text box on y axis
** @param [r] just [float] Justification (see PLPLOT)
** @param [r] text [const char*] Text
**
** @return [void]
** @@
******************************************************************************/

static void GraphText(float xx1, float yy1, float xx2, float yy2,
		      float just,const char *text)
{
    ajDebug("=g= plptex(%.2f, %.2f, %.2f, %.2f, %.2f, '%s') "
	    "[xy xy just text]\n",
	    xx1, yy1, xx2, yy2, just, text);
    plptex(xx1,yy1,xx2,yy2,just, text);

    return;
}




/* @func ajGraphicsGetOutfiles *************************************************
**
** Information on files created for graph output
**
** @param [w] files [AjPList*] List of graph files
** @return [ajint] Number of files (will match length of list)
** @@
******************************************************************************/

ajint ajGraphicsGetOutfiles(AjPList* files)
{
    ajint i;
    ajint j;
    char tmp[1024];
    AjPStr tmpStr = NULL;

    i = plfileinfo(tmp);

    ajDebug("ajGraphInfo i:%d tmp:'%s'\n", i, tmp);

    *files = ajListstrNew();

    ajDebug("ajGraphInfo new list\n");

    if(!i)
	return 0;

    if(i < 0)				/* single filename  in tmp*/
    {
	ajStrAssignC(&tmpStr, tmp);
	ajListstrPushAppend(*files, tmpStr);
	tmpStr=NULL;
	return 1;
    }

    for(j=1; j<=i; j++) /* family: tmp has format, i is no. of files */
    {
	ajDebug("ajGraphInfo printing file %d\n", j);
	ajFmtPrintS(&tmpStr, tmp, j);
	ajDebug("ajGraphInfo storing file '%S'\n", tmpStr);
	ajListstrPushAppend(*files, tmpStr);
	tmpStr=NULL;
    }

    return i;
}



/* @obsolete ajGraphInfo
** @rename ajGraphicsGetOutfiles
*/

__deprecated ajint ajGraphInfo(AjPList* files)
{
    return ajGraphicsGetOutfiles(files);
}


/* @funcstatic GraphDatafileNext **********************************************
**
** Opens the first or next graphData file
**
** return [void]
** @@
******************************************************************************/

static void GraphDatafileNext(void)
{
    AjPStr tempstr;

    ajDebug("GraphDatafileNext lines %d (%.3f, %.3f, %.3f, %.3f)\n",
	     graphData->Lines,
	     graphData->xmin, graphData->xmax,
	     graphData->ymin, graphData->ymax);

    if(graphData)
    {
	tempstr = NULL;
	ajFileClose(&graphData->File);
	graphData->Lines = 0;
	tempstr = ajFmtStr("%S%d%S",
			   graphData->FName,++graphData->Num,graphData->Ext);
	graphData->File = ajFileNewOutNameS(tempstr);

	if(!graphData->File)
	{
	    ajErr("Could not open graph file %S\n",tempstr);
	    return;
	}

	ajDebug("Writing graph %d data to %S\n",graphData->Num,tempstr);

	if(graphData->Num > 1)		/* ajGraphOpenWin will do the first */
	{
	    ajFmtPrintF(graphData->File,
			"##Graphic\n##Screen x1 %f y1 %f x2 %f y2 %f\n",
			graphData->xmin, graphData->ymin,
			graphData->xmax, graphData->ymax);
	    graphData->Lines++;
	    graphData->Lines++;
	}

	ajListstrPushAppend(graphData->List, tempstr);
    }

    return;
}




/* @func ajGraphIsData **************************************************
**
** returns ajTrue if the graph is creating a data file
**
** @param [r] thys [const AjPGraph] Graph object
** @return [AjBool] ajTrue if the graph is of type data
** @@
******************************************************************************/

AjBool ajGraphIsData(const AjPGraph thys)
{
    return thys->isdata;
}




/* @funcstatic GraphColourBack *************************************************
**
** Set the background colour to either black or white depending on the
** current settings.
**
** @return [void]
** @@
******************************************************************************/

static void GraphColourBack(void)
{
    ajDebug("GraphColourBack currentbgwhite: %B\n", currentbgwhite);

    if(currentbgwhite)
    {
	GraphPen(0, 255, 255, 255);
	GraphPen(MAXCOL, 0, 0, 0);

	graphColourName[0] = "WHITE";
	graphColourName[MAXCOL] = "BLACK";

	ajDebug("GraphColourBack pens 0 '%s' and pen %d '%s'\n",
		graphColourName[0], MAXCOL, graphColourName[MAXCOL]);
    }

    return;
}


/* @func ajGraphicsSetBgcolourReset ********************************************
**
** Reset the background colour to either black or white depending on the
** current settings.
**
** @return [void]
** @@
******************************************************************************/

void ajGraphicsSetBgcolourReset(void)
{
    GraphColourBack();
    return;
}


/* @obsolete ajGraphColourBack
** @rename ajGraphicsSetBgcolourReset
*/

__deprecated void ajGraphColourBack(void)
{
    GraphColourBack();
    return;
}


/* @func ajGraphUnused ********************************************************
**
** Unused functions to avoid compiler warnings
**
** @return [void]
** @@
******************************************************************************/

void ajGraphUnused(void)
{
    float f = 0.0;
    ajint i = 0;
    AjPGraph thys = NULL;
    AjPGraphdata graphdata = NULL;

    GraphPrint(thys);
    GraphDataPrint(graphdata);
    GraphCheckPoints(0, &f, &f);
    GraphCheckFlags(0);
    GraphArrayGapsI(0, &i, &i);

    return;
}

#endif
