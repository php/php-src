dnl $Id$
dnl config.m4 for extension phpdoc
dnl don't forget to call PHP_EXTENSION(phpdoc)

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(phpdoc, for phpdoc support,
dnl Make sure that the comment is aligned:
dnl [  --with-phpdoc             Include phpdoc support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(phpdoc, whether to enable phpdoc support,
[  --enable-phpdoc           Enable phpdoc support])

if test "$PHP_PHPDOC" != "no"; then
  dnl If you will not be testing anything external, like existence of
  dnl headers, libraries or functions in them, just uncomment the 
  dnl following line and you are ready to go.
  AC_DEFINE(HAVE_PHPDOC, 1, [ ])
  dnl Write more examples of tests here...
  PHP_EXTENSION(phpdoc, $ext_shared)
fi
