/* $Id: plplot.h,v 1.13 2006/08/31 11:25:43 arjenmarkus Exp $

    Macros and prototypes for the PLplot package.  This header file must
    be included by all user codes.

    Note: some systems allow the Fortran & C namespaces to clobber each
    other.  So for PLplot to work from Fortran, we do some rather nasty
    things to the externally callable C function names.  This shouldn't
    affect any user programs in C as long as this file is included.

    Copyright (C) 1992  Maurice J. LeBrun, Geoff Furnish, Tony Richardson.
    Copyright (C) 2004  Alan W. Irwin
    Copyright (C) 2004  Rafael Laboissiere
    Copyright (C) 2004  Andrew Ross

    This file is part of PLplot.

    PLplot is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License as published
    by the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    PLplot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with PLplot; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef __PLPLOT_H__
#define __PLPLOT_H__

#include "plConfig.h"

/*--------------------------------------------------------------------------*\
 *    USING PLplot
 *
 * To use PLplot from C or C++, it is only necessary to
 *
 *      #include "plplot.h"
 *
 * This file does all the necessary setup to make PLplot accessible to
 * your program as documented in the manual.  Additionally, this file
 * allows you to request certain behavior by defining certain symbols
 * before inclusion.  At the moment the only one is:
 *
 * #define DOUBLE       or..
 * #define PL_DOUBLE
 *
 * This causes PLplot to use doubles instead of floats.  Use the type
 * PLFLT everywhere in your code, and it will always be the right thing.
 *
 * Note: most of the functions visible here begin with "pl", while all
 * of the data types and switches begin with "PL".  Eventually everything
 * will conform to this rule in order to keep namespace pollution of the
 * user code to a minimum.  All the PLplot source files actually include
 * "plplotP.h", which includes this file as well as all the internally-
 * visible declarations, etc.
\*--------------------------------------------------------------------------*/

/* The majority of PLplot source files require these, so.. */
/* Under ANSI C, they can be included any number of times. */

#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------------------*\
 *        SYSTEM IDENTIFICATION
 *
 * Several systems are supported directly by PLplot.  In order to avoid
 * confusion, one id macro per system is used.  Since different compilers
 * may predefine different system id macros, we need to check all the
 * possibilities, and then set the one we will be referencing.  These are:
 *
 * __cplusplus                Any C++ compiler
 * __unix                     Any Unix-like system
 * __hpux                     Any HP/UX system
 * __aix                      Any AIX system
 * __linux                    Linux for i386
 * (others...)
 *
\*--------------------------------------------------------------------------*/

#ifdef unix                     /* the old way */
#ifndef __unix
#define __unix
#endif
#endif

/* Make sure Unix systems define "__unix" */

#if defined(SX) ||                              /* NEC Super-UX */      \
    (defined(_IBMR2) && defined(_AIX)) ||       /* AIX */               \
    defined(__hpux) ||                          /* HP/UX */             \
    defined(sun) ||                             /* SUN */               \
    defined(CRAY) ||                            /* Cray */              \
    defined(__convexc__) ||                     /* CONVEX */            \
    (defined(__alpha) && defined(__osf__))      /* DEC Alpha AXP/OSF */

#ifndef __unix
#define __unix
#endif
#endif

#ifdef WIN32
#define __noreturn
#endif

/* A wrapper used in some header files so they can be compiled with cc */

#define PLARGS(a)       a

/*--------------------------------------------------------------------------*\
 * Base types for PLplot
 *
 * Only those that are necessary for function prototypes are defined here.
 * Notes:
 *
 * PLINT is typedef'd to a long by default.  This is a change from some
 * previous versions, where a int was used.  However, so long as you have
 * used type PLINT for integer array arguments as specified by the API,
 * this change will be transparent for you.
 *
 * short is currently used for device page coordinates, so they are
 * bounded by (-32767, 32767).  This gives a max resolution of about 3000
 * dpi, and improves performance in some areas over using a PLINT.
 *
 * PLUNICODE should be a 32-bit unsigned integer on all platforms.
 * For now, we are using unsigned int for our Linux ix86 unicode experiments,
 * but that doesn't guarantee 32 bits exactly on all platforms so this will
 * be subject to change.
\*--------------------------------------------------------------------------*/

#if defined(PL_DOUBLE) || defined(DOUBLE)
typedef double PLFLT;
#define PLFLT_MAX DBL_MAX
#define PLFLT_MIN DBL_MIN
#else
typedef float PLFLT;
#define PLFLT_MAX  FLT_MAX
#define PLFLT_MIN  FLT_MIN
#endif

typedef long PLINT;

#if defined(WIN32)
#if defined(__PLSTUBS_H__)
#define API WINAPI
#else
#define API
#endif
#include <windows.h>
#else
#define API
#endif


/* Subject to change. */
#if defined(HAVE_STDINT_H)
#include <stdint.h>
/* This is apparently portable if stdint.h exists. */
typedef uint32_t PLUNICODE;
#else
typedef unsigned int PLUNICODE;
#endif

/* For identifying logical (boolean) arguments */
typedef PLINT PLBOOL;

/* For passing user data, as with X's XtPointer */

typedef void* PLPointer;

/*--------------------------------------------------------------------------*\
 * Complex data types and other good stuff
\*--------------------------------------------------------------------------*/

/* Switches for escape function call. */
/* Some of these are obsolete but are retained in order to process
   old metafiles */

#define PLESC_SET_RGB           1       /* obsolete */
#define PLESC_ALLOC_NCOL        2       /* obsolete */
#define PLESC_SET_LPB           3       /* obsolete */
#define PLESC_EXPOSE            4       /* handle window expose */
#define PLESC_RESIZE            5       /* handle window resize */
#define PLESC_REDRAW            6       /* handle window redraw */
#define PLESC_TEXT              7       /* switch to text screen */
#define PLESC_GRAPH             8       /* switch to graphics screen */
#define PLESC_FILL              9       /* fill polygon */
#define PLESC_DI                10      /* handle DI command */
#define PLESC_FLUSH             11      /* flush output */
#define PLESC_EH                12      /* handle Window events */
#define PLESC_GETC              13      /* get cursor position */
#define PLESC_SWIN              14      /* set window parameters */
#define PLESC_DOUBLEBUFFERING	15	/* configure double buffering */
#define PLESC_XORMOD		16	/* set xor mode */
#define PLESC_SET_COMPRESSION	17	/* AFR: set compression */
#define PLESC_CLEAR		18      /* RL: clear graphics region */
#define PLESC_DASH		19	/* RL: draw dashed line */
#define PLESC_HAS_TEXT		20	/* driver draws text */
#define PLESC_IMAGE		21	/* handle image */
#define PLESC_IMAGEOPS          22      /* plimage related operations */
#define PLESC_PL2DEVCOL		23	/* convert PLColor to device color */
#define PLESC_DEV2PLCOL		24	/* convert device color to PLColor */
#define PLESC_SETBGFG		25	/* set BG, FG colors */
#define PLESC_DEVINIT		26	/* alternate device initialization */

/* image operations */
#define ZEROW2B   1
#define ZEROW2D   2
#define ONEW2B    3
#define ONEW2D    4

/* Window parameter tags */

#define PLSWIN_DEVICE           1       /* device coordinates */
#define PLSWIN_WORLD            2       /* world coordinates */

/* PLplot Option table & support constants */

/* Option-specific settings */

#define PL_OPT_ENABLED          0x0001  /* Obsolete */
#define PL_OPT_ARG              0x0002  /* Option has an argment */
#define PL_OPT_NODELETE         0x0004  /* Don't delete after processing */
#define PL_OPT_INVISIBLE        0x0008  /* Make invisible */
#define PL_OPT_DISABLED         0x0010  /* Processing is disabled */

/* Option-processing settings -- mutually exclusive */

