dnl
dnl $Id$
dnl

AC_ARG_WITH(webjames,
[  --with-webjames=SRCDIR  Build PHP as a WebJames module (RISC OS only)],[
  PHP_EXPAND_PATH($withval, WEBJAMES)
  INSTALL_IT="\
    echo '\\#Created by PHP configure' > $WEBJAMES/build/php; \
    echo 'PHP_LIBS = -l$abs_srcdir/$SAPI_STATIC \$(PHP_LIBS) \$(EXTRA_LIBS)' >> $WEBJAMES/build/php; \
    echo 'PHP_LDFLAGS = \$(NATIVE_RPATHS) \$(PHP_LDFLAGS)' >> $WEBJAMES/build/php; \
    echo 'PHP_CFLAGS = -DPHP \$(COMMON_FLAGS) \$(CFLAGS) \$(EXTRA_CFLAGS) -I$abs_srcdir/sapi/webjames' >> $WEBJAMES/build/php; \
    echo 'WIMPSLOT = \$\$(WIMPSLOT_PHP)' >> $WEBJAMES/build/php; \
    echo 'REMOVE_ATTRIBUTE=Action php-script.*' >> $WEBJAMES/build/php;"
  PHP_WEBJAMES="yes, using $WEBJAMES"
  PHP_ADD_INCLUDE($WEBJAMES)
  PHP_BUILD_STATIC
  PHP_SAPI=webjames
],[
  PHP_WEBJAMES="no"
])

AC_MSG_CHECKING(for webjames)
AC_MSG_RESULT($PHP_WEBJAMES)
