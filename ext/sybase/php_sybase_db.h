/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SYBASE_DB_H
#define PHP_SYBASE_DB_H

#if HAVE_SYBASE

extern zend_module_entry sybase_module_entry;
#define sybase_module_ptr &sybase_module_entry

PHP_MINIT_FUNCTION(sybase);
PHP_RINIT_FUNCTION(sybase);
PHP_MSHUTDOWN_FUNCTION(sybase);
PHP_RSHUTDOWN_FUNCTION(sybase);
PHP_MINFO_FUNCTION(sybase);
PHP_FUNCTION(sybase_connect);
PHP_FUNCTION(sybase_pconnect);
PHP_FUNCTION(sybase_close);
PHP_FUNCTION(sybase_select_db);
PHP_FUNCTION(sybase_query);
PHP_FUNCTION(sybase_free_result);
PHP_FUNCTION(sybase_get_last_message);
PHP_FUNCTION(sybase_num_rows);
PHP_FUNCTION(sybase_num_fields);
PHP_FUNCTION(sybase_fetch_row);
PHP_FUNCTION(sybase_fetch_array);
PHP_FUNCTION(sybase_fetch_object);
PHP_FUNCTION(sybase_data_seek);
PHP_FUNCTION(sybase_affected_rows);
PHP_FUNCTION(sybase_result);
PHP_FUNCTION(sybase_field_seek);
PHP_FUNCTION(sybase_min_error_severity);
PHP_FUNCTION(sybase_min_message_severity);

PHP_FUNCTION(sybase_db_query);
PHP_FUNCTION(sybase_list_fields);
PHP_FUNCTION(sybase_fetch_lengths);
PHP_FUNCTION(sybase_fetch_field);
PHP_FUNCTION(sybase_field_seek);
PHP_FUNCTION(sybase_free_result);
PHP_FUNCTION(sybase_field_name);
PHP_FUNCTION(sybase_field_table);
PHP_FUNCTION(sybase_field_len);
PHP_FUNCTION(sybase_field_type);
PHP_FUNCTION(sybase_field_flags);



#include <sybfront.h>
#include <sybdb.h>
#include <syberror.h>

#define coltype(j) dbcoltype(sybase_ptr->link,j)
#define intcol(i) ((int) *(DBINT *) dbdata(sybase_ptr->link,i))
#define smallintcol(i) ((int) *(DBSMALLINT *) dbdata(sybase_ptr->link,i))
#define tinyintcol(i) ((int) *(DBTINYINT *) dbdata(sybase_ptr->link,i))
#define anyintcol(j) (coltype(j)==SYBINT4?intcol(j):(coltype(j)==SYBINT2?smallintcol(j):tinyintcol(j)))
#define charcol(i) ((DBCHAR *) dbdata(sybase_ptr->link,i))
#define floatcol(i) ((float) *(DBFLT8 *) dbdata(sybase_ptr->link,i))

typedef struct sybase_link_struct sybase_link;

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	char *appname;
	char *server_message;
	int le_link,le_plink,le_result;
	long min_error_severity,min_message_severity;
	long cfg_min_error_severity,cfg_min_message_severity;
	long compatability_mode;
} sybase_module;

struct sybase_link_struct {
	LOGINREC *login;
	DBPROCESS *link;
	int valid;
};

#define SYBASE_ROWS_BLOCK 128

typedef struct {
	char *name,*column_source;
	int max_length, numeric;
	int type;
} sybase_field;

typedef struct {
	pval ***data;
	sybase_field *fields;
	sybase_link *sybase_ptr;
	int cur_row,cur_field;
	int num_rows,num_fields;
} sybase_result;


extern sybase_module php_sybase_module;

#else

#define sybase_module_ptr NULL

#endif

#define phpext_sybase_ptr sybase_module_ptr

#endif /* PHP_SYBASE_DB_H */
