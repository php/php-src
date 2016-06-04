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
   | Author: Ilia Alshanetsky <ilia@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * Portions of this code are based on Berkeley's uuencode/uudecode
 * implementation.
 *
 * Copyright (c) 1983, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <math.h>

#include "php.h"
#include "php_uuencode.h"

#define PHP_UU_ENC(c) ((c) ? ((c) & 077) + ' ' : '`')
#define PHP_UU_ENC_C2(c) PHP_UU_ENC(((*(c) << 4) & 060) | ((*((c) + 1) >> 4) & 017))
#define PHP_UU_ENC_C3(c) PHP_UU_ENC(((*(c + 1) << 2) & 074) | ((*((c) + 2) >> 6) & 03))

#define PHP_UU_DEC(c) (((c) - ' ') & 077)

PHPAPI zend_string *php_uuencode(char *src, size_t src_len) /* {{{ */
{
	size_t len = 45;
	char *p, *s, *e, *ee;
	zend_string *dest;

	/* encoded length is ~ 38% greater than the original */
	dest = zend_string_alloc((size_t)ceil(src_len * 1.38) + 46, 0);
	p = ZSTR_VAL(dest);
	s = src;
	e = src + src_len;

	while ((s + 3) < e) {
		ee = s + len;
		if (ee > e) {
			ee = e;
			len = ee - s;
			if (len % 3) {
				ee = s + (int) (floor((double)len / 3) * 3);
			}
		}
		*p++ = PHP_UU_ENC(len);

		while (s < ee) {
			*p++ = PHP_UU_ENC(*s >> 2);
			*p++ = PHP_UU_ENC_C2(s);
			*p++ = PHP_UU_ENC_C3(s);
			*p++ = PHP_UU_ENC(*(s + 2) & 077);

			s += 3;
		}

		if (len == 45) {
			*p++ = '\n';
		}
	}

	if (s < e) {
		if (len == 45) {
			*p++ = PHP_UU_ENC(e - s);
			len = 0;
		}

		*p++ = PHP_UU_ENC(*s >> 2);
		*p++ = PHP_UU_ENC_C2(s);
		*p++ = ((e - s) > 1) ? PHP_UU_ENC_C3(s) : PHP_UU_ENC('\0');
		*p++ = ((e - s) > 2) ? PHP_UU_ENC(*(s + 2) & 077) : PHP_UU_ENC('\0');
	}

	if (len < 45) {
		*p++ = '\n';
	}

	*p++ = PHP_UU_ENC('\0');
	*p++ = '\n';
	*p = '\0';

	dest = zend_string_truncate(dest, p - ZSTR_VAL(dest), 0);
	return dest;
}
/* }}} */

PHPAPI zend_string *php_uudecode(char *src, size_t src_len) /* {{{ */
{
	size_t len, total_len=0;
	char *s, *e, *p, *ee;
	zend_string *dest;

	dest = zend_string_alloc((size_t) ceil(src_len * 0.75), 0);
	p = ZSTR_VAL(dest);
	s = src;
	e = src + src_len;

	while (s < e) {
		if ((len = PHP_UU_DEC(*s++)) <= 0) {
			break;
		}
		/* sanity check */
		if (len > src_len) {
			goto err;
		}

		total_len += len;

		ee = s + (len == 45 ? 60 : (int) floor(len * 1.33));
		/* sanity check */
		if (ee > e) {
			goto err;
		}

		while (s < ee) {
			if(s+4 > e) {
				goto err;
			}
			*p++ = PHP_UU_DEC(*s) << 2 | PHP_UU_DEC(*(s + 1)) >> 4;
			*p++ = PHP_UU_DEC(*(s + 1)) << 4 | PHP_UU_DEC(*(s + 2)) >> 2;
			*p++ = PHP_UU_DEC(*(s + 2)) << 6 | PHP_UU_DEC(*(s + 3));
			s += 4;
		}

		if (len < 45) {
			break;
		}

		/* skip \n */
		s++;
	}

	assert(p >= ZSTR_VAL(dest));
	if ((len = total_len) > (size_t)(p - ZSTR_VAL(dest))) {
		*p++ = PHP_UU_DEC(*s) << 2 | PHP_UU_DEC(*(s + 1)) >> 4;
		if (len > 1) {
			*p++ = PHP_UU_DEC(*(s + 1)) << 4 | PHP_UU_DEC(*(s + 2)) >> 2;
			if (len > 2) {
				*p++ = PHP_UU_DEC(*(s + 2)) << 6 | PHP_UU_DEC(*(s + 3));
			}
		}
	}

	ZSTR_LEN(dest) = total_len;
	ZSTR_VAL(dest)[ZSTR_LEN(dest)] = '\0';

	return dest;

err:
	zend_string_free(dest);

	return NULL;
}
/* }}} */

/* {{{ proto string convert_uuencode(string data)
   uuencode a string */
PHP_FUNCTION(convert_uuencode)
{
	zend_string *src;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &src) == FAILURE || ZSTR_LEN(src) < 1) {
		RETURN_FALSE;
	}

	RETURN_STR(php_uuencode(ZSTR_VAL(src), ZSTR_LEN(src)));
}
/* }}} */

/* {{{ proto string convert_uudecode(string data)
   decode a uuencoded string */
PHP_FUNCTION(convert_uudecode)
{
	zend_string *src;
	zend_string *dest;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &src) == FAILURE || ZSTR_LEN(src) < 1) {
		RETURN_FALSE;
	}

	if ((dest = php_uudecode(ZSTR_VAL(src), ZSTR_LEN(src))) == NULL) {
		php_error_docref(NULL, E_WARNING, "The given parameter is not a valid uuencoded string");
		RETURN_FALSE;
	}

	RETURN_STR(dest);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
