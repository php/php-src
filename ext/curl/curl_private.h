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
   | Author: Sterling Hughes <sterling@php.net>                           |
   |         Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
*/

#ifndef _PHP_CURL_PRIVATE_H
#define _PHP_CURL_PRIVATE_H

#include "php_curl.h"

#define PHP_CURL_DEBUG 0

#include "php_version.h"
#define PHP_CURL_VERSION PHP_VERSION

#include <curl/curl.h>
#include <curl/multi.h>

#define CURLOPT_RETURNTRANSFER 19913
#define CURLOPT_BINARYTRANSFER 19914 /* For Backward compatibility */
#define PHP_CURL_STDOUT 0
#define PHP_CURL_FILE   1
#define PHP_CURL_USER   2
#define PHP_CURL_DIRECT 3
#define PHP_CURL_RETURN 4
#define PHP_CURL_IGNORE 7

#define SAVE_CURL_ERROR(__handle, __err) \
    do { (__handle)->err.no = (int) __err; } while (0)

PHP_MINIT_FUNCTION(curl);
PHP_MSHUTDOWN_FUNCTION(curl);
PHP_MINFO_FUNCTION(curl);

typedef struct {
	zval                  func_name;
	zend_fcall_info_cache fci_cache;
	FILE                 *fp;
	smart_str             buf;
	int                   method;
	zval					stream;
} php_curl_write;

typedef struct {
	zval                  func_name;
	zend_fcall_info_cache fci_cache;
	FILE                 *fp;
	zend_resource        *res;
	int                   method;
	zval                  stream;
} php_curl_read;

typedef struct {
	zval                  func_name;
	zend_fcall_info_cache fci_cache;
} php_curl_callback;

typedef struct {
	php_curl_write    *write;
	php_curl_write    *write_header;
	php_curl_read     *read;
	zval               std_err;
	php_curl_callback *progress;
	php_curl_callback  *xferinfo;
	php_curl_callback  *fnmatch;
#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
	php_curl_callback  *sshhostkey;
#endif
} php_curl_handlers;

struct _php_curl_error  {
	char str[CURL_ERROR_SIZE + 1];
	int  no;
};

struct _php_curl_send_headers {
	zend_string *str;
};

struct _php_curl_free {
	zend_llist post;
	zend_llist stream;
	HashTable *slist;
};

typedef struct {
	CURL                         *cp;
	php_curl_handlers             handlers;
	struct _php_curl_free        *to_free;
	struct _php_curl_send_headers header;
	struct _php_curl_error        err;
	bool                     in_callback;
	uint32_t*                     clone;
	zval                          postfields;
	/* For CURLOPT_PRIVATE */
	zval private_data;
	/* CurlShareHandle object set using CURLOPT_SHARE. */
	struct _php_curlsh *share;
	zend_object                   std;
} php_curl;

#define CURLOPT_SAFE_UPLOAD -1

typedef struct {
	php_curl_callback	*server_push;
} php_curlm_handlers;

typedef struct {
	CURLM      *multi;
	zend_llist  easyh;
	php_curlm_handlers handlers;
	struct {
		int no;
	} err;
	zend_object std;
} php_curlm;

typedef struct _php_curlsh {
	CURLSH                   *share;
	struct {
		int no;
	} err;
	zend_object std;
} php_curlsh;

php_curl *init_curl_handle_into_zval(zval *curl);
void init_curl_handle(php_curl *ch);
void _php_curl_cleanup_handle(php_curl *);
void _php_curl_multi_cleanup_list(void *data);
void _php_curl_verify_handlers(php_curl *ch, bool reporterror);
void _php_setup_easy_copy_handlers(php_curl *ch, php_curl *source);

static inline php_curl *curl_from_obj(zend_object *obj) {
	return (php_curl *)((char *)(obj) - XtOffsetOf(php_curl, std));
}

#define Z_CURL_P(zv) curl_from_obj(Z_OBJ_P(zv))

static inline php_curlsh *curl_share_from_obj(zend_object *obj) {
	return (php_curlsh *)((char *)(obj) - XtOffsetOf(php_curlsh, std));
}

#define Z_CURL_SHARE_P(zv) curl_share_from_obj(Z_OBJ_P(zv))

void curl_multi_register_handlers(void);
void curl_share_register_handlers(void);
void curlfile_register_class(void);
zend_result curl_cast_object(zend_object *obj, zval *result, int type);

#endif  /* _PHP_CURL_PRIVATE_H */