#define PL_OPT_FUNC             0x0100  /* Call handler function */
#define PL_OPT_BOOL             0x0200  /* Set *var = 1 */
#define PL_OPT_INT              0x0400  /* Set *var = atoi(optarg) */
#define PL_OPT_FLOAT            0x0800  /* Set *var = atof(optarg) */
#define PL_OPT_STRING           0x1000  /* Set var = optarg */

/* Global mode settings */
/* These override per-option settings */

#define PL_PARSE_PARTIAL        0x0000  /* For backward compatibility */
#define PL_PARSE_FULL           0x0001  /* Process fully & exit if error */
#define PL_PARSE_QUIET          0x0002  /* Don't issue messages */
#define PL_PARSE_NODELETE       0x0004  /* Don't delete options after */
					/* processing */
#define PL_PARSE_SHOWALL        0x0008  /* Show invisible options */
#define PL_PARSE_OVERRIDE       0x0010  /* Obsolete */
#define PL_PARSE_NOPROGRAM      0x0020  /* Program name NOT in *argv[0].. */
#define PL_PARSE_NODASH         0x0040  /* Set if leading dash NOT required */
#define PL_PARSE_SKIP           0x0080  /* Skip over unrecognized args */

/* FCI (font characterization integer) related constants. */
#define PL_FCI_MARK 0x80000000
#define PL_FCI_IMPOSSIBLE 0x00000000
#define PL_FCI_HEXDIGIT_MASK 0xf
#define PL_FCI_HEXPOWER_MASK 0x7
#define PL_FCI_HEXPOWER_IMPOSSIBLE 0xf
/* These define hexpower values corresponding to each font attribute. */
#define PL_FCI_FAMILY  0x0
#define PL_FCI_STYLE  0x1
#define PL_FCI_WEIGHT  0x2
/* These are legal values for font family attribute */
#define PL_FCI_SANS 0x0
#define PL_FCI_SERIF 0x1
#define PL_FCI_MONO 0x2
#define PL_FCI_SCRIPT 0x3
#define PL_FCI_SYMBOL 0x4
/* These are legal values for font style attribute */
#define PL_FCI_UPRIGHT 0x0
#define PL_FCI_ITALIC 0x1
#define PL_FCI_OBLIQUE 0x2
/* These are legal values for font weight attribute */
#define PL_FCI_MEDIUM 0x0
#define PL_FCI_BOLD 0x1

/* Obsolete names */

#define plParseInternalOpts(a, b, c)    plparseopts(a, b, c)
#define plSetInternalOpt(a, b)          plSetOpt(a, b)

/* Option table definition */

typedef struct {
    char *opt;
    int  (*handler)     (char *, char *, void *);
    void *client_data;
    void *var;
    long mode;
    char *syntax;
    char *desc;
} PLOptionTable;

/* PLplot Graphics Input structure */

#define PL_MAXKEY 16

typedef struct {
    int type;                   /* of event (CURRENTLY UNUSED) */
    unsigned int state;         /* key or button mask */
    unsigned int keysym;        /* key selected */
    unsigned int button;        /* mouse button selected */
    PLINT subwindow;            /* subwindow (alias subpage, alias subplot) number */
    char string[PL_MAXKEY];     /* translated string */
    int pX, pY;                 /* absolute device coordinates of pointer */
    PLFLT dX, dY;		/* relative device coordinates of pointer */
    PLFLT wX, wY;               /* world coordinates of pointer */
} PLGraphicsIn;

/* Structure for describing the plot window */

#define PL_MAXWINDOWS   64      /* Max number of windows/page tracked */

typedef struct {
    PLFLT dxmi, dxma, dymi, dyma;	/* min, max window rel dev coords */
    PLFLT wxmi, wxma, wymi, wyma;       /* min, max window world coords */
} PLWindow;

/* Structure for doing display-oriented operations via escape commands */
/* May add other attributes in time */

typedef struct {
    unsigned int x, y;                  /* upper left hand corner */
    unsigned int width, height;         /* window dimensions */
} PLDisplay;

/* Macro used (in some cases) to ignore value of argument */
/* I don't plan on changing the value so you can hard-code it */

#define PL_NOTSET (-42)

/* See plcont.c for examples of the following */

/*
 * PLfGrid is for passing (as a pointer to the first element) an arbitrarily
 * dimensioned array.  The grid dimensions MUST be stored, with a maximum of 3
 * dimensions assumed for now.
 */

typedef struct {
    PLFLT *f;
    PLINT nx, ny, nz;
} PLfGrid;

/*
 * PLfGrid2 is for passing (as an array of pointers) a 2d function array.  The
 * grid dimensions are passed for possible bounds checking.
 */

typedef struct {
    PLFLT **f;
    PLINT nx, ny;
} PLfGrid2;

/*
 * NOTE: a PLfGrid3 is a good idea here but there is no way to exploit it yet
 * so I'll leave it out for now.
 */

/*
 * PLcGrid is for passing (as a pointer to the first element) arbitrarily
 * dimensioned coordinate transformation arrays.  The grid dimensions MUST be
 * stored, with a maximum of 3 dimensions assumed for now.
 */

typedef struct {
    PLFLT *xg, *yg, *zg;
    PLINT nx, ny, nz;
} PLcGrid;

/*
 * PLcGrid2 is for passing (as arrays of pointers) 2d coordinate
 * transformation arrays.  The grid dimensions are passed for possible bounds
 * checking.
 */

typedef struct {
    PLFLT **xg, **yg, **zg;
    PLINT nx, ny;
} PLcGrid2;

/*
 * NOTE: a PLcGrid3 is a good idea here but there is no way to exploit it yet
 * so I'll leave it out for now.
 */

/* PLColor is the usual way to pass an rgb color value. */

typedef struct {
    unsigned char r;            /* red */
    unsigned char g;            /* green */
    unsigned char b;            /* blue */
    char *name;
} PLColor;

/* PLControlPt is how cmap1 control points are represented. */

typedef struct {
    PLFLT h;                    /* hue */
    PLFLT l;                    /* lightness */
    PLFLT s;                    /* saturation */
    PLFLT p;                    /* position */
    int rev;                    /* if set, interpolate through h=0 */
} PLControlPt;

/* A PLBufferingCB is a control block for interacting with devices
   that support double buffering. */

typedef struct {
    PLINT cmd;
    PLINT result;
} PLBufferingCB;

#define PLESC_DOUBLEBUFFERING_ENABLE     1
#define PLESC_DOUBLEBUFFERING_DISABLE    2
#define PLESC_DOUBLEBUFFERING_QUERY      3


/*--------------------------------------------------------------------------*\
 *              BRAINDEAD-ness
 *
 * Some systems allow the Fortran & C namespaces to clobber each other.
 * For PLplot to work from Fortran on these systems, we must name the the
 * externally callable C functions something other than their Fortran entry
 * names.  In order to make this as easy as possible for the casual user,
 * yet reversible to those who abhor my solution, I have done the
 * following:
 *
 *      The C-language bindings are actually different from those
 *      described in the manual.  Macros are used to convert the
 *      documented names to the names used in this package.  The
 *      user MUST include plplot.h in order to get the name
 *      redefinition correct.
 *
 * Sorry to have to resort to such an ugly kludge, but it is really the
 * best way to handle the situation at present.  If all available
 * compilers offer a way to correct this stupidity, then perhaps we can
 * eventually reverse it.
 *
 * If you feel like screaming at someone (I sure do), please
 * direct it at your nearest system vendor who has a braindead shared
 * C/Fortran namespace.  Some vendors do offer compiler switches that
 * change the object names, but then everybody who wants to use the
 * package must throw these same switches, leading to no end of trouble.
 *
 * Note that this definition should not cause any noticable effects except
 * when debugging PLplot calls, in which case you will need to remember
 * the real function names (same as before but with a 'c_' prepended).
 *
 * Also, to avoid macro conflicts, the BRAINDEAD part must not be expanded
 * in the stub routines.
 *
 * Aside: the reason why a shared Fortran/C namespace is deserving of the
 * BRAINDEAD characterization is that it completely precludes the the kind
 * of universal API that is attempted (more or less) with PLplot, without
 * Herculean efforts (e.g. remapping all of the C bindings by macros as
 * done here).  The vendors of such a scheme, in order to allow a SINGLE
 * type of argument to be passed transparently between C and Fortran,
 * namely, a pointer to a conformable data type, have slammed the door on
 * insertion of stub routines to handle the conversions needed for other
 * data types.  Intelligent linkers could solve this problem, but these are
 * not anywhere close to becoming universal.  So meanwhile, one must live
 * with either stub routines for the inevitable data conversions, or a
 * different API.  The former is what is used here, but is made far more
 * difficult in a braindead shared Fortran/C namespace.
\*--------------------------------------------------------------------------*/

