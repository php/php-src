dnl $Id$
dnl config.m4 for extension reflection

PHP_ARG_ENABLE(reflection, whether to enable reflection support,
[  --disable-reflection    Disable reflection support], yes, no)

if test "$PHP_REFLECTION" != "no"; then
  AC_DEFINE(HAVE_REFLECTION, 1, [Whether Reflection is enabled])
  PHP_NEW_EXTENSION(reflection, php_reflection.c, no)
fi
