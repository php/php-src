dnl $Id$
dnl
dnl This file contains local autoconf functions.

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
    $2="`pwd`/$1"
  fi
])

dnl
dnl AC_ADD_LIBPATH(path)
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBPATH,[
  AC_EXPAND_PATH($1, ai_p)
  AC_PHP_ONCE(LIBPATH, $ai_p, [
    EXTRA_LIBS="$EXTRA_LIBS -L$ai_p"
    RAW_RPATHS="$RAW_RPATHS ${raw_runpath_switch}$ai_p"
    if test -n "$APXS" ; then
      RPATHS="$RPATHS ${apxs_runpath_switch}$ai_p'"
    else
      RPATHS="$RPATHS ${ld_runpath_switch}$ai_p"
    fi
  ])
])

dnl
dnl AC_ADD_INCLUDE(path)
dnl
dnl add a include path
dnl
AC_DEFUN(AC_ADD_INCLUDE,[
  AC_EXPAND_PATH($1, ai_p)
  AC_PHP_ONCE(INCLUDEPATH, $ai_p, [
    INCLUDES="$INCLUDES -I$ai_p"
  ])
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


dnl
dnl Check for cc option
dnl
AC_DEFUN(AC_CHECK_CC_OPTION,[
  echo "main(){return 0;}" > conftest.$ac_ext
  opt="$1"
  var=`echo $ac_n "$opt$ac_c"|tr -c a-zA-Z0-9 _`
  AC_MSG_CHECKING([if compiler supports -$1 really])
  ac_php_compile="${CC-cc} -$opt -c $CFLAGS $CPPFLAGS conftest.$ac_ext 2>&1"
  if eval $ac_php_compile | egrep "$opt" > /dev/null 2>&1 ; then
    eval php_cc_$var=no
	AC_MSG_RESULT(no)
  else
    eval php_cc_$var=yes
	AC_MSG_RESULT(yes)
  fi
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
dnl Stuff to do when setting up a new extension.
dnl XXX have to change the hardcoding of ".a" when we want to be able
dnl to make dynamic libraries as well.
dnl
AC_DEFUN(PHP_EXTENSION,[
  EXT_SUBDIRS="$EXT_SUBDIRS $1"
  if test "$2" != "shared" -a "$2" != "yes"; then
    _extlib="libphpext_$1.a"
    EXT_LIBS="$EXT_LIBS $1/$_extlib"
    EXTINFO_DEPS="$EXTINFO_DEPS ../ext/$1/extinfo.c.stub"
    EXT_STATIC="$EXT_STATIC $1"
  else
    EXT_SHARED="$EXT_SHARED $1"
  fi
dnl   EXT_INCLUDE_CODE="\#include \"ext/$1/php3_$1.h\"\\n$EXT_INCLUDE_CODE"
dnl   EXT_MODULE_PTRS="phpext_$1_ptr, $EXT_MODULE_PTRS"
dnl "
])

AC_SUBST(EXT_SUBDIRS)
AC_SUBST(EXT_STATIC)
AC_SUBST(EXT_SHARED)
AC_SUBST(EXT_LIBS)
AC_SUBST(EXTINFO_DEPS)
dnl AC_SUBST(EXT_INCLUDE_CODE)
dnl AC_SUBST(EXT_MODULES_PTRS)
