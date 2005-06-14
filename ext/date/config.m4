dnl $Id$
dnl config.m4 for date extension

AC_DEFINE(HAVE_DATE, 1, [Whether you want date/time support]) 
AC_DEFINE(TIMEZONE_DB_PATH, $libdir/timezonedb, [The path to the timezone database])
PHP_NEW_EXTENSION(date, php_date.c lib/datetime.c lib/dow.c lib/parse_date.c lib/parse_tz.c lib/tm2unixtime.c lib/unixtime2tm.c, no)
