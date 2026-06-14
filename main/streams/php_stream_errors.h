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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_STREAM_ERRORS_H
#define PHP_STREAM_ERRORS_H

#include "php.h"
#include "php_streams.h"
#include "stream_errors_decl.h"

BEGIN_EXTERN_C()

/* Error mode context options (internal C constants) */
#define PHP_STREAM_ERROR_MODE_ERROR 0
#define PHP_STREAM_ERROR_MODE_EXCEPTION 1
#define PHP_STREAM_ERROR_MODE_SILENT 2

/* Error store context options (internal C constants) */
#define PHP_STREAM_ERROR_STORE_AUTO 0
#define PHP_STREAM_ERROR_STORE_NONE 1
#define PHP_STREAM_ERROR_STORE_NON_TERM 2
#define PHP_STREAM_ERROR_STORE_TERMINAL 3
#define PHP_STREAM_ERROR_STORE_ALL 4

/* Maximum operation nesting depth */
#define PHP_STREAM_ERROR_MAX_DEPTH 1000
/* Operations pool size to prevent extra allocations */
#define PHP_STREAM_ERROR_OPERATION_POOL_SIZE 8

/* Shorthand for error code enum values */
#define PHP_STREAM_EC(name) ZEND_ENUM_StreamErrorCode_##name

/* Error code range boundaries (case_id based, ranges are [start, end)) */
#define STREAM_EC_IO_START        PHP_STREAM_EC(ReadFailed)
#define STREAM_EC_IO_END          PHP_STREAM_EC(Disabled)
#define STREAM_EC_FS_START        PHP_STREAM_EC(Disabled)
#define STREAM_EC_FS_END          PHP_STREAM_EC(NotImplemented)
#define STREAM_EC_WRAPPER_START   PHP_STREAM_EC(NotImplemented)
#define STREAM_EC_WRAPPER_END     PHP_STREAM_EC(FilterNotFound)
#define STREAM_EC_FILTER_START    PHP_STREAM_EC(FilterNotFound)
#define STREAM_EC_FILTER_END      PHP_STREAM_EC(CastFailed)
#define STREAM_EC_CAST_START      PHP_STREAM_EC(CastFailed)
#define STREAM_EC_CAST_END        PHP_STREAM_EC(NetworkSendFailed)
#define STREAM_EC_NETWORK_START   PHP_STREAM_EC(NetworkSendFailed)
#define STREAM_EC_NETWORK_END     PHP_STREAM_EC(ArchivingFailed)
#define STREAM_EC_ENCODING_START  PHP_STREAM_EC(ArchivingFailed)
#define STREAM_EC_ENCODING_END    PHP_STREAM_EC(AllocationFailed)
#define STREAM_EC_RESOURCE_START  PHP_STREAM_EC(AllocationFailed)
#define STREAM_EC_RESOURCE_END    PHP_STREAM_EC(LockFailed)
#define STREAM_EC_LOCK_START      PHP_STREAM_EC(LockFailed)
#define STREAM_EC_LOCK_END        PHP_STREAM_EC(UserspaceNotImplemented)
#define STREAM_EC_USERSPACE_START PHP_STREAM_EC(UserspaceNotImplemented)
#define STREAM_EC_USERSPACE_END   (PHP_STREAM_EC(UserspaceCallFailed) + 1)

/* Wrapper name for PHP errors */
#define PHP_STREAM_ERROR_WRAPPER_DEFAULT_NAME ":na"
#define PHP_STREAM_ERROR_WRAPPER_NAME(_wrapper) \
	(_wrapper ? _wrapper->wops->label : PHP_STREAM_ERROR_WRAPPER_DEFAULT_NAME)

/* Error entry in chain (internal linked list) */
typedef struct _php_stream_error_entry {
	zend_string *message;
	zend_enum_StreamErrorCode code;
	char *wrapper_name;
	char *param;
	char *docref;
	int severity;
	bool terminating;
	struct _php_stream_error_entry *next;
} php_stream_error_entry;

/* Active error operation */
typedef struct _php_stream_error_operation {
	php_stream_error_entry *first_error;
	php_stream_error_entry *last_error;
	uint32_t error_count;
} php_stream_error_operation;

/* Stored completed operation */
typedef struct _php_stream_stored_error {
	php_stream_error_entry *first_error;
	uint32_t error_count;
	struct _php_stream_stored_error *next;
} php_stream_stored_error;

typedef struct {
	php_stream_error_operation *current_operation;
	uint32_t operation_depth;
	php_stream_stored_error *stored_errors;
	uint32_t stored_count;
	php_stream_error_operation operation_pool[PHP_STREAM_ERROR_OPERATION_POOL_SIZE];
	php_stream_error_operation *overflow_operations;
	uint32_t overflow_capacity;
} php_stream_error_state;

/* Error operation management */
PHPAPI php_stream_error_operation *php_stream_error_operation_begin(void);
PHPAPI void php_stream_error_operation_end(php_stream_context *context);
PHPAPI void php_stream_error_operation_end_for_stream(php_stream *stream);
PHPAPI void php_stream_error_operation_abort(void);

