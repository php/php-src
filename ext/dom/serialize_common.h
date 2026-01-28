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

#ifndef SERIALIZE_COMMON_H
#define SERIALIZE_COMMON_H

#include <Zend/zend_types.h>
#include <libxml/tree.h>

/* The lengths are merely here for optimization purposes, this cannot be used to compare substrings. */
static zend_always_inline bool dom_local_name_compare_ex(const xmlNode *node, const char *tag, size_t tag_length, size_t name_length)
{
	return name_length == tag_length && memcmp((const char *) node->name, tag, name_length + 1) == 0;
}

#endif
