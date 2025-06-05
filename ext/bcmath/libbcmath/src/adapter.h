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
#include "config.h"
#endif

#include "zend.h"

/* Needed for BCG() macro and PHP_ROUND_XXX */
#include "../../php_bcmath.h"

typedef zend_string bc_string;
#define BC_STR_VAL(s) ZSTR_VAL(s)
#define BC_STR_LEN(s) ZSTR_LEN(s)
#define bc_string_alloc(len, persistent) zend_string_alloc((len), (persistent))

#endif
