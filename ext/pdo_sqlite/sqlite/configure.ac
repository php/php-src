#
# This file describes a "configure" script that is used to build
# makefiles for a particular platform.  Process this file using 
# Autoconf version 1.13 in order to generate that script.  All 
# lines of this file up to the AC_INIT macro are ignored.
#
# The build process allows for using a cross-compiler.  But the default
# action is to target the same platform that we are running on.  The
# configure script needs to discover the following properties of the 
# build and target systems:
#
#    srcdir
#
#        The is the name of the directory that contains the
#        "configure" shell script.  All source files are
#        located relative to this directory.
#
#    bindir
#
#        The name of the directory where executables should be
#        written by the "install" target of the makefile.
#
#    program_prefix
#
#        Add this prefix to the names of all executables that run
#        on the target machine.  Default: ""
#
#    ENABLE_SHARED
#
#        True if shared libraries should be generated.
#
#    BUILD_CC
#
#        The name of a command that is used to convert C
#        source files into executables that run on the build
#        platform.
#
#    BUILD_CFLAGS
#
#        Switches that the build compiler needs in order to construct
#        command-line programs.
#
#    BUILD_LIBS
#
#        Libraries that the build compiler needs in order to construct
#        command-line programs.
#
#    BUILD_EXEEXT
#
#        The filename extension for executables on the build
#        platform.  "" for Unix and ".exe" for Windows.
#
#    TARGET_CC
#
#        The name of a command that runs on the build platform
#        and converts C source files into *.o files for the
#        target platform.  In other words, the cross-compiler.
#
#    TARGET_CFLAGS
#
#        Switches that the target compiler needs to turn C source files
#        into *.o files.  Do not include TARGET_TCL_INC in this list.
#        Makefiles might add additional switches such as "-I.".
#
#    TARGET_TCL_LIBS
#
#        This is the library directives passed to the target linker
#        that cause the executable to link against Tcl.  This might
#        be a switch like "-ltcl8.0" or pathnames of library file
#        like "../../src/libtcl8.0.a".
#
#    TARGET_TCL_INC
#
#        This variables define the directory that contain header
#        files for Tcl.  If the compiler is able to find <tcl.h>
#        on its own, then this can be blank.
#
#    TARGET_READLINE_LIBS
#
#        This is the library directives passed to the target linker
#        that cause the executable to link against the readline library.
#        This might be a switch like "-lreadline" or pathnames of library
#        file like "../../src/libreadline.a".
#
#    TARGET_READLINE_INC
#
#        This variables define the directory that contain header
#        files for the readline library.  If the compiler is able 
#        to find <readline.h> on its own, then this can be blank.
#
#    TARGET_LINK
#
#        The name of the linker that combines *.o files generated
#        by TARGET_CC into executables for the target platform.
#
#    TARGET_LIBS
#
#        Additional libraries or other switch that the target linker needs
#        to build an executable on the target.  Do not include
#        on this list any libraries in TARGET_TCL_LIBS and
#        TARGET_READLINE_LIBS, etc.
#
#    TARGET_EXEEXT
#
#        The filename extension for executables on the
#        target platform.  "" for Unix and ".exe" for windows.
#
# The generated configure script will make an attempt to guess
# at all of the above parameters.  You can override any of
# the guesses by setting the environment variable named
# "config_AAAA" where "AAAA" is the name of the parameter
# described above.  (Exception: srcdir cannot be set this way.)
# If you have a file that sets one or more of these environment
# variables, you can invoke configure as follows:
#
#           configure --with-hints=FILE
#
# where FILE is the name of the file that sets the environment
# variables.  FILE should be an absolute pathname.
#
# If you have a Tcl/Tk/BLT source distribution available, then the
# files in that distribution will be used instead of any other
# Tcl/Tk/BLT files the script might discover if you tell the configure
# script about the source tree.  Use commandline options:
#
#         --with-tcl=PATH  --with-tk=PATH  --with-blt=PATH
#
# Or set environment variables config_WITH_TCL, config_WITH_TK, or
# config_WITH_BLT.
#
# This configure.in file is easy to reuse on other projects.  Just
# change the argument to AC_INIT().  And disable any features that
# you don't need (for example BLT) by erasing or commenting out
# the corresponding code.
#
AC_INIT(src/sqlite.h.in)

dnl Put the RCS revision string after AC_INIT so that it will also
dnl show in in configure.
# The following RCS revision string applies to configure.in
# $Revision$

