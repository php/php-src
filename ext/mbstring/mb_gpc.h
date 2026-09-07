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
   | Author: Rui Hirokawa <hirokawa@php.net>                              |
   |         Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
 */

/* {{{ includes */
#include "php.h"
/* }}} */

/* {{{ typedefs */
typedef struct _php_mb_encoding_handler_info_t {
	const char *separator;
	const mbfl_encoding *to_encoding;
	const mbfl_encoding **from_encodings;
	size_t num_from_encodings;
	int data_type;
	bool report_errors;
} php_mb_encoding_handler_info_t;

/* }}}*/

/* {{{ prototypes */
SAPI_POST_HANDLER_FUNC(php_mb_post_handler);
MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data);

int _php_mb_enable_encoding_translation(int flag);
const mbfl_encoding *_php_mb_encoding_handler_ex(const php_mb_encoding_handler_info_t *info, zval *arg, char *res);
/* }}} */
