/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include <stddef.h>
#include <fcntl.h>

#include "php_streams_int.h"

static HashTable stream_filters_hash;

PHPAPI HashTable *php_get_stream_filters_hash()
{
	return &stream_filters_hash;
}

PHPAPI int php_stream_filter_register_factory(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC)
{
	return zend_hash_add(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern), factory, sizeof(*factory), NULL);
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC)
{
	return zend_hash_del(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern));
}

/* Buckets */

PHPAPI php_stream_bucket *php_stream_bucket_new(php_stream *stream, char *buf, size_t buflen, int own_buf, int buf_persistent TSRMLS_DC)
{
	int is_persistent = php_stream_is_persistent(stream);
	php_stream_bucket *bucket;

	bucket = (php_stream_bucket*)pemalloc(sizeof(php_stream_bucket), is_persistent);

	if (bucket == NULL) {
		return NULL;
	}
	
	bucket->next = bucket->prev = NULL;

	if (is_persistent && !buf_persistent) {
		/* all data in a persistent bucket must also be persistent */
		bucket->buf = pemalloc(buflen, 1);
		
		if (bucket->buf == NULL) {
			pefree(bucket, 1);
			return NULL;
		}
		
		memcpy(bucket->buf, buf, buflen);
		bucket->buflen = buflen;
		bucket->own_buf = 1;
	} else {
		bucket->buf = buf;
		bucket->buflen = buflen;
		bucket->own_buf = own_buf;
	}
	bucket->is_persistent = is_persistent;
	bucket->refcount = 1;

	return bucket;
}

/* Given a bucket, returns a version of that bucket with a writeable buffer.
 * If the original bucket has a refcount of 1 and owns its buffer, then it
 * is returned unchanged.
 * Otherwise, a copy of the buffer is made.
 * In both cases, the original bucket is unlinked from its brigade.
 * If a copy is made, the original bucket is delref'd.
 * */
PHPAPI php_stream_bucket *php_stream_bucket_make_writeable(php_stream_bucket *bucket TSRMLS_DC)
{
	php_stream_bucket *retval;

	php_stream_bucket_unlink(bucket TSRMLS_CC);
	
	if (bucket->refcount == 1 && bucket->own_buf) {
		return bucket;
	}

	retval = (php_stream_bucket*)pemalloc(sizeof(php_stream_bucket), bucket->is_persistent);
	memcpy(retval, bucket, sizeof(*retval));

	retval->buf = pemalloc(retval->buflen, retval->is_persistent);
	memcpy(retval->buf, bucket->buf, retval->buflen);

	retval->refcount = 1;
	retval->own_buf = 1;

	php_stream_bucket_delref(bucket TSRMLS_CC);
	
	return retval;
}

PHPAPI int php_stream_bucket_split(php_stream_bucket *in, php_stream_bucket **left, php_stream_bucket **right, size_t length TSRMLS_DC)
{
	*left = (php_stream_bucket*)pecalloc(1, sizeof(php_stream_bucket), in->is_persistent);
	*right = (php_stream_bucket*)pecalloc(1, sizeof(php_stream_bucket), in->is_persistent);

	if (*left == NULL || *right == NULL) {
		goto exit_fail;
	}

	(*left)->buf = pemalloc(length, in->is_persistent);
	(*left)->buflen = length;
	memcpy((*left)->buf, in->buf, length);
	(*left)->refcount = 1;
	(*left)->own_buf = 1;
	(*left)->is_persistent = in->is_persistent;
	
	(*right)->buflen = in->buflen - length;
	(*right)->buf = pemalloc((*right)->buflen, in->is_persistent);
	memcpy((*right)->buf, in->buf + length, (*right)->buflen);
	(*right)->refcount = 1;
	(*right)->own_buf = 1;
	(*right)->is_persistent = in->is_persistent;
	
	return SUCCESS;
	
exit_fail:
	if (*right) {
		if ((*right)->buf) {
			pefree((*right)->buf, in->is_persistent);
		}
		pefree(*right, in->is_persistent);
	}
	if (*left) {
		if ((*left)->buf) {
			pefree((*left)->buf, in->is_persistent);
		}
		pefree(*left, in->is_persistent);
	}
	return FAILURE;
}

