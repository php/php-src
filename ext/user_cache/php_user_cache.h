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

/* Keeps the per-fetch hot path (key lookup, graph decode, safe-direct
 * handlers) clustered in .text.hot: the repeated-fetch working set spans
 * several translation units and is sensitive to I-cache placement. */
#if (defined(__GNUC__) && ZEND_GCC_VERSION >= 4003) || __has_attribute(hot)
# define PHP_UCACHE_HOT __attribute__((hot))
#else
# define PHP_UCACHE_HOT
#endif

/* Public API for extensions, SAPIs and embedders. */
/* Keep in sync with ucache_availability_enum_case(). */
typedef enum {
	PHP_UCACHE_REASON_NONE = 0,
	PHP_UCACHE_REASON_DISABLED_BY_INI,
	PHP_UCACHE_REASON_SHM_INIT_FAILED,
	PHP_UCACHE_REASON_SAPI_NOT_ENABLED,
	PHP_UCACHE_REASON_BACKEND_NOT_INITIALIZED_BEFORE_WORKER,
	PHP_UCACHE_REASON_BACKEND_INITIALIZED_AFTER_WORKER,
	PHP_UCACHE_REASON_CGI_BOUNDARY_UNAVAILABLE,
	PHP_UCACHE_REASON_APACHE_BOUNDARY_UNAVAILABLE,
	PHP_UCACHE_REASON_LSAPI_BOUNDARY_UNAVAILABLE,
	PHP_UCACHE_REASON_REQUEST_SHUTDOWN
} php_ucache_reason_t;

/* Handlers for copying native object state without invoking user code.
 * All handlers take the destination before the source; copy() and
 * state_unserialize() always receive a freshly created, empty destination
 * object. A clone_value callback always writes *dst (IS_UNDEF on failure).
 * A state handler that fails may leave its destination partially built: the
 * caller owns and releases it. */
typedef bool (*php_ucache_safe_direct_clone_value_func_t)(
		void *ctx,
		zval *dst,
		zval *src);

typedef bool (*php_ucache_safe_direct_value_has_unstorable_func_t)(
		void *ctx,
		const zval *value);

typedef bool (*php_ucache_safe_direct_state_copy_func_t)(
		void *ctx,
		zend_object *new_object,
		zend_object *old_object,
		php_ucache_safe_direct_clone_value_func_t clone_value);

typedef bool (*php_ucache_safe_direct_state_has_unstorable_func_t)(
		void *ctx,
		const zval *value,
		php_ucache_safe_direct_value_has_unstorable_func_t value_has_unstorable);

typedef bool (*php_ucache_safe_direct_state_serialize_func_t)(
		zval *state,
		const zval *object);

typedef bool (*php_ucache_safe_direct_state_unserialize_func_t)(
		zval *object,
		zval *state);

typedef struct {
	bool prefer_request_local_prototype;
	php_ucache_safe_direct_state_copy_func_t copy;
	php_ucache_safe_direct_state_has_unstorable_func_t state_has_unstorable;
	php_ucache_safe_direct_state_serialize_func_t state_serialize;
	php_ucache_safe_direct_state_unserialize_func_t state_unserialize;
} php_ucache_safe_direct_handlers_t;

typedef struct _php_ucache_partition php_ucache_partition_t;

BEGIN_EXTERN_C()

/* The handler structure is copied and may be temporary. */
ZEND_API void php_ucache_safe_direct_register_class(
		zend_class_entry *ce,
		const php_ucache_safe_direct_handlers_t *handlers);
/* SAPI and embedder integration. */
ZEND_API void php_ucache_opt_in(void);
ZEND_API bool php_ucache_startup_default_context_storage(void);
ZEND_API php_ucache_partition_t *php_ucache_partition_create(const char *name);
ZEND_API bool php_ucache_partition_startup_storage(php_ucache_partition_t *partition);
ZEND_API void php_ucache_partition_activate(php_ucache_partition_t *partition);
/* Activate a request partition keyed by a caller-composed boundary id; the
 * id does not need to be NUL-terminated. */
ZEND_API void php_ucache_activate_boundary_partition_by_id(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		php_ucache_reason_t failure_reason);
/* Activate a request partition using DOCUMENT_ROOT or SERVER_NAME. */
ZEND_API void php_ucache_activate_boundary_partition(
		const char *sapi_prefix,
		const char *(*get_env)(const char *name),
		php_ucache_reason_t failure_reason);

#ifdef ZTS
/* Called by php_tsrm_startup_ex() before module startup. */
size_t php_ucache_globals_size(void);
void php_ucache_globals_startup(void);
#endif

extern zend_module_entry user_cache_module_entry;

#define phpext_user_cache_ptr &user_cache_module_entry

END_EXTERN_C()

#endif /* PHP_USER_CACHE_H */