/* State cleanup function */
PHPAPI void php_stream_error_state_cleanup(void);

/* Retrieve last stored errors as array of StreamError objects */
PHPAPI void php_stream_error_get_last(zval *return_value);

/* Clear all stored errors */
PHPAPI void php_stream_error_clear_stored(void);

/* Wrapper error reporting functions */
PHPAPI void php_stream_wrapper_error_with_name(const char *wrapper_name,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, zend_enum_StreamErrorCode code, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_wrapper_error(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_wrapper_error_param(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *param, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 9, 10);

PHPAPI void php_stream_wrapper_error_param2(php_stream_wrapper *wrapper,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, zend_enum_StreamErrorCode code, const char *param1, const char *param2,
		const char *fmt, ...) ZEND_ATTRIBUTE_FORMAT(printf, 10, 11);

PHPAPI void php_stream_error(php_stream *stream, const char *docref, int severity,
		bool terminating, zend_enum_StreamErrorCode code, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 6, 7);

/* Legacy wrapper error log functions */
PHPAPI void php_stream_wrapper_log_error(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 7, 8);

PHPAPI void php_stream_wrapper_log_error_param(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *param, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_display_wrapper_name_errors(const char *wrapper_name,
		php_stream_context *context, zend_enum_StreamErrorCode code, const char *path,
		const char *caption);

PHPAPI void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper,
		php_stream_context *context, zend_enum_StreamErrorCode code, const char *path,
		const char *caption);

PHPAPI void php_stream_tidy_wrapper_name_error_log(const char *wrapper_name);
PHPAPI void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper);

/* Convenience macros - code argument is the bare case name (e.g. RenameFailed) */
#define php_stream_wrapper_warn(wrapper, context, options, code, ...) \
	php_stream_wrapper_error(wrapper, context, NULL, options, E_WARNING, true, \
			PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_wrapper_warn_name(wrapper_name, context, options, code, ...) \
	php_stream_wrapper_error_with_name( \
			wrapper_name, context, NULL, options, E_WARNING, true, \
			PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_wrapper_warn_nt(wrapper, context, options, code, ...) \
	php_stream_wrapper_error(wrapper, context, NULL, options, E_WARNING, false, \
			PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_wrapper_notice(wrapper, context, options, code, ...) \
	php_stream_wrapper_error(wrapper, context, NULL, options, E_NOTICE, false, \
			PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_wrapper_warn_param(wrapper, context, options, code, param, ...) \
	php_stream_wrapper_error_param( \
			wrapper, context, NULL, options, E_WARNING, true, \
			PHP_STREAM_EC(code), param, __VA_ARGS__)

#define php_stream_wrapper_warn_param_nt(wrapper, context, options, code, param, ...) \
	php_stream_wrapper_error_param( \
			wrapper, context, NULL, options, E_WARNING, false, \
			PHP_STREAM_EC(code), param, __VA_ARGS__)

#define php_stream_wrapper_warn_param2(wrapper, context, options, code, param1, param2, ...) \
	php_stream_wrapper_error_param2( \
			wrapper, context, NULL, options, E_WARNING, true, \
			PHP_STREAM_EC(code), param1, param2, __VA_ARGS__)

#define php_stream_wrapper_warn_param2_nt(wrapper, context, options, code, param1, param2, ...) \
	php_stream_wrapper_error_param2( \
			wrapper, context, NULL, options, E_WARNING, false, \
			PHP_STREAM_EC(code), param1, param2, __VA_ARGS__)

#define php_stream_warn(stream, code, ...) \
	php_stream_error(stream, NULL, E_WARNING, true, PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_warn_nt(stream, code, ...) \
	php_stream_error(stream, NULL, E_WARNING, false, PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_warn_docref(stream, docref, code, ...) \
	php_stream_error(stream, docref, E_WARNING, true, PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_notice(stream, code, ...) \
	php_stream_error(stream, NULL, E_NOTICE, false, PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_fatal(stream, code, ...) \
	php_stream_error(stream, NULL, E_ERROR, true, PHP_STREAM_EC(code), __VA_ARGS__)

/* Legacy log variants */
#define php_stream_wrapper_log_warn(wrapper, context, options, code, ...) \
	php_stream_wrapper_log_error(wrapper, context, options, E_WARNING, true, \
			PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_wrapper_log_warn_nt(wrapper, context, options, code, ...) \
	php_stream_wrapper_log_error(wrapper, context, options, E_WARNING, false, \
			PHP_STREAM_EC(code), __VA_ARGS__)

#define php_stream_wrapper_log_notice(wrapper, context, options, code, ...) \
	php_stream_wrapper_log_error(wrapper, context, options, E_NOTICE, false, \
			PHP_STREAM_EC(code), __VA_ARGS__)

END_EXTERN_C()

#endif /* PHP_STREAM_ERRORS_H */
