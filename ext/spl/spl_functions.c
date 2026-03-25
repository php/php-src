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
