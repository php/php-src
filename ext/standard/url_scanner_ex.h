/*
  +----------------------------------------------------------------------+
  | PHP version 4.0                                                      |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Sascha Schumann <sascha@schumann.cx>                        |
  +----------------------------------------------------------------------+
*/

#ifndef URL_SCANNER_EX_H
#define URL_SCANNER_EX_H

PHP_RSHUTDOWN_FUNCTION(url_scanner_ex);
PHP_RINIT_FUNCTION(url_scanner_ex);

char *url_adapt_ext(const char *src, size_t srclen, const char *name, const char *value, size_t *newlen);

typedef struct {
	char *c;
	size_t len;
	size_t a;
} smart_str;

typedef struct {
	smart_str arg;
	smart_str tag;
	smart_str para;
	smart_str work;
	smart_str result;
	int state;
	smart_str name;
	smart_str value;
} url_adapt_state_ex_t;

#endif
