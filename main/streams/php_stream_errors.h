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

/* Error code ranges */
#define STREAM_ERROR_CODE_IO_START 10
#define STREAM_ERROR_CODE_IO_END 30
#define STREAM_ERROR_CODE_FILESYSTEM_START 30
#define STREAM_ERROR_CODE_FILESYSTEM_END 70
#define STREAM_ERROR_CODE_WRAPPER_START 70
#define STREAM_ERROR_CODE_WRAPPER_END 90
#define STREAM_ERROR_CODE_FILTER_START 90
#define STREAM_ERROR_CODE_FILTER_END 100
#define STREAM_ERROR_CODE_CAST_START 100
#define STREAM_ERROR_CODE_CAST_END 110
#define STREAM_ERROR_CODE_NETWORK_START 110
#define STREAM_ERROR_CODE_NETWORK_END 130
#define STREAM_ERROR_CODE_ENCODING_START 130
#define STREAM_ERROR_CODE_ENCODING_END 140
#define STREAM_ERROR_CODE_RESOURCE_START 140
#define STREAM_ERROR_CODE_RESOURCE_END 150
#define STREAM_ERROR_CODE_LOCK_START 150
#define STREAM_ERROR_CODE_LOCK_END 160
#define STREAM_ERROR_CODE_USERSPACE_START 160
#define STREAM_ERROR_CODE_USERSPACE_END 170

/* X-macro defining all error codes */
#define PHP_STREAM_ERROR_CODES(V) \
	/* General errors */ \
	V(NONE, None, 0) \
	V(GENERIC, Generic, 1) \
	/* I/O operation errors (10-29) */ \
	V(READ_FAILED, ReadFailed, 10) \
	V(WRITE_FAILED, WriteFailed, 11) \
	V(SEEK_FAILED, SeekFailed, 12) \
	V(SEEK_NOT_SUPPORTED, SeekNotSupported, 13) \
	V(FLUSH_FAILED, FlushFailed, 14) \
	V(TRUNCATE_FAILED, TruncateFailed, 15) \
	V(CONNECT_FAILED, ConnectFailed, 16) \
	V(BIND_FAILED, BindFailed, 17) \
	V(LISTEN_FAILED, ListenFailed, 18) \
	V(NOT_WRITABLE, NotWritable, 19) \
	V(NOT_READABLE, NotReadable, 20) \
	/* File system operations (30-69) */ \
	V(DISABLED, Disabled, 30) \
	V(NOT_FOUND, NotFound, 31) \
	V(PERMISSION_DENIED, PermissionDenied, 32) \
	V(ALREADY_EXISTS, AlreadyExists, 33) \
	V(INVALID_PATH, InvalidPath, 34) \
	V(PATH_TOO_LONG, PathTooLong, 35) \
	V(OPEN_FAILED, OpenFailed, 36) \
	V(CREATE_FAILED, CreateFailed, 37) \
	V(DUP_FAILED, DupFailed, 38) \
	V(UNLINK_FAILED, UnlinkFailed, 39) \
	V(RENAME_FAILED, RenameFailed, 40) \
	V(MKDIR_FAILED, MkdirFailed, 41) \
	V(RMDIR_FAILED, RmdirFailed, 42) \
	V(STAT_FAILED, StatFailed, 43) \
	V(META_FAILED, MetaFailed, 44) \
	V(CHMOD_FAILED, ChmodFailed, 45) \
	V(CHOWN_FAILED, ChownFailed, 46) \
	V(COPY_FAILED, CopyFailed, 47) \
	V(TOUCH_FAILED, TouchFailed, 48) \
	V(INVALID_MODE, InvalidMode, 49) \
	V(INVALID_META, InvalidMeta, 50) \
	V(MODE_NOT_SUPPORTED, ModeNotSupported, 51) \
	V(READONLY, Readonly, 52) \
	V(RECURSION_DETECTED, RecursionDetected, 53) \
	/* Wrapper/protocol operations (70-89) */ \
	V(NOT_IMPLEMENTED, NotImplemented, 70) \
	V(NO_OPENER, NoOpener, 71) \
	V(PERSISTENT_NOT_SUPPORTED, PersistentNotSupported, 72) \
	V(WRAPPER_NOT_FOUND, WrapperNotFound, 73) \
	V(WRAPPER_DISABLED, WrapperDisabled, 74) \
	V(PROTOCOL_UNSUPPORTED, ProtocolUnsupported, 75) \
	V(WRAPPER_REGISTRATION_FAILED, WrapperRegistrationFailed, 76) \
	V(WRAPPER_UNREGISTRATION_FAILED, WrapperUnregistrationFailed, 77) \
	V(WRAPPER_RESTORATION_FAILED, WrapperRestorationFailed, 78) \
	/* Filter operations (90-99) */ \
	V(FILTER_NOT_FOUND, FilterNotFound, 90) \
	V(FILTER_FAILED, FilterFailed, 91) \
	/* Cast/conversion operations (100-109) */ \
	V(CAST_FAILED, CastFailed, 100) \
	V(CAST_NOT_SUPPORTED, CastNotSupported, 101) \
	V(MAKE_SEEKABLE_FAILED, MakeSeekableFailed, 102) \
	V(BUFFERED_DATA_LOST, BufferedDataLost, 103) \
	/* Network/socket operations (110-129) */ \
	V(NETWORK_SEND_FAILED, NetworkSendFailed, 110) \
	V(NETWORK_RECV_FAILED, NetworkRecvFailed, 111) \
	V(SSL_NOT_SUPPORTED, SslNotSupported, 112) \
	V(RESUMPTION_FAILED, ResumptionFailed, 113) \
	V(SOCKET_PATH_TOO_LONG, SocketPathTooLong, 114) \
	V(OOB_NOT_SUPPORTED, OobNotSupported, 115) \
	V(PROTOCOL_ERROR, ProtocolError, 116) \
	V(INVALID_URL, InvalidUrl, 117) \
	V(INVALID_RESPONSE, InvalidResponse, 118) \
	V(INVALID_HEADER, InvalidHeader, 119) \
	V(INVALID_PARAM, InvalidParam, 120) \
	V(REDIRECT_LIMIT, RedirectLimit, 121) \
	V(AUTH_FAILED, AuthFailed, 122) \
	/* Encoding/decoding/archiving operations (130-139) */ \
	V(ARCHIVING_FAILED, ArchivingFailed, 130) \
	V(ENCODING_FAILED, EncodingFailed, 131) \
	V(DECODING_FAILED, DecodingFailed, 132) \
	V(INVALID_FORMAT, InvalidFormat, 133) \
	/* Resource/allocation operations (140-149) */ \
	V(ALLOCATION_FAILED, AllocationFailed, 140) \
	V(TEMPORARY_FILE_FAILED, TemporaryFileFailed, 141) \
	/* Locking operations (150-159) */ \
	V(LOCK_FAILED, LockFailed, 150) \
	V(LOCK_NOT_SUPPORTED, LockNotSupported, 151) \
	/* Userspace stream operations (160-169) */ \
	V(USERSPACE_NOT_IMPLEMENTED, UserspaceNotImplemented, 160) \
	V(USERSPACE_INVALID_RETURN, UserspaceInvalidReturn, 161) \
	V(USERSPACE_CALL_FAILED, UserspaceCallFailed, 162)