#ifndef BRAINDEAD
#define BRAINDEAD
#endif

#ifdef BRAINDEAD

#if !defined(__PLSTUBS_H__) && !defined(__PLDLL_H__)

#define    pl_setcontlabelformat c_pl_setcontlabelformat
#define    pl_setcontlabelparam c_pl_setcontlabelparam
#define    pladv        c_pladv
#define    plarrows     c_plarrows
#define    plaxes       c_plaxes
#define    plbin        c_plbin
#define    plbop        c_plbop
#define    plbox        c_plbox
#define    plbox3       c_plbox3
#define    plcalc_world	c_plcalc_world
#define    plclear	c_plclear
#define    plcol0       c_plcol0
#define    plcol1       c_plcol1
#define    plcont       c_plcont
#define    plcpstrm     c_plcpstrm
#define    plend        c_plend
#define    plend1       c_plend1
#define    plenv        c_plenv
#define    plenv0	c_plenv0
#define    pleop        c_pleop
#define    plerrx       c_plerrx
#define    plerry       c_plerry
#define    plfamadv     c_plfamadv
#define    plfill       c_plfill
#define    plfill3	c_plfill3
#define    plflush      c_plflush
#define    plfont       c_plfont
#define    plfontld     c_plfontld
#define    plgchr       c_plgchr
#define    plgcol0      c_plgcol0
#define    plgcolbg     c_plgcolbg
#define    plgcompression	c_plgcompression
#define    plgdev       c_plgdev
#define    plgdidev     c_plgdidev
#define    plgdiori     c_plgdiori
#define    plgdiplt     c_plgdiplt
#define    plgfam       c_plgfam
#define    plgfamadv    c_plgfamadv
#define    plgfci       c_plgfci
#define    plgfnam      c_plgfnam
#define    plglevel     c_plglevel
#define    plgpage      c_plgpage
#define    plgra        c_plgra
#define    plgriddata   c_plgriddata
#define    plgspa       c_plgspa
#define    plgstrm      c_plgstrm
#define    plgver       c_plgver
#define    plgvpd	c_plgvpd
#define    plgvpw	c_plgvpw
#define    plgxax       c_plgxax
#define    plgyax       c_plgyax
#define    plgzax       c_plgzax
#define    plhist       c_plhist
#define    plhls        c_plhls
#define    plhlsrgb     c_plhlsrgb
#define    plinit       c_plinit
#define    pljoin       c_pljoin
#define    pllab        c_pllab
#define    pllightsource	c_pllightsource
#define    plline       c_plline
#define    plline3      c_plline3
#define    pllsty       c_pllsty
#define    plmap        c_plmap
#define    plmeridians  c_plmeridians
#define    plmesh	c_plmesh
#define    plmeshc	c_plmeshc
#define    plmkstrm     c_plmkstrm
#define    plmtex       c_plmtex
#define    plot3d       c_plot3d
#define    plot3dc	c_plot3dc
#define    plot3dcl     c_plot3dcl
#define    plparseopts  c_plparseopts
#define    plpat        c_plpat
#define    plpoin       c_plpoin
#define    plpoin3      c_plpoin3
#define    plpoly3      c_plpoly3
#define    plprec       c_plprec
#define    plpsty       c_plpsty
#define    plptex       c_plptex
#define    plreplot     c_plreplot
#define    plrgb        c_plrgb
#define    plrgb1       c_plrgb1
#define    plrgbhls     c_plrgbhls
#define    plschr       c_plschr
#define    plscmap0     c_plscmap0
#define    plscmap0n    c_plscmap0n
#define    plscmap1	c_plscmap1
#define    plscmap1l    c_plscmap1l
#define    plscmap1n	c_plscmap1n
#define    plscol0      c_plscol0
#define    plscolbg     c_plscolbg
#define    plscolor     c_plscolor
#define    plscompression	c_plscompression
#define    plsdev       c_plsdev
#define    plsdidev     c_plsdidev
#define    plsdimap     c_plsdimap
#define    plsdiori     c_plsdiori
#define    plsdiplt	c_plsdiplt
#define    plsdiplz	c_plsdiplz
#define    plsesc       c_plsesc
#define    plsetopt	c_plsetopt
#define    plsfam       c_plsfam
#define    plxsfam      c_plxsfam
#define    plsfci       c_plsfci
#define    plsfnam      c_plsfnam
#define    plxsfnam      c_plxsfnam
#define    plshade      c_plshade
#define    plshade1     c_plshade1
#define    plshades	c_plshades
#define    plsmaj       c_plsmaj
#define    plsmem	c_plsmem
#define    plsmin       c_plsmin
#define    plsori       c_plsori
#define    plspage      c_plspage
#define    plspause     c_plspause
#define    plsstrm      c_plsstrm
#define    plssub       c_plssub
#define    plssym       c_plssym
#define    plstar       c_plstar
#define    plstart      c_plstart
#define    plstripa	c_plstripa
#define    plstripc	c_plstripc
#define    plstripd	c_plstripd
#define    plstyl       c_plstyl
#define    plsurf3d	c_plsurf3d
#define    plsurf3dl    c_plsurf3dl
#define    plsvect      c_plsvect
#define    plsvpa       c_plsvpa
#define    plsxax       c_plsxax
#define    plsyax       c_plsyax
#define    plsym        c_plsym
#define    plszax       c_plszax
#define    pltext       c_pltext
#define    plvasp       c_plvasp
#define    plvect       c_plvect
#define    plvpas       c_plvpas
#define    plvpor       c_plvpor
#define    plvsta       c_plvsta
#define    plw3d        c_plw3d
#define    plwid        c_plwid
#define    plwind       c_plwind
#define    plxormod	c_plxormod

#else

#if defined(__PLDLL_H__)

#define API WINAPI

