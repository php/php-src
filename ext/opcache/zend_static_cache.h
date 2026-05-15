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

#ifndef ZEND_OPCACHE_STATIC_CACHE_SAFE_DIRECT_HANDLERS_FWD
# define ZEND_OPCACHE_STATIC_CACHE_SAFE_DIRECT_HANDLERS_FWD
typedef struct _zend_opcache_static_cache_safe_direct_handlers zend_opcache_static_cache_safe_direct_handlers;
#endif

#ifndef ZEND_OPCACHE_STATIC_CACHE_SAFE_DIRECT_HANDLERS_DEFINED
# define ZEND_OPCACHE_STATIC_CACHE_SAFE_DIRECT_HANDLERS_DEFINED
struct _zend_opcache_static_cache_safe_direct_handlers {
	bool allows_custom_serializers;
	zend_opcache_static_cache_safe_direct_state_copy_func_t copy;
	zend_opcache_static_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	zend_opcache_static_cache_safe_direct_state_serialize_func_t state_serialize;
	zend_opcache_static_cache_safe_direct_state_unserialize_func_t state_unserialize;
};
#endif

BEGIN_EXTERN_C()

zend_result zend_opcache_register_functions(int module_type);
zend_result zend_opcache_static_cache_minit(void);
void zend_opcache_static_cache_mshutdown(void);
zend_result zend_opcache_static_cache_rshutdown(void);
void zend_opcache_static_cache_invalidate_all(void);
void zend_opcache_static_cache_volatile_get_status(zval *return_value);
void zend_opcache_static_cache_persistent_get_status(zval *return_value);
bool zend_opcache_static_cache_volatile_is_enabled(void);
bool zend_opcache_static_cache_volatile_is_available(void);
const char *zend_opcache_static_cache_volatile_failure_reason(void);
bool zend_opcache_static_cache_persistent_is_enabled(void);
bool zend_opcache_static_cache_persistent_is_available(void);
const char *zend_opcache_static_cache_persistent_failure_reason(void);

END_EXTERN_C()

#endif /* ZEND_STATIC_CACHE_H */
