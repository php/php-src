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

#ifndef PHP_URI_PARSER_WHATWG_H
#define PHP_URI_PARSER_WHATWG_H

#include "php_uri_common.h"
#include "lexbor/url/url.h"

PHPAPI extern const php_uri_parser php_uri_parser_whatwg;

lxb_url_t *php_uri_parser_whatwg_parse_ex(const char *uri_str, size_t uri_str_len, const lxb_url_t *lexbor_base_url, zval *errors, bool silent);

PHP_RINIT_FUNCTION(uri_parser_whatwg);
ZEND_MODULE_POST_ZEND_DEACTIVATE_D(uri_parser_whatwg);

#endif
