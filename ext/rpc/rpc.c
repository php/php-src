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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_rpc.h"
#include "rpc.h"
#include "layer.h"
#include "hash.h"

static void rpc_instance_dtor(void *);
static void rpc_class_dtor(void *);
static void rpc_string_dtor(void *);

static void rpc_objects_delete(void *, zend_object_handle TSRMLS_DC);

/* object handler */
static zval* rpc_read(zval *, zval *, int  TSRMLS_DC);
static void rpc_write(zval *, zval *, zval * TSRMLS_DC);
static zval** rpc_get_property(zval *, zval * TSRMLS_DC);
static zval* rpc_get(zval * TSRMLS_DC);
static void rpc_set(zval **, zval * TSRMLS_DC);
static int rpc_has_property(zval *, zval *, int  TSRMLS_DC);
static void rpc_unset_property(zval *, zval * TSRMLS_DC);
static HashTable* rpc_get_properties(zval * TSRMLS_DC);
static union _zend_function* rpc_get_method(zval *, char *, int TSRMLS_DC);
static union _zend_function* rpc_get_constructor(zval * TSRMLS_DC);
static zend_class_entry* rpc_get_class_entry(zval * TSRMLS_DC);
static int rpc_compare(zval *, zval * TSRMLS_DC);
/**/

/* pseudo handler */
static void rpc_internal_get(rpc_internal *, char *, zend_uint, zval *);
static void rpc_internal_set(rpc_internal *, char *, zend_uint, zval *);
/**/

extern zend_object_handlers rpc_proxy_handlers;

static zend_object_handlers rpc_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,

	rpc_read,
	rpc_write,
	rpc_get_property,
	NULL,
	rpc_get,
	rpc_set,
	rpc_has_property,
	rpc_unset_property,
	rpc_get_properties,
	rpc_get_method,
	NULL,
	rpc_get_constructor,
	rpc_get_class_entry,
	NULL,
	rpc_compare
};

/* {{{ rpc_functions[]
 */
function_entry rpc_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ rpc_module_entry
 */
