/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifndef PHP_MYSQLI_STRUCTS_H
#define PHP_MYSQLI_STRUCTS_H

/* A little hack to prevent build break, when mysql is used together with
 * c-client, which also defines LIST.
 */
#ifdef LIST
#undef LIST
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef MYSQLI_USE_MYSQLND
#include "ext/mysqlnd/mysqlnd.h"
#include "mysqli_mysqlnd.h"
#else

/*
  The libmysql headers (a PITA) also define it and there will be an warning.
  Undef it and later we might need to define it again.
*/
#ifdef HAVE_MBRLEN
#undef HAVE_MBRLEN
#define WE_HAD_MBRLEN
#endif
#ifdef HAVE_MBSTATE_T
#undef HAVE_MBSTATE_T
#define WE_HAD_MBSTATE_T
#endif

#if defined(ulong) && !defined(HAVE_ULONG)
#define HAVE_ULONG
#endif

#include <my_global.h>

#if !defined(HAVE_MBRLEN) && defined(WE_HAD_MBRLEN)
#define HAVE_MBRLEN 1
#endif

#if !defined(HAVE_MBSTATE_T) && defined(WE_HAD_MBSTATE_T)
#define HAVE_MBSTATE_T 1
#endif

/*
  We need more than mysql.h because we need CHARSET_INFO in one place.
  This order has been borrowed from the ODBC driver. Nothing can be removed
  from the list of headers :(
*/

#include <my_sys.h>
#include <mysql.h>
#include <errmsg.h>
#include <my_list.h>
#include <m_string.h>
#include <mysqld_error.h>
#include <my_list.h>
#include <m_ctype.h>
#include "mysqli_libmysql.h"
#endif /* MYSQLI_USE_MYSQLND */


#define MYSQLI_VERSION_ID		101009

enum mysqli_status {
	MYSQLI_STATUS_UNKNOWN=0,
	MYSQLI_STATUS_CLEARED,
	MYSQLI_STATUS_INITIALIZED,
	MYSQLI_STATUS_VALID
};

typedef struct {
	char		*val;
	ulong		buflen;
	ulong		output_len;
	ulong		type;
} VAR_BUFFER;

typedef struct {
	unsigned int	var_cnt;
	VAR_BUFFER		*buf;
	zval			**vars;
	char			*is_null;
} BIND_BUFFER;

typedef struct {
	MYSQL_STMT	*stmt;
	BIND_BUFFER	param;
	BIND_BUFFER	result;
	char		*query;
#ifndef MYSQLI_USE_MYSQLND
	/* used to manage refcount with libmysql (already implement in mysqlnd) */
	zend_object_handle link_handle;
#endif
} MY_STMT;

typedef struct {
	MYSQL			*mysql;
	char			*hash_key;
	zval			*li_read;
	php_stream		*li_stream;
	unsigned int 	multi_query;
	zend_bool		persistent;
#if defined(MYSQLI_USE_MYSQLND)
	int				async_result_fetch_type;
#endif
} MY_MYSQL;

typedef struct {
	void				*ptr;		/* resource: (mysql, result, stmt)   */
	void				*info;		/* additional buffer				 */
	enum mysqli_status	status;		/* object status */
} MYSQLI_RESOURCE;

typedef struct _mysqli_object {
	zend_object 		zo;
	void 				*ptr;
	HashTable 			*prop_handler;
} mysqli_object; /* extends zend_object */

typedef struct st_mysqli_warning MYSQLI_WARNING;

struct st_mysqli_warning {
	zval	reason;
	zval	sqlstate;
	int		errorno;
   	MYSQLI_WARNING	*next;
};

typedef struct _mysqli_property_entry {
	const char *pname;
	size_t pname_length;
	int (*r_func)(mysqli_object *obj, zval **retval TSRMLS_DC);
	int (*w_func)(mysqli_object *obj, zval *value TSRMLS_DC);
} mysqli_property_entry;


typedef struct {
	zend_ptr_stack free_links;
} mysqli_plist_entry;

#ifdef PHP_WIN32
#define PHP_MYSQLI_API __declspec(dllexport)
#define MYSQLI_LLU_SPEC "%I64u"
#define MYSQLI_LL_SPEC "%I64d"
#ifndef L64
#define L64(x) x##i64
#endif
typedef __int64 my_longlong;
#else
# if defined(__GNUC__) && __GNUC__ >= 4
#  define PHP_MYSQLI_API __attribute__ ((visibility("default")))
# else
#  define PHP_MYSQLI_API
# endif
/* we need this for PRIu64 and PRId64 */
#include <inttypes.h>
#define MYSQLI_LLU_SPEC "%" PRIu64
#define MYSQLI_LL_SPEC "%" PRId64
#ifndef L64
#define L64(x) x##LL
#endif
typedef int64_t my_longlong;
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_class_entry *mysqli_link_class_entry;
extern zend_class_entry *mysqli_stmt_class_entry;
extern zend_class_entry *mysqli_result_class_entry;
extern zend_class_entry *mysqli_driver_class_entry;
extern zend_class_entry *mysqli_warning_class_entry;
extern zend_class_entry *mysqli_exception_class_entry;
extern int php_le_pmysqli(void);
extern void php_mysqli_dtor_p_elements(void *data);

