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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_STATIC_CACHE_H
#define ZEND_STATIC_CACHE_H

#include "php.h"

typedef bool (*zend_opcache_static_cache_safe_direct_clone_value_func_t)(
	void *context,
	zval *dst,
	zval *src
);

typedef bool (*zend_opcache_static_cache_safe_direct_value_has_unstorable_func_t)(
	void *context,
	const zval *value
);

typedef bool (*zend_opcache_static_cache_safe_direct_state_copy_func_t)(
	void *context,
	zend_object *old_object,
	zend_object *new_object,
	zend_opcache_static_cache_safe_direct_clone_value_func_t clone_value
);

typedef bool (*zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t)(
	void *context,
	const zval *value,
	zend_opcache_static_cache_safe_direct_value_has_unstorable_func_t value_has_unstorable
);

typedef bool (*zend_opcache_static_cache_safe_direct_state_serialize_func_t)(
	const zval *object,
	zval *state
);

typedef bool (*zend_opcache_static_cache_safe_direct_state_unserialize_func_t)(
	zval *object,
	zval *state
);

typedef struct _zend_opcache_static_cache_safe_direct_serializer_path zend_opcache_static_cache_safe_direct_serializer_path;
typedef struct _zend_opcache_static_cache_safe_direct_handlers zend_opcache_static_cache_safe_direct_handlers;
typedef struct _zend_opcache_static_cache_partition zend_opcache_static_cache_partition;

struct _zend_opcache_static_cache_safe_direct_serializer_path {
	bool state_includes_properties;
	zend_function *serialize;
	zend_function *unserialize;
};

struct _zend_opcache_static_cache_safe_direct_handlers {
	bool allows_custom_serializers;
	zend_opcache_static_cache_safe_direct_serializer_path serializer_path;
	zend_opcache_static_cache_safe_direct_state_copy_func_t copy;
	zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	zend_opcache_static_cache_safe_direct_state_serialize_func_t state_serialize;
	zend_opcache_static_cache_safe_direct_state_unserialize_func_t state_unserialize;
};

BEGIN_EXTERN_C()

zend_result zend_opcache_register_functions(int module_type);
zend_result zend_opcache_static_cache_minit(void);
void zend_opcache_static_cache_mshutdown(void);
zend_result zend_opcache_static_cache_rshutdown(void);
void zend_opcache_static_cache_opt_in(void);
void zend_opcache_static_cache_invalidate_all(void);
zend_opcache_static_cache_partition *zend_opcache_static_cache_partition_create(const char *name);
bool zend_opcache_static_cache_partition_startup_before_request(zend_opcache_static_cache_partition *partition);
void zend_opcache_static_cache_partition_activate(zend_opcache_static_cache_partition *partition);
void zend_opcache_static_cache_volatile_get_status(zval *return_value);
void zend_opcache_static_cache_pinned_get_status(zval *return_value);
bool zend_opcache_static_cache_volatile_is_enabled(void);
bool zend_opcache_static_cache_volatile_is_available(void);
const char *zend_opcache_static_cache_volatile_failure_reason(void);
bool zend_opcache_static_cache_pinned_is_enabled(void);
bool zend_opcache_static_cache_pinned_is_available(void);
const char *zend_opcache_static_cache_pinned_failure_reason(void);
void zend_opcache_static_cache_safe_direct_register_class(
	zend_class_entry *ce,
	const zend_opcache_static_cache_safe_direct_handlers *handlers
);

END_EXTERN_C()

#endif /* ZEND_STATIC_CACHE_H */