PHPAPI void php_stream_bucket_delref(php_stream_bucket *bucket TSRMLS_DC)
{
	if (--bucket->refcount == 0) {
		if (bucket->own_buf) {
			pefree(bucket->buf, bucket->is_persistent);
		}
		pefree(bucket, bucket->is_persistent);
	}
}

PHPAPI void php_stream_bucket_prepend(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket TSRMLS_DC)
{
	bucket->next = brigade->head;
	bucket->prev = NULL;

	if (brigade->head) {
		brigade->head->prev = bucket;
	} else {
		brigade->tail = bucket;
	}
	brigade->head = bucket;
	bucket->brigade = brigade;
}

PHPAPI void php_stream_bucket_append(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket TSRMLS_DC)
{
	bucket->prev = brigade->tail;
	bucket->next = NULL;

	if (brigade->tail) {
		brigade->tail->next = bucket;
	} else {
		brigade->head = bucket;
	}
	brigade->tail = bucket;
	bucket->brigade = brigade;
}

PHPAPI void php_stream_bucket_unlink(php_stream_bucket *bucket TSRMLS_DC)
{
	if (bucket->prev) {
		bucket->prev->next = bucket->next;
	} else {
		bucket->brigade->head = bucket->next;
	}
	if (bucket->next) {
		bucket->next->prev = bucket->prev;
	} else {
		bucket->brigade->tail = bucket->prev;
	}
	bucket->brigade = NULL;
	bucket->next = bucket->prev = NULL;
}
	







/* We allow very simple pattern matching for filter factories:
 * if "convert.charset.utf-8/sjis" is requested, we search first for an exact
 * match. If that fails, we try "convert.charset.*", then "convert.*"
 * This means that we don't need to clog up the hashtable with a zillion
 * charsets (for example) but still be able to provide them all as filters */
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_stream_filter_factory *factory;
	php_stream_filter *filter = NULL;
	int n;
	char *period;

	n = strlen(filtername);
	
	if (SUCCESS == zend_hash_find(&stream_filters_hash, (char*)filtername, n, (void**)&factory)) {
		filter = factory->create_filter(filtername, filterparams, persistent TSRMLS_CC);
	} else if ((period = strrchr(filtername, '.'))) {
		/* try a wildcard */
		char *wildname;

		wildname = estrdup(filtername);
		period = wildname + (period - filtername);
		while (period && !filter) {
			*period = '\0';
			strcat(wildname, ".*");
			if (SUCCESS == zend_hash_find(&stream_filters_hash, wildname, strlen(wildname), (void**)&factory)) {
				filter = factory->create_filter(filtername, filterparams, persistent TSRMLS_CC);
			}

			*period = '\0';
			period = strrchr(wildname, '.');
		}
		efree(wildname);
	}

	if (filter == NULL) {
		/* TODO: these need correct docrefs */
		if (factory == NULL)
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to locate filter \"%s\"", filtername);
		else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create or locate filter \"%s\"", filtername);
	}
	
	return filter;
}

PHPAPI php_stream_filter *_php_stream_filter_alloc(php_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC)
{
	php_stream_filter *filter;

	filter = (php_stream_filter*) pemalloc_rel_orig(sizeof(php_stream_filter), persistent);
	memset(filter, 0, sizeof(php_stream_filter));

	filter->fops = fops;
	filter->abstract = abstract;
	filter->is_persistent = persistent;
	
	return filter;
}

PHPAPI void php_stream_filter_free(php_stream_filter *filter TSRMLS_DC)
{
	if (filter->fops->dtor)
		filter->fops->dtor(filter TSRMLS_CC);
	pefree(filter, filter->is_persistent);
}

