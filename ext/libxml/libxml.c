/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo <shane@php.net>                               |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define IS_EXT_MODULE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#define PHP_XML_INTERNAL
#include "zend_variables.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"

#if HAVE_LIBXML

#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>

#include "php_libxml.h"

/* a true global for initialization */
int _php_libxml_initialized = 0;

#ifdef ZTS
int libxml_globals_id;
#else
PHP_LIBXML_API php_libxml_globals libxml_globals;
#endif

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_LIBXML
ZEND_GET_MODULE(libxml)
# ifdef PHP_WIN32
# include "zend_arg_defs.c"
# endif
#endif /* COMPILE_DL_LIBXML */
/* }}} */

/* {{{ function prototypes */
PHP_MINIT_FUNCTION(libxml);
PHP_RINIT_FUNCTION(libxml);
PHP_MSHUTDOWN_FUNCTION(libxml);
PHP_RSHUTDOWN_FUNCTION(libxml);
PHP_MINFO_FUNCTION(libxml);

/* }}} */

/* {{{ extension definition structures */
function_entry libxml_functions[] = {
	PHP_FE(libxml_set_streams_context, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry libxml_module_entry = {
    STANDARD_MODULE_HEADER,
	"libxml",                /* extension name */
	libxml_functions,        /* extension function list */
	PHP_MINIT(libxml),       /* extension-wide startup function */
	PHP_MSHUTDOWN(libxml),   /* extension-wide shutdown function */
	PHP_RINIT(libxml),       /* per-request startup function */
	PHP_RSHUTDOWN(libxml),   /* per-request shutdown function */
	PHP_MINFO(libxml),       /* information function */
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

/* }}} */

/* {{{ startup, shutdown and info functions */
#ifdef ZTS
static void php_libxml_init_globals(php_libxml_globals *libxml_globals_p TSRMLS_DC)
{
	LIBXML(stream_context) = NULL;
}
#endif

/* Channel libxml file io layer through the PHP streams subsystem.
 * This allows use of ftps:// and https:// urls */

int php_libxml_streams_IO_match_wrapper(const char *filename)
{
	TSRMLS_FETCH();
	return php_stream_locate_url_wrapper(filename, NULL, 0 TSRMLS_CC) ? 1 : 0;
}

void *php_libxml_streams_IO_open_wrapper(const char *filename)
{
	char resolved_path[MAXPATHLEN + 1];
	php_stream_statbuf ssbuf;
	php_stream_context *context = NULL;
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open = NULL;

	TSRMLS_FETCH();
	xmlURIUnescapeString(filename, 0, resolved_path);
	path_to_open = resolved_path;

	/* logic copied from _php_stream_stat, but we only want to fail
	   if the wrapper supports stat, otherwise, figure it out from
	   the open.  This logic is only to support hiding warnings
	   that the streams layer puts out at times, but for libxml we
	   may try to open files that don't exist, but it is not a failure
	   in xml processing (eg. DTD files)  */
	wrapper = php_stream_locate_url_wrapper(resolved_path, &path_to_open, ENFORCE_SAFE_MODE TSRMLS_CC);
	if (wrapper && wrapper->wops->url_stat) {
		if (wrapper->wops->url_stat(wrapper, path_to_open, &ssbuf TSRMLS_CC) == -1) {
			return NULL;
		}
	}

	if (LIBXML(stream_context)) {
		context = zend_fetch_resource(&LIBXML(stream_context) TSRMLS_CC, -1, "Stream-Context", NULL, 1, php_le_stream_context());
		return php_stream_open_wrapper_ex((char *)resolved_path, "rb", ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL, context);
	}
	return php_stream_open_wrapper((char *)resolved_path, "rb", ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);
}

int php_libxml_streams_IO_read(void *context, char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_read((php_stream*)context, buffer, len);
}

int php_libxml_streams_IO_write(void *context, const char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_write((php_stream*)context, buffer, len);
}

int php_libxml_streams_IO_close(void *context)
{
	TSRMLS_FETCH();
	return php_stream_close((php_stream*)context);
}

static void php_libxml_error_handler(void *ctx, const char *msg, ...)
{
	va_list ap;
	char *buf;
	int len;

	va_start(ap, msg);
	len = vspprintf(&buf, 0, msg, ap);
	va_end(ap);
	
	/* remove any trailing \n */
	while (len && buf[--len] == '\n') {
		buf[len] = '\0';
	}

	php_error(E_WARNING, "%s", buf);
	efree(buf);
}

PHP_LIBXML_API void php_libxml_initialize() {
	if (!_php_libxml_initialized) {
		/* we should be the only one's to ever init!! */
		xmlInitParser();

		/* Enable php stream/wrapper support for libxml 
		   we only use php streams, so we do not enable
		   the default io handlers in libxml.
		*/
		xmlRegisterInputCallbacks(
			php_libxml_streams_IO_match_wrapper, 
			php_libxml_streams_IO_open_wrapper,
			php_libxml_streams_IO_read, 
			php_libxml_streams_IO_close);

		xmlRegisterOutputCallbacks(
			php_libxml_streams_IO_match_wrapper, 
			php_libxml_streams_IO_open_wrapper,
			php_libxml_streams_IO_write, 
			php_libxml_streams_IO_close);

		/* report errors via handler rather than stderr */
		xmlSetGenericErrorFunc(NULL, php_libxml_error_handler);

		_php_libxml_initialized = 1;
	}
}

PHP_LIBXML_API void php_libxml_shutdown() {
	if (_php_libxml_initialized) {
		/* reset libxml generic error handling */
		xmlSetGenericErrorFunc(NULL, NULL);
		xmlCleanupParser();
		_php_libxml_initialized = 0;
	}
}

PHP_MINIT_FUNCTION(libxml)
{
	php_libxml_initialize();

#ifdef ZTS
	ts_allocate_id(&libxml_globals_id, sizeof(php_libxml_globals), (ts_allocate_ctor) php_libxml_init_globals, NULL);
#else
	LIBXML(stream_context) = NULL;
#endif

	return SUCCESS;
}


PHP_RINIT_FUNCTION(libxml)
{
    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(libxml)
{
	php_libxml_shutdown();
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(libxml)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(libxml)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "libXML support", "active");
	php_info_print_table_row(2, "libXML Version", LIBXML_DOTTED_VERSION);
	php_info_print_table_row(2, "libXML streams", "enabled");
	php_info_print_table_end();
}
/* }}} */


/* {{{ proto void libxml_set_streams_context(resource streams_context) 
   Set the streams context for the next libxml document load or write */
PHP_FUNCTION(libxml_set_streams_context)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
		return;
	}
	if (LIBXML(stream_context)) {
		ZVAL_DELREF(LIBXML(stream_context));
		LIBXML(stream_context) = NULL;
	}
	ZVAL_ADDREF(arg);
	LIBXML(stream_context) = arg;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
