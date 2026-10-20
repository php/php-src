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
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSON_ENCODER_H
#define	PHP_JSON_ENCODER_H

#include "php.h"
#include "zend_smart_str.h"

typedef struct _php_json_encoder php_json_encoder;

struct _php_json_encoder {
	int depth;
	int max_depth;
	php_json_error_code error_code;
};

static inline void php_json_encode_init(php_json_encoder *encoder)
{
	memset(encoder, 0, sizeof(php_json_encoder));
}

zend_result php_json_encode_zval(smart_str *buf, zval *val, int options, php_json_encoder *encoder);

zend_result php_json_escape_string(smart_str *buf, const char *s, size_t len, int options, php_json_encoder *encoder);

#endif	/* PHP_JSON_ENCODER_H */
