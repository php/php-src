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
#    TCL_*
#
#        Lots of values are read in from the tclConfig.sh script,
#        if that script is available.  This values are used for
#        constructing and installing the TCL extension.
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
AC_PROG_AWK

#########
# Set up an appropriate program prefix
#
if test "$program_prefix" = "NONE"; then
  program_prefix=""
fi
AC_SUBST(program_prefix)

VERSION=[`cat $srcdir/VERSION | sed 's/^\([0-9]*\.*[0-9]*\).*/\1/'`]
echo "Version set to $VERSION"
AC_SUBST(VERSION)
RELEASE=`cat $srcdir/VERSION`
echo "Release set to $RELEASE"
AC_SUBST(RELEASE)
VERSION_NUMBER=[`cat $srcdir/VERSION  \
                           | sed 's/[^0-9]/ /g' \
                | awk '{printf "%d%03d%03d",$1,$2,$3}'`]
echo "Version number set to $VERSION_NUMBER"
AC_SUBST(VERSION_NUMBER)

#########
# Check to see if the --with-hints=FILE option is used.  If there is none,
# then check for a files named "$host.hints" and ../$hosts.hints where
# $host is the hostname of the build system.  If still no hints are
# found, try looking in $system.hints and ../$system.hints where
# $system is the result of uname -s.
#
AC_ARG_WITH(hints,
  AC_HELP_STRING([--with-hints=FILE],[Read configuration options from FILE]),
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
AC_HELP_STRING([--enable-threadsafe],[Support threadsafe operation]),,enable_threadsafe=no)
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
# Do we want to allow a connection created in one thread to be used
# in another thread.  This does not work on many Linux systems (ex: RedHat 9)
# due to bugs in the threading implementations.  This is thus off by default.
#
AC_ARG_ENABLE(cross-thread-connections, 
AC_HELP_STRING([--enable-cross-thread-connections],[Allow connection sharing across threads]),,enable_xthreadconnect=no)
AC_MSG_CHECKING([whether to allow connections to be shared across threads])
if test "$enable_xthreadconnect" = "no"; then
  XTHREADCONNECT=''
  AC_MSG_RESULT([no])
else
  XTHREADCONNECT='-DSQLITE_ALLOW_XTHREAD_CONNECT=1'
  AC_MSG_RESULT([yes])
fi
AC_SUBST(XTHREADCONNECT)

##########
# Do we want to set threadsOverrideEachOthersLocks variable to be 1 (true) by
# default. Normally, a test at runtime is performed to determine the
# appropriate value of this variable. Use this option only if you're sure that
# threads can safely override each others locks in all runtime situations.
#
AC_ARG_ENABLE(threads-override-locks, 
AC_HELP_STRING([--enable-threads-override-locks],[Threads can override each others locks]),,enable_threads_override_locks=no)
AC_MSG_CHECKING([whether threads can override each others locks])
if test "$enable_threads_override_locks" = "no"; then
  THREADSOVERRIDELOCKS='-1'
  AC_MSG_RESULT([no])
else
  THREADSOVERRIDELOCKS='1'
  AC_MSG_RESULT([yes])
fi
AC_SUBST(THREADSOVERRIDELOCKS)

##########
# Do we want to support release
#
AC_ARG_ENABLE(releasemode, 
AC_HELP_STRING([--enable-releasemode],[Support libtool link to release mode]),,enable_releasemode=no)
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
AC_HELP_STRING([--enable-tempstore],[Use an in-ram database for temporary tables (never,no,yes,always)]),,enable_tempstore=no)
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
  yes ) 
     TEMP_STORE=3
    AC_MSG_RESULT([always])
  ;;
  * ) 
    TEMP_STORE=1
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
  if test $OS2_SHELL ; then
    OS_UNIX=0
    OS_WIN=0
    OS_OS2=1
    TARGET_CFLAGS="$TARGET_CFLAGS -DOS_OS2=1"
    if test "$ac_compiler_gnu" == "yes" ; then
      TARGET_CFLAGS="$TARGET_CFLAGS -Zomf -Zexe -Zmap"
      BUILD_CFLAGS="$BUILD_CFLAGS -Zomf -Zexe"
    fi
  else
    OS_UNIX=0
    OS_WIN=1
    OS_OS2=0
    tclsubdir=win
    TARGET_CFLAGS="$TARGET_CFLAGS -DOS_WIN=1"
  fi
