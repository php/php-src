/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sam Ruby (rubys@us.ibm.com)                                  |
   +----------------------------------------------------------------------+
*/

#include <jni.h>

#include "php.h"
#include "php_globals.h"

#include "SAPI.h"

#include <stdio.h>
#include "php.h"
#ifdef PHP_WIN32
#include "win32/time.h"
#include "win32/signal.h"
#include <process.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SETLOCALE
#include <locale.h>
#endif
#include "zend.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/php_dir.h"
#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#include "win32/php_registry.h"
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

JNIEXPORT void JNICALL Java_net_php_reflect_setEnv
  (JNIEnv *newJenv, jclass self TSRMLS_DC);

typedef struct {
	JNIEnv *jenv;
	jobject servlet;
	char *cookies;
} servlet_request;

extern zend_module_entry java_module_entry;

static zend_module_entry *additional_php_extensions[] = {
  &java_module_entry
};

#define EXTCOUNT (sizeof(additional_php_extensions)/sizeof(zend_module_entry *))

/***************************************************************************/

/*
 * JNI convenience utilities
 */

#define SETSTRING(target, source) \
	{ const char *UTFString; \
	if (source) { \
	UTFString = (*jenv)->GetStringUTFChars(jenv, source, 0); \
	target = estrdup(UTFString); \
	(*jenv)->ReleaseStringUTFChars(jenv, source, UTFString); \
	} else { \
	target = 0; \
	} }

#define FREESTRING(target) \
	{ if (target) { efree(target); target=0; } }

void ThrowIOException (JNIEnv *jenv, char *msg) {
	jclass iox = (*jenv)->FindClass (jenv, "java/io/IOException");
	(*jenv)->ThrowNew (jenv, iox, (msg?msg:"null") );
}

void ThrowServletException (JNIEnv *jenv, char *msg) {
	jclass sx = (*jenv)->FindClass (jenv, "javax/servlet/ServletException");
	(*jenv)->ThrowNew (jenv, sx, msg);
}

/***************************************************************************/

/*
 * sapi callbacks
 */

static int sapi_servlet_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	if (!SG(server_context)) {
		fprintf(stderr, str);
		return 0;
	}

	{
	JNIEnv *jenv = ((servlet_request*)SG(server_context))->jenv;
	jobject servlet = ((servlet_request*)SG(server_context))->servlet;

	jclass servletClass = (*jenv)->GetObjectClass(jenv, servlet);
	jmethodID write = (*jenv)->GetMethodID(jenv, servletClass, "write",
		"(Ljava/lang/String;)V");
	char *copy = malloc(str_length+1);
	jstring arg;
	memcpy(copy, str, str_length);
	copy[str_length] = 0;
	arg=(*jenv)->NewStringUTF(jenv, copy);
	free(copy);
	(*jenv)->CallVoidMethod(jenv, servlet, write, arg);
	(*jenv)->DeleteLocalRef(jenv, arg);
	return str_length;
	}
}


static void sapi_servlet_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
	if (!sapi_header) return;
	if (!SG(server_context)) return;

	{
	JNIEnv *jenv = ((servlet_request*)SG(server_context))->jenv;
	jobject servlet = ((servlet_request*)SG(server_context))->servlet;

	jclass servletClass = (*jenv)->GetObjectClass(jenv, servlet);
	jmethodID header = (*jenv)->GetMethodID(jenv, servletClass, "header",
		"(Ljava/lang/String;)V");
	jstring arg=(*jenv)->NewStringUTF(jenv, sapi_header->header);
	(*jenv)->CallVoidMethod(jenv, servlet, header, arg);
	(*jenv)->DeleteLocalRef(jenv, arg);
	}
}


static int sapi_servlet_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	if (count_bytes == 0) {
		return 0;
	} else {
		JNIEnv *jenv = ((servlet_request*)SG(server_context))->jenv;
		jobject servlet = ((servlet_request*)SG(server_context))->servlet;

		jclass servletClass = (*jenv)->GetObjectClass(jenv, servlet);
		jmethodID readPost = (*jenv)->GetMethodID(jenv, servletClass, 
			"readPost", "(I)Ljava/lang/String;");
		jstring post = (*jenv)->CallObjectMethod(jenv, servlet, readPost, 
			count_bytes);
	
		const char *postAsUTF = (*jenv)->GetStringUTFChars(jenv, post, 0);
		uint read_bytes=(*jenv)->GetStringLength(jenv, post);
		if (read_bytes>count_bytes) read_bytes=count_bytes;
	
		memcpy(buffer, postAsUTF, read_bytes);
		if (read_bytes<count_bytes) buffer[read_bytes]=0;
	
		(*jenv)->ReleaseStringUTFChars(jenv, post, postAsUTF);
	
		return read_bytes;
	}
}


static char *sapi_servlet_read_cookies(TSRMLS_D)
{
	JNIEnv *jenv = ((servlet_request*)SG(server_context))->jenv;
	jobject servlet = ((servlet_request*)SG(server_context))->servlet;

	jclass servletClass = (*jenv)->GetObjectClass(jenv, servlet);
	jmethodID readCookies = (*jenv)->GetMethodID(jenv, servletClass, 
		"readCookies", "()Ljava/lang/String;");
	jstring cookies = (*jenv)->CallObjectMethod(jenv, servlet, readCookies);

	SETSTRING( ((servlet_request*)SG(server_context))->cookies, cookies);

	return ((servlet_request*)SG(server_context))->cookies;
}

