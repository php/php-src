dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(overload,whether to enable user-space object overloading support,
[  --enable-overload       Enable user-space object overloading support.])

if test "$PHP_OVERLOAD" != "no"; then
	AC_DEFINE(HAVE_OVERLOAD, 1, [ ])
	PHP_EXTENSION(overload, $ext_shared)
fi
