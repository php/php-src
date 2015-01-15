/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef URL_SCANNER_EX_H
#define URL_SCANNER_EX_H

PHP_MINIT_FUNCTION(url_scanner_ex);
PHP_MSHUTDOWN_FUNCTION(url_scanner_ex);

PHP_RINIT_FUNCTION(url_scanner_ex);
PHP_RSHUTDOWN_FUNCTION(url_scanner_ex);

PHPAPI char *php_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen);
PHPAPI int php_url_scanner_add_var(char *name, size_t name_len, char *value, size_t value_len, int urlencode);
PHPAPI int php_url_scanner_reset_vars(void);

#include "zend_smart_str_public.h"

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

#endif