#define    c_pl_setcontlabelformat pl_setcontlabelformat
#define    c_pl_setcontlabelparam pl_setcontlabelparam
#define    c_pladv          pladv
#define    c_plarrows       plarrows
#define    c_plaxes         plaxes
#define    c_plbin          plbin
#define    c_plbop          plbop
#define    c_plbox          plbox
#define    c_plbox3         plbox3
#define    c_plcalc_world   plcalc_world
#define    c_plclear        plclear
#define    c_plcol0         plcol0
#define    c_plcol1         plcol1
#define    c_plcont         plcont
#define    c_plcpstrm       plcpstrm
#define    c_plend          plend
#define    c_plend1         plend1
#define    c_plenv          plenv
#define    c_plenv0         plenv0
#define    c_pleop          pleop
#define    c_plerrx         plerrx
#define    c_plerry         plerry
#define    c_plfamadv       plfamadv
#define    c_plfill         plfill
#define    c_plfill3        plfill3
#define    c_plflush        plflush
#define    c_plfont         plfont
#define    c_plfontld       plfontld
#define    c_plgchr         plgchr
#define    c_plgcol0        plgcol0
#define    c_plgcolbg       plgcolbg
#define    c_plgcompression plgcompression
#define    c_plgdev         plgdev
#define    c_plgdidev       plgdidev
#define    c_plgdiori       plgdiori
#define    c_plgdiplt       plgdiplt
#define    c_plgfam         plgfam
#define    c_plgfci         plgfci
#define    c_plgfnam        plgfnam
#define    c_plglevel       plglevel
#define    c_plgpage        plgpage
#define    c_plgra          plgra
#define    c_plgriddata     plgriddata
#define    c_plgspa         plgspa
#define    c_plgstrm        plgstrm
#define    c_plgver         plgver
#define    c_plgvpd         plgvpd
#define    c_plgvpw         plgvpw
#define    c_plgxax         plgxax
#define    c_plgyax         plgyax
#define    c_plgzax         plgzax
#define    c_plhist         plhist
#define    c_plhls          plhls
#define    c_plhlsrgb       plhlsrgb
#define    c_plinit         plinit
#define    c_pljoin         pljoin
#define    c_pllab          pllab
#define    c_pllightsource  pllightsource
#define    c_plline         plline
#define    c_plline3        plline3
#define    c_pllsty         pllsty
#define    c_plmesh         plmesh
#define    c_plmeshc        plmeshc
#define    c_plmkstrm       plmkstrm
#define    c_plmtex         plmtex
#define    c_plot3d         plot3d
#define    c_plot3dc        plot3dc
#define    c_plot3dcl       plot3dcl
#define    c_plpat          plpat
#define    c_plpoin         plpoin
#define    c_plpoin3        plpoin3
#define    c_plpoly3        plpoly3
#define    c_plprec         plprec
#define    c_plpsty         plpsty
#define    c_plptex         plptex
#define    c_plreplot       plreplot
#define    c_plrgb          plrgb
#define    c_plrgb1         plrgb1
#define    c_plrgbhls       plrgbhls
#define    c_plschr         plschr
#define    c_plscmap0       plscmap0
#define    c_plscmap0n      plscmap0n
#define    c_plscmap1       plscmap1
#define    c_plscmap1l      plscmap1l
#define    c_plscmap1n      plscmap1n
#define    c_plscol0        plscol0
#define    c_plscolbg       plscolbg
#define    c_plscolor       plscolor
#define    c_plscompression plscompression
#define    c_plsdev         plsdev
#define    c_plsdidev       plsdidev
#define    c_plsdimap       plsdimap
#define    c_plsdiori       plsdiori
#define    c_plsdiplt       plsdiplt
#define    c_plsdiplz       plsdiplz
#define    c_plsesc         plsesc
#define    c_plsetopt       plsetopt
#define    c_plsfam         plsfam
#define    c_plsfci         plsfci
#define    c_plsfnam        plsfnam
#define    c_plxsfnam       plxsfnam
#define    c_plshade        plshade
#define    c_plshade1       plshade1
#define    c_plshades       plshades
#define    c_plsmaj         plsmaj
#define    c_plsmem         plsmem
#define    c_plsmin         plsmin
#define    c_plsori         plsori
#define    c_plspage        plspage
#define    c_plspause       plspause
#define    c_plsstrm        plsstrm
#define    c_plssub         plssub
#define    c_plssym         plssym
#define    c_plstar         plstar
#define    c_plstart        plstart
#define    c_plstripa       plstripa
#define    c_plstripc       plstripc
#define    c_plstripd       plstripd
#define    c_plstyl         plstyl
#define    c_plsurf3d       plsurf3d
#define    c_plsurf3dl      plsurf3dl
#define    c_plsvect        plsvect
#define    c_plsvpa         plsvpa
#define    c_plsxax         plsxax
#define    c_plsyax         plsyax
#define    c_plsym          plsym
#define    c_plszax         plszax
#define    c_pltext         pltext
#define    c_plvasp         plvasp
#define    c_plvect         plvect
#define    c_plvpas         plvpas
#define    c_plvpor         plvpor
#define    c_plvsta         plvsta
#define    c_plw3d          plw3d
#define    c_plwid          plwid
#define    c_plwind         plwind
#define    c_plxormod       plxormod

#define    plAlloc2dGrid    win_plAlloc2dGrid
#define    plFree2dGrid     win_plFree2dGrid
#define    plGetCursor      win_plGetCursor
#define    c_plparseopts    win_plParseOpts
#define    plparseopts      win_plParseOpts
#define    plMergeOpts      win_plMergeOpts

#else
#define API
#endif

#endif /* __PLSTUBS_H__ */

#endif  /* BRAINDEAD */

/* Redefine some old function names for backward compatibility */

#ifndef __PLSTUBS_H__   /* i.e. do not expand this in the stubs */

#define    plclr        pleop
#define    plpage       plbop
#define    plcol        plcol0
#define    plcontf      plfcont
#define    Alloc2dGrid  plAlloc2dGrid
#define    Free2dGrid   plFree2dGrid
#define    MinMax2dGrid plMinMax2dGrid
#define    plP_gvpd	plgvpd
#define    plP_gvpw	plgvpw
#define    plotsh3d(x,y,z,nx,ny,opt)     plsurf3d(x,y,z,nx,ny,opt, NULL, 0)
#define    plHLS_RGB    plhlsrgb
#define    plRGB_HLS    plrgbhls

#endif /* __PLSTUBS_H__ */

/*--------------------------------------------------------------------------*\
 *              Function Prototypes
\*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

	/* All void types */

	/* C routines callable from stub routines come first */

/* set the format of the contour labels */

void API
c_pl_setcontlabelformat(PLINT lexp, PLINT sigdig);

/* set offset and spacing of contour labels */

void API
c_pl_setcontlabelparam(PLFLT offset, PLFLT size, PLFLT spacing, PLINT active);

/* Advance to subpage "page", or to the next one if "page" = 0. */

void API
c_pladv(PLINT page);

/* simple arrow plotter. */

void API
c_plarrows(PLFLT *u, PLFLT *v, PLFLT *x, PLFLT *y, PLINT n,
           PLFLT scale, PLFLT dx, PLFLT dy) ;

void API
c_plvect(PLFLT **u, PLFLT **v, PLINT nx, PLINT ny, PLFLT scale,
void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
              PLPointer pltr_data);

void API
c_plsvect(PLFLT *arrowx, PLFLT *arrowy, PLINT npts, PLINT fill);

/* This functions similarly to plbox() except that the origin of the axes */
/* is placed at the user-specified point (x0, y0). */

void API
c_plaxes(PLFLT x0, PLFLT y0, const char *xopt, PLFLT xtick, PLINT nxsub,
	 const char *yopt, PLFLT ytick, PLINT nysub);

/* Plot a histogram using x to store data values and y to store frequencies */

/* Flags for plbin() - opt argument */
#define PL_BIN_DEFAULT          0
#define PL_BIN_CENTRED          1
#define PL_BIN_NOEXPAND         2
#define PL_BIN_NOEMPTY          4

void API
c_plbin(PLINT nbin, PLFLT *x, PLFLT *y, PLINT center);

/* Start new page.  Should only be used with pleop(). */

void API
c_plbop(void);

/* This draws a box around the current viewport. */

void API
c_plbox(const char *xopt, PLFLT xtick, PLINT nxsub,
	const char *yopt, PLFLT ytick, PLINT nysub);

/* This is the 3-d analogue of plbox(). */

void API
c_plbox3(const char *xopt, const char *xlabel, PLFLT xtick, PLINT nsubx,
	 const char *yopt, const char *ylabel, PLFLT ytick, PLINT nsuby,
	 const char *zopt, const char *zlabel, PLFLT ztick, PLINT nsubz);

/* Calculate world coordinates and subpage from relative device coordinates. */

void API
c_plcalc_world(PLFLT rx, PLFLT ry, PLFLT *wx, PLFLT *wy, PLINT *window);

/* Clear current subpage. */

void API
c_plclear(void);

/* Set color, map 0.  Argument is integer between 0 and 15. */

void API
c_plcol0(PLINT icol0);

