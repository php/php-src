## Process this file with automake to produce Makefile.in

SUBDIRS= src doc

MAINTAINERCLEANFILES =  aclocal.m4 config.h.in configure Makefile.in \
			stamp-h.in *~

dist-hook:
	cp $(srcdir)/doc/bcmath.1 $(distdir)/doc
	cp $(srcdir)/src/private.h $(distdir)/src
	cp $(srcdir)/FAQ $(distdir)

