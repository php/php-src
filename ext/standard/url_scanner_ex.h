/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

#ifndef URL_SCANNER_EX_H
#define URL_SCANNER_EX_H

PHP_MINIT_FUNCTION(url_scanner_ex);
PHP_MSHUTDOWN_FUNCTION(url_scanner_ex);
int php_url_scanner_ex_activate(TSRMLS_D);
int php_url_scanner_ex_deactivate(TSRMLS_D);

char *url_adapt_ext_ex(const char *src, size_t srclen, const char *name, const char *value, char *udata_name, char *udata_value, size_t *newlen, zend_bool do_flush TSRMLS_DC);

char *url_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, char *udata_name, char *udata_value, size_t *newlen TSRMLS_DC);

char *url_adapt_flush(size_t * TSRMLS_DC);

#include "php_smart_str_public.h"

typedef struct {
	/* Used by the mainloop of the scanner */
	smart_str tag; /* read only */
	smart_str arg; /* read only */
	smart_str val; /* read only */
	smart_str buf;

	/* The result buffer */
	smart_str result;

	/* The data which is appended to each relative URL */
	smart_str q_name;
	smart_str q_value;

	smart_str q_udata_name;
	smart_str q_udata_value;

	char *lookup_data;
	int state;
	
	/* Everything above is zeroed in RINIT */
	HashTable *tags;
} url_adapt_state_ex_t;

#endif