/* Set color, map 1.  Argument is a float between 0. and 1. */

void API
c_plcol1(PLFLT col1);

/* Draws a contour plot from data in f(nx,ny).  Is just a front-end to
 * plfcont, with a particular choice for f2eval and f2eval_data.
 */

void API
c_plcont(PLFLT **f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
	 PLINT ky, PLINT ly, PLFLT *clevel, PLINT nlevel,
	 void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	 PLPointer pltr_data);

/* Draws a contour plot using the function evaluator f2eval and data stored
 * by way of the f2eval_data pointer.  This allows arbitrary organizations
 * of 2d array data to be used.
 */

void API
plfcont(PLFLT (*f2eval) (PLINT, PLINT, PLPointer),
	PLPointer f2eval_data,
	PLINT nx, PLINT ny, PLINT kx, PLINT lx,
	PLINT ky, PLINT ly, PLFLT *clevel, PLINT nlevel,
	void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	PLPointer pltr_data);

/* Copies state parameters from the reference stream to the current stream. */

void API
c_plcpstrm(PLINT iplsr, PLINT flags);

/* Converts input values from relative device coordinates to relative plot */
/* coordinates. */

void API
pldid2pc(PLFLT *xmin, PLFLT *ymin, PLFLT *xmax, PLFLT *ymax);

/* Converts input values from relative plot coordinates to relative */
/* device coordinates. */

void API
pldip2dc(PLFLT *xmin, PLFLT *ymin, PLFLT *xmax, PLFLT *ymax);

/* End a plotting session for all open streams. */

void API
c_plend(void);

/* End a plotting session for the current stream only. */

void API
c_plend1(void);

/* Simple interface for defining viewport and window. */

void API
c_plenv(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	PLINT just, PLINT axis);


/* similar to plenv() above, but in multiplot mode does not advance the subpage,
 instead the current subpage is cleared */

void API
c_plenv0(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	PLINT just, PLINT axis);

/* End current page.  Should only be used with plbop(). */

void API
c_pleop(void);

/* Plot horizontal error bars (xmin(i),y(i)) to (xmax(i),y(i)) */

void API
c_plerrx(PLINT n, PLFLT *xmin, PLFLT *xmax, PLFLT *y);

/* Plot vertical error bars (x,ymin(i)) to (x(i),ymax(i)) */

void API
c_plerry(PLINT n, PLFLT *x, PLFLT *ymin, PLFLT *ymax);

/* Advance to the next family file on the next new page */

void API
c_plfamadv(void);

/* Pattern fills the polygon bounded by the input points. */

void API
c_plfill(PLINT n, PLFLT *x, PLFLT *y);

/* Pattern fills the 3d polygon bounded by the input points. */

void API
c_plfill3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z);

/* Flushes the output stream.  Use sparingly, if at all. */

void API
c_plflush(void);

/* Sets the global font flag to 'ifont'. */

void API
c_plfont(PLINT ifont);

/* Load specified font set. */

void API
c_plfontld(PLINT fnt);

/* Get character default height and current (scaled) height */

void API
c_plgchr(PLFLT *p_def, PLFLT *p_ht);

/* Returns 8 bit RGB values for given color from color map 0 */

void API
c_plgcol0(PLINT icol0, PLINT *r, PLINT *g, PLINT *b);

/* Returns the background color by 8 bit RGB value */

void API
c_plgcolbg(PLINT *r, PLINT *g, PLINT *b);

/* Returns the current compression setting */

void API
c_plgcompression(PLINT *compression);

/* Get the current device (keyword) name */

void API
c_plgdev(char *p_dev);

/* Retrieve current window into device space */

void API
c_plgdidev(PLFLT *p_mar, PLFLT *p_aspect, PLFLT *p_jx, PLFLT *p_jy);

/* Get plot orientation */

void API
c_plgdiori(PLFLT *p_rot);

/* Retrieve current window into plot space */

void API
c_plgdiplt(PLFLT *p_xmin, PLFLT *p_ymin, PLFLT *p_xmax, PLFLT *p_ymax);

/* Get FCI (font characterization integer) */

void API
c_plgfci(PLUNICODE *pfci);

/* Get family file parameters */

void API
c_plgfam(PLINT *p_fam, PLINT *p_num, PLINT *p_bmax);

/* Get the (current) output file name.  Must be preallocated to >80 bytes */

void API
c_plgfnam(char *fnam);

/* Get the (current) run level.  */

void API
c_plglevel(PLINT *p_level);

/* Get output device parameters. */

void API
c_plgpage(PLFLT *p_xp, PLFLT *p_yp,
	  PLINT *p_xleng, PLINT *p_yleng, PLINT *p_xoff, PLINT *p_yoff);

/* Switches to graphics screen. */

void API
c_plgra(void);

  /* grid irregularly sampled data */

void API
c_plgriddata(PLFLT *x, PLFLT *y, PLFLT *z, PLINT npts,
	   PLFLT *xg, PLINT nptsx, PLFLT *yg,  PLINT nptsy,
	   PLFLT **zg, PLINT type, PLFLT data);

  /* type of gridding algorithm for plgriddata() */

#define GRID_CSA    1 /* Bivariate Cubic Spline approximation */
#define GRID_DTLI   2 /* Delaunay Triangulation Linear Interpolation */
#define GRID_NNI    3 /* Natural Neighbors Interpolation */
#define GRID_NNIDW  4 /* Nearest Neighbors Inverse Distance Weighted */
#define GRID_NNLI   5 /* Nearest Neighbors Linear Interpolation */
#define GRID_NNAIDW 6 /* Nearest Neighbors Around Inverse Distance Weighted  */

/* Get subpage boundaries in absolute coordinates */

void API
c_plgspa(PLFLT *xmin, PLFLT *xmax, PLFLT *ymin, PLFLT *ymax);

/* Get current stream number. */

void API
c_plgstrm(PLINT *p_strm);

/* Get the current library version number */

void API
c_plgver(char *p_ver);

/* Get viewport boundaries in normalized device coordinates */

void API
c_plgvpd(PLFLT *p_xmin, PLFLT *p_xmax, PLFLT *p_ymin, PLFLT *p_ymax);

/* Get viewport boundaries in world coordinates */

void API
c_plgvpw(PLFLT *p_xmin, PLFLT *p_xmax, PLFLT *p_ymin, PLFLT *p_ymax);

/* Get x axis labeling parameters */

void API
c_plgxax(PLINT *p_digmax, PLINT *p_digits);

/* Get y axis labeling parameters */

void API
c_plgyax(PLINT *p_digmax, PLINT *p_digits);

/* Get z axis labeling parameters */

void API
c_plgzax(PLINT *p_digmax, PLINT *p_digits);

/* Draws a histogram of n values of a variable in array data[0..n-1] */

/* Flags for plhist() - opt argument; note: some flags are passed to
   plbin() for the actual plotting */
#define PL_HIST_DEFAULT         0
#define PL_HIST_NOSCALING       1
#define PL_HIST_IGNORE_OUTLIERS 2
#define PL_HIST_NOEXPAND        8
#define PL_HIST_NOEMPTY        16

void API
c_plhist(PLINT n, PLFLT *data, PLFLT datmin, PLFLT datmax,
	 PLINT nbin, PLINT opt);

/* Set current color (map 0) by hue, lightness, and saturation. */

void API
c_plhls(PLFLT h, PLFLT l, PLFLT s);

/* Functions for converting between HLS and RGB color space */

void API
c_plhlsrgb(PLFLT h, PLFLT l, PLFLT s, PLFLT *p_r, PLFLT *p_g, PLFLT *p_b);

/* Initializes PLplot, using preset or default options */

void API
c_plinit(void);

/* Draws a line segment from (x1, y1) to (x2, y2). */

void API
c_pljoin(PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2);

/* Simple routine for labelling graphs. */

void API
c_pllab(const char *xlabel, const char *ylabel, const char *tlabel);

