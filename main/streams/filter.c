/*
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
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include <stddef.h>
#include <fcntl.h>

#include "php_streams_int.h"

/* Global filter hash, copied to FG(stream_filters) on registration of volatile filter */
static HashTable stream_filters_hash;

/* Should only be used during core initialization */
PHPAPI HashTable *php_get_stream_filters_hash_global(void)
{
	return &stream_filters_hash;
}

/* Normal hash selection/retrieval call */
PHPAPI HashTable *_php_get_stream_filters_hash(void)
{
	return (FG(stream_filters) ? FG(stream_filters) : &stream_filters_hash);
}

/* API for registering GLOBAL filters */
PHPAPI int php_stream_filter_register_factory(const char *filterpattern, const php_stream_filter_factory *factory)
{
	int ret;
	zend_string *str = zend_string_init_interned(filterpattern, strlen(filterpattern), 1);
	ret = zend_hash_add_ptr(&stream_filters_hash, str, (void*)factory) ? SUCCESS : FAILURE;
	zend_string_release_ex(str, 1);
	return ret;
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern)
{
	return zend_hash_str_del(&stream_filters_hash, filterpattern, strlen(filterpattern));
}

/* API for registering VOLATILE wrappers */
PHPAPI int php_stream_filter_register_factory_volatile(zend_string *filterpattern, const php_stream_filter_factory *factory)
{
	if (!FG(stream_filters)) {
		ALLOC_HASHTABLE(FG(stream_filters));
		zend_hash_init(FG(stream_filters), zend_hash_num_elements(&stream_filters_hash) + 1, NULL, NULL, 0);
		zend_hash_copy(FG(stream_filters), &stream_filters_hash, NULL);
	}

	return zend_hash_add_ptr(FG(stream_filters), filterpattern, (void*)factory) ? SUCCESS : FAILURE;
}

/* Buckets */

