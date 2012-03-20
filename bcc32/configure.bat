@echo off
::: Don't set environment variable in batch file other than autoexec.bat
::: to avoid "Out of environment space" problem on Windows 95/98.
::: set TMPMAKE=~tmp~.mak

echo> ~tmp~.mak ####
echo>> ~tmp~.mak conf = %0
echo>> ~tmp~.mak $(conf:\=/): nul
echo>> ~tmp~.mak 	@del ~setup~.mak
echo>> ~tmp~.mak 	@-$(MAKE) -l$(MAKEFLAGS) -f $(@D)setup.mak \
if exist pathlist.tmp del pathlist.tmp
if exist confargs.mk del confargs.mk
:loop
if "%1" == "" goto :end
if "%1" == "--prefix" goto :prefix
if "%1" == "prefix" goto :prefix
if "%1" == "--srcdir" goto :srcdir
if "%1" == "srcdir" goto :srcdir
if "%1" == "--target" goto :target
if "%1" == "target" goto :target
if "%1" == "--with-static-linked-ext" goto :extstatic
if "%1" == "--program-suffix" goto :suffix
if "%1" == "RUBY_SUFFIX" goto :suffix
if "%1" == "--program-name" goto :installname
if "%1" == "--install-name" goto :installname
if "%1" == "RUBY_INSTALL_NAME" goto :installname
if "%1" == "--so-name" goto :soname
if "%1" == "RUBY_SO_NAME" goto :soname
if "%1" == "--enable-install-doc" goto :enable-rdoc
if "%1" == "--disable-install-doc" goto :disable-rdoc
if "%1" == "--extout" goto :extout
if "%1" == "EXTOUT" goto :extout
if "%1" == "--with-baseruby" goto :baseruby
if "%1" == "BASERUBY" goto :baseruby
if "%1" == "--path" goto :path
if "%1" == "-h" goto :help
if "%1" == "--help" goto :help
  echo>>confargs.tmp 	%1 \
  shift
goto :loop
:srcdir
  echo>> ~tmp~.mak 	-Dsrcdir=%2 \
  echo>>confargs.tmp 	--srcdir=%2 \
  shift
  shift
goto :loop
:prefix
  echo>> ~tmp~.mak 	-Dprefix=%2 \
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:suffix
  echo>>confargs.mk !ifndef RUBY_SUFFIX
  echo>>confargs.mk RUBY_SUFFIX = %2
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:installname
  echo>>confargs.mk !ifndef RUBY_INSTALL_NAME
  echo>>confargs.mk RUBY_INSTALL_NAME = %2
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:soname
  echo>>confargs.mk !ifndef RUBY_SO_NAME
  echo>>confargs.mk RUBY_SO_NAME = %2
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:target
  echo>> ~tmp~.mak 	%2 \
  echo>>confargs.tmp 	--target=%2 \
  shift
  shift
goto :loop
:extstatic
  echo>>confargs.mk !ifndef EXTSTATIC
  echo>>confargs.mk EXTSTATIC = static
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1 \
  shift
goto :loop
:enable-rdoc
  echo>>confargs.mk !ifndef RDOCTARGET
  echo>>confargs.mk RDOCTARGET = install-doc
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1 \
  shift
goto :loop
:disable-rdoc
  echo>>confargs.mk !ifndef RDOCTARGET
  echo>>confargs.mk RDOCTARGET = install-nodoc
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1 \
  shift
goto :loop
:extout
  echo>>confargs.mk !ifndef EXTOUT
  echo>>confargs.mk EXTOUT = %2
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:baseruby
  echo>>confargs.mk !ifndef BASERUBY
  echo>>confargs.mk BASERUBY = %2
  echo>>confargs.mk !endif
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:path
  echo>>pathlist.tmp %2;\
  echo>>confargs.tmp 	%1=%2 \
  shift
  shift
goto :loop
:help
  echo Configuration:
  echo   --help                  display this help
  echo   --srcdir=DIR            find the sources in DIR [configure dir or `..']
  echo Installation directories:
  echo   --prefix=PREFIX         install files in PREFIX (ignored currently)
  echo System types:
  echo   --target=TARGET         configure for TARGET [i386-bccwin32]
  echo Optional Package:
  echo   --with-baseruby=RUBY    use RUBY as baseruby [ruby]
  echo   --with-static-linked-ext link external modules statically
  echo   --enable-install-doc    install rdoc indexes during install
  del *.tmp
  del ~tmp~.mak
goto :exit
:end
echo>> ~tmp~.mak 	-Dbcc32dir=$(@D)
if not exist confargs.tmp goto :noconfargs
    echo>>confargs.mk configure_args = \
    type>>confargs.mk confargs.tmp
    echo.>>confargs.mk
    echo>>confargs.mk ####
:noconfargs
if not exist pathlist.tmp goto :nopathlist
    echo>>confargs.mk pathlist = \
    type>>confargs.mk pathlist.tmp
    echo.>>confargs.mk
    echo>>confargs.mk ####
    echo>>confargs.mk PATH = $(pathlist:;=/bin;)$(PATH)
    echo>>confargs.mk INCLUDE = $(pathlist:;=/include;)
    echo>>confargs.mk LIB = $(pathlist:;=/lib;)
:nopathlist
if exist confargs.mk copy confargs.mk ~setup~.mak > nul
type>>~setup~.mak ~tmp~.mak
del *.tmp > nul
del ~tmp~.mak > nul
make -s -f ~setup~.mak
:exit
