dnl
dnl $Id$
dnl

PHP_ARG_WITH(iconv, for iconv support,
[  --with-iconv[=DIR]      Include iconv support])

if test "$PHP_ICONV" != "no"; then

  PHP_SETUP_ICONV(ICONV_SHARED_LIBADD, [
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
        AC_DEFINE(ICONV_SUPPORTS_ERRNO,1,[Whether iconv supports error no or not])
],[
	AC_MSG_RESULT(no)
])

    AC_DEFINE(HAVE_ICONV,1,[Whether to build ICONV support or not])
    PHP_NEW_EXTENSION(iconv, iconv.c, $ext_shared)
    PHP_SUBST(ICONV_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR(Please reinstall the iconv library.)
  ])

fi