PHPAPI php_stream_bucket *php_stream_bucket_new(php_stream *stream, char *buf, size_t buflen, uint8_t own_buf, uint8_t buf_persistent)
{
	int is_persistent = php_stream_is_persistent(stream);
	php_stream_bucket *bucket;

	bucket = (php_stream_bucket*)pemalloc(sizeof(php_stream_bucket), is_persistent);
	bucket->next = bucket->prev = NULL;

	if (is_persistent && !buf_persistent) {
		/* all data in a persistent bucket must also be persistent */
		bucket->buf = pemalloc(buflen, 1);
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
	bucket->brigade = NULL;

	return bucket;
}

/* Given a bucket, returns a version of that bucket with a writeable buffer.
 * If the original bucket has a refcount of 1 and owns its buffer, then it
 * is returned unchanged.
 * Otherwise, a copy of the buffer is made.
 * In both cases, the original bucket is unlinked from its brigade.
 * If a copy is made, the original bucket is delref'd.
 * */
PHPAPI php_stream_bucket *php_stream_bucket_make_writeable(php_stream_bucket *bucket)
{
	php_stream_bucket *retval;

	php_stream_bucket_unlink(bucket);

	if (bucket->refcount == 1 && bucket->own_buf) {
		return bucket;
	}

	retval = (php_stream_bucket*)pemalloc(sizeof(php_stream_bucket), bucket->is_persistent);
	memcpy(retval, bucket, sizeof(*retval));

	retval->buf = pemalloc(retval->buflen, retval->is_persistent);
	memcpy(retval->buf, bucket->buf, retval->buflen);

	retval->refcount = 1;
	retval->own_buf = 1;

	php_stream_bucket_delref(bucket);

	return retval;
}

PHPAPI int php_stream_bucket_split(php_stream_bucket *in, php_stream_bucket **left, php_stream_bucket **right, size_t length)
{
	*left = (php_stream_bucket*)pecalloc(1, sizeof(php_stream_bucket), in->is_persistent);
	*right = (php_stream_bucket*)pecalloc(1, sizeof(php_stream_bucket), in->is_persistent);

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
}

PHPAPI void php_stream_bucket_delref(php_stream_bucket *bucket)
{
	if (--bucket->refcount == 0) {
		if (bucket->own_buf) {
			pefree(bucket->buf, bucket->is_persistent);
		}
		pefree(bucket, bucket->is_persistent);
	}
}

PHPAPI void php_stream_bucket_prepend(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket)
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

PHPAPI void php_stream_bucket_append(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket)
{
	if (brigade->tail == bucket) {
		return;
	}

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

PHPAPI void php_stream_bucket_unlink(php_stream_bucket *bucket)
{
	if (bucket->prev) {
		bucket->prev->next = bucket->next;
	} else if (bucket->brigade) {
		bucket->brigade->head = bucket->next;
	}
	if (bucket->next) {
		bucket->next->prev = bucket->prev;
	} else if (bucket->brigade) {
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
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, zval *filterparams, uint8_t persistent)
{
	HashTable *filter_hash = (FG(stream_filters) ? FG(stream_filters) : &stream_filters_hash);
	const php_stream_filter_factory *factory = NULL;
	php_stream_filter *filter = NULL;
	size_t n;
	char *period;

	n = strlen(filtername);

	if (NULL != (factory = zend_hash_str_find_ptr(filter_hash, filtername, n))) {
		filter = factory->create_filter(filtername, filterparams, persistent);
	} else if ((period = strrchr(filtername, '.'))) {
		/* try a wildcard */
		char *wildname;

		wildname = safe_emalloc(1, n, 3);
		memcpy(wildname, filtername, n+1);
		period = wildname + (period - filtername);
		while (period && !filter) {
			ZEND_ASSERT(period[0] == '.');
			period[1] = '*';
			period[2] = '\0';
			if (NULL != (factory = zend_hash_str_find_ptr(filter_hash, wildname, strlen(wildname)))) {
				filter = factory->create_filter(filtername, filterparams, persistent);
			}

			*period = '\0';
			period = strrchr(wildname, '.');
		}
		efree(wildname);
	}

	if (filter == NULL) {
		/* TODO: these need correct docrefs */
		if (factory == NULL)
			php_error_docref(NULL, E_WARNING, "Unable to locate filter \"%s\"", filtername);
		else
			php_error_docref(NULL, E_WARNING, "Unable to create or locate filter \"%s\"", filtername);
	}

	return filter;
}

PHPAPI php_stream_filter *_php_stream_filter_alloc(const php_stream_filter_ops *fops, void *abstract, uint8_t persistent STREAMS_DC)
{
	php_stream_filter *filter;

	filter = (php_stream_filter*) pemalloc_rel_orig(sizeof(php_stream_filter), persistent);
	memset(filter, 0, sizeof(php_stream_filter));

	filter->fops = fops;
	Z_PTR(filter->abstract) = abstract;
	filter->is_persistent = persistent;

	return filter;
}

PHPAPI void php_stream_filter_free(php_stream_filter *filter)
{
	if (filter->fops->dtor)
		filter->fops->dtor(filter);
	pefree(filter, filter->is_persistent);
}

PHPAPI int php_stream_filter_prepend_ex(php_stream_filter_chain *chain, php_stream_filter *filter)
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

	return SUCCESS;
}

PHPAPI void _php_stream_filter_prepend(php_stream_filter_chain *chain, php_stream_filter *filter)
{
	php_stream_filter_prepend_ex(chain, filter);
}

PHPAPI int php_stream_filter_append_ex(php_stream_filter_chain *chain, php_stream_filter *filter)
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

	if (&(stream->readfilters) == chain && (stream->writepos - stream->readpos) > 0) {
		/* Let's going ahead and wind anything in the buffer through this filter */
		php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
		php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out;
		php_stream_filter_status_t status;
		php_stream_bucket *bucket;
		size_t consumed = 0;

		bucket = php_stream_bucket_new(stream, (char*) stream->readbuf + stream->readpos, stream->writepos - stream->readpos, 0, 0);
		php_stream_bucket_append(brig_inp, bucket);
		status = filter->fops->filter(stream, filter, brig_inp, brig_outp, &consumed, PSFS_FLAG_NORMAL);

		if (stream->readpos + consumed > (uint32_t)stream->writepos) {
			/* No behaving filter should cause this. */
			status = PSFS_ERR_FATAL;
		}

		switch (status) {
			case PSFS_ERR_FATAL:
				while (brig_in.head) {
					bucket = brig_in.head;
					php_stream_bucket_unlink(bucket);
					php_stream_bucket_delref(bucket);
				}
				while (brig_out.head) {
					bucket = brig_out.head;
					php_stream_bucket_unlink(bucket);
					php_stream_bucket_delref(bucket);
				}
				php_error_docref(NULL, E_WARNING, "Filter failed to process pre-buffered data");
				return FAILURE;
			case PSFS_FEED_ME:
				/* We don't actually need data yet,
				   leave this filter in a feed me state until data is needed.
				   Reset stream's internal read buffer since the filter is "holding" it. */
				stream->readpos = 0;
				stream->writepos = 0;
				break;
			case PSFS_PASS_ON:
				/* If any data is consumed, we cannot rely upon the existing read buffer,
				   as the filtered data must replace the existing data, so invalidate the cache */
				stream->writepos = 0;
				stream->readpos = 0;

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

					php_stream_bucket_unlink(bucket);
					php_stream_bucket_delref(bucket);
				}
				break;
		}
	}

	return SUCCESS;
}