#########
# Programs needed
#
AC_PROG_LIBTOOL
AC_PROG_INSTALL

#########
# Set up an appropriate program prefix
#
if test "$program_prefix" = "NONE"; then
  program_prefix=""
fi
AC_SUBST(program_prefix)

if test -f VERSION; then
  VERSION=`cat VERSION`
  echo "Version set to $VERSION"
fi
AC_SUBST(VERSION)

#########
# Check to see if the --with-hints=FILE option is used.  If there is none,
# then check for a files named "$host.hints" and ../$hosts.hints where
# $host is the hostname of the build system.  If still no hints are
# found, try looking in $system.hints and ../$system.hints where
# $system is the result of uname -s.
#
AC_ARG_WITH(hints,
  [  --with-hints=FILE       Read configuration options from FILE],
  hints=$withval)
if test "$hints" = ""; then
  host=`hostname | sed 's/\..*//'`
  if test -r $host.hints; then
    hints=$host.hints
  else
     if test -r ../$host.hints; then
       hints=../$host.hints
     fi
  fi
fi
if test "$hints" = ""; then
  sys=`uname -s`
  if test -r $sys.hints; then
    hints=$sys.hints
  else
     if test -r ../$sys.hints; then
       hints=../$sys.hints
     fi
  fi
fi
if test "$hints" != ""; then
  AC_MSG_RESULT(reading hints from $hints)
  . $hints
fi

#########
# Locate a compiler for the build machine.  This compiler should
# generate command-line programs that run on the build machine.
#
default_build_cflags="-g"
if test "$config_BUILD_CC" = ""; then
  AC_PROG_CC
  if test "$cross_compiling" = "yes"; then
    AC_MSG_ERROR([unable to find a compiler for building build tools])
  fi
  BUILD_CC=$CC
  default_build_cflags=$CFLAGS
else
  BUILD_CC=$config_BUILD_CC
  AC_MSG_CHECKING([host compiler])
  CC=$BUILD_CC
  AC_MSG_RESULT($BUILD_CC)
fi
AC_MSG_CHECKING([switches for the host compiler])
if test "$config_BUILD_CFLAGS" != ""; then
  CFLAGS=$config_BUILD_CFLAGS
  BUILD_CFLAGS=$config_BUILD_CFLAGS
else
  BUILD_CFLAGS=$default_build_cflags
fi
AC_MSG_RESULT($BUILD_CFLAGS)
if test "$config_BUILD_LIBS" != ""; then
  BUILD_LIBS=$config_BUILD_LIBS
fi
AC_SUBST(BUILD_CC)
AC_SUBST(BUILD_CFLAGS)
AC_SUBST(BUILD_LIBS)

##########
# Locate a compiler that converts C code into *.o files that run on
# the target machine.
#
AC_MSG_CHECKING([target compiler])
if test "$config_TARGET_CC" != ""; then
  TARGET_CC=$config_TARGET_CC
else
  TARGET_CC=$BUILD_CC
fi
AC_MSG_RESULT($TARGET_CC)
AC_MSG_CHECKING([switches on the target compiler])
if test "$config_TARGET_CFLAGS" != ""; then
  TARGET_CFLAGS=$config_TARGET_CFLAGS
else
  TARGET_CFLAGS=$BUILD_CFLAGS
fi
AC_MSG_RESULT($TARGET_CFLAGS)
AC_MSG_CHECKING([target linker])
if test "$config_TARGET_LINK" = ""; then
  TARGET_LINK=$TARGET_CC
else
  TARGET_LINK=$config_TARGET_LINK
fi
AC_MSG_RESULT($TARGET_LINK)
AC_MSG_CHECKING([switches on the target compiler])
if test "$config_TARGET_TFLAGS" != ""; then
  TARGET_TFLAGS=$config_TARGET_TFLAGS
else
  TARGET_TFLAGS=$BUILD_CFLAGS
fi
if test "$config_TARGET_RANLIB" != ""; then
  TARGET_RANLIB=$config_TARGET_RANLIB
else
  AC_PROG_RANLIB
  TARGET_RANLIB=$RANLIB
fi
if test "$config_TARGET_AR" != ""; then
  TARGET_AR=$config_TARGET_AR
else
  TARGET_AR='ar cr'
