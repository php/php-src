PHP_ARG_WITH([iconv],
  [for iconv support],
  [AS_HELP_STRING([[--without-iconv[=DIR]]],
    [Exclude iconv support])],
  [yes])

if test "$PHP_ICONV" != "no"; then

  PHP_SETUP_ICONV(ICONV_SHARED_LIBADD, [
    iconv_avail="yes";
  ],[
    iconv_avail="no";
  ])

  if test "$iconv_avail" != "no"; then
    if test -z "$ICONV_DIR"; then
      for i in /usr/local /usr; do
        if test -f "$i/include/iconv.h" || test -f "$i/include/giconv.h"; then
          PHP_ICONV_PREFIX="$i"
          break
        fi
      done
      if test -z "$PHP_ICONV_PREFIX"; then
        PHP_ICONV_PREFIX="/usr"
      fi
    else
      PHP_ICONV_PREFIX="$ICONV_DIR"
    fi

    CFLAGS="-I$PHP_ICONV_PREFIX/include $CFLAGS"
    LDFLAGS="-L$PHP_ICONV_PREFIX/$PHP_LIBDIR $LDFLAGS"

    if test -r "$PHP_ICONV_PREFIX/include/giconv.h"; then
      PHP_ICONV_H_PATH="$PHP_ICONV_PREFIX/include/giconv.h"
    else
      PHP_ICONV_H_PATH="$PHP_ICONV_PREFIX/include/iconv.h"
	fi

    AC_MSG_CHECKING([if iconv is glibc's])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <gnu/libc-version.h>]], [[gnu_get_libc_version();]])],[
      AC_MSG_RESULT(yes)
      iconv_impl_name="glibc"
    ],[
      AC_MSG_RESULT(no)
    ])

    if test -z "$iconv_impl_name"; then
      AC_MSG_CHECKING([if using GNU libiconv])
      php_iconv_old_ld="$LDFLAGS"
      LDFLAGS="-liconv $LDFLAGS"
      AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <$PHP_ICONV_H_PATH>