zend_module_entry rpc_module_entry = {
	STANDARD_MODULE_HEADER,
	"rpc",
	rpc_functions,
	ZEND_MINIT(rpc),
	ZEND_MSHUTDOWN(rpc),
	NULL,
	NULL,
	ZEND_MINFO(rpc),
	"0.1a",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

zend_class_entry rpc_class_entry;

static zend_function *rpc_ctor;
static HashTable *handlers;
static TsHashTable *pool;
static TsHashTable *classes;
static zend_llist *classes_list;

#ifdef COMPILE_DL_RPC
ZEND_GET_MODULE(rpc);
#endif

ZEND_INI_BEGIN()
ZEND_INI_END()

/* {{{ ZEND_MINIT_FUNCTION
 */
ZEND_MINIT_FUNCTION(rpc)
{
	zend_internal_function *zif;
	zend_class_entry *rpc_entry;

	/* rpc base class entry */
	INIT_CLASS_ENTRY(rpc_class_entry, "rpc", NULL);
	rpc_entry = zend_register_internal_class(&rpc_class_entry TSRMLS_CC);

	handlers = (HashTable *) pemalloc(sizeof(HashTable), TRUE);
	pool = (TsHashTable *) pemalloc(sizeof(TsHashTable), TRUE);
	classes = (TsHashTable *) pemalloc(sizeof(TsHashTable), TRUE);
	classes_list = (zend_llist *) pemalloc(sizeof(zend_llist), TRUE);

	zend_hash_init(handlers, 0, NULL, NULL, TRUE);	
	zend_ts_hash_init(pool, sizeof(rpc_internal **), NULL, rpc_instance_dtor, TRUE);
	zend_ts_hash_init(classes, 0, NULL, NULL, TRUE);
	zend_llist_init(classes_list, sizeof(rpc_class_hash **), rpc_class_dtor, TRUE);

	FOREACH_HANDLER {
		/*
			handle = DL_LOAD(path);
	if (!handle) {
#ifndef ZEND_WIN32
		fprintf(stderr, "Failed loading %s:  %s\n", path, DL_ERROR());
#else
		fprintf(stderr, "Failed loading %s\n", path);
#endif
		return FAILURE;
	}

	extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
	new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if (!extension_version_info || !new_extension) {
		fprintf(stderr, "%s doesn't appear to be a valid Zend extension\n", path);
		return FAILURE;
	}
*/
		zend_class_entry ce;

		HANDLER.rpc_handler_init(module_number TSRMLS_CC);
		
		/* create a class entry for every rpc handler */
		INIT_CLASS_ENTRY(ce,
						 HANDLER.name,
						 HANDLER.methods);

		ce.create_object = rpc_objects_new;

		/* load all available rpc handler into a hash */
		zend_hash_add(handlers, HANDLER.name, strlen(HANDLER.name) + 1, &(HANDLER.handlers), sizeof(rpc_object_handlers *), NULL);

		/* register classes and functions */
		*HANDLER.ce = zend_register_internal_class_ex(&ce, rpc_entry, NULL TSRMLS_CC);
		zend_register_functions(NULLHANDLER.functions, NULL, MODULE_PERSISTENT TSRMLS_CC);
		zend_register_ini_entries(HANDLER.ini, module_number TSRMLS_CC);
	}


	zif = (zend_internal_function *) emalloc(sizeof(zend_internal_function));

	zif->type = ZEND_INTERNAL_FUNCTION;
	zif->function_name = rpc_entry->name;
	zif->scope = rpc_entry;
	zif->arg_types = NULL;
	zif->handler = ZEND_FN(rpc_load);

	/* add new constructor to the method table */
	zend_hash_add(&(rpc_entry->function_table), rpc_entry->name, rpc_entry->name_length + 1, zif, sizeof(zend_function), &rpc_ctor);
	efree(zif);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MSHUTDOWN_FUNCTION
 */
ZEND_MSHUTDOWN_FUNCTION(rpc)
{
	FOREACH_HANDLER {
		HANDLER.rpc_handler_shutdown(TSRMLS_C);
	}

	/* destroy instances first */
	zend_ts_hash_destroy(pool);

	zend_ts_hash_destroy(classes);
	zend_llist_destroy(classes_list);
	zend_hash_destroy(handlers);

	pefree(handlers, TRUE);
	pefree(classes, TRUE);

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MINFO_FUNCTION
 */
ZEND_MINFO_FUNCTION(rpc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rpc support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static void rpc_class_dtor(void *pDest)
{
	rpc_class_hash **hash;

	hash = (rpc_class_hash **) pDest;

	if ((*hash)->singleton) {
		RPC_HT(*hash)->rpc_dtor((*hash)->data);
	}

	zend_ts_hash_destroy(&((*hash)->methods));
	zend_ts_hash_destroy(&((*hash)->properties));

	free((*hash)->name.str);
	pefree((*hash), TRUE);
}

static void rpc_string_dtor(void *pDest)
{
	rpc_string **string;

	string = (rpc_string **) pDest;

	free((*string)->str);
	pefree(*string, TRUE);
}

static void rpc_instance_dtor(void *pDest)
{
	rpc_internal **intern;

	intern = (rpc_internal **) pDest;

	RPC_HT(*intern)->rpc_dtor((*intern)->data);

	pefree(*intern, TRUE);
}

static zend_object_value rpc_create_proxy(TSRMLS_D)
{
	zend_object_value *zov;
	rpc_proxy *proxy_intern;

	/* set up the object value struct */
	zov = (zend_object_value*) pemalloc(sizeof(zend_object_value), TRUE);
	zov->handlers = &rpc_proxy_handlers;

	/* set up the internal representation of the proxy */
	proxy_intern = (rpc_proxy *) pemalloc(sizeof(rpc_proxy), TRUE);

	/* store the instance in a hash and set the key as handle, thus
	 * we can find it later easily
	 */
/*	tsrm_mutex_lock(proxy->mx_writer);
	{
		zov->handle = zend_hash_next_free_element(TS_HASH(proxy));
		zend_ts_hash_next_index_insert(proxy, &proxy_intern, sizeof(rpc_proxy *), NULL);
	}
	tsrm_mutex_unlock(proxy->mx_writer);
*/
	return *zov;
}

/* object handler */

static void rpc_objects_delete(void *object, zend_object_handle handle TSRMLS_DC)
{
	rpc_internal *intern = (rpc_internal *) object;

	if (RPC_CLASS(intern) && RPC_CLASS(intern)->singleton) {
		pefree(intern, TRUE);
	} else if (RPC_CLASS(intern) && RPC_CLASS(intern)->poolable) {
		if (RPC_CLASS(intern)->name.str) {
			zend_ts_hash_add(pool, RPC_CLASS(intern)->name.str, RPC_CLASS(intern)->name.len + 1, &intern, sizeof(rpc_internal *), NULL);
		} else {
			zend_ts_hash_index_update(pool, RPC_CLASS(intern)->name.len + 1, &intern, sizeof(rpc_internal *), NULL);
		}
	} else {
		if (intern->data != NULL) {
			RPC_HT(intern)->rpc_dtor(intern->data);
		}
		pefree(intern, TRUE);
	}
}

static zval* rpc_read(zval *object, zval *member, int type TSRMLS_DC)
{
	zval *return_value;
	GET_INTERNAL(intern);

	/* seting up the return value and decrease the refcounter as we don't
	 * keep a reference to this zval.
	 */
	MAKE_STD_ZVAL(return_value);
	ZVAL_DELREF(return_value);
	ZVAL_NULL(return_value);

	if (intern->hash && Z_TYPE_P(member) == IS_LONG) {
		rpc_internal_get(intern, NULL, Z_LVAL_P(member), return_value);
	} else if (Z_TYPE_P(member) == IS_STRING) {
		rpc_internal_get(intern, Z_STRVAL_P(member), Z_STRLEN_P(member), return_value);
	} else {
		/* TODO: exception here */
	}

	return return_value;
}

static void rpc_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	GET_INTERNAL(intern);

	if (intern->hash && Z_TYPE_P(member) == IS_LONG) {
		rpc_internal_set(intern, NULL, Z_LVAL_P(member), value);
	} else if (Z_TYPE_P(member) == IS_STRING) {
		rpc_internal_set(intern, Z_STRVAL_P(member), Z_STRLEN_P(member), value);
	} else {
		/* TODO: exception here */
	}
}

static zval** rpc_get_property(zval *object, zval *member TSRMLS_DC)
{
	zval **return_value;
	GET_INTERNAL(intern);

	return_value = emalloc(sizeof(zval *));
	MAKE_STD_ZVAL(*return_value);
	Z_TYPE_P(object) = IS_OBJECT;
	(*return_value)->value.obj = rpc_create_proxy(TSRMLS_C);

	return return_value;
}

static zval* rpc_get(zval *property TSRMLS_DC)
{
	/* not yet implemented */
	return NULL;
}

static void rpc_set(zval **property, zval *value TSRMLS_DC)
{
	/* not yet implemented */
}

static int rpc_has_property(zval *object, zval *member, int check_empty TSRMLS_DC)
{
//	GET_INTERNAL(intern);
	/* FIXME */
	return FAILURE;
}

static void rpc_unset_property(zval *object, zval *member TSRMLS_DC)
{
//	GET_INTERNAL(intern);
	/* FIXME */
}

static HashTable* rpc_get_properties(zval *object TSRMLS_DC)
{
//	GET_INTERNAL(intern);
	/* FIXME */
	return NULL;
}

static union _zend_function* rpc_get_method(zval *object, char *method, int method_len TSRMLS_DC)
{
	zend_function *function;
	unsigned char *ref_types = NULL;
	GET_INTERNAL(intern);

	if (zend_ts_hash_find(&(intern->function_table), method, method_len + 1, &function) != SUCCESS) {
		zend_internal_function *zif;

		/* get reftypes */
		if (RPC_HT(intern)->rpc_describe) {
			char *arg_types;
			rpc_string method_name;

			method_name.str = method;
			method_name.len = method_len;

			RPC_HT(intern)->rpc_describe(method_name, intern->data, &arg_types, &ref_types);
		}

		zif = (zend_internal_function *) emalloc(sizeof(zend_internal_function));
		zif->arg_types = ref_types;
		zif->function_name = method;
		zif->handler = ZEND_FN(rpc_call);
		zif->scope = intern->ce;
		zif->type = ZEND_INTERNAL_FUNCTION;

		/* add new method to the method table */
		zend_ts_hash_add(&(intern->function_table), method, method_len + 1, zif, sizeof(zend_function), &function);
		efree(zif);
	}

	return function;
}

static union _zend_function* rpc_get_constructor(zval *object TSRMLS_DC)
{
	return rpc_ctor;
}

static zend_class_entry* rpc_get_class_entry(zval *object TSRMLS_DC)
{
	GET_INTERNAL(intern);

	return intern->ce;
}

static int rpc_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	/* FIXME */
	return FAILURE;
}

/**/

/* constructor */
ZEND_FUNCTION(rpc_load)
{
	zval *object = getThis();
	zval ***args, ***args_free;
	zend_uint num_args = ZEND_NUM_ARGS();
	zend_class_entry overloaded_class_entry;
	rpc_class_hash *class_hash;
	rpc_class_hash **class_hash_find = NULL;
	rpc_internal *intern;
	rpc_string hash_val, class_val;
	int retval, append = 0;
	char *arg_types;

	/* check if we were called as a constructor or as a function */
	if (!object) {
		/* we were called as a function so we have to figure out which rpc layer was requested
		 * and then we have to set up a zval containing the object
		 */

		/* get class entry */
		GET_CLASS(ce);

		/* set up a new zval container */
		object = return_value;

		Z_TYPE_P(object) = IS_OBJECT;

		/* create a new object */
		object->value.obj = rpc_objects_new(*ce TSRMLS_CC);

		/* now everything is set up the same way as if we were called as a constructor */
	}

	if (GET_INTERNAL_EX(intern, object) != SUCCESS) {
		/* TODO: exception */
	}

	/* fetch further parameters */
	GET_ARGS_EX(num_args, args, args_free, 1);

	/* if classname != integer */
	if ((zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 1 TSRMLS_CC, "l", &class_val.str) != SUCCESS) ||
		/* or we have no hash function */
		!(RPC_HT(intern)->rpc_hash) ||
		/* or integer hashing is not allowed */
		!(RPC_HT(intern)->hash_type & HASH_AS_INT)) {

		/* else check for string - classname */
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 1 TSRMLS_CC, "s", &class_val.str, &class_val.len) != SUCCESS) {
			/* none of the two possibilities */
			/* TODO: exception */
			rpc_error(E_WARNING, "wrong arguments for %s()", get_active_function_name(TSRMLS_C));
			ZVAL_NULL(object);
			return;
		} else {
			/* hash classname if hashing function exists */
			if (RPC_HT(intern)->rpc_hash) {

				GET_SIGNATURE(intern, class_val.str, class_val.len, hash_val, num_args, arg_types);

				/* check if already hashed */	
 				if (zend_ts_hash_find(classes, hash_val.str, hash_val.len + 1, (void **) &class_hash_find) != SUCCESS) {
					class_hash = pemalloc(sizeof(rpc_class_hash), TRUE);

					/* set up the cache */
					zend_ts_hash_init(&(class_hash->methods), 0, NULL, rpc_string_dtor, TRUE);
					zend_ts_hash_init(&(class_hash->properties), 0, NULL, rpc_string_dtor, TRUE);
					class_hash->handlers = intern->handlers;
					class_hash->singleton = FALSE;
					class_hash->poolable = FALSE;
					class_hash->data = NULL;

					/* do hashing */
					if (RPC_HT(intern)->rpc_hash(class_val, (rpc_string *)(class_hash), NULL, num_args, arg_types, CLASS) != SUCCESS) {
						/* TODO: exception */
						ZVAL_NULL(object);
						return;
					}

					/* overload class entry */
					RPC_HT(intern)->rpc_name(class_val, &class_val, NULL, CLASS);
					INIT_CLASS_ENTRY(overloaded_class_entry, NULL, NULL);
					overloaded_class_entry.name = class_val.str;
					overloaded_class_entry.name_length = class_val.len;
					class_hash->ce = zend_register_internal_class_ex(&overloaded_class_entry, intern->ce, NULL TSRMLS_CC);
					intern->ce = class_hash->ce;

					/* register with non-hashed key
					 * also track all instaces in a llist for destruction later on, because there might be duplicate entries in
					 * the hashtable and we can't determine if a pointer references to an already freed element
					 */
					zend_ts_hash_add(classes, hash_val.str, hash_val.len + 1, &class_hash, sizeof(rpc_class_hash *), (void **) &class_hash_find);
					tsrm_mutex_lock(classes->mx_writer);
					zend_llist_add_element(classes_list, class_hash_find);
					tsrm_mutex_unlock(classes->mx_writer);

					if (class_hash->name.str) {
						/* register string hashcode */
						zend_ts_hash_add(classes, class_hash->name.str, class_hash->name.len + 1, &class_hash, sizeof(rpc_class_hash *), NULL);
					} else if (!class_hash->name.str && (RPC_HT(intern)->hash_type & HASH_AS_INT)) {
						/* register int hashcode */
						zend_ts_hash_index_update(classes, class_hash->name.len, &class_hash, sizeof(rpc_class_hash *), NULL);
					}
				} else {
					class_hash = *class_hash_find;
					intern->ce = class_hash->ce;
				}

				FREE_SIGNATURE(hash_val, arg_types);
			} else {
				/* overload class entry */
				INIT_CLASS_ENTRY(overloaded_class_entry, class_val.str, NULL);
				intern->ce = zend_register_internal_class_ex(&overloaded_class_entry, intern->ce, NULL TSRMLS_CC);
			}
		}
	} else {
		/* integer classname (hashcode) */
		if (zend_ts_hash_index_find(classes, class_val.len, (void**) &class_hash_find) != SUCCESS) {
			class_hash = pemalloc(sizeof(rpc_class_hash), TRUE);

			/* set up the cache */
			class_hash->name.str = NULL;
			class_hash->name.len = class_val.len;
			class_hash->handlers = intern->handlers;
			class_hash->singleton = FALSE;
			class_hash->poolable = FALSE;
			class_hash->data = NULL;

			zend_ts_hash_init(&(class_hash->methods), 0, NULL, rpc_string_dtor, TRUE);
			zend_ts_hash_init(&(class_hash->properties), 0, NULL, rpc_string_dtor, TRUE);

			/* overload class entry */
			RPC_HT(intern)->rpc_name(class_val, &class_val, NULL, CLASS);
			INIT_CLASS_ENTRY(overloaded_class_entry, class_val.str, NULL);
			class_hash->ce = zend_register_internal_class_ex(&overloaded_class_entry, intern->ce, NULL TSRMLS_CC);
			intern->ce = class_hash->ce;

			/* register int hashcode, we don't know more */
			zend_ts_hash_index_update(classes, class_hash->name.len, &class_hash, sizeof(rpc_class_hash *), NULL);
		} else {
			class_hash = *class_hash_find;
			intern->ce = class_hash->ce;
		}
	}
	
	/* if hash function available */
	if (RPC_HT(intern)->rpc_hash) {
		rpc_internal *pool_intern;
	
		/* assign cache structure */
		RPC_CLASS(intern) = class_hash;
		
		if (zend_ts_hash_remove_key_or_index(pool, RPC_CLASS(intern)->name.str, RPC_CLASS(intern)->name.len + 1, (void **) &pool_intern) == SUCCESS) {
			intern->data = pool_intern->data;
			
			pefree(pool_intern, TRUE);
			retval = SUCCESS;
		} else if (RPC_CLASS(intern)->singleton) {
			/* singleton */
			intern->data = RPC_CLASS(intern)->data;
			retval = SUCCESS;
		} else {
			/* call the rpc ctor */
			retval = RPC_HT(intern)->rpc_ctor(*(rpc_string *)(class_hash), &(intern->data), num_args, args);
		}
	} else {
		/* disable caching from now on */
		intern->hash = NULL;

		/* call the rpc ctor */
		retval = RPC_HT(intern)->rpc_ctor(class_val, &(intern->data), num_args, args);
	}

	efree(args_free);

	if (retval != SUCCESS) {
		/* TODO: exception */
		RETURN_NULL();
	}
}

