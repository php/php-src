/* $Id$ */
/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if PHP_WIN32
# include <windows.h>
# include <Wincrypt.h>
#endif

#ifdef HAVE_ATOMIC_H /* Solaris 10 defines atomic API within */
# include <atomic.h>
#else
# include <signal.h>
#endif
#include "php_crypt_r.h"
#include "crypt_freesec.h"

#if !PHP_WIN32
#include "ext/standard/md5.h"
#endif

#ifdef ZTS
MUTEX_T php_crypt_extended_init_lock;
#endif

/* TODO: enable it when enabling vista/2k8 mode in tsrm */
#if 0
CONDITION_VARIABLE initialized;
#endif

void php_init_crypt_r()
{
#ifdef ZTS
	php_crypt_extended_init_lock = tsrm_mutex_alloc();
#endif
}

void php_shutdown_crypt_r()
{
#ifdef ZTS
	tsrm_mutex_free(php_crypt_extended_init_lock);
#endif
}

void _crypt_extended_init_r(void)
{
#ifdef PHP_WIN32
	LONG volatile initialized = 0;
#elif defined(HAVE_ATOMIC_H) /* Solaris 10 defines atomic API within */
	volatile unsigned int initialized = 0;
#else
	static volatile sig_atomic_t initialized = 0;
#endif

#ifdef ZTS
	tsrm_mutex_lock(php_crypt_extended_init_lock);
#endif

	if (!initialized) {
#ifdef PHP_WIN32
		InterlockedIncrement(&initialized);
#elif defined(HAVE_SYNC_FETCH_AND_ADD)
		__sync_fetch_and_add(&initialized, 1);
#elif defined(HAVE_ATOMIC_H) /* Solaris 10 defines atomic API within */
		membar_producer();
		atomic_add_int(&initialized, 1);
#endif
		_crypt_extended_init();
	}
#ifdef ZTS
	tsrm_mutex_unlock(php_crypt_extended_init_lock);
#endif
}

/* MD% crypt implementation using the windows CryptoApi */
#define MD5_MAGIC "$1$"
#define MD5_MAGIC_LEN 3

static unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void
to64(char *s, int32_t v, int n)
{
	while (--n >= 0) {
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

#if PHP_WIN32
char * php_md5_crypt_r(const char *pw, const char *salt, char *out) {
	HCRYPTPROV hCryptProv;
	HCRYPTHASH ctx, ctx1;
	DWORD i, pwl, sl;
	const BYTE magic_md5[4] = "$1$";
	const DWORD magic_md5_len = 3;
	DWORD        dwHashLen;
	int pl;
	__int32 l;
	const char *sp = salt;
	const char *ep = salt;
	char *p = NULL;
	char *passwd = out;
	unsigned char final[16];

	/* Acquire a cryptographic provider context handle. */
	if(!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		return NULL;
	}

	pwl = (DWORD) strlen(pw);

	/* Refine the salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if (strncmp(sp, MD5_MAGIC, MD5_MAGIC_LEN) == 0) {
		sp += MD5_MAGIC_LEN;
	}

	/* It stops at the first '$', max 8 chars */
	for (ep = sp; *ep != '\0' && *ep != '$' && ep < (sp + 8); ep++) {
		continue;
	}

	/* get the length of the true salt */
	sl = (DWORD)(ep - sp);

	/* Create an empty hash object. */
	if(!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &ctx)) {
		goto _destroyProv;
	}

	/* The password first, since that is what is most unknown */
	if(!CryptHashData(ctx, (BYTE *)pw, pwl, 0)) {
		goto _destroyCtx0;
	}

	/* Then our magic string */
	if(!CryptHashData(ctx, magic_md5, magic_md5_len, 0)) {
		goto _destroyCtx0;
	}

	/* Then the raw salt */
	if(!CryptHashData( ctx, (BYTE *)sp, sl, 0)) {
		goto _destroyCtx0;
	}

	/* MD5(pw,salt,pw), valid. */
	/* Then just as many characters of the MD5(pw,salt,pw) */
	if(!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &ctx1)) {
		goto _destroyCtx0;
	}
	if(!CryptHashData(ctx1, (BYTE *)pw, pwl, 0)) {
		goto _destroyCtx1;
	}
	if(!CryptHashData(ctx1, (BYTE *)sp, sl, 0)) {
		goto _destroyCtx1;
	}
	if(!CryptHashData(ctx1, (BYTE *)pw, pwl, 0)) {
		goto _destroyCtx1;
	}

	dwHashLen = 16;
	CryptGetHashParam(ctx1, HP_HASHVAL, final, &dwHashLen, 0);
	/*  MD5(pw,salt,pw). Valid. */

	for (pl = pwl; pl > 0; pl -= 16) {
		CryptHashData(ctx, final, (DWORD)(pl > 16 ? 16 : pl), 0);
	}

	/* Don't leave anything around in vm they could use. */
	ZEND_SECURE_ZERO(final, sizeof(final));

	/* Then something really weird... */
	for (i = pwl; i != 0; i >>= 1) {
		if ((i & 1) != 0) {
			CryptHashData(ctx, (const BYTE *)final, 1, 0);
		} else {
			CryptHashData(ctx, (const BYTE *)pw, 1, 0);
		}
	}

	memcpy(passwd, MD5_MAGIC, MD5_MAGIC_LEN);

	if (strncpy_s(passwd + MD5_MAGIC_LEN, MD5_HASH_MAX_LEN - MD5_MAGIC_LEN, sp, sl + 1) != 0) {
		goto _destroyCtx1;
	}
	passwd[MD5_MAGIC_LEN + sl] = '\0';
	strcat_s(passwd, MD5_HASH_MAX_LEN, "$");

	dwHashLen = 16;

	/* Fetch the ctx hash value */
	CryptGetHashParam(ctx, HP_HASHVAL, final, &dwHashLen, 0);

	for (i = 0; i < 1000; i++) {
		if(!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &ctx1)) {
			goto _destroyCtx1;
		}

		if ((i & 1) != 0) {
			if(!CryptHashData(ctx1, (BYTE *)pw, pwl, 0)) {
				goto _destroyCtx1;
			}
		} else {
			if(!CryptHashData(ctx1, (BYTE *)final, 16, 0)) {
				goto _destroyCtx1;
			}
		}

		if ((i % 3) != 0) {
			if(!CryptHashData(ctx1, (BYTE *)sp, sl, 0)) {
				goto _destroyCtx1;
			}
		}

		if ((i % 7) != 0) {
			if(!CryptHashData(ctx1, (BYTE *)pw, pwl, 0)) {
				goto _destroyCtx1;
			}
		}

		if ((i & 1) != 0) {
			if(!CryptHashData(ctx1, (BYTE *)final, 16, 0)) {
				goto _destroyCtx1;
			}
		} else {
			if(!CryptHashData(ctx1, (BYTE *)pw, pwl, 0)) {
				goto _destroyCtx1;
			}
		}

		/* Fetch the ctx hash value */
		dwHashLen = 16;
		CryptGetHashParam(ctx1, HP_HASHVAL, final, &dwHashLen, 0);
		if(!(CryptDestroyHash(ctx1))) {
			goto _destroyCtx0;
		}
	}

	ctx1 = (HCRYPTHASH) NULL;

	p = passwd + sl + MD5_MAGIC_LEN + 1;

	l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(p,l,4); p += 4;
	l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(p,l,4); p += 4;
	l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(p,l,4); p += 4;
	l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(p,l,4); p += 4;
	l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(p,l,4); p += 4;
	l = final[11]; to64(p,l,2); p += 2;

	*p = '\0';

	ZEND_SECURE_ZERO(final, sizeof(final));


_destroyCtx1:
	if (ctx1) {
		if (!CryptDestroyHash(ctx1)) {

		}
	}

_destroyCtx0:
	CryptDestroyHash(ctx);

_destroyProv:
	/* Release the provider handle.*/
	if(hCryptProv) {
		if(!(CryptReleaseContext(hCryptProv, 0))) {
			return NULL;
		}
	}

	return out;
}
#else

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
	php_uint32 l;
	int pl;

	pwl = strlen(pw);

	/* Refine the salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if (strncmp(sp, MD5_MAGIC, MD5_MAGIC_LEN) == 0)
		sp += MD5_MAGIC_LEN;

	/* It stops at the first '$', max 8 chars */
	for (ep = sp; *ep != '\0' && *ep != '$' && ep < (sp + 8); ep++)
		continue;

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
	memset(final, 0, sizeof(final));

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

#undef MD5_MAGIC
#undef MD5_MAGIC_LEN
#endif

