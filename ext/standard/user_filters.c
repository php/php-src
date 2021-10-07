/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   | Sara Golemon (pollita@php.net)                                       |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_globals.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"

#define PHP_STREAM_BRIGADE_RES_NAME	"userfilter.bucket brigade"
#define PHP_STREAM_BUCKET_RES_NAME "userfilter.bucket"
#define PHP_STREAM_FILTER_RES_NAME "userfilter.filter"

struct php_user_filter_data {
	zend_class_entry *ce;
	/* variable length; this *must* be last in the structure */
	zend_string *classname;
};

/* to provide context for calling into the next filter from user-space */
static int le_userfilters;
static int le_bucket_brigade;
static int le_bucket;

/* define the base filter class */

PHP_FUNCTION(user_filter_nop)
{
}
ZEND_BEGIN_ARG_INFO(arginfo_php_user_filter_filter, 0)
	ZEND_ARG_INFO(0, in)
	ZEND_ARG_INFO(0, out)
	ZEND_ARG_INFO(1, consumed)
	ZEND_ARG_INFO(0, closing)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_user_filter_onCreate, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_user_filter_onClose, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry user_filter_class_funcs[] = {
	PHP_NAMED_FE(filter,	PHP_FN(user_filter_nop),		arginfo_php_user_filter_filter)
	PHP_NAMED_FE(onCreate,	PHP_FN(user_filter_nop),		arginfo_php_user_filter_onCreate)
	PHP_NAMED_FE(onClose,	PHP_FN(user_filter_nop),		arginfo_php_user_filter_onClose)
	PHP_FE_END
};

static zend_class_entry user_filter_class_entry;

static ZEND_RSRC_DTOR_FUNC(php_bucket_dtor)
{
	php_stream_bucket *bucket = (php_stream_bucket *)res->ptr;
	if (bucket) {
		php_stream_bucket_delref(bucket);
		bucket = NULL;
	}
}

