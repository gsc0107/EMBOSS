## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = ehmmcalibrate ehmmconvert ehmmemit ehmmfetch ehmmindex \
	ehmmbuild ehmmalign ehmmpfam ehmmsearch 
               
if LOCALLINK
INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus
else
INCLUDES = -I${embprefix}/include
endif

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif

ehmmcalibrate_SOURCES = ehmmcalibrate.c
ehmmconvert_SOURCES   = ehmmconvert.c
ehmmemit_SOURCES      = ehmmemit.c
ehmmfetch_SOURCES     = ehmmfetch.c
ehmmindex_SOURCES     = ehmmindex.c
ehmmbuild_SOURCES     = ehmmbuild.c
ehmmalign_SOURCES     = ehmmalign.c
ehmmpfam_SOURCES      = ehmmpfam.c
ehmmsearch_SOURCES    = ehmmsearch.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot $(XLIB)
endif