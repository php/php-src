#ifndef HANDLER_H
#define HANDLER_H

#include "php.h"

#define RPC_HANDLER(layer)				{#layer, layer##_handler_init, &layer##_object_handlers,	\
										&layer##_class_entry, layer##_function_entry,				\
										layer##_method_entry}

#define RPC_DECLARE_HANDLER(layer)		void layer##_handler_init();					\
										rpc_object_handlers layer##_object_handlers;	\
										zend_class_entry layer##_class_entry;			\
										function_entry layer##_function_entry[];		\
										function_entry layer##_method_entry[];

#define RPC_INIT_FUNCTION(layer)		void layer##_handler_init()

#define RPC_REGISTER_HANDLERS_START(layer)		zend_class_entry layer##_class_entry;				\
												rpc_object_handlers layer##_object_handlers = {

#define RPC_REGISTER_HANDLERS_END()				};

  
#define RPC_FUNCTION_ENTRY_START(layer)	function_entry layer##_function_entry[] = {		\
											ZEND_FALIAS(layer##_load, rpc_load, NULL)	\
											ZEND_FALIAS(layer##_call, rpc_call, NULL)	\
											ZEND_FALIAS(layer##_get, rpc_get, NULL)		\
											ZEND_FALIAS(layer##_set, rpc_set, NULL)

#define RPC_FUNCTION_ENTRY_END()			{NULL, NULL, NULL}							\
										};

#define RPC_METHOD_ENTRY_START(layer)	function_entry layer##_method_entry[] = {

#define RPC_METHOD_ENTRY_END()				{NULL, NULL, NULL}							\
										};

#define POOL		TRUE
#define DONT_POOL	FALSE

#define DONT_HASH	0
#define HASH_AS_INT	1
#define HASH_AS_STRING 2
#define HASH_WITH_SIGNATURE 4
#define HASH_AS_INT_WITH_SIGNATURE (HASH_AS_INT + HASH_WITH_SIGNATURE)
#define HASH_AS_STRING_WITH_SIGNATURE (HASH_AS_STRING + HASH_WITH_SIGNATURE)

#define CLASS 0
#define METHOD 1
#define PROPERTY 2


/* rpc handler that have to be implemented by a 
 * specific rpc layer
 */
typedef struct _rpc_object_handlers {
	const zend_bool pool_instances;
	const zend_uint hash_type;
	int (*rpc_hash)(char *name, zend_uint name_len, char **hash, zend_uint *hash_len, int num_args, zval **args[], int type);
	int (*rpc_ctor)(char *class_name, zend_uint class_name_len, void **data, int num_args, zval **args[]);
	int (*rpc_dtor)(void **data);
	int (*rpc_call)(char *method_name, zend_uint method_name_len, void **data, zval **return_value, int num_args, zval **args[]);
	int (*rpc_get)(char *property_name, zend_uint property_name_len, zval *return_value, void **data);
	int (*rpc_set)(char *property_name, zend_uint property_name_len, zval *value, zval *return_value, void **data);
	int (*rpc_compare)(void **data1, void **data2);
	int (*rpc_get_classname)(char **class_name, zend_uint *class_name_length, void **data);
	int (*rpc_has_property)(char *property_name, zend_uint property_name_length, void **data);
	int (*rpc_unset_property)(char *property_name, zend_uint property_name_length, void **data);
	int (*rpc_get_properties)(HashTable **properties, void **data);
} rpc_object_handlers;

/* handler entry */
typedef struct _rpc_handler_entry {
	char				*name;
	void (*rpc_handler_init)();
	rpc_object_handlers	*handlers;
	zend_class_entry	*ce;
	function_entry		*functions;
	function_entry		*methods;
} rpc_handler_entry;

/* string */
typedef struct _rpc_string {
	char *str;
	zend_uint len;
} rpc_string;

/* class/method/function hash */
typedef struct _rpc_class_hash {
	rpc_string name; /* must be first entry */
	WormHashTable methods;
	WormHashTable properties;
} rpc_class_hash;

/* internal data */
typedef struct _rpc_internal {
	char				*class_name;
	zend_uint			class_name_len;
	zend_class_entry	*ce;
	rpc_object_handlers	**handlers;
	void				*data;
	zend_uint			refcount;
	zend_uint			clonecount;
	zend_bool			pool_instances;
	rpc_class_hash		*hash;
	WormHashTable		function_table;
	MUTEX_T				mx_handler;
} rpc_internal;


#endif /* HANDLER_H */