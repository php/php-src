dnl
dnl $Id$
dnl

PHP_ARG_WITH(iconv, for iconv support,
[  --without-iconv[=DIR]   Include iconv support], yes)

if test "$PHP_ICONV" != "no"; then

  PHP_SETUP_ICONV(ICONV_SHARED_LIBADD, [
    iconv_avail="yes";
  ],[
    iconv_avail="no";
  ])

  if test "$iconv_avail" != "no"; then
    iconv_cflags_save="$CFLAGS"
    iconv_ldflags_save="$LDFLAGS"

    if test -z "$ICONV_DIR"; then
      PHP_ICONV_PREFIX="/usr"
    else
      PHP_ICONV_PREFIX="$ICONV_DIR"
    fi

    CFLAGS="-I$PHP_ICONV_PREFIX/include $CFLAGS"
    LDFLAGS="-L$PHP_ICONV_PREFIX/lib $LDFLAGS"

    if test -r $PHP_ICONV_PREFIX/include/giconv.h; then
      PHP_ICONV_H_PATH="$PHP_ICONV_PREFIX/include/giconv.h"
    else
      PHP_ICONV_H_PATH="$PHP_ICONV_PREFIX/include/iconv.h"
    fi 

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

    AC_MSG_CHECKING([if iconv supports errno])
    AC_TRY_RUN([
#include <$PHP_ICONV_H_PATH>
#include <errno.h>

int main() {
  iconv_t cd;
  cd = iconv_open( "*blahblah*", "*blahblah*" );
  if (cd == (iconv_t)(-1)) {
    if (errno == EINVAL) {
      return 0;
	} else {
      return 1;
    }
  }
  iconv_close( cd );
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

    AC_MSG_CHECKING([if your cpp allows macro usage in include lines])
    AC_TRY_COMPILE([
#define FOO <$PHP_ICONV_H_PATH>
#include FOO
    ], [], [
      AC_MSG_RESULT([yes])
      PHP_DEFINE([PHP_ICONV_H_PATH], [<$PHP_ICONV_H_PATH>])
      AC_DEFINE_UNQUOTED([PHP_ICONV_H_PATH], [<$PHP_ICONV_H_PATH>], [Path to iconv.h])
    ], [
      AC_MSG_RESULT([no])
    ])

    CFLAGS="$iconv_cflags_save"
    LDFLAGS="$iconv_ldflags_save"

    PHP_NEW_EXTENSION(iconv, iconv.c, $ext_shared,, [-I\"$PHP_ICONV_PREFIX/include\"])
    PHP_SUBST(ICONV_SHARED_LIBADD)
  else
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
fi
