dnl $Id$
dnl config.m4 for extension SPL

PHP_ARG_ENABLE(spl, enable SPL suppport,
[  --enable-spl            Enable Standard PHP Library])

dnl first enable/disable all hooks

PHP_ARG_ENABLE(spl, enable all hooks,
[  --enable-spl-hook-all       SPL: Enable all hooks])

dnl now all single enable/disable for hooks

PHP_ARG_ENABLE(spl, enable hook on foreach,
[  --disable-spl-foreach       SPL: Disable hook on forach], yes)

PHP_ARG_ENABLE(spl, enable hook on array read,
[  --enable-spl-array-read     SPL: Enable hook on array read])

PHP_ARG_ENABLE(spl, enable hook on array write,
[  --enable-spl-array-write    SPL: Enable hook on array write (+read)])

dnl last do checks on hooks

if test "$PHP_SPL" != "no"; then
  if test "$PHP_SPL_HOOK_ALL" != "no" -o "$PHP_SPL_FOREACH" != "no"; then
    AC_DEFINE(SPL_FOREACH, 1, [Activate opcode hook on foreach])
    PHP_SPL="yes"
  fi
  if test "$PHP_SPL_HOOK_ALL" != "no" -o "$PHP_SPL_ARRAY_READ" != "no" -o "$PHP_SPL_ARRAY_WRITE" != "no"; then
    AC_DEFINE(SPL_ARRAY_READ, 1, [Activate opcode hook on array read])
    PHP_SPL="yes"
  fi
  if test "$PHP_SPL_HOOK_ALL" != "no" -o "$PHP_SPL_ARRAY_WRITE" != "no"; then
    AC_DEFINE(SPL_ARRAY_WRITE, 1, [Activate opcode hook on array write])
    PHP_SPL="yes"
  fi
fi

if test "$PHP_SPL" != "no"; then
    AC_DEFINE(HAVE_SPL, 1, [Whether you want SPL (Standard Php Library) support]) 
    PHP_NEW_EXTENSION(spl, php_spl.c spl_functions.c spl_engine.c spl_foreach.c spl_array.c, $ext_shared)
fi
