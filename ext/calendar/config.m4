dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(calendar,whether to enable calendar conversion support,
[  --enable-calendar       Enable support for calendar conversion])

if test "$PHP_CALENDAR" = "yes"; then
  AC_DEFINE(HAVE_CALENDAR,1,[ ])
  PHP_EXTENSION(calendar, $ext_shared)
fi
