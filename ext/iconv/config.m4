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
    iconv_cflags_save="$CFLAGS"
    CFLAGS="$CFLAGS $INCLUDES"
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
      AC_TRY_LINK([#include <gnu/libc-version.h>],[gnu_get_libc_version();],
      [
        AC_MSG_RESULT(yes)
        iconv_impl_name="glibc"
      ],[
        AC_MSG_RESULT(no)
      ])
    else
      case "$iconv_lib_name" in
        iconv [)]
          AC_MSG_CHECKING([if iconv is Konstantin Chugeuv's])
          AC_TRY_LINK([#include <iconv.h>],[iconv_ccs_init(NULL, NULL);],
          [
            AC_MSG_RESULT(yes)
            iconv_impl_name="bsd"
          ],[
            AC_MSG_RESULT(no)
            iconv_impl_name="gnu_libiconv"
          ])
          ;;

        giconv [)]
          iconv_impl_name="gnu_libiconv"
          ;;

        biconv [)]
          iconv_impl_name="bsd"
          ;;
      esac
    fi 

    case "$iconv_impl_name" in
      gnu_libiconv [)]
        PHP_DEFINE([PHP_ICONV_IMPL],[\"libiconv\"])
        AC_DEFINE([PHP_ICONV_IMPL],["libiconv"],[Which iconv implementation to use])
        ;;

      bsd [)]
        PHP_DEFINE([HAVE_BSD_ICONV],1)
        AC_DEFINE([HAVE_BSD_ICONV],1,[Konstantin Chugeuv's iconv implementation])
        PHP_DEFINE([PHP_ICONV_IMPL],[\"BSD iconv\"])
        AC_DEFINE([PHP_ICONV_IMPL],["BSD iconv"],[Which iconv implementation to use])
        ;;

      glibc [)]
        PHP_DEFINE([HAVE_GLIBC_ICONV],1)
        AC_DEFINE([HAVE_GLIBC_ICONV],1,[glibc's iconv implementation])
        PHP_DEFINE([PHP_ICONV_IMPL],[\"glibc\"])
        AC_DEFINE([PHP_ICONV_IMPL],["glibc"],[Which iconv implementation to use])
        ;;
    esac

    CFLAGS="$iconv_cflags_save"

    PHP_NEW_EXTENSION(iconv, iconv.c, $ext_shared)
    PHP_SUBST(ICONV_SHARED_LIBADD)
  else
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
fi