/* Sets position of the light source */
void API
c_pllightsource(PLFLT x, PLFLT y, PLFLT z);

/* Draws line segments connecting a series of points. */

void API
c_plline(PLINT n, PLFLT *x, PLFLT *y);

/* Draws a line in 3 space.  */

void API
c_plline3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z);

/* Set line style. */

void API
c_pllsty(PLINT lin);

/* plot continental outline in world coordinates */

void API
plmap(void (*mapform)(PLINT, PLFLT *, PLFLT *), char *type,
      PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat);

/* Plot the latitudes and longitudes on the background. */

void API
plmeridians(void (*mapform)(PLINT, PLFLT *, PLFLT *),
	    PLFLT dlong, PLFLT dlat,
	    PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat);

/* Plots a mesh representation of the function z[x][y]. */

void API
c_plmesh(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt);

/* Plots a mesh representation of the function z[x][y] with contour */

void API
c_plmeshc(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt,
	  PLFLT *clevel, PLINT nlevel);

/* Creates a new stream and makes it the default.  */

void API
c_plmkstrm(PLINT *p_strm);

/* Prints out "text" at specified position relative to viewport */

void API
c_plmtex(const char *side, PLFLT disp, PLFLT pos, PLFLT just,
	 const char *text);

/* Plots a 3-d representation of the function z[x][y]. */

void API
c_plot3d(PLFLT *x, PLFLT *y, PLFLT **z,
	 PLINT nx, PLINT ny, PLINT opt, PLINT side);

/* Plots a 3-d representation of the function z[x][y] with contour. */

void API
c_plot3dc(PLFLT *x, PLFLT *y, PLFLT **z,
	 PLINT nx, PLINT ny, PLINT opt,
	 PLFLT *clevel, PLINT nlevel);

/* Plots a 3-d representation of the function z[x][y] with contour and
 * y index limits. */

void
c_plot3dcl(PLFLT *x, PLFLT *y, PLFLT **z,
         PLINT nx, PLINT ny, PLINT opt,
         PLFLT *clevel, PLINT nlevel,
         PLINT ixstart, PLINT ixn, PLINT *indexymin, PLINT*indexymax);

/*
 * definitions for the opt argument in plot3dc() and plsurf3d()
 *
 * DRAW_LINEX *must* be 1 and DRAW_LINEY *must* be 2, because of legacy code!
 */

#define DRAW_LINEX  (1 << 0) /* draw lines parallel to the X axis */
#define DRAW_LINEY  (1 << 1) /* draw lines parallel to the Y axis */
#define DRAW_LINEXY (DRAW_LINEX | DRAW_LINEY) /* draw lines parallel to both the X and Y axis */
#define MAG_COLOR   (1 << 2) /* draw the mesh with a color dependent of the magnitude */
#define BASE_CONT   (1 << 3) /* draw contour plot at bottom xy plane */
#define TOP_CONT    (1 << 4) /* draw contour plot at top xy plane */
#define SURF_CONT   (1 << 5) /* draw contour plot at surface */
#define DRAW_SIDES  (1 << 6) /* draw sides */
#define FACETED     (1 << 7) /* draw outline for each square that makes up the surface */
#define MESH        (1 << 8) /* draw mesh */

  /*
   *  valid options for plot3dc():
   *
   *  DRAW_SIDES, BASE_CONT, TOP_CONT (not yet),
   *  MAG_COLOR, DRAW_LINEX, DRAW_LINEY, DRAW_LINEXY.
   *
   *  valid options for plsurf3d():
   *
   *  MAG_COLOR, BASE_CONT, SURF_CONT, FACETED, DRAW_SIDES.
   */

/* Set fill pattern directly. */

void API
c_plpat(PLINT nlin, PLINT *inc, PLINT *del);

/* Plots array y against x for n points using ASCII code "code".*/

void API
c_plpoin(PLINT n, PLFLT *x, PLFLT *y, PLINT code);

/* Draws a series of points in 3 space. */

void API
c_plpoin3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT code);

/* Draws a polygon in 3 space.  */

void API
c_plpoly3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT *draw, PLINT ifcc);

/* Set the floating point precision (in number of places) in numeric labels. */

void API
c_plprec(PLINT setp, PLINT prec);

/* Set fill pattern, using one of the predefined patterns.*/

void API
c_plpsty(PLINT patt);

/* Prints out "text" at world cooordinate (x,y). */

void API
c_plptex(PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just, const char *text);

/* Replays contents of plot buffer to current device/file. */

void API
c_plreplot(void);

/* Set line color by red, green, blue from  0. to 1. */

void API
c_plrgb(PLFLT r, PLFLT g, PLFLT b);

/* Set line color by 8 bit RGB values. */

void API
c_plrgb1(PLINT r, PLINT g, PLINT b);

/* Functions for converting between HLS and RGB color space */

void API
c_plrgbhls(PLFLT r, PLFLT g, PLFLT b, PLFLT *p_h, PLFLT *p_l, PLFLT *p_s);

/* Set character height. */

void API
c_plschr(PLFLT def, PLFLT scale);

/* Set color map 0 colors by 8 bit RGB values */

void API
c_plscmap0(PLINT *r, PLINT *g, PLINT *b, PLINT ncol0);

/* Set number of colors in cmap 0 */

void API
c_plscmap0n(PLINT ncol0);

/* Set color map 1 colors by 8 bit RGB values */

void API
c_plscmap1(PLINT *r, PLINT *g, PLINT *b, PLINT ncol1);

/* Set color map 1 colors using a piece-wise linear relationship between */
/* intensity [0,1] (cmap 1 index) and position in HLS or RGB color space. */

void API
c_plscmap1l(PLBOOL itype, PLINT npts, PLFLT *intensity,
	    PLFLT *coord1, PLFLT *coord2, PLFLT *coord3, PLBOOL *rev);

/* Set number of colors in cmap 1 */

void API
c_plscmap1n(PLINT ncol1);

/* Set a given color from color map 0 by 8 bit RGB value */

void API
c_plscol0(PLINT icol0, PLINT r, PLINT g, PLINT b);

/* Set the background color by 8 bit RGB value */

void API
c_plscolbg(PLINT r, PLINT g, PLINT b);

/* Used to globally turn color output on/off */

void API
c_plscolor(PLINT color);

/* Set the compression level */

void API
c_plscompression(PLINT compression);

/* Set the device (keyword) name */

void API
c_plsdev(const char *devname);

/* Set window into device space using margin, aspect ratio, and */
/* justification */

void API
c_plsdidev(PLFLT mar, PLFLT aspect, PLFLT jx, PLFLT jy);

/* Set up transformation from metafile coordinates. */

void API
c_plsdimap(PLINT dimxmin, PLINT dimxmax, PLINT dimymin, PLINT dimymax,
	   PLFLT dimxpmm, PLFLT dimypmm);

/* Set plot orientation, specifying rotation in units of pi/2. */

void API
c_plsdiori(PLFLT rot);

/* Set window into plot space */

void API
c_plsdiplt(PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax);

/* Set window into plot space incrementally (zoom) */

void API
c_plsdiplz(PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax);

/* Set the escape character for text strings. */

void API
c_plsesc(char esc);

/* Set family file parameters */

void API
c_plsfam(PLINT fam, PLINT num, PLINT bmax);
/* Set FCI (font characterization integer) */

void API
c_plsfci(PLUNICODE fci);

/* Set the output file name. */

void API
c_plsfnam(const char *fnam);

void API
c_plxsfnam(const char *fnam, const char *ext);


/* Shade region. */

void API
c_plshade(PLFLT **a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
	  PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	  PLFLT shade_min, PLFLT shade_max,
	  PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
	  PLINT min_color, PLINT min_width,
	  PLINT max_color, PLINT max_width,
	  void (API *fill) (PLINT, PLFLT *, PLFLT *), PLBOOL rectangular,
	  void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	  PLPointer pltr_data);

