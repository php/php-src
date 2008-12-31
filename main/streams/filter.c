/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

/* $Id$ */

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
PHPAPI HashTable *_php_get_stream_filters_hash(TSRMLS_D)
{
	return (FG(stream_filters) ? FG(stream_filters) : &stream_filters_hash);
}

/* API for registering GLOBAL filters */
PHPAPI int php_stream_filter_register_factory(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC)
{
	return zend_hash_add(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern) + 1, factory, sizeof(*factory), NULL);
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC)
{
	return zend_hash_del(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern) + 1);
}

/* API for registering VOLATILE wrappers */
PHPAPI int php_stream_filter_register_factory_volatile(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC)
{
	if (!FG(stream_filters)) {
		php_stream_filter_factory tmpfactory;

		ALLOC_HASHTABLE(FG(stream_filters));
		zend_hash_init(FG(stream_filters), zend_hash_num_elements(&stream_filters_hash), NULL, NULL, 1);
		zend_hash_copy(FG(stream_filters), &stream_filters_hash, NULL, &tmpfactory, sizeof(php_stream_filter_factory));
	}

	return zend_hash_add(FG(stream_filters), (char*)filterpattern, strlen(filterpattern) + 1, factory, sizeof(*factory), NULL);
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
		bucket->buf.s = pemalloc(buflen, 1);
		
		if (bucket->buf.s == NULL) {
			pefree(bucket, 1);
			return NULL;
		}
		
		memcpy(bucket->buf.s, buf, buflen);
		bucket->buflen = buflen;
		bucket->own_buf = 1;
	} else {
		bucket->buf.s = buf;
		bucket->buflen = buflen;
		bucket->own_buf = own_buf;
	}
	bucket->buf_type = IS_STRING;
	bucket->is_persistent = is_persistent;
	bucket->refcount = 1;
	bucket->brigade = NULL;

	return bucket;
}

PHPAPI php_stream_bucket *php_stream_bucket_new_unicode(php_stream *stream, UChar *buf, int32_t buflen, int own_buf, int buf_persistent TSRMLS_DC)
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
		bucket->buf.u = safe_pemalloc(sizeof(UChar), buflen, 0, 1);
		
		if (bucket->buf.u == NULL) {
			pefree(bucket, 1);
			return NULL;
		}
		
		memcpy(bucket->buf.u, buf, buflen);
		bucket->buflen = buflen;
		bucket->own_buf = 1;
	} else {
		bucket->buf.u = buf;
		bucket->buflen = buflen;
		bucket->own_buf = own_buf;
	}
	bucket->buf_type = IS_UNICODE;
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
PHPAPI php_stream_bucket *php_stream_bucket_make_writeable(php_stream_bucket *bucket TSRMLS_DC)
{
	php_stream_bucket *retval;

	php_stream_bucket_unlink(bucket TSRMLS_CC);
	
	if (bucket->refcount == 1 && bucket->own_buf) {
		return bucket;
	}

	retval = (php_stream_bucket*)pemalloc(sizeof(php_stream_bucket), bucket->is_persistent);
	memcpy(retval, bucket, sizeof(*retval));

	if (bucket->buf_type == IS_UNICODE) {
		retval->buf.u = safe_pemalloc(sizeof(UChar), retval->buflen, 0, retval->is_persistent);
		memcpy(retval->buf.u, bucket->buf.u, UBYTES(retval->buflen));
	} else {
		retval->buf.s = pemalloc(retval->buflen, retval->is_persistent);
		memcpy(retval->buf.s, bucket->buf.s, retval->buflen);
	}

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

	if (in->buf_type == IS_UNICODE) {
		(*left)->buf.u = safe_pemalloc(sizeof(UChar), length, 0, in->is_persistent);
		(*left)->buflen = length;
		memcpy((*left)->buf.u, in->buf.u, UBYTES(length));
	
		(*right)->buflen = in->buflen - length;
		(*right)->buf.u = pemalloc(UBYTES((*right)->buflen), in->is_persistent);
		memcpy((*right)->buf.u, in->buf.u + length, UBYTES((*right)->buflen));
	} else {
		(*left)->buf.s = pemalloc(length, in->is_persistent);
		(*left)->buflen = length;
		memcpy((*left)->buf.s, in->buf.s, length);
	
		(*right)->buflen = in->buflen - length;
		(*right)->buf.s = pemalloc((*right)->buflen, in->is_persistent);
		memcpy((*right)->buf.s, in->buf.s + length, (*right)->buflen);
	}

	(*left)->refcount = 1;
	(*left)->own_buf = 1;
	(*left)->is_persistent = in->is_persistent;
	(*left)->buf_type = in->buf_type;

	(*right)->refcount = 1;
	(*right)->own_buf = 1;
	(*right)->is_persistent = in->is_persistent;
	(*right)->buf_type = in->buf_type;
	
	return SUCCESS;
	
exit_fail:
	if (*right) {
		if ((*right)->buf.v) {
			pefree((*right)->buf.v, in->is_persistent);
		}
		pefree(*right, in->is_persistent);
	}
	if (*left) {
		if ((*left)->buf.v) {
			pefree((*left)->buf.v, in->is_persistent);
		}
		pefree(*left, in->is_persistent);
	}
	return FAILURE;
}

