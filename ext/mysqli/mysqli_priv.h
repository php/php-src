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
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLI_PRIV_H
#define MYSQLI_PRIV_H

#ifdef PHP_MYSQL_UNIX_SOCK_ADDR
#ifdef MYSQL_UNIX_ADDR
#undef MYSQL_UNIX_ADDR
#endif
#define MYSQL_UNIX_ADDR PHP_MYSQL_UNIX_SOCK_ADDR
#endif

extern const zend_function_entry mysqli_functions[];
extern const zend_function_entry mysqli_link_methods[];
extern const zend_function_entry mysqli_stmt_methods[];
extern const zend_function_entry mysqli_result_methods[];
extern const zend_function_entry mysqli_driver_methods[];
extern const zend_function_entry mysqli_warning_methods[];
extern const zend_function_entry mysqli_exception_methods[];

extern const mysqli_property_entry mysqli_link_property_entries[];
extern const mysqli_property_entry mysqli_result_property_entries[];
extern const mysqli_property_entry mysqli_stmt_property_entries[];
extern const mysqli_property_entry mysqli_driver_property_entries[];
extern const mysqli_property_entry mysqli_warning_property_entries[];

extern const zend_property_info mysqli_link_property_info_entries[];
extern const zend_property_info mysqli_result_property_info_entries[];
extern const zend_property_info mysqli_stmt_property_info_entries[];
extern const zend_property_info mysqli_driver_property_info_entries[];
extern const zend_property_info mysqli_warning_property_info_entries[];

extern int php_le_pmysqli(void);
extern void php_mysqli_dtor_p_elements(void *data);

extern void php_mysqli_close(MY_MYSQL * mysql, int close_type, int resource_status);

extern void php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAMETERS, int override_flag, int into_object);
extern void php_clear_stmt_bind(MY_STMT *stmt);
extern void php_clear_mysql(MY_MYSQL *);
extern MYSQLI_WARNING *php_get_warnings(MYSQLND_CONN_DATA * mysql);

extern void php_clear_warnings(MYSQLI_WARNING *w);
extern void php_free_stmt_bind_buffer(BIND_BUFFER bbuf, int type);
extern void php_mysqli_report_error(const char *sqlstate, int errorno, const char *error);
extern void php_mysqli_report_index(const char *query, unsigned int status);
extern void php_mysqli_throw_sql_exception(char *sqlstate, int errorno, char *format, ...);

#define PHP_MYSQLI_EXPORT(__type) PHP_MYSQLI_API __type

PHP_MYSQLI_EXPORT(zend_object *) mysqli_objects_new(zend_class_entry *);

#define MYSQLI_DISABLE_MQ if (mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
	mysql->multi_query = 0; \
}

#define MYSQLI_ENABLE_MQ if (!mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
	mysql->multi_query = 1; \
}

/* Numbers that cannot be represented as a signed int are converted to a string instead (affects 32-bit builds). */
#define MYSQLI_RETURN_LONG_INT(__val) \
{ \
	if ((__val) < ZEND_LONG_MAX) {		\
		RETURN_LONG((zend_long) (__val));		\
	} else {				\
		/* always used with my_ulonglong -> %llu */ \
		RETURN_STR(strpprintf(0, MYSQLI_LLU_SPEC, (__val)));	\
	} \
}

#define MYSQLI_STORE_RESULT 0
#define MYSQLI_USE_RESULT 	1
#define MYSQLI_ASYNC	 	8
#define MYSQLI_STORE_RESULT_COPY_DATA 16

/* for mysqli_fetch_assoc */
#define MYSQLI_ASSOC	1
#define MYSQLI_NUM		2
#define MYSQLI_BOTH		3

/* fetch types */
#define FETCH_SIMPLE		1
#define FETCH_RESULT		2

/*** REPORT MODES ***/
#define MYSQLI_REPORT_OFF           0
#define MYSQLI_REPORT_ERROR			1
#define MYSQLI_REPORT_STRICT		2
#define MYSQLI_REPORT_INDEX			4
#define MYSQLI_REPORT_CLOSE			8
#define MYSQLI_REPORT_ALL		  255

#define MYSQLI_REPORT_MYSQL_ERROR(mysql) \
if ((MyG(report_mode) & MYSQLI_REPORT_ERROR) && mysql_errno(mysql)) { \
	php_mysqli_report_error(mysql_sqlstate(mysql), mysql_errno(mysql), mysql_error(mysql)); \
}

#define MYSQLI_REPORT_STMT_ERROR(stmt) \
if ((MyG(report_mode) & MYSQLI_REPORT_ERROR) && mysql_stmt_errno(stmt)) { \
	php_mysqli_report_error(mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt), mysql_stmt_error(stmt)); \
}

void mysqli_common_connect(INTERNAL_FUNCTION_PARAMETERS, bool is_real_connect, bool in_ctor);

void php_mysqli_init(INTERNAL_FUNCTION_PARAMETERS, bool is_method);

#endif /* MYSQLI_PRIV_H */