ZEND_FUNCTION(rpc_call)
{
	zval *object = getThis();
	zval ***args, ***args_free;
	zend_uint num_args = ZEND_NUM_ARGS();
	char *hash = NULL, *arg_types;
	int hash_len, retval, strip = 0;

	/* check if we were called as a method or as a function */
	if (!object) {
		/* we were called as a function so we have to figure out which rpc layer was requested */

		/* get class entry */
		GET_CLASS(ce);

		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 2 TSRMLS_CC, "Ol", &object, *ce, &hash_len) != SUCCESS) {
			if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 2 TSRMLS_CC, "Os", &object, *ce, &hash, &hash_len) != SUCCESS) {
				/* none of the two possibilities */
				/* TODO: exception */
				rpc_error(E_WARNING, "wrong arguments for %s()", get_active_function_name(TSRMLS_C));
			}
		}

		strip = 2;
	} else {
		hash = get_active_function_name(TSRMLS_C);
		hash_len = strlen(hash);
	}

	GET_ARGS_EX(num_args, args, args_free, strip);
	
	/* scope for internal data */
	{
		rpc_string hash_val, *method_hash, **method_hash_find;
		GET_INTERNAL(intern);

		method_hash = (rpc_string *) pemalloc(sizeof(rpc_string), TRUE);
		method_hash->str = hash;
		method_hash->len = hash_len;
		
		if (intern->hash) {
			/* cache method table lookups */
		
			if (!hash && !(RPC_HT(intern)->hash_type & HASH_AS_INT)) {
				/* TODO: exception */
			} else if(hash) {
				/* string passed */
				GET_METHOD_SIGNATURE(intern, method_hash, hash_val, num_args, arg_types);

				/* check if already hashed */	
				if (zend_ts_hash_find(&(intern->hash->methods), hash_val.str, hash_val.len + 1, (void **) &method_hash_find) != SUCCESS) {
					if (RPC_HT(intern)->rpc_hash(*method_hash, method_hash, intern->data, num_args, arg_types, METHOD) != SUCCESS) {
						/* TODO: exception */
						RETURN_NULL();
					}

					/* register with non-hashed key */
					zend_ts_hash_add(&(intern->hash->methods), hash_val.str, hash_val.len + 1, &method_hash, sizeof(rpc_string *), NULL);
				} else {
					pefree(method_hash, TRUE);
					method_hash = *method_hash_find;
				}

				FREE_SIGNATURE(hash_val, arg_types);
			}
		}

		/* actually this should not be neccesary, but who knows :)
		 * considering possible thread implementations in future php versions
		 * and srm it is better to do concurrency checks
		 */
		tsrm_mutex_lock(intern->mx_handler);
		retval = RPC_HT(intern)->rpc_call(*method_hash, &(intern->data), return_value, num_args, args);
		tsrm_mutex_unlock(intern->mx_handler);
	}

	efree(args_free);

	if (retval != SUCCESS) {
		/* TODO: exception here */
	}
}

