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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_mysqli.h"
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"
#include "zend_attributes.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "zend_interfaces.h"
#include "zend_attributes.h"
#include "mysqli_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(mysqli)
static PHP_GINIT_FUNCTION(mysqli);

#define MYSQLI_ADD_PROPERTIES(a, b) \
{ \
	int i = 0; \
	while (b[i].pname != NULL) { \
		mysqli_add_property((a), (b)[i].pname, (b)[i].pname_length, \
							(mysqli_read_t)(b)[i].r_func, (mysqli_write_t)(b)[i].w_func); \
		i++; \
	} \
}

#define ERROR_ARG_POS(arg_num) (getThis() ? (arg_num-1) : (arg_num))

static HashTable classes;
static zend_object_handlers mysqli_object_handlers;
static zend_object_handlers mysqli_object_driver_handlers;
static zend_object_handlers mysqli_object_link_handlers;
static zend_object_handlers mysqli_object_result_handlers;
static zend_object_handlers mysqli_object_stmt_handlers;
static zend_object_handlers mysqli_object_warning_handlers;
static HashTable mysqli_driver_properties;
static HashTable mysqli_link_properties;
static HashTable mysqli_result_properties;
static HashTable mysqli_stmt_properties;
static HashTable mysqli_warning_properties;

zend_class_entry *mysqli_link_class_entry;
zend_class_entry *mysqli_stmt_class_entry;
zend_class_entry *mysqli_result_class_entry;
zend_class_entry *mysqli_driver_class_entry;
zend_class_entry *mysqli_warning_class_entry;
zend_class_entry *mysqli_exception_class_entry;


typedef int (*mysqli_read_t)(mysqli_object *obj, zval *rv, bool quiet);
typedef int (*mysqli_write_t)(mysqli_object *obj, zval *newval);

typedef struct _mysqli_prop_handler {
	zend_string *name;
	mysqli_read_t read_func;
	mysqli_write_t write_func;
} mysqli_prop_handler;

static int le_pmysqli;

static void free_prop_handler(zval *el) {
	pefree(Z_PTR_P(el), 1);
}

/* Destructor for mysqli entries in free_links/used_links */
void php_mysqli_dtor_p_elements(void *data)
{
	MYSQL *mysql = (MYSQL *)data;
	mysqli_close(mysql, MYSQLI_CLOSE_IMPLICIT);
}


ZEND_RSRC_DTOR_FUNC(php_mysqli_dtor)
{
	if (res->ptr) {
		mysqli_plist_entry *plist = (mysqli_plist_entry *)res->ptr;
		zend_ptr_stack_clean(&plist->free_links, php_mysqli_dtor_p_elements, 0);
		zend_ptr_stack_destroy(&plist->free_links);
		free(plist);
	}
}


int php_le_pmysqli(void)
{
	return le_pmysqli;
}

/* {{{ php_clear_stmt_bind */
void php_clear_stmt_bind(MY_STMT *stmt)
{
	if (stmt->stmt) {
		if (mysqli_stmt_close(stmt->stmt, true)) {
			php_error_docref(NULL, E_WARNING, "Error occurred while closing statement");
			return;
		}
	}

	/*
	  mysqlnd keeps track of the binding and has freed its
	  structures in stmt_close() above
	*/
	if (stmt->query) {
		efree(stmt->query);
	}
	efree(stmt);
}
/* }}} */

/* {{{ php_clear_mysql */
void php_clear_mysql(MY_MYSQL *mysql) {
	if (mysql->hash_key) {
		zend_string_release_ex(mysql->hash_key, 0);
		mysql->hash_key = NULL;
	}
	if (!Z_ISUNDEF(mysql->li_read)) {
		zval_ptr_dtor(&(mysql->li_read));
		ZVAL_UNDEF(&mysql->li_read);
	}
}
/* }}} */

