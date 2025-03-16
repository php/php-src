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

#include <stdint.h>
#include <string.h>

#define YESCRYPT_INTERNAL
#include "yescrypt.h"

static const char * const itoa64 =
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static const uint8_t atoi64_partial[77] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	64, 64, 64, 64, 64, 64, 64,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
	64, 64, 64, 64, 64, 64,
	38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
};

static uint8_t *encode64_uint32(uint8_t *dst, size_t dstlen,
    uint32_t src, uint32_t min)
{
	uint32_t start = 0, end = 47, chars = 1, bits = 0;

	if (src < min)
		return NULL;
	src -= min;

	do {
		uint32_t count = (end + 1 - start) << bits;
		if (src < count)
			break;
		if (start >= 63)
			return NULL;
		start = end + 1;
		end = start + (62 - end) / 2;
		src -= count;
		chars++;
		bits += 6;
	} while (1);

	if (dstlen <= chars) /* require room for a NUL terminator */
		return NULL;

	*dst++ = itoa64[start + (src >> bits)];

	while (--chars) {
		bits -= 6;
		*dst++ = itoa64[(src >> bits) & 0x3f];
	}

	*dst = 0; /* NUL terminate just in case */

	return dst;
}

static inline uint32_t atoi64(uint8_t src)
{
	if (src >= '.' && src <= 'z')
		return atoi64_partial[src - '.'];

	return 64;
}

static const uint8_t *decode64_uint32(uint32_t *dst,
    const uint8_t *src, uint32_t min)
{
	uint32_t start = 0, end = 47, chars = 1, bits = 0;
	uint32_t c;

	c = atoi64(*src++);
	if (c > 63)
		goto fail;

	*dst = min;
	while (c > end) {
		*dst += (end + 1 - start) << bits;
		start = end + 1;
		end = start + (62 - end) / 2;
		chars++;
		bits += 6;
	}

	*dst += (c - start) << bits;

	while (--chars) {
		c = atoi64(*src++);
		if (c > 63)
			goto fail;
		bits -= 6;
		*dst += c << bits;
	}

	return src;

fail:
	*dst = 0;
	return NULL;
}

uint8_t *yescrypt_encode64_uint32_fixed(uint8_t *dst, size_t dstlen,
										uint32_t src, uint32_t srcbits)
{
	uint32_t bits;

	for (bits = 0; bits < srcbits; bits += 6) {
		if (dstlen < 2)
			return NULL;
		*dst++ = itoa64[src & 0x3f];
		dstlen--;
		src >>= 6;
	}

	if (src || dstlen < 1)
		return NULL;

	*dst = 0; /* NUL terminate just in case */

	return dst;
}

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

static const uint8_t *decode64_uint32_fixed(uint32_t *dst, uint32_t dstbits,
    const uint8_t *src)
{
	uint32_t bits;

	*dst = 0;
	for (bits = 0; bits < dstbits; bits += 6) {
		uint32_t c = atoi64(*src++);
		if (c > 63) {
			*dst = 0;
			return NULL;
		}
		*dst |= c << bits;
	}

	return src;
}

const uint8_t *yescrypt_decode64(uint8_t *dst, size_t *dstlen,
							     const uint8_t *src, size_t srclen)
{
	size_t dstpos = 0;

	while (dstpos <= *dstlen && srclen) {
		uint32_t value = 0, bits = 0;
		while (srclen--) {
			uint32_t c = atoi64(*src);
			if (c > 63) {
				srclen = 0;
				break;
			}
			src++;
			value |= c << bits;
			bits += 6;
			if (bits >= 24)
				break;
		}
		if (!bits)
			break;
		if (bits < 12) /* must have at least one full byte */
			goto fail;
		while (dstpos++ < *dstlen) {
			*dst++ = value;
			value >>= 8;
			bits -= 8;
			if (bits < 8) { /* 2 or 4 */
				if (value) /* must be 0 */
					goto fail;
				bits = 0;
				break;
			}
		}
		if (bits)
			goto fail;
	}

	if (!srclen && dstpos <= *dstlen) {
		*dstlen = dstpos;
		return src;
	}

	fail:
	*dstlen = 0;
	return NULL;
}

