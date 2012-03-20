# -*- makefile -*-

!if "$(srcdir)" != ""
bcc32dir = $(srcdir)/bcc32
!elseif "$(bcc32dir)" == "bcc32/"
srcdir = .
!elseif "$(bcc32dir:/bcc32/=)/bcc32/" == "$(bcc32dir)"
srcdir = $(bcc32dir:/bcc32/=)
!else
srcdir = $(bcc32dir)/..
!endif
!ifndef prefix
prefix = /usr
!endif
OS = bccwin32
RT = $(OS)
BANG = !
APPEND = echo.>>$(MAKEFILE)
!ifdef MAKEFILE
MAKE = $(MAKE) -f $(MAKEFILE)
!else
MAKEFILE = Makefile
!endif

all: Makefile
Makefile: -prologue- -generic- -epilogue-
i386-$(OS): -prologue- -i386- -epilogue-
i486-$(OS): -prologue- -i486- -epilogue-
i586-$(OS): -prologue- -i586- -epilogue-
i686-$(OS): -prologue- -i686- -epilogue-
alpha-$(OS): -prologue- -alpha- -epilogue-

-prologue-: -basic-vars- -version- -system-vars-

-basic-vars-: nul
	@echo Creating $(MAKEFILE)
	@type > $(MAKEFILE) &&|
\#\#\# Makefile for ruby $(OS) \#\#\#
$(BANG)ifndef srcdir
srcdir = $(srcdir:\=/)
$(BANG)endif
$(BANG)ifndef prefix
prefix = $(prefix:\=/)
$(BANG)endif
$(BANG)if !defined(BASERUBY)
!if defined(BASERUBY)
BASERUBY = $(BASERUBY)
!endif
|
!if !defined(BASERUBY)
	@for %I in (ruby.exe) do @echo BASERUBY = "%~$$PATH:I" >> $(MAKEFILE)
!endif
	@type >> $(MAKEFILE) &&|
$(BANG)endif
|
!if exist(confargs.mk)
	@type confargs.mk >> $(MAKEFILE)
	@del confargs.mk
!endif

-system-vars-: -runtime- -bormm-

-bormm-: nul
	@-ilink32 -q -Gn -x usebormm.lib > nul
	@-if exist usebormm.tds $(APPEND) MEMLIB = usebormm.lib
	@if exist usebormm.* del usebormm.*

-osname-: nul
	@echo OS =  >>$(MAKEFILE)

-runtime-: nul
	type > conftest.c &&|
\#include <stdio.h>
int main(){printf("");return 0;}
|
	bcc32 conftest.c cw32i.lib > nul
	tdump conftest.exe < nul > conftest.i
	grep "^Imports from CC" conftest.i > conftest.c
	cpp32 -P- -DFile=\# -DImports=RTNAME -Dfrom== conftest.c > nul
	$(MAKE) > nul -DBANG=$(BANG) -f &&|
-runtime-: nul
$(BANG)include conftest.i
RT = $$(RTNAME:.DLL=)
OS = $$(RT:CC32=)
-runtime-:
	del conftest.*
$(BANG)if "$$(OS)" == "50"
	echo OS = bccwin32 >> $(MAKEFILE)
$(BANG)else
	echo OS = bccwin32_$$(OS) >> $(MAKEFILE)
$(BANG)endif
|
	@echo RT = $$(OS) >> $(MAKEFILE)

-version-: nul
	@cpp32 -I$(srcdir) -P- -o$(MAKEFILE) > nul &&|
\#define RUBY_REVISION 0
\#include "version.h"
MAJOR = RUBY_API_VERSION_MAJOR
MINOR = RUBY_API_VERSION_MINOR
TEENY = RUBY_API_VERSION_TEENY

BORLANDC = __BORLANDC__
|
	@$(MAKE) > nul -DBANG=$(BANG) -f &&,
-version-: nul
$(BANG)include $(MAKEFILE)
$(BANG)include $(MAKEFILE).i
-version-:
	@del $(MAKEFILE).i
	@type >> $(MAKEFILE) &&|
MAJOR = $$(MAJOR)
MINOR = $$(MINOR)
TEENY = $$(TEENY)
BORLANDC = $$(BORLANDC)
|
,

-generic-: nul
!if defined(PROCESSOR_ARCHITECTURE) ||  defined(PROCESSOR_LEVEL)
	@type >> $(MAKEFILE) &&|
!if defined(PROCESSOR_ARCHITECTURE)
$(BANG)ifndef PROCESSOR_ARCHITECTURE
PROCESSOR_ARCHITECTURE = $(PROCESSOR_ARCHITECTURE)
$(BANG)endif
!endif
!if defined(PROCESSOR_LEVEL)
$(BANG)ifndef PROCESSOR_LEVEL
PROCESSOR_LEVEL = $(PROCESSOR_LEVEL)
$(BANG)endif
!endif
|
!endif

-alpha-: nul
	@$(APPEND) !ifndef PROCESSOR_ARCHITECTURE
	@$(APPEND) PROCESSOR_ARCHITECTURE = alpha
	@$(APPEND) !endif
-ix86-: nul
	@$(APPEND) !ifndef PROCESSOR_ARCHITECTURE
	@$(APPEND) PROCESSOR_ARCHITECTURE = x86
	@$(APPEND) !endif

-i386-: -ix86-
	@$(APPEND) !ifndef PROCESSOR_LEVEL
	@$(APPEND) PROCESSOR_LEVEL = 3
	@$(APPEND) !endif
-i486-: -ix86-
	@$(APPEND) !ifndef PROCESSOR_LEVEL
	@$(APPEND) PROCESSOR_LEVEL = 4
	@$(APPEND) !endif
-i586-: -ix86-
	@$(APPEND) !ifndef PROCESSOR_LEVEL
	@$(APPEND) PROCESSOR_LEVEL = 5
	@$(APPEND) !endif
-i686-: -ix86-
	@$(APPEND) !ifndef PROCESSOR_LEVEL
	@$(APPEND) PROCESSOR_LEVEL = 6
	@$(APPEND) !endif

-epilogue-: -encs-

-encs-: nul
	@$(MAKE) -f $(srcdir)/win32/enc-setup.mak srcdir="$(srcdir)" MAKEFILE=$(MAKEFILE)

-epilogue-: nul
	@type >> $(MAKEFILE) &&|

\# RUBY_INSTALL_NAME = ruby
\# RUBY_SO_NAME = $$(RT)-$$(RUBY_INSTALL_NAME)$$(MAJOR)$$(MINOR)
\# CFLAGS = -q $$(DEBUGFLAGS) $$(OPTFLAGS) $$(PROCESSOR_FLAG) -w- -wsus -wcpt -wdup -wext -wrng -wrpt -wzdi
\# CPPFLAGS = -I. -I$$(srcdir) -I$$(srcdir)/missing -DLIBRUBY_SO=\"$$(LIBRUBY_SO)\"
\# STACK = 0x2000000
\# LDFLAGS = -S:$$(STACK)
\# RFLAGS = $$(iconinc)
\# EXTLIBS = cw32.lib import32.lib user32.lib kernel32.lib
$(BANG)include $$(srcdir)/bcc32/Makefile.sub
|
	@echo type "`$(MAKE)'" to make ruby for $(OS).