/* {{{ mysqli_objects_free_storage */
static void mysqli_objects_free_storage(zend_object	*object)
{
	mysqli_object 	*intern = php_mysqli_fetch_object(object);
	MYSQLI_RESOURCE	*my_res = (MYSQLI_RESOURCE *)intern->ptr;

	if (my_res) {
		efree(my_res);
	}
	zend_object_std_dtor(&intern->zo);
}
/* }}} */

/* mysqli_link_free_storage partly doubles the work of PHP_FUNCTION(mysqli_close) */

/* {{{ mysqli_link_free_storage */
static void mysqli_link_free_storage(zend_object *object)
{
	mysqli_object 	*intern = php_mysqli_fetch_object(object);
	MYSQLI_RESOURCE	*my_res = (MYSQLI_RESOURCE *)intern->ptr;

	if (my_res && my_res->ptr) {
		MY_MYSQL *mysql = (MY_MYSQL *)my_res->ptr;
		if (mysql->mysql) {
			php_mysqli_close(mysql, MYSQLI_CLOSE_EXPLICIT, my_res->status);
		}
		php_clear_mysql(mysql);
		efree(mysql);
		my_res->status = MYSQLI_STATUS_UNKNOWN;
	}
	mysqli_objects_free_storage(object);
}
/* }}} */

/* {{{ mysql_driver_free_storage */
static void mysqli_driver_free_storage(zend_object *object)
{
	mysqli_objects_free_storage(object);
}
/* }}} */

/* {{{ mysqli_stmt_free_storage */
static void mysqli_stmt_free_storage(zend_object *object)
{
	mysqli_object 	*intern = php_mysqli_fetch_object(object);
	MYSQLI_RESOURCE	*my_res = (MYSQLI_RESOURCE *)intern->ptr;

	if (my_res && my_res->ptr) {
		MY_STMT *stmt = (MY_STMT *)my_res->ptr;
		php_clear_stmt_bind(stmt);
	}
	mysqli_objects_free_storage(object);
}
/* }}} */

/* {{{ mysqli_result_free_storage */
static void mysqli_result_free_storage(zend_object *object)
{
	mysqli_object 	*intern = php_mysqli_fetch_object(object);
	MYSQLI_RESOURCE	*my_res = (MYSQLI_RESOURCE *)intern->ptr;

	if (my_res && my_res->ptr) {
		mysql_free_result(my_res->ptr);
	}
	mysqli_objects_free_storage(object);
}
/* }}} */

/* {{{ mysqli_warning_free_storage */
static void mysqli_warning_free_storage(zend_object *object)
{
	mysqli_object 	*intern = php_mysqli_fetch_object(object);
	MYSQLI_RESOURCE	*my_res = (MYSQLI_RESOURCE *)intern->ptr;

	if (my_res && my_res->ptr) {
		php_clear_warnings((MYSQLI_WARNING *)my_res->info);
		my_res->ptr = NULL;
	}
	mysqli_objects_free_storage(object);
}
/* }}} */

/* {{{ mysqli_read_na */
static int mysqli_read_na(mysqli_object *obj, zval *retval, bool quiet)
{
	if (!quiet) {
		zend_throw_error(NULL, "Cannot read property");
	}

	return FAILURE;
}
/* }}} */

/* {{{ mysqli_read_property */
zval *mysqli_read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	mysqli_object *obj = php_mysqli_fetch_object(object);
	if (obj->prop_handler) {
		mysqli_prop_handler *hnd = zend_hash_find_ptr(obj->prop_handler, name);
		if (hnd) {
			if (hnd->read_func(obj, rv, type == BP_VAR_IS) == SUCCESS) {
				return rv;
			} else {
				return &EG(uninitialized_zval);
			}
		}
	}

	return zend_std_read_property(object, name, type, cache_slot, rv);
}
/* }}} */

