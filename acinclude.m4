dnl $Id$
dnl
dnl This file contains local autoconf functions.

dnl
dnl PHP_SET_SYM_FILE(path)
dnl
dnl set the path of the file which contains the symbol export list
dnl
AC_DEFUN(PHP_SET_SYM_FILE,
[
  PHP_SYM_FILE="$1"
])

dnl
dnl PHP_BUILD_THREAD_SAFE
dnl
AC_DEFUN(PHP_BUILD_THREAD_SAFE,[
  PHP_THREAD_SAFETY=yes
])

dnl
dnl PHP_BUILD_SHARED
dnl
AC_DEFUN(PHP_BUILD_SHARED,[
  php_build_target=shared
])

dnl
dnl PHP_BUILD_STATIC
dnl
AC_DEFUN(PHP_BUILD_STATIC,[
  php_build_target=static
])

dnl
dnl PHP_BUILD_PROGRAM
dnl
AC_DEFUN(PHP_BUILD_PROGRAM,[
  php_build_target=program
])

dnl
dnl AC_PHP_ONCE(namespace, variable, code)
dnl
dnl execute code, if variable is not set in namespace
dnl
AC_DEFUN(AC_PHP_ONCE,[
  unique=`echo $ac_n "$2$ac_c" | tr -c -d a-zA-Z0-9`
  cmd="echo $ac_n \"\$$1$unique$ac_c\""
  if test -n "$unique" && test "`eval $cmd`" = "" ; then
    eval "$1$unique=set"
    $3
  fi
])

dnl
dnl AC_EXPAND_PATH(path, variable)
dnl
dnl expands path to an absolute path and assigns it to variable
dnl
AC_DEFUN(AC_EXPAND_PATH,[
  if test -z "$1" || echo "$1" | grep '^/' >/dev/null ; then
    $2="$1"
  else
    ep_dir="`dirname \"$1\"`"
    ep_realdir="`(cd \"$ep_dir\" && pwd)`"
    $2="$ep_realdir/`basename \"$1\"`"
  fi
])

dnl
dnl AC_ADD_LIBPATH(path)
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBPATH,[
  if test "$1" != "/usr/lib"; then
    AC_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(LIBPATH, $ai_p, [
      EXTRA_LIBS="$EXTRA_LIBS -L$ai_p"
      PHP_RPATHS="$PHP_RPATHS $ai_p"
    ])
  fi
])

dnl
dnl AC_BUILD_RPATH()
dnl
dnl builds RPATH from PHP_RPATHS
dnl
AC_DEFUN(AC_BUILD_RPATH,[
  if test "$enable_rpath" = "yes" && test -n "$PHP_RPATHS"; then
    OLD_RPATHS="$PHP_RPATHS"
	PHP_RPATHS=""
	for i in $OLD_RPATHS; do
	    PHP_RPATHS="$PHP_RPATHS -R $i"
        NATIVE_RPATHS="$NATIVE_RPATHS ${ld_runpath_switch}$i"
	  done
  fi
])

dnl
dnl AC_ADD_INCLUDE(path)
dnl
dnl add a include path
dnl
AC_DEFUN(AC_ADD_INCLUDE,[
  if test "$1" != "/usr/include"; then
    AC_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(INCLUDEPATH, $ai_p, [
      INCLUDES="$INCLUDES -I$ai_p"
    ])
  fi
])

dnl
dnl AC_ADD_LIBRARY(library)
dnl
dnl add a library to the link line
dnl
AC_DEFUN(AC_ADD_LIBRARY,[
  AC_PHP_ONCE(LIBRARY, $1, [
    EXTRA_LIBS="$EXTRA_LIBS -l$1"
  ])
])

dnl
dnl AC_ADD_LIBRARY_WITH_PATH(library, path)
dnl
dnl add a library to the link line and path to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBRARY_WITH_PATH,[
  AC_ADD_LIBPATH($2)
  AC_ADD_LIBRARY($1)
])


AC_DEFUN(AM_SET_LIBTOOL_VARIABLE,[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])

