PHP_ARG_WITH([mhash],
  [for mhash support],
  [AS_HELP_STRING([--with-mhash],
    [Include mhash support])])

AS_VAR_IF([PHP_MHASH], [no],, [
  AC_MSG_WARN([The --with-mhash option and mhash* functions are deprecated as of PHP 8.1.0])
  AC_DEFINE([PHP_MHASH_BC], [1], [Define to 1 if mhash support is enabled.])
])

AS_VAR_IF([ac_cv_c_bigendian_php], [yes], [
  EXT_HASH_SHA3_SOURCES=
  AC_DEFINE([HAVE_SLOW_HASH3], [1],
    [Define to 1 if the PHP hash extension uses the slow SHA3 algorithm.])
  AC_MSG_WARN([Using SHA3 slow implementation on bigendian])
  SHA3_DIR=
], [
  AC_CHECK_SIZEOF([long])
  AC_MSG_CHECKING([if we're at 64-bit platform])
  AS_IF([test "$ac_cv_sizeof_long" -eq 4],[
    AC_MSG_RESULT([no])
    SHA3_DIR="sha3/generic32lc"
    SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-inplace32BI.c"
    dnl Add -Wno-implicit-fallthrough flag as it happens on 32 bit builds
    AX_CHECK_COMPILE_FLAG([-Wno-implicit-fallthrough],
      [PHP_HASH_CFLAGS="$PHP_HASH_CFLAGS -Wno-implicit-fallthrough"],,
      [-Werror])
  ],[
    AC_MSG_RESULT([yes])
    SHA3_DIR="sha3/generic64lc"
    SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-opt64.c"
  ])
  EXT_HASH_SHA3_SOURCES="$SHA3_OPT_SRC $SHA3_DIR/KeccakHash.c $SHA3_DIR/KeccakSponge.c"
  PHP_HASH_CFLAGS="$PHP_HASH_CFLAGS -I@ext_srcdir@/$SHA3_DIR -DKeccakP200_excluded -DKeccakP400_excluded -DKeccakP800_excluded -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
])

PHP_NEW_EXTENSION([hash], m4_normalize([
    $EXT_HASH_SHA3_SOURCES
    hash_adler32.c
    hash_crc32.c
    hash_fnv.c
    hash_gost.c
    hash_haval.c
    hash_joaat.c
    hash_md.c
    hash_murmur.c
    hash_ripemd.c
    hash_sha_ni.c
    hash_sha_sse2.c
    hash_sha.c
    hash_sha3.c
    hash_snefru.c
    hash_tiger.c
    hash_whirlpool.c
    hash_xxhash.c
    hash.c
    murmur/PMurHash.c
    murmur/PMurHash128.c
  ]),
  [no],,
  [$PHP_HASH_CFLAGS])
PHP_ADD_BUILD_DIR([$ext_builddir/murmur])
AS_VAR_IF([SHA3_DIR],,, [PHP_ADD_BUILD_DIR([$ext_builddir/$SHA3_DIR])])
PHP_INSTALL_HEADERS([ext/hash], m4_normalize([
  php_hash_adler32.h
  php_hash_crc32.h
  php_hash_fnv.h
  php_hash_gost.h
  php_hash_haval.h
  php_hash_joaat.h
  php_hash_md.h
  php_hash_murmur.h
  php_hash_ripemd.h
  php_hash_sha.h
  php_hash_sha3.h
  php_hash_snefru.h
  php_hash_tiger.h
  php_hash_whirlpool.h
  php_hash_xxhash.h
  php_hash.h
  xxhash/xxhash.h
]))
