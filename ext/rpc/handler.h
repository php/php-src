#ifndef HANDLER_H
#define HANDLER_H

#include "php.h"

#define RPC_HANDLER(layer)				{#layer, layer##_handler_init, &layer##_object_handlers,	\
										&layer##_class_entry, layer##_function_entry,				\
										layer##_method_entry, &layer##_pool}

#define RPC_DECLARE_HANDLER(layer)		void layer##_handler_init();					\
										const int layer##_pool;								\
										rpc_object_handlers layer##_object_handlers;	\
										zend_class_entry layer##_class_entry;			\
										function_entry layer##_function_entry[];		\
										function_entry layer##_method_entry[];

#define RPC_INIT_FUNCTION(layer)		void layer##_handler_init()

#define RPC_REGISTER_HANDLERS_START(layer, p)	zend_class_entry layer##_class_entry;				\
												const int layer##_pool = p;							\
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
#define HASH_AS_INT	TRUE
#define HASH_AS_STRING FALSE
#define DONT_HASH	FALSE

#define METHOD 0
#define PROPERTY 1


/* rpc handler that have to be implemented by a 
 * specific rpc layer
 */
typedef struct _rpc_object_handlers {
	int (*rpc_hash)(char *name, zend_uint name_len, char **hash, zend_uint *hash_len, int type);
	int hash_type;
	int (*rpc_ctor)(char *class_name, zend_uint class_name_len, void **data, INTERNAL_FUNCTION_PARAMETERS);
	int (*rpc_dtor)(void **data);
	int (*rpc_call)(char *method_name, zend_uint method_name_len, void **data, INTERNAL_FUNCTION_PARAMETERS);
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
	const int			*pool_instances;
} rpc_handler_entry;

/* internal data */
typedef struct _rpc_internal {
	zend_class_entry	*ce;
	rpc_object_handlers	**handlers;
	void				**data;
	unsigned			refcount;
	unsigned			clonecount;
	int					pool_instances;
} rpc_internal;

#endif /* HANDLER_H */