PHPAPI void php_stream_bucket_delref(php_stream_bucket *bucket TSRMLS_DC)
{
	if (--bucket->refcount == 0) {
		if (bucket->own_buf) {
			pefree(bucket->buf.v, bucket->is_persistent);
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

PHPAPI void php_stream_bucket_unlink(php_stream_bucket *bucket TSRMLS_DC)
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
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	HashTable *filter_hash = (FG(stream_filters) ? FG(stream_filters) : &stream_filters_hash);
	php_stream_filter_factory *factory = NULL;
	php_stream_filter *filter = NULL;
	int n;
	char *period;

	n = strlen(filtername);
	
	if (SUCCESS == zend_hash_find(filter_hash, (char*)filtername, n + 1, (void**)&factory)) {
		filter = factory->create_filter(filtername, filterparams, persistent TSRMLS_CC);
	} else if ((period = strrchr(filtername, '.'))) {
		/* try a wildcard */
		char *wildname;

		wildname = emalloc(n+3);
		memcpy(wildname, filtername, n+1);
		period = wildname + (period - filtername);
		while (period && !filter) {
			*period = '\0';
			strcat(wildname, ".*");
			if (SUCCESS == zend_hash_find(filter_hash, wildname, strlen(wildname) + 1, (void**)&factory)) {
				filter = factory->create_filter(filtername, filterparams, persistent TSRMLS_CC);
			}

			*period = '\0';
			period = strrchr(wildname, '.');
		}
		efree(wildname);
	}

	if (filter == NULL) {
		/* TODO: these need correct docrefs */
		if (factory == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to locate filter \"%s\"", filtername);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create or locate filter \"%s\"", filtername);
		}
		return NULL;
	}

	filter->name = pestrdup(filtername, filter->is_persistent);
	
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
	pefree(filter->name, filter->is_persistent);
	pefree(filter, filter->is_persistent);
}

PHPAPI void _php_stream_filter_prepend(php_stream_filter_chain *chain, php_stream_filter *filter TSRMLS_DC)
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