PHP_MINIT_FUNCTION(user_filters)
{
	zend_class_entry *php_user_filter;
	/* init the filter class ancestor */
	INIT_CLASS_ENTRY(user_filter_class_entry, "php_user_filter", user_filter_class_funcs);
	if ((php_user_filter = zend_register_internal_class(&user_filter_class_entry)) == NULL) {
		return FAILURE;
	}
	zend_declare_property_string(php_user_filter, "filtername", sizeof("filtername")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(php_user_filter, "params", sizeof("params")-1, "", ZEND_ACC_PUBLIC);

	/* init the filter resource; it has no dtor, as streams will always clean it up
	 * at the correct time */
	le_userfilters = zend_register_list_destructors_ex(NULL, NULL, PHP_STREAM_FILTER_RES_NAME, 0);

	if (le_userfilters == FAILURE) {
		return FAILURE;
	}

	/* Filters will dispose of their brigades */
	le_bucket_brigade = zend_register_list_destructors_ex(NULL, NULL, PHP_STREAM_BRIGADE_RES_NAME, module_number);
	/* Brigades will dispose of their buckets */
	le_bucket = zend_register_list_destructors_ex(php_bucket_dtor, NULL, PHP_STREAM_BUCKET_RES_NAME, module_number);

	if (le_bucket_brigade == FAILURE) {
		return FAILURE;
	}

	REGISTER_LONG_CONSTANT("PSFS_PASS_ON",			PSFS_PASS_ON,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_FEED_ME",			PSFS_FEED_ME,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_ERR_FATAL",		PSFS_ERR_FATAL,			CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PSFS_FLAG_NORMAL",		PSFS_FLAG_NORMAL,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_FLAG_FLUSH_INC",	PSFS_FLAG_FLUSH_INC,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_FLAG_FLUSH_CLOSE",	PSFS_FLAG_FLUSH_CLOSE,	CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(user_filters)
{
	if (BG(user_filter_map)) {
		zend_hash_destroy(BG(user_filter_map));
		efree(BG(user_filter_map));
		BG(user_filter_map) = NULL;
	}

	return SUCCESS;
}

static void userfilter_dtor(php_stream_filter *thisfilter)
{
	zval *obj = &thisfilter->abstract;
	zval func_name;
	zval retval;

	if (obj == NULL) {
		/* If there's no object associated then there's nothing to dispose of */
		return;
	}

	ZVAL_STRINGL(&func_name, "onclose", sizeof("onclose")-1);

	call_user_function(NULL,
			obj,
			&func_name,
			&retval,
			0, NULL);

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	/* kill the object */
	zval_ptr_dtor(obj);
}

php_stream_filter_status_t userfilter_filter(
			php_stream *stream,
			php_stream_filter *thisfilter,
			php_stream_bucket_brigade *buckets_in,
			php_stream_bucket_brigade *buckets_out,
			size_t *bytes_consumed,
			int flags
			)
{
	int ret = PSFS_ERR_FATAL;
	zval *obj = &thisfilter->abstract;
	zval func_name;
	zval retval;
	zval args[4];
	zval zpropname;
	int call_result;

	/* the userfilter object probably doesn't exist anymore */
	if (CG(unclean_shutdown)) {
		return ret;
	}

	/* Make sure the stream is not closed while the filter callback executes. */
	uint32_t orig_no_fclose = stream->flags & PHP_STREAM_FLAG_NO_FCLOSE;
	stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;

	if (!zend_hash_str_exists_ind(Z_OBJPROP_P(obj), "stream", sizeof("stream")-1)) {
		zval tmp;

		/* Give the userfilter class a hook back to the stream */
		php_stream_to_zval(stream, &tmp);
		Z_ADDREF(tmp);
		add_property_zval(obj, "stream", &tmp);
		/* add_property_zval increments the refcount which is unwanted here */
		zval_ptr_dtor(&tmp);
	}

	ZVAL_STRINGL(&func_name, "filter", sizeof("filter")-1);

	/* Setup calling arguments */
	ZVAL_RES(&args[0], zend_register_resource(buckets_in, le_bucket_brigade));
	ZVAL_RES(&args[1], zend_register_resource(buckets_out, le_bucket_brigade));

	if (bytes_consumed) {
		ZVAL_LONG(&args[2], *bytes_consumed);
	} else {
		ZVAL_NULL(&args[2]);
	}

	ZVAL_BOOL(&args[3], flags & PSFS_FLAG_FLUSH_CLOSE);

	call_result = call_user_function_ex(NULL,
			obj,
			&func_name,
			&retval,
			4, args,
			0, NULL);

	zval_ptr_dtor(&func_name);

	if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		convert_to_long(&retval);
		ret = (int)Z_LVAL(retval);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "failed to call filter function");
	}

	if (bytes_consumed) {
		*bytes_consumed = zval_get_long(&args[2]);
	}

	if (buckets_in->head) {
		php_stream_bucket *bucket = buckets_in->head;

		php_error_docref(NULL, E_WARNING, "Unprocessed filter buckets remaining on input brigade");
		while ((bucket = buckets_in->head)) {
			/* Remove unconsumed buckets from the brigade */
			php_stream_bucket_unlink(bucket);
			php_stream_bucket_delref(bucket);
		}
	}
	if (ret != PSFS_PASS_ON) {
		php_stream_bucket *bucket = buckets_out->head;
		while (bucket != NULL) {
			php_stream_bucket_unlink(bucket);
			php_stream_bucket_delref(bucket);
			bucket = buckets_out->head;
		}
	}

	/* filter resources are cleaned up by the stream destructor,
	 * keeping a reference to the stream resource here would prevent it
	 * from being destroyed properly */
	ZVAL_STRINGL(&zpropname, "stream", sizeof("stream")-1);
	Z_OBJ_HANDLER_P(obj, unset_property)(obj, &zpropname, NULL);
	zval_ptr_dtor(&zpropname);

	zval_ptr_dtor(&args[3]);
	zval_ptr_dtor(&args[2]);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	stream->flags &= ~PHP_STREAM_FLAG_NO_FCLOSE;
	stream->flags |= orig_no_fclose;

	return ret;
}

static const php_stream_filter_ops userfilter_ops = {
	userfilter_filter,
	userfilter_dtor,
	"user-filter"
};

static php_stream_filter *user_filter_factory_create(const char *filtername,
		zval *filterparams, uint8_t persistent)
{
	struct php_user_filter_data *fdat = NULL;
	php_stream_filter *filter;
	zval obj, zfilter;
	zval func_name;
	zval retval;
	size_t len;

	/* some sanity checks */
	if (persistent) {
		php_error_docref(NULL, E_WARNING,
				"cannot use a user-space filter with a persistent stream");
		return NULL;
	}

	len = strlen(filtername);

	/* determine the classname/class entry */
	if (NULL == (fdat = zend_hash_str_find_ptr(BG(user_filter_map), (char*)filtername, len))) {
		char *period;

		/* Userspace Filters using ambiguous wildcards could cause problems.
           i.e.: myfilter.foo.bar will always call into myfilter.foo.*
                 never seeing myfilter.*
           TODO: Allow failed userfilter creations to continue
                 scanning through the list */
		if ((period = strrchr(filtername, '.'))) {
			char *wildcard = safe_emalloc(len, 1, 3);

			/* Search for wildcard matches instead */
			memcpy(wildcard, filtername, len + 1); /* copy \0 */
			period = wildcard + (period - filtername);
			while (period) {
				ZEND_ASSERT(period[0] == '.');
				period[1] = '*';
				period[2] = '\0';
				if (NULL != (fdat = zend_hash_str_find_ptr(BG(user_filter_map), wildcard, strlen(wildcard)))) {
					period = NULL;
				} else {
					*period = '\0';
					period = strrchr(wildcard, '.');
				}
			}
			efree(wildcard);
		}
		if (fdat == NULL) {
			php_error_docref(NULL, E_WARNING,
					"Err, filter \"%s\" is not in the user-filter map, but somehow the user-filter-factory was invoked for it!?", filtername);
			return NULL;
		}
	}

	/* bind the classname to the actual class */
	if (fdat->ce == NULL) {
		if (NULL == (fdat->ce = zend_lookup_class(fdat->classname))) {
			php_error_docref(NULL, E_WARNING,
					"user-filter \"%s\" requires class \"%s\", but that class is not defined",
					filtername, ZSTR_VAL(fdat->classname));
			return NULL;
		}
	}

	/* create the object */
	if (object_init_ex(&obj, fdat->ce) == FAILURE) {
		return NULL;
	}

	filter = php_stream_filter_alloc(&userfilter_ops, NULL, 0);
	if (filter == NULL) {
		zval_ptr_dtor(&obj);
		return NULL;
	}

	/* filtername */
	add_property_string(&obj, "filtername", (char*)filtername);

	/* and the parameters, if any */
	if (filterparams) {
		add_property_zval(&obj, "params", filterparams);
	} else {
		add_property_null(&obj, "params");
	}

	/* invoke the constructor */
	ZVAL_STRINGL(&func_name, "oncreate", sizeof("oncreate")-1);

	call_user_function(NULL,
			&obj,
			&func_name,
			&retval,
			0, NULL);

	if (Z_TYPE(retval) != IS_UNDEF) {
		if (Z_TYPE(retval) == IS_FALSE) {
			/* User reported filter creation error "return false;" */
			zval_ptr_dtor(&retval);

			/* Kill the filter (safely) */
			ZVAL_UNDEF(&filter->abstract);
			php_stream_filter_free(filter);

			/* Kill the object */
			zval_ptr_dtor(&obj);

			/* Report failure to filter_alloc */
			return NULL;
		}
		zval_ptr_dtor(&retval);
	}
	zval_ptr_dtor(&func_name);

	/* set the filter property, this will be used during cleanup */
	ZVAL_RES(&zfilter, zend_register_resource(filter, le_userfilters));
	ZVAL_OBJ(&filter->abstract, Z_OBJ(obj));
	add_property_zval(&obj, "filter", &zfilter);
	/* add_property_zval increments the refcount which is unwanted here */
	zval_ptr_dtor(&zfilter);

	return filter;
}

static const php_stream_filter_factory user_filter_factory = {
	user_filter_factory_create
};

static void filter_item_dtor(zval *zv)
{
	struct php_user_filter_data *fdat = Z_PTR_P(zv);
	zend_string_release_ex(fdat->classname, 0);
	efree(fdat);
}

/* {{{ proto object stream_bucket_make_writeable(resource brigade)
   Return a bucket object from the brigade for operating on */
PHP_FUNCTION(stream_bucket_make_writeable)
{
	zval *zbrigade, zbucket;
	php_stream_bucket_brigade *brigade;
	php_stream_bucket *bucket;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zbrigade)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if ((brigade = (php_stream_bucket_brigade*)zend_fetch_resource(
					Z_RES_P(zbrigade), PHP_STREAM_BRIGADE_RES_NAME, le_bucket_brigade)) == NULL) {
		RETURN_FALSE;
	}

	ZVAL_NULL(return_value);

	if (brigade->head && (bucket = php_stream_bucket_make_writeable(brigade->head))) {
		ZVAL_RES(&zbucket, zend_register_resource(bucket, le_bucket));
		object_init(return_value);
		add_property_zval(return_value, "bucket", &zbucket);
		/* add_property_zval increments the refcount which is unwanted here */
		zval_ptr_dtor(&zbucket);
		add_property_stringl(return_value, "data", bucket->buf, bucket->buflen);
		add_property_long(return_value, "datalen", bucket->buflen);
	}
}
/* }}} */

