
PHP_ARG_ENABLE(dbx,whether to enable dbx support,
[  --enable-dbx            Enable dbx])

if test "$PHP_DBX" != "no"; then
  PHP_EXTENSION(dbx, $ext_shared)
fi
