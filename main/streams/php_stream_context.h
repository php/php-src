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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* Stream context and status notification related definitions */

/* callback for status notifications */
typedef void (*php_stream_notification_func)(php_stream_context *context,
		int notifycode, int severity,
		char *xmsg, int xcode,
		size_t bytes_sofar, size_t bytes_max,
		void * ptr);

#define PHP_STREAM_NOTIFIER_PROGRESS	1

/* TODO: Remove dependence on ext/standard/file.h for the default context global */
#define php_stream_context_get_default(without_context) \
	(without_context) ? NULL : FG(default_context) ? FG(default_context) : \
		(FG(default_context) = php_stream_context_alloc())

/* Attempt to fetch context from the zval passed,
   If no context was passed, use the default context
   The default context has not yet been created, do it now. */
#define php_stream_context_from_zval(zcontext, nocontext) ( \
		(zcontext) ? zend_fetch_resource_ex(zcontext, "Stream-Context", php_le_stream_context()) : \
		php_stream_context_get_default(nocontext))

#define php_stream_context_to_zval(context, zval) { ZVAL_RES(zval, (context)->res); GC_ADDREF((context)->res); }

typedef struct _php_stream_notifier php_stream_notifier;

struct _php_stream_notifier {
	php_stream_notification_func func;
	void (*dtor)(php_stream_notifier *notifier);
	zval ptr;
	int mask;
	size_t progress, progress_max; /* position for progress notification */
};

struct _php_stream_context {
	php_stream_notifier *notifier;
	zval options;	/* hash keyed by wrapper family or specific wrapper */
	zend_resource *res;	/* used for auto-cleanup */
};

BEGIN_EXTERN_C()
PHPAPI int php_le_stream_context(void);
PHPAPI void php_stream_context_free(php_stream_context *context);
PHPAPI php_stream_context *php_stream_context_alloc(void);
PHPAPI zval *php_stream_context_get_option(php_stream_context *context,
		const char *wrappername, const char *optionname);
PHPAPI void php_stream_context_set_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval *optionvalue);

PHPAPI php_stream_notifier *php_stream_notification_alloc(void);
PHPAPI void php_stream_notification_free(php_stream_notifier *notifier);
END_EXTERN_C()

/* not all notification codes are implemented */
#define PHP_STREAM_NOTIFY_RESOLVE		1
#define PHP_STREAM_NOTIFY_CONNECT		2
#define PHP_STREAM_NOTIFY_AUTH_REQUIRED		3
#define PHP_STREAM_NOTIFY_MIME_TYPE_IS	4
#define PHP_STREAM_NOTIFY_FILE_SIZE_IS	5
#define PHP_STREAM_NOTIFY_REDIRECTED	6
#define PHP_STREAM_NOTIFY_PROGRESS		7
#define PHP_STREAM_NOTIFY_COMPLETED		8
#define PHP_STREAM_NOTIFY_FAILURE		9
#define PHP_STREAM_NOTIFY_AUTH_RESULT	10

#define PHP_STREAM_NOTIFY_SEVERITY_INFO	0
#define PHP_STREAM_NOTIFY_SEVERITY_WARN	1
#define PHP_STREAM_NOTIFY_SEVERITY_ERR	2

BEGIN_EXTERN_C()
static zend_always_inline bool php_stream_zend_parse_arg_into_stream_context(
	zval *arg,
	php_stream_context **destination_context_ptr,
	bool check_null,
	php_stream_context *default_context
) {
	if (EXPECTED(Z_TYPE_P(arg) == IS_RESOURCE)) {
		*destination_context_ptr = (php_stream_context*)zend_fetch_resource_ex(arg, "Stream-Context", php_le_stream_context());
		if (UNEXPECTED(*destination_context_ptr == NULL)) {
			return false;
		}
	} else if (check_null && EXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		if (default_context) {
			*destination_context_ptr = default_context;
		} else {
			*destination_context_ptr = NULL;
		}
	} else {
		return false;
	}
	return true;
}

#define PHP_Z_PARAM_STREAM_CONTEXT_EX(destination_context_ptr, check_null, null_as_default_context) \
	Z_PARAM_PROLOGUE(0, 0); \
	php_stream_context *php_param_default_context = php_stream_context_get_default(null_as_default_context); \
	if (UNEXPECTED(!php_stream_zend_parse_arg_into_stream_context(_arg, &destination_context_ptr, check_null, php_param_default_context))) { \
		_error_code = ZPP_ERROR_FAILURE; \
		if (!EG(exception)) { \
			_expected_type = check_null ? Z_EXPECTED_RESOURCE_OR_NULL : Z_EXPECTED_RESOURCE; \
			_error_code = ZPP_ERROR_WRONG_ARG; \
		} \
		break; \
	}
#define PHP_Z_PARAM_STREAM_CONTEXT(dest) PHP_Z_PARAM_STREAM_CONTEXT_EX(dest, false, false)
#define PHP_Z_PARAM_STREAM_CONTEXT_OR_NULL(dest) PHP_Z_PARAM_STREAM_CONTEXT_EX(dest, true, false)
#define PHP_Z_PARAM_STREAM_CONTEXT_OR_NULL_AS_DEFAULT_CONTEXT(dest) PHP_Z_PARAM_STREAM_CONTEXT_EX(dest, true, true)

PHPAPI void php_stream_notification_notify(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr);
PHPAPI php_stream_context *php_stream_context_set(php_stream *stream, php_stream_context *context);
END_EXTERN_C()

#define php_stream_notify_info(context, code, xmsg, xcode)	do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), (code), PHP_STREAM_NOTIFY_SEVERITY_INFO, \
				(xmsg), (xcode), 0, 0, NULL); } } while (0)

#define php_stream_notify_progress(context, bsofar, bmax) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_PROGRESS, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			NULL, 0, (bsofar), (bmax), NULL); } } while(0)

#define php_stream_notify_completed(context) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_COMPLETED, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			NULL, 0, (context)->notifier->progress, (context)->notifier->progress_max, NULL); } } while(0)

#define php_stream_notify_progress_init(context, sofar, bmax) do { if ((context) && (context)->notifier) { \
	(context)->notifier->progress = (sofar); \
	(context)->notifier->progress_max = (bmax); \
	(context)->notifier->mask |= PHP_STREAM_NOTIFIER_PROGRESS; \
	php_stream_notify_progress((context), (sofar), (bmax)); } } while (0)

#define php_stream_notify_progress_increment(context, dsofar, dmax) do { if ((context) && (context)->notifier && ((context)->notifier->mask & PHP_STREAM_NOTIFIER_PROGRESS)) { \
	(context)->notifier->progress += (dsofar); \
	(context)->notifier->progress_max += (dmax); \
	php_stream_notify_progress((context), (context)->notifier->progress, (context)->notifier->progress_max); } } while (0)

#define php_stream_notify_file_size(context, file_size, xmsg, xcode) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_FILE_SIZE_IS, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			(xmsg), (xcode), 0, (file_size), NULL); } } while(0)

#define php_stream_notify_error(context, code, xmsg, xcode) do { if ((context) && (context)->notifier) {\
	php_stream_notification_notify((context), (code), PHP_STREAM_NOTIFY_SEVERITY_ERR, \
			(xmsg), (xcode), 0, 0, NULL); } } while(0)
