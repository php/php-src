/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
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

PHP_RINIT_FUNCTION(url_scanner_ex);
PHP_RSHUTDOWN_FUNCTION(url_scanner_ex);

char *php_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen TSRMLS_DC);

int php_url_scanner_add_var(char *name, int name_len, char *value, int value_len, int urlencode TSRMLS_DC);
int php_url_scanner_reset_vars(TSRMLS_D);

int php_url_scanner_ex_activate(TSRMLS_D);
int php_url_scanner_ex_deactivate(TSRMLS_D);

#include "php_smart_str_public.h"

typedef struct {
	/* Used by the mainloop of the scanner */
	smart_str tag; /* read only */
	smart_str arg; /* read only */
	smart_str val; /* read only */
	smart_str buf;

	/* The result buffer */
	smart_str result;

	/* The data which is appended to each relative URL/FORM */
	smart_str form_app, url_app;

	int active;

	char *lookup_data;
	int state;
	
	/* Everything above is zeroed in RINIT */
	HashTable *tags;
} url_adapt_state_ex_t;

typedef struct {
	smart_str var;
	smart_str val;
} url_adapt_var_t;

#endif
