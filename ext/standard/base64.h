/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   |         Xinchen Hui <laruence@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef BASE64_H
#define BASE64_H

/*
 * NEON implementation is based on https://github.com/WojciechMula/base64simd
 * which is copyrighted to:
 * Copyright (c) 2015-2018, Wojciech Mula
 * All rights reserved.
 *
 * SSSE3 and AVX2 implementation are based on https://github.com/aklomp/base64
 * which is copyrighted to:
 *
 * Copyright (c) 2005-2007, Nick Galbreath
 * Copyright (c) 2013-2017, Alfred Klomp
 * Copyright (c) 2015-2017, Wojciech Mula
 * Copyright (c) 2016-2017, Matthieu Darbois
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

PHP_FUNCTION(base64_decode);
PHP_FUNCTION(base64_encode);

#if (ZEND_INTRIN_AVX2_FUNC_PTR || ZEND_INTRIN_SSSE3_FUNC_PTR) && !ZEND_INTRIN_AVX2_NATIVE
PHP_MINIT_FUNCTION(base64_intrin);
#endif

PHPAPI extern zend_string *php_base64_encode(const unsigned char *, size_t);
PHPAPI extern zend_string *php_base64_decode_ex(const unsigned char *, size_t, zend_bool);

static inline zend_string *php_base64_encode_str(const zend_string *str) {
	return php_base64_encode((const unsigned char*)(ZSTR_VAL(str)), ZSTR_LEN(str));
}

static inline zend_string *php_base64_decode(const unsigned char *str, size_t len) {
	return php_base64_decode_ex(str, len, 0);
}
static inline zend_string *php_base64_decode_str(const zend_string *str) {
	return php_base64_decode_ex((const unsigned char*)(ZSTR_VAL(str)), ZSTR_LEN(str), 0);
}

#endif /* BASE64_H */
