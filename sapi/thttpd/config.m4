AC_ARG_WITH(thttpd,
[  --with-thttpd=SRCDIR],[
  test -d $withval || AC_MSG_RESULT(thttpd directory does not exist ($withval))
  AC_EXPAND_PATH($withval, THTTPD)
  INSTALL_IT="\
    echo 'PHP_LIBS = -L. -lphp4 \$(PHP_LIBS) \$(EXTRA_LIBS)' > $THTTPD/php_makefile; \
    echo 'PHP_LDFLAGS = \$(NATIVE_RPATHS) \$(PHP_LDFLAGS)' >> $THTTPD/php_makefile; \
    cp $abs_srcdir/sapi/thttpd/php_thttpd.h $SAPI_STATIC $THTTPD;\
    test -f $THTTPD/php_patched || \
    (cd $THTTPD && patch < $abs_srcdir/sapi/thttpd/thttpd_patch && touch php_patched)"
  AC_MSG_CHECKING(for thttpd)
  AC_MSG_RESULT(yes - $THTTPD)
  AC_ADD_INCLUDE($THTTPD)
  PHP_BUILD_STATIC
  PHP_SAPI=thttpd
])