/* {{{ mysqli_write_property */
zval *mysqli_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	mysqli_object *obj = php_mysqli_fetch_object(object);
	if (obj->prop_handler) {
		const mysqli_prop_handler *hnd = zend_hash_find_ptr(obj->prop_handler, name);
		if (hnd) {
			if (!hnd->write_func) {
				zend_throw_error(NULL, "Cannot write read-only property %s::$%s",
					ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
				return &EG(error_zval);
			}

			zend_property_info *prop = zend_get_property_info(object->ce, name, /* silent */ true);
			if (prop && ZEND_TYPE_IS_SET(prop->type)) {
				zval tmp;
				ZVAL_COPY(&tmp, value);
				if (!zend_verify_property_type(prop, &tmp,
							ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)))) {
					zval_ptr_dtor(&tmp);
					return &EG(error_zval);
				}
				hnd->write_func(obj, &tmp);
				zval_ptr_dtor(&tmp);
			} else {
				hnd->write_func(obj, value);
			}
			return value;
		}
	}
	return zend_std_write_property(object, name, value, cache_slot);
}
/* }}} */

/* {{{ void mysqli_add_property(HashTable *h, char *pname, mysqli_read_t r_func, mysqli_write_t w_func) */
void mysqli_add_property(HashTable *h, const char *pname, size_t pname_len, mysqli_read_t r_func, mysqli_write_t w_func) {
	mysqli_prop_handler	p;

	p.name = zend_string_init_interned(pname, pname_len, 1);
	p.read_func = (r_func) ? r_func : mysqli_read_na;
	p.write_func = w_func;
	zend_hash_add_mem(h, p.name, &p, sizeof(mysqli_prop_handler));
	zend_string_release_ex(p.name, 1);
}
/* }}} */

static int mysqli_object_has_property(zend_object *object, zend_string *name, int has_set_exists, void **cache_slot) /* {{{ */
{
	mysqli_object *obj = php_mysqli_fetch_object(object);
	mysqli_prop_handler	*p;
	int ret = 0;

	if ((p = zend_hash_find_ptr(obj->prop_handler, name)) != NULL) {
		switch (has_set_exists) {
			case ZEND_PROPERTY_EXISTS:
				ret = 1;
				break;
			case ZEND_PROPERTY_NOT_EMPTY: {
				zval rv;
				zval *value = mysqli_read_property(object, name, BP_VAR_IS, cache_slot, &rv);
				if (value != &EG(uninitialized_zval)) {
					convert_to_boolean(value);
					ret = Z_TYPE_P(value) == IS_TRUE ? 1 : 0;
				}
				break;
			}
			case ZEND_PROPERTY_ISSET: {
				zval rv;
				zval *value = mysqli_read_property(object, name, BP_VAR_IS, cache_slot, &rv);
				if (value != &EG(uninitialized_zval)) {
					ret = Z_TYPE_P(value) != IS_NULL? 1 : 0;
					zval_ptr_dtor(value);
				}
				break;
			}
			EMPTY_SWITCH_DEFAULT_CASE();
		}
	} else {
		ret = zend_std_has_property(object, name, has_set_exists, cache_slot);
	}

	return ret;
} /* }}} */

HashTable *mysqli_object_get_debug_info(zend_object *object, int *is_temp)
{
	mysqli_object *obj = php_mysqli_fetch_object(object);
	HashTable *retval, *props = obj->prop_handler;
	mysqli_prop_handler *entry;

	retval = zend_new_array(zend_hash_num_elements(props) + 1);

	ZEND_HASH_MAP_FOREACH_PTR(props, entry) {
		zval rv;
		zval *value;

		value = mysqli_read_property(object, entry->name, BP_VAR_IS, 0, &rv);
		if (value != &EG(uninitialized_zval)) {
			zend_hash_add(retval, entry->name, value);
		}
	} ZEND_HASH_FOREACH_END();

	*is_temp = 1;
	return retval;
}

