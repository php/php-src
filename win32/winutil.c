/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   *         Pierre Joye <pierre@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include <wincrypt.h>

PHPAPI char *php_win32_error_to_msg(HRESULT error)
{
	char *buf = NULL;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |	FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	(LPTSTR)&buf, 0, NULL
	);

	return (buf ? (char *) buf : "");
}

int php_win32_check_trailing_space(const char * path, const int path_len) {
	if (path_len < 1) {
		return 1;
	}
	if (path) {
		if (path[0] == ' ' || path[path_len - 1] == ' ') {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 0;
	}
}

HCRYPTPROV   hCryptProv;
unsigned int has_crypto_ctx = 0;

#ifdef ZTS
MUTEX_T php_lock_win32_cryptoctx;
void php_win32_init_rng_lock()
{
	php_lock_win32_cryptoctx = tsrm_mutex_alloc();
}

void php_win32_free_rng_lock()
{
	tsrm_mutex_lock(php_lock_win32_cryptoctx);
	if (has_crypto_ctx == 1) {
		CryptReleaseContext(hCryptProv, 0);
		has_crypto_ctx = 0;
	}
	tsrm_mutex_unlock(php_lock_win32_cryptoctx);
	tsrm_mutex_free(php_lock_win32_cryptoctx);

}
#else
#define php_win32_init_rng_lock();
#define php_win32_free_rng_lock();
#endif



PHPAPI int php_win32_get_random_bytes(unsigned char *buf, size_t size) {  /* {{{ */

	BOOL ret;

#ifdef ZTS
	tsrm_mutex_lock(php_lock_win32_cryptoctx);
#endif

	if (has_crypto_ctx == 0) {
		/* CRYPT_VERIFYCONTEXT > only hashing&co-like use, no need to acces prv keys */
		if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET|CRYPT_VERIFYCONTEXT )) {
			/* Could mean that the key container does not exist, let try
			   again by asking for a new one. If it fails here, it surely means that the user running
               this process does not have the permission(s) to use this container.
             */
			if (GetLastError() == NTE_BAD_KEYSET) {
				if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET | CRYPT_VERIFYCONTEXT )) {
					has_crypto_ctx = 1;
				} else {
					has_crypto_ctx = 0;
				}
			}
		} else {
			has_crypto_ctx = 1;
		}
	}

#ifdef ZTS
	tsrm_mutex_unlock(php_lock_win32_cryptoctx);
#endif

	if (has_crypto_ctx == 0) {
		return FAILURE;
	}

	/* XXX should go in the loop if size exceeds UINT_MAX */
	ret = CryptGenRandom(hCryptProv, (DWORD)size, buf);

	if (ret) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
