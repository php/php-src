/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#include "../rpc.h"
#include "../handler.h"

#include "skeleton.h"

/* protos */
static int skeleton_hash(rpc_string, rpc_string *, void *, int, char *, int);
static int skeleton_name(rpc_string, rpc_string *, void *, int);
static int skeleton_ctor(rpc_string, void **, int , zval ***);
static int skeleton_dtor(void *);
static int skeleton_describe(rpc_string, void *, char **, unsigned char **);
static int skeleton_call(rpc_string, void **, zval *, int, zval ***);
static int skeleton_get(rpc_string, zval *, void **);
static int skeleton_set(rpc_string, zval *, void **);
static int skeleton_skeletonpare(void **, void **);
static int skeleton_has_property(rpc_string, void **);
static int skeleton_unset_property(rpc_string, void **);
static int skeleton_get_properties(HashTable **, void **);

/* register rpc callback function */
RPC_REGISTER_HANDLERS_BEGIN(skeleton)
FALSE,						/* poolable TRUE|FALSE*/
DONT_HASH,					/* hash function name lookups to avoid reflection of the object for each
				             * method call. hashing is done either by mapping only the function name
							 * to a coresponding method id or by taking the whole method signature into
							 * account. possible values:
							 * DONT_HASH|HASH_AS_INT|HASH_AS_STRING|
							 * HASH_AS_INT_WITH_SIGNATURE|HASH_AS_STRING_WITH_SIGNATURE
							 */
skeleton_hash,				/* the hash function, can be NULL */
skeleton_name,				/* the reverse hash function, can be NULL */
skeleton_ctor,				/* constructor */
skeleton_dtor,				/* destructor */
skeleton_describe,			/* function to reflect methods to get information about parameter types.
							 * parameters can be forced to be by reference this way. can be NULL.
							 */
skeleton_call,				/* method call handler */
skeleton_get,				/* property get handler */
skeleton_set,				/* property set handler */
skeleton_compare,			/* compare handler, can be NULL */
skeleton_has_property,		/* reflection functions */
skeleton_unset_property,	/* can be NULL */
skeleton_get_properties
RPC_REGISTER_HANDLERS_END()

/* register ini settings */
PHP_INI_BEGIN()
	/* TODO: palce your ini entries here */
PHP_INI_END()

/* register userspace functions */
RPC_FUNCTION_ENTRY_BEGIN(skeleton)
	/* TODO: add your userspace functions here */
	ZEND_FE(skeleton_function, NULL)
RPC_FUNCTION_ENTRY_END()

/* register class methods */
RPC_METHOD_ENTRY_BEGIN(skeleton)
	/* TODO: add your class methods here */
	ZEND_FALIAS(method, skeleton_function, NULL)
RPC_METHOD_ENTRY_END()

zend_module_entry skeleton_module_entry = {
	ZE2_STANDARD_MODULE_HEADER,
	"skeleton",
	RPC_FUNCTION_ENTRY(skeleton),
	ZEND_MINIT(skeleton),
	ZEND_MSHUTDOWN(skeleton),
	NULL,
	NULL,
	ZEND_MINFO(skeleton),
	"0.1a",
	STANDARD_MODULE_PROPERTIES
};

ZEND_MINIT_FUNCTION(skeleton)
{
	/* TODO: place your init stuff here */

	RPC_REGISTER_LAYER(skeleton);
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(skeleton)
{
	/* TODO: place your shutdown stuff here */

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

ZEND_MINFO_FUNCTION(skeleton)
{
	DISPLAY_INI_ENTRIES();
}

#ifdef COMPILE_DL_COM
ZEND_GET_MODULE(skeleton);
#endif

/* rpc handler functions */
static int skeleton_hash(rpc_string name, rpc_string *hash, void *data, int num_args, char *arg_types, int type)
{
	/* TODO: implement your hash function here. if you have specified any of the HASH_AS_INT constants, simply set
	 * hash->str to NULL and set hash->len to the int hash value.
	 * arg_types is a zend_parse_parameters() like string containing the types of the parameters passed enabling you
	 * to find the best match if you want to hash WITH_SIGNATURE.
	 * type is one of CLASS|METHOD|PROPERTY.
	 */
	hash->str = strdup(name.str);
	hash->len = name.len;

	return SUCCESS;
}

static int skeleton_name(rpc_string hash, rpc_string *name, void *data, int type)
{
	/* TODO: do the opposite of what you did above */
	return FAILURE;
}

static int skeleton_ctor(rpc_string class_name, void **data, int num_args, zval **args[])
{
	/* TODO: use *data as a pointer to your internal data. if you want to enable your instances for
	 * pooling or to be used as singletons then you have to use malloc() and free() instead of
	 * emalloc() and efree() because emalloc()'ed memory will be efree()'ed on script shutdown.
	 * ATTENTION: take care about possible memory holes when you use malloc()
	 * calls to the handler functions are mutual exclusive per userspace instance, thus if you use
	 * the same internal datastructure accross multiple userspace instances of php objects you have
	 * to care for thread safety yourself (this again applies only if you want to make your instances
	 * poolable/singleton-able), if you have an internal data structure per instance, then you don't
	 * have to care for thread safety as the handler functions are locked by a mutex.
	 */
	return SUCCESS;
}

static int skeleton_dtor(void *data)
{
	/* TODO: free everything you alloc'ed above */
	return SUCCESS;
}

static int skeleton_describe(rpc_string method_name, void *data, char **arg_types, unsigned char **ref_types)
{
	/* TODO: return a zend_parse_parameters() like string in arg_types to describe the
	 * parameters taken by the specific function. if one of the parameters should be forced be reference then
	 * you have to set ref_types to an array describing the function parameters as you would in the
	 * ZEND_FE() macro as the last parameter.
	 */
	return SUCCESS;
}

static int skeleton_call(rpc_string method_name, void **data, zval *return_value, int num_args, zval **args[])
{
	/* TODO: implement call handler. if you passed back an arg_types string in the describe function the arguments
	 * are already converted to the corresponding types, if there are too few or too much, a warning is already issued.
	 * if arg_types was NULL you have to check for the right parameter count and types yourself.
	 */
	return SUCCESS;
}

static int skeleton_get(rpc_string property_name, zval *return_value, void **data)
{
	/* TODO: implement get handler */
	return SUCCESS;
}

static int skeleton_set(rpc_string property_name, zval *value, void **data)
{
	/* TODO: implement set handler */
	return SUCCESS;
}

static int skeleton_compare(void **data1, void **data2)
{
	/* TODO: implement compare handler */
	return SUCCESS;
}

static int skeleton_has_property(rpc_string property_name, void **data)
{
	/* TODO: implement has property handler */
	return SUCCESS;
}

static int skeleton_unset_property(rpc_string property_name, void **data)
{
	/* TODO: implement unset property handler */
	return SUCCESS;
}

static int skeleton_get_properties(HashTable **properties, void **data)
{
	/* TODO: implement get properties handler */
	return SUCCESS;
}


/* custom functions */
ZEND_FUNCTION(skeleton_function)
{
}