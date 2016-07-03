/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Stream context and status notification related definitions */

/* callback for status notifications */
typedef void (*php_stream_notification_func)(php_stream_context *context,
		int notifycode, int severity,
		char *xmsg, int xcode,
		size_t bytes_sofar, size_t bytes_max,
		void * ptr TSRMLS_DC);

#define PHP_STREAM_NOTIFIER_PROGRESS	1

/* Attempt to fetch context from the zval passed,
   If no context was passed, use the default context
   The default context has not yet been created, do it now. */
#define php_stream_context_from_zval(zcontext, nocontext) ( \
		(zcontext) ? zend_fetch_resource(&(zcontext) TSRMLS_CC, -1, "Stream-Context", NULL, 1, php_le_stream_context(TSRMLS_C)) : \
		(nocontext) ? NULL : \
		FG(default_context) ? FG(default_context) : \
		(FG(default_context) = php_stream_context_alloc(TSRMLS_C)) )

#define php_stream_context_to_zval(context, zval) { ZVAL_RESOURCE(zval, (context)->rsrc_id); zend_list_addref((context)->rsrc_id); }

typedef struct _php_stream_notifier php_stream_notifier;

struct _php_stream_notifier {
	php_stream_notification_func func;
	void (*dtor)(php_stream_notifier *notifier);
	void *ptr;
	int mask;
	size_t progress, progress_max; /* position for progress notification */
};

struct _php_stream_context {
	php_stream_notifier *notifier;
	zval *options;	/* hash keyed by wrapper family or specific wrapper */
	int rsrc_id;	/* used for auto-cleanup */
};

BEGIN_EXTERN_C()
PHPAPI void php_stream_context_free(php_stream_context *context);
PHPAPI php_stream_context *php_stream_context_alloc(TSRMLS_D);
PHPAPI int php_stream_context_get_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval ***optionvalue);
PHPAPI int php_stream_context_set_option(php_stream_context *context,
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
PHPAPI void php_stream_notification_notify(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC);
PHPAPI php_stream_context *php_stream_context_set(php_stream *stream, php_stream_context *context);
END_EXTERN_C()

#define php_stream_notify_info(context, code, xmsg, xcode)	do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), (code), PHP_STREAM_NOTIFY_SEVERITY_INFO, \
				(xmsg), (xcode), 0, 0, NULL TSRMLS_CC); } } while (0)
			
#define php_stream_notify_progress(context, bsofar, bmax) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_PROGRESS, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			NULL, 0, (bsofar), (bmax), NULL TSRMLS_CC); } } while(0)

#define php_stream_notify_progress_init(context, sofar, bmax) do { if ((context) && (context)->notifier) { \
	(context)->notifier->progress = (sofar); \
	(context)->notifier->progress_max = (bmax); \
	(context)->notifier->mask |= PHP_STREAM_NOTIFIER_PROGRESS; \
	php_stream_notify_progress((context), (sofar), (bmax)); } } while (0)

#define php_stream_notify_progress_increment(context, dsofar, dmax) do { if ((context) && (context)->notifier && (context)->notifier->mask & PHP_STREAM_NOTIFIER_PROGRESS) { \
	(context)->notifier->progress += (dsofar); \
	(context)->notifier->progress_max += (dmax); \
	php_stream_notify_progress((context), (context)->notifier->progress, (context)->notifier->progress_max); } } while (0)

#define php_stream_notify_file_size(context, file_size, xmsg, xcode) do { if ((context) && (context)->notifier) { \
	php_stream_notification_notify((context), PHP_STREAM_NOTIFY_FILE_SIZE_IS, PHP_STREAM_NOTIFY_SEVERITY_INFO, \
			(xmsg), (xcode), 0, (file_size), NULL TSRMLS_CC); } } while(0)
	
#define php_stream_notify_error(context, code, xmsg, xcode) do { if ((context) && (context)->notifier) {\
	php_stream_notification_notify((context), (code), PHP_STREAM_NOTIFY_SEVERITY_ERR, \
			(xmsg), (xcode), 0, 0, NULL TSRMLS_CC); } } while(0)
	

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
