dnl $Id$
dnl config.m4 for extension ftp
dnl don't forget to call PHP_EXTENSION(ftp)

PHP_ARG_ENABLE(ftp,whether to enable FTP support,
[  --enable-ftp            Enable FTP support])

if test "$PHP_FTP" = "yes"; then
  AC_DEFINE(HAVE_FTP,1,[Whether you want FTP support])
  PHP_EXTENSION(ftp, $ext_shared)
fi
