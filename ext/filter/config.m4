dnl $Id$
dnl config.m4 for input filtering extension

PHP_ARG_ENABLE(filter, whether to enable input filter support,
[  --disable-filter           Disable input filter support], yes)

if test "$PHP_FILTER" != "no"; then
  PHP_NEW_EXTENSION(filter, filter.c sanitizing_filters.c logical_filters.c callback_filter.c, $ext_shared)
  PHP_SUBST(FILTER_SHARED_LIBADD)
fi
