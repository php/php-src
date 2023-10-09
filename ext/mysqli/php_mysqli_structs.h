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
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_MYSQLI_STRUCTS_H
#define PHP_MYSQLI_STRUCTS_H

/* A little hack to prevent build break, when mysql is used together with
 * c-client, which also defines LIST.
 */
#ifdef LIST
#undef LIST
#endif

#include "ext/mysqlnd/mysqlnd.h"
#include "mysqli_mysqlnd.h"


#define MYSQLI_VERSION_ID		101009

enum mysqli_status {
	MYSQLI_STATUS_UNKNOWN=0,
	MYSQLI_STATUS_INITIALIZED,
	MYSQLI_STATUS_VALID
};

typedef struct {
	char		*val;
	zend_ulong		buflen;
	zend_ulong		output_len;
	zend_ulong		type;
} VAR_BUFFER;

typedef struct {
	unsigned int	var_cnt;
	VAR_BUFFER		*buf;
	zval			*vars;
	my_bool			*is_null;
} BIND_BUFFER;

typedef struct {
	MYSQL_STMT	*stmt;
	BIND_BUFFER	param;
	BIND_BUFFER	result;
	char		*query;
} MY_STMT;

typedef struct {
	MYSQL			*mysql;
	zend_string		*hash_key;
	zval			li_read;
	php_stream		*li_stream;
	unsigned int 	multi_query;
	bool		persistent;
	int				async_result_fetch_type;
} MY_MYSQL;

typedef struct {
	void				*ptr;		/* resource: (mysql, result, stmt)   */
	void				*info;		/* additional buffer				 */
	enum mysqli_status	status;		/* object status */
} MYSQLI_RESOURCE;

typedef struct _mysqli_object {
	void 				*ptr;
	HashTable 			*prop_handler;
	zend_object 		zo;
} mysqli_object; /* extends zend_object */

static inline mysqli_object *php_mysqli_fetch_object(zend_object *obj) {
	return (mysqli_object *)((char*)(obj) - XtOffsetOf(mysqli_object, zo));
}

#define Z_MYSQLI_P(zv) php_mysqli_fetch_object(Z_OBJ_P((zv)))

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
	int (*r_func)(mysqli_object *obj, zval *retval, bool quiet);
	int (*w_func)(mysqli_object *obj, zval *value);
} mysqli_property_entry;

typedef struct {
	zend_ptr_stack free_links;
} mysqli_plist_entry;

#ifdef PHP_WIN32
#define PHP_MYSQLI_API __declspec(dllexport)
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
#ifndef L64
#define L64(x) x##LL
#endif
typedef int64_t my_longlong;
#endif

/* we need this for PRIu64 and PRId64 */
#include <inttypes.h>
#define MYSQLI_LLU_SPEC "%" PRIu64
#define MYSQLI_LL_SPEC "%" PRId64

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

extern void php_mysqli_close(MY_MYSQL * mysql, int close_type, int resource_status);

extern const zend_object_iterator_funcs php_mysqli_result_iterator_funcs;
extern zend_object_iterator *php_mysqli_result_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

extern void php_mysqli_fetch_into_hash_aux(zval *return_value, MYSQL_RES * result, zend_long fetchtype);

#define MYSQLI_DISABLE_MQ if (mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
	mysql->multi_query = 0; \
}

#define MYSQLI_ENABLE_MQ if (!mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
	mysql->multi_query = 1; \
}

#define MYSQLI_REGISTER_RESOURCE_EX(__ptr, __zval)  \
	(Z_MYSQLI_P(__zval))->ptr = __ptr;

#define MYSQLI_RETVAL_RESOURCE(__ptr, __ce) \
	RETVAL_OBJ(mysqli_objects_new(__ce)); \
	MYSQLI_REGISTER_RESOURCE_EX(__ptr, return_value)

#define MYSQLI_REGISTER_RESOURCE(__ptr, __ce) \
{\
	zval *object = getThis(); \
	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry)) { \
		object = return_value; \
		ZVAL_OBJ(object, mysqli_objects_new(__ce)); \
	} \
	MYSQLI_REGISTER_RESOURCE_EX(__ptr, object)\
}

#define MYSQLI_FETCH_RESOURCE(__ptr, __type, __id, __name, __check) \
{ \
	MYSQLI_RESOURCE *my_res; \
	mysqli_object *intern = Z_MYSQLI_P(__id); \
	if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {\
		zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(intern->zo.ce->name));\
		RETURN_THROWS();\
  	}\
	__ptr = (__type)my_res->ptr; \
	if (my_res->status < __check) { \
		zend_throw_error(NULL, "%s object is not fully initialized", ZSTR_VAL(intern->zo.ce->name)); \
		RETURN_THROWS();\
	}\
}

#define MYSQLI_FETCH_RESOURCE_BY_OBJ(__ptr, __type, __obj, __name, __check) \
{ \
	MYSQLI_RESOURCE *my_res; \
	if (!(my_res = (MYSQLI_RESOURCE *)(__obj->ptr))) {\
		zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(intern->zo.ce->name));\
		return;\
	}\
	__ptr = (__type)my_res->ptr; \
	if (my_res->status < __check) { \
		zend_throw_error(NULL, "%s object is not fully initialized", ZSTR_VAL(intern->zo.ce->name)); \
		return;\
	}\
}

#define MYSQLI_FETCH_RESOURCE_CONN(__ptr, __id, __check) \
{ \
	MYSQLI_FETCH_RESOURCE((__ptr), MY_MYSQL *, (__id), "mysqli_link", (__check)); \
	if (!(__ptr)->mysql) { \
		zend_throw_error(NULL, "%s object is not fully initialized", ZSTR_VAL(Z_OBJCE_P(__id)->name)); \
		RETURN_THROWS(); \
	} \
}

#define MYSQLI_FETCH_RESOURCE_STMT(__ptr, __id, __check) \
{ \
	MYSQLI_FETCH_RESOURCE((__ptr), MY_STMT *, (__id), "mysqli_stmt", (__check)); \
	ZEND_ASSERT((__ptr)->stmt && "Missing statement?"); \
}

#define MYSQLI_SET_STATUS(__id, __value) \
{ \
	mysqli_object *intern = Z_MYSQLI_P(__id); \
	((MYSQLI_RESOURCE *)intern->ptr)->status = __value; \
} \

#define MYSQLI_CLEAR_RESOURCE(__id) \
{ \
	mysqli_object *intern = Z_MYSQLI_P(__id); \
	efree(intern->ptr); \
	intern->ptr = NULL; \
}


ZEND_BEGIN_MODULE_GLOBALS(mysqli)
	zend_long			num_links;
	zend_long			max_links;
	zend_long 			num_active_persistent;
	zend_long 			num_inactive_persistent;
	zend_long			max_persistent;
	bool				allow_persistent;
	zend_ulong			default_port;
	char				*default_host;
	char				*default_user;
	char				*default_pw;
	char				*default_socket;
	bool				allow_local_infile;
	char				*local_infile_directory;
	zend_long			error_no;
	char				*error_msg;
	zend_long			report_mode;
	bool 				rollback_on_cached_plink;
ZEND_END_MODULE_GLOBALS(mysqli)

#define MyG(v) ZEND_MODULE_GLOBALS_ACCESSOR(mysqli, v)

#if defined(ZTS) && defined(COMPILE_DL_MYSQLI)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(mysqli)

#endif	/* PHP_MYSQLI_STRUCTS.H */
