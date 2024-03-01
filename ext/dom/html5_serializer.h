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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef HTML5_SERIALIZER_H
#define HTML5_SERIALIZER_H

#include <Zend/zend_types.h>
#include <libxml/tree.h>

typedef struct {
	zend_result (*write_string)(void *application_data, const char *buf);
	zend_result (*write_string_len)(void *application_data, const char *buf, size_t len);
	void *application_data;
} dom_html5_serialize_context;

zend_result dom_html5_serialize(dom_html5_serialize_context *ctx, const xmlNode *node);

#endif
