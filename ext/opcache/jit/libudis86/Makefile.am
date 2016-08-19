#
# -- udis86/libudis86
#

PYTHON  = @PYTHON@
OPTABLE = @top_srcdir@/docs/x86/optable.xml

MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libudis86.la

libudis86_la_SOURCES = \
	itab.c \
	decode.c \
	syn.c \
	syn-intel.c \
	syn-att.c \
	udis86.c \
	udint.h \
	syn.h \
	decode.h

include_ladir = ${includedir}/libudis86
include_la_HEADERS = \
	types.h \
	extern.h \
	itab.h


BUILT_SOURCES = \
	itab.c \
	itab.h

#
# DLLs may not contain undefined symbol references.  
# We have the linker check this explicitly.
#
if TARGET_WINDOWS
libudis86_la_LDFLAGS = -no-undefined -version-info 0:0:0
endif

itab.c itab.h: $(OPTABLE) \
               $(top_srcdir)/scripts/ud_itab.py \
               $(top_srcdir)/scripts/ud_opcode.py
	$(PYTHON) $(top_srcdir)/scripts/ud_itab.py $(OPTABLE) $(srcdir)


clean-local:
	rm -rf $(BUILT_SOURCES)

maintainer-clean-local:
