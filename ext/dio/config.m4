dnl
dnl $Id$
dnl 

PHP_ARG_ENABLE(dio, whether to enable direct I/O support,
[  --enable-dio            Enable direct I/O support])

if test "$PHP_DIO" != "no"; then
  PHP_EXTENSION(dio, $ext_shared)
fi
