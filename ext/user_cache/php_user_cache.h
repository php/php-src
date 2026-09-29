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

#ifndef PHP_UCACHE_H
#define PHP_UCACHE_H

#include "php.h"

#if (defined(__GNUC__) && ZEND_GCC_VERSION >= 4003) || __has_attribute(hot)
# define PHP_UCACHE_HOT __attribute__((hot))
#else
# define PHP_UCACHE_HOT
#endif

#define PHP_UCACHE_HOST_API_VERSION 1

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
	PHP_UCACHE_REASON_REQ_SHUTDOWN
} php_ucache_reason;

typedef bool (*php_ucache_safe_direct_clone_val_func_t)(
		void *ctx,
		zval *dst,
		zval *src);

typedef bool (*php_ucache_safe_direct_copy_func_t)(
		void *ctx,
		zend_object *new_obj,
		zend_object *old_obj,
		php_ucache_safe_direct_clone_val_func_t clone_val);

typedef bool (*php_ucache_safe_direct_state_serialize_func_t)(
		zval *state,
		const zval *obj);

typedef bool (*php_ucache_safe_direct_state_unserialize_func_t)(
		zval *obj,
		zval *state);

typedef bool (*php_ucache_safe_direct_is_internal_prop_func_t)(
		const zend_object *obj,
		const zend_string *name);

typedef struct {
	bool prefer_req_local_proto;
	php_ucache_safe_direct_copy_func_t copy;
	php_ucache_safe_direct_state_serialize_func_t state_serialize;
	php_ucache_safe_direct_state_unserialize_func_t state_unserialize;
	php_ucache_safe_direct_is_internal_prop_func_t is_internal_prop;
} php_ucache_safe_direct_handlers;

typedef struct _php_ucache_partition php_ucache_partition;

typedef enum {
	PHP_UCACHE_MODE_REQ,
	PHP_UCACHE_MODE_PERSISTENT,
	PHP_UCACHE_MODE_HOST_MANAGED
} php_ucache_mode;

typedef enum {
	PHP_UCACHE_EXEC_REQ,
	PHP_UCACHE_EXEC_PERSISTENT
} php_ucache_exec_mode;

typedef enum {
	PHP_UCACHE_SCOPE_COMPLETE,
	PHP_UCACHE_SCOPE_ABORTED
} php_ucache_scope_outcome;

typedef uint64_t php_ucache_scope_token;

BEGIN_EXTERN_C()

ZEND_API void php_ucache_safe_direct_register_class(
		zend_class_entry *ce,
		const php_ucache_safe_direct_handlers *handlers);
ZEND_API zend_result php_ucache_opt_in(php_ucache_mode mode);
ZEND_API bool php_ucache_is_enabled_by_ini(void);
ZEND_API zend_result php_ucache_exec_prepare(
		php_ucache_exec_mode mode,
		php_ucache_partition *partition);
ZEND_API zend_result php_ucache_exec_cancel_prepare(void);
ZEND_API zend_result php_ucache_logical_req_begin(php_ucache_scope_token *token);
ZEND_API zend_result php_ucache_logical_req_end(
		php_ucache_scope_token token,
		php_ucache_scope_outcome outcome);
ZEND_API bool php_ucache_startup_default_ctx_storage(void);
ZEND_API php_ucache_partition *php_ucache_partition_create(const char *name);
ZEND_API void php_ucache_partition_set_max_procs(php_ucache_partition *partition, uint32_t max_procs);
ZEND_API bool php_ucache_partition_startup_storage(php_ucache_partition *partition);
ZEND_API void php_ucache_partition_activate(php_ucache_partition *partition);
ZEND_API void php_ucache_activate_boundary_partition_by_id(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		php_ucache_reason failure_reason);
ZEND_API void php_ucache_activate_boundary_partition(
		const char *sapi_prefix,
		const char *doc_root,
		php_ucache_reason failure_reason);

#ifdef ZTS
size_t php_ucache_globals_size(void);
void php_ucache_globals_startup(void);
#endif

extern zend_module_entry user_cache_module_entry;

#define phpext_user_cache_ptr &user_cache_module_entry

END_EXTERN_C()

#endif /* PHP_UCACHE_H */