/* Generate C enum for internal use */
typedef enum _StreamErrorCode {
#define V(uc_name, name, val) STREAM_ERROR_CODE_##uc_name = val,
	PHP_STREAM_ERROR_CODES(V)
#undef V
} StreamErrorCode;

/* Wrapper name for PHP errors */
#define PHP_STREAM_ERROR_WRAPPER_DEFAULT_NAME ":na"
#define PHP_STREAM_ERROR_WRAPPER_NAME(_wrapper) \
	(_wrapper ? _wrapper->wops->label : PHP_STREAM_ERROR_WRAPPER_DEFAULT_NAME)

/* Error entry in chain */
typedef struct _php_stream_error_entry {
	zend_string *message;
	StreamErrorCode code;
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
	struct _php_stream_stored_error *next;
} php_stream_stored_error;

typedef struct {
	/* Stack of active operations (LIFO for nesting) */
	php_stream_error_operation *current_operation;
	uint32_t operation_depth;
	/* List of completed/stored operations (most recent first) */
	php_stream_stored_error *stored_errors;
	uint32_t stored_count;
	/* Pre-allocated operation pool */
	php_stream_error_operation operation_pool[PHP_STREAM_ERROR_OPERATION_POOL_SIZE];
	/* Overflow operations (for deep nesting beyond pool size) */
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

/* Error object creation */
PHPAPI void php_stream_error_create_object(zval *zv, php_stream_error_entry *entry);

/* Wrapper error reporting functions */
PHPAPI void php_stream_wrapper_error_with_name(const char *wrapper_name,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, int code, const char *fmt, ...) ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_wrapper_error(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating, int code, const char *fmt,
		...) ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_wrapper_error_param(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating, int code,
		const char *param, const char *fmt, ...) ZEND_ATTRIBUTE_FORMAT(printf, 9, 10);

PHPAPI void php_stream_wrapper_error_param2(php_stream_wrapper *wrapper,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, int code, const char *param1, const char *param2, const char *fmt, ...)
		ZEND_ATTRIBUTE_FORMAT(printf, 10, 11);

PHPAPI void php_stream_error(php_stream *stream, const char *docref, int severity, bool terminating,
		int code, const char *fmt, ...) ZEND_ATTRIBUTE_FORMAT(printf, 6, 7);

/* Legacy wrapper error log functions */
PHPAPI void php_stream_wrapper_log_error(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating, int code,
		const char *fmt, ...) ZEND_ATTRIBUTE_FORMAT(printf, 7, 8);

PHPAPI void php_stream_wrapper_log_error_param(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating, int code,
		const char *param, const char *fmt, ...) ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_display_wrapper_name_errors(const char *wrapper_name,
		php_stream_context *context, int code, const char *path, const char *caption);

PHPAPI void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper,
		php_stream_context *context, int code, const char *path, const char *caption);