dnl
dnl Check for cc option
dnl
AC_DEFUN(AC_CHECK_CC_OPTION,[
  echo "main(){return 0;}" > conftest.$ac_ext
  opt="$1"
  var=`echo $ac_n "$opt$ac_c"|tr -c a-zA-Z0-9 _`
  AC_MSG_CHECKING([if compiler supports -$1 really])
  ac_php_compile="${CC-cc} -$opt -o conftest $CFLAGS $CPPFLAGS conftest.$ac_ext 2>&1"
  if eval $ac_php_compile 2>&1 | egrep "$opt" > /dev/null 2>&1 ; then
    eval php_cc_$var=no
	AC_MSG_RESULT(no)
  else
    if eval ./conftest 2>/dev/null ; then
      eval php_cc_$var=yes
	  AC_MSG_RESULT(yes)
    else
      eval php_cc_$var=no
	  AC_MSG_RESULT(no)
    fi
  fi
])

AC_DEFUN(PHP_REGEX,[

if test "$REGEX_TYPE" = "php"; then
  REGEX_LIB=regex/libregex.la
  REGEX_DIR=regex
  AC_DEFINE(HSREGEX)
  AC_DEFINE(REGEX,1)
  PHP_OUTPUT(regex/Makefile)
elif test "$REGEX_TYPE" = "system"; then
  AC_DEFINE(REGEX,0)
elif test "$REGEX_TYPE" = "apache"; then
  AC_DEFINE(REGEX,2)
fi

AC_MSG_CHECKING(which regex library to use)
AC_MSG_RESULT($REGEX_TYPE)

AC_SUBST(REGEX_DIR)
AC_SUBST(REGEX_LIB)
AC_SUBST(HSREGEX)
])

dnl
dnl See if we have broken header files like SunOS has.
dnl
AC_DEFUN(AC_MISSING_FCLOSE_DECL,[
  AC_MSG_CHECKING([for fclose declaration])
  AC_TRY_COMPILE([#include <stdio.h>],[int (*func)() = fclose],[
    AC_DEFINE(MISSING_FCLOSE_DECL,0)
    AC_MSG_RESULT(ok)
  ],[
    AC_DEFINE(MISSING_FCLOSE_DECL,1)
    AC_MSG_RESULT(missing)
  ])
])

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN(AC_BROKEN_SPRINTF,[
  AC_MSG_CHECKING([for broken sprintf])
  AC_TRY_RUN([main() { char buf[20]; exit (sprintf(buf,"testing 123")!=11); }],[
    AC_DEFINE(BROKEN_SPRINTF,0)
    AC_MSG_RESULT(ok)
  ],[
    AC_DEFINE(BROKEN_SPRINTF,1)
    AC_MSG_RESULT(broken)
  ],[
    AC_DEFINE(BROKEN_SPRINTF,0)
    AC_MSG_RESULT(cannot check, guessing ok)
  ])
])

dnl
dnl PHP_EXTENSION(extname [, shared])
dnl
dnl Includes an extension in the build.
dnl
dnl "extname" is the name of the ext/ subdir where the extension resides
dnl "shared" can be set to "shared" or "yes" to build the extension as
dnl a dynamically loadable library.
dnl
AC_DEFUN(PHP_EXTENSION,[
  if test -d "$cwd/$srcdir/ext/$1" ; then
    EXT_SUBDIRS="$EXT_SUBDIRS $1"
    if test "$2" != "shared" -a "$2" != "yes"; then
      _extlib="libphpext_$1.a"
      EXT_LTLIBS="$EXT_LTLIBS ext/$1/libphpext_$1.la"
	  EXT_LIBS="$EXT_LIBS $1/$_extlib"
      EXT_STATIC="$EXT_STATIC $1"
    else
      EXT_SHARED="$EXT_SHARED $1"
    fi
    PHP_OUTPUT(ext/$1/Makefile)
  fi
])

AC_SUBST(EXT_SUBDIRS)
AC_SUBST(EXT_STATIC)
AC_SUBST(EXT_SHARED)
AC_SUBST(EXT_LIBS)
AC_SUBST(EXT_LTLIBS)

