dnl
dnl $ Id: $
dnl

PHP_ARG_ENABLE(unicode, whether to enable unicode functions,
[  --disable-unicode        Disable Unicode API support])

if test "$PHP_UNICODE" != "no"; then
  PHP_SUBST(UNICODE_SHARED_LIBADD)
  AC_DEFINE(HAVE_UNICODE, 1, [ ])
  PHP_NEW_EXTENSION(unicode, unicode.c locale.c unicode_filter.c, $ext_shared)
fi

