PHP_ARG_ENABLE(dio, whether to enable dio support,
[  --enable-dio           Enable dio support])

if test "$PHP_DIO" != "no"; then
  PHP_EXTENSION(dio, $ext_shared)
fi
