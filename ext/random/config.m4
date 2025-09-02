AC_CHECK_DECL([arc4random_buf],
  [AC_DEFINE([HAVE_ARC4RANDOM_BUF], [1],
    [Define to 1 if you have the 'arc4random_buf' function.])])

dnl
dnl Check for CCRandomGenerateBytes
dnl header absent in previous macOs releases
dnl
AC_CHECK_HEADERS([CommonCrypto/CommonRandom.h],,, [dnl
  #include <sys/types.h>
  #include <Availability.h>
  #include <CommonCrypto/CommonCryptoError.h>
])

dnl
dnl Mostly for non Linux systems
dnl
AC_CHECK_FUNCS([getrandom])

dnl
dnl Setup extension
dnl
PHP_NEW_EXTENSION([random], m4_normalize([
    csprng.c
    engine_mt19937.c
    engine_pcgoneseq128xslrr64.c
    engine_secure.c
    engine_user.c
    engine_xoshiro256starstar.c
    gammasection.c
    random.c
    randomizer.c
    zend_utils.c
  ]),
  [no],,
  [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
PHP_INSTALL_HEADERS([ext/random], m4_normalize([
  php_random_csprng.h
  php_random_uint128.h
  php_random.h
]))
