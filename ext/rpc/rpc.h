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

#ifndef RPC_H
#define RPC_H

#define RPC_HT(intern) (*((intern)->handlers))
#define RPC_CLASS(intern) ((intern)->hash)

#define GET_INTERNAL(intern)	rpc_internal *intern;								\
								if (GET_INTERNAL_EX(intern, object) != SUCCESS) {	\
									/* TODO: exception */							\
								}

#define GET_INTERNAL_EX(intern, object)	(((intern = zend_object_store_get_object(object TSRMLS_CC)) == NULL) ? FAILURE : SUCCESS)

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

#define GET_METHOD_SIGNATURE(intern, method, hash_val, num_args, arg_types)											\
			GET_SIGNATURE(intern, method->str, method->len, hash_val, num_args, arg_types)

#define GET_SIGNATURE(intern, name, name_len, hash_val, num_args, arg_types)										\
						hash_val.len = name_len;																	\
																													\
						if ((*intern->handlers)->hash_type & HASH_WITH_SIGNATURE) {								\
							zend_uint _signature_counter;															\
																													\
							arg_types = (char *) emalloc(sizeof(char) * (num_args + 1));							\
							hash_val.len += num_args + 1;															\
																													\
							for (_signature_counter = 0; _signature_counter < num_args; _signature_counter++) {		\
								switch (Z_TYPE_PP(args[_signature_counter])) {										\
									case IS_NULL:																	\
										arg_types[_signature_counter] = 'n';										\
										break;																		\
									case IS_LONG:																	\
										arg_types[_signature_counter] = 'l';										\
										break;																		\
									case IS_DOUBLE:																	\
										arg_types[_signature_counter] = 'd';										\
										break;																		\
									case IS_STRING:																	\
										arg_types[_signature_counter] = 's';										\
										break;																		\
									case IS_ARRAY:																	\
										arg_types[_signature_counter] = 'a';										\
										break;																		\
									case IS_OBJECT:																	\
										arg_types[_signature_counter] = 'o';										\
										break;																		\
									case IS_BOOL:																	\
										arg_types[_signature_counter] = 'b';										\
										break;																		\
									case IS_RESOURCE:																\
										arg_types[_signature_counter] = 'r';										\
										break;																		\
									default:																		\
										arg_types[_signature_counter] = 'u';										\
								}																					\
							}																						\
																													\
							arg_types[_signature_counter] = '\0';													\
						} else {																					\
							arg_types = (char *) emalloc(sizeof(char));												\
							arg_types[0] = '\0';																	\
						}																							\
																													\
						hash_val.str = (char *) emalloc(sizeof(char) * (hash_val.len + 2));							\
						memcpy(hash_val.str, arg_types, num_args + 1);												\
						memcpy(&hash_val.str[hash_val.len - name_len],												\
							   name, name_len + 1);

#define FREE_SIGNATURE(hash_val, arg_types)																			\
						efree(arg_types);																			\
						efree(hash_val.str);

#define ALLOC_CLASS_HASH(_class_hash, _handlers) \
		if (_class_hash = pemalloc(sizeof(rpc_class_hash), TRUE)) { \
			/* set up the cache */ \
			zend_ts_hash_init(&(_class_hash->methods), 0, NULL, rpc_string_dtor, TRUE); \
			zend_ts_hash_init(&(_class_hash->properties), 0, NULL, rpc_string_dtor, TRUE); \
			_class_hash->singleton = FALSE; \
			_class_hash->poolable = FALSE; \
			_class_hash->data = NULL; \
			_class_hash->handlers = _handlers; \
		}

#define INIT_RPC_OBJECT(__intern, __clh) \
			(__intern)->ce = (__clh)->ce; \
			(__intern)->function_table.hash = (__intern)->ce->function_table;

#define OVERLOAD_RPC_CLASS(__name, __intern, __clh) { \
			zend_class_entry overloaded_class_entry; \
			INIT_CLASS_ENTRY(overloaded_class_entry, NULL, NULL); \
			overloaded_class_entry.name = __name.str; \
			overloaded_class_entry.name_length = (__name.str != NULL) ? __name.len : 0; \
			(__clh)->ce = zend_register_internal_class_ex(&overloaded_class_entry, (__intern)->ce, NULL TSRMLS_CC); \
			INIT_RPC_OBJECT(__intern, __clh); \
		}

#define  SEPARATE_RPC_CLASS(__intern) \
			(__intern)->free_function_table = 1; \
			zend_ts_hash_init(&((__intern)->function_table), 0, NULL, NULL, TRUE); \
			zend_hash_copy(&((__intern)->function_table.hash), &((__intern)->ce->function_table), NULL, NULL, 0);

#define REGISTER_RPC_CLASS(__name, __class_hash) { \
			rpc_class_hash **_tmp; \
			if ((__name).str != NULL) { \
				zend_ts_hash_add(&classes, (__name).str, (__name).len + 1, &(__class_hash), sizeof(rpc_class_hash *), (void **) &_tmp); \
			} \
            \
			tsrm_mutex_lock(classes.mx_writer); \
			zend_llist_add_element(&classes_list, _tmp); \
			tsrm_mutex_unlock(classes.mx_writer); \
			\
			if ((__class_hash)->name.str) { \
				zend_ts_hash_add(&classes, (__class_hash)->name.str, (__class_hash)->name.len + 1, &(__class_hash), sizeof(rpc_class_hash *), NULL); \
			} else { \
				zend_ts_hash_index_update(&classes, class_hash->name.len, &class_hash, sizeof(rpc_class_hash *), NULL); \
			} \
		}


#endif