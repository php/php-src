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
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
 * TODO: Rewrite for buckets.
 * Concept:
 *       The user defined filter class should implement a method named
 *       "filter" with the following proto:
 *       long filter(object brigade_in, object brigade_out, long &consumed, long flags);
 *
 *       brigade_in and brigade_out are overloaded objects that wrap around
 *       the php_stream_bucket_brigades passed to the underlying filter method.
 *       The brigades have methods for retrieving the head of the brigade as
 *       an overloaded bucket object, a method for appending a
 *       bucket object to the end of the brigade, and a method for creating a new
 *       bucket at the end of the brigade.
 *
 *       The bucket object has methods to unlink it from it's containing brigade,
 *       split into two buckets, and retrieve the buffer from a bucket.
 *       
 *       This approach means that there doesn't need to be very much magic between
 *       userspace and the real C interface.
 */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"

struct php_user_filter_data {
	zend_class_entry *ce;
	/* variable length; this *must* be last in the structure */
	char classname[1];
};

/* to provide context for calling into the next filter from user-space */
static int le_userfilters;

#define GET_FILTER_FROM_OBJ()	{ \
	zval **tmp; \
	if (FAILURE == zend_hash_index_find(Z_OBJPROP_P(this_ptr), 0, (void**)&tmp)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "filter property vanished"); \
		RETURN_FALSE; \
	} \
	ZEND_FETCH_RESOURCE(filter, php_stream_filter*, tmp, -1, "filter", le_userfilters); \
}

/* define the base filter class */

PHP_FUNCTION(user_filter_nop)
{
}

static zend_function_entry user_filter_class_funcs[] = {
	PHP_NAMED_FE(write,		PHP_FN(user_filter_nop),		NULL)
	PHP_NAMED_FE(read,		PHP_FN(user_filter_nop),		NULL)
	PHP_NAMED_FE(flush,		PHP_FN(user_filter_nop),		NULL)
	PHP_NAMED_FE(oncreate,	PHP_FN(user_filter_nop),		NULL)
	PHP_NAMED_FE(onclose,	PHP_FN(user_filter_nop),		NULL)
	{ NULL, NULL, NULL }
};

static zend_class_entry user_filter_class_entry;

PHP_MINIT_FUNCTION(user_filters)
{
	/* init the filter class ancestor */
	INIT_CLASS_ENTRY(user_filter_class_entry, "php_user_filter", user_filter_class_funcs);
	if (NULL == zend_register_internal_class(&user_filter_class_entry TSRMLS_CC)) {
		return FAILURE;
	}

	/* init the filter resource; it has no dtor, as streams will always clean it up
	 * at the correct time */
	le_userfilters = zend_register_list_destructors_ex(NULL, NULL, "stream filter", 0);

	if (le_userfilters == FAILURE)
		return FAILURE;
	
	return SUCCESS;
}

static void userfilter_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	zval *obj = (zval*)thisfilter->abstract;
	zval func_name;
	zval *retval = NULL;
	zval **tmp; 

	ZVAL_STRINGL(&func_name, "onclose", sizeof("onclose")-1, 0);

	call_user_function_ex(NULL,
			&obj,
			&func_name,
			&retval,
			0, NULL,
			0, NULL TSRMLS_CC);

	if (retval)
		zval_ptr_dtor(&retval);

	if (SUCCESS == zend_hash_index_find(Z_OBJPROP_P(obj), 0, (void**)&tmp)) { 
		zend_list_delete(Z_LVAL_PP(tmp));
		FREE_ZVAL(*tmp);
	} 

	/* kill the object */
	zval_ptr_dtor(&obj);
}

php_stream_filter_status_t userfilter_filter(
			php_stream *stream,
			php_stream_filter *thisfilter,
			php_stream_bucket_brigade *buckets_in,
			php_stream_bucket_brigade *buckets_out,
			size_t *bytes_consumed,
			int flags
			TSRMLS_DC)
{
	int ret = EOF;
	zval *obj = (zval*)thisfilter->abstract;
	zval func_name;
	zval *retval = NULL;
	zval **args[1];
	zval *zcount;
	int call_result;

	ZVAL_STRINGL(&func_name, "filter", sizeof("filter")-1, 0);

	ALLOC_INIT_ZVAL(zcount);
	ZVAL_BOOL(zcount, flags & PSFS_FLAG_FLUSH_CLOSE);
	args[0] = &zcount;

	call_result = call_user_function_ex(NULL,
			&obj,
			&func_name,
			&retval,
			1, args,
			0, NULL TSRMLS_CC);

	if (call_result == SUCCESS && retval != NULL) {
		convert_to_long(retval);
		ret = Z_LVAL_P(retval);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to call filter function");
	}

	if (retval)
		zval_ptr_dtor(&retval);
	zval_ptr_dtor(&zcount);

	return PSFS_ERR_FATAL;
}

