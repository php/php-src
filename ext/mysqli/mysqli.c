/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_mysqli.h"
#include "zend_default_classes.h"

#define MYSQLI_STORE_RESULT 0
#define MYSQLI_USE_RESULT 1

ZEND_DECLARE_MODULE_GLOBALS(mysqli)
static zend_object_handlers mysqli_object_handlers;
static HashTable classes;
static HashTable mysqli_link_properties;
static HashTable mysqli_result_properties;
static HashTable mysqli_stmt_properties;
PR_MAIN *prmain;
extern void php_mysqli_connect(INTERNAL_FUNCTION_PARAMETERS);

typedef int (*mysqli_read_t)(mysqli_object *obj, zval **retval TSRMLS_DC);
typedef int (*mysqli_write_t)(mysqli_object *obj, zval *newval TSRMLS_DC);

typedef struct _mysqli_prop_handler {
	mysqli_read_t read_func;
	mysqli_write_t write_func;
} mysqli_prop_handler;

/* {{{ php_free_stmt_bind_buffer */
void php_free_stmt_bind_buffer(BIND_BUFFER bbuf, int type)
{
	unsigned int i;

	if (!bbuf.var_cnt) {
		return;
	}

	if (bbuf.is_null) {
		efree(bbuf.is_null);
	}

	for (i=0; i < bbuf.var_cnt; i++) {
		if (type == FETCH_RESULT) {
			if (bbuf.buf[i].type == IS_STRING) {
				efree(bbuf.buf[i].buffer);
			}
		}
		if (bbuf.vars[i]) {
			ZVAL_DELREF(bbuf.vars[i]);
		}	
	}

	if (bbuf.vars) {
		efree(bbuf.vars);
	}

	if (type == FETCH_RESULT) {
		efree(bbuf.buf);
	}
	bbuf.var_cnt = 0;
	return;
}
/* }}} */

/* {{{ php_clear_stmt_bind */
void php_clear_stmt_bind(STMT *stmt)
{
	if (stmt->stmt) {
		mysql_stmt_close(stmt->stmt);
	}

	php_free_stmt_bind_buffer(stmt->param, FETCH_SIMPLE);
	php_free_stmt_bind_buffer(stmt->result, FETCH_RESULT);

	efree(stmt);
	return;
}
/* }}} */

/* {{{ mysqli_objects_dtor
 */
static void mysqli_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	mysqli_object 	*intern = (mysqli_object *)object;
	MYSQLI_RESOURCE	*my_res = (MYSQLI_RESOURCE *)intern->ptr;

	zend_hash_destroy(intern->zo.properties);
	FREE_HASHTABLE(intern->zo.properties);

	/* link object */
	if (intern->zo.ce == mysqli_link_class_entry) {
		if (my_res && my_res->ptr) {
			mysql_close(my_res->ptr);
		}
	} else if (intern->zo.ce == mysqli_stmt_class_entry) { /* stmt object */
		if (my_res && my_res->ptr) {
			php_clear_stmt_bind((STMT *)my_res->ptr);
		}
	} else if (intern->zo.ce == mysqli_result_class_entry) { /* result object */
		if (my_res && my_res->ptr) {
			mysql_free_result(my_res->ptr);
		}
	}
	my_efree(my_res);
	efree(object);
}
/* }}} */

/* {{{ mysqli_objects_clone
 */
static void mysqli_objects_clone(void *object, void **object_clone TSRMLS_DC)
{
	/* TODO */
}
/* }}} */

/* {{{ mysqli_read_na */
static int mysqli_read_na(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	*retval = NULL;
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot read property");
	return FAILURE;
}
/* }}} */

/* {{{ mysqli_write_na */
static int mysqli_write_na(mysqli_object *obj, zval *newval TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot write property");
	return FAILURE;
}
/* }}} */

