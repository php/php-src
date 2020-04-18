/*
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

int php_json_encode_zval(smart_str *buf, zval *val, int options, php_json_encoder *encoder);

#endif	/* PHP_JSON_ENCODER_H */
