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
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* libbcmath is heavily optimized for PHP. This file consolidates
 * the features that libbcmath depends on from PHP. */

#ifndef _BCMATH_ADAPTER_H_
#define _BCMATH_ADAPTER_H_

#ifdef HAVE_CONFIG_H
#  ifdef PHP_WIN32
#    include "../../../../main/config.w32.h"
#  else
#    include <php_config.h>
#  endif
#endif

#include "zend.h"

/* Needed for BCG() macro and PHP_ROUND_XXX */
#include "../../php_bcmath.h"

typedef zend_string bc_string;
#define BC_STR_VAL(s) ZSTR_VAL(s)
#define BC_STR_LEN(s) ZSTR_LEN(s)
#define bc_string_alloc(len, persistent) zend_string_alloc((len), (persistent))

typedef zend_long bc_long;
#define BC_LONG_MIN_DIGITS LONG_MIN_DIGITS
#define BC_L(i) Z_L(i)
#define BC_LONG_MAX ZEND_LONG_MAX
#define BC_LONG_MIN ZEND_LONG_MIN

#define bc_safe_emalloc(n, size, offset) (safe_emalloc((n), (size), (offset)))
#define bc_efree(ptr) (efree((ptr)))

#define bc_pemalloc(size, persistent) (pemalloc((size), (persistent)))
#define bc_pefree(ptr, persistent) (pefree((ptr), (persistent)))

#define bc_safe_address_guarded(n, size, offset) (zend_safe_address_guarded((n), (size), (offset)))
#define BC_MM_ALIGNMENT ZEND_MM_ALIGNMENT

#define BC_ROUND_HALF_UP        PHP_ROUND_HALF_UP
#define BC_ROUND_HALF_DOWN      PHP_ROUND_HALF_DOWN
#define BC_ROUND_HALF_EVEN      PHP_ROUND_HALF_EVEN
#define BC_ROUND_HALF_ODD       PHP_ROUND_HALF_ODD
#define BC_ROUND_CEILING        PHP_ROUND_CEILING
#define BC_ROUND_FLOOR          PHP_ROUND_FLOOR
#define BC_ROUND_TOWARD_ZERO    PHP_ROUND_TOWARD_ZERO
#define BC_ROUND_AWAY_FROM_ZERO PHP_ROUND_AWAY_FROM_ZERO

#define BC_EMPTY_SWITCH_DEFAULT_CASE() EMPTY_SWITCH_DEFAULT_CASE()

#ifdef PHP_HAVE_BUILTIN_CTZL
#  define BC_HAVE_BUILTIN_CTZL
#endif

#define BC_ASSERT(cond) ZEND_ASSERT(cond)

#define BC_BYTES_SWAP64(x) ZEND_BYTES_SWAP64(x)
#define BC_BYTES_SWAP32(x) ZEND_BYTES_SWAP32(x)

#ifdef WORDS_BIGENDIAN
#  define BC_LITTLE_ENDIAN 0
#else
#  define BC_LITTLE_ENDIAN 1
#endif

#endif
