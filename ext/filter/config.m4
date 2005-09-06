dnl $Id$
dnl config.m4 for input filtering extension

PHP_ARG_ENABLE(filter, whether to enable input filter support,
[  --enable-filter           Enable input filter support])

if test "$PHP_FILTER" != "no"; then
  PHP_SUBST(FILTER_SHARED_LIBADD)
  PHP_NEW_EXTENSION(filter, filter.c sanitizing_filters.c logical_filters.c callback_filter.c, $ext_shared)
  CPPFLAGS="$CPPFLAGS -Wall"
fi
