/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_APACHE_H
#define PHP_APACHE_H

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"

/* Declare this so we can get to it from outside the sapi_apache2.c file */
extern module AP_MODULE_DECLARE_DATA php4_module;

/* A way to specify the location of the php.ini dir in an apache directive */
extern char *apache2_php_ini_path_override;

/* The server_context used by PHP */
typedef struct php_struct {
	int state;
	request_rec *r;
	ap_filter_t *f; /* downstream output filters after the PHP filter. */
	/* Length of post_data buffer */
	int post_len;
	/* Index for reading from buffer */
	int post_idx;
	/* stat structure of the current file */	
	struct stat finfo;
	/* Buffer for request body filter */
	char *post_data;
	/* Whether or not we've processed PHP in the output filters yet. */
	int request_processed;
} php_struct;

void *merge_php_config(apr_pool_t *p, void *base_conf, void *new_conf);
void *create_php_config(apr_pool_t *p, char *dummy);
char *get_php_config(void *conf, char *name, size_t name_len);
void apply_config(void *);
extern const command_rec php_dir_cmds[];

#define APR_ARRAY_FOREACH_OPEN(arr, key, val) 		\
{													\
	apr_table_entry_t *elts;						\
	int i;											\
	elts = (apr_table_entry_t *) arr->elts;			\
	for (i = 0; i < arr->nelts; i++) {				\
		key = elts[i].key;							\
		val = elts[i].val;

#define APR_ARRAY_FOREACH_CLOSE() }}

#endif /* PHP_APACHE_H */
