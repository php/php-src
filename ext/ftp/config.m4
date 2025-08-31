PHP_ARG_ENABLE([ftp],
  [whether to enable FTP support],
  [AS_HELP_STRING([--enable-ftp],
    [Enable FTP support])])

dnl Empty variable means 'no' (for phpize builds).
AS_VAR_IF([PHP_OPENSSL],, [PHP_OPENSSL=no])

PHP_ARG_WITH([ftp-ssl],
  [whether to enable FTP over SSL support],
  [AS_HELP_STRING([--with-ftp-ssl],
    [Explicitly enable FTP over SSL support when building without openssl
    extension or when using phpize. If the openssl extension is enabled at the
    configure step (--with-openssl), FTP-SSL is enabled implicitly regardless of
    this option.])],
  [$PHP_OPENSSL],
  [no])

if test "$PHP_FTP" = "yes"; then
  AC_DEFINE([HAVE_FTP], [1],
    [Define to 1 if the PHP extension 'ftp' is available.])
  PHP_NEW_EXTENSION([ftp], [php_ftp.c ftp.c], [$ext_shared])

  AS_VAR_IF([PHP_FTP_SSL], [no],, [
    PHP_SETUP_OPENSSL([FTP_SHARED_LIBADD])
    PHP_SUBST([FTP_SHARED_LIBADD])
    AC_DEFINE([HAVE_FTP_SSL], [1], [Define to 1 if FTP over SSL is enabled.])
  ])
fi
