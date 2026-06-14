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

#ifndef PHP_JSON_SCANNER_H
#define	PHP_JSON_SCANNER_H

#include "php.h"
#include "php_json.h"

typedef unsigned char php_json_ctype;

typedef struct _php_json_error_location {
	/** first column of the error */
	size_t first_column;
	/** first line of the error */
	size_t first_line;
	/** last column of the error */
	size_t last_column;
	/** last line of the error */
	size_t last_line;
} php_json_error_location;

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
	php_json_error_location errloc; /* error location */
	int utf8_invalid;               /* whether utf8 is invalid */
	int utf8_invalid_count;         /* number of extra character for invalid utf8 */
} php_json_scanner;

#define PHP_JSON_SCANNER_LOCATION(scanner, slocation) (scanner).errloc.slocation

void php_json_scanner_init(php_json_scanner *scanner, const char *str, size_t str_len, int options);
int php_json_scan(php_json_scanner *s);

#endif	/* PHP_JSON_SCANNER_H */
