#include "com.h"

/* protos */
static int com_hash(rpc_string, rpc_string *, int, char *, int);
static int com_name(rpc_string, rpc_string *, int);
static int com_ctor(rpc_string, void **, int , zval ***);
static int com_dtor(void **);
static int com_call(rpc_string, void **, zval **, int, zval ***);
static int com_get(rpc_string, zval *, void **);
static int com_set(rpc_string, zval *, void **);
static int com_compare(void **, void **);
static int com_has_property(rpc_string, void **);
static int com_unset_property(rpc_string, void **);
static int com_get_properties(HashTable **, void **);


/* register rpc callback function */
RPC_REGISTER_HANDLERS_START(com)
TRUE,							/* poolable */
HASH_AS_INT_WITH_SIGNATURE,
com_hash,
com_name,
com_ctor,
com_dtor,
com_call,
com_get,
com_set,
com_compare,
com_has_property,
com_unset_property,
com_get_properties
RPC_REGISTER_HANDLERS_END()

/* register ini settings */
RPC_INI_START(com)
PHP_INI_ENTRY_EX("com.allow_dcom", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_typelib", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_verbose", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_casesensitive", "1", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
RPC_INI_END()

/* register userspace functions */
RPC_FUNCTION_ENTRY_START(com)
	ZEND_FALIAS(com_invoke, rpc_call, NULL)
	ZEND_FE(com_addref, NULL)
	ZEND_FE(com_release, NULL)
RPC_FUNCTION_ENTRY_END()

/* register class methods */
RPC_METHOD_ENTRY_START(com)
	ZEND_FALIAS(addref, com_addref, NULL)
	ZEND_FALIAS(release, com_release, NULL)
RPC_METHOD_ENTRY_END()

/* init function that is called before the class is registered
 * so you can do any tricky stuff in here
 */
RPC_INIT_FUNCTION(com)
{
}

/* rpc handler functions */

static int com_hash(rpc_string name, rpc_string *hash, int num_args, char *arg_types, int type)
{
	hash->str = strdup(name.str);
	hash->len = name.len;

	return SUCCESS;
}

static int com_name(rpc_string hash, rpc_string *name, int type)
{
	name->str = strdup(hash.str);
	name->len = hash.len;

	return SUCCESS;
}

static int com_ctor(rpc_string class_name, void **data, int num_args, zval **args[])
{
	return SUCCESS;
}

static int com_dtor(void **data)
{
	return SUCCESS;
}

static int com_call(rpc_string method_name, void **data, zval **return_value, int num_args, zval **args[])
{
	return SUCCESS;
}

static int com_get(rpc_string property_name, zval *return_value, void **data)
{
	return SUCCESS;
}

static int com_set(rpc_string property_name, zval *value, void **data)
{
	return SUCCESS;
}

static int com_compare(void **data1, void **data2)
{
	return SUCCESS;
}

static int com_has_property(rpc_string property_name, void **data)
{
	return SUCCESS;
}

static int com_unset_property(rpc_string property_name, void **data)
{
	return SUCCESS;
}

static int com_get_properties(HashTable **properties, void **data)
{
	return SUCCESS;
}

/* custom functions */
ZEND_FUNCTION(com_addref)
{
}

ZEND_FUNCTION(com_release)
{
}