/* {{{ php_stream_bucket_attach */
static void php_stream_bucket_attach(int append, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zbrigade, *zobject;
	zval *pzbucket, *pzdata;
	php_stream_bucket_brigade *brigade;
	php_stream_bucket *bucket;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(zbrigade)
		Z_PARAM_OBJECT(zobject)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (NULL == (pzbucket = zend_hash_str_find_deref(Z_OBJPROP_P(zobject), "bucket", sizeof("bucket")-1))) {
		php_error_docref(NULL, E_WARNING, "Object has no bucket property");
		RETURN_FALSE;
	}

	if ((brigade = (php_stream_bucket_brigade*)zend_fetch_resource(
					Z_RES_P(zbrigade), PHP_STREAM_BRIGADE_RES_NAME, le_bucket_brigade)) == NULL) {
		RETURN_FALSE;
	}

	if ((bucket = (php_stream_bucket *)zend_fetch_resource_ex(pzbucket, PHP_STREAM_BUCKET_RES_NAME, le_bucket)) == NULL) {
		RETURN_FALSE;
	}

	if (NULL != (pzdata = zend_hash_str_find_deref(Z_OBJPROP_P(zobject), "data", sizeof("data")-1)) && Z_TYPE_P(pzdata) == IS_STRING) {
		if (!bucket->own_buf) {
			bucket = php_stream_bucket_make_writeable(bucket);
		}
		if (bucket->buflen != Z_STRLEN_P(pzdata)) {
			bucket->buf = perealloc(bucket->buf, Z_STRLEN_P(pzdata), bucket->is_persistent);
			bucket->buflen = Z_STRLEN_P(pzdata);
		}
		memcpy(bucket->buf, Z_STRVAL_P(pzdata), bucket->buflen);
	}

	if (append) {
		php_stream_bucket_append(brigade, bucket);
	} else {
		php_stream_bucket_prepend(brigade, bucket);
	}
	/* This is a hack necessary to accommodate situations where bucket is appended to the stream
 	 * multiple times. See bug35916.phpt for reference.
	 */
	if (bucket->refcount == 1) {
		bucket->refcount++;
	}
}
/* }}} */

