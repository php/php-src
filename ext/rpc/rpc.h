#ifndef RPC_H
#define RPC_H

#define FOREACH_HANDLER for (__handler_counter=0; __handler_counter < HANDLER_COUNT; __handler_counter++)
#define HANDLER handler_entries[__handler_counter]
#define HANDLER_COUNT (sizeof(handler_entries) / sizeof(rpc_handler_entry))

#define GET_INTERNAL(intern)	rpc_internal **intern;								\
								if (GET_INTERNAL_EX(intern, object) != SUCCESS) {	\
									/* TODO: exception */							\
								}

#define GET_INTERNAL_EX(intern, object)	zend_worm_hash_index_find(instance, object->value.obj.handle, (void **) &intern)

#define GET_CLASS(ce)	char *key;																				\
						int key_len;																			\
						zend_class_entry **ce;																	\
																												\
						/* the name of the rpc layer is prepended to '_load' so lets strip everything after		\
						 * the first '_' away from the function name											\
						 */																						\
						key = estrdup(get_active_function_name(TSRMLS_C));										\
						key_len = strchr(key, '_') - key;														\
						key[key_len] = '\0';																	\
																												\
						/* get the class entry for the requested rpc layer */									\
						if (zend_hash_find(CG(class_table), key, key_len + 1, (void **) &ce) != SUCCESS) {		\
							efree(key);																			\
							/* TODO: exception here */															\
						} else {																				\
							efree(key);																			\
						}

#define GET_ARGS_EX(num_args, args, args_free, strip)										\
						GET_ARGS(num_args, args)											\
																							\
						args_free = args;													\
																							\
						/* strip away the first parameters */								\
						num_args -= strip;													\
						args = (num_args > 0) ? &args[strip] : NULL;

#define GET_ARGS(num_args, args)															\
						args = (zval ***) emalloc(sizeof(zval **) * num_args);				\
																							\
						if (zend_get_parameters_array_ex(num_args, args) != SUCCESS) {		\
							efree(args);													\
							/* TODO: exception */											\
						}

#define RPC_REFCOUNT(intern) ((*intern)->refcount)
#define RPC_ADDREF(intern) (++RPC_REFCOUNT(intern))
#define RPC_DELREF(intern) (--RPC_REFCOUNT(intern))

#define RPC_CLONECOUNT(intern) ((*intern)->clonecount)
#define RPC_ADDCLONE(intern) (++RPC_CLONECOUNT(intern))
#define RPC_DELCLONE(intern) (--RPC_CLONECOUNT(intern))

static int __handler_counter;

#endif