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
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

#ifndef URL_SCANNER_EX_H
#define URL_SCANNER_EX_H

PHP_MINIT_FUNCTION(url_scanner_ex);
PHP_MSHUTDOWN_FUNCTION(url_scanner_ex);

PHP_RINIT_FUNCTION(url_scanner_ex);
PHP_RSHUTDOWN_FUNCTION(url_scanner_ex);

PHPAPI char *php_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen, bool encode);
PHPAPI zend_result php_url_scanner_add_session_var(const char *name, size_t name_len, const char *value, size_t value_len, bool encode);
PHPAPI zend_result php_url_scanner_reset_session_var(zend_string *name, int encode);
PHPAPI zend_result php_url_scanner_reset_session_vars(void);
PHPAPI zend_result php_url_scanner_add_var(const char *name, size_t name_len, const char *value, size_t value_len, bool encode);
PHPAPI zend_result php_url_scanner_reset_var(zend_string *name, int encode);
PHPAPI zend_result php_url_scanner_reset_vars(void);

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

	int type;
	smart_str attr_val;
	int tag_type;
	int attr_type;

	/* Everything above is zeroed in RINIT */
	HashTable *tags;
} url_adapt_state_ex_t;

#endif
