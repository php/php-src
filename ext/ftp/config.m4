PHP_ARG_ENABLE([ftp],
  [whether to enable FTP support],
  [AS_HELP_STRING([--enable-ftp],
    [Enable FTP support])])

PHP_ARG_WITH([ftp-ssl],
  [whether to explicitly enable FTP SSL support],
  [AS_HELP_STRING([--with-ftp-ssl],
    [Explicitly enable SSL support in ext/ftp when not building with
    ext/openssl. If ext/openssl is enabled at the configure step, SSL is enabled
    implicitly.])],
  [no],
  [no])

if test "$PHP_FTP" = "yes"; then
  AC_DEFINE(HAVE_FTP,1,[Whether you want FTP support])
  PHP_NEW_EXTENSION([ftp], [php_ftp.c ftp.c], [$ext_shared])

  dnl Empty variable means 'no' (for phpize builds).
  test -z "$PHP_OPENSSL" && PHP_OPENSSL=no

  if test "$PHP_OPENSSL" != "no" || test "$PHP_FTP_SSL" != "no"; then
    PHP_SETUP_OPENSSL([FTP_SHARED_LIBADD])
    PHP_SUBST([FTP_SHARED_LIBADD])
    AC_DEFINE(HAVE_FTP_SSL,1,[Whether FTP over SSL is supported])
  fi
fi