/* PHP change: code to parse the settings extracted from yescrypt_r */
const uint8_t *yescrypt_parse_settings(const uint8_t *setting, yescrypt_params_t *params, const yescrypt_binary_t *key)
{
	if (setting[0] != '$' ||
		(setting[1] != '7' && setting[1] != 'y') ||
		setting[2] != '$')
		return NULL;
	const uint8_t *src = setting + 3;

	if (setting[1] == '7') {
		uint32_t N_log2 = atoi64(*src++);
		if (N_log2 < 1 || N_log2 > 63)
			return NULL;
		params->N = (uint64_t)1 << N_log2;

		src = decode64_uint32_fixed(&params->r, 30, src);
		if (!src)
			return NULL;

		src = decode64_uint32_fixed(&params->p, 30, src);
		if (!src)
			return NULL;

		if (key)
			return NULL;
	} else {
		uint32_t flavor, N_log2;

		src = decode64_uint32(&flavor, src, 0);
		if (!src)
			return NULL;

		if (flavor < YESCRYPT_RW) {
			params->flags = flavor;
		} else if (flavor <= YESCRYPT_RW + (YESCRYPT_RW_FLAVOR_MASK >> 2)) {
			params->flags = YESCRYPT_RW + ((flavor - YESCRYPT_RW) << 2);
		} else {
			return NULL;
		}

		src = decode64_uint32(&N_log2, src, 1);
		if (!src || N_log2 > 63)
			return NULL;
		params->N = (uint64_t)1 << N_log2;

		src = decode64_uint32(&params->r, src, 1);
		if (!src)
			return NULL;

		if (*src != '$') {
			uint32_t have;

			src = decode64_uint32(&have, src, 1);
			if (!src)
				return NULL;

			if (have & 1) {
				src = decode64_uint32(&params->p, src, 2);
				if (!src)
					return NULL;
			}

			if (have & 2) {
				src = decode64_uint32(&params->t, src, 1);
				if (!src)
					return NULL;
			}

			if (have & 4) {
				src = decode64_uint32(&params->g, src, 1);
				if (!src)
					return NULL;
			}

			if (have & 8) {
				uint32_t NROM_log2;
				src = decode64_uint32(&NROM_log2, src, 1);
				if (!src || NROM_log2 > 63)
					return NULL;
				params->NROM = (uint64_t)1 << NROM_log2;
			}
		}

		if (*src++ != '$')
			return NULL;
	}

	return src;
}

static uint32_t N2log2(uint64_t N)
{
	uint32_t N_log2;

	if (N < 2)
		return 0;

	N_log2 = 2;
	while (N >> N_log2 != 0)
		N_log2++;
	N_log2--;

	if (N >> N_log2 != 1)
		return 0;

	return N_log2;
}

uint8_t *yescrypt_encode_params_r(const yescrypt_params_t *params,
    const uint8_t *src, size_t srclen,
    uint8_t *buf, size_t buflen)
{
	uint32_t flavor, N_log2, NROM_log2, have;
	uint8_t *dst;

	if (srclen > SIZE_MAX / 16)
		return NULL;

	if (params->flags < YESCRYPT_RW) {
		flavor = params->flags;
	} else if ((params->flags & YESCRYPT_MODE_MASK) == YESCRYPT_RW &&
	    params->flags <= (YESCRYPT_RW | YESCRYPT_RW_FLAVOR_MASK)) {
		flavor = YESCRYPT_RW + (params->flags >> 2);
	} else {
		return NULL;
	}

	N_log2 = N2log2(params->N);
	if (!N_log2)
		return NULL;

	NROM_log2 = N2log2(params->NROM);
	if (params->NROM && !NROM_log2)
		return NULL;

	if ((uint64_t)params->r * (uint64_t)params->p >= (1U << 30))
		return NULL;

	dst = buf;
	*dst++ = '$';
	*dst++ = 'y';
	*dst++ = '$';

	dst = encode64_uint32(dst, buflen - (dst - buf), flavor, 0);
	if (!dst)
		return NULL;

	dst = encode64_uint32(dst, buflen - (dst - buf), N_log2, 1);
	if (!dst)
		return NULL;

	dst = encode64_uint32(dst, buflen - (dst - buf), params->r, 1);
	if (!dst)
		return NULL;

	have = 0;
	if (params->p != 1)
		have |= 1;
	if (params->t)
		have |= 2;
	if (params->g)
		have |= 4;
	if (NROM_log2)
		have |= 8;

	if (have) {
		dst = encode64_uint32(dst, buflen - (dst - buf), have, 1);
		if (!dst)
			return NULL;
	}

	if (params->p != 1) {
		dst = encode64_uint32(dst, buflen - (dst - buf), params->p, 2);
		if (!dst)
			return NULL;
	}

	if (params->t) {
		dst = encode64_uint32(dst, buflen - (dst - buf), params->t, 1);
		if (!dst)
			return NULL;
	}

	if (params->g) {
		dst = encode64_uint32(dst, buflen - (dst - buf), params->g, 1);
		if (!dst)
			return NULL;
	}

	if (NROM_log2) {
		dst = encode64_uint32(dst, buflen - (dst - buf), NROM_log2, 1);
		if (!dst)
			return NULL;
	}

	if (dst >= buf + buflen)
		return NULL;

	*dst++ = '$';

	dst = encode64(dst, buflen - (dst - buf), src, srclen);
	if (!dst || dst >= buf + buflen)
		return NULL;

	*dst = 0; /* NUL termination */

	return buf;
}
