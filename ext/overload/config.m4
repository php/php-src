dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(overload,whether to enable user-space object overloading support,
[  --disable-overload      Disable user-space object overloading support.], yes)

if test "$PHP_OVERLOAD" != "no"; then
  if test -f $php_abs_top_srcdir/Zend/zend_objects.h; then
    AC_MSG_CHECKING(for overload)
    AC_MSG_RESULT(not needed with ZendEngine 2)
  else
    AC_DEFINE(HAVE_OVERLOAD, 1, [ ])
    PHP_NEW_EXTENSION(overload, overload.c, $ext_shared)
  fi
fi
