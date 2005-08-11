/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
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

/* Global filter hash, copied to FG(stream_filters) on registration of volatile filter */
static HashTable stream_filters_hash;

/* Should only be used during core initialization */
PHPAPI HashTable *php_get_stream_filters_hash_global()
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
	return zend_hash_add(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern), factory, sizeof(*factory), NULL);
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC)
{
	return zend_hash_del(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern));
}

/* API for registering VOLATILE wrappers */
PHPAPI int php_stream_filter_register_factory_volatile(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC)
{
	if (!FG(stream_filters)) {
		php_stream_filter_factory tmpfactory;

		ALLOC_HASHTABLE(FG(stream_filters));
		zend_hash_init(FG(stream_filters), 0, NULL, NULL, 1);
		zend_hash_copy(FG(stream_filters), &stream_filters_hash, NULL, &tmpfactory, sizeof(php_stream_filter_factory));
	}

	return zend_hash_add(FG(stream_filters), (char*)filterpattern, strlen(filterpattern), factory, sizeof(*factory), NULL);
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
		bucket->buf.str.val = pemalloc(buflen, 1);
		
		if (bucket->buf.str.val == NULL) {
			pefree(bucket, 1);
			return NULL;
		}
		
		memcpy(bucket->buf.str.val, buf, buflen);
		bucket->buf.str.len = buflen;
		bucket->own_buf = 1;
	} else {
		bucket->buf.str.val = buf;
		bucket->buf.str.len = buflen;
		bucket->own_buf = own_buf;
	}
	bucket->is_unicode = 0;
	bucket->is_persistent = is_persistent;
	bucket->refcount = 1;

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
		bucket->buf.ustr.val = safe_pemalloc(sizeof(UChar), buflen, 0, 1);
		
		if (bucket->buf.ustr.val == NULL) {
			pefree(bucket, 1);
			return NULL;
		}
		
		memcpy(bucket->buf.ustr.val, buf, buflen);
		bucket->buf.ustr.len = buflen;
		bucket->own_buf = 1;
	} else {
		bucket->buf.ustr.val = buf;
		bucket->buf.ustr.len = buflen;
		bucket->own_buf = own_buf;
	}
	bucket->is_unicode = 1;
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

	if (bucket->is_unicode) {
		retval->buf.ustr.val = safe_pemalloc(sizeof(UChar), retval->buf.ustr.len, 0, retval->is_persistent);
		memcpy(retval->buf.ustr.val, bucket->buf.ustr.val, retval->buf.ustr.len * sizeof(UChar));
	} else {
		retval->buf.str.val = pemalloc(retval->buf.str.len, retval->is_persistent);
		memcpy(retval->buf.str.val, bucket->buf.str.val, retval->buf.str.len);
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

	if (in->is_unicode) {
		(*left)->buf.ustr.val = safe_pemalloc(sizeof(UChar), length, 0, in->is_persistent);
		(*left)->buf.ustr.len = length;
		memcpy((*left)->buf.str.val, in->buf.str.val, length * sizeof(UChar));
	
		(*right)->buf.ustr.len = in->buf.ustr.len - length;
		(*right)->buf.ustr.val = pemalloc((*right)->buf.ustr.len, in->is_persistent);
		memcpy((*right)->buf.ustr.val, in->buf.ustr.val + (length * sizeof(UChar)), (*right)->buf.str.len * sizeof(UChar));
	} else {
		(*left)->buf.str.val = pemalloc(length, in->is_persistent);
		(*left)->buf.str.len = length;
		memcpy((*left)->buf.str.val, in->buf.str.val, length);
	
		(*right)->buf.str.len = in->buf.str.len - length;
		(*right)->buf.str.val = pemalloc((*right)->buf.str.len, in->is_persistent);
		memcpy((*right)->buf.str.val, in->buf.str.val + length, (*right)->buf.str.len);
	}

	(*left)->refcount = 1;
	(*left)->own_buf = 1;
	(*left)->is_persistent = in->is_persistent;
	(*left)->is_unicode = in->is_unicode;

	(*right)->refcount = 1;
	(*right)->own_buf = 1;
	(*right)->is_persistent = in->is_persistent;
	(*right)->is_unicode = in->is_unicode;
	
	return SUCCESS;
	
exit_fail:
	if (*right) {
		if ((*right)->is_unicode) {
			if ((*right)->buf.ustr.val) {
				pefree((*right)->buf.ustr.val, in->is_persistent);
			}
		} else {
			if ((*right)->buf.str.val) {
				pefree((*right)->buf.str.val, in->is_persistent);
			}
		}
		pefree(*right, in->is_persistent);
	}
	if (*left) {
		if ((*left)->is_unicode) {
			if ((*left)->buf.ustr.val) {
				pefree((*left)->buf.ustr.val, in->is_persistent);
			}
		} else {
			if ((*left)->buf.str.val) {
				pefree((*left)->buf.str.val, in->is_persistent);
			}
		}
		pefree(*left, in->is_persistent);
	}
	return FAILURE;
}

