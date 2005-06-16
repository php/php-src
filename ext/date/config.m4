dnl $Id$
dnl config.m4 for date extension

PHP_DATE_CFLAGS="-I@ext_srcdir@/lib"
timelib_sources="lib/dow.c lib/parse_date.c lib/parse_tz.c 
                 lib/timelib.c lib/tm2unixtime.c lib/unixtime2tm.c"

PHP_NEW_EXTENSION(date, php_date.c $timelib_sources, no,, $PHP_DATE_CFLAGS)
PHP_ADD_BUILD_DIR([$ext_builddir/lib])
