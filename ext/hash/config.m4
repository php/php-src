dnl config.m4 for extension hash

PHP_ARG_WITH(mhash, for mhash support,
[  --with-mhash[=DIR]        Include mhash support])

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

  if test $ac_cv_c_bigendian_php = yes; then
    EXT_HASH_SHA3_SOURCES="hash_sha3.c"
    AC_DEFINE(HAVE_SLOW_HASH3, 1, [Define is hash3 algo is available])
    AC_MSG_WARN("Use SHA3 slow implementation on bigendian")
  else
    PHP_CHECK_64BIT([
      SHA3_DIR="sha3/generic32lc"
      SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-inplace32BI.c"
    ],[
      SHA3_DIR="sha3/generic64lc"
      SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-opt64.c"
    ])
    EXT_HASH_SHA3_SOURCES="$SHA3_OPT_SRC $SHA3_DIR/KeccakHash.c $SHA3_DIR/KeccakSponge.c hash_sha3.c"
    PHP_HASH_CFLAGS="-I@ext_srcdir@/$SHA3_DIR -DKeccakP200_excluded -DKeccakP400_excluded -DKeccakP800_excluded"

    PHP_ADD_BUILD_DIR(ext/hash/$SHA3_DIR, 1)
  fi

  EXT_HASH_SOURCES="hash.c hash_md.c hash_sha.c hash_ripemd.c hash_haval.c \
    hash_tiger.c hash_gost.c hash_snefru.c hash_whirlpool.c hash_adler32.c \
    hash_crc32.c hash_fnv.c hash_joaat.c $EXT_HASH_SHA3_SOURCES"
  EXT_HASH_HEADERS="php_hash.h php_hash_md.h php_hash_sha.h php_hash_ripemd.h \
    php_hash_haval.h php_hash_tiger.h php_hash_gost.h php_hash_snefru.h \
    php_hash_whirlpool.h php_hash_adler32.h php_hash_crc32.h \
    php_hash_fnv.h php_hash_joaat.h php_hash_sha3.h"

  PHP_NEW_EXTENSION(hash, $EXT_HASH_SOURCES, $ext_shared,,$PHP_HASH_CFLAGS)
  ifdef([PHP_INSTALL_HEADERS], [
  	PHP_INSTALL_HEADERS(ext/hash, $EXT_HASH_HEADERS)
  ])
fi
