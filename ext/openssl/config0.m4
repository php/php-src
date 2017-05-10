dnl
dnl $Id$
dnl

PHP_ARG_WITH(openssl, for OpenSSL support,
[  --with-openssl[=DIR]      Include OpenSSL support (requires OpenSSL >= 1.0.1)])

PHP_ARG_WITH(kerberos, for Kerberos support,
[  --with-kerberos[=DIR]     OPENSSL: Include Kerberos support], no, no)

PHP_ARG_WITH(system-ciphers, whether to use system default cipher list instead of hardcoded value,
[  --with-system-ciphers   OPENSSL: Use system default cipher list instead of hardcoded value], no, no)

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION(openssl, openssl.c xp_ssl.c, $ext_shared)
  PHP_SUBST(OPENSSL_SHARED_LIBADD)

  if test "$PHP_KERBEROS" != "no"; then
    PHP_SETUP_KERBEROS(OPENSSL_SHARED_LIBADD)
  fi

  AC_CHECK_FUNCS([RAND_egd])

  PHP_SETUP_OPENSSL(OPENSSL_SHARED_LIBADD, 
  [
    AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
  ], [
    AC_MSG_ERROR([OpenSSL check failed. Please check config.log for more information.])
  ])
  if test "$PHP_SYSTEM_CIPHERS" != "no"; then
    AC_DEFINE(USE_OPENSSL_SYSTEM_CIPHERS,1,[ Use system default cipher list instead of hardcoded value ])
  fi

  AC_MSG_CHECKING([for pthread_atfork requirement])
	AC_TRY_RUN([
	#include <pthread.h>
	#include <openssl/opensslv.h>
	
	int routine() {}
	int main(int argc, char *argv[])
	{
#if OPENSSL_VERSION_NUMBER < 0x10100001L
		pthread_atfork(NULL, NULL, routine);
		return 0;
#else
		return 1;
#endif	
	}
	], [
	  AC_MSG_RESULT([required])
	  AC_DEFINE(OPENSSL_USE_ATFORK, 1, [Whether atfork hack is required for openssl])
	  OPENSSL_EXTRA_LIBS+="-pthread"
	  PHP_SUBST(OPENSSL_EXTRA_LIBS)
	], [
	  AC_MSG_RESULT([not required])
	])
fi
