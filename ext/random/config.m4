dnl
dnl Check for arc4random on BSD systems
dnl
AC_CHECK_DECLS([arc4random_buf])

dnl
dnl Check for CCRandomGenerateBytes
dnl header absent in previous macOs releases
dnl
AC_CHECK_HEADERS([CommonCrypto/CommonRandom.h], [], [],
[
	#include <sys/types.h>
	#include <Availability.h>
	#include <CommonCrypto/CommonCryptoError.h>
])

dnl
dnl Setup extension
dnl
PHP_NEW_EXTENSION(random,
      random.c \
      engine_combinedlcg.c \
      engine_mt19937.c \
      engine_pcgoneseq128xslrr64.c \
      engine_xoshiro256starstar.c \
      engine_secure.c \
      engine_user.c \
      randomizer.c,
      no,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
PHP_INSTALL_HEADERS([ext/random], [php_random.h])
