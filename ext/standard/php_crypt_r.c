/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Pierre Alain Joye  <pajoye@php.net                          |
   +----------------------------------------------------------------------+
 */

/*
 * License for the Unix md5crypt implementation (md5_crypt):
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 * from FreeBSD: crypt.c,v 1.5 1996/10/14 08:34:02 phk Exp
 * via OpenBSD: md5crypt.c,v 1.9 1997/07/23 20:58:27 kstailey Exp
 * via NetBSD: md5crypt.c,v 1.4.2.1 2002/01/22 19:31:59 he Exp
 *
 */

#include "php.h"

#include <string.h>

#ifdef PHP_WIN32
# include <windows.h>
# include <Wincrypt.h>
#endif

#include "php_crypt_r.h"
#include "crypt_freesec.h"
#include "ext/standard/md5.h"

#ifdef ZTS
MUTEX_T php_crypt_extended_init_lock;
#endif

void php_init_crypt_r(void)
{
#ifdef ZTS
	php_crypt_extended_init_lock = tsrm_mutex_alloc();
#endif
}

void php_shutdown_crypt_r(void)
{
#ifdef ZTS
	tsrm_mutex_free(php_crypt_extended_init_lock);
#endif
}

void _crypt_extended_init_r(void)
{
	static int initialized = 0;

#ifdef ZTS
	tsrm_mutex_lock(php_crypt_extended_init_lock);
#endif

	if (!initialized) {
		initialized = 1;
		_crypt_extended_init();
	}

#ifdef ZTS
	tsrm_mutex_unlock(php_crypt_extended_init_lock);
#endif
}

/* MD5 crypt implementation using the windows CryptoApi */
#define MD5_MAGIC "$1$"
#define MD5_MAGIC_LEN 3

static const unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/* Convert a 16/32 bit integer to Base64 string representation */
static void to64(char *s, int32_t v, int n)
{
	while (--n >= 0) {
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

/*
 * MD5 password encryption.
 */
char * php_md5_crypt_r(const char *pw, const char *salt, char *out)
{
	ZEND_TLS char passwd[MD5_HASH_MAX_LEN], *p;
	const char *sp, *ep;
	unsigned char final[16];
	unsigned int i, sl, pwl;
	PHP_MD5_CTX	ctx, ctx1;
	uint32_t l;
	int pl;

	pwl = strlen(pw);

	/* Refine the salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if (strncmp(sp, MD5_MAGIC, MD5_MAGIC_LEN) == 0)
		sp += MD5_MAGIC_LEN;

	/* It stops at the first '$', max 8 chars */
	for (ep = sp; *ep != '\0' && *ep != '$' && ep < (sp + 8); ep++);

	/* get the length of the true salt */
	sl = ep - sp;

	PHP_MD5Init(&ctx);

	/* The password first, since that is what is most unknown */
	PHP_MD5Update(&ctx, (const unsigned char *)pw, pwl);

	/* Then our magic string */
	PHP_MD5Update(&ctx, (const unsigned char *)MD5_MAGIC, MD5_MAGIC_LEN);

	/* Then the raw salt */
	PHP_MD5Update(&ctx, (const unsigned char *)sp, sl);

	/* Then just as many characters of the MD5(pw,salt,pw) */
	PHP_MD5Init(&ctx1);
	PHP_MD5Update(&ctx1, (const unsigned char *)pw, pwl);
	PHP_MD5Update(&ctx1, (const unsigned char *)sp, sl);
	PHP_MD5Update(&ctx1, (const unsigned char *)pw, pwl);
	PHP_MD5Final(final, &ctx1);

	for (pl = pwl; pl > 0; pl -= 16)
		PHP_MD5Update(&ctx, final, (unsigned int)(pl > 16 ? 16 : pl));

	/* Don't leave anything around in vm they could use. */
	ZEND_SECURE_ZERO(final, sizeof(final));

	/* Then something really weird... */
	for (i = pwl; i != 0; i >>= 1)
		if ((i & 1) != 0)
		    PHP_MD5Update(&ctx, final, 1);
		else
		    PHP_MD5Update(&ctx, (const unsigned char *)pw, 1);

	/* Now make the output string */
	memcpy(passwd, MD5_MAGIC, MD5_MAGIC_LEN);
	strlcpy(passwd + MD5_MAGIC_LEN, sp, sl + 1);
	strcat(passwd, "$");

	PHP_MD5Final(final, &ctx);

	/*
	 * And now, just to make sure things don't run too fast. On a 60 MHz
	 * Pentium this takes 34 msec, so you would need 30 seconds to build
	 * a 1000 entry dictionary...
	 */
	for (i = 0; i < 1000; i++) {
		PHP_MD5Init(&ctx1);

		if ((i & 1) != 0)
			PHP_MD5Update(&ctx1, (const unsigned char *)pw, pwl);
		else
			PHP_MD5Update(&ctx1, final, 16);

		if ((i % 3) != 0)
			PHP_MD5Update(&ctx1, (const unsigned char *)sp, sl);

		if ((i % 7) != 0)
			PHP_MD5Update(&ctx1, (const unsigned char *)pw, pwl);

		if ((i & 1) != 0)
			PHP_MD5Update(&ctx1, final, 16);
		else
			PHP_MD5Update(&ctx1, (const unsigned char *)pw, pwl);

		PHP_MD5Final(final, &ctx1);
	}

	p = passwd + sl + MD5_MAGIC_LEN + 1;

	l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(p,l,4); p += 4;
	l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(p,l,4); p += 4;
	l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(p,l,4); p += 4;
	l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(p,l,4); p += 4;
	l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(p,l,4); p += 4;
	l =		       final[11]		; to64(p,l,2); p += 2;
	*p = '\0';

	/* Don't leave anything around in vm they could use. */
	ZEND_SECURE_ZERO(final, sizeof(final));
	return (passwd);
}