else
  OS_UNIX=1
  OS_WIN=0
  OS_OS2=0
  tclsubdir=unix
  TARGET_CFLAGS="$TARGET_CFLAGS -DOS_UNIX=1"
fi

AC_SUBST(BUILD_EXEEXT)
AC_SUBST(OS_UNIX)
AC_SUBST(OS_WIN)
AC_SUBST(OS_OS2)
AC_SUBST(TARGET_EXEEXT)

##########
# Extract generic linker options from the environment.
#
if test "$config_TARGET_LIBS" != ""; then
  TARGET_LIBS=$config_TARGET_LIBS
else
  TARGET_LIBS=""
fi

##########
# Figure out all the parameters needed to compile against Tcl.
#
# This code is derived from the SC_PATH_TCLCONFIG and SC_LOAD_TCLCONFIG
# macros in the in the tcl.m4 file of the standard TCL distribution.
# Those macros could not be used directly since we have to make some
# minor changes to accomodate systems that do not have TCL installed.
#
AC_ARG_ENABLE(tcl, AC_HELP_STRING([--disable-tcl],[do not build TCL extension]),
      [use_tcl=$enableval],[use_tcl=yes])
if test "${use_tcl}" = "yes" ; then
  AC_ARG_WITH(tcl, AC_HELP_STRING([--with-tcl=DIR],[directory containing tcl configuration (tclConfig.sh)]), with_tclconfig=${withval})
  AC_MSG_CHECKING([for Tcl configuration])
  AC_CACHE_VAL(ac_cv_c_tclconfig,[
    # First check to see if --with-tcl was specified.
    if test x"${with_tclconfig}" != x ; then
      if test -f "${with_tclconfig}/tclConfig.sh" ; then
        ac_cv_c_tclconfig=`(cd ${with_tclconfig}; pwd)`
      else
        AC_MSG_ERROR([${with_tclconfig} directory doesn't contain tclConfig.sh])
      fi
    fi
    # then check for a private Tcl installation
    if test x"${ac_cv_c_tclconfig}" = x ; then
      for i in \
            ../tcl \
            `ls -dr ../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
            `ls -dr ../tcl[[8-9]].[[0-9]] 2>/dev/null` \
            `ls -dr ../tcl[[8-9]].[[0-9]]* 2>/dev/null` \
            ../../tcl \
            `ls -dr ../../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
            `ls -dr ../../tcl[[8-9]].[[0-9]] 2>/dev/null` \
            `ls -dr ../../tcl[[8-9]].[[0-9]]* 2>/dev/null` \
            ../../../tcl \
            `ls -dr ../../../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
            `ls -dr ../../../tcl[[8-9]].[[0-9]] 2>/dev/null` \
            `ls -dr ../../../tcl[[8-9]].[[0-9]]* 2>/dev/null`
      do
        if test -f "$i/unix/tclConfig.sh" ; then
          ac_cv_c_tclconfig=`(cd $i/unix; pwd)`
          break
        fi
      done
    fi

    # check in a few common install locations
    if test x"${ac_cv_c_tclconfig}" = x ; then
      for i in \
            `ls -d ${libdir} 2>/dev/null` \
            `ls -d /usr/local/lib 2>/dev/null` \
            `ls -d /usr/contrib/lib 2>/dev/null` \
            `ls -d /usr/lib 2>/dev/null`
      do
        if test -f "$i/tclConfig.sh" ; then
           ac_cv_c_tclconfig=`(cd $i; pwd)`
           break
        fi
      done
    fi

    # check in a few other private locations
    if test x"${ac_cv_c_tclconfig}" = x ; then
      for i in \
         ${srcdir}/../tcl \
         `ls -dr ${srcdir}/../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
         `ls -dr ${srcdir}/../tcl[[8-9]].[[0-9]] 2>/dev/null` \
         `ls -dr ${srcdir}/../tcl[[8-9]].[[0-9]]* 2>/dev/null`
      do
        if test -f "$i/unix/tclConfig.sh" ; then
          ac_cv_c_tclconfig=`(cd $i/unix; pwd)`
          break
        fi
      done
    fi
  ])

  if test x"${ac_cv_c_tclconfig}" = x ; then
    use_tcl=no
    AC_MSG_WARN(Can't find Tcl configuration definitions)
    AC_MSG_WARN(*** Without Tcl the regression tests cannot be executed ***)
    AC_MSG_WARN(*** Consider using --with-tcl=... to define location of Tcl ***)
  else
    TCL_BIN_DIR=${ac_cv_c_tclconfig}
    AC_MSG_RESULT(found $TCL_BIN_DIR/tclConfig.sh)

    AC_MSG_CHECKING([for existence of $TCL_BIN_DIR/tclConfig.sh])
    if test -f "$TCL_BIN_DIR/tclConfig.sh" ; then
      AC_MSG_RESULT([loading])
      . $TCL_BIN_DIR/tclConfig.sh
    else
      AC_MSG_RESULT([file not found])
    fi
    
    #
    # If the TCL_BIN_DIR is the build directory (not the install directory),
    # then set the common variable name to the value of the build variables.
    # For example, the variable TCL_LIB_SPEC will be set to the value
    # of TCL_BUILD_LIB_SPEC. An extension should make use of TCL_LIB_SPEC
    # instead of TCL_BUILD_LIB_SPEC since it will work with both an
    # installed and uninstalled version of Tcl.
    #
    
   if test -f $TCL_BIN_DIR/Makefile ; then
      TCL_LIB_SPEC=${TCL_BUILD_LIB_SPEC}
      TCL_STUB_LIB_SPEC=${TCL_BUILD_STUB_LIB_SPEC}
      TCL_STUB_LIB_PATH=${TCL_BUILD_STUB_LIB_PATH}
    fi
    
    #
    # eval is required to do the TCL_DBGX substitution
    #
    
    eval "TCL_LIB_FILE=\"${TCL_LIB_FILE}\""
    eval "TCL_LIB_FLAG=\"${TCL_LIB_FLAG}\""
    eval "TCL_LIB_SPEC=\"${TCL_LIB_SPEC}\""
    
    eval "TCL_STUB_LIB_FILE=\"${TCL_STUB_LIB_FILE}\""
    eval "TCL_STUB_LIB_FLAG=\"${TCL_STUB_LIB_FLAG}\""
    eval "TCL_STUB_LIB_SPEC=\"${TCL_STUB_LIB_SPEC}\""
    
    AC_SUBST(TCL_VERSION)
    AC_SUBST(TCL_BIN_DIR)
    AC_SUBST(TCL_SRC_DIR)
    AC_SUBST(TCL_LIBS)
    AC_SUBST(TCL_INCLUDE_SPEC)
    
    AC_SUBST(TCL_LIB_FILE)
    AC_SUBST(TCL_LIB_FLAG)
    AC_SUBST(TCL_LIB_SPEC)
    
    AC_SUBST(TCL_STUB_LIB_FILE)
    AC_SUBST(TCL_STUB_LIB_FLAG)
    AC_SUBST(TCL_STUB_LIB_SPEC)
  fi
fi
if test "${use_tcl}" = "no" ; then
  HAVE_TCL=""
else
  HAVE_TCL=1
fi
AC_SUBST(HAVE_TCL)

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
# Figure out what C libraries are required to compile programs
# that use "fdatasync()" function.
#
CC=$TARGET_CC
LIBS=$TARGET_LIBS
AC_SEARCH_LIBS(fdatasync, [rt])
TARGET_LIBS="$LIBS"

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
# check for debug enabled
AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug],[enable debugging & verbose explain]),
      [use_debug=$enableval],[use_debug=no])
if test "${use_debug}" = "yes" ; then
  TARGET_DEBUG="-DSQLITE_DEBUG=1"
else
  TARGET_DEBUG="-DNDEBUG"
fi
AC_SUBST(TARGET_DEBUG)

#########
# Figure out whether or not we have a "usleep()" function.
#
AC_CHECK_FUNC(usleep, [TARGET_CFLAGS="$TARGET_CFLAGS -DHAVE_USLEEP=1"])

#--------------------------------------------------------------------
# Redefine fdatasync as fsync on systems that lack fdatasync
#--------------------------------------------------------------------

AC_CHECK_FUNC(fdatasync, [TARGET_CFLAGS="$TARGET_CFLAGS -DHAVE_FDATASYNC=1"])

#########
# Put out accumulated miscellaneous LIBRARIES
#
AC_SUBST(TARGET_LIBS)

#########
# Generate the output files.
#
AC_OUTPUT([
Makefile
sqlite3.pc
])