PHPAPI void _php_stream_filter_append(php_stream_filter_chain *chain, php_stream_filter *filter TSRMLS_DC)
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

	if (&(stream->readfilters) == chain) {
		/* Let's going ahead and wind anything in the buffer through this filter */
		php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
		php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out;
		php_stream_filter_status_t status = PSFS_FEED_ME;
		php_stream_bucket *bucket;
		size_t consumed = 0;

		if ((stream->writepos - stream->readpos) > 0) {
			if (stream->readbuf_type == IS_UNICODE) {
				bucket = php_stream_bucket_new_unicode(stream, stream->readbuf.u + stream->readpos, stream->writepos - stream->readpos, 0, 0 TSRMLS_CC);
			} else {
				bucket = php_stream_bucket_new(stream, stream->readbuf.s + stream->readpos, stream->writepos - stream->readpos, 0, 0 TSRMLS_CC);
			}
			php_stream_bucket_append(brig_inp, bucket TSRMLS_CC);
			status = filter->fops->filter(stream, filter, brig_inp, brig_outp, &consumed, PSFS_FLAG_NORMAL TSRMLS_CC);

			if (stream->readpos + consumed > stream->writepos || consumed < 0) {
				/* No behaving filter should cause this. */
				status = PSFS_ERR_FATAL;
			}
		}

		if (status == PSFS_ERR_FATAL) {
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter failed to process pre-buffered data.  Not adding to filterchain");
		} else {
			/* This filter addition may change the readbuffer type.
			   Since all the previously held data is in the bucket brigade,
			   we can reappropriate the buffer that already exists (if one does) */
			if (stream->readbuf_type == IS_UNICODE && (filter->fops->flags & PSFO_FLAG_OUTPUTS_UNICODE) == 0) {
				/* Buffer is currently based on unicode characters, but filter only outputs STRING adjust counting */
				stream->readbuf_type = IS_STRING;
				stream->readbuflen *= UBYTES(1);
			} else if (stream->readbuf_type == IS_STRING && (filter->fops->flags & PSFO_FLAG_OUTPUTS_STRING) == 0) {
				/* Buffer is currently based on binary characters, but filter only outputs UNICODE adjust counting */
				stream->readbuf_type = IS_UNICODE;
				stream->readbuflen /= UBYTES(1);
			}

			if (status == PSFS_FEED_ME) {
				/* We don't actually need data yet,
				   leave this filter in a feed me state until data is needed. 
				   Reset stream's internal read buffer since the filter is "holding" it. */
				stream->readpos = 0;
				stream->writepos = 0;
			} else if (status == PSFS_PASS_ON) {
				/* If any data is consumed, we cannot rely upon the existing read buffer,
				   as the filtered data must replace the existing data, so invalidate the cache */
				/* note that changes here should be reflected in
				   main/streams/streams.c::php_stream_fill_read_buffer */
				stream->writepos = 0;
				stream->readpos = 0;

				while (brig_outp->head) {
					bucket = brig_outp->head;

					/* Convert for stream type */
					if (bucket->buf_type != stream->readbuf_type) {
						/* Stream expects different type than bucket contains, convert slopily */
						php_stream_bucket_convert_notranscode(bucket, stream->readbuf_type);
					}

					/* Grow buffer to hold this bucket if need be.
					   TODO: See warning in main/stream/streams.c::php_stream_fill_read_buffer */
					if (stream->readbuflen - stream->writepos < bucket->buflen) {
						stream->readbuflen += bucket->buflen;
						stream->readbuf.v = perealloc(stream->readbuf.v, PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->readbuflen), stream->is_persistent);
					}

					/* Append to readbuf */
					if (stream->readbuf_type == IS_UNICODE) {
						memcpy(stream->readbuf.u + stream->writepos, bucket->buf.u, UBYTES(bucket->buflen));
					} else {
						memcpy(stream->readbuf.s + stream->writepos, bucket->buf.s, bucket->buflen);
					}
					stream->writepos += bucket->buflen;

					php_stream_bucket_unlink(bucket TSRMLS_CC);
					php_stream_bucket_delref(bucket TSRMLS_CC);
				}
			}
		}
	} /* end of readfilters specific code */
}

PHPAPI int _php_stream_filter_check_chain(php_stream_filter_chain *chain TSRMLS_DC)
{
	php_stream_filter *filter;
	long last_output = PSFO_FLAG_OUTPUTS_ANY;

	for(filter = chain->head; filter; filter = filter->next) {
		if ((((filter->fops->flags & PSFO_FLAG_ACCEPT_MASK) << PSFO_FLAG_ACCEPT_SHIFT) & last_output) == 0) {
			/* Nothing which the last filter outputs is accepted by this filter */
			return FAILURE;
		}
		if (filter->fops->flags & PSFO_FLAG_OUTPUTS_SAME) {
			continue;
		}
		if (filter->fops->flags & PSFO_FLAG_OUTPUTS_OPPOSITE) {
			last_output = ((last_output & PSFO_FLAG_OUTPUTS_STRING)  ? PSFO_FLAG_OUTPUTS_UNICODE : 0) |
						  ((last_output & PSFO_FLAG_OUTPUTS_UNICODE) ? PSFO_FLAG_OUTPUTS_STRING  : 0);
			continue;
		}
		last_output = filter->fops->flags & PSFO_FLAG_OUTPUTS_ANY;
	}

	return SUCCESS;
}

PHPAPI int _php_stream_filter_output_prefer_unicode(php_stream_filter *filter TSRMLS_DC)
{
	php_stream_filter_chain *chain = filter->chain;
	php_stream_filter *f;
	int inverted = 0;
	int preferred = (chain == &chain->stream->readfilters ? 1 : 0);

	for (f = filter->next; f ; f = f->next) {
		if ((f->fops->flags & PSFO_FLAG_ACCEPTS_STRING) == 0) {
			return inverted ^= 1;
		}
		if ((f->fops->flags & PSFO_FLAG_ACCEPTS_UNICODE) == 0) {
			return inverted;
		}
		if (((f->fops->flags & PSFO_FLAG_OUTPUTS_SAME) == 0) &&
			((f->fops->flags & PSFO_FLAG_OUTPUTS_OPPOSITE) == 0)) {
			/* Input type for next filter won't effect output -- Might as well go for unicode */
			return inverted ^ 1;
		}
		if (f->fops->flags & PSFO_FLAG_OUTPUTS_SAME) {
			continue;
		}
		if (f->fops->flags & PSFO_FLAG_OUTPUTS_OPPOSITE) {
			inverted ^= 1;
			continue;
		}
	}

	return preferred ^ inverted;
}

