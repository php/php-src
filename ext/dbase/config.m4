dnl
dnl $Id$
dnl

AC_ARG_WITH(dbase,[],[enable_dbase=$withval])

PHP_ARG_ENABLE(dbase,whether to enable dbase support,
[  --enable-dbase          Enable the bundled dbase library])

if test "$PHP_DBASE" = "yes"; then
  AC_DEFINE(DBASE,1,[ ])
  PHP_EXTENSION(dbase, $ext_shared)
fi