/* {{{ mysqli_read_property */
zval *mysqli_read_property(zval *object, zval *member, zend_bool silent TSRMLS_DC)
{
	zval tmp_member;
	zval *retval;
	mysqli_object *obj;
	mysqli_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (mysqli_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		ret = zend_hash_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}
	if (ret == SUCCESS) {
		ret = hnd->read_func(obj, &retval TSRMLS_CC);
		if (ret == SUCCESS) {
			/* ensure we're creating a temporary variable */
			retval->refcount = 0;
		} else {
			retval = EG(uninitialized_zval_ptr);
		}
	} else {
		std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, silent TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return(retval);
}
/* }}} */

/* {{{ mysqli_read_property */
void mysqli_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	zval tmp_member;
	mysqli_object *obj;
	mysqli_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (mysqli_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		ret = zend_hash_find((HashTable *)obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}
	if (ret == SUCCESS) {
		hnd->write_func(obj, value TSRMLS_CC);
		if (! PZVAL_IS_REF(value) && value->refcount == 0) {
			value->refcount++;
			zval_ptr_dtor(&value);
		}
	} else {
		std_hnd = zend_get_std_object_handlers();
		std_hnd->write_property(object, member, value TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */


void mysqli_add_property(HashTable *h, char *pname, mysqli_read_t r_func, mysqli_write_t w_func TSRMLS_DC) {
	mysqli_prop_handler		p;

	p.read_func = (r_func) ? r_func : mysqli_read_na; 
	p.write_func = (w_func) ? w_func : mysqli_write_na;

	zend_hash_add(h, pname, strlen(pname) + 1, &p, sizeof(mysqli_prop_handler), NULL);
}

/* {{{ mysqli_objects_new
 */
PHP_MYSQLI_EXPORT(zend_object_value) mysqli_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	mysqli_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(mysqli_object));
	intern->zo.ce = class_type;
	intern->zo.in_get = 0;
	intern->zo.in_set = 0;
	intern->ptr = NULL;
	intern->prop_handler = NULL;

	zend_hash_find(&classes, class_type->name, class_type->name_length + 1, (void **) &intern->prop_handler);

	ALLOC_HASHTABLE(intern->zo.properties);
	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, mysqli_objects_dtor, mysqli_objects_clone TSRMLS_CC);
	retval.handlers = &mysqli_object_handlers;

	return retval;
}
/* }}} */
	
/* {{{ mysqli_module_entry
 */
zend_module_entry mysqli_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mysqli",
	mysqli_functions,
	PHP_MINIT(mysqli),
	PHP_MSHUTDOWN(mysqli),
	PHP_RINIT(mysqli),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mysqli),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mysqli),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MYSQLI
ZEND_GET_MODULE(mysqli)
#endif

/* {{{ PHP_INI_BEGIN
*/
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY_EX("mysqli.max_links",			"-1",	PHP_INI_SYSTEM,		OnUpdateLong,		max_links,			zend_mysqli_globals,		mysqli_globals, display_link_numbers)
	STD_PHP_INI_ENTRY("mysqli.default_host",			NULL,	PHP_INI_ALL,		OnUpdateString,		default_host,		zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_user",			NULL,	PHP_INI_ALL,		OnUpdateString,		default_user,		zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_pw",				NULL,	PHP_INI_ALL,		OnUpdateString,		default_pw,			zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_port",			"3306",	PHP_INI_ALL,		OnUpdateLong,		default_port,		zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_socket",			NULL,	PHP_INI_ALL,		OnUpdateStringUnempty,	default_socket,	zend_mysqli_globals,		mysqli_globals)
PHP_INI_END()

/* }}} */

/* {{{ php_mysqli_init_globals
 */
