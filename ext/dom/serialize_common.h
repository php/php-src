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

#ifndef SERIALIZE_COMMON_H
#define SERIALIZE_COMMON_H

#include <Zend/zend_types.h>
#include <libxml/tree.h>

static zend_always_inline bool dom_local_name_compare_ex(const xmlNode *node, const char *tag, size_t tag_length, size_t name_length)
{
	return name_length == tag_length && zend_binary_strcmp((const char *) node->name, name_length, tag, tag_length) == 0;
}

#endif