/* {{{ mysqli_objects_new */
PHP_MYSQLI_EXPORT(zend_object *) mysqli_objects_new(zend_class_entry *class_type)
{
	mysqli_object *intern;
	zend_class_entry *mysqli_base_class;
	zend_object_handlers *handlers;

	intern = zend_object_alloc(sizeof(mysqli_object), class_type);

	mysqli_base_class = class_type;
	while (mysqli_base_class->type != ZEND_INTERNAL_CLASS &&
		   mysqli_base_class->parent != NULL) {
		mysqli_base_class = mysqli_base_class->parent;
	}
	intern->prop_handler = zend_hash_find_ptr(&classes, mysqli_base_class->name);

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	/* link object */
	if (instanceof_function(class_type, mysqli_link_class_entry)) {
		handlers = &mysqli_object_link_handlers;
	} else if (instanceof_function(class_type, mysqli_driver_class_entry)) { /* driver object */
		handlers = &mysqli_object_driver_handlers;
	} else if (instanceof_function(class_type, mysqli_stmt_class_entry)) { /* stmt object */
		handlers = &mysqli_object_stmt_handlers;
	} else if (instanceof_function(class_type, mysqli_result_class_entry)) { /* result object */
		handlers = &mysqli_object_result_handlers;
	} else if (instanceof_function(class_type, mysqli_warning_class_entry)) { /* warning object */
		handlers = &mysqli_object_warning_handlers;
	} else {
		handlers = &mysqli_object_handlers;
	}

	intern->zo.handlers = handlers;

	return &intern->zo;
}
/* }}} */

#include "ext/mysqlnd/mysqlnd_reverse_api.h"
static MYSQLND *mysqli_convert_zv_to_mysqlnd(zval * zv)
{
	if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), mysqli_link_class_entry)) {
		MY_MYSQL *mysql;
		MYSQLI_RESOURCE  *my_res;
		mysqli_object *intern = Z_MYSQLI_P(zv);
		if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {
			/* We know that we have a mysqli object, so this failure should be emitted */
			zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(intern->zo.ce->name));
			return NULL;
		}
		mysql = (MY_MYSQL *)(my_res->ptr);
		return mysql ? mysql->mysql : NULL;
	}
	return NULL;
}

static const MYSQLND_REVERSE_API mysqli_reverse_api = {
	&mysqli_module_entry,
	mysqli_convert_zv_to_mysqlnd
};

