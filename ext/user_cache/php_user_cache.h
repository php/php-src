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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_USER_CACHE_H
#define PHP_USER_CACHE_H

#include "php.h"

/* Public API for extensions, SAPIs and embedders. */
/* Keep in sync with user_cache_availability_enum_case(). */
typedef enum {
	PHP_USER_CACHE_REASON_NONE = 0,
	PHP_USER_CACHE_REASON_DISABLED_BY_INI,
	PHP_USER_CACHE_REASON_SHM_INIT_FAILED,
	PHP_USER_CACHE_REASON_SAPI_NOT_ENABLED,
	PHP_USER_CACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER,
	PHP_USER_CACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER,
	PHP_USER_CACHE_REASON_CGI_BOUNDARY_UNAVAILABLE,
	PHP_USER_CACHE_REASON_LSAPI_BOUNDARY_UNAVAILABLE,
	PHP_USER_CACHE_REASON_REQUEST_SHUTDOWN
} php_user_cache_reason;

/* Handlers for copying native object state without invoking user code.
 * All handlers take the destination before the source; callers pass a NULL
 * clone_value callback to probe copy capability, so copy handlers must
 * return false without side effects in that case. */
typedef bool (*php_user_cache_safe_direct_clone_value_func_t)(
	void *context,
	zval *dst,
	zval *src
);

typedef bool (*php_user_cache_safe_direct_value_has_unstorable_func_t)(
	void *context,
	const zval *value
);

typedef bool (*php_user_cache_safe_direct_state_copy_func_t)(
	void *context,
	zend_object *new_object,
	zend_object *old_object,
	php_user_cache_safe_direct_clone_value_func_t clone_value
);

typedef bool (*php_user_cache_safe_direct_state_has_unstorable_func_t)(
	void *context,
	const zval *value,
	php_user_cache_safe_direct_value_has_unstorable_func_t value_has_unstorable
);

typedef bool (*php_user_cache_safe_direct_state_serialize_func_t)(
	zval *state,
	const zval *object
);

typedef bool (*php_user_cache_safe_direct_state_unserialize_func_t)(
	zval *object,
	zval *state
);

typedef struct {
	bool prefer_request_local_prototype;
	php_user_cache_safe_direct_state_copy_func_t copy;
	php_user_cache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	php_user_cache_safe_direct_state_serialize_func_t state_serialize;
	php_user_cache_safe_direct_state_unserialize_func_t state_unserialize;
} php_user_cache_safe_direct_handlers;

typedef struct _php_user_cache_partition php_user_cache_partition;

BEGIN_EXTERN_C()

/* The handler structure is copied and may be temporary. */
ZEND_API void php_user_cache_safe_direct_register_class(
	zend_class_entry *ce,
	const php_user_cache_safe_direct_handlers *handlers
);
/* SAPI and embedder integration. */
ZEND_API void php_user_cache_opt_in(void);
ZEND_API bool php_user_cache_startup_default_context_storage(void);
ZEND_API php_user_cache_partition *php_user_cache_partition_create(const char *name);
ZEND_API bool php_user_cache_partition_startup_storage(php_user_cache_partition *partition);
ZEND_API void php_user_cache_partition_activate(php_user_cache_partition *partition);
/* Activate a request partition using DOCUMENT_ROOT or SERVER_NAME. */
ZEND_API void php_user_cache_activate_boundary_partition(
	const char *sapi_prefix,
	const char *(*get_env)(const char *name),
	void (*log_message)(const char *message),
	php_user_cache_reason failure_reason
);
ZEND_API void php_user_cache_boundary_partitions_shutdown(void);

#ifdef ZTS
/* Called by php_tsrm_startup_ex() before module startup. */
size_t php_user_cache_globals_size(void);
void php_user_cache_globals_startup(void);
#endif

extern zend_module_entry user_cache_module_entry;

#define phpext_user_cache_ptr &user_cache_module_entry

END_EXTERN_C()

#endif /* PHP_USER_CACHE_H */
