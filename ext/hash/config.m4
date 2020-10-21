PHP_ARG_WITH([mhash],
  [for mhash support],
  [AS_HELP_STRING([[--with-mhash]],
    [Include mhash support])])

if test "$PHP_MHASH" != "no"; then
  if test "$PHP_HASH" = "no"; then
    PHP_HASH="yes"
  fi

  AC_DEFINE(PHP_MHASH_BC, 1, [ ])
fi

EXT_HASH_BLAKE3_SOURCES="hash_blake3.c blake3/blake3.c blake3/blake3_dispatch.c blake3/blake3_portable.c"

if test $ac_cv_c_bigendian_php = yes; then
  dnl todo: check if blake3/blake3_neon.c is applicable
  EXT_HASH_SHA3_SOURCES="hash_sha3.c"
  AC_DEFINE(HAVE_SLOW_HASH3, 1, [Define is hash3 algo is available])
  AC_MSG_WARN("Use SHA3 slow implementation on bigendian")
else
  AC_CHECK_SIZEOF([long])
  AC_MSG_CHECKING([if we're at 64-bit platform])
  AS_IF([test "$ac_cv_sizeof_long" -eq 4],[
    AC_MSG_RESULT([no])
    if test $ac_cv_target_x86 = yes; then 
      dnl i think there are some 32bit x86 cpus with SSE2 but.. cba
      PHP_HASH_CFLAGS="$PHP_HASH_CFLAGS -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
    fi
    SHA3_DIR="sha3/generic32lc"
    SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-inplace32BI.c"
  ],[
    AC_MSG_RESULT([yes])
    SHA3_DIR="sha3/generic64lc"
    SHA3_OPT_SRC="$SHA3_DIR/KeccakP-1600-opt64.c"
    if test $ac_cv_target_x86 = yes; then 
      if test $ac_cv_target_windows = yes; then
        dnl x86_64 windows gnuc
        EXT_HASH_BLAKE3_SOURCES="$EXT_HASH_BLAKE3_SOURCES blake3/blake3_avx512_x86-64_windows_gnu.S blake3/blake3_avx2_x86-64_windows_gnu.S blake3/blake3_sse41_x86-64_windows_gnu.S blake3/blake3_sse2_x86-64_windows_gnu.S"
      else
        if test $ac_cv_target_unix = yes; then 
          dnl x86_64 unix gnuc
          EXT_HASH_BLAKE3_SOURCES="$EXT_HASH_BLAKE3_SOURCES blake3/blake3_avx512_x86-64_unix.S blake3/blake3_avx2_x86-64_unix.S blake3/blake3_sse41_x86-64_unix.S blake3/blake3_sse2_x86-64_unix.S"
        else
          dnl blake still has C instrictics versions (eg ext/hash/blake3/blake3_avx512.c  ) which COULD be used in this situation, 
          dnl we're targeting x86_64 but neither windows nor unix-ish.. 
          dnl i have nothing to test this on, so i'm just disabling it for now, feel free to fix it.
          PHP_HASH_CFLAGS="$PHP_HASH_CFLAGS -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
        fi
      fi
    fi
  ])
  EXT_HASH_SHA3_SOURCES="$SHA3_OPT_SRC $SHA3_DIR/KeccakHash.c $SHA3_DIR/KeccakSponge.c hash_sha3.c"
  PHP_HASH_CFLAGS="-I@ext_srcdir@/$SHA3_DIR -DKeccakP200_excluded -DKeccakP400_excluded -DKeccakP800_excluded -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"

  PHP_ADD_BUILD_DIR(ext/hash/$SHA3_DIR, 1)
fi

EXT_HASH_SOURCES="hash.c hash_md.c hash_sha.c hash_ripemd.c hash_haval.c \
  hash_tiger.c hash_gost.c hash_snefru.c hash_whirlpool.c hash_adler32.c \
  hash_crc32.c hash_fnv.c hash_joaat.c $EXT_HASH_SHA3_SOURCES \
  $EXT_HASH_BLAKE3_SOURCES"
EXT_HASH_HEADERS="php_hash.h php_hash_md.h php_hash_sha.h php_hash_ripemd.h \
  php_hash_haval.h php_hash_tiger.h php_hash_gost.h php_hash_snefru.h \
  php_hash_whirlpool.h php_hash_adler32.h php_hash_crc32.h \
  php_hash_fnv.h php_hash_joaat.h php_hash_sha3.h php_hash_blake3.h"

PHP_NEW_EXTENSION(hash, $EXT_HASH_SOURCES, 0,,$PHP_HASH_CFLAGS)
PHP_INSTALL_HEADERS(ext/hash, $EXT_HASH_HEADERS)