void API
c_plshade1(PLFLT *a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
	 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	 PLFLT shade_min, PLFLT shade_max,
	 PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
	 PLINT min_color, PLINT min_width,
	 PLINT max_color, PLINT max_width,
	 void (*fill) (PLINT, PLFLT *, PLFLT *), PLBOOL rectangular,
	 void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	 PLPointer pltr_data);

void API
c_plshades( PLFLT **a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
	  PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	  PLFLT *clevel, PLINT nlevel, PLINT fill_width,
	  PLINT cont_color, PLINT cont_width,
	  void (*fill) (PLINT, PLFLT *, PLFLT *), PLINT rectangular,
	  void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	  PLPointer pltr_data);

void API
plfshade(PLFLT (*f2eval) (PLINT, PLINT, PLPointer),
	 PLPointer f2eval_data,
	 PLFLT (*c2eval) (PLINT, PLINT, PLPointer),
	 PLPointer c2eval_data,
	 PLINT nx, PLINT ny,
	 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	 PLFLT shade_min, PLFLT shade_max,
	 PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
	 PLINT min_color, PLINT min_width,
	 PLINT max_color, PLINT max_width,
	 void (*fill) (PLINT, PLFLT *, PLFLT *), PLBOOL rectangular,
	 void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	 PLPointer pltr_data);

/* Set up lengths of major tick marks. */

void API
c_plsmaj(PLFLT def, PLFLT scale);

/* Set the memory area to be plotted (with the 'mem' driver) */

void API
c_plsmem(PLINT maxx, PLINT maxy, void *plotmem);

/* Set up lengths of minor tick marks. */

void API
c_plsmin(PLFLT def, PLFLT scale);

/* Set orientation.  Must be done before calling plinit. */

void API
c_plsori(PLINT ori);

/* Set output device parameters.  Usually ignored by the driver. */

void API
c_plspage(PLFLT xp, PLFLT yp, PLINT xleng, PLINT yleng,
	  PLINT xoff, PLINT yoff);

/* Set the pause (on end-of-page) status */

void API
c_plspause(PLBOOL pause);

/* Set stream number.  */

void API
c_plsstrm(PLINT strm);

/* Set the number of subwindows in x and y */

void API
c_plssub(PLINT nx, PLINT ny);

/* Set symbol height. */

void API
c_plssym(PLFLT def, PLFLT scale);

/* Initialize PLplot, passing in the windows/page settings. */

void API
c_plstar(PLINT nx, PLINT ny);

/* Initialize PLplot, passing the device name and windows/page settings. */

void API
c_plstart(const char *devname, PLINT nx, PLINT ny);

/* Add a point to a stripchart.  */

void API
c_plstripa(PLINT id, PLINT pen, PLFLT x, PLFLT y);

/* Create 1d stripchart */

void API
c_plstripc(PLINT *id, char *xspec, char *yspec,
	PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
	PLFLT xlpos, PLFLT ylpos,
	PLBOOL y_ascl, PLBOOL acc,
	PLINT colbox, PLINT collab,
	PLINT colline[], PLINT styline[], char *legline[],
	char *labx, char *laby, char *labtop);

/* Deletes and releases memory used by a stripchart.  */

void API
c_plstripd(PLINT id);

  /* plots a 2d image (or a matrix too large for plshade() ) */

void API
plimage( PLFLT **data, PLINT nx, PLINT ny,
	 PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
	 PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax);

/* Set up a new line style */

void API
c_plstyl(PLINT nms, PLINT *mark, PLINT *space);

/* Plots the 3d surface representation of the function z[x][y]. */

void API
c_plsurf3d(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny,
	   PLINT opt, PLFLT *clevel, PLINT nlevel);

/* Plots the 3d surface representation of the function z[x][y] with y
 * index limits. */

void
c_plsurf3dl(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny,
           PLINT opt, PLFLT *clevel, PLINT nlevel,
           PLINT ixstart, PLINT ixn, PLINT *indexymin, PLINT*indexymax);

/* Sets the edges of the viewport to the specified absolute coordinates */

void API
c_plsvpa(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax);

/* Set x axis labeling parameters */

void API
c_plsxax(PLINT digmax, PLINT digits);

/* Set inferior X window */

void API
plsxwin(PLINT window_id);

/* Set y axis labeling parameters */

void API
c_plsyax(PLINT digmax, PLINT digits);

/* Plots array y against x for n points using Hershey symbol "code" */

void API
c_plsym(PLINT n, PLFLT *x, PLFLT *y, PLINT code);

/* Set z axis labeling parameters */

void API
c_plszax(PLINT digmax, PLINT digits);

/* Switches to text screen. */

void API
c_pltext(void);

/* Sets the edges of the viewport with the given aspect ratio, leaving */
/* room for labels. */

void API
c_plvasp(PLFLT aspect);

/* Creates the largest viewport of the specified aspect ratio that fits */
/* within the specified normalized subpage coordinates. */

void API
c_plvpas(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect);

/* Creates a viewport with the specified normalized subpage coordinates. */

void API
c_plvpor(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax);

/* Defines a "standard" viewport with seven character heights for */
/* the left margin and four character heights everywhere else. */

void API
c_plvsta(void);

/* Set up a window for three-dimensional plotting. */

void API
c_plw3d(PLFLT basex, PLFLT basey, PLFLT height, PLFLT xmin0,
	PLFLT xmax0, PLFLT ymin0, PLFLT ymax0, PLFLT zmin0,
	PLFLT zmax0, PLFLT alt, PLFLT az);

/* Set pen width. */

void API
c_plwid(PLINT width);

/* Set up world coordinates of the viewport boundaries (2d plots). */

void API
c_plwind(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax);

/*  set xor mode; mode = 1-enter, 0-leave, status = 0 if not interactive device  */

void API
c_plxormod(PLBOOL mode, PLBOOL *status);

/*--------------------------------------------------------------------------*\
 *              Functions for use from C or C++ only
\*--------------------------------------------------------------------------*/

/* Returns a list of file-oriented device names and their menu strings */

void API
plgFileDevs(char ***p_menustr, char ***p_devname, int *p_ndev);

/* Returns a list of all device names and their menu strings */

void API
plgDevs(char ***p_menustr, char ***p_devname, int *p_ndev);

/* Set the function pointer for the keyboard event handler */

void API
plsKeyEH(void (*KeyEH) (PLGraphicsIn *, void *, int *), void *KeyEH_data);

/* Set the function pointer for the (mouse) button event handler */

void API
plsButtonEH(void (*ButtonEH) (PLGraphicsIn *, void *, int *),
	    void *ButtonEH_data);

/* Sets an optional user bop handler */

void API
plsbopH(void (*handler) (void *, int *), void *handler_data);

/* Sets an optional user eop handler */

void API
plseopH(void (*handler) (void *, int *), void *handler_data);

/* Set the variables to be used for storing error info */

void API
plsError(PLINT *errcode, char *errmsg);

/* Sets an optional user exit handler. */

void API
plsexit(int (*handler) (char *));

/* Sets an optional user abort handler. */

void API
plsabort(void (*handler) (char *));

	/* Transformation routines */

/* Identity transformation. */

void API
pltr0(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data);

/* Does linear interpolation from singly dimensioned coord arrays. */

void API
pltr1(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data);

/* Does linear interpolation from doubly dimensioned coord arrays */
/* (column dominant, as per normal C 2d arrays). */

/* void API */
void
pltr2(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data);

/* Just like pltr2() but uses pointer arithmetic to get coordinates from */
/* 2d grid tables.  */

void API
pltr2p(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data);

/* Identity transformation for plots from Fortran. */

void API
pltr0f(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data);

/* Does linear interpolation from doubly dimensioned coord arrays */
/* (row dominant, i.e. Fortran ordering). */

void API
pltr2f(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data);

