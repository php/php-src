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

#include "php_uri_common.h"
#include "lexbor/url/url.h"

extern const uri_handler_t lexbor_uri_handler;

lxb_url_t *lexbor_parse_uri_ex(const zend_string *uri_str, const lxb_url_t *lexbor_base_url, zval *errors, bool silent);

zend_result lexbor_request_init(void);
void lexbor_request_shutdown(void);

#endif
