#include "com.h"

/* protos */
static int com_hash(char *name, zend_uint name_len, char **hash, zend_uint *hash_len, int type);
static int com_ctor(char *class_name, zend_uint class_name_len, void **data, int num_args, zval **args[]);
static int com_dtor(void **data);
static int com_call(char *method_name, zend_uint method_name_len, void **data, INTERNAL_FUNCTION_PARAMETERS);
static int com_get(char *property_name, zend_uint property_name_len, zval *return_value, void **data);
static int com_set(char *property_name, zend_uint property_name_len, zval *value, zval *return_value, void **data);
static int com_compare(void **data1, void **data2);
static int com_get_classname(char **class_name, zend_uint *class_name_length, void **data);
static int com_has_property(char *property_name, zend_uint property_name_length, void **data);
static int com_unset_property(char *property_name, zend_uint property_name_length, void **data);
static int com_get_properties(HashTable **properties, void **data);


/* register rpc callback function */
RPC_REGISTER_HANDLERS_START(com)
POOL,
HASH_AS_INT,
com_hash,
com_ctor,
com_dtor,
com_call,
com_get,
com_set,
com_compare,
com_get_classname,
com_has_property,
com_unset_property,
com_get_properties
RPC_REGISTER_HANDLERS_END()

/* register userspace functions */
RPC_FUNCTION_ENTRY_START(com)
	ZEND_FALIAS(com_invoke, rpc_call, NULL)
	ZEND_FE(com_addref, NULL)
RPC_FUNCTION_ENTRY_END()

/* register class methods */
RPC_METHOD_ENTRY_START(com)
	ZEND_FALIAS(addref, com_addref, NULL)
RPC_METHOD_ENTRY_END()

/* init function that is called before the class is registered
 * so you can do any tricky stuff in here
 */
RPC_INIT_FUNCTION(com)
{
}

/* rpc handler functions */

static int com_hash(char *name, zend_uint name_len, char **hash, zend_uint *hash_len, int type)
{
	*hash = strdup(name);
	*hash_len = name_len;

	return SUCCESS;
}

static int com_ctor(char *class_name, zend_uint class_name_len, void **data, int num_args, zval **args[])
{
	return SUCCESS;
}

static int com_dtor(void **data)
{
	return SUCCESS;
}

static int com_call(char *method_name, zend_uint method_name_len, void **data, INTERNAL_FUNCTION_PARAMETERS)
{
	return SUCCESS;
}

static int com_get(char *property_name, zend_uint property_name_len, zval *return_value, void **data)
{
	return SUCCESS;
}

static int com_set(char *property_name, zend_uint property_name_len, zval *value, zval *return_value, void **data)
{
	return SUCCESS;
}

static int com_compare(void **data1, void **data2)
{
	return SUCCESS;
}

static int com_get_classname(char **class_name, zend_uint *class_name_length, void **data)
{
	return SUCCESS;
}

static int com_has_property(char *property_name, zend_uint property_name_length, void **data)
{
	return SUCCESS;
}

static int com_unset_property(char *property_name, zend_uint property_name_length, void **data)
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