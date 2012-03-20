@echo off

setlocal

echo> ~tmp~.mak ####
echo> ~ver~.mak ####

:loop
if "%1" == "" goto :end
if "%1" == "--srcdir" goto :srcdir
if "%1" == "srcdir" goto :srcdir
if "%1" == "--target" goto :target
if "%1" == "target" goto :target
if "%1" == "--with-static-linked-ext" goto :extstatic
if "%1" == "--extout" goto :extout
if "%1" == "--with-baseruby" goto :baseruby
if "%1" == "-h" goto :help
if "%1" == "--help" goto :help
  shift
goto :loop
:srcdir
  echo>> ~tmp~.mak srcdir=%2
  echo> ~ver~.mak srcdir=%2
  set srcdir=%2
  shift
  shift
goto :loop
:target
  echo>> ~tmp~.mak arch=%2
  set arch=%2
  shift
  shift
goto :loop
:extstatic
  echo>> ~tmp~.mak 	EXTSTATIC=static
  shift
goto :loop
:extout
  echo>> ~tmp~.mak 	EXTOUT=%2
  set EXTOUT=%2
  shift
  shift
goto :loop
:baseruby
  echo>> ~tmp~.mak 	BASERUBY=%2
  set BASERUBY=%2
  shift
  shift
goto :loop
:help
  echo Configuration:
  echo   --help                    display this help
  echo   --srcdir=DIR              find the sources in DIR [configure dir or ..]
  echo System types:
  echo   --target=TARGET           configure for TARGET [arm-symbianelf]
  echo Optional Package:
  echo   --with-baseruby=RUBY      use RUBY as baseruby [ruby]
  echo   --with-static-linked-ext  link external modules statically
  del ~tmp~.mak > nul
goto :exit
:end

echo>> ~ver~.mak CC = arm-none-symbianelf-gcc
echo>> ~ver~.mak CPP = $(CC) -E
if "%srcdir%" == "" echo>> ~ver~.mak srcdir=..
echo>> ~ver~.mak all:
echo>> ~ver~.mak ^	@echo^>  ~tmp~.c #define RUBY_REVISION 0
echo>> ~ver~.mak ^	@echo^>^> ~tmp~.c #define RUBY_LIB_VERSION_STYLE 3
echo>> ~ver~.mak ^	@echo^>^> ~tmp~.c #include "version.h"
echo>> ~ver~.mak ^	@echo^>^> ~tmp~.c MAJOR = RUBY_API_VERSION_MAJOR
echo>> ~ver~.mak ^	@echo^>^> ~tmp~.c MINOR = RUBY_API_VERSION_MINOR
echo>> ~ver~.mak ^	@echo^>^> ~tmp~.c TEENY = RUBY_API_VERSION_TEENY
echo>> ~ver~.mak ^	@$(CPP) -I$(srcdir) -I$(srcdir)\include ~tmp~.c ^| find "=" ^>^>~tmp~.mak
echo>> ~ver~.mak ^	@del /Q ~tmp~.c

make -f ~ver~.mak
del /Q ~ver~.mak

:: Defaults
if "%srcdir%" == "" echo>> ~tmp~.mak srcdir=..
if "%arch%" == "" echo>> ~tmp~.mak arch=arm-symbianelf
if "%EXTOUT%" == "" echo>> ~tmp~.mak EXTOUT=$(srcdir)/.ext
if "%BASERUBY%" == "" echo>> ~tmp~.mak BASERUBY=ruby
::

echo>> ~tmp~.mak arch_hdrdir = $(EXTOUT)/include/$(arch)
echo>> ~tmp~.mak hdrdir = $(srcdir)/include

echo>> ~tmp~.mak ifndef EXTSTATIC
echo>> ~tmp~.mak EXT_LIST=stringio bigdecimal zlib
echo>> ~tmp~.mak endif

echo>> ~tmp~.mak all:
echo>> ~tmp~.mak ^	@if not exist $(subst /,\,$(arch_hdrdir))\ruby\nul md $(subst /,\,$(arch_hdrdir)\ruby)
echo>> ~tmp~.mak ^	$(call config_h,$(subst /,\,$(arch_hdrdir))\ruby\config.h)
echo>> ~tmp~.mak ^	@if not exist group\nul md group
echo>> ~tmp~.mak ^	$(call pre_build_mk,pre-build.mk)
echo>> ~tmp~.mak ^	$(call bld_inf,group\bld.inf)
echo>> ~tmp~.mak ^	$(call ruby_mmp,group\ruby.mmp,64000,2000000,16000000)
echo>> ~tmp~.mak ifndef EXTSTATIC
echo>> ~tmp~.mak ^	$(call ext_mmp,group\,stringio,$(STRINGIO_UID))
echo>> ~tmp~.mak ^	$(call ext_mmp,group\,bigdecimal,$(BIGDECIMAL_UID),,libm.lib)
echo>> ~tmp~.mak ^	$(call ext_mmp,group\,zlib,$(ZLIB_UID),,libz.lib)
echo>> ~tmp~.mak endif
echo>> ~tmp~.mak ^	@if not exist sis\nul md sis
echo>> ~tmp~.mak ^	$(call ruby_pkg,sis\ruby.pkg)
echo>> ~tmp~.mak ifndef EXTSTATIC
echo>> ~tmp~.mak ^	$(call core_ext_pkg,sis\ruby_core_ext.pkg)
echo>> ~tmp~.mak ^	$(call ext_bigdecimal,sis\ruby_core_ext.pkg)
echo>> ~tmp~.mak ^	$(call ext_pkg,sis\ruby_core_ext.pkg,stringio)
echo>> ~tmp~.mak ^	$(call ext_pkg,sis\ruby_core_ext.pkg,zlib)
echo>> ~tmp~.mak ^	@if not exist eabi\nul md eabi
echo>> ~tmp~.mak ^	$(call ext_def,eabi\,stringio)
echo>> ~tmp~.mak ^	$(call ext_def,eabi\,bigdecimal)
echo>> ~tmp~.mak ^	$(call ext_def,eabi\,zlib)
echo>> ~tmp~.mak endif

echo>> ~tmp~.mak include setup

make -f ~tmp~.mak
del /Q ~tmp~.mak

:exit
