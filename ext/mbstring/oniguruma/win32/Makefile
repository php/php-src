# Oniguruma Makefile for Win32

product_name = oniguruma

CPPFLAGS = 
CFLAGS = -O2 -nologo
LDFLAGS =
LOADLIBES =
ARLIB = lib
ARLIB_FLAGS = -nologo
ARDLL = cl
ARDLL_FLAGS = -nologo -LD $(LINKFLAGS) -dll
LINKFLAGS = -link -incremental:no -pdb:none

INSTALL = install -c
CP      = copy
CC = cl
DEFS = -DHAVE_CONFIG_H -DNOT_RUBY -DEXPORT
RUBYDIR = ..

subdirs = 

libbase   = onig
libname   = $(libbase)_s.lib
dllname   = $(libbase).dll
dlllib    = $(libbase).lib

onigheaders  = oniguruma.h regint.h regparse.h
posixheaders = onigposix.h
headers      = $(posixheaders) $(onigheaders)

onigobjs     = reggnu.obj regerror.obj regparse.obj regcomp.obj regexec.obj
posixobjs    = regposix.obj regposerr.obj
libobjs      = $(onigobjs) $(posixobjs)

onigsources  = regerror.c regparse.c regcomp.c regexec.c reggnu.c
posixsources = regposix.c regposerr.c
libsources   = $(posixsources) $(onigsources)
rubysources  = regex.c $(onigsources)

patchfiles   = re.c.168.patch re.c.180.patch
distfiles    = README COPYING INSTALL-RUBY HISTORY \
		Makefile.in configure.in config.h.in configure \
		$(headers) $(libsources) regex.c $(patchfiles) \
		test.rb testconv.rb
testc        = testc
testp        = testp

makeargs = $(MFLAGS) CPPFLAGS='$(CPPFLAGS)' CFLAGS='$(CFLAGS)' CC='$(CC)'

.SUFFIXES:
.SUFFIXES: .obj .c .h .ps .dvi .info .texinfo

.c.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) /I. /c $<

# targets
default: all

all: $(libname) $(dllname) 

$(libname): $(libobjs)
	$(ARLIB) $(ARLIB_FLAGS) -out:$@ $(libobjs)

$(dllname): $(libobjs)
	$(ARDLL) $(libobjs) -Fe$@ $(ARDLL_FLAGS)

regparse.obj: regparse.c $(onigheaders) config.h
regcomp.obj:  regcomp.c  $(onigheaders) config.h
regexec.obj:  regexec.c  regint.h oniguruma.h config.h
reggnu.obj:   reggnu.c   regint.h oniguruma.h config.h
regerror.obj: regerror.c regint.h oniguruma.h config.h
regposix.obj: regposix.c $(posixheaders) oniguruma.h config.h
regposerr.obj: regposerr.c $(posixheaders) config.h

# Ruby test
rtest:
	$(RUBYDIR)\win32\ruby -w -Ke test.rb

# C library test
ctest: $(testc)
	.\$(testc)

# POSIX C library test
ptest: $(testp)
	.\$(testp)

$(testc): $(testc).c $(libname)
	$(CC) -nologo -o $(testc) $(testc).c $(libname)

$(testp): $(testc).c $(dlllib)
	$(CC) -nologo -DPOSIX_TEST -DIMPORT -o $(testp) $(testc).c $(dlllib)

clean:
	del *.obj *.lib *.exp *.dll $(testp).exe $(testc).exe $(testc).obj


16: cpruby
	patch -d $(RUBYDIR) -p0 < re.c.168.patch

18: cpruby
	patch -d $(RUBYDIR) -p0 < re.c.180.patch

# backup file suffix
SORIG = ruby_orig

cpruby:
	$(CP) $(RUBYDIR)\regex.c $(RUBYDIR)\regex.c.$(SORIG)
	$(CP) $(RUBYDIR)\regex.h $(RUBYDIR)\regex.h.$(SORIG)
	$(CP) $(RUBYDIR)\re.c    $(RUBYDIR)\re.c.$(SORIG)
# $(rubysources)
	$(CP) regex.c      $(RUBYDIR)
	$(CP) regerror.c   $(RUBYDIR)
	$(CP) regparse.c   $(RUBYDIR)
	$(CP) regcomp.c    $(RUBYDIR)
	$(CP) regexec.c    $(RUBYDIR)
	$(CP) reggnu.c     $(RUBYDIR)
# $(onigheaders)
	$(CP) oniguruma.h  $(RUBYDIR)\regex.h
	$(CP) regint.h     $(RUBYDIR)
	$(CP) regparse.h   $(RUBYDIR)

rback:
	$(CP) $(RUBYDIR)\regex.c.$(SORIG) $(RUBYDIR)\regex.c
	$(CP) $(RUBYDIR)\regex.h.$(SORIG) $(RUBYDIR)\regex.h
	$(CP) $(RUBYDIR)\re.c.$(SORIG)    $(RUBYDIR)\re.c

samples:
	$(CC) $(CFLAGS) -I. -DIMPORT -o simple sample\simple.c  $(dlllib)
	$(CC) $(CFLAGS) -I. -DIMPORT -o posix  sample\posix.c   $(dlllib)
	$(CC) $(CFLAGS) -I. -DIMPORT -o names  sample\names.c   $(dlllib)
