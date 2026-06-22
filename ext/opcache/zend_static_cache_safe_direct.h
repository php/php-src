/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
*/

/* Helpers for the "safe direct" registry: internal value-type classes (DateTime,
 * SplFixedArray, ArrayObject, ...) register C-level copy/state handlers that the
 * static cache uses to clone and capture their instances, which a generic
 * property copy cannot reconstruct. These wrappers locate the registered base
 * class for a given class and detect subclasses that override the base's
 * serialization behaviour (and so cannot reuse the registered handlers). */

#ifndef ZEND_STATIC_CACHE_SAFE_DIRECT_H
#define ZEND_STATIC_CACHE_SAFE_DIRECT_H

#include "zend_static_cache_internal.h"

static zend_always_inline zend_class_entry *zend_opcache_serializer_find_safe_direct_cache_base(zend_class_entry *ce)
{
	zend_class_entry *base_ce = NULL;

	if (zend_opcache_static_cache_safe_direct_copy_func(ce, &base_ce) == NULL) {
		return NULL;
	}

	return base_ce;
}

static zend_always_inline bool zend_opcache_serializer_class_magic_method_changed(zend_class_entry *ce,
		zend_class_entry *base_ce, const char *name, size_t name_len)
{
	zend_function *func, *base_func;

	func = zend_hash_str_find_ptr(&ce->function_table, name, name_len);
	base_func = zend_hash_str_find_ptr(&base_ce->function_table, name, name_len);

	if (func == NULL) {
		return base_func != NULL;
	}

	if (func == base_func || func->common.scope == base_ce) {
		return false;
	}

	return true;
}

static zend_always_inline bool zend_opcache_serializer_safe_direct_cache_allows_custom_serializers(
		zend_class_entry *base_ce)
{
	return zend_opcache_static_cache_safe_direct_allows_custom_serializers(base_ce);
}

static zend_always_inline bool zend_opcache_serializer_has_safe_direct_cache_overrides(zend_class_entry *ce,
		zend_class_entry *base_ce)
{
	if (ce == base_ce) {
		return false;
	}

	if ((ce->__serialize != base_ce->__serialize || ce->__unserialize != base_ce->__unserialize) &&
		!zend_opcache_serializer_safe_direct_cache_allows_custom_serializers(base_ce)
	) {
		return true;
	}

	return zend_opcache_serializer_class_magic_method_changed(ce, base_ce, ZEND_STRL("__sleep")) ||
		zend_opcache_serializer_class_magic_method_changed(ce, base_ce, ZEND_STRL("__wakeup"))
	;
}

#endif /* ZEND_STATIC_CACHE_SAFE_DIRECT_H */
