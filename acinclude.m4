dnl $Id$
dnl
dnl This file contains local autoconf functions.


dnl
dnl Check for cc option
dnl
AC_DEFUN(AC_CHECK_CC_OPTION,[
  echo "main(){return 0;}" > conftest.$ac_ext
  opt="$1"
  var=`echo -n $opt|tr -c a-zA-Z0-9 _`
  AC_MSG_CHECKING([if compiler supports $1 really])
  ac_compile='${CC-cc} -$opt -c $CFLAGS $CPPFLAGS conftest.$ac_ext 2>&1'
  if eval $ac_compile | egrep "$opt" > /dev/null 2>&1 ; then
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
  _extlib="libphpext_$1.a"
  EXT_LIBS="$EXT_LIBS $1/$_extlib"
  EXTINFO_DEPS="$EXTINFO_DEPS ../ext/$1/extinfo.c.stub"
])

AC_SUBST(EXT_SUBDIRS)
AC_SUBST(EXT_LIBS)
AC_SUBST(EXTINFO_DEPS)