PHPAPI void php_stream_filter_prepend(php_stream_filter_chain *chain, php_stream_filter *filter TSRMLS_DC)
{
	filter->next = chain->head;
	filter->prev = NULL;

	if (chain->head) {
		chain->head->prev = filter;
	} else {
		chain->tail = filter;
	}
	chain->head = filter;
	filter->chain = chain;
}

PHPAPI void php_stream_filter_append(php_stream_filter_chain *chain, php_stream_filter *filter TSRMLS_DC)
{
	php_stream *stream = chain->stream;

	filter->prev = chain->tail;
	filter->next = NULL;
	if (chain->tail) {
		chain->tail->next = filter;
	} else {
		chain->head = filter;
	}
	chain->tail = filter;
	filter->chain = chain;

	if ((stream->writepos - stream->readpos) > 0) {
		/* Let's going ahead and wind anything in the buffer through this filter */
		php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
		php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out;
		php_stream_filter_status_t status;
		php_stream_bucket *bucket;
		size_t consumed = 0;

		bucket = php_stream_bucket_new(stream, stream->readbuf + stream->readpos, stream->writepos - stream->readpos, 0, 0 TSRMLS_CC);
		php_stream_bucket_append(brig_inp, bucket TSRMLS_CC);
		status = filter->fops->filter(stream, filter, brig_inp, brig_outp, &consumed, PSFS_FLAG_NORMAL TSRMLS_CC);

		if (stream->readpos + consumed > stream->writepos || consumed < 0) {
			/* No behaving filter should cause this. */
			status = PSFS_ERR_FATAL;
		}

		switch (status) {
			case PSFS_ERR_FATAL:
				/* If this first cycle simply fails then there's something wrong with the filter.
				   Pull the filter off the chain and leave the read buffer alone. */
				if (chain->head == filter) {
					chain->head = NULL;
					chain->tail = NULL;
				} else {
					filter->prev->next = NULL;
					chain->tail = filter->prev;
				}
				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter failed to process pre-buffered data.  Not adding to filterchain.");
				break;
			case PSFS_FEED_ME:
				/* We don't actually need data yet,
				   leave this filter in a feed me state until data is needed. 
				   Reset stream's internal read buffer since the filter is "holding" it. */
				stream->readpos = 0;
				stream->writepos = 0;
				break;
			case PSFS_PASS_ON:
				/* Put any filtered data onto the readbuffer stack.
				   Previously read data has been at least partially consumed. */
				stream->readpos += consumed;

				if (stream->writepos == stream->readpos) {
					/* Entirely consumed */
					stream->writepos = 0;
					stream->readpos = 0;
				}

				while (brig_outp->head) {
					bucket = brig_outp->head;
					/* Grow buffer to hold this bucket if need be.
					   TODO: See warning in main/stream/streams.c::php_stream_fill_read_buffer */
					if (stream->readbuflen - stream->writepos < bucket->buflen) {
						stream->readbuflen += bucket->buflen;
						stream->readbuf = perealloc(stream->readbuf, stream->readbuflen, stream->is_persistent);
					}
					memcpy(stream->readbuf + stream->writepos, bucket->buf, bucket->buflen);
					stream->writepos += bucket->buflen;

					php_stream_bucket_unlink(bucket TSRMLS_CC);
					php_stream_bucket_delref(bucket TSRMLS_CC);
				}
				break;
		}
	}

}

PHPAPI php_stream_filter *php_stream_filter_remove(php_stream_filter *filter, int call_dtor TSRMLS_DC)
{
	if (filter->prev) {
		filter->prev->next = filter->next;
	} else {
		filter->chain->head = filter->next;
	}
	if (filter->next) {
		filter->next->prev = filter->prev;
	} else {
		filter->chain->tail = filter->prev;
	}
	if (call_dtor) {
		php_stream_filter_free(filter TSRMLS_CC);
		return NULL;
	}
	return filter;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