int main() {
  printf("%d", _libiconv_version);
  return 0;
}
      ]])],[
        AC_MSG_RESULT(yes)
        iconv_impl_name="gnu_libiconv"
      ],[
        AC_MSG_RESULT(no)
        LDFLAGS="$php_iconv_old_ld"
      ],[
        AC_MSG_RESULT([no, cross-compiling])
        LDFLAGS="$php_iconv_old_ld"
      ])
    fi

    if test -z "$iconv_impl_name"; then
      AC_MSG_CHECKING([if iconv is Konstantin Chuguev's])
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <iconv.h>]], [[iconv_ccs_init(NULL, NULL);]])],[
        AC_MSG_RESULT(yes)
        iconv_impl_name="bsd"
      ],[
        AC_MSG_RESULT(no)
      ])
    fi

    if test -z "$iconv_impl_name"; then
      AC_MSG_CHECKING([if using IBM iconv])
      php_iconv_old_ld="$LDFLAGS"
      LDFLAGS="-liconv $LDFLAGS"
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <iconv.h>]], [[cstoccsid("");]])],[
        AC_MSG_RESULT(yes)
        iconv_impl_name="ibm"
      ],[
        AC_MSG_RESULT(no)
        LDFLAGS="$php_iconv_old_ld"
      ])
    fi

    echo > ext/iconv/php_have_bsd_iconv.h
    echo > ext/iconv/php_have_glibc_iconv.h
    echo > ext/iconv/php_have_libiconv.h
    echo > ext/iconv/php_have_ibm_iconv.h

    case "$iconv_impl_name" in
      gnu_libiconv [)]
        PHP_DEFINE([PHP_ICONV_IMPL],[\"libiconv\"],[ext/iconv])
        AC_DEFINE([PHP_ICONV_IMPL],["libiconv"],[Which iconv implementation to use])
        PHP_DEFINE([HAVE_LIBICONV],1,[ext/iconv])
        PHP_ADD_LIBRARY_WITH_PATH(iconv, "$PHP_ICONV_PREFIX/$PHP_LIBDIR", ICONV_SHARED_LIBADD)
        ;;

      bsd [)]
        PHP_DEFINE([HAVE_BSD_ICONV],1,[ext/iconv])
        AC_DEFINE([HAVE_BSD_ICONV],1,[Konstantin Chuguev's iconv implementation])
        PHP_DEFINE([PHP_ICONV_IMPL],[\"BSD iconv\"],[ext/iconv])
        AC_DEFINE([PHP_ICONV_IMPL],["BSD iconv"],[Which iconv implementation to use])
        ;;

      glibc [)]
        PHP_DEFINE([HAVE_GLIBC_ICONV],1,[ext/iconv])
        AC_DEFINE([HAVE_GLIBC_ICONV],1,[glibc's iconv implementation])
        PHP_DEFINE([PHP_ICONV_IMPL],[\"glibc\"],[ext/iconv])
        AC_DEFINE([PHP_ICONV_IMPL],["glibc"],[Which iconv implementation to use])
        ;;
      ibm [)]
        PHP_DEFINE([HAVE_IBM_ICONV],1,[ext/iconv])
        AC_DEFINE([HAVE_IBM_ICONV],1,[IBM iconv implementation])
        PHP_DEFINE([PHP_ICONV_IMPL],[\"IBM iconv\"],[ext/iconv])
        AC_DEFINE([PHP_ICONV_IMPL],["IBM iconv"],[Which iconv implementation to use])
        ;;
    esac

    AC_MSG_CHECKING([if iconv supports errno])
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
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
    ]])],[
      AC_MSG_RESULT(yes)
    ],[
      AC_MSG_RESULT(no)
      AC_MSG_ERROR(iconv does not support errno)
    ],[
      AC_MSG_RESULT(yes, cross-compiling)
    ])

    AC_MSG_CHECKING([if iconv supports //IGNORE])
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <$PHP_ICONV_H_PATH>
#include <stdlib.h>

int main() {
  iconv_t cd = iconv_open( "UTF-8//IGNORE", "UTF-8" );
  if(cd == (iconv_t)-1) {
    return 1;
  }
  char *in_p = "\xC3\xC3\xC3\xB8";
  size_t in_left = 4, out_left = 4096;
  char *out = malloc(out_left);
  char *out_p = out;
  size_t result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
  if(result == (size_t)-1) {
    return 1;
  }
  return 0;
}
   ]])],[
      AC_MSG_RESULT(yes)
      PHP_DEFINE([ICONV_BROKEN_IGNORE],0,[ext/iconv])
      AC_DEFINE([ICONV_BROKEN_IGNORE],0,[Whether iconv supports IGNORE])
    ],[
      AC_MSG_RESULT(no)
      PHP_DEFINE([ICONV_BROKEN_IGNORE],1,[ext/iconv])
      AC_DEFINE([ICONV_BROKEN_IGNORE],1,[Whether iconv supports IGNORE])
    ],[
      AC_MSG_RESULT(no, cross-compiling)
      PHP_DEFINE([ICONV_BROKEN_IGNORE],0,[ext/iconv])
      AC_DEFINE([ICONV_BROKEN_IGNORE],0,[Whether iconv supports IGNORE])
    ])

    AC_MSG_CHECKING([if your cpp allows macro usage in include lines])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#define FOO <$PHP_ICONV_H_PATH>
#include FOO
    ]], [])], [
      AC_MSG_RESULT([yes])
      PHP_DEFINE([PHP_ICONV_H_PATH], [<$PHP_ICONV_H_PATH>],[ext/iconv])
      AC_DEFINE_UNQUOTED([PHP_ICONV_H_PATH], [<$PHP_ICONV_H_PATH>], [Path to iconv.h])
    ], [
      AC_MSG_RESULT([no])
    ])

    PHP_NEW_EXTENSION(iconv, iconv.c, $ext_shared,, [-I\"$PHP_ICONV_PREFIX/include\" -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
    PHP_SUBST(ICONV_SHARED_LIBADD)
    PHP_INSTALL_HEADERS([ext/iconv/])
  else
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
fi
