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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "php.h"

void spl_set_private_debug_info_property(
	const zend_class_entry *ce,
	const char *property,
	size_t property_len,
	HashTable *debug_info,
	zval *value
)
{
	zend_string *mangled_named = zend_mangle_property_name(
		ZSTR_VAL(ce->name),
		ZSTR_LEN(ce->name),
		property,
		property_len,
		/* persistent */ false
	);
	zend_hash_update(debug_info, mangled_named, value);
	zend_string_release_ex(mangled_named, /* persistent */ false);
}