/***************************************************************************/


/*
 * sapi maintenance
 */

static int php_servlet_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}

static sapi_module_struct servlet_sapi_module = {
	"java_servlet",					/* name */
	"Java Servlet",					/* pretty name */
									
	php_servlet_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	sapi_servlet_ub_write,			/* unbuffered write */
	NULL,							/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	NULL,							/* send headers handler */
	sapi_servlet_send_header,		/* send header handler */

	sapi_servlet_read_post,			/* read POST data */
	sapi_servlet_read_cookies,		/* read Cookies */

	NULL,							/* register server variables */
	NULL,							/* Log message */

	STANDARD_SAPI_MODULE_PROPERTIES
};


JNIEXPORT void JNICALL Java_net_php_servlet_startup
	(JNIEnv *jenv, jobject self)
{

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
#endif

	sapi_startup(&servlet_sapi_module);

	if (php_module_startup(&servlet_sapi_module, additional_php_extensions, EXTCOUNT)==FAILURE) {
		ThrowServletException(jenv,"module startup failure");
		return;
	}
}


JNIEXPORT void JNICALL Java_net_php_servlet_shutdown
	(JNIEnv *jenv, jobject self)
{
	TSRMLS_FETCH();

	php_module_shutdown(TSRMLS_C);
#ifdef ZTS
	tsrm_shutdown();
#endif
	return;
}

/***************************************************************************/

/*
 * define a Java object to PHP
 */

JNIEXPORT jlong JNICALL Java_net_php_servlet_define
	(JNIEnv *jenv, jobject self, jstring name) 
{
	pval *pzval;
	jlong addr = 0;
	const char *nameAsUTF = (*jenv)->GetStringUTFChars(jenv, name, 0);
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(pzval);
	addr = (jlong)(long) pzval;

	zend_hash_add(&EG(symbol_table), (char*)nameAsUTF, 
		strlen(nameAsUTF)+1, &pzval, sizeof(pval *), NULL);
	(*jenv)->ReleaseStringUTFChars(jenv, name, nameAsUTF);

	return addr;
}

/*
 * execute a script
 */

JNIEXPORT void JNICALL Java_net_php_servlet_send
	(JNIEnv *jenv, jobject self, 
	 jstring requestMethod, jstring queryString,
	 jstring requestURI, jstring pathTranslated,
	 jstring contentType, jint contentLength, 
	 jstring authUser, jboolean display_source_mode)
{
	zend_file_handle file_handle;
	int retval;
#ifndef VIRTUAL_DIR
	char cwd[MAXPATHLEN];
#endif
	TSRMLS_FETCH();

	zend_first_try {
		SG(server_context) = emalloc(sizeof(servlet_request));
		((servlet_request*)SG(server_context))->jenv=jenv;
		((servlet_request*)SG(server_context))->servlet=self;
		((servlet_request*)SG(server_context))->cookies=0;

		/*
		 * Initialize the request
		 */

		SETSTRING( SG(request_info).auth_user, authUser );
		SETSTRING( SG(request_info).request_method, requestMethod );
		SETSTRING( SG(request_info).query_string, queryString );
		SETSTRING( SG(request_info).request_uri, requestURI );
		SETSTRING( SG(request_info).content_type, contentType );
		SG(sapi_headers).http_response_code = 200;
		SG(request_info).content_length = contentLength;
		SG(request_info).auth_password = NULL;
		if (php_request_startup(TSRMLS_C)==FAILURE) {
			ThrowServletException(jenv,"request startup failure");
			return;
		}

		/*
		 * Parse the file
		 */
		SETSTRING( SG(request_info).path_translated, pathTranslated );
#ifdef VIRTUAL_DIR
		retval = php_fopen_primary_script(&file_handle TSRMLS_CC);
#else
		/*
		 * The java runtime doesn't like the working directory to be
		 * changed, so save it and change it back as quickly as possible
		 * in the hopes that Java doesn't notice.
		 */
		getcwd(cwd, MAXPATHLEN);
		retval = php_fopen_primary_script(&file_handle TSRMLS_CC);
		chdir(cwd);
#endif
		
		if (retval == FAILURE) {
			php_request_shutdown((void *) 0);
			php_module_shutdown(TSRMLS_C);
			ThrowIOException(jenv, file_handle.filename);
			return;
		}

		/*
		 * Execute the request
		 */
		Java_net_php_reflect_setEnv(jenv, 0 TSRMLS_CC);

		if (display_source_mode) {
			zend_syntax_highlighter_ini syntax_highlighter_ini;

			if (open_file_for_scanning(&file_handle TSRMLS_CC)==SUCCESS) {
				php_get_highlight_struct(&syntax_highlighter_ini);
				sapi_send_headers(TSRMLS_C);
				zend_highlight(&syntax_highlighter_ini TSRMLS_CC);
			}
		} else {
			php_execute_script(&file_handle TSRMLS_CC);
			php_header();			/* Make sure headers have been sent */
		}

		/*
		 * Clean up
		 */
		
		FREESTRING(SG(request_info).query_string);
		FREESTRING(SG(request_info).request_uri);
		FREESTRING(SG(request_info).path_translated);
		FREESTRING(((servlet_request*)SG(server_context))->cookies);    
		efree(SG(server_context));
		SG(server_context)=0;

		if (!display_source_mode) php_request_shutdown((void *) 0);
	} zend_catch {
		ThrowServletException(jenv,"bailout");
	} zend_end_try();
}