extern void php_mysqli_close(MY_MYSQL * mysql, int close_type, int resource_status TSRMLS_DC);

extern zend_object_iterator_funcs php_mysqli_result_iterator_funcs;
extern zend_object_iterator *php_mysqli_result_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

extern void php_mysqli_fetch_into_hash_aux(zval *return_value, MYSQL_RES * result, long fetchtype TSRMLS_DC);

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_RuntimeException;
#endif

#define MYSQLI_DISABLE_MQ if (mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
	mysql->multi_query = 0; \
}

#define MYSQLI_ENABLE_MQ if (!mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
	mysql->multi_query = 1; \
}

#define REGISTER_MYSQLI_CLASS_ENTRY(name, mysqli_entry, class_functions) { \
	zend_class_entry ce; \
	INIT_CLASS_ENTRY(ce, name,class_functions); \
	ce.create_object = mysqli_objects_new; \
	mysqli_entry = zend_register_internal_class(&ce TSRMLS_CC); \
} \

#define MYSQLI_REGISTER_RESOURCE_EX(__ptr, __zval)  \
	((mysqli_object *) zend_object_store_get_object(__zval TSRMLS_CC))->ptr = __ptr;

#define MYSQLI_RETURN_RESOURCE(__ptr, __ce) \
	Z_TYPE_P(return_value) = IS_OBJECT; \
	(return_value)->value.obj = mysqli_objects_new(__ce TSRMLS_CC); \
	MYSQLI_REGISTER_RESOURCE_EX(__ptr, return_value)

#define MYSQLI_REGISTER_RESOURCE(__ptr, __ce) \
{\
	zval *object = getThis();\
	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry TSRMLS_CC)) {\
		object = return_value;\
		Z_TYPE_P(object) = IS_OBJECT;\
		(object)->value.obj = mysqli_objects_new(__ce TSRMLS_CC);\
	}\
	MYSQLI_REGISTER_RESOURCE_EX(__ptr, object)\
}

#define MYSQLI_FETCH_RESOURCE(__ptr, __type, __id, __name, __check) \
{ \
	MYSQLI_RESOURCE *my_res; \
	mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
	if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {\
  		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", intern->zo.ce->name);\
  		RETURN_NULL();\
  	}\
	__ptr = (__type)my_res->ptr; \
	if (__check && my_res->status < __check) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %s\n", intern->zo.ce->name); \
		RETURN_NULL();\
	}\
}

#define MYSQLI_FETCH_RESOURCE_BY_OBJ(__ptr, __type, __obj, __name, __check) \
{ \
	MYSQLI_RESOURCE *my_res; \
	if (!(my_res = (MYSQLI_RESOURCE *)(__obj->ptr))) {\
  		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", intern->zo.ce->name);\
  		return;\
  	}\
	__ptr = (__type)my_res->ptr; \
	if (__check && my_res->status < __check) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %s\n", intern->zo.ce->name); \
		return;\
	}\
}


#define MYSQLI_FETCH_RESOURCE_CONN(__ptr, __id, __check) \
{ \
	MYSQLI_FETCH_RESOURCE((__ptr), MY_MYSQL *, (__id), "mysqli_link", (__check)); \
	if (!(__ptr)->mysql) { \
		mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %s\n", intern->zo.ce->name); \
		RETURN_NULL();\
	} \
}

#define MYSQLI_FETCH_RESOURCE_STMT(__ptr, __id, __check) \
{ \
	MYSQLI_FETCH_RESOURCE((__ptr), MY_STMT *, (__id), "mysqli_stmt", (__check)); \
	if (!(__ptr)->stmt) { \
		mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %s\n", intern->zo.ce->name); \
		RETURN_NULL();\
	} \
}


#define MYSQLI_SET_STATUS(__id, __value) \
{ \
	mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
	((MYSQLI_RESOURCE *)intern->ptr)->status = __value; \
} \

#define MYSQLI_CLEAR_RESOURCE(__id) \
{ \
	mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
	efree(intern->ptr); \
	intern->ptr = NULL; \
}


ZEND_BEGIN_MODULE_GLOBALS(mysqli)
	long			default_link;
	long			num_links;
	long			max_links;
	long 			num_active_persistent;
	long 			num_inactive_persistent;
	long			max_persistent;
	long			allow_persistent;
	unsigned long	default_port;
	char			*default_host;
	char			*default_user;
	char			*default_socket;
	char			*default_pw;
	long			reconnect;
	long			allow_local_infile;
	long			strict;
	long			error_no;
	char			*error_msg;
	long			report_mode;
	HashTable		*report_ht;
	unsigned long	multi_query;
	unsigned long	embedded;
	zend_bool 		rollback_on_cached_plink;
ZEND_END_MODULE_GLOBALS(mysqli)


#ifdef ZTS
#define MyG(v) TSRMG(mysqli_globals_id, zend_mysqli_globals *, v)
#else
#define MyG(v) (mysqli_globals.v)
#endif

#define my_estrdup(x) (x) ? estrdup(x) : NULL
#define my_efree(x) if (x) efree(x)

ZEND_EXTERN_MODULE_GLOBALS(mysqli)

#endif	/* PHP_MYSQLI_STRUCTS.H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