PHPAPI int _php_stream_filter_product(php_stream_filter_chain *chain, int type TSRMLS_DC)
{
	php_stream_filter *f;	

	for (f = chain->head; f; f = f->next) {
		if ((type == IS_STRING && (f->fops->flags & PSFO_FLAG_ACCEPTS_STRING) == 0) ||
			(type == IS_UNICODE && (f->fops->flags & PSFO_FLAG_ACCEPTS_UNICODE) == 0)) {
			/* At some point, the type produced conflicts with the type accepted */
			return 0;
		}

		if (f->fops->flags & PSFO_FLAG_OUTPUTS_OPPOSITE) {
			type = (type == IS_STRING) ? IS_UNICODE : IS_STRING;
			continue;
		}
		if ((f->fops->flags & PSFO_FLAG_OUTPUTS_SAME) ||
			(f->fops->flags & PSFO_FLAG_OUTPUTS_ANY)) {
			continue;
		}
		if (f->fops->flags & PSFO_FLAG_OUTPUTS_UNICODE) {
			type = IS_UNICODE;
			continue;
		}
		type = IS_STRING;
	}

	return type;
}

PHPAPI int _php_stream_filter_flush(php_stream_filter *filter, int finish TSRMLS_DC)
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

		status = filter->fops->filter(stream, filter, inp, outp, NULL, flags TSRMLS_CC);
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
		if (stream->readpos > stream->chunk_size) {
			/* Back the buffer up */
			memcpy(stream->readbuf.s, stream->readbuf.s + PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->readpos), PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->writepos - stream->readpos));
			stream->writepos -= stream->readpos;
			stream->readpos = 0;
		}
		if (flushed_size > (stream->readbuflen - stream->writepos)) {
			/* Grow the buffer */
			stream->readbuf.v = perealloc(stream->readbuf.v, PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->writepos + flushed_size + stream->chunk_size), stream->is_persistent);
		}
		while ((bucket = inp->head)) {
			/* Convert if necessary */
			if (bucket->buf_type != stream->readbuf_type) {
				/* Stream expects different type than what's in bucket, convert slopily */
				php_stream_bucket_convert_notranscode(bucket, stream->readbuf_type);
			}

			/* Append to readbuf */
			if (stream->readbuf_type == IS_UNICODE) {
				 memcpy(stream->readbuf.u + stream->writepos, bucket->buf.u, UBYTES(bucket->buflen));
			} else {
				 memcpy(stream->readbuf.s + stream->writepos, bucket->buf.s, bucket->buflen);
			}
			stream->writepos += bucket->buflen;
			php_stream_bucket_unlink(bucket TSRMLS_CC);
			php_stream_bucket_delref(bucket TSRMLS_CC);
		}


	} else if (chain == &(stream->writefilters)) {
		/* Send flushed data to the stream */
		while ((bucket = inp->head)) {
			/* Convert if necessary */
			if (bucket->buf_type == IS_UNICODE) {
				/* Force data to binary, adjusting buflen */
				php_stream_bucket_convert_notranscode(bucket, IS_STRING);
			}

			/* Must be binary by this point */
			stream->ops->write(stream, bucket->buf.s, bucket->buflen TSRMLS_CC);

			php_stream_bucket_unlink(bucket TSRMLS_CC);
			php_stream_bucket_delref(bucket TSRMLS_CC);
		}
	}

	return SUCCESS;
}

PHPAPI php_stream_filter *php_stream_filter_remove(php_stream_filter *filter, int call_dtor TSRMLS_DC)
{
	/* UTODO: Figure out a sane way to "defilter" so that unicode converters can be swapped around
	   For now, at least fopen(,'b') + stream_encoding($fp, 'charset') works since there's nothing to remove */

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

	if (filter->rsrc_id > 0) {
		zend_list_delete(filter->rsrc_id);
	}

	if (call_dtor) {
		php_stream_filter_free(filter TSRMLS_CC);
		return NULL;
	}
	return filter;
}

