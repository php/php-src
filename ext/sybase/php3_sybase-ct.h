/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHP3_SYBASE_CT_H
#define _PHP3_SYBASE_CT_H

#if COMPILE_DL
#undef HAVE_SYBASE_CT
#define HAVE_SYBASE_CT 1
#endif

#if HAVE_SYBASE_CT

#define CTLIB_VERSION CS_VERSION_100

extern php3_module_entry sybct_module_entry;
#define sybct_module_ptr &sybct_module_entry

extern int php3_minit_sybct(INIT_FUNC_ARGS);
extern int php3_rinit_sybct(INIT_FUNC_ARGS);
extern int php3_mshutdown_sybct(SHUTDOWN_FUNC_ARGS);
extern int php3_rshutdown_sybct(SHUTDOWN_FUNC_ARGS);
extern void php3_info_sybct(void);
extern void php3_sybct_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_select_db(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_query(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_get_last_message(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_num_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_fetch_array(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_fetch_object(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_data_seek(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_affected_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_field_seek(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_min_client_severity(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_min_server_severity(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sybct_fetch_field(INTERNAL_FUNCTION_PARAMETERS);


#include <ctpublic.h>

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	char *appname;
	char *hostname;
	char *server_message;
	int le_link,le_plink,le_result;
	long min_server_severity, min_client_severity;
	long cfg_min_server_severity, cfg_min_client_severity;
} sybct_module;

typedef struct {
	CS_CONNECTION *connection;
	CS_COMMAND *cmd;
	int valid;
	int deadlock;
	int dead;
	long affected_rows;
} sybct_link;

#define SYBASE_ROWS_BLOCK 128

typedef struct {
	char *name,*column_source;
	int max_length, numeric;
	CS_INT type;
} sybct_field;

typedef struct {
	pval **data;
	sybct_field *fields;
	sybct_link *sybct_ptr;
	int cur_row,cur_field;
	int num_rows,num_fields;
} sybct_result;


extern sybct_module php3_sybct_module;

#else

#define sybct_module_ptr NULL

#endif

#endif /* _PHP3_SYBASE_CT_H */
