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

/* Error mode context options */
#define PHP_STREAM_ERROR_MODE_ERROR      0
#define PHP_STREAM_ERROR_MODE_EXCEPTION  1
#define PHP_STREAM_ERROR_MODE_SILENT     2

/* Error store context options */
#define PHP_STREAM_ERROR_STORE_AUTO      0
#define PHP_STREAM_ERROR_STORE_NONE      1
#define PHP_STREAM_ERROR_STORE_NON_TERM  2
#define PHP_STREAM_ERROR_STORE_TERMINAL  3
#define PHP_STREAM_ERROR_STORE_ALL       4

/* Stream Error Codes*/
/* No error */
#define STREAM_ERROR_CODE_NONE                          0
/* Generic unspecified error */
#define STREAM_ERROR_CODE_GENERIC                       1
/* Stream I/O operations */
#define STREAM_ERROR_CODE_READ_FAILED                   10
#define STREAM_ERROR_CODE_WRITE_FAILED                  11
#define STREAM_ERROR_CODE_SEEK_FAILED                   12
#define STREAM_ERROR_CODE_SEEK_NOT_SUPPORTED            13
#define STREAM_ERROR_CODE_FLUSH_FAILED                  14
#define STREAM_ERROR_CODE_TRUNCATE_FAILED               15
#define STREAM_ERROR_CODE_NOT_WRITABLE                  16
#define STREAM_ERROR_CODE_NOT_READABLE                  17
/* File system operations */
#define STREAM_ERROR_CODE_NOT_FOUND                     30
#define STREAM_ERROR_CODE_PERMISSION_DENIED             31
#define STREAM_ERROR_CODE_ALREADY_EXISTS                32
#define STREAM_ERROR_CODE_INVALID_PATH                  33
#define STREAM_ERROR_CODE_PATH_TOO_LONG                 34
#define STREAM_ERROR_CODE_UNLINK_FAILED                 35
#define STREAM_ERROR_CODE_RENAME_FAILED                 36
#define STREAM_ERROR_CODE_MKDIR_FAILED                  37
#define STREAM_ERROR_CODE_RMDIR_FAILED                  38
#define STREAM_ERROR_CODE_STAT_FAILED                   39
#define STREAM_ERROR_CODE_CHMOD_FAILED                  40
#define STREAM_ERROR_CODE_CHOWN_FAILED                  41
#define STREAM_ERROR_CODE_TOUCH_FAILED                  42
#define STREAM_ERROR_CODE_INVALID_MODE                  43
#define STREAM_ERROR_CODE_MODE_NOT_SUPPORTED            44
#define STREAM_ERROR_CODE_READONLY                      45
#define STREAM_ERROR_CODE_RECURSION_DETECTED            46
/* Wrapper/protocol operations */
#define STREAM_ERROR_CODE_WRAPPER_NOT_FOUND             60
#define STREAM_ERROR_CODE_WRAPPER_DISABLED              61
#define STREAM_ERROR_CODE_PROTOCOL_UNSUPPORTED          62
#define STREAM_ERROR_CODE_WRAPPER_REGISTRATION_FAILED   63
#define STREAM_ERROR_CODE_WRAPPER_UNREGISTRATION_FAILED 64
#define STREAM_ERROR_CODE_WRAPPER_RESTORATION_FAILED    65
/* Filter operations */
#define STREAM_ERROR_CODE_FILTER_NOT_FOUND              80
#define STREAM_ERROR_CODE_FILTER_FAILED                 81
/* Cast/conversion operations */
#define STREAM_ERROR_CODE_CAST_FAILED                   90
#define STREAM_ERROR_CODE_CAST_NOT_SUPPORTED            91
#define STREAM_ERROR_CODE_MAKE_SEEKABLE_FAILED          92
#define STREAM_ERROR_CODE_BUFFERED_DATA_LOST            93
/* Network/socket operations */
#define STREAM_ERROR_CODE_NETWORK_SEND_FAILED           100
#define STREAM_ERROR_CODE_NETWORK_RECV_FAILED           101
#define STREAM_ERROR_CODE_SSL_NOT_SUPPORTED             102
#define STREAM_ERROR_CODE_SOCKET_PATH_TOO_LONG          103
#define STREAM_ERROR_CODE_OOB_NOT_SUPPORTED             104
#define STREAM_ERROR_CODE_PROTOCOL_ERROR                105
#define STREAM_ERROR_CODE_INVALID_URL                   106
#define STREAM_ERROR_CODE_INVALID_RESPONSE              107
#define STREAM_ERROR_CODE_REDIRECT_LIMIT                108
#define STREAM_ERROR_CODE_AUTH_FAILED                   109
/* Encoding/decoding operations */
#define STREAM_ERROR_CODE_ENCODING_FAILED               120
#define STREAM_ERROR_CODE_INVALID_FORMAT                121
/* Resource/allocation operations */
#define STREAM_ERROR_CODE_ALLOCATION_FAILED             130
#define STREAM_ERROR_CODE_TEMPORARY_FILE_FAILED         131
/* Locking operations */
#define STREAM_ERROR_CODE_LOCK_FAILED                   140
#define STREAM_ERROR_CODE_LOCK_NOT_SUPPORTED            141
/* Userspace stream operations */
#define STREAM_ERROR_CODE_USERSPACE_NOT_IMPLEMENTED     150
#define STREAM_ERROR_CODE_USERSPACE_INVALID_RETURN      151

