#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_rpc.h"

#include "rpc.h"
#include "layer.h"

ZEND_DECLARE_MODULE_GLOBALS(rpc)

static void rpc_globals_ctor(zend_rpc_globals *rpc_globals TSRMLS_DC);
static void rpc_instance_dtor(void *pDest);
static void rpc_handlers_dtor(void *pDest);
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

ZEND_FUNCTION(CONSTRUCTOR);

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

static HashTable *instance;

#ifdef COMPILE_DL_RPC
ZEND_GET_MODULE(rpc)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
/* TODO: add module specific ini settings here */
PHP_INI_END()
/* }}} */

static void rpc_globals_ctor(zend_rpc_globals *rpc_globals TSRMLS_DC)
{
	RPC_G(handle) = 0;

	RPC_G(handlers) = (HashTable *) pemalloc(sizeof(HashTable), TRUE);
	RPC_G(instance) = (HashTable *) pemalloc(sizeof(HashTable), TRUE);

	zend_hash_init(RPC_G(handlers), 0, NULL, NULL, TRUE);	
	zend_hash_init(RPC_G(instance), 0, NULL, rpc_instance_dtor, TRUE);	
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(rpc)
{
	int i;

	ZEND_INIT_MODULE_GLOBALS(rpc, rpc_globals_ctor, NULL);
	REGISTER_INI_ENTRIES();

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
		zend_hash_add(RPC_G(handlers), handler_entries[i].name, strlen(handler_entries[i].name), &(handler_entries[i].handlers), sizeof(rpc_object_handlers *), NULL);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(rpc)
{
	zend_hash_destroy(RPC_G(handlers));
	zend_hash_destroy(RPC_G(instance));

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

static void rpc_instance_dtor(void *pDest)
{
	rpc_internal **intern;
	
	intern = (rpc_internal **) pDest;

	/* destruct intern */
}

static zend_object_value rpc_create_object(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value *zov;
	rpc_internal *intern;
	
	zov = (zend_object_value*) pemalloc(sizeof(zend_object_value), TRUE);
	intern = (rpc_internal *) pemalloc(sizeof(rpc_internal), TRUE);

	zov->handle = RPC_G(handle)++;
	zov->handlers = &rpc_handlers;

	intern->ce = class_type;
	intern->refcount = 0;
	intern->data = NULL;
	zend_hash_find(RPC_G(handlers), class_type->name, class_type->name_length, (void **) &(intern->handlers));

	zend_hash_index_update_or_next_insert(RPC_G(instance), zov->handle, &intern, sizeof(rpc_internal *), NULL, HASH_ADD);

	return *zov;
}

/**/

static void rpc_add_ref(zval *object)
{
	ZVAL_ADDREF(object);
}

static void rpc_del_ref(zval *object)
{
	if (ZVAL_REFCOUNT(object) > 0) {
		ZVAL_DELREF(object);
	}

	if (ZVAL_REFCOUNT(object) == 0) {
		rpc_delete(object);
	}
}

static void rpc_delete(zval *object)
{
	TSRMLS_FETCH();
	GET_INTERNAL(intern);

	rpc_instance_dtor(intern);
}

static zend_object_value rpc_clone(zval *object)
{
//	TSRMLS_FETCH();
//	GET_INTERNAL(intern);

	/* FIXME */
	return object->value.obj;
}

static zval* rpc_read(zval *object, zval *member, int type TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return NULL;
}

static void rpc_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
//	GET_INTERNAL(intern);
}

static zval** rpc_get_property(zval *object, zval *member TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return NULL;
}

static zval **rpc_get_property_zval(zval *object, zval *member TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return NULL;
}

static zval* rpc_get(zval *property TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return NULL;
}

static void rpc_set(zval **property, zval *value TSRMLS_DC)
{
//	GET_INTERNAL(intern);
}

static int rpc_has_property(zval *object, zval *member, int check_empty TSRMLS_DC)
{
//	GET_INTERNAL(intern);
	
	/* FIXME */
	return FAILURE;
}

static void rpc_unset_property(zval *object, zval *member TSRMLS_DC)
{
//	GET_INTERNAL(intern);
}

static HashTable* rpc_get_properties(zval *object TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return NULL;
}

static union _zend_function* rpc_get_method(zval *object, char *method, int method_len TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return NULL;
}

static int rpc_call(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
//	zval *object = getThis();
//	GET_INTERNAL(intern);

	return SUCCESS;
}

static union _zend_function* rpc_get_constructor(zval *object TSRMLS_DC)
{
	zend_internal_function *rpc_ctor;
	GET_INTERNAL(intern);
	
	rpc_ctor = (zend_internal_function *) emalloc(sizeof(zend_internal_function));

	rpc_ctor->type = ZEND_INTERNAL_FUNCTION;
	rpc_ctor->scope = (*intern)->ce;
	rpc_ctor->handler = CONSTRUCTOR_FN;

	return (zend_function *) rpc_ctor;
}

static int rpc_get_classname(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
//	GET_INTERNAL(intern);

	/* FIXME */
	return FAILURE;
}

static int rpc_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	/* FIXME */
	return FAILURE;
}

ZEND_FUNCTION(CONSTRUCTOR)
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
