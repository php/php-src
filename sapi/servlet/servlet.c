/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sam Ruby (rubys@us.ibm.com)                                  |
   +----------------------------------------------------------------------+
*/

#include <jni.h>

#include "dl/phpdl.h"
#include "php.h"
#include "php_globals.h"

#include "SAPI.h"

#include <stdio.h>
#include "php.h"
#if WIN32|WINNT
#include "win32/time.h"
#include "win32/signal.h"
#include <process.h>
#else
#include "build-defs.h"
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
#include "main.h"
#include "fopen-wrappers.h"
#include "ext/standard/php3_standard.h"
#include "ext/standard/php3_dir.h"
#include "snprintf.h"
#if WIN32|WINNT
#include <io.h>
#include <fcntl.h>
#include "win32/syslog.h"
#include "win32/php_registry.h"
#else
#include <syslog.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#if USE_SAPI
#include "serverapi/sapi.h"
void *gLock;
#ifndef THREAD_SAFE
struct sapi_request_info *sapi_rqst;
#endif
#endif

#if WIN32|WINNT || !defined(HAVE_GETOPT)
#include "php_getopt.h"
#endif

PHPAPI extern char *php3_ini_path;

JNIEXPORT void JNICALL Java_net_php_reflect_setEnv
  (JNIEnv *newJenv, jclass self);

PHPAPI extern char *optarg;
PHPAPI extern int optind;

typedef struct {
	JNIEnv *jenv;
	jobject servlet;
	char *cookies;
} servlet_request;

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
	printf("IOException: %s\n", msg);
}

void ThrowServletException (JNIEnv *jenv, char *msg) {
	jclass sx = (*jenv)->FindClass (jenv, "javax/servlet/ServletException");
	(*jenv)->ThrowNew (jenv, sx, msg);
	printf("ServletException: %s\n", msg);
}

/***************************************************************************/

/*
 * sapi callbacks
 */

static int zend_servlet_ub_write(const char *str, uint str_length)
{
	SLS_FETCH();
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
	jstring arg=(*jenv)->NewStringUTF(jenv, str);
	(*jenv)->CallVoidMethod(jenv, servlet, write, arg);
	(*jenv)->DeleteLocalRef(jenv, arg);
	return str_length;
	}
}


static void sapi_servlet_send_header(sapi_header_struct *sapi_header, void *server_context)
{
	SLS_FETCH();
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


static int sapi_servlet_read_post(char *buffer, uint count_bytes SLS_DC)
{
	JNIEnv *jenv = ((servlet_request*)SG(server_context))->jenv;
	jobject servlet = ((servlet_request*)SG(server_context))->servlet;

	jclass servletClass = (*jenv)->GetObjectClass(jenv, servlet);
	jmethodID readPost = (*jenv)->GetMethodID(jenv, servletClass, "readPost",
		"(I)Ljava/lang/String;");
	jstring post = (*jenv)->CallObjectMethod(jenv, servlet, readPost, 
		count_bytes);

	const char *postAsUTF = (*jenv)->GetStringUTFChars(jenv, post, 0);
	uint read_bytes=(*jenv)->GetStringLength(jenv, post);
	if (read_bytes>count_bytes) read_bytes=count_bytes;

	memcpy(buffer, postAsUTF, count_bytes);

	(*jenv)->ReleaseStringUTFChars(jenv, post, postAsUTF);

	return read_bytes;
}


static char *sapi_servlet_read_cookies(SLS_D)
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

static sapi_module_struct sapi_module = {
	"PHP Language",					/* name */
									
	php_module_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	zend_servlet_ub_write,			/* unbuffered write */

	php_error,						/* error handler */

	NULL,							/* header handler */
	NULL,							/* send headers handler */
	sapi_servlet_send_header,		/* send header handler */

	sapi_servlet_read_post,			/* read POST data */
	sapi_servlet_read_cookies,		/* read Cookies */

	STANDARD_SAPI_MODULE_PROPERTIES
};


JNIEXPORT void JNICALL Java_net_php_servlet_startup
	(JNIEnv *jenv, jobject self)
{

#ifdef ZTS
	tsrm_startup(1,1,0);
#else
	if (setjmp(EG(bailout))!=0) {
		ThrowServletException(jenv,"bailout");
		return;
	}
#endif

	sapi_startup(&sapi_module);

	if (php_module_startup(&sapi_module)==FAILURE) {
		ThrowServletException(jenv,"module startup failure");
		return;
	}
}


JNIEXPORT void JNICALL Java_net_php_servlet_shutdown
	(JNIEnv *jenv, jobject self)
{
	SLS_FETCH();

	php_module_shutdown();
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
	ELS_FETCH();
	const char *nameAsUTF = (*jenv)->GetStringUTFChars(jenv, name, 0);

	MAKE_STD_ZVAL(pzval);
	(pval*)(long)addr = pzval;

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
	 jstring pathInfo, jstring pathTranslated,
	 jstring contentType, jint contentLength, jstring authUser)
{

	zend_file_handle file_handle;
	jlong addr = 0;
	SLS_FETCH();
	PLS_FETCH();
	CLS_FETCH();
	ELS_FETCH();

#ifdef ZTS
	if (setjmp(EG(bailout))!=0) {
		ThrowServletException(jenv,"bailout");
		return;
	}
#endif

	SG(server_context) = emalloc(sizeof(servlet_request));
	((servlet_request*)SG(server_context))->jenv=jenv;
	((servlet_request*)SG(server_context))->servlet=self;
	((servlet_request*)SG(server_context))->cookies=0;

	CG(extended_info) = 0;

	/*
	 * Initialize the request
	 */
	SETSTRING( SG(request_info).auth_user, authUser );
	SETSTRING( SG(request_info).request_method, requestMethod );
	SETSTRING( SG(request_info).query_string, queryString );
	SETSTRING( SG(request_info).request_uri, pathInfo );
	SETSTRING( SG(request_info).content_type, contentType );
	SG(request_info).content_length = contentLength;
	SG(request_info).auth_password = NULL;
	if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
		ThrowServletException(jenv,"request startup failure");
		return;
	}

	/*
	 * Parse the file
	 */
	SETSTRING( SG(request_info).path_translated, pathTranslated );
	file_handle.handle.fp = php3_fopen_for_parser();
	file_handle.filename = SG(request_info).path_translated;
	file_handle.free_filename = 0;
	file_handle.type = ZEND_HANDLE_FP;

	if (!file_handle.handle.fp) {
		php_request_shutdown((void *) 0);
		php_module_shutdown();
		ThrowIOException(jenv,file_handle.filename);
		return;
	}

	/*
	 * Execute the request
	 */
	Java_net_php_reflect_setEnv(jenv, 0);
	php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);

	/*
	 * Clean up
	 */
	php3_header();			/* Make sure headers have been sent */
	
	FREESTRING(SG(request_info).request_method);
	FREESTRING(SG(request_info).query_string);
	FREESTRING(SG(request_info).request_uri);
	FREESTRING(SG(request_info).path_translated);
	FREESTRING(SG(request_info).content_type);
	FREESTRING(SG(request_info).auth_user);
	FREESTRING(((servlet_request*)SG(server_context))->cookies);    
	efree(SG(server_context));
	SG(server_context)=0;

	php_request_shutdown((void *) 0);
}

