/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

  $Id: php_mysqli_structs.h 302179 2010-08-13 09:57:04Z andrey $
*/

#ifndef MYSQLI_PRIV_H
#define MYSQLI_PRIV_H

#ifdef PHP_MYSQL_UNIX_SOCK_ADDR
#ifdef MYSQL_UNIX_ADDR
#undef MYSQL_UNIX_ADDR
#endif
#define MYSQL_UNIX_ADDR PHP_MYSQL_UNIX_SOCK_ADDR
#endif

/* character set support */
#if defined(MYSQLND_VERSION_ID) || MYSQL_VERSION_ID > 50009
#define HAVE_MYSQLI_GET_CHARSET
#endif

#if defined(MYSQLND_VERSION_ID) || (MYSQL_VERSION_ID > 40112 && MYSQL_VERSION_ID < 50000) || MYSQL_VERSION_ID > 50005
#define HAVE_MYSQLI_SET_CHARSET
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

extern void php_mysqli_close(MY_MYSQL * mysql, int close_type, int resource_status TSRMLS_DC);

extern void php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAMETERS, int override_flag, int into_object);
extern void php_clear_stmt_bind(MY_STMT *stmt TSRMLS_DC);
extern void php_clear_mysql(MY_MYSQL *);
#ifdef MYSQLI_USE_MYSQLND
extern MYSQLI_WARNING *php_get_warnings(MYSQLND_CONN_DATA * mysql TSRMLS_DC);
#else
extern MYSQLI_WARNING *php_get_warnings(MYSQL * mysql TSRMLS_DC);
#endif

extern void php_clear_warnings(MYSQLI_WARNING *w);
extern void php_free_stmt_bind_buffer(BIND_BUFFER bbuf, int type);
extern void php_mysqli_report_error(const char *sqlstate, int errorno, const char *error TSRMLS_DC);
extern void php_mysqli_report_index(const char *query, unsigned int status TSRMLS_DC);
extern void php_mysqli_throw_sql_exception(char *sqlstate, int errorno TSRMLS_DC, char *format, ...);

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_RuntimeException;
#endif

#define PHP_MYSQLI_EXPORT(__type) PHP_MYSQLI_API __type

PHP_MYSQLI_EXPORT(zend_object_value) mysqli_objects_new(zend_class_entry * TSRMLS_DC);


#define MYSQLI_DISABLE_MQ if (mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
	mysql->multi_query = 0; \
}

#define MYSQLI_ENABLE_MQ if (!mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
	mysql->multi_query = 1; \
}


#define MYSQLI_RETURN_LONG_LONG(__val) \
{ \
	if ((__val) < LONG_MAX) {		\
		RETURN_LONG((long) (__val));		\
	} else {				\
		char *ret;			\
		/* always used with my_ulonglong -> %llu */ \
		int l = spprintf(&ret, 0, MYSQLI_LLU_SPEC, (__val));	\
		RETURN_STRINGL(ret, l, 0);		\
	}					\
}

#define MYSQLI_STORE_RESULT 0
#define MYSQLI_USE_RESULT 	1
#ifdef MYSQLI_USE_MYSQLND
#define MYSQLI_ASYNC	 	8
#else
/* libmysql */
#define MYSQLI_ASYNC	 	0
#endif

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
	php_mysqli_report_error(mysql_sqlstate(mysql), mysql_errno(mysql), mysql_error(mysql) TSRMLS_CC); \
}

#define MYSQLI_REPORT_STMT_ERROR(stmt) \
if ((MyG(report_mode) & MYSQLI_REPORT_ERROR) && mysql_stmt_errno(stmt)) { \
	php_mysqli_report_error(mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt), mysql_stmt_error(stmt) TSRMLS_CC); \
}

void mysqli_common_connect(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_real_connect, zend_bool in_ctor);

void php_mysqli_init(INTERNAL_FUNCTION_PARAMETERS);

#endif /* MYSQLI_PRIV_H */
