
## Process this file with automake to produce Makefile.in

if ISCYGWIN
CYGWIN_LDFLAGS = -no-undefined -Wl,--add-stdcall-alias \
-Wl,--export-all-symbols -Wl,--enable-auto-import
if LOCALLINK
CYGWIN_LDHMMER = -L../../../plplot -L../../../ajax/pcre -L../../../ajax/expat \
-L../../../ajax/zlib -L../../../ajax/core -L../../../ajax/graphics \
-L../../../ajax/ensembl -L../../../ajax/ajaxdb -L../../../ajax/acd \
-L../../../nucleus \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leplplot -lezlib \
-leexpat -lepcre
else
CYGWIN_LDHMMER = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl \
                -lajaxg -lajax -leplplot -leexpat -lepcre -lezlib
endif
endif

if LOCALLINK
INCLUDES = -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/zlib \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl -I../../../ajax/ajaxdb \
	-I../../../ajax/acd -I../../../plplot
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs -L../../../ajax/expat/.libs \
-L../../../ajax/zlib/.libs -L../../../ajax/core/.libs \
-L../../../ajax/graphics/.libs -L../../../ajax/ensembl/.libs \
-L../../../ajax/ajaxdb/.libs -L../../../ajax/acd/.libs \
-L../../../nucleus/.libs \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre \
-lezlib -leplplot
else
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot
endif
endif
endif

AM_CFLAGS =  $(AIX_CFLAGS) $(WARN_CFLAGS) $(DEVWARN_CFLAGS)

bin_PROGRAMS = ohmmcalibrate ohmmconvert ohmmemit ohmmfetch ohmmindex \
	ohmmbuild ohmmalign ohmmpfam ohmmsearch oalistat

lib_LTLIBRARIES = libhmmer.la

HMMERSRC = alphabet.c core_algorithms.c debug.c emit.c emulation.c \
	   histogram.c hmmio.c mathsupport.c masks.c misc.c \
	   modelmakers.c plan7.c plan9.c prior.c threads.c tophits.c \
	   trace.c aligneval.c alignio.c cluster.c dayhoff.c file.c \
	   getopt.c gsi.c hsregex.c interleaved.c iupac.c msf.c \
	   revcomp.c selex.c sqerror.c sqio.c sre_ctype.c sre_math.c \
	   sre_string.c stack.c translate.c types.c weight.c

noinst_HEADERS = config.h funcs.h globals.h gnuregex.h sqfuncs.h \
	squid.h structs.h version.h

libhmmer_la_SOURCES = $(HMMERSRC)

VERS_INF = -version-info 2:1:1

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS) $(CYGWIN_LDHMMER)
else
LINKFLAGS = $(VERS_INF)
endif

libhmmer_la_LDFLAGS = $(LINKFLAGS)

ohmmcalibrate_SOURCES = ohmmcalibrate.c
ohmmconvert_SOURCES   = ohmmconvert.c
ohmmemit_SOURCES      = ohmmemit.c
ohmmfetch_SOURCES     = ohmmfetch.c
ohmmindex_SOURCES     = ohmmindex.c
ohmmbuild_SOURCES     = ohmmbuild.c
ohmmalign_SOURCES     = ohmmalign.c
ohmmpfam_SOURCES      = ohmmpfam.c
ohmmsearch_SOURCES    = ohmmsearch.c
oalistat_SOURCES      = oalistat.c


if LOCALLINK
LDADD = libhmmer.la \
	../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la \
	../../../ajax/zlib/libezlib.la \
	../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la \
	../../../plplot/libeplplot.la \
	$(XLIB)
else
LDADD = libhmmer.la -L${embprefix}/lib -lnucleus -lacd -lajaxdb \
	-lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot $(XLIB)
endif
