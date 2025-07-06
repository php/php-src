/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_URIPARSER_H
#define PHP_URIPARSER_H

#include <uriparser/Uri.h>
#include "php_uri_common.h"

extern const uri_handler_t uriparser_uri_handler;

typedef struct uriparser_uris_t {
	UriUriA uri;
	UriUriA normalized_uri;
	bool normalized_uri_initialized;
} uriparser_uris_t;

PHP_MINIT_FUNCTION(uri_uriparser);

zend_result uriparser_read_userinfo(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval);

void *uriparser_parse_uri_ex(const zend_string *uri_str, const uriparser_uris_t *uriparser_base_url, bool silent);

#endif
