dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(overload,whether to enable user-space object overloading support,
[  --disable-overload      Disable user-space object overloading support.], yes)

if test "$PHP_OVERLOAD" != "no"; then
	AC_DEFINE(HAVE_OVERLOAD, 1, [ ])
	PHP_EXTENSION(overload, $ext_shared)
fi