fi
AC_MSG_RESULT($TARGET_TFLAGS)
AC_SUBST(TARGET_CC)
AC_SUBST(TARGET_CFLAGS)
AC_SUBST(TARGET_LINK)
AC_SUBST(TARGET_LFLAGS)
AC_SUBST(TARGET_RANLIB)
AC_SUBST(TARGET_AR)

# Set the $cross variable if we are cross-compiling.  Make
# it 0 if we are not.
#
AC_MSG_CHECKING([if host and target compilers are the same])
if test "$BUILD_CC" = "$TARGET_CC"; then
  cross=0
  AC_MSG_RESULT(yes)
else
  cross=1
  AC_MSG_RESULT(no)
fi

##########
# Do we want to support multithreaded use of sqlite
#
AC_ARG_ENABLE(threadsafe, 
[  --enable-threadsafe           Support threadsafe operation],,enable_threadsafe=no)
AC_MSG_CHECKING([whether to support threadsafe operation])
if test "$enable_threadsafe" = "no"; then
  THREADSAFE=0
  AC_MSG_RESULT([no])
else
  THREADSAFE=1
  AC_MSG_RESULT([yes])
fi
AC_SUBST(THREADSAFE)

if test "$THREADSAFE" = "1"; then
  LIBS=""
  AC_CHECK_LIB(pthread, pthread_create)
  TARGET_THREAD_LIB="$LIBS"
  LIBS=""
else
  TARGET_THREAD_LIB=""
fi
AC_SUBST(TARGET_THREAD_LIB)

##########
# Do we want to support release
#
AC_ARG_ENABLE(releasemode, 
[  --enable-releasemode           Support libtool link to release mode],,enable_releasemode=no)
AC_MSG_CHECKING([whether to support shared library linked as release mode or not])
if test "$enable_releasemode" = "no"; then
  ALLOWRELEASE=""
  AC_MSG_RESULT([no])
else
  ALLOWRELEASE="-release `cat VERSION`"
  AC_MSG_RESULT([yes])
fi
AC_SUBST(ALLOWRELEASE)

##########
# Do we want temporary databases in memory
#
AC_ARG_ENABLE(tempstore, 
[  --enable-tempstore     Use an in-ram database for temporary tables (never,no,yes,always)],,enable_tempstore=yes)
AC_MSG_CHECKING([whether to use an in-ram database for temporary tables])
case "$enable_tempstore" in
  never ) 
    TEMP_STORE=0
    AC_MSG_RESULT([never])
  ;;
  no ) 
    TEMP_STORE=1
    AC_MSG_RESULT([no])
  ;;
  always ) 
     TEMP_STORE=3
    AC_MSG_RESULT([always])
  ;;
  * ) 
    TEMP_STORE=2
    AC_MSG_RESULT([yes])
  ;;
esac

AC_SUBST(TEMP_STORE)

###########
# Lots of things are different if we are compiling for Windows using
# the CYGWIN environment.  So check for that special case and handle
# things accordingly.
#
AC_MSG_CHECKING([if executables have the .exe suffix])
if test "$config_BUILD_EXEEXT" = ".exe"; then
  CYGWIN=yes
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(unknown)
fi
if test "$CYGWIN" != "yes"; then
  AC_CYGWIN
fi
if test "$CYGWIN" = "yes"; then
  BUILD_EXEEXT=.exe
else
  BUILD_EXEEXT=$EXEEXT
fi
if test "$cross" = "0"; then
  TARGET_EXEEXT=$BUILD_EXEEXT
else
  TARGET_EXEEXT=$config_TARGET_EXEEXT
fi
if test "$TARGET_EXEEXT" = ".exe"; then
  OS_UNIX=0
  OS_WIN=1
  tclsubdir=win
  TARGET_CFLAGS="$TARGET_CFLAGS -DOS_WIN=1"
else
  OS_UNIX=1
  OS_WIN=0
  tclsubdir=unix
  TARGET_CFLAGS="$TARGET_CFLAGS -DOS_UNIX=1"
fi

AC_SUBST(BUILD_EXEEXT)
AC_SUBST(OS_UNIX)
AC_SUBST(OS_WIN)
AC_SUBST(TARGET_EXEEXT)

##########
# Extract generic linker options from the environment.
#
if test "$config_TARGET_LIBS" != ""; then
  TARGET_LIBS=$config_TARGET_LIBS
else
  TARGET_LIBS=""
fi
AC_SUBST(TARGET_LIBS)

