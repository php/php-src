dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(yp,whether to include YP support,
[  --enable-yp             Include YP support.])

if test "$PHP_YP" != "no"; then
  AC_DEFINE(HAVE_YP,1,[ ])
  PHP_EXTENSION(yp, $ext_shared)
  case $host_alias in
  *solaris*)
    AC_DEFINE(SOLARIS_YP,1,[ ]) ;;
  esac
fi
