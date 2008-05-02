dnl $Id$
dnl config.m4 for extension reflection

PHP_NEW_EXTENSION(reflection, php_reflection.c, no)
AC_DEFINE(HAVE_REFLECTION, 1)