static void php_mysqli_init_globals(zend_mysqli_globals *mysqli_globals)
{
	mysqli_globals->num_links = 0;
	mysqli_globals->max_links = 0;
	mysqli_globals->default_port = 0;
	mysqli_globals->default_host = NULL;
	mysqli_globals->default_user = NULL;
	mysqli_globals->default_pw = NULL;
	mysqli_globals->default_socket = NULL;
	mysqli_globals->profiler = 0;
	mysqli_globals->multi_query = 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mysqli)
{
	zend_class_entry *ce;
	
	ZEND_INIT_MODULE_GLOBALS(mysqli, php_mysqli_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	memcpy(&mysqli_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	mysqli_object_handlers.clone_obj = zend_objects_store_clone_obj;
	mysqli_object_handlers.read_property = mysqli_read_property;
	mysqli_object_handlers.write_property = mysqli_write_property;
	mysqli_object_handlers.get_property_ptr_ptr = NULL;
//	mysqli_object_handlers.call_method = php_mysqli_connect;

/* todo: call method */

	zend_hash_init(&classes, 0, NULL, NULL, 1);

	REGISTER_MYSQLI_CLASS_ENTRY("mysqli", mysqli_link_class_entry, mysqli_link_methods);

	ce = mysqli_link_class_entry;
	zend_hash_init(&mysqli_link_properties, 0, NULL, NULL, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_link_properties, mysqli_link_property_entries);
	zend_hash_add(&classes, ce->name, ce->name_length+1, &mysqli_link_properties, sizeof(mysqli_link_properties), NULL);

	REGISTER_MYSQLI_CLASS_ENTRY("mysqli_result", mysqli_result_class_entry, mysqli_result_methods);
	ce = mysqli_result_class_entry;
	zend_hash_init(&mysqli_result_properties, 0, NULL, NULL, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_result_properties, mysqli_result_property_entries);
	zend_hash_add(&classes, ce->name, ce->name_length+1, &mysqli_result_properties, sizeof(mysqli_result_properties), NULL);

	REGISTER_MYSQLI_CLASS_ENTRY("mysqli_stmt", mysqli_stmt_class_entry, mysqli_stmt_methods);
	ce = mysqli_stmt_class_entry;
	zend_hash_init(&mysqli_stmt_properties, 0, NULL, NULL, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_stmt_properties, mysqli_stmt_property_entries);
	zend_hash_add(&classes, ce->name, ce->name_length+1, &mysqli_stmt_properties, sizeof(mysqli_stmt_properties), NULL);
	
	/* mysqli_options */
	REGISTER_LONG_CONSTANT("MYSQLI_READ_DEFAULT_GROUP", MYSQL_READ_DEFAULT_GROUP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_READ_DEFAULT_FILE", MYSQL_READ_DEFAULT_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_OPT_CONNECT_TIMEOUT", MYSQL_OPT_CONNECT_TIMEOUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_OPT_LOCAL_INFILE", MYSQL_OPT_LOCAL_INFILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_INIT_COMMAND", MYSQL_INIT_COMMAND, CONST_CS | CONST_PERSISTENT);

	/* mysqli_real_connect flags */	
	REGISTER_LONG_CONSTANT("MYSQLI_CLIENT_SSL", CLIENT_SSL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_CLIENT_COMPRESS",CLIENT_COMPRESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_CLIENT_INTERACTIVE", CLIENT_INTERACTIVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_CLIENT_IGNORE_SPACE", CLIENT_IGNORE_SPACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_CLIENT_NO_SCHEMA", CLIENT_NO_SCHEMA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_CLIENT_MULTI_QUERIES", CLIENT_MULTI_QUERIES, CONST_CS | CONST_PERSISTENT);
	

	/* for mysqli_query */
	REGISTER_LONG_CONSTANT("MYSQLI_STORE_RESULT", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_USE_RESULT", 1, CONST_CS | CONST_PERSISTENT);

	/* for mysqli_fetch_assoc */
	REGISTER_LONG_CONSTANT("MYSQLI_ASSOC", MYSQLI_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_NUM", MYSQLI_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_BOTH", MYSQLI_BOTH, CONST_CS | CONST_PERSISTENT);
	
	
	/* column information */
	REGISTER_LONG_CONSTANT("MYSQLI_NOT_NULL_FLAG", NOT_NULL_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_PRI_KEY_FLAG", PRI_KEY_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_UNIQUE_KEY_FLAG", UNIQUE_KEY_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_MULTIPLE_KEY_FLAG", MULTIPLE_KEY_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_BLOB_FLAG", BLOB_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_UNSIGNED_FLAG", UNSIGNED_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_ZEROFILL_FLAG", ZEROFILL_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_AUTO_INCREMENT_FLAG", AUTO_INCREMENT_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TIMESTAMP_FLAG", TIMESTAMP_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_SET_FLAG", SET_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_NUM_FLAG", NUM_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_PART_KEY_FLAG", PART_KEY_FLAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_GROUP_FLAG", GROUP_FLAG, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_DECIMAL", FIELD_TYPE_DECIMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_TINY", FIELD_TYPE_TINY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_SHORT", FIELD_TYPE_SHORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_LONG", FIELD_TYPE_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_FLOAD", FIELD_TYPE_FLOAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_DOUBLE", FIELD_TYPE_DOUBLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_NULL", FIELD_TYPE_NULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_TIMESTAMP", FIELD_TYPE_TIMESTAMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_LONGLONG", FIELD_TYPE_LONGLONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_INT24", FIELD_TYPE_INT24, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_DATE", FIELD_TYPE_DATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_TIME", FIELD_TYPE_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_DATETIME", FIELD_TYPE_DATETIME	, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_YEAR", FIELD_TYPE_YEAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_NEWDATE", FIELD_TYPE_NEWDATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_ENUM", FIELD_TYPE_ENUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_SET", FIELD_TYPE_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_TINY_BLOB", FIELD_TYPE_TINY_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_MEDIUM_BLOB", FIELD_TYPE_MEDIUM_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_LONG_BLOB", FIELD_TYPE_LONG_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_BLOB", FIELD_TYPE_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_VAR_STRING", FIELD_TYPE_VAR_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_STRING", FIELD_TYPE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_CHAR", FIELD_TYPE_CHAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_INTERVAL", FIELD_TYPE_INTERVAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_TYPE_GEOMETRY", FIELD_TYPE_GEOMETRY, CONST_CS | CONST_PERSISTENT);

	/* bindtypes for mysqli_bind_result */
	REGISTER_LONG_CONSTANT("MYSQLI_BIND_STRING", MYSQLI_BIND_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_BIND_INT", MYSQLI_BIND_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_BIND_DOUBLE", MYSQLI_BIND_DOUBLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_BIND_SEND_DATA", MYSQLI_BIND_SEND_DATA, CONST_CS | CONST_PERSISTENT);

	/* replication */
	REGISTER_LONG_CONSTANT("MYSQLI_RPL_MASTER", MYSQL_RPL_MASTER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_RPL_SLAVE", MYSQL_RPL_SLAVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_RPL_ADMIN", MYSQL_RPL_ADMIN, CONST_CS | CONST_PERSISTENT);
	
	/* bind blob support */
	REGISTER_LONG_CONSTANT("MYSQLI_NO_DATA", MYSQL_NO_DATA, CONST_CS | CONST_PERSISTENT);

	/* profiler support */
	REGISTER_LONG_CONSTANT("MYSQLI_PR_REPORT_STDERR", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_PR_REPORT_PORT", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQLI_PR_REPORT_FILE", 3, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mysqli)
{
	zend_hash_destroy(&mysqli_link_properties);
	zend_hash_destroy(&classes);

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mysqli)
{
	MyG(error_msg) = NULL;
	MyG(error_no) = 0;

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mysqli)
{
	if (MyG(error_msg)) {
		efree(MyG(error_msg));
	}

	if (MyG(profiler)) {
		if (prmain->header.child) {
			php_mysqli_profiler_report((PR_COMMON *)prmain, 0);
		}
		switch (prmain->mode) {
			case MYSQLI_PR_REPORT_FILE:
				fclose(prmain->fp);
				efree(prmain->name);
			break;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mysqli)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "MysqlI Support", "enabled");
	php_info_print_table_row(2, "Client API version", mysql_get_client_info());
	php_info_print_table_row(2, "MYSQLI_SOCKET", MYSQL_UNIX_ADDR);
	
	
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_mysqli_fetch_into_hash
 */
void php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAMETERS, int override_flags, int into_object)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	int				fetchtype;
	unsigned int	i;
	MYSQL_FIELD		*fields;
	MYSQL_ROW		row;
	unsigned long	*field_len;
	PR_RESULT		*prresult;
	PR_COMMAND		*prcommand;
	zval            *ctor_params = NULL;
	zend_class_entry *ce = NULL;

	if (into_object) {
		char *class_name;
		int class_name_len;

		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|sz", &mysql_result, mysqli_result_class_entry, &class_name, &class_name_len, &ctor_params) == FAILURE) {
			return;
		}
		if (ZEND_NUM_ARGS() < (getThis() ? 1 : 2)) {
			ce = zend_standard_class_def;
		} else {
			ce = zend_fetch_class(class_name, class_name_len, ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
		}
		if (!ce) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not find class '%s'", class_name);
			return;
		}
		fetchtype = MYSQLI_ASSOC;
	} else {
		if (override_flags) {
			if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
				return;
			}
			fetchtype = override_flags;
		} else {
			fetchtype = MYSQLI_NUM;
			if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &fetchtype) == FAILURE) {
				return;
			}
		}
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, prresult, PR_RESULT *, &mysql_result, "mysqli_result"); 

	MYSQLI_PROFILER_COMMAND_START(prcommand, prresult);

	fields = mysql_fetch_fields(result);
	if (!(row = mysql_fetch_row(result))) {
		RETURN_NULL();
	}

	array_init(return_value);
	field_len = mysql_fetch_lengths(result);

	for (i = 0; i < mysql_num_fields(result); i++) {
		if (row[i]) {
			zval *res;

			MAKE_STD_ZVAL(res);

			/* check if we need magic quotes */
			if (PG(magic_quotes_runtime)) {
				Z_TYPE_P(res) = IS_STRING;
				Z_STRVAL_P(res) = php_addslashes(row[i], field_len[i], &Z_STRLEN_P(res), 0 TSRMLS_CC);
			} else {
				ZVAL_STRINGL(res, row[i], field_len[i], 1);	
			}

			if (fetchtype & MYSQLI_NUM) {
				add_index_zval(return_value, i, res);
			}
			if (fetchtype & MYSQLI_ASSOC) {
				if (fetchtype & MYSQLI_NUM) {
					ZVAL_ADDREF(res);
				}
				add_assoc_zval(return_value, fields[i].name, res);
			}
		} else {
			if (fetchtype & MYSQLI_NUM) {
				add_index_null(return_value, i);
			}
			if (fetchtype & MYSQLI_ASSOC) {
				add_assoc_null(return_value, fields[i].name);
			}
		}
	}

	if (into_object) {
		zval dataset = *return_value;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval *retval_ptr; 
	
		object_and_properties_init(return_value, ce, NULL);
		zend_merge_properties(return_value, Z_ARRVAL(dataset), 1 TSRMLS_CC);
	
		if (ce->constructor) {
			fci.size = sizeof(fci);
			fci.function_table = &ce->function_table;
			fci.function_name = NULL;
			fci.symbol_table = NULL;
			fci.object_pp = &return_value;
			fci.retval_ptr_ptr = &retval_ptr;
			if (ctor_params && Z_TYPE_P(ctor_params) != IS_NULL) {
				if (Z_TYPE_P(ctor_params) == IS_ARRAY) {
					HashTable *ht = Z_ARRVAL_P(ctor_params);
					Bucket *p;
	
					fci.param_count = 0;
					fci.params = emalloc(sizeof(zval*) * ht->nNumOfElements);
					p = ht->pListHead;
					while (p != NULL) {
						fci.params[fci.param_count++] = (zval**)p->pData;
						p = p->pListNext;
					}
				} else {
					/* Two problems why we throw exceptions here: PHP is typeless
					 * and hence passing one argument that's not an array could be
					 * by mistake and the other way round is possible, too. The 
					 * single value is an array. Also we'd have to make that one
					 * argument passed by reference.
					 */
					zend_throw_exception(zend_exception_get_default(), "Parameter ctor_params must be an array", 0 TSRMLS_CC);
					return;
				}
			} else {
				fci.param_count = 0;
				fci.params = NULL;
			}
			fci.no_separation = 1;

			fcc.initialized = 1;
			fcc.function_handler = ce->constructor;
			fcc.calling_scope = EG(scope);
			fcc.object_pp = &return_value;
		
			if (zend_call_function(&fci, &fcc TSRMLS_CC) == FAILURE) {
				zend_throw_exception_ex(zend_exception_get_default(), 0 TSRMLS_CC, "Could not execute %s::%s()", ce->name, ce->constructor->common.function_name);
			} else {
				if (retval_ptr) {
					zval_ptr_dtor(&retval_ptr);
				}
			}
			if (fci.params) {
				efree(fci.params);
			}
		} else if (ctor_params) {
			zend_throw_exception_ex(zend_exception_get_default(), 0 TSRMLS_CC, "Class %s does not have a constructor hence you cannot use ctor_params", ce->name);
		}
	}

	if (MyG(profiler)) {
		char tmp[10];
		sprintf ((char *)&tmp,"row[%d]", mysql_num_fields(result));
		MYSQLI_PROFILER_COMMAND_RETURNSTRING(prcommand, tmp);
		prresult->fetched_rows++;
	}
}
/* }}} */

/* {{{ php_mysqli_set_error
 */
PHP_MYSQLI_API void php_mysqli_set_error(long mysql_errno, char *mysql_err TSRMLS_DC)
{
	MyG(error_no) = mysql_errno;
	if (MyG(error_msg)) {
		efree(MyG(error_msg));
	}
	MyG(error_msg) = estrdup(mysql_err);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
