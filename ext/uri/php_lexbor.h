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

#include <lexbor/url/url.h>

lxb_url_parser_t *lexbor_init_parser(void);
void lexbor_destroy_parser(lxb_url_parser_t *parser);

void lexbor_parse_url(zend_string *url_str, zend_string *base_url_str, zval *return_value, zval *errors);

#endif
