## Process this file with automake to produce Makefile.in

SUBDIRS = @EXT_SUBDIRS@

noinst_LIBRARIES=libphpext.a

libphpext.a: @EXT_LIBS@
	top_srcdir=$(top_srcdir) $(top_builddir)/scripts/mkextlib $@ $(SUBDIRS)
	-@test "`uname -s`" = "Rhapsody" && $(RANLIB) $@