ZEND_FUNCTION(rpc_set)
{
	zval *object, *value;
	char *property = NULL;
	int property_len;
	rpc_internal *intern;
	/* get class entry */
	GET_CLASS(ce);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 3 TSRMLS_CC, "Olz", &object, *ce, &property_len, &value) != SUCCESS) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 3 TSRMLS_CC, "Osz", &object, *ce, &property, &property_len, &value) != SUCCESS) {
			/* none of the two possibilities */
			/* TODO: exception */
			rpc_error(E_WARNING, "wrong arguments for %s()", get_active_function_name(TSRMLS_C));
		}
	}

	GET_INTERNAL_EX(intern, object);
	if (!property && !intern->hash) {
		/* TODO: exception here */
	} else {
		rpc_internal_set(intern, property, property_len, value);
	}
}

ZEND_FUNCTION(rpc_get)
{
	zval *object;
	char *property = NULL;
	int property_len;
	rpc_internal *intern;
	/* get class entry */
	GET_CLASS(ce);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 2 TSRMLS_CC, "Ol", &object, *ce, &property_len) != SUCCESS) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 2 TSRMLS_CC, "Os", &object, *ce, &property, &property_len) != SUCCESS) {
			/* none of the two possibilities */
			/* TODO: exception */
			rpc_error(E_WARNING, "wrong arguments for %s()", get_active_function_name(TSRMLS_C));
		}
	}

	GET_INTERNAL_EX(intern, object);
	if (!property && !intern->hash) {
		/* TODO: exception here */
	} else {
		rpc_internal_get(intern, property, property_len, return_value);
	}
}

