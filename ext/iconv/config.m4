dnl
dnl $Id$
dnl

PHP_ARG_WITH(iconv, for iconv support,
[  --with-iconv[=DIR]      Include iconv support])

if test "$PHP_ICONV" != "no"; then

  PHP_SETUP_ICONV(ICONV_SHARED_LIBADD, [
    iconv_avail="yes";
  ],[
    iconv_avail="no";
  ])

  if test "$iconv_avail" != "no"; then

    AC_MSG_CHECKING([if iconv supports errno])
    AC_TRY_RUN([
#define LIBICONV_PLUG
#include <iconv.h>
#include <errno.h>
#if defined(_LIBICONV_H)
#define icv_open(a, b) libiconv_open(a, b)
#define icv_close(a) libiconv_close(a)
#define icv(a, b, c, d, e) libiconv(a, b, c, d, e)
#else
#define icv_open(a, b) iconv_open(a, b)
#define icv_close(a) iconv_close(a)
#define icv(a, b, c, d, e) iconv(a, b, c, d, e)
#endif

int main() {
	iconv_t cd;
	cd = icv_open( "*blahblah*", "*blahblah*" );
	if( cd == (iconv_t)(-1) ) {
		if( errno == EINVAL ) {
			return 0;
		} else {
			return 1;
		}
	}
	icv_close( cd );
	return 2;
}
    ],[
      AC_MSG_RESULT(yes)
      PHP_DEFINE([ICONV_SUPPORTS_ERRNO],1)
      AC_DEFINE([ICONV_SUPPORTS_ERRNO],1,[Whether iconv supports error no or not])
    ],[
      AC_MSG_RESULT(no)
      PHP_DEFINE([ICONV_SUPPORTS_ERRNO],0)
      AC_DEFINE([ICONV_SUPPORTS_ERRNO],0,[Whether iconv supports error no or not])
    ])

    if test -z "$iconv_lib_name"; then
      AC_MSG_CHECKING([if iconv is glibc's])
      AC_TRY_COMPILE([#include <iconv.h>],[void __gconv(); int main() { __gconv(); }],
      [
        AC_MSG_RESULT(yes)
        PHP_DEFINE([HAVE_GLIBC_ICONV],1)
        AC_DEFINE([HAVE_GLIBC_ICONV],1,[glibc's iconv implementation])
        PHP_DEFINE([PHP_ICONV_IMPL],[\"glibc\"])
        AC_DEFINE([PHP_ICONV_IMPL],["glibc"],[Which iconv implementation to use])
      ],[
        AC_MSG_RESULT(no)
      ])
    else
      case "$iconv_lib_name" in
        iconv [)]
          PHP_DEFINE([PHP_ICONV_IMPL],[\"libiconv\"])
          AC_DEFINE([PHP_ICONV_IMPL],["libiconv"],[Which iconv implementation to use])
          ;;
        giconv [)]
          PHP_DEFINE([PHP_ICONV_IMPL],[\"giconv\"])
          AC_DEFINE([PHP_ICONV_IMPL],["giconv"],[Which iconv implementation to use])
          ;;
      esac
    fi 

    PHP_NEW_EXTENSION(iconv, iconv.c, $ext_shared)
    PHP_SUBST(ICONV_SHARED_LIBADD)
  else
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
fi
