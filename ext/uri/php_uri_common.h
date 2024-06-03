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

#ifndef PHP_URI_COMMON_H
#define PHP_URI_COMMON_H

#include <lexbor/url/url.h>

extern zend_class_entry *uri_ce;
extern zend_class_entry *whatwg_error_ce;

ZEND_BEGIN_MODULE_GLOBALS(uri)
	lxb_url_parser_t *lexbor_parser;
	zend_long lexbor_urls;
ZEND_END_MODULE_GLOBALS(uri)

#if defined(ZTS) && defined(COMPILE_DL_URI)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(uri)

#define URI_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(uri, v)

zend_result uri_object_write_property(zend_object *object, zend_string *property_name, zval *value);

#endif
