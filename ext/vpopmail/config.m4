dnl $Id$
dnl config.m4 for extension vpopmail

PHP_ARG_WITH(vpopmail, whether to include vpopmail support,
[  --with-vpopmail[=DIR]      Include vpopmail support])

if test "$PHP_VPOPMAIL" != "no"; then

  for i in /home/vpopmail /home/popmail /var/qmail/vpopmail /var/qmail/popmail $PHP_VPOPMAIL; do
    if test -r $i/vpopmail.h; then
      VPOPMAIL_INC_DIR=$i
    elif test -r $i/include/vpopmail.h; then
      VPOPMAIL_INC_DIR=$i/include
    fi

    if test -r $i/libvpopmail.a; then
      VPOPMAIL_LIB_DIR=$i
    elif test -r $i/lib/libvpopmail.a; then
      VPOPMAIL_LIB_DIR=$i/lib
    fi
  done

  if test -z "$VPOPMAIL_INC_DIR"; then
    AC_MSG_ERROR(Could not find vpopmail.h. Please make sure you have
                 vpopmail installed. Use
                 ./configure --with-vpopmail=<vpopmail-home-dir> if necessary)
  fi

  if test -z "$VPOPMAIL_LIB_DIR"; then
    AC_MSG_ERROR(Could not find libvpopmail.a. Please make sure you have
                 vpopmail installed. Use
                 ./configure --with-vpopmail=<vpopmail-home-dir> if necessary)
  fi

  AC_MSG_RESULT(found in $VPOPMAIL_LIB_DIR)

  AC_ADD_INCLUDE($VPOPMAIL_INC_DIR)

  PHP_SUBST(VPOPMAIL_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(vpopmail, $VPOPMAIL_LIB_DIR, VPOPMAIL_SHARED_LIBADD)

  AC_DEFINE(HAVE_VPOPMAIL, 1, [ ])

  PHP_EXTENSION(vpopmail, $ext_shared)
fi
