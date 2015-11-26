dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(ftp,whether to enable FTP support,
[  --enable-ftp            Enable FTP support])

PHP_ARG_WITH(openssl-dir,OpenSSL dir for FTP,
[  --with-openssl-dir[=DIR]  FTP: openssl install prefix], no, no)

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