/* Example linear transformation function for contour plotter. */

void API
xform(PLFLT x, PLFLT y, PLFLT * tx, PLFLT * ty);

	/* Function evaluators */

/* Does a lookup from a 2d function array.  Array is of type (PLFLT **), */
/* and is column dominant (normal C ordering). */

PLFLT API
plf2eval2(PLINT ix, PLINT iy, PLPointer plf2eval_data);

/* Does a lookup from a 2d function array.  Array is of type (PLFLT *), */
/* and is column dominant (normal C ordering). */

PLFLT API
plf2eval(PLINT ix, PLINT iy, PLPointer plf2eval_data);

/* Does a lookup from a 2d function array.  Array is of type (PLFLT *), */
/* and is row dominant (Fortran ordering). */

PLFLT API
plf2evalr(PLINT ix, PLINT iy, PLPointer plf2eval_data);

	/* Command line parsing utilities */

/* Clear internal option table info structure. */

void API
plClearOpts(void);

/* Reset internal option table info structure. */

void API
plResetOpts(void);

/* Merge user option table into internal info structure. */

int API
plMergeOpts(PLOptionTable *options, char *name, char **notes);

/* Set the strings used in usage and syntax messages. */

void API
plSetUsage(char *program_string, char *usage_string);

/* Process input strings, treating them as an option and argument pair. */
/* The first is for the external API, the second the work routine declared
   here for backward compatibilty. */

int API
c_plsetopt(char *opt, char *optarg);

int API
plSetOpt(char *opt, char *optarg);

/* Process options list using current options info. */

int API
c_plparseopts(int *p_argc, char **argv, PLINT mode);

/* Print usage & syntax message. */

void API
plOptUsage(void);

	/* Miscellaneous */

/* Set the output file pointer */

void API
plgfile(FILE **p_file);

/* Get the output file pointer */

void API
plsfile(FILE *file);

/* Get the escape character for text strings. */

void API
plgesc(char *p_esc);

/* Front-end to driver escape function. */

void API
pl_cmd(PLINT op, void *ptr);

/* Return full pathname for given file if executable */

int API
plFindName(char *p);

/* Looks for the specified executable file according to usual search path. */

char * API
plFindCommand(char *fn);

/* Gets search name for file by concatenating the dir, subdir, and file */
/* name, allocating memory as needed.  */

void API
plGetName(char *dir, char *subdir, char *filename, char **filespec);

/* Prompts human to input an integer in response to given message. */

PLINT API
plGetInt(char *s);

/* Prompts human to input a float in response to given message. */

PLFLT API
plGetFlt(char *s);

	/* Nice way to allocate space for a vectored 2d grid */

/* Allocates a block of memory for use as a 2-d grid of PLFLT's.  */

void API
plAlloc2dGrid(PLFLT ***f, PLINT nx, PLINT ny);

/* Frees a block of memory allocated with plAlloc2dGrid(). */

void API
plFree2dGrid(PLFLT **f, PLINT nx, PLINT ny);

/* Find the maximum and minimum of a 2d matrix allocated with plAllc2dGrid(). */

void API
plMinMax2dGrid(PLFLT **f, PLINT nx, PLINT ny, PLFLT *fmax, PLFLT *fmin);

/* Wait for graphics input event and translate to world coordinates */

int API
plGetCursor(PLGraphicsIn *gin);

/* Translates relative device coordinates to world coordinates.  */

int API
plTranslateCursor(PLGraphicsIn *gin);

/* Deprecated function names which are handled as wrappers for strict
 * backwards compatibility of the library API*/

int API
plParseOpts(int *p_argc, char **argv, PLINT mode);

void API
plHLS_RGB(PLFLT h, PLFLT l, PLFLT s, PLFLT *p_r, PLFLT *p_g, PLFLT *p_b);

void API
plRGB_HLS(PLFLT r, PLFLT g, PLFLT b, PLFLT *p_h, PLFLT *p_l, PLFLT *p_s);


#ifdef __cplusplus
}
#endif

#ifdef __PLSTUBS_H__
#undef    c_pl_setcontlabelformat
#undef    c_pl_setcontlabelparam
#undef    c_pladv
#undef    c_plarrows
#undef    c_plaxes
#undef    c_plbin
#undef    c_plbop
#undef    c_plbox
#undef    c_plbox3
#undef    c_plcalc_world
#undef    c_plclear
#undef    c_plcol0
#undef    c_plcol1
#undef    c_plcont
#undef    c_plcpstrm
#undef    c_plend
#undef    c_plend1
#undef    c_plenv
#undef    c_plenv0
#undef    c_pleop
#undef    c_plerrx
#undef    c_plerry
#undef    c_plfamadv
#undef    c_plfill
#undef    c_plfill3
#undef    c_plflush
#undef    c_plfont
#undef    c_plfontld
#undef    c_plgchr
#undef    c_plgcol0
#undef    c_plgcolbg
#undef    c_plgcompression
#undef    c_plgdev
#undef    c_plgdidev
#undef    c_plgdiori
#undef    c_plgdiplt
#undef    c_plgfam
#undef    c_plgfci
#undef    c_plgfnam
#undef    c_plglevel
#undef    c_plgpage
#undef    c_plgra
#undef    c_plgriddata
#undef    c_plgspa
#undef    c_plgstrm
#undef    c_plgver
#undef    c_plgvpd
#undef    c_plgvpw
#undef    c_plgxax
#undef    c_plgyax
#undef    c_plgzax
#undef    c_plhist
#undef    c_plhls
#undef    c_plhlsrgb
#undef    c_plinit
#undef    c_pljoin
#undef    c_pllab
#undef    c_pllightsource
#undef    c_plline
#undef    c_plline3
#undef    c_pllsty
#undef    c_plmesh
#undef    c_plmeshc
#undef    c_plmkstrm
#undef    c_plmtex
#undef    c_plot3d
#undef    c_plot3dc
#undef    c_plot3dcl
#undef    c_plpat
#undef    c_plpoin
#undef    c_plpoin3
#undef    c_plpoly3
#undef    c_plprec
#undef    c_plpsty
#undef    c_plptex
#undef    c_plreplot
#undef    c_plrgb
#undef    c_plrgb1
#undef    c_plrgbhls
#undef    c_plschr
#undef    c_plscmap0
#undef    c_plscmap0n
#undef    c_plscmap1
#undef    c_plscmap1l
#undef    c_plscmap1n
#undef    c_plscol0
#undef    c_plscolbg
#undef    c_plscolor
#undef    c_plscompression
#undef    c_plsdev
#undef    c_plsdidev
#undef    c_plsdimap
#undef    c_plsdiori
#undef    c_plsdiplt
#undef    c_plsdiplz
#undef    c_plsesc
#undef    c_plsetopt
#undef    c_plsfam
#undef    c_plsfci
#undef    c_plsfnam
#undef    c_plxsfnam
#undef    c_plshade
#undef    c_plshade1
#undef    c_plshades
#undef    c_plsmaj
#undef    c_plsmem
#undef    c_plsmin
#undef    c_plsori
#undef    c_plspage
#undef    c_plspause
#undef    c_plsstrm
#undef    c_plssub
#undef    c_plssym
#undef    c_plstar
#undef    c_plstart
#undef    c_plstripa
#undef    c_plstripc
#undef    c_plstripd
#undef    c_plstyl
#undef    c_plsurf3d
#undef    c_plsurf3dl
#undef    c_plsvect
#undef    c_plsvpa
#undef    c_plsxax
#undef    c_plsyax
#undef    c_plsym
#undef    c_plszax
#undef    c_pltext
#undef    c_plvasp
#undef    c_plvect
#undef    c_plvpas
#undef    c_plvpor
#undef    c_plvsta
#undef    c_plw3d
#undef    c_plwid
#undef    c_plwind
#undef    c_plxormod
#endif

#endif  /* __PLPLOT_H__ */
