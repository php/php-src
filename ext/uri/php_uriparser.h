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
#include <php_uri_common.h>

zend_result uriparser_init_parser(void);
void *uriparser_parse_uri(const zend_string *uri_str, const zend_string *base_url_str, zval *errors);
void uriparser_instantiate_uri(zval *zv);
zend_string *uriparser_uri_to_string(void *uri_object_internal);
void uriparser_free_uri(void *uri_object_internal);
zend_result uriparser_destroy_parser(void);

extern const uri_handler_t uriparser_uri_handler;

#endif
