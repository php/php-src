dnl $Id$
dnl config.m4 for extension ctype
dnl don't forget to call PHP_EXTENSION(ctype)

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(ctype, for ctype support,
dnl Make sure that the comment is aligned:
dnl [  --with-ctype             Include ctype support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(ctype, whether to enable ctype support,[  --enable-ctype           Enable ctype support])

if test "$PHP_CTYPE" != "no"; then
  dnl If you will not be testing anything external, like existence of
  dnl headers, libraries or functions in them, just uncomment the 
  dnl following line and you are ready to go.
  AC_DEFINE(HAVE_CTYPE, 1, [ ])
  dnl Write more examples of tests here...
  PHP_EXTENSION(ctype, $ext_shared)
fi
