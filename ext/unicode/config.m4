dnl
dnl $Id$
dnl

PHP_SUBST(UNICODE_SHARED_LIBADD)
AC_DEFINE(HAVE_UNICODE, 1, [ ])
PHP_NEW_EXTENSION(unicode, unicode.c locale.c unicode_filter.c unicode_iterators.c collator.c, $ext_shared)
