PHP_ARG_ENABLE(embed,whether to enable the embedded SAPI module,
[  --enable-embed])

PHP_ARG_ENABLE(embed-shared,whether to build it as shared library,
[  --enable-embed-shared])

if test "$PHP_EMBED" = "yes"; then
  if test "$PHP_EMBED_SHARED" = "yes"; then
    ac_type=shared
  else
    ac_type=static
  fi
  PHP_SELECT_SAPI(embed, $ac_type, php_embed.c)
fi