PHPAPI void php_stream_tidy_wrapper_name_error_log(const char *wrapper_name);
PHPAPI void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper);

/* Convenience macros */
#define php_stream_wrapper_warn(wrapper, context, options, code, ...) \
	php_stream_wrapper_error(wrapper, context, NULL, options, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_wrapper_warn_name(wrapper_name, context, options, code, ...) \
	php_stream_wrapper_error_with_name( \
			wrapper_name, context, NULL, options, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_wrapper_warn_nt(wrapper, context, options, code, ...) \
	php_stream_wrapper_error(wrapper, context, NULL, options, E_WARNING, false, code, __VA_ARGS__)

#define php_stream_wrapper_notice(wrapper, context, options, code, ...) \
	php_stream_wrapper_error(wrapper, context, NULL, options, E_NOTICE, false, code, __VA_ARGS__)

#define php_stream_wrapper_warn_param(wrapper, context, options, code, param, ...) \
	php_stream_wrapper_error_param( \
			wrapper, context, NULL, options, E_WARNING, true, code, param, __VA_ARGS__)

#define php_stream_wrapper_warn_param_nt(wrapper, context, options, code, param, ...) \
	php_stream_wrapper_error_param( \
			wrapper, context, NULL, options, E_WARNING, false, code, param, __VA_ARGS__)

#define php_stream_wrapper_warn_param2(wrapper, context, options, code, param1, param2, ...) \
	php_stream_wrapper_error_param2( \
			wrapper, context, NULL, options, E_WARNING, true, code, param1, param2, __VA_ARGS__)

#define php_stream_wrapper_warn_param2_nt(wrapper, context, options, code, param1, param2, ...) \
	php_stream_wrapper_error_param2( \
			wrapper, context, NULL, options, E_WARNING, false, code, param1, param2, __VA_ARGS__)

#define php_stream_warn(stream, code, ...) \
	php_stream_error(stream, NULL, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_warn_nt(stream, code, ...) \
	php_stream_error(stream, NULL, E_WARNING, false, code, __VA_ARGS__)

#define php_stream_warn_docref(stream, docref, code, ...) \
	php_stream_error(stream, docref, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_notice(stream, code, ...) \
	php_stream_error(stream, NULL, E_NOTICE, false, code, __VA_ARGS__)

#define php_stream_fatal(stream, code, ...) \
	php_stream_error(stream, NULL, E_ERROR, true, code, __VA_ARGS__)

/* Legacy log variants */
#define php_stream_wrapper_log_warn(wrapper, context, options, code, ...) \
	php_stream_wrapper_log_error(wrapper, context, options, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_wrapper_log_warn_nt(wrapper, context, options, code, ...) \
	php_stream_wrapper_log_error(wrapper, context, options, E_WARNING, false, code, __VA_ARGS__)

#define php_stream_wrapper_log_notice(wrapper, context, options, code, ...) \
	php_stream_wrapper_log_error(wrapper, context, options, E_NOTICE, false, code, __VA_ARGS__)

END_EXTERN_C()

#endif /* PHP_STREAM_ERRORS_H */
