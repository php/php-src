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

/* Error code definition for registration */
typedef struct {
    int code;
    const char *name;
} php_stream_error_code_def;

/* Stored error entry */
typedef struct {
    zend_string *message;
    int code;
    const char *wrapper_name;  /* Points to wrapper->wops->label, no need to duplicate */
    const char *param;         /* Points to passed string, caller manages lifetime for storage */
    int severity;
    bool terminal;
} php_stream_error_entry;

/* Sentinel for error code array termination */
#define PHP_STREAM_ERROR_CODE_END {0, NULL}

/* Error code registration */
PHPAPI void php_stream_wrapper_register_error_codes(
    php_stream_wrapper *wrapper,
    const php_stream_error_code_def *codes
);

PHPAPI const char *php_stream_wrapper_get_error_name(
    php_stream_wrapper *wrapper,
    int code
);

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
