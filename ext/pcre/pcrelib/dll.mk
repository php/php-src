# dll.mk - auxilary Makefile to easy build dll's for mingw32 target
# ver. 0.6 of 1999-03-25
#
# Homepage of this makefile - http://www.is.lg.ua/~paul/devel/
# Homepage of original mingw32 project -
#		      http://www.fu.is.saga-u.ac.jp/~colin/gcc.html
#
# How to use:
# This makefile can:
# 1. Create automatical .def file from list of objects
# 2. Create .dll from objects and .def file, either automatical, or your
#    hand-written (maybe) file, which must have same basename as dll
# WARNING! There MUST be object, which name match dll's name. Make sux.
# 3. Create import library from .def (as for .dll, only its name required,
#    not dll itself)
#    By convention implibs for dll have .dll.a suffix, e.g. libstuff.dll.a
#    Why not just libstuff.a? 'Cos that's name for static lib, ok?
# Process divided into 3 phases because:
# 1. Pre-existent .def possible
# 2. Generating implib is enough time-consuming
#
# Variables:
#   DLL_LDLIBS  - libs for linking dll
#   DLL_LDFLAGS - flags for linking dll
#
# By using $(DLL_SUFFIX) instead of 'dll', e.g. stuff.$(DLL_SUFFIX)
# you may help porting makefiles to other platforms
#
# Put this file in your make's include path (e.g. main include dir, for
# more information see include section in make doc). Put in the beginning
# of your own Makefile line "include dll.mk". Specify dependences, e.g.:
#
# Do all stuff in one step
# libstuff.dll.a: $(OBJECTS) stuff.def
# stuff.def: $(OBJECTS)
#
# Steps separated, pre-provided .def, link with user32
#
# DLL_LDLIBS=-luser32
# stuff.dll: $(OBJECTS)
# libstuff.dll.a: $(OBJECTS)


DLLWRAP=dllwrap
DLLTOOL=dlltool

DLL_SUFFIX=dll

.SUFFIXES: .o .$(DLL_SUFFIX)

_%.def: %.o
      $(DLLTOOL) --export-all --output-def $@ $^

%.$(DLL_SUFFIX): %.o
      $(DLLWRAP) --dllname $(notdir $@) --driver-name $(CC) --def $*.def -o $@ $(filter %.o,$^) $(DLL_LDFLAGS) $(DLL_LDLIBS)

lib%.$(DLL_SUFFIX).a:%.def
      $(DLLTOOL) --dllname $(notdir $*.dll) --def $< --output-lib $@

# End
