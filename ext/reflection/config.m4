dnl $Id$
dnl config.m4 for extension bitset

PHP_ARG_ENABLE(reflection, whether to enable reflection support,
[  --disable-reflection      Disable reflection support])

if test "$PHP_REFLECTION" != "no"; then
  if test "$ext_shared" = "yes"; then
    AC_MSG_ERROR(Cannot build reflectino as a shared module)
  fi
  PHP_NEW_EXTENSION(reflection, php_reflection.c)
fi
