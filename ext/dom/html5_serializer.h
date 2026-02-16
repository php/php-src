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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef HTML5_SERIALIZER_H
#define HTML5_SERIALIZER_H

#include <Zend/zend_types.h>
#include <libxml/tree.h>
#include "private_data.h"

typedef struct {
	zend_result (*write_string)(void *application_data, const char *buf);
	zend_result (*write_string_len)(void *application_data, const char *buf, size_t len);
	void *application_data;
	php_dom_private_data *private_data;
} dom_html5_serialize_context;

zend_result dom_html5_serialize(dom_html5_serialize_context *ctx, const xmlNode *node);
zend_result dom_html5_serialize_outer(dom_html5_serialize_context *ctx, const xmlNode *node);

#endif