ZEND_FUNCTION(rpc_singleton)
{
	zval *object;
	rpc_internal *intern;
	/* get class entry */
	GET_CLASS(ce);

	zend_parse_parameters(1 TSRMLS_CC, "O", &object, *ce);

	GET_INTERNAL_EX(intern, object);

	if (!RPC_CLASS(intern)) {
		/* TODO: exception here, no hashing */
	} else if (!RPC_CLASS(intern)->singleton) {
		RPC_CLASS(intern)->singleton = TRUE;
		RPC_CLASS(intern)->data = intern->data;
	}
}

ZEND_FUNCTION(rpc_poolable)
{
	zval *object;
	rpc_internal *intern;
	/* get class entry */
	GET_CLASS(ce);

	zend_parse_parameters(1 TSRMLS_CC, "O", &object, *ce);

	GET_INTERNAL_EX(intern, object);

	if (RPC_HT(intern)->poolable && RPC_CLASS(intern) && (RPC_HT(intern)->poolable == TRUE)) {
		RPC_CLASS(intern)->poolable = TRUE;
	} else {
		/* TODO: exception here, no hashing */
	}
}

ZEND_API void rpc_error(int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	zend_error(type, format, args);
	va_end(args);
}

ZEND_API zend_object_value rpc_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value *zov;
	rpc_internal *intern;
	
	/* set up the object value struct */
	zov = (zend_object_value*) pemalloc(sizeof(zend_object_value), TRUE);
	zov->handlers = &rpc_handlers;

	/* set up the internal representation of our rpc instance */
	intern = (rpc_internal *) pemalloc(sizeof(rpc_internal), TRUE);

	intern->ce = class_type;
	intern->data = NULL;
	intern->function_table.hash = intern->ce->function_table;
	intern->function_table.reader = 0;
	intern->function_table.mx_reader = tsrm_mutex_alloc();
	intern->function_table.mx_writer = tsrm_mutex_alloc();
	intern->mx_handler = tsrm_mutex_alloc();

	if (zend_hash_find(handlers, class_type->name, class_type->name_length + 1, (void **) &(intern->handlers)) != SUCCESS) {
		/* TODO: exception */
	}

	zov->handle = zend_objects_store_put(intern, rpc_objects_delete, NULL TSRMLS_CC);

	return *zov;
}

