PHP_ARG_WITH([mhash],
  [for mhash support],
  [AS_HELP_STRING([[--with-mhash]],
    [Include mhash support])])

if test "$PHP_MHASH" != "no"; then
  AC_DEFINE(PHP_MHASH_BC, 1, [ ])
fi

if test $ac_cv_c_bigendian_php = yes; then
  EXT_HASH_SHA3_SOURCES="hash_sha3.c"
  AC_DEFINE(HAVE_SLOW_HASH3, 1, [Define if hash3 algo is available])
  AC_MSG_WARN("Use SHA3 slow implementation on bigendian")
else
  AC_CHECK_SIZEOF([long])
  AC_MSG_CHECKING([if we're at 64-bit platform])
  AS_IF([test "$ac_cv_sizeof_long" -eq 4],[
    AC_MSG_RESULT([no])
    SHA3_DIR="sha3/generic32lc"
    SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-inplace32BI.c"
  ],[
    AC_MSG_RESULT([yes])
    SHA3_DIR="sha3/generic64lc"
    SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-opt64.c"
  ])
  EXT_HASH_SHA3_SOURCES="$SHA3_OPT_SRC $SHA3_DIR/KeccakHash.c $SHA3_DIR/KeccakSponge.c hash_sha3.c"
  dnl Add -Wno-implicit-fallthrough flag as it happens on 32 bit builds
  PHP_HASH_CFLAGS="-Wno-implicit-fallthrough -I@ext_srcdir@/$SHA3_DIR -DKeccakP200_excluded -DKeccakP400_excluded -DKeccakP800_excluded -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"

  PHP_ADD_BUILD_DIR(ext/hash/$SHA3_DIR, 1)
fi

PHP_ADD_BUILD_DIR(ext/hash/murmur, 1)
PHP_HASH_CFLAGS="$PHP_HASH_CFLAGS -I@ext_srcdir@/xxhash"

EXT_HASH_SOURCES="hash.c hash_md.c hash_sha.c hash_ripemd.c hash_haval.c \
  hash_tiger.c hash_gost.c hash_snefru.c hash_whirlpool.c hash_adler32.c \
  hash_crc32.c hash_fnv.c hash_joaat.c $EXT_HASH_SHA3_SOURCES
  murmur/PMurHash.c murmur/PMurHash128.c hash_murmur.c hash_xxhash.c"
EXT_HASH_HEADERS="php_hash.h php_hash_md.h php_hash_sha.h php_hash_ripemd.h \
  php_hash_haval.h php_hash_tiger.h php_hash_gost.h php_hash_snefru.h \
  php_hash_whirlpool.h php_hash_adler32.h php_hash_crc32.h \
  php_hash_fnv.h php_hash_joaat.h php_hash_sha3.h php_hash_murmur.h \
  php_hash_xxhash.h"

PHP_NEW_EXTENSION(hash, $EXT_HASH_SOURCES, 0,,$PHP_HASH_CFLAGS)
PHP_INSTALL_HEADERS(ext/hash, $EXT_HASH_HEADERS)
