# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_check_define.html
# ===========================================================================
#
# SYNOPSIS
#
#   AC_CHECK_DEFINE([symbol], [ACTION-IF-FOUND], [ACTION-IF-NOT])
#   AX_CHECK_DEFINE([includes],[symbol], [ACTION-IF-FOUND], [ACTION-IF-NOT])
#
# DESCRIPTION
#
#   Complements AC_CHECK_FUNC but it does not check for a function but for a
#   define to exist. Consider a usage like:
#
#    AC_CHECK_DEFINE(__STRICT_ANSI__, CFLAGS="$CFLAGS -D_XOPEN_SOURCE=500")
#
# LICENSE
#
#   Copyright (c) 2008 Guido U. Draheim <guidod@gmx.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 11

AU_ALIAS([AC_CHECK_DEFINED], [AC_CHECK_DEFINE])
AC_DEFUN([AC_CHECK_DEFINE],[
AS_VAR_PUSHDEF([ac_var],[ac_cv_defined_$1])dnl
AC_CACHE_CHECK([for $1 defined], ac_var,
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[
  #ifdef $1
  int ok;
  (void)ok;
  #else
  choke me
  #endif
]])],[AS_VAR_SET(ac_var, yes)],[AS_VAR_SET(ac_var, no)]))
AS_IF([test AS_VAR_GET(ac_var) != "no"], [$2], [$3])dnl
AS_VAR_POPDEF([ac_var])dnl
])

AU_ALIAS([AX_CHECK_DEFINED], [AX_CHECK_DEFINE])
AC_DEFUN([AX_CHECK_DEFINE],[
AS_VAR_PUSHDEF([ac_var],[ac_cv_defined_$2_$1])dnl
AC_CACHE_CHECK([for $2 defined in $1], ac_var,
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <$1>]], [[
  #ifdef $2
  int ok;
  (void)ok;
  #else
  choke me
  #endif
]])],[AS_VAR_SET(ac_var, yes)],[AS_VAR_SET(ac_var, no)]))
AS_IF([test AS_VAR_GET(ac_var) != "no"], [$3], [$4])dnl
AS_VAR_POPDEF([ac_var])dnl
])

AC_DEFUN([AX_CHECK_FUNC],
[AS_VAR_PUSHDEF([ac_var], [ac_cv_func_$2])dnl
AC_CACHE_CHECK([for $2], ac_var,
dnl AC_LANG_FUNC_LINK_TRY
[AC_LINK_IFELSE([AC_LANG_PROGRAM([$1
                #undef $2
                char $2 ();],[
                char (*f) () = $2;
                return f != $2; ])],
                [AS_VAR_SET(ac_var, yes)],
                [AS_VAR_SET(ac_var, no)])])
AS_IF([test AS_VAR_GET(ac_var) = yes], [$3], [$4])dnl
AS_VAR_POPDEF([ac_var])dnl
])# AC_CHECK_FUNC
