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

#ifndef ZEND_USER_CACHE_H
#define ZEND_USER_CACHE_H

#include "php.h"

typedef bool (*zend_opcache_user_cache_safe_direct_clone_value_func_t)(
	void *context,
	zval *dst,
	zval *src
);

typedef bool (*zend_opcache_user_cache_safe_direct_value_has_unstorable_func_t)(
	void *context,
	const zval *value
);

typedef bool (*zend_opcache_user_cache_safe_direct_state_copy_func_t)(
	void *context,
	zend_object *old_object,
	zend_object *new_object,
	zend_opcache_user_cache_safe_direct_clone_value_func_t clone_value
);

typedef bool (*zend_opcache_user_cache_safe_direct_state_has_unstorable_func_t)(
	void *context,
	const zval *value,
	zend_opcache_user_cache_safe_direct_value_has_unstorable_func_t value_has_unstorable
);

typedef bool (*zend_opcache_user_cache_safe_direct_state_serialize_func_t)(
	const zval *object,
	zval *state
);

typedef bool (*zend_opcache_user_cache_safe_direct_state_unserialize_func_t)(
	zval *object,
	zval *state
);

typedef struct _zend_opcache_user_cache_partition zend_opcache_user_cache_partition;

typedef struct {
	bool prefer_request_local_prototype;
	zend_opcache_user_cache_safe_direct_state_copy_func_t copy;
	zend_opcache_user_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	zend_opcache_user_cache_safe_direct_state_serialize_func_t state_serialize;
	zend_opcache_user_cache_safe_direct_state_unserialize_func_t state_unserialize;
} zend_opcache_user_cache_safe_direct_handlers;

BEGIN_EXTERN_C()

/* Exported for SAPIs built outside the OPcache module on Windows. */
ZEND_API void zend_opcache_user_cache_opt_in(void);
ZEND_API bool zend_opcache_user_cache_startup_default_context_storage(void);
ZEND_API zend_opcache_user_cache_partition *zend_opcache_user_cache_partition_create(const char *name);
ZEND_API bool zend_opcache_user_cache_partition_startup_storage(zend_opcache_user_cache_partition *partition);
ZEND_API void zend_opcache_user_cache_activate_request_unavailable(const char *failure_reason);
ZEND_API void zend_opcache_user_cache_partition_activate(zend_opcache_user_cache_partition *partition);
void zend_opcache_user_cache_minit(void);
void zend_opcache_user_cache_mshutdown(void);
zend_result zend_opcache_user_cache_rshutdown(void);
zend_result zend_opcache_user_cache_post_deactivate(void);
void zend_opcache_user_cache_invalidate_all(void);
void zend_opcache_user_cache_safe_direct_register_class(
	zend_class_entry *ce,
	const zend_opcache_user_cache_safe_direct_handlers *handlers
);

END_EXTERN_C()

#endif /* ZEND_USER_CACHE_H */
