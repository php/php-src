PHP_ARG_ENABLE([ftp],
  [whether to enable FTP support],
  [AS_HELP_STRING([--enable-ftp],
    [Enable FTP support])])

dnl TODO: Rename this option for master.
PHP_ARG_WITH([openssl-dir],
  [whether to explicitly enable FTP SSL support],
  [AS_HELP_STRING([[--with-openssl-dir]],
    [FTP: Whether to enable FTP SSL support without ext/openssl])],
  [no],
  [no])

if test "$PHP_FTP" = "yes"; then
  AC_DEFINE(HAVE_FTP,1,[Whether you want FTP support])
  PHP_NEW_EXTENSION(ftp, php_ftp.c ftp.c, $ext_shared)

  dnl Empty variable means 'no'
  test -z "$PHP_OPENSSL" && PHP_OPENSSL=no

  if test "$PHP_OPENSSL" != "no" || test "$PHP_OPENSSL_DIR" != "no"; then
    PHP_SETUP_OPENSSL(FTP_SHARED_LIBADD)
    PHP_SUBST(FTP_SHARED_LIBADD)
    AC_DEFINE(HAVE_FTP_SSL,1,[Whether FTP over SSL is supported])
  fi
fi
