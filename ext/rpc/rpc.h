#ifndef RPC_H
#define RPC_H

#define CONSTRUCTOR __construct
#define CONSTRUCTOR_FN ZEND_FN(__construct)

#define GET_INTERNAL(intern) GET_INTERNAL_EX(intern, object)
#define GET_INTERNAL_EX(intern, object)	rpc_internal **intern;																\
										zend_hash_index_find(RPC_G(instance), object->value.obj.handle, (void **) &intern);

#endif