/* {{{ proto void stream_bucket_prepend(resource brigade, object bucket)
   Prepend bucket to brigade */
PHP_FUNCTION(stream_bucket_prepend)
{
	php_stream_bucket_attach(0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void stream_bucket_append(resource brigade, object bucket)
   Append bucket to brigade */
PHP_FUNCTION(stream_bucket_append)
{
	php_stream_bucket_attach(1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto resource stream_bucket_new(resource stream, string buffer)
   Create a new bucket for use on the current stream */
PHP_FUNCTION(stream_bucket_new)
{
	zval *zstream, zbucket;
	php_stream *stream;
	char *buffer;
	char *pbuffer;
	size_t buffer_len;
	php_stream_bucket *bucket;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(zstream)
		Z_PARAM_STRING(buffer, buffer_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	php_stream_from_zval(stream, zstream);

	pbuffer = pemalloc(buffer_len, php_stream_is_persistent(stream));

	memcpy(pbuffer, buffer, buffer_len);

	bucket = php_stream_bucket_new(stream, pbuffer, buffer_len, 1, php_stream_is_persistent(stream));

	if (bucket == NULL) {
		RETURN_FALSE;
	}

	ZVAL_RES(&zbucket, zend_register_resource(bucket, le_bucket));
	object_init(return_value);
	add_property_zval(return_value, "bucket", &zbucket);
	/* add_property_zval increments the refcount which is unwanted here */
	zval_ptr_dtor(&zbucket);
	add_property_stringl(return_value, "data", bucket->buf, bucket->buflen);
	add_property_long(return_value, "datalen", bucket->buflen);
}
/* }}} */

/* {{{ proto array stream_get_filters(void)
   Returns a list of registered filters */
PHP_FUNCTION(stream_get_filters)
{
	zend_string *filter_name;
	HashTable *filters_hash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	filters_hash = php_get_stream_filters_hash();

	if (filters_hash) {
		ZEND_HASH_FOREACH_STR_KEY(filters_hash, filter_name) {
			if (filter_name) {
				add_next_index_str(return_value, zend_string_copy(filter_name));
			}
		} ZEND_HASH_FOREACH_END();
	}
	/* It's okay to return an empty array if no filters are registered */
}
/* }}} */

/* {{{ proto bool stream_filter_register(string filtername, string classname)
   Registers a custom filter handler class */
PHP_FUNCTION(stream_filter_register)
{
	zend_string *filtername, *classname;
	struct php_user_filter_data *fdat;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(filtername)
		Z_PARAM_STR(classname)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	RETVAL_FALSE;

	if (!ZSTR_LEN(filtername)) {
		php_error_docref(NULL, E_WARNING, "Filter name cannot be empty");
		return;
	}

	if (!ZSTR_LEN(classname)) {
		php_error_docref(NULL, E_WARNING, "Class name cannot be empty");
		return;
	}

	if (!BG(user_filter_map)) {
		BG(user_filter_map) = (HashTable*) emalloc(sizeof(HashTable));
		zend_hash_init(BG(user_filter_map), 8, NULL, (dtor_func_t) filter_item_dtor, 0);
	}

	fdat = ecalloc(1, sizeof(struct php_user_filter_data));
	fdat->classname = zend_string_copy(classname);

	if (zend_hash_add_ptr(BG(user_filter_map), filtername, fdat) != NULL &&
			php_stream_filter_register_factory_volatile(filtername, &user_filter_factory) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		zend_string_release_ex(classname, 0);
		efree(fdat);
	}
}
/* }}} */
