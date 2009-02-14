dnl $Id$
dnl
dnl This file contains local autoconf functions.

AC_DEFUN([LIBZEND_BISON_CHECK],[
  # we only support certain bison versions
  bison_version_list="1.28 1.35 1.75 1.875 2.0 2.1 2.2 2.3 2.4 2.4.1"

  # for standalone build of Zend Engine
  test -z "$SED" && SED=sed

  bison_version=none
  if test "$YACC"; then
    AC_CACHE_CHECK([for bison version], php_cv_bison_version, [
      bison_version_vars=`bison --version 2> /dev/null | grep 'GNU Bison' | cut -d ' ' -f 4 | $SED -e 's/\./ /' | tr -d a-z`
      php_cv_bison_version=invalid
      if test -n "$bison_version_vars"; then
        set $bison_version_vars
        bison_version="${1}.${2}"
        for bison_check_version in $bison_version_list; do
          if test "$bison_version" = "$bison_check_version"; then
            php_cv_bison_version="$bison_check_version (ok)"
            break
          fi
        done
      fi
    ])
  fi
  case $php_cv_bison_version in
    ""|invalid[)]
      bison_msg="bison versions supported for regeneration of the Zend/PHP parsers: $bison_version_list (found: $bison_version)."
      AC_MSG_WARN([$bison_msg])
      YACC="exit 0;"
      ;;
  esac
])

AC_DEFUN([ZEND_FP_EXCEPT],[
  AC_CACHE_CHECK(whether fp_except is defined, ac_cv_type_fp_except,[
    AC_TRY_COMPILE([
#include <floatingpoint.h>
],[
fp_except x = (fp_except) 0;
],[
     ac_cv_type_fp_except=yes
],[
     ac_cv_type_fp_except=no
],[
     ac_cv_type_fp_except=no
])])
  if test "$ac_cv_type_fp_except" = "yes"; then
    AC_DEFINE(HAVE_FP_EXCEPT, 1, [whether floatingpoint.h defines fp_except])
  fi
])

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN([AC_ZEND_BROKEN_SPRINTF],[
  AC_CACHE_CHECK(whether sprintf is broken, ac_cv_broken_sprintf,[
    AC_TRY_RUN([main() {char buf[20];exit(sprintf(buf,"testing 123")!=11); }],[
      ac_cv_broken_sprintf=no
    ],[
      ac_cv_broken_sprintf=yes
    ],[
      ac_cv_broken_sprintf=no
    ])
  ])
  if test "$ac_cv_broken_sprintf" = "yes"; then
    ac_result=1
  else
    ac_result=0
  fi
  AC_DEFINE_UNQUOTED(ZEND_BROKEN_SPRINTF, $ac_result, [Whether sprintf is broken])
])

dnl
dnl AC_ZEND_C_BIGENDIAN
dnl Replacement macro for AC_C_BIGENDIAN
dnl
AC_DEFUN([AC_ZEND_C_BIGENDIAN],
[AC_CACHE_CHECK([whether byte ordering is bigendian], ac_cv_c_bigendian_php,
 [
  ac_cv_c_bigendian_php=unknown
  AC_TRY_RUN(
  [
int main(void)
{
        short one = 1;
        char *cp = (char *)&one;

        if (*cp == 0) {
                return(0);
        } else {
                return(1);
        }
}
  ], [ac_cv_c_bigendian_php=yes], [ac_cv_c_bigendian_php=no], [ac_cv_c_bigendian_php=unknown])
 ])
 if test $ac_cv_c_bigendian_php = yes; then
   AC_DEFINE(WORDS_BIGENDIAN, [], [Define if processor uses big-endian word])
 fi
])

AC_DEFUN([AM_SET_LIBTOOL_VARIABLE],[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])
