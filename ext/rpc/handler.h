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

#ifndef HANDLER_H
#define HANDLER_H

#include "php.h"
#include "php_ini.h"

#define RPC_REGISTER_LAYER(layer)		rpc_register_layer(&layer##_handler_entry TSRMLS_CC);
#define RPC_DECLARE_HANDLER(layer)		rpc_object_handlers layer##_object_handlers;				\
										zend_class_entry *layer##_class_entry;						\
										function_entry layer##_function_entry[];					\
										function_entry layer##_method_entry[];						\
										static rpc_handler_entry layer##_handler_entry =			\
											{#layer, &layer##_object_handlers, &layer##_class_entry,\
											layer##_function_entry, layer##_method_entry}

#define RPC_REGISTER_HANDLERS_BEGIN(layer)		zend_class_entry *layer##_class_entry;				\
												rpc_object_handlers layer##_object_handlers = {

#define RPC_REGISTER_HANDLERS_END()				};

#define RPC_FUNCTION_ENTRY(layer)		layer##_function_entry
#define RPC_FUNCTION_ENTRY_BEGIN(layer)	function_entry layer##_function_entry[] = {				\
											ZEND_FALIAS(layer##_load, rpc_load, NULL)			\
											ZEND_FALIAS(layer##_call, rpc_call, NULL)			\
											ZEND_FALIAS(layer##_get, rpc_get, NULL)				\
											ZEND_FALIAS(layer##_set, rpc_get, NULL)				\
											ZEND_FALIAS(layer##_singleton, rpc_singleton, NULL)	\
											ZEND_FALIAS(layer##_poolable, rpc_poolable, NULL)

#define RPC_FUNCTION_ENTRY_END()			{NULL, NULL, NULL}							\
										};

#define RPC_METHOD_ENTRY_BEGIN(layer)	function_entry layer##_method_entry[] = {

#define RPC_METHOD_ENTRY_END()				{NULL, NULL, NULL}							\
										};

#define DONT_HASH	0
#define HASH_AS_INT	1
#define HASH_AS_STRING 2
#define HASH_WITH_SIGNATURE 4
#define HASH_AS_INT_WITH_SIGNATURE (HASH_AS_INT & HASH_WITH_SIGNATURE)
#define HASH_AS_STRING_WITH_SIGNATURE (HASH_AS_STRING & HASH_WITH_SIGNATURE)

#define CLASS 0
#define METHOD 1
#define PROPERTY 2


/* string */
typedef struct _rpc_string {
	char *str;
	zend_uint len;
} rpc_string;

/* rpc handler that have to be implemented by a 
 * specific rpc layer
 */
typedef struct _rpc_object_handlers {
	const zend_bool poolable;
	const zend_uint hash_type;
	int (*rpc_hash)(rpc_string name, rpc_string *hash, void *data, int num_args, char *arg_types, int type);
	int (*rpc_name)(rpc_string hash, rpc_string *name, void *data, int type);
	int (*rpc_ctor)(rpc_string class_name, void **data, int num_args, zval **args[]);
	int (*rpc_dtor)(void *data);
	int (*rpc_describe)(rpc_string method_name, void *data, char **arg_types, unsigned char **ref_types);
	int (*rpc_call)(rpc_string method_name, void **data, zval *return_value, int num_args, zval **args[]);
	int (*rpc_get)(rpc_string property_name, zval *return_value, void **data);
	int (*rpc_set)(rpc_string property_name, zval *value, void **data);
	int (*rpc_compare)(void **data1, void **data2);
	int (*rpc_has_property)(rpc_string property_name, void **data);
	int (*rpc_unset_property)(rpc_string property_name, void **data);
	int (*rpc_get_properties)(HashTable **properties, void **data);
} rpc_object_handlers;

/* handler entry */
typedef struct _rpc_handler_entry {
	char					*name;
	rpc_object_handlers		*handlers;
	zend_class_entry		**ce;
	function_entry			*functions;
	function_entry			*methods;
} rpc_handler_entry;

/* class/method/function hash */
typedef struct _rpc_class_hash {
	rpc_string			name;		/* must be first entry */
	zend_bool			poolable;
	zend_bool			singleton;
	TsHashTable			methods;
	TsHashTable			properties;
	rpc_object_handlers	**handlers;
	zend_class_entry	*ce;
	void				*data;
} rpc_class_hash;

/* internal data */
typedef struct _rpc_internal {
	MUTEX_T					mx_handler;
	TsHashTable				function_table;
	rpc_object_handlers		**handlers;
	rpc_class_hash			*hash;
	zend_class_entry		*ce;
	void					*data;
} rpc_internal;

/* proxy data */
typedef struct _rpc_proxy {
	zend_uint				dummy;
} rpc_proxy;

ZEND_API rpc_register_layer(rpc_handler_entry *entry TSRMLS_DC);

#endif /* HANDLER_H */