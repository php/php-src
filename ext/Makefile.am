## Process this file with automake to produce Makefile.in

SUBDIRS = @EXT_SUBDIRS@
EXT_STATIC = @EXT_STATIC@

noinst_LIBRARIES=libphpext.a

libphpext.a: @EXT_LIBS@
	top_srcdir=$(top_srcdir) $(top_builddir)/scripts/mkextlib $@ $(EXT_STATIC)
	-@test "`uname -s`" = "Rhapsody" && $(RANLIB) $@
