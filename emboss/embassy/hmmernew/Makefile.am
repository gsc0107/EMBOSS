#
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src emboss_acd emboss_doc

EXTRA_DIST = depcomp ltmain.sh install-sh config.sub config.guess

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS)
	tar cBf - emboss_doc | ( cd $(distdir); tar xBf - ; cd emboss_doc; rm -rf CVS; rm -rf master) 