static php_stream_filter_ops userfilter_ops = {
	userfilter_filter,
	userfilter_dtor,
	"user-filter"
};

static php_stream_filter *user_filter_factory_create(const char *filtername,
		const char *filterparams, int filterparamslen, int persistent TSRMLS_DC)
{
	struct php_user_filter_data *fdat = NULL;
	php_stream_filter *filter;
	zval *obj, *zfilter;
	zval func_name;
	zval *retval = NULL;
	
	/* some sanity checks */
	if (persistent) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"cannot use a user-space filter with a persistent stream");
		return NULL;
	}

	/* determine the classname/class entry */
	if (FAILURE == zend_hash_find(BG(user_filter_map), (char*)filtername,
				strlen(filtername), (void**)&fdat)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Err, filter \"%s\" is not in the user-filter map, but somehow the user-filter-factory was invoked for it!?", filtername);
		return NULL;
	}

	/* bind the classname to the actual class */
	if (fdat->ce == NULL) {
		if (FAILURE == zend_hash_find(EG(class_table), fdat->classname, strlen(fdat->classname)+1,
					(void **)&fdat->ce)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"user-filter \"%s\" requires class \"%s\", but that class is not defined",
					filtername, fdat->classname);
			return NULL;
		}
#ifdef ZEND_ENGINE_2
		fdat->ce = *(zend_class_entry**)fdat->ce;
#endif

	}

	filter = php_stream_filter_alloc(&userfilter_ops, NULL, 0);
	if (filter == NULL) {
		return NULL;
	}

	ALLOC_INIT_ZVAL(zfilter);
	ZEND_REGISTER_RESOURCE(zfilter, filter, le_userfilters);
	
	/* create the object */
	ALLOC_ZVAL(obj);
	object_init_ex(obj, fdat->ce);
	ZVAL_REFCOUNT(obj) = 1;
	PZVAL_IS_REF(obj) = 1;

	/* set the filter property */
	filter->abstract = obj;
	
	zend_hash_index_update(Z_OBJPROP_P(obj), 0, &zfilter, sizeof(zfilter), NULL);

	/* filtername */
	add_property_string(obj, "filtername", (char*)filtername, 1);
	
	/* and the parameters, if any */
	if (filterparams) {
		add_property_stringl(obj, "params", (char*)filterparams, filterparamslen, 1);
	} else {
		add_property_null(obj, "params");
	}

	/* invoke the constructor */
	ZVAL_STRINGL(&func_name, "oncreate", sizeof("oncreate")-1, 0);

	call_user_function_ex(NULL,
			&obj,
			&func_name,
			&retval,
			0, NULL,
			0, NULL TSRMLS_CC);

	if (retval)
		zval_ptr_dtor(&retval);

	return filter;
}

static php_stream_filter_factory user_filter_factory = {
	user_filter_factory_create
};

static void filter_item_dtor(struct php_user_filter_data *fdat)
{
}

/* {{{ proto array stream_get_filters(void)
   Returns a list of registered filters */
PHP_FUNCTION(stream_get_filters)
{
	char *filter_name;
	int key_flags, filter_name_len = 0;
	HashTable *filters_hash;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	array_init(return_value);

	filters_hash = php_get_stream_filters_hash();

	if (filters_hash) {
		for(zend_hash_internal_pointer_reset(filters_hash);
			(key_flags = zend_hash_get_current_key_ex(filters_hash, &filter_name, &filter_name_len, NULL, 0, NULL)) != HASH_KEY_NON_EXISTANT;
			zend_hash_move_forward(filters_hash))
				if (key_flags == HASH_KEY_IS_STRING)
					add_next_index_stringl(return_value, filter_name, filter_name_len, 1);
	}
	/* It's okay to return an empty array if no filters are registered */
}
/* }}} */	

/* {{{ proto bool stream_register_filter(string filtername, string classname)
   Registers a custom filter handler class */
PHP_FUNCTION(stream_register_filter)
{
	char *filtername, *classname;
	int filtername_len, classname_len;
	struct php_user_filter_data *fdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filtername, &filtername_len,
				&classname, &classname_len) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	if (!BG(user_filter_map)) {
		BG(user_filter_map) = (HashTable*) emalloc(sizeof(HashTable));
		zend_hash_init(BG(user_filter_map), 5, NULL, (dtor_func_t) filter_item_dtor, 0);
	}

	fdat = ecalloc(1, sizeof(*fdat) + classname_len);
	memcpy(fdat->classname, classname, classname_len);
	zend_str_tolower(fdat->classname, classname_len);

	if (zend_hash_add(BG(user_filter_map), filtername, filtername_len, (void*)fdat,
				sizeof(*fdat) + classname_len, NULL) == SUCCESS &&
			php_stream_filter_register_factory(filtername, &user_filter_factory TSRMLS_CC) == SUCCESS) {
		RETVAL_TRUE;
	}

	efree(fdat);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