PHPAPI void php_stream_bucket_delref(php_stream_bucket *bucket TSRMLS_DC)
{
	if (--bucket->refcount == 0) {
		if (bucket->own_buf) {
			pefree(bucket->is_unicode ? bucket->buf.ustr.val : bucket->buf.str.val, bucket->is_persistent);
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
	HashTable *filter_hash = (FG(stream_filters) ? FG(stream_filters) : &stream_filters_hash);
	php_stream_filter_factory *factory;
	php_stream_filter *filter = NULL;
	int n;
	char *period;

	n = strlen(filtername);
	
	if (SUCCESS == zend_hash_find(filter_hash, (char*)filtername, n, (void**)&factory)) {
		filter = factory->create_filter(filtername, filterparams, persistent TSRMLS_CC);
	} else if ((period = strrchr(filtername, '.'))) {
		/* try a wildcard */
		char *wildname;

		wildname = estrdup(filtername);
		period = wildname + (period - filtername);
		while (period && !filter) {
			*period = '\0';
			strcat(wildname, ".*");
			if (SUCCESS == zend_hash_find(filter_hash, wildname, strlen(wildname), (void**)&factory)) {
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

	if (&(stream->readfilters) == chain && (stream->readbuf_avail) > 0) {
		/* Let's going ahead and wind anything in the buffer through this filter */
		php_stream_bucket_brigade brig_out = { NULL, NULL };
		php_stream_bucket_brigade *brig_outp = &brig_out;
		php_stream_filter_status_t status;
		php_stream_bucket *bucket;

		if (stream->readbuf_ofs) {
			/* Mask readbuf_ofs from filter */
			bucket = stream->readbuf.head;
			if (bucket->is_unicode) {
				bucket->buf.ustr.len -= stream->readbuf_ofs;
				memmove(bucket->buf.ustr.val, bucket->buf.ustr.val + (stream->readbuf_ofs * sizeof(UChar)), bucket->buf.ustr.len * sizeof(UChar));
			} else {
				bucket->buf.str.len -= stream->readbuf_ofs;
				memmove(bucket->buf.str.val, bucket->buf.str.val + stream->readbuf_ofs, bucket->buf.str.len);
			}
		}

		status = filter->fops->filter(stream, filter, &stream->readbuf, brig_outp, NULL, PSFS_FLAG_NORMAL TSRMLS_CC);

		switch (status) {
			case PSFS_ERR_FATAL:
				/* filter is fundamentally broken, invalidate readbuf and strip the filter */
				if (chain->head == filter) {
					chain->head = NULL;
					chain->tail = NULL;
				} else {
					filter->prev->next = NULL;
					chain->tail = filter->prev;
				}
				php_stream_flush_readbuf(stream);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter failed to process pre-buffered data");
				/* Passthru -- Anything successfully filtered can go back on the readbuf */
			case PSFS_PASS_ON:
				stream->readbuf_ofs = stream->readbuf_avail = 0;

				/* Merge brig_out */
				while((bucket = brig_out.head)) {
					php_stream_bucket_unlink(bucket TSRMLS_CC);
					php_stream_bucket_append(&stream->readbuf, bucket TSRMLS_CC);
				}

				/* Count available bytes */
				for(bucket = stream->readbuf.head; bucket; bucket = bucket->next) {
					stream->readbuf_avail += bucket->is_unicode ? bucket->buf.ustr.len : bucket->buf.str.len;
				}
				break;
			case PSFS_FEED_ME:
				/* We don't actually need data yet,
				   leave this filter in a feed me state until data is needed. 
				   Reset stream's internal read buffer since the filter is "holding" it. */
				stream->readbuf.head = stream->readbuf.tail = NULL;
				stream->readbuf_avail = stream->readbuf_ofs = 0;
				break;
		}
	}
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
	int preferred = (chain = &chain->stream->readfilters ? 1 : 0);

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
		flushed_size += bucket->is_unicode ? bucket->buf.ustr.len : bucket->buf.str.len;
	}

	if (flushed_size == 0) {
		/* Unlikely, but possible */
		return SUCCESS;
	}

	if (chain == &(stream->readfilters)) {
		if (stream->readbuf.head) {
			/* Merge inp with readbuf */
			for(bucket = inp->head; bucket; bucket = bucket->next) {
				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_append(&stream->readbuf, bucket TSRMLS_CC);
				stream->readbuf_avail += bucket->is_unicode ? bucket->buf.ustr.len : bucket->buf.str.len;
			}
		} else {
			/* Just plop it in */
			stream->readbuf = *inp;
			stream->readbuf_avail = flushed_size;
			stream->readbuf_ofs = 0;
		}
	} else if (chain == &(stream->writefilters)) {
		/* Send flushed data to the stream */
		while ((bucket = inp->head)) {
			char *data;
			int datalen;

			if (bucket->is_unicode) {
				data = bucket->buf.ustr.val;
				datalen = bucket->buf.ustr.len * sizeof(UChar);
			} else {
				data = bucket->buf.str.val;
				datalen = bucket->buf.str.len;
			}
			stream->ops->write(stream, data, datalen TSRMLS_CC);
			php_stream_bucket_unlink(bucket TSRMLS_CC);
			php_stream_bucket_delref(bucket TSRMLS_CC);
		}
	}

	return SUCCESS;
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

	if (filter->rsrc_id > 0) {
		zend_list_delete(filter->rsrc_id);
	}

	if (call_dtor) {
		php_stream_filter_free(filter TSRMLS_CC);
		return NULL;
	}
	return filter;
}

PHPAPI int php_stream_bucket_tounicode(php_stream *stream, php_stream_bucket **pbucket, off_t *offset TSRMLS_DC)
{
	int is_persistent = php_stream_is_persistent(stream);
	php_stream_bucket *bucket = *pbucket, *prior = bucket->prev, *next = bucket->next;
	php_stream_bucket_brigade *brigade = bucket->brigade;
	UErrorCode status = U_ZERO_ERROR;
	UChar *val;
	int32_t len;

	if (bucket->is_unicode) {
		return SUCCESS;
	}

	zend_convert_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &val, &len, bucket->buf.str.val, bucket->buf.str.len, &status);

	if (U_FAILURE(status)) {
		efree(val);
		return FAILURE;
	}

	php_stream_bucket_unlink(bucket TSRMLS_CC);
	php_stream_bucket_delref(bucket TSRMLS_CC);

	/* convert from unicode defaults to non-persistent */
	bucket = php_stream_bucket_new_unicode(stream, val, len, 1, is_persistent TSRMLS_CC);	
	if (is_persistent) {
		efree(val);
	}

	bucket->brigade = brigade;
	bucket->prev = prior;
	bucket->next = next;

	if (prior) {
		prior->next = bucket;
	} else if (brigade) {
		brigade->head = bucket;
	}

	if (next) {
		next->prev = bucket;
	} else if (brigade) {
		brigade->tail = bucket;
	}

	*pbucket = bucket;

	return SUCCESS;
}

PHPAPI int php_stream_bucket_tostring(php_stream *stream, php_stream_bucket **pbucket, off_t *offset TSRMLS_DC)
{
	int is_persistent = php_stream_is_persistent(stream);
	php_stream_bucket *bucket = *pbucket, *prior = bucket->prev, *next = bucket->next;
	php_stream_bucket_brigade *brigade = bucket->brigade;
	UErrorCode status = U_ZERO_ERROR;
	char *val;
	int len;

	if (!bucket->is_unicode) {
		return SUCCESS;
	}

	zend_convert_from_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &val, &len, bucket->buf.ustr.val, bucket->buf.ustr.len, &status);

	if (U_FAILURE(status)) {
		efree(val);
		return FAILURE;
	}

	php_stream_bucket_unlink(bucket TSRMLS_CC);
	php_stream_bucket_delref(bucket TSRMLS_CC);

	/* convert from unicode defaults to non-persistent */
	bucket = php_stream_bucket_new(stream, val, len, 1, is_persistent TSRMLS_CC);	
	if (is_persistent) {
		efree(val);
	}

	bucket->brigade = brigade;
	bucket->prev = prior;
	bucket->next = next;

	if (prior) {
		prior->next = bucket;
	} else if (brigade) {
		brigade->head = bucket;
	}

	if (next) {
		next->prev = bucket;
	} else if (brigade) {
		brigade->tail = bucket;
	}

	*pbucket = bucket;

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
