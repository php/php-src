/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   |         Xinchen Hui <laruence@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef BASE64_H
#define BASE64_H

/*
 * NEON and AVX512 implementation are based on https://github.com/WojciechMula/base64simd
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

#if (defined(ZEND_INTRIN_AVX2_FUNC_PTR) || defined(ZEND_INTRIN_SSSE3_FUNC_PTR) || defined(ZEND_INTRIN_AVX512_FUNC_PTR) || defined(ZEND_INTRIN_AVX512_VBMI_FUNC_PTR)) && !defined(ZEND_INTRIN_AVX2_NATIVE)
PHP_MINIT_FUNCTION(base64_intrin);
#endif

/* php_base64_encode_ex flags */
#define PHP_BASE64_NO_PADDING 1

PHPAPI extern zend_string *php_base64_encode_ex(const unsigned char *, size_t, zend_long flags);
PHPAPI extern zend_string *php_base64_decode_ex(const unsigned char *, size_t, bool);

static inline zend_string *php_base64_encode(const unsigned char *str, size_t len) {
	return php_base64_encode_ex(str, len, 0);
}
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
