if test "$PHP_TCLINK" != "no"; then
  PHP_NEW_EXTENSION(tclink, tclink.c, $ext_shared)
  TCLINK_SHARED_LIBADD="-lcrypto -lssl"
  PHP_SUBST(TCLINK_SHARED_LIBADD)
  AC_DEFINE(HAVE_TCLINK_EXT,1,[ ])
fi
