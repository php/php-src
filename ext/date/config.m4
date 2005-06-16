dnl $Id$
dnl config.m4 for date extension

PHP_NEW_EXTENSION(date, php_date.c lib/timelib.c lib/dow.c lib/parse_date.c lib/parse_tz.c lib/tm2unixtime.c lib/unixtime2tm.c)
PHP_ADD_BUILD_DIR([$ext_builddir/lib])