/*******************/

static void rpc_internal_get(rpc_internal *intern, char *property, zend_uint property_len, zval *return_value)
{
	int retval;
	rpc_string *property_hash, **property_hash_find;

	Z_TYPE_P(return_value) = IS_NULL;

	property_hash = (rpc_string *) pemalloc(sizeof(rpc_string), TRUE);
	property_hash->str = property;
	property_hash->len = property_len;
	
	if (intern->hash) {
		/* cache method table lookups */
	
		if (!property && !(RPC_HT(intern)->hash_type & HASH_AS_INT)) {
			/* TODO: exception */
		} else if(property) {
			/* check if already hashed */	
			if (zend_ts_hash_find(&(intern->hash->properties), property, property_len + 1, (void **) &property_hash_find) != SUCCESS) {
				if (RPC_HT(intern)->rpc_hash(*property_hash, property_hash, intern->data, 0, NULL, PROPERTY) != SUCCESS) {
					/* TODO: exception */
					RETURN_NULL();
				}

				/* register with non-hashed key */
				zend_ts_hash_add(&(intern->hash->properties), property, property_len + 1, &property_hash, sizeof(rpc_string *), NULL);
			} else {
				pefree(property_hash, TRUE);
				property_hash = *property_hash_find;
			}
		}
	}


	tsrm_mutex_lock(intern->mx_handler);
	retval = RPC_HT(intern)->rpc_get(*property_hash, return_value, intern->data);
	tsrm_mutex_unlock(intern->mx_handler);

	if (retval != SUCCESS) {
		/* TODO: exception here */
	}
}

static void rpc_internal_set(rpc_internal *intern, char *property, zend_uint property_len, zval *value)
{
	int retval;
	rpc_string property_name;

	property_name.str = property;
	property_name.len = property_len;

	tsrm_mutex_lock(intern->mx_handler);
	retval = RPC_HT(intern)->rpc_set(property_name, value, intern->data);
	tsrm_mutex_unlock(intern->mx_handler);

	if (retval != SUCCESS) {
		/* TODO: exception here */
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
