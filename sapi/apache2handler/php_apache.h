/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_APACHE_H
#define PHP_APACHE_H

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"

#include "php.h"
#include "main/php_streams.h"

/* Enable per-module logging in Apache 2.4+ */
#ifdef APLOG_USE_MODULE
APLOG_USE_MODULE(php7);
#endif

/* Declare this so we can get to it from outside the sapi_apache2.c file */
extern module AP_MODULE_DECLARE_DATA php7_module;

/* A way to specify the location of the php.ini dir in an apache directive */
extern char *apache2_php_ini_path_override;

/* The server_context used by PHP */
typedef struct php_struct {
	int state;
	request_rec *r;
	apr_bucket_brigade *brigade;
	/* stat structure of the current file */
	zend_stat_t finfo;
	/* Whether or not we've processed PHP in the output filters yet. */
	int request_processed;
	/* final content type */
	char *content_type;
} php_struct;

void *merge_php_config(apr_pool_t *p, void *base_conf, void *new_conf);
void *create_php_config(apr_pool_t *p, char *dummy);
char *get_php_config(void *conf, char *name, size_t name_len);
void apply_config(void *);
extern const command_rec php_dir_cmds[];
void php_ap2_register_hook(apr_pool_t *p);

#define APR_ARRAY_FOREACH_OPEN(arr, key, val) 		\
{													\
	apr_table_entry_t *elts;						\
	int i;											\
	elts = (apr_table_entry_t *) arr->elts;			\
	for (i = 0; i < arr->nelts; i++) {				\
		key = elts[i].key;							\
		val = elts[i].val;

#define APR_ARRAY_FOREACH_CLOSE() }}

typedef struct {
	zend_bool engine;
	zend_bool xbithack;
	zend_bool last_modified;
} php_apache2_info_struct;

extern zend_module_entry apache2_module_entry;

#ifdef ZTS
extern int php_apache2_info_id;
#define AP2(v) ZEND_TSRMG(php_apache2_info_id, php_apache2_info_struct *, v)
ZEND_TSRMLS_CACHE_EXTERN()
#else
extern php_apache2_info_struct php_apache2_info;
#define AP2(v) (php_apache2_info.v)
#endif

/* fix for gcc4 visibility patch */
#ifndef PHP_WIN32
# undef AP_MODULE_DECLARE_DATA
# define AP_MODULE_DECLARE_DATA PHPAPI
#endif

#endif /* PHP_APACHE_H */
