dnl $Id$
dnl config.m4 for extension hash

PHP_ARG_ENABLE(hash, whether to enable hash support,
[  --enable-hash           Enable hash support])

if test "$PHP_HASH" != "no"; then
  AC_DEFINE(HAVE_HASH_EXT,1,[Have HASH Extension])
  PHP_NEW_EXTENSION(hash, hash.c hash_md.c hash_sha.c hash_ripemd.c hash_haval.c hash_tiger.c hash_gost.c hash_snefru.c hash_whirlpool.c, $ext_shared)
  ifdef([PHP_INSTALL_HEADERS], [
  	PHP_INSTALL_HEADERS(ext/hash, php_hash.h php_hash_md.h php_hash_sha.h php_hash_ripemd.h php_hash_haval.h php_hash_tiger.h php_hash_gost.h php_hash_snefru.h php_hash_whirlpool.h)
  ])
fi
