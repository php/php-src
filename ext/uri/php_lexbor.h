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

#ifndef PHP_LEXBOR_H
#define PHP_LEXBOR_H

#include <php_uri_common.h>
#include <lexbor/url/url.h>

zend_result lexbor_init_parser(void);
void *lexbor_parse_uri(const zend_string *url_str, const zend_string *base_url_str, zval *errors);
void lexbor_instantiate_uri(zval *zv);
zend_string *lexbor_to_string(void *uri_object_internal);
void lexbor_free_uri(void *uri_object_internal);
zend_result lexbor_destroy_parser(void);

extern const uri_handler_t lexbor_uri_handler;

#endif