dnl
dnl Solaris requires main code to be position independent in order
dnl to let shared objects find symbols.  Weird.  Ugly.
dnl
dnl Must be run after all --with-NN options that let the user
dnl choose dynamic extensions, and after the gcc test.
dnl
AC_DEFUN(PHP_SOLARIS_PIC_WEIRDNESS,[
  AC_MSG_CHECKING(whether -fPIC is required)
  if test "$EXT_SHARED" != ""; then
    os=`uname -sr 2>/dev/null`
    case "$os" in
        "SunOS 5.6"|"SunOS 5.7")
          case "$CC" in
	    gcc*|egcs*) CFLAGS="$CFLAGS -fPIC";;
	    *) CFLAGS="$CFLAGS -fpic";;
	  esac
	  AC_MSG_RESULT(yes);;
	*)
	  AC_MSG_RESULT(no);;
    esac
  else
    AC_MSG_RESULT(no)
  fi
])

dnl
dnl Checks whether $withval is "shared" or starts with "shared,XXX"
dnl and sets $shared to "yes" or "no", and removes "shared,?" stuff
dnl from $withval.
dnl
AC_DEFUN(PHP_WITH_SHARED,[
    case $withval in
	shared)
	    shared=yes
	    withval=yes
	    ;;
	shared,*)
	    shared=yes
	    withval=`echo $withval | sed -e 's/^shared,//'`      
	    ;;
	*)
	    shared=no
	    ;;
    esac
])

dnl The problem is that the default compilation flags in Solaris 2.6 won't
dnl let programs access large files;  you need to tell the compiler that
dnl you actually want your programs to work on large files.  For more
dnl details about this brain damage please see:
dnl http://www.sas.com/standards/large.file/x_open.20Mar96.html

dnl Written by Paul Eggert <eggert@twinsun.com>.

AC_DEFUN(AC_SYS_LFS,
[dnl
  # If available, prefer support for large files unless the user specified
  # one of the CPPFLAGS, LDFLAGS, or LIBS variables.
  AC_MSG_CHECKING(whether large file support needs explicit enabling)
  ac_getconfs=''
  ac_result=yes
  ac_set=''
  ac_shellvars='CPPFLAGS LDFLAGS LIBS'
  for ac_shellvar in $ac_shellvars; do
    case $ac_shellvar in
      CPPFLAGS) ac_lfsvar=LFS_CFLAGS ;;
      *) ac_lfsvar=LFS_$ac_shellvar ;;
    esac
    eval test '"${'$ac_shellvar'+set}"' = set && ac_set=$ac_shellvar
    (getconf $ac_lfsvar) >/dev/null 2>&1 || { ac_result=no; break; }
    ac_getconf=`getconf $ac_lfsvar`
    ac_getconfs=$ac_getconfs$ac_getconf
    eval ac_test_$ac_shellvar=\$ac_getconf
  done
  case "$ac_result$ac_getconfs" in
    yes) ac_result=no ;;
  esac
  case "$ac_result$ac_set" in
    yes?*) ac_result="yes, but $ac_set is already set, so use its settings"
  esac
  AC_MSG_RESULT($ac_result)
  case $ac_result in
    yes)
      for ac_shellvar in $ac_shellvars; do
        eval $ac_shellvar=\$ac_test_$ac_shellvar
      done ;;
  esac
])

AC_DEFUN(AC_SOCKADDR_SA_LEN,[
  AC_CACHE_CHECK([for field sa_len in struct sockaddr],ac_cv_sockaddr_sa_len,[
    AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/socket.h>],
    [struct sockaddr s; s.sa_len;],
    [ac_cv_sockaddr_sa_len=yes
     AC_DEFINE(HAVE_SOCKADDR_SA_LEN)],
    [ac_cv_sockaddr_sa_len=no])
  ])
])

dnl ## PHP_AC_OUTPUT(file)
dnl ## adds "file" to the list of files generated by AC_OUTPUT
dnl ## This macro can be used several times.
AC_DEFUN(PHP_OUTPUT,[
  PHP_OUTPUT_FILES="$PHP_OUTPUT_FILES $1"
])