PHPAPI int _php_stream_bucket_convert(php_stream_bucket *bucket, unsigned char type, UConverter *conv TSRMLS_DC)
{
	if (bucket->buf_type == type) {
		return SUCCESS;
	}

	if (conv) {
		/* transcode current type using provided converter */
		if (type == IS_UNICODE) {
			UErrorCode status = U_ZERO_ERROR;
			UChar *dest;
			int destlen;

			zend_string_to_unicode_ex(conv, &dest, &destlen, bucket->buf.s, bucket->buflen, &status);

			if (bucket->own_buf) {
				pefree(bucket->buf.s, bucket->is_persistent);
			}

			/* Might be dangerous, double check this (or, better, get a persistent version of zend_string_to_unicode_ex() */
			bucket->is_persistent = 0;

			bucket->buf_type = IS_UNICODE;
			bucket->buf.u = dest;
			bucket->buflen = destlen;
			bucket->own_buf = 1;
			return SUCCESS;
		} else {
			UErrorCode status = U_ZERO_ERROR;
			char *dest;
			int destlen, num_conv;

			num_conv = zend_unicode_to_string_ex(conv, &dest, &destlen, bucket->buf.u, bucket->buflen, &status);
			if (U_FAILURE(status)) {
				int32_t offset = u_countChar32(bucket->buf.u, num_conv);

				zend_raise_conversion_error_ex("Could not convert Unicode string to binary string", conv, ZEND_FROM_UNICODE, offset TSRMLS_CC);
			}

			if (bucket->own_buf) {
				pefree(bucket->buf.u, bucket->is_persistent);
			}

			/* See above */
			bucket->is_persistent = 0;

			bucket->buf_type = IS_STRING;
			bucket->buf.s = dest;
			bucket->buflen = destlen;
			bucket->own_buf = 1;
			return SUCCESS;
		}
	} else {
		/* Convert without transcode, usually a bad idea as it creates ugly data,
		   When binary streams receive unicode data (because of filters or writing unicode strings)
		   this is the only option */
		if (type == IS_UNICODE) {
			if (bucket->buflen == 0) {
				/* Shortcut conversion for empty buckets */
				if (bucket->own_buf) {
					pefree(bucket->buf.s, bucket->is_persistent);
				}
				bucket->buf_type = IS_UNICODE;
				bucket->buf.u = EMPTY_STR;
				bucket->own_buf = 0;
				bucket->buflen = 0;
				return SUCCESS;
			}

			if (bucket->own_buf) {
				/* one UChar padding for partial units, one more for terminating NULL */
				bucket->buf.s = perealloc(bucket->buf.s, bucket->buflen + UBYTES(2), bucket->is_persistent);
			} else {
				int destlen = ceil(bucket->buflen / sizeof(UChar));
				UChar *dest = pemalloc(UBYTES(destlen + 2), bucket->is_persistent);
				memcpy(dest, bucket->buf.s, bucket->buflen);
				bucket->buf.u = dest;
				bucket->own_buf = 1;
			}
			memset(bucket->buf.s + bucket->buflen, 0, UBYTES(2));
			bucket->buf_type = IS_UNICODE;
			bucket->buflen = ceil(bucket->buflen / sizeof(UChar));
			return SUCCESS;
		} else { /* IS_STRING */
			bucket->buf_type = IS_STRING;
			bucket->buflen *= 2;
			return SUCCESS;
		}
	}

	/* Never reached */
	return FAILURE;
}

PHPAPI int _php_stream_encoding_apply(php_stream *stream, int writechain, const char *encoding, uint16_t error_mode, UChar *subst TSRMLS_DC)
{
	int encoding_len = strlen(encoding);
	int buflen = sizeof("unicode.from.") + encoding_len - 1; /* might be "to", but "from" is long enough for both */
	char *buf = emalloc(buflen + 1);
	php_stream_filter *filter;
	zval *filterparams;

	if (writechain) {
		memcpy(buf, "unicode.to.", sizeof("unicode.to.") - 1);
		memcpy(buf + sizeof("unicode.to.") - 1, encoding, encoding_len + 1);
	} else {
		memcpy(buf, "unicode.from.", sizeof("unicode.from.") - 1);
		memcpy(buf + sizeof("unicode.from.") - 1, encoding, encoding_len + 1);
	}

	ALLOC_INIT_ZVAL(filterparams);
	array_init(filterparams);
	add_ascii_assoc_long(filterparams, "error_mode", error_mode);
	if (subst) {
		add_ascii_assoc_unicode(filterparams, "subst_char", subst, 1);
	}
	filter = php_stream_filter_create(buf, filterparams, php_stream_is_persistent(stream) TSRMLS_CC);
	efree(buf);
	zval_ptr_dtor(&filterparams);

	if (!filter) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to apply encoding for charset: %s\n", encoding);
		return FAILURE;
	}

	php_stream_filter_append(writechain ? &stream->writefilters : &stream->readfilters, filter);

	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
