dnl $Id$
dnl config.m4 for extension hash

PHP_ARG_WITH(mhash, for mhash support,
[  --with-mhash[=DIR]      Include mhash support])

PHP_ARG_ENABLE(hash, whether to enable hash support,
[  --disable-hash          Disable hash support], yes)

if test "$PHP_MHASH" != "no"; then
  if test "$PHP_HASH" = "no"; then
    PHP_HASH="yes"
  fi

  AC_DEFINE(PHP_MHASH_BC, 1, [ ])
fi

if test "$PHP_HASH" != "no"; then
  AC_DEFINE(HAVE_HASH_EXT,1,[Have HASH Extension])

  PHP_C_BIGENDIAN

  AC_CHECK_SIZEOF(short, 2)
  AC_CHECK_SIZEOF(int, 4)
  AC_CHECK_SIZEOF(long, 4)
  AC_CHECK_SIZEOF(long long, 8)
  
  EXT_HASH_SOURCES="hash.c hash_md.c hash_sha.c hash_ripemd.c hash_haval.c \
    hash_tiger.c hash_gost.c hash_snefru.c hash_whirlpool.c hash_adler32.c \
    hash_crc32.c hash_fnv.c hash_joaat.c"
  EXT_HASH_HEADERS="php_hash.h php_hash_md.h php_hash_sha.h php_hash_ripemd.h \
    php_hash_haval.h php_hash_tiger.h php_hash_gost.h php_hash_snefru.h \
    php_hash_whirlpool.h php_hash_adler32.h php_hash_crc32.h \
    php_hash_fnv.h php_hash_joaat.h php_hash_types.h"
  
  PHP_NEW_EXTENSION(hash, $EXT_HASH_SOURCES, $ext_shared)
  ifdef([PHP_INSTALL_HEADERS], [
  	PHP_INSTALL_HEADERS(ext/hash, $EXT_HASH_HEADERS)
  ])
fi