PHPAPI void _php_stream_filter_append(php_stream_filter_chain *chain, php_stream_filter *filter)
{
	if (php_stream_filter_append_ex(chain, filter) != SUCCESS) {
		if (chain->head == filter) {
			chain->head = NULL;
			chain->tail = NULL;
		} else {
			filter->prev->next = NULL;
			chain->tail = filter->prev;
		}
	}
}

PHPAPI int _php_stream_filter_flush(php_stream_filter *filter, int finish)
{
	php_stream_bucket_brigade brig_a = { NULL, NULL }, brig_b = { NULL, NULL }, *inp = &brig_a, *outp = &brig_b, *brig_temp;
	php_stream_bucket *bucket;
	php_stream_filter_chain *chain;
	php_stream_filter *current;
	php_stream *stream;
	size_t flushed_size = 0;
	long flags = (finish ? PSFS_FLAG_FLUSH_CLOSE : PSFS_FLAG_FLUSH_INC);

	if (!filter->chain || !filter->chain->stream) {
		/* Filter is not attached to a chain, or chain is somehow not part of a stream */
		return FAILURE;
	}

	chain = filter->chain;
	stream = chain->stream;

	for(current = filter; current; current = current->next) {
		php_stream_filter_status_t status;

		status = filter->fops->filter(stream, current, inp, outp, NULL, flags);
		if (status == PSFS_FEED_ME) {
			/* We've flushed the data far enough */
			return SUCCESS;
		}
		if (status == PSFS_ERR_FATAL) {
			return FAILURE;
		}
		/* Otherwise we have data available to PASS_ON
			Swap the brigades and continue */
		brig_temp = inp;
		inp = outp;
		outp = brig_temp;
		outp->head = NULL;
		outp->tail = NULL;

		flags = PSFS_FLAG_NORMAL;
	}

	/* Last filter returned data via PSFS_PASS_ON
		Do something with it */

	for(bucket = inp->head; bucket; bucket = bucket->next) {
		flushed_size += bucket->buflen;
	}

	if (flushed_size == 0) {
		/* Unlikely, but possible */
		return SUCCESS;
	}

	if (chain == &(stream->readfilters)) {
		/* Dump any newly flushed data to the read buffer */
		if (stream->readpos > 0) {
			/* Back the buffer up */
			memcpy(stream->readbuf, stream->readbuf + stream->readpos, stream->writepos - stream->readpos);
			stream->readpos = 0;
			stream->writepos -= stream->readpos;
		}
		if (flushed_size > (stream->readbuflen - stream->writepos)) {
			/* Grow the buffer */
			stream->readbuf = perealloc(stream->readbuf, stream->writepos + flushed_size + stream->chunk_size, stream->is_persistent);
		}
		while ((bucket = inp->head)) {
			memcpy(stream->readbuf + stream->writepos, bucket->buf, bucket->buflen);
			stream->writepos += bucket->buflen;
			php_stream_bucket_unlink(bucket);
			php_stream_bucket_delref(bucket);
		}
	} else if (chain == &(stream->writefilters)) {
		/* Send flushed data to the stream */
		while ((bucket = inp->head)) {
			stream->ops->write(stream, bucket->buf, bucket->buflen);
			php_stream_bucket_unlink(bucket);
			php_stream_bucket_delref(bucket);
		}
	}

	return SUCCESS;
}

PHPAPI php_stream_filter *php_stream_filter_remove(php_stream_filter *filter, int call_dtor)
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

	if (filter->res) {
		zend_list_delete(filter->res);
	}

	if (call_dtor) {
		php_stream_filter_free(filter);
		return NULL;
	}
	return filter;
}
