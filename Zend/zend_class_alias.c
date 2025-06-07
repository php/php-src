/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Daniel Scherzer <daniel.e.scherzer@gmail.com>               |
   +----------------------------------------------------------------------+
*/

#include "zend_class_alias.h"
#include "zend_errors.h"
#include "zend_string.h"
#include "zend.h"

zend_class_alias * zend_class_alias_init(zend_class_entry *ce) {
	zend_class_alias *alias = malloc(sizeof(zend_class_alias));
	// refcount field is only there for compatibility with other structures
	GC_SET_REFCOUNT(alias, 1);

	alias->ce = ce;
	alias->name = NULL;
	alias->attributes = NULL;
	alias->alias_flags = 0;

	return alias;
}

ZEND_COLD zend_result ZEND_FASTCALL get_deprecation_suffix_from_attribute(HashTable *attributes, zend_class_entry* scope, zend_string **message_suffix);

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_deprecated_class_alias(const zend_class_alias *alias) {
	zend_string *message_suffix = ZSTR_EMPTY_ALLOC();

	if (get_deprecation_suffix_from_attribute(alias->attributes, NULL, &message_suffix) == FAILURE) {
		return;
	}

	zend_error_unchecked(E_USER_DEPRECATED, "Alias %s for class %s is deprecated%S",
		ZSTR_VAL(alias->name),
		ZSTR_VAL(alias->ce->name),
		message_suffix
	);

	zend_string_release(message_suffix);
}
