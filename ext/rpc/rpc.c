#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_rpc.h"

#include "layer.h"

ZEND_DECLARE_MODULE_GLOBALS(rpc)

static zend_object_value rpc_create_object(zend_class_entry *class_type TSRMLS_DC);

/* object handler */
static void rpc_add_ref(zval *object);
static void rpc_del_ref(zval *object);
static void rpc_delete(zval *object);
static zend_object_value rpc_clone(zval *object);
static zval* rpc_read(zval *object, zval *member, int type TSRMLS_DC);
static void rpc_write(zval *object, zval *member, zval *value TSRMLS_DC);
static zval** rpc_get_property(zval *object, zval *member TSRMLS_DC);
static zval** rpc_get_property_zval(zval *object, zval *member TSRMLS_DC);
static zval* rpc_get(zval *property TSRMLS_DC);
static void rpc_set(zval **property, zval *value TSRMLS_DC);
static int rpc_has_property(zval *object, zval *member, int check_empty TSRMLS_DC);
static void rpc_unset_property(zval *object, zval *member TSRMLS_DC);
static HashTable* rpc_get_properties(zval *object TSRMLS_DC);
static union _zend_function* rpc_get_method(zval *object, char *method, int method_len TSRMLS_DC);
static int rpc_call(char *method, INTERNAL_FUNCTION_PARAMETERS);
static union _zend_function* rpc_get_constructor(zval *object TSRMLS_DC);
static int rpc_get_classname(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC);
static int rpc_compare(zval *object1, zval *object2 TSRMLS_DC);
/**/

ZEND_FUNCTION(rpc_ctor);

static int le_rpc;
static zend_object_handlers rpc_handlers = {
	rpc_add_ref,
	rpc_del_ref,
	rpc_delete,
	rpc_clone,
	rpc_read,
	rpc_write,
	rpc_get_property,
	rpc_get_property_zval,
	rpc_get,
	rpc_set,
	rpc_has_property,
	rpc_unset_property,
	rpc_get_properties,
	rpc_get_method,
	rpc_call,
	rpc_get_constructor,
	rpc_get_classname,
	rpc_compare
};

/* {{{ rpc_functions[]
 */
function_entry rpc_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ rpc_module_entry
 */
zend_module_entry rpc_module_entry = {
	STANDARD_MODULE_HEADER,
	"rpc",
	rpc_functions,
	PHP_MINIT(rpc),
	PHP_MSHUTDOWN(rpc),
	NULL,
	NULL,
	PHP_MINFO(rpc),
	"0.1a",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_RPC
ZEND_GET_MODULE(rpc)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
/* TODO: add module specific ini settings here */
PHP_INI_END()
/* }}} */

/* {{{ php_rpc_init_globals
 */
static void php_rpc_init_globals(zend_rpc_globals *rpc_globals)
{
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(rpc)
{
	int i;

	ZEND_INIT_MODULE_GLOBALS(rpc, php_rpc_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	RPC_G(handle) = 0;
	ALLOC_HASHTABLE(RPC_G(instance));
	zend_hash_init(RPC_G(instance), 0, NULL, ZVAL_PTR_DTOR, 0);
	
	for (i=0; i < (sizeof(handler_entries) / sizeof(rpc_handler_entry)); i++) {
		handler_entries[i].rpc_handler_init();
		
		INIT_OVERLOADED_CLASS_ENTRY((*(handler_entries[i].ce)),
									handler_entries[i].name,
									NULL,
									NULL,
									NULL,
									NULL);

		handler_entries[i].ce->create_object = rpc_create_object;

		zend_register_internal_class(handler_entries[i].ce TSRMLS_CC);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(rpc)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rpc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rpc support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static zend_object_value rpc_create_object(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value *zov;
	
	zov = (zend_object_value*) emalloc(sizeof(zend_object_value));
	zov->handle = RPC_G(handle)++;
	zov->handlers = &rpc_handlers;

	zend_hash_index_update_or_next_insert(RPC_G(instance), zov->handle, class_type, sizeof(zend_class_entry),NULL, HASH_ADD);

	return *zov;
}


static void rpc_add_ref(zval *object)
{
}

static void rpc_del_ref(zval *object)
{
}

static void rpc_delete(zval *object)
{
}

static zend_object_value rpc_clone(zval *object)
{
}

static zval* rpc_read(zval *object, zval *member, int type TSRMLS_DC)
{
}

static void rpc_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
}

static zval** rpc_get_property(zval *object, zval *member TSRMLS_DC)
{
}

static zval **rpc_get_property_zval(zval *object, zval *member TSRMLS_DC)
{
}

static zval* rpc_get(zval *property TSRMLS_DC)
{
}

static void rpc_set(zval **property, zval *value TSRMLS_DC)
{
}

static int rpc_has_property(zval *object, zval *member, int check_empty TSRMLS_DC)
{
}

static void rpc_unset_property(zval *object, zval *member TSRMLS_DC)
{
}

static HashTable* rpc_get_properties(zval *object TSRMLS_DC)
{
}

static union _zend_function* rpc_get_method(zval *object, char *method, int method_len TSRMLS_DC)
{
}

static int rpc_call(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *object = getThis();
}

static union _zend_function* rpc_get_constructor(zval *object TSRMLS_DC)
{
	zend_function *rpc_ctor;
	
	rpc_ctor = (zend_function *) emalloc(sizeof(zend_function));

	rpc_ctor->common.function_name = "__construct";
	zend_hash_index_find(RPC_G(instance), object->value.obj.handle, &(rpc_ctor->common.scope));

	return rpc_ctor;
}

static int rpc_get_classname(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
}

static int rpc_compare(zval *object1, zval *object2 TSRMLS_DC)
{
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