/* Stored error entry */
typedef struct {
    zend_string *message;
    int code;
    const char *wrapper_name;  /* Points to wrapper->wops->label, no need to duplicate */
    const char *param;         /* Points to passed string, caller manages lifetime for storage */
    int severity;
    bool terminal;
} php_stream_error_entry;

/* Main error reporting functions */
PHPAPI void php_stream_wrapper_error(
    php_stream_wrapper *wrapper,
    php_stream_context *context,
    int options,
    int severity,
    bool terminal,
    int code,
    const char *fmt,
    ...
) ZEND_ATTRIBUTE_FORMAT(printf, 7, 8);

PHPAPI void php_stream_wrapper_error_param(
    php_stream_wrapper *wrapper,
    php_stream_context *context,
    int options,
    int severity,
    bool terminal,
    int code,
    const char *param,
    const char *fmt,
    ...
) ZEND_ATTRIBUTE_FORMAT(printf, 8, 9);

PHPAPI void php_stream_wrapper_error_param2(
    php_stream_wrapper *wrapper,
    php_stream_context *context,
    int options,
    int severity,
    bool terminal,
    int code,
    const char *param1,
    const char *param2,
    const char *fmt,
    ...
) ZEND_ATTRIBUTE_FORMAT(printf, 9, 10);

PHPAPI void php_stream_error(
    php_stream *stream,
    int severity,
    bool terminal,
    int code,
    const char *fmt,
    ...
) ZEND_ATTRIBUTE_FORMAT(printf, 5, 6);

/* Legacy wrapper error log - updated API */
PHPAPI void php_stream_wrapper_log_error(
    const php_stream_wrapper *wrapper,
    int options,
    int severity,
    bool terminal,
    int code,
    const char *fmt,
    ...
) ZEND_ATTRIBUTE_FORMAT(printf, 6, 7);

PHPAPI void php_stream_wrapper_log_error_param(
    const php_stream_wrapper *wrapper,
    int options,
    int severity,
    bool terminal,
    int code,
    const char *param,
    const char *fmt,
    ...
) ZEND_ATTRIBUTE_FORMAT(printf, 7, 8);

PHPAPI void php_stream_display_wrapper_errors(
    php_stream_wrapper *wrapper,
    const char *path,
    const char *caption
);

PHPAPI void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper);

void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper, const char *path, const char *caption);
void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper);

/* Convenience macros */
#define php_stream_wrapper_warn(wrapper, context, options, code, ...) \
    php_stream_wrapper_error(wrapper, context, options, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_wrapper_warn_nt(wrapper, context, options, code, ...) \
    php_stream_wrapper_error(wrapper, context, options, E_WARNING, false, code, __VA_ARGS__)

#define php_stream_wrapper_notice(wrapper, context, options, code, ...) \
    php_stream_wrapper_error(wrapper, context, options, E_NOTICE, false, code, __VA_ARGS__)

#define php_stream_wrapper_warn_param(wrapper, context, options, code, param, ...) \
    php_stream_wrapper_error_param(wrapper, context, options, E_WARNING, true, code, param, __VA_ARGS__)

#define php_stream_warn(stream, code, ...) \
    php_stream_error(stream, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_warn_nt(stream, code, ...) \
    php_stream_error(stream, E_WARNING, false, code, __VA_ARGS__)

#define php_stream_notice(stream, code, ...) \
    php_stream_error(stream, E_NOTICE, false, code, __VA_ARGS__)

#define php_stream_fatal(stream, code, ...) \
    php_stream_error(stream, E_ERROR, true, code, __VA_ARGS__)

/* Legacy log variants */
#define php_stream_wrapper_log_warn(wrapper, options, code, ...) \
    php_stream_wrapper_log_error(wrapper, options, E_WARNING, true, code, __VA_ARGS__)

#define php_stream_wrapper_log_warn_nt(wrapper, options, code, ...) \
    php_stream_wrapper_log_error(wrapper, options, E_WARNING, false, code, __VA_ARGS__)

#define php_stream_wrapper_log_notice(wrapper, options, code, ...) \
    php_stream_wrapper_log_error(wrapper, options, E_NOTICE, false, code, __VA_ARGS__)

END_EXTERN_C()

#endif /* PHP_STREAM_ERRORS_H */
