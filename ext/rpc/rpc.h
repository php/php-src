#ifndef RPC_H
#define RPC_H

#define FOREACH_HANDLER for (__handler_counter=0; __handler_counter < HANDLER_COUNT; __handler_counter++)
#define HANDLER handler_entries[__handler_counter]
#define HANDLER_COUNT (sizeof(handler_entries) / sizeof(rpc_handler_entry))

#define GET_INTERNAL(intern)	rpc_internal **intern;			\
								GET_INTERNAL_EX(intern, object)
#define GET_INTERNAL_EX(intern, object)	if (zend_hash_index_find(instance, object->value.obj.handle, (void **) &intern) == FAILURE) {	\
											/* TODO: exception */																		\
										}

#define RPC_REFCOUNT(intern) ((*intern)->refcount)
#define RPC_ADDREF(intern) (++RPC_REFCOUNT(intern))
#define RPC_DELREF(intern) (--RPC_REFCOUNT(intern))

static int __handler_counter;

#endif