dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(ctype, whether to enable ctype functions,
[  --disable-ctype         Disable ctype functions], yes)

if test "$PHP_CTYPE" != "no"; then
  AC_DEFINE(HAVE_CTYPE, 1, [ ])
  PHP_EXTENSION(ctype, $ext_shared)
fi
