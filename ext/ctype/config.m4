dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(ctype, whether to enable ctype support,
[  --enable-ctype          Enable ctype support])

if test "$PHP_CTYPE" != "no"; then
  dnl If you will not be testing anything external, like existence of
  dnl headers, libraries or functions in them, just uncomment the 
  dnl following line and you are ready to go.
  AC_DEFINE(HAVE_CTYPE, 1, [ ])
  dnl Write more examples of tests here...
  PHP_EXTENSION(ctype, $ext_shared)
fi