/* {{{ PHP_INI_BEGIN */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY_EX("mysqli.max_links",			"-1",	PHP_INI_SYSTEM,		OnUpdateLong,		max_links,			zend_mysqli_globals,		mysqli_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mysqli.max_persistent",		"-1",	PHP_INI_SYSTEM,		OnUpdateLong,		max_persistent,		zend_mysqli_globals,		mysqli_globals,	display_link_numbers)
	STD_PHP_INI_BOOLEAN("mysqli.allow_persistent",		"1",	PHP_INI_SYSTEM,		OnUpdateBool,		allow_persistent,	zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_BOOLEAN("mysqli.rollback_on_cached_plink",	"0",PHP_INI_SYSTEM,		OnUpdateBool,		rollback_on_cached_plink,	zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_host",			NULL,	PHP_INI_ALL,		OnUpdateString,		default_host,		zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_user",			NULL,	PHP_INI_ALL,		OnUpdateString,		default_user,		zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_pw",				NULL,	PHP_INI_ALL,		OnUpdateString,		default_pw,			zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.default_port",			"3306",	PHP_INI_ALL,		OnUpdateLong,		default_port,		zend_mysqli_globals,		mysqli_globals)
#ifdef PHP_MYSQL_UNIX_SOCK_ADDR
	STD_PHP_INI_ENTRY("mysqli.default_socket",			MYSQL_UNIX_ADDR,PHP_INI_ALL,OnUpdateStringUnempty,	default_socket,	zend_mysqli_globals,		mysqli_globals)
#else
	STD_PHP_INI_ENTRY("mysqli.default_socket",			NULL,	PHP_INI_ALL,		OnUpdateStringUnempty,	default_socket,	zend_mysqli_globals,		mysqli_globals)
#endif
	STD_PHP_INI_BOOLEAN("mysqli.allow_local_infile",	"0",	PHP_INI_SYSTEM,		OnUpdateBool,		allow_local_infile,	zend_mysqli_globals,		mysqli_globals)
	STD_PHP_INI_ENTRY("mysqli.local_infile_directory",	NULL,	PHP_INI_SYSTEM,		OnUpdateString,		local_infile_directory,	zend_mysqli_globals,	mysqli_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(mysqli)
{
#if defined(COMPILE_DL_MYSQLI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	mysqli_globals->num_links = 0;
	mysqli_globals->max_links = -1;
	mysqli_globals->num_active_persistent = 0;
	mysqli_globals->num_inactive_persistent = 0;
	mysqli_globals->max_persistent = -1;
	mysqli_globals->allow_persistent = 1;
	mysqli_globals->default_port = 0;
	mysqli_globals->default_host = NULL;
	mysqli_globals->default_user = NULL;
	mysqli_globals->default_pw = NULL;
	mysqli_globals->default_socket = NULL;
	mysqli_globals->report_mode = MYSQLI_REPORT_ERROR|MYSQLI_REPORT_STRICT;;
	mysqli_globals->allow_local_infile = 0;
	mysqli_globals->local_infile_directory = NULL;
	mysqli_globals->rollback_on_cached_plink = false;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mysqli)
{
	REGISTER_INI_ENTRIES();

	memcpy(&mysqli_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	mysqli_object_handlers.offset = XtOffsetOf(mysqli_object, zo);
	mysqli_object_handlers.free_obj = mysqli_objects_free_storage;
	mysqli_object_handlers.clone_obj = NULL;
	mysqli_object_handlers.read_property = mysqli_read_property;
	mysqli_object_handlers.write_property = mysqli_write_property;
	mysqli_object_handlers.has_property = mysqli_object_has_property;
	mysqli_object_handlers.get_debug_info = mysqli_object_get_debug_info;
	memcpy(&mysqli_object_driver_handlers, &mysqli_object_handlers, sizeof(zend_object_handlers));
	mysqli_object_driver_handlers.free_obj = mysqli_driver_free_storage;
	memcpy(&mysqli_object_link_handlers, &mysqli_object_handlers, sizeof(zend_object_handlers));
	mysqli_object_link_handlers.free_obj = mysqli_link_free_storage;
	memcpy(&mysqli_object_result_handlers, &mysqli_object_handlers, sizeof(zend_object_handlers));
	mysqli_object_result_handlers.free_obj = mysqli_result_free_storage;
	memcpy(&mysqli_object_stmt_handlers, &mysqli_object_handlers, sizeof(zend_object_handlers));
	mysqli_object_stmt_handlers.free_obj = mysqli_stmt_free_storage;
	memcpy(&mysqli_object_warning_handlers, &mysqli_object_handlers, sizeof(zend_object_handlers));
	mysqli_object_warning_handlers.free_obj = mysqli_warning_free_storage;

	zend_hash_init(&classes, 0, NULL, NULL, 1);

	/* persistent connections */
	le_pmysqli = zend_register_list_destructors_ex(NULL, php_mysqli_dtor,
		"MySqli persistent connection", module_number);

	mysqli_exception_class_entry = register_class_mysqli_sql_exception(spl_ce_RuntimeException);

	mysqli_driver_class_entry = register_class_mysqli_driver();
	mysqli_driver_class_entry->create_object = mysqli_objects_new;
	zend_hash_init(&mysqli_driver_properties, 0, NULL, free_prop_handler, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_driver_properties, mysqli_driver_property_entries);
	zend_hash_add_ptr(&classes, mysqli_driver_class_entry->name, &mysqli_driver_properties);

	mysqli_link_class_entry = register_class_mysqli();
	mysqli_link_class_entry->create_object = mysqli_objects_new;
	zend_hash_init(&mysqli_link_properties, 0, NULL, free_prop_handler, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_link_properties, mysqli_link_property_entries);
	zend_hash_add_ptr(&classes, mysqli_link_class_entry->name, &mysqli_link_properties);

	mysqli_warning_class_entry = register_class_mysqli_warning();
	mysqli_warning_class_entry->create_object = mysqli_objects_new;
	zend_hash_init(&mysqli_warning_properties, 0, NULL, free_prop_handler, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_warning_properties, mysqli_warning_property_entries);
	zend_hash_add_ptr(&classes, mysqli_warning_class_entry->name, &mysqli_warning_properties);

	mysqli_result_class_entry = register_class_mysqli_result(zend_ce_aggregate);
	mysqli_result_class_entry->create_object = mysqli_objects_new;
	mysqli_result_class_entry->get_iterator = php_mysqli_result_get_iterator;
	zend_hash_init(&mysqli_result_properties, 0, NULL, free_prop_handler, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_result_properties, mysqli_result_property_entries);
	zend_hash_add_ptr(&classes, mysqli_result_class_entry->name, &mysqli_result_properties);

	mysqli_stmt_class_entry = register_class_mysqli_stmt();
	mysqli_stmt_class_entry->create_object = mysqli_objects_new;
	zend_hash_init(&mysqli_stmt_properties, 0, NULL, free_prop_handler, 1);
	MYSQLI_ADD_PROPERTIES(&mysqli_stmt_properties, mysqli_stmt_property_entries);
	zend_hash_add_ptr(&classes, mysqli_stmt_class_entry->name, &mysqli_stmt_properties);

	register_mysqli_symbols(module_number);

	mysqlnd_reverse_api_register_api(&mysqli_reverse_api);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(mysqli)
{
	zend_hash_destroy(&mysqli_driver_properties);
	zend_hash_destroy(&mysqli_result_properties);
	zend_hash_destroy(&mysqli_stmt_properties);
	zend_hash_destroy(&mysqli_warning_properties);
	zend_hash_destroy(&mysqli_link_properties);
	zend_hash_destroy(&classes);

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(mysqli)
{
	MyG(error_msg) = NULL;
	MyG(error_no) = 0;
	MyG(report_mode) = MYSQLI_REPORT_ERROR|MYSQLI_REPORT_STRICT;

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(mysqli)
{
	/* check persistent connections, move used to free */

	if (MyG(error_msg)) {
		efree(MyG(error_msg));
	}

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(mysqli)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_row(2, "MysqlI Support", "enabled");
	php_info_print_table_row(2, "Client API library version", mysql_get_client_info());
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, MyG(num_active_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, MyG(num_inactive_persistent));
	php_info_print_table_row(2, "Inactive Persistent Links", buf);
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, MyG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Dependencies */
static const  zend_module_dep mysqli_deps[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_REQUIRED("mysqlnd")
	ZEND_MOD_END
};

/* {{{ mysqli_module_entry */
zend_module_entry mysqli_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	mysqli_deps,
	"mysqli",
	ext_functions,
	PHP_MINIT(mysqli),
	PHP_MSHUTDOWN(mysqli),
	PHP_RINIT(mysqli),
	PHP_RSHUTDOWN(mysqli),
	PHP_MINFO(mysqli),
	PHP_MYSQLI_VERSION,
	PHP_MODULE_GLOBALS(mysqli),
	PHP_GINIT(mysqli),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_MYSQLI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(mysqli)
#endif


PHP_METHOD(mysqli_stmt, __construct)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MY_STMT				*stmt;
	MYSQLI_RESOURCE		*mysqli_resource;
	char				*statement = NULL;
	size_t					statement_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|s!", &mysql_link, mysqli_link_class_entry, &statement, &statement_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	stmt = (MY_STMT *) ecalloc(1, sizeof(MY_STMT));

	if (!(stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)stmt;
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;

	MYSQLI_REGISTER_RESOURCE_EX(mysqli_resource, getThis());

	if (statement) {
		if(mysql_stmt_prepare(stmt->stmt, statement, statement_len)) {
			MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
			RETURN_FALSE;
		}
		mysqli_resource->status = MYSQLI_STATUS_VALID;
	}
}

PHP_METHOD(mysqli_result, __construct)
{
	MY_MYSQL			*mysql;
	MYSQL_RES			*result = NULL;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	zend_long				resmode = MYSQLI_STORE_RESULT;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &mysql_link, mysqli_link_class_entry, &resmode) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	switch (resmode) {
		case MYSQLI_STORE_RESULT:
			result = mysql_store_result(mysql->mysql);
			break;
		case MYSQLI_USE_RESULT:
			result = mysql_use_result(mysql->mysql);
			break;
		default:
			zend_argument_value_error(2, "must be either MYSQLI_STORE_RESULT or MYSQLI_USE_RESULT");
			RETURN_THROWS();
	}

	if (!result) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysqli_server_status(mysql->mysql));
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;

	MYSQLI_REGISTER_RESOURCE_EX(mysqli_resource, getThis());
}

PHP_METHOD(mysqli_result, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

/* {{{ php_mysqli_fetch_into_hash_aux */
void php_mysqli_fetch_into_hash_aux(zval *return_value, MYSQL_RES * result, zend_long fetchtype)
{
	mysqlnd_fetch_into(result, ((fetchtype & MYSQLI_NUM)? MYSQLND_FETCH_NUM:0) | ((fetchtype & MYSQLI_ASSOC)? MYSQLND_FETCH_ASSOC:0), return_value);
	/* TODO: We don't have access to the connection object at this point, so we use low-level
	 * mysqlnd APIs to access the error information. We should try to pass through the connection
	 * object instead. */
	if (MyG(report_mode) & MYSQLI_REPORT_ERROR) {
		MYSQLND_CONN_DATA *conn = result->conn;
		unsigned error_no = conn->m->get_error_no(conn);
		if (error_no) {
			php_mysqli_report_error(
				conn->m->get_sqlstate(conn), error_no, conn->m->get_error_str(conn));
		}
	}
}
/* }}} */

/* TODO Split this up */
/* {{{ php_mysqli_fetch_into_hash */
void php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAMETERS, int override_flags, int into_object)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	zend_long			fetchtype;
	HashTable			*ctor_params = NULL;
	zend_class_entry *ce = NULL;

	if (into_object) {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|Ch", &mysql_result, mysqli_result_class_entry, &ce, &ctor_params) == FAILURE) {
			RETURN_THROWS();
		}
		if (ce == NULL) {
			ce = zend_standard_class_def;
		}
		if (UNEXPECTED(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))) {
			zend_throw_error(NULL, "Class %s cannot be instantiated", ZSTR_VAL(ce->name));
			RETURN_THROWS();
		}
		fetchtype = MYSQLI_ASSOC;
	} else {
		if (override_flags) {
			if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
				RETURN_THROWS();
			}
			fetchtype = override_flags;
		} else {
			fetchtype = MYSQLI_BOTH;
			if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &fetchtype) == FAILURE) {
				RETURN_THROWS();
			}
		}
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (fetchtype < MYSQLI_ASSOC || fetchtype > MYSQLI_BOTH) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be one of MYSQLI_NUM, MYSQLI_ASSOC, or MYSQLI_BOTH");
		RETURN_THROWS();
	}

	php_mysqli_fetch_into_hash_aux(return_value, result, fetchtype);

	if (into_object && Z_TYPE_P(return_value) == IS_ARRAY) {
		zval dataset;

		ZVAL_COPY_VALUE(&dataset, return_value);

		object_init_ex(return_value, ce);
		HashTable *prop_table = zend_symtable_to_proptable(Z_ARR(dataset));
		zval_ptr_dtor(&dataset);
		if (!ce->default_properties_count && !ce->__set) {
			Z_OBJ_P(return_value)->properties = prop_table;
		} else {
			zend_merge_properties(return_value, prop_table);
			zend_array_release(prop_table);
		}

		if (ce->constructor) {
			zend_call_known_function(ce->constructor, Z_OBJ_P(return_value), Z_OBJCE_P(return_value),
				/* retval */ NULL, /* argc */ 0, /* params */ NULL, ctor_params);
		} else if (ctor_params && zend_hash_num_elements(ctor_params) > 0) {
			zend_argument_value_error(ERROR_ARG_POS(3),
				"must be empty when the specified class (%s) does not have a constructor",
				ZSTR_VAL(ce->name)
			);
		}
	}
}
/* }}} */
