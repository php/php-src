dnl
dnl $Id$
dnl

PHP_ARG_WITH(thttpd,,
[  --with-thttpd=SRCDIR    Build PHP as thttpd module], no, no)

AC_MSG_CHECKING([for thttpd])

if test "$PHP_THTTPD" != "no"; then
  if test ! -d $PHP_THTTPD; then
    AC_MSG_RESULT(thttpd directory does not exist ($PHP_THTTPD))
  fi

  PHP_EXPAND_PATH($PHP_THTTPD, THTTPD)
  
  if grep thttpd.2.21b $PHP_THTTPD/version.h >/dev/null; then
    patch="test -f $THTTPD/php_patched || \
    (cd $THTTPD && patch -p1 < $abs_srcdir/sapi/thttpd/thttpd_patch && touch php_patched)"

  elif grep Premium $PHP_THTTPD/version.h >/dev/null; then
    patch=
  else
    AC_MSG_ERROR([This version only supports thttpd-2.21b and Premium thttpd])
  fi
  PHP_TARGET_RDYNAMIC
  INSTALL_IT="\
    echo 'PHP_LIBS = -L. -lphp7 \$(PHP_LIBS) \$(EXTRA_LIBS)' > $THTTPD/php_makefile; \
    echo 'PHP_LDFLAGS = \$(NATIVE_RPATHS) \$(PHP_LDFLAGS)' >> $THTTPD/php_makefile; \
    echo 'PHP_CFLAGS = \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(CPPFLAGS) \$(EXTRA_CFLAGS)' >> $THTTPD/php_makefile; \
    rm -f $THTTPD/php_thttpd.c $THTTPD/php_thttpd.h $THTTPD/libphp7.a; \
    \$(LN_S) $abs_srcdir/sapi/thttpd/thttpd.c $THTTPD/php_thttpd.c; \
    \$(LN_S) $abs_srcdir/sapi/thttpd/php_thttpd.h $abs_builddir/$SAPI_STATIC $THTTPD/;\
    $patch"
  PHP_THTTPD="yes, using $THTTPD"
  PHP_ADD_INCLUDE($THTTPD)
  PHP_SELECT_SAPI(thttpd, static)
fi
AC_MSG_RESULT($PHP_THTTPD)
