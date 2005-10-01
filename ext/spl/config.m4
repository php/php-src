dnl $Id$
dnl config.m4 for extension SPL

PHP_ARG_ENABLE(spl, enable SPL suppport,
[  --disable-spl           Disable Standard PHP Library], yes)

if test "$PHP_SPL" != "no"; then
  if test "$ext_shared" = "yes"; then
    AC_MSG_ERROR(Cannot build SPL as a shared module)
  fi
  AC_DEFINE(HAVE_SPL, 1, [Whether you want SPL (Standard PHP Library) support]) 
  PHP_NEW_EXTENSION(spl, php_spl.c spl_functions.c spl_engine.c spl_iterators.c spl_array.c spl_directory.c spl_sxe.c spl_exceptions.c spl_observer.c, $ext_shared)
fi
