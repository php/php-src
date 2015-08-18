dnl $Id$
dnl
dnl This file contains local autoconf functions.

AC_DEFUN([LIBZEND_BISON_CHECK],[
  # we only support certain bison versions;
  # min: 2.4 (i.e. 204, major * 100 + minor for easier comparison)
  bison_version_min="204"
  # non-working versions, e.g. "3.0 3.2";
  # remove "none" when introducing the first incompatible bison version an 
  # separate any following additions by spaces
  bison_version_exclude=""

  # for standalone build of Zend Engine
  test -z "$SED" && SED=sed

  bison_version=none
  if test "$YACC"; then
    AC_CACHE_CHECK([for bison version], php_cv_bison_version, [
      bison_version_vars=`$YACC --version 2> /dev/null | grep 'GNU Bison' | cut -d ' ' -f 4 | $SED -e 's/\./ /g' | tr -d a-z`
      php_cv_bison_version=invalid
      if test -n "$bison_version_vars"; then
        set $bison_version_vars
        bison_version="${1}.${2}"
        bison_version_num="`expr ${1} \* 100 + ${2}`"
        if test $bison_version_num -ge $bison_version_min; then
          php_cv_bison_version="$bison_version (ok)"
          for bison_check_version in $bison_version_exclude; do
            if test "$bison_version" = "$bison_check_version"; then
              php_cv_bison_version=invalid
              break
            fi
          done
        fi
      fi
    ])
  fi
  case $php_cv_bison_version in
    ""|invalid[)]
      bison_msg="This bison version is not supported for regeneration of the Zend/PHP parsers (found: $bison_version, min: $bison_version_min, excluded: $bison_version_exclude)."
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

dnl x87 floating point internal precision control checks
dnl See: http://wiki.php.net/rfc/rounding
AC_DEFUN([ZEND_CHECK_FLOAT_PRECISION],[
  AC_MSG_CHECKING([for usable _FPU_SETCW])
  AC_TRY_LINK([
    #include <fpu_control.h>
  ],[
    fpu_control_t fpu_oldcw, fpu_cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    _FPU_GETCW(fpu_oldcw);
    fpu_cw = (fpu_oldcw & ~_FPU_EXTENDED & ~_FPU_SINGLE) | _FPU_DOUBLE;
    _FPU_SETCW(fpu_cw);
    result = a / b;
    _FPU_SETCW(fpu_oldcw);
  ], [ac_cfp_have__fpu_setcw=yes], [ac_cfp_have__fpu_setcw=no])
  if test "$ac_cfp_have__fpu_setcw" = "yes" ; then
    AC_DEFINE(HAVE__FPU_SETCW, 1, [whether _FPU_SETCW is present and usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
  
  AC_MSG_CHECKING([for usable fpsetprec])
  AC_TRY_LINK([
    #include <machine/ieeefp.h>
  ],[
    fp_prec_t fpu_oldprec;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    fpu_oldprec = fpgetprec();
    fpsetprec(FP_PD);
    result = a / b;
    fpsetprec(fpu_oldprec);
  ], [ac_cfp_have_fpsetprec=yes], [ac_cfp_have_fpsetprec=no])
  if test "$ac_cfp_have_fpsetprec" = "yes" ; then
    AC_DEFINE(HAVE_FPSETPREC, 1, [whether fpsetprec is present and usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([for usable _controlfp])
  AC_TRY_LINK([
    #include <float.h>
  ],[
    unsigned int fpu_oldcw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    fpu_oldcw = _controlfp(0, 0);
    _controlfp(_PC_53, _MCW_PC);
    result = a / b;
    _controlfp(fpu_oldcw, _MCW_PC);
  ], [ac_cfp_have__controlfp=yes], [ac_cfp_have__controlfp=no])
  if test "$ac_cfp_have__controlfp" = "yes" ; then
    AC_DEFINE(HAVE__CONTROLFP, 1, [whether _controlfp is present usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([for usable _controlfp_s])
  AC_TRY_LINK([
   #include <float.h>
  ],[
    unsigned int fpu_oldcw, fpu_cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    _controlfp_s(&fpu_cw, 0, 0);
    fpu_oldcw = fpu_cw;
    _controlfp_s(&fpu_cw, _PC_53, _MCW_PC);
    result = a / b;
    _controlfp_s(&fpu_cw, fpu_oldcw, _MCW_PC);
  ], [ac_cfp_have__controlfp_s=yes], [ac_cfp_have__controlfp_s=no])
  if test "$ac_cfp_have__controlfp_s" = "yes" ; then
    AC_DEFINE(HAVE__CONTROLFP_S, 1, [whether _controlfp_s is present and usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([whether FPU control word can be manipulated by inline assembler])
  AC_TRY_LINK([
    /* nothing */
  ],[
    unsigned int oldcw, cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    __asm__ __volatile__ ("fnstcw %0" : "=m" (*&oldcw));
    cw = (oldcw & ~0x0 & ~0x300) | 0x200;
    __asm__ __volatile__ ("fldcw %0" : : "m" (*&cw));

    result = a / b;

    __asm__ __volatile__ ("fldcw %0" : : "m" (*&oldcw));
  ], [ac_cfp_have_fpu_inline_asm_x86=yes], [ac_cfp_have_fpu_inline_asm_x86=no])
  if test "$ac_cfp_have_fpu_inline_asm_x86" = "yes" ; then
    AC_DEFINE(HAVE_FPU_INLINE_ASM_X86, 1, [whether FPU control word can be manipulated by inline assembler])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
])
