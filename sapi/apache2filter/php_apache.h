/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_APACHE_H
#define PHP_APACHE_H

typedef struct php_struct {
	int state;
	ap_bucket_brigade *bb;
	ap_filter_t *f;
	/* Length of post_data buffer */
	int post_len;
	/* Index for reading from buffer */
	int post_idx;
	/* Buffer for request body filter */
	char *post_data;
} php_struct;

int php_apache_register_module(void);
void *merge_php_config(apr_pool_t *p, void *base_conf, void *new_conf);
void *create_php_config(apr_pool_t *p, char *dummy);
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
