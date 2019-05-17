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
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSON_SCANNER_H
#define	PHP_JSON_SCANNER_H

#include "php.h"
#include "php_json.h"

typedef unsigned char php_json_ctype;

typedef struct _php_json_scanner {
	php_json_ctype *cursor;         /* cursor position */
	php_json_ctype *token;          /* token position */
	php_json_ctype *limit;          /* the last read character + 1 position */
	php_json_ctype *marker;         /* marker position for backtracking */
	php_json_ctype *ctxmarker;      /* marker position for context backtracking */
	php_json_ctype *str_start;      /* start position of the string */
	php_json_ctype *pstr;           /* string pointer for escapes conversion */
	zval value;                     /* value */
	int str_esc;                    /* number of extra characters for escaping */
	int state;                      /* condition state */
	int options;                    /* options */
	php_json_error_code errcode;    /* error type if there is an error */
	int utf8_invalid;               /* whether utf8 is invalid */
	int utf8_invalid_count;         /* number of extra character for invalid utf8 */
} php_json_scanner;


void php_json_scanner_init(php_json_scanner *scanner, char *str, size_t str_len, int options);
int php_json_scan(php_json_scanner *s);

#endif	/* PHP_JSON_SCANNER_H */
