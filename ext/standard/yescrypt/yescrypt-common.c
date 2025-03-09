/*-
 * Copyright 2013-2018 Alexander Peslyak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>

#include "insecure_memzero.h"

#define YESCRYPT_INTERNAL
#include "yescrypt.h"

static uint8_t *encode64(uint8_t *dst, size_t dstlen,
    const uint8_t *src, size_t srclen)
{
	size_t i;

	for (i = 0; i < srclen; ) {
		uint8_t *dnext;
		uint32_t value = 0, bits = 0;
		do {
			value |= (uint32_t)src[i++] << bits;
			bits += 8;
		} while (bits < 24 && i < srclen);
		dnext = yescrypt_encode64_uint32_fixed(dst, dstlen, value, bits);
		if (!dnext)
			return NULL;
		dstlen -= dnext - dst;
		dst = dnext;
	}

	if (dstlen < 1)
		return NULL;

	*dst = 0; /* NUL terminate just in case */

	return dst;
}

uint8_t *yescrypt_r(const yescrypt_shared_t *shared, yescrypt_local_t *local,
    const uint8_t *passwd, size_t passwdlen,
    const uint8_t *setting,
    const yescrypt_binary_t *key,
    uint8_t *buf, size_t buflen)
{
	unsigned char saltbin[64], hashbin[32];
	const uint8_t *src, *saltstr, *salt;
	uint8_t *dst;
	size_t need, prefixlen, saltstrlen, saltlen;
	yescrypt_params_t params = { .p = 1 };

	/* PHP change: extracted the settings parsing */
	src = yescrypt_parse_settings(setting, &params, key);
	if (!src) {
		return NULL;
	}

	prefixlen = src - setting;

	saltstr = src;
	src = (uint8_t *)strrchr((char *)saltstr, '$');
	if (src)
		saltstrlen = src - saltstr;
	else
		saltstrlen = strlen((char *)saltstr);

	if (setting[1] == '7') {
		salt = saltstr;
		saltlen = saltstrlen;
	} else {
		const uint8_t *saltend;

		saltlen = sizeof(saltbin);
		saltend = yescrypt_decode64(saltbin, &saltlen, saltstr, saltstrlen);

		if (!saltend || (size_t)(saltend - saltstr) != saltstrlen)
			goto fail;

		salt = saltbin;

		if (key) {
			/* PHP change: removed so we don't carry the encrypt implementation */
			ZEND_UNREACHABLE();
		}
	}

	need = prefixlen + saltstrlen + 1 + HASH_LEN + 1;
	if (need > buflen || need < saltstrlen)
		goto fail;

	if (yescrypt_kdf(shared, local, passwd, passwdlen, salt, saltlen,
	    &params, hashbin, sizeof(hashbin)))
		goto fail;

	if (key) {
		/* PHP change: removed so we don't carry the encrypt implementation */
		ZEND_UNREACHABLE();
	}

	dst = buf;
	memcpy(dst, setting, prefixlen + saltstrlen);
	dst += prefixlen + saltstrlen;
	*dst++ = '$';

	dst = encode64(dst, buflen - (dst - buf), hashbin, sizeof(hashbin));
	insecure_memzero(hashbin, sizeof(hashbin));
	if (!dst || dst >= buf + buflen)
		return NULL;

	*dst = 0; /* NUL termination */

	return buf;

fail:
	insecure_memzero(saltbin, sizeof(saltbin));
	insecure_memzero(hashbin, sizeof(hashbin));
	return NULL;
}