##########
# Figure out what C libraries are required to compile Tcl programs.
#
if test "$config_TARGET_TCL_LIBS" != ""; then
  TARGET_TCL_LIBS="$config_TARGET_TCL_LIBS"
else
  if test "$with_tcl" != ""; then
    extra=`echo $with_tcl/$tclsubdir/libtcl8*.a`
  fi
  CC=$TARGET_CC
  AC_CHECK_FUNC(sin, LIBS="", LIBS="-lm")
  AC_CHECK_LIB(dl, dlopen)
  otherlibs=$LIBS
  if test "$extra" != ""; then
    LIBS=$extra
  else 
    LIBS=""
    AC_SEARCH_LIBS(Tcl_Init, dnl
        tcl8.4 tcl8.3 tcl84 tcl83 tcl,,,$otherlibs)
  fi
  TARGET_TCL_LIBS="$LIBS $otherlibs"
fi
AC_SUBST(TARGET_TCL_LIBS)

##########
# Figure out where to get the TCL header files.
#
AC_MSG_CHECKING([TCL header files])
found=no
if test "$config_TARGET_TCL_INC" != ""; then
  TARGET_TCL_INC=$config_TARGET_TCL_INC
  found=yes
else
  if test "$with_tcl" != ""; then
    TARGET_TCL_INC="-I$with_tcl/generic -I$with_tcl/$tclsubdir"
    found=yes
  else
    TARGET_TCL_INC=""
    found=no
  fi
fi
if test "$found" = "yes"; then
  AC_MSG_RESULT($TARGET_TCL_INC)
else
  AC_MSG_RESULT(not specified: still searching...)
  AC_CHECK_HEADER(tcl.h, [found=yes])
fi
if test "$found" = "no"; then
  for dir in /usr/local /usr/X11 /usr/X11R6 /usr/pkg /usr/contrib /usr; do
    AC_CHECK_FILE($dir/include/tcl.h, found=yes)
    if test "$found" = "yes"; then
      TARGET_TCL_INC="-I$dir/include"
      break
    fi
  done
fi
if test "$found" = "no"; then
  TARGET_TCL_INC="-DNO_TCL=1"
fi
AC_SUBST(TARGET_TCL_INC)

##########
# Figure out what C libraries are required to compile programs
# that use "readline()" library.
#
if test "$config_TARGET_READLINE_LIBS" != ""; then
  TARGET_READLINE_LIBS="$config_TARGET_READLINE_LIBS"
else
  CC=$TARGET_CC
  LIBS=""
  AC_SEARCH_LIBS(tgetent, [readline ncurses curses termcap])
  AC_CHECK_LIB([readline], [readline])
  TARGET_READLINE_LIBS="$LIBS"
fi
AC_SUBST(TARGET_READLINE_LIBS)

##########
# Figure out where to get the READLINE header files.
#
AC_MSG_CHECKING([readline header files])
found=no
if test "$config_TARGET_READLINE_INC" != ""; then
  TARGET_READLINE_INC=$config_TARGET_READLINE_INC
  found=yes
fi
if test "$found" = "yes"; then
  AC_MSG_RESULT($TARGET_READLINE_INC)
else
  AC_MSG_RESULT(not specified: still searching...)
  AC_CHECK_HEADER(readline.h, [found=yes])
fi
if test "$found" = "no"; then
  for dir in /usr /usr/local /usr/local/readline /usr/contrib /mingw; do
    AC_CHECK_FILE($dir/include/readline.h, found=yes)
    if test "$found" = "yes"; then
      TARGET_READLINE_INC="-I$dir/include"
      break
    fi
    AC_CHECK_FILE($dir/include/readline/readline.h, found=yes)
    if test "$found" = "yes"; then
      TARGET_READLINE_INC="-I$dir/include/readline"
      break
    fi
  done
fi
if test "$found" = "yes"; then
  if test "$TARGET_READLINE_LIBS" = ""; then
    TARGET_HAVE_READLINE=0
  else
    TARGET_HAVE_READLINE=1
  fi
else
  TARGET_HAVE_READLINE=0
fi
AC_SUBST(TARGET_READLINE_INC)
AC_SUBST(TARGET_HAVE_READLINE)

#########
# Figure out whether or not we have a "usleep()" function.
#
AC_CHECK_FUNC(usleep, [TARGET_CFLAGS="$TARGET_CFLAGS -DHAVE_USLEEP=1"])

#########
# Generate the output files.
#
AC_OUTPUT([
Makefile
sqlite3.pc
])
