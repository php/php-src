dnl
dnl $Id$
dnl
dnl
dnl TL_DEF_HAVE(what [, why])
dnl
dnl Generates 'AC_DEFINE(HAVE_WHAT, 1, [WHY])'
dnl
AC_DEFUN([TL_DEF_HAVE],[AC_DEFINE([HAVE_]translit($1,a-z_.-,A-Z___),1,[ $2 ])])dnl

dnl
dnl TL_CHECK_INT_TYPE(type)
dnl
AC_DEFUN([TL_CHECK_INT_TYPE],[
AC_CACHE_CHECK([for $1], ac_cv_int_type_$1, [
AC_TRY_COMPILE([
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if HAVE_INTTYPES_H
# include <inttypes.h>
#elif HAVE_STDINT_H   
# include <stdint.h>  
#endif],
[if (($1 *) 0)
  return 0;   
if (sizeof ($1))
  return 0;
], [ac_cv_int_type_$1=yes], [ac_cv_int_type_$1=no])
])
if test "$ac_cv_int_type_$1" = "yes"; then
  TL_DEF_HAVE($1, [Define if $1 type is present.])
fi
])dnl

dnl
dnl AC_TIMELIB_C_BIGENDIAN
dnl Replacement macro for AC_C_BIGENDIAN
dnl
AC_DEFUN([AC_TIMELIB_C_BIGENDIAN],
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
])dnl

dnl Check for types, sizes, etc. needed by timelib
AC_CHECK_SIZEOF(long, 8)
AC_CHECK_SIZEOF(int, 4)
TL_CHECK_INT_TYPE(int32_t)
TL_CHECK_INT_TYPE(uint32_t)

dnl Check for headers needed by timelib
AC_CHECK_HEADERS([ \
sys/types.h \
inttypes.h \
stdint.h \
string.h \
stdlib.h
])

dnl Check for strtoll, atoll
AC_CHECK_FUNCS(strtoll atoll strftime)
