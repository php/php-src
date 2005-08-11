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
   | Borrowed code from:                                                  |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define _GNU_SOURCE
#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include "ext/standard/basic_functions.h" /* for BG(mmap_file) (not strictly required) */
#include "ext/standard/php_string.h" /* for php_memnstr, used by php_stream_get_record() */
#include <stddef.h>
#include <fcntl.h>
#include "php_streams_int.h"

/* {{{ resource and registration code */
/* Global wrapper hash, copied to FG(stream_wrappers) on registration of volatile wrapper */
static HashTable url_stream_wrappers_hash;
static int le_stream = FAILURE; /* true global */
static int le_pstream = FAILURE; /* true global */
static int le_stream_filter = FAILURE; /* true global */

PHPAPI int php_file_le_stream(void)
{
	return le_stream;
}

PHPAPI int php_file_le_pstream(void)
{
	return le_pstream;
}

PHPAPI int php_file_le_stream_filter(void)
{
	return le_stream_filter;
}

PHPAPI HashTable *_php_stream_get_url_stream_wrappers_hash(TSRMLS_D)
{
	return (FG(stream_wrappers) ? FG(stream_wrappers) : &url_stream_wrappers_hash);
}

PHPAPI HashTable *php_stream_get_url_stream_wrappers_hash_global(void)
{
	return &url_stream_wrappers_hash;
}

static int _php_stream_release_context(list_entry *le, void *pContext TSRMLS_DC)
{
	if (le->ptr == pContext) {
		return --le->refcount == 0;
	}
	return 0;
}

static int forget_persistent_resource_id_numbers(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_stream *stream;

	if (Z_TYPE_P(rsrc) != le_pstream) {
		return 0;
	}

	stream = (php_stream*)rsrc->ptr;

#if STREAM_DEBUG
fprintf(stderr, "forget_persistent: %s:%p\n", stream->ops->label, stream);
#endif

	stream->rsrc_id = FAILURE;

	if (stream->context) {
		zend_hash_apply_with_argument(&EG(regular_list),
				(apply_func_arg_t) _php_stream_release_context,
				stream->context TSRMLS_CC);
		stream->context = NULL;
	}
	
	return 0;
}

PHP_RSHUTDOWN_FUNCTION(streams)
{
	zend_hash_apply(&EG(persistent_list), (apply_func_t)forget_persistent_resource_id_numbers TSRMLS_CC);
	return SUCCESS;
}

PHPAPI int php_stream_from_persistent_id(const char *persistent_id, php_stream **stream TSRMLS_DC)
{
	zend_rsrc_list_entry *le;

	if (zend_hash_find(&EG(persistent_list), (char*)persistent_id, strlen(persistent_id)+1, (void*) &le) == SUCCESS) {
		if (Z_TYPE_P(le) == le_pstream) {
			if (stream) {
				*stream = (php_stream*)le->ptr;
				le->refcount++;
				(*stream)->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, *stream, le_pstream);
			}
			return PHP_STREAM_PERSISTENT_SUCCESS;
		}
		return PHP_STREAM_PERSISTENT_FAILURE;
	}
	return PHP_STREAM_PERSISTENT_NOT_EXIST;
}

/* }}} */

/* {{{ wrapper error reporting */
void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper, const char *path, const char *caption TSRMLS_DC)
{
	char *tmp = estrdup(path);
	char *msg;
	int free_msg = 0;

	if (wrapper) {
		if (wrapper->err_count > 0) {
			int i;
			size_t l;
			int brlen;
			char *br;

			if (PG(html_errors)) {
				brlen = 7;
				br = "<br />\n";
			} else {
				brlen = 1;
				br = "\n";
			}

			for (i = 0, l = 0; i < wrapper->err_count; i++) {
				l += strlen(wrapper->err_stack[i]);
				if (i < wrapper->err_count - 1) {
					l += brlen;
				}
			}
			msg = emalloc(l + 1);
			msg[0] = '\0';
			for (i = 0; i < wrapper->err_count; i++) {
				strcat(msg, wrapper->err_stack[i]);
				if (i < wrapper->err_count - 1) { 
					strcat(msg, br);
				}
			}

			free_msg = 1;
		} else {
			msg = strerror(errno);
		}
	} else {
		msg = "no suitable wrapper could be found";
	}

	php_strip_url_passwd(tmp);
	php_error_docref1(NULL TSRMLS_CC, tmp, E_WARNING, "%s: %s", caption, msg);
	efree(tmp);
	if (free_msg) {
		efree(msg);
	}
}

void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper TSRMLS_DC)
{
	if (wrapper) {
		/* tidy up the error stack */
		int i;

		for (i = 0; i < wrapper->err_count; i++) {
			efree(wrapper->err_stack[i]);
		}
		if (wrapper->err_stack) {
			efree(wrapper->err_stack);
		}
		wrapper->err_stack = NULL;
		wrapper->err_count = 0;
	}
}

PHPAPI void php_stream_wrapper_log_error(php_stream_wrapper *wrapper, int options TSRMLS_DC, const char *fmt, ...)
{
	va_list args;
	char *buffer = NULL;

	va_start(args, fmt);
	vspprintf(&buffer, 0, fmt, args);
	va_end(args);

	if (options & REPORT_ERRORS || wrapper == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", buffer);
		efree(buffer);
	} else {
		/* append to stack */
		wrapper->err_stack = erealloc(wrapper->err_stack, (wrapper->err_count + 1) * sizeof(char *));
		if (wrapper->err_stack) {
			wrapper->err_stack[wrapper->err_count++] = buffer;
		}
	}
}


/* }}} */

/* allocate a new stream for a particular ops */
PHPAPI php_stream *_php_stream_alloc(php_stream_ops *ops, void *abstract, const char *persistent_id, const char *mode STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_stream *ret;

	ret = (php_stream*) pemalloc_rel_orig(sizeof(php_stream), persistent_id ? 1 : 0);

	memset(ret, 0, sizeof(php_stream));

	ret->readfilters.stream = ret;
	ret->writefilters.stream = ret;

#if STREAM_DEBUG
fprintf(stderr, "stream_alloc: %s:%p persistent=%s\n", ops->label, ret, persistent_id);
#endif

	ret->ops = ops;
	ret->abstract = abstract;
	ret->is_persistent = persistent_id ? 1 : 0;
	ret->chunk_size = FG(def_chunk_size);

	if (FG(auto_detect_line_endings)) {
		ret->flags |= PHP_STREAM_FLAG_DETECT_EOL;
	}

	if (persistent_id) {
		zend_rsrc_list_entry le;

		Z_TYPE(le) = le_pstream;
		le.ptr = ret;
		le.refcount = 0;

		if (FAILURE == zend_hash_update(&EG(persistent_list), (char *)persistent_id,
					strlen(persistent_id) + 1,
					(void *)&le, sizeof(le), NULL)) {
			
			pefree(ret, 1);
			return NULL;
		}
	}

	ret->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, ret, persistent_id ? le_pstream : le_stream);
	strlcpy(ret->mode, mode, sizeof(ret->mode));

	return ret;
}
/* }}} */

static int _php_stream_free_persistent(list_entry *le, void *pStream TSRMLS_DC)
{
	return le->ptr == pStream;
}

PHPAPI int _php_stream_free(php_stream *stream, int close_options TSRMLS_DC) /* {{{ */
{
	int ret = 1;
	int remove_rsrc = 1;
	int preserve_handle = close_options & PHP_STREAM_FREE_PRESERVE_HANDLE ? 1 : 0;
	int release_cast = 1;

#if STREAM_DEBUG
fprintf(stderr, "stream_free: %s:%p[%s] in_free=%d opts=%08x\n", stream->ops->label, stream, stream->orig_path, stream->in_free, close_options);
#endif

	/* recursion protection */
	if (stream->in_free) {
		return 1;
	}

	stream->in_free++;

	/* if we are releasing the stream only (and preserving the underlying handle),
	 * we need to do things a little differently.
	 * We are only ever called like this when the stream is cast to a FILE*
	 * for include (or other similar) purposes.
	 * */
	if (preserve_handle) {
		if (stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FOPENCOOKIE) {
			/* If the stream was fopencookied, we must NOT touch anything
			 * here, as the cookied stream relies on it all.
			 * Instead, mark the stream as OK to auto-clean */
			php_stream_auto_cleanup(stream);
			stream->in_free--;
			return 0;
		}
		/* otherwise, make sure that we don't close the FILE* from a cast */
		release_cast = 0;
	}

#if STREAM_DEBUG
fprintf(stderr, "stream_free: %s:%p[%s] preserve_handle=%d release_cast=%d remove_rsrc=%d\n",
		stream->ops->label, stream, stream->orig_path, preserve_handle, release_cast, remove_rsrc);
#endif

	/* make sure everything is saved */
	_php_stream_flush(stream, 1 TSRMLS_CC);
		
	/* If not called from the resource dtor, remove the stream from the resource list. */
	if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) == 0 && remove_rsrc) {
		zend_list_delete(stream->rsrc_id);
	}

	/* Remove stream from any context link list */
	if (stream->context && stream->context->links) {
		php_stream_context_del_link(stream->context, stream);
	}

	if (close_options & PHP_STREAM_FREE_CALL_DTOR) {
		if (release_cast && stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FOPENCOOKIE) {
			/* calling fclose on an fopencookied stream will ultimately
				call this very same function.  If we were called via fclose,
				the cookie_closer unsets the fclose_stdiocast flags, so
				we can be sure that we only reach here when PHP code calls
				php_stream_free.
				Lets let the cookie code clean it all up.
			 */
			stream->in_free = 0;
			return fclose(stream->stdiocast);
		}

		ret = stream->ops->close(stream, preserve_handle ? 0 : 1 TSRMLS_CC);
		stream->abstract = NULL;

		/* tidy up any FILE* that might have been fdopened */
		if (release_cast && stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FDOPEN && stream->stdiocast) {
			fclose(stream->stdiocast);
			stream->stdiocast = NULL;
			stream->fclose_stdiocast = PHP_STREAM_FCLOSE_NONE;
		}
	}

	if (close_options & PHP_STREAM_FREE_RELEASE_STREAM) {
		while (stream->readfilters.head) {
			php_stream_filter_remove(stream->readfilters.head, 1 TSRMLS_CC);
		}
		while (stream->writefilters.head) {
			php_stream_filter_remove(stream->writefilters.head, 1 TSRMLS_CC);
		}

		if (stream->wrapper && stream->wrapper->wops && stream->wrapper->wops->stream_closer) {
			stream->wrapper->wops->stream_closer(stream->wrapper, stream TSRMLS_CC);
			stream->wrapper = NULL;
		}

		if (stream->wrapperdata) {
			zval_ptr_dtor(&stream->wrapperdata);
			stream->wrapperdata = NULL;
		}

		while (stream->readbuf.head) {
			php_stream_bucket *bucket = stream->readbuf.head;

			php_stream_bucket_unlink(bucket TSRMLS_CC);
			php_stream_bucket_delref(bucket TSRMLS_CC);
		}

		if (stream->is_persistent && (close_options & PHP_STREAM_FREE_PERSISTENT)) {
			/* we don't work with *stream but need its value for comparison */
			zend_hash_apply_with_argument(&EG(persistent_list), (apply_func_arg_t) _php_stream_free_persistent, stream TSRMLS_CC);
		}
#if ZEND_DEBUG
		if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) && (stream->__exposed == 0) && (EG(error_reporting) & E_WARNING)) {
			/* it leaked: Lets deliberately NOT pefree it so that the memory manager shows it
			 * as leaked; it will log a warning, but lets help it out and display what kind
			 * of stream it was. */
			char *leakinfo;
			spprintf(&leakinfo, 0, __FILE__ "(%d) : Stream of type '%s' %p (path:%s) was not closed\n", __LINE__, stream->ops->label, stream, stream->orig_path);

			if (stream->orig_path) {
				pefree(stream->orig_path, stream->is_persistent);
				stream->orig_path = NULL;
			}
			
# if defined(PHP_WIN32)
			OutputDebugString(leakinfo);
# else
			fprintf(stderr, "%s", leakinfo);
# endif
			efree(leakinfo);
		} else {
			if (stream->orig_path) {
				pefree(stream->orig_path, stream->is_persistent);
				stream->orig_path = NULL;
			}

			pefree(stream, stream->is_persistent);
		}
#else
		if (stream->orig_path) {
			pefree(stream->orig_path, stream->is_persistent);
			stream->orig_path = NULL;
		}

		pefree(stream, stream->is_persistent);
#endif
	}

	return ret;
}
/* }}} */

/* {{{ generic stream operations */

static void php_stream_fill_read_buffer(php_stream *stream, size_t size TSRMLS_DC)
{
	if (stream->readfilters.head) {
		char *chunk_buf;
		int err_flag = 0;
		php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
		php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out, *brig_swap;

		/* allocate a buffer for reading chunks */
		chunk_buf = emalloc(stream->chunk_size);

		while (!err_flag && (stream->readbuf_avail < (off_t)size)) {
			size_t justread = 0;
			int flags;
			php_stream_bucket *bucket;
			php_stream_filter_status_t status = PSFS_ERR_FATAL;
			php_stream_filter *filter;

			/* read a chunk into a bucket */
			justread = stream->ops->read(stream, chunk_buf, stream->chunk_size TSRMLS_CC);
			if (justread > 0) {
				bucket = php_stream_bucket_new(stream, chunk_buf, justread, 0, 0 TSRMLS_CC);

				/* after this call, bucket is owned by the brigade */
				php_stream_bucket_append(brig_inp, bucket TSRMLS_CC);

				flags = PSFS_FLAG_NORMAL;
			} else {
				flags = stream->eof ? PSFS_FLAG_FLUSH_CLOSE : PSFS_FLAG_FLUSH_INC;
			}
		
			/* wind the handle... */
			for (filter = stream->readfilters.head; filter; filter = filter->next) {
				status = filter->fops->filter(stream, filter, brig_inp, brig_outp, NULL, flags TSRMLS_CC);

				if (status != PSFS_PASS_ON) {
					break;
				}
				
				/* brig_out becomes brig_in.
				 * brig_in will always be empty here, as the filter MUST attach any un-consumed buckets
				 * to its own brigade */
				brig_swap = brig_inp;
				brig_inp = brig_outp;
				brig_outp = brig_swap;
				memset(brig_outp, 0, sizeof(*brig_outp));
			}
			
			switch (status) {
				case PSFS_PASS_ON:
					/* we get here when the last filter in the chain has data to pass on.
					 * in this situation, we are passing the brig_in brigade into the
					 * stream read buffer */
					while ((bucket = brig_inp->head)) {
						php_stream_bucket *tail = stream->readbuf.tail;
						php_stream_bucket_unlink(bucket TSRMLS_CC);
						if (bucket->is_unicode &&
							U16_IS_SURROGATE(*bucket->buf.ustr.val) &&
							!U16_IS_SURROGATE_LEAD(*bucket->buf.ustr.val) &&
							tail && tail->is_unicode &&
							U16_IS_SURROGATE(tail->buf.ustr.val[tail->buf.ustr.len - 1]) &&
							U16_IS_SURROGATE_LEAD(tail->buf.ustr.val[tail->buf.ustr.len - 1])) {
							/* Surrogate pair got split between buckets -- Unlikely */
							UChar *tmp;

							tmp = peumalloc(bucket->buf.ustr.len + 1, bucket->is_persistent);
							*tmp = stream->readbuf.tail->buf.ustr.val[--tail->buf.ustr.len];
							memmove(tmp + UBYTES(1), bucket->buf.ustr.val, UBYTES(bucket->buf.ustr.len));
							pefree(bucket->buf.ustr.val, bucket->is_persistent);
							bucket->buf.ustr.val = tmp;

							if (tail->buf.ustr.len <= 0) {
								/* Tail was only a one UChar bucket */
								php_stream_bucket_unlink(tail TSRMLS_CC);
								php_stream_bucket_delref(tail TSRMLS_CC);
							} else if (tail == stream->readbuf.head && (tail->buf.ustr.len <= stream->readbuf_ofs)) {
								/* Tail was head and last char was only unused portion */
								php_stream_bucket_unlink(tail TSRMLS_CC);
								php_stream_bucket_delref(tail TSRMLS_CC);
								stream->readbuf_ofs = 0;
							}
						}
						php_stream_bucket_append(&stream->readbuf, bucket TSRMLS_CC);
						stream->readbuf_avail += bucket->is_unicode ? bucket->buf.ustr.len : bucket->buf.str.len;
					}
					break;

				case PSFS_FEED_ME:
					/* when a filter needs feeding, there is no brig_out to deal with.
					 * we simply continue the loop; if the caller needs more data,
					 * we will read again, otherwise out job is done here */
					if (justread == 0) {
						/* there is no data */
						err_flag = 1;
						break;
					}
					continue;

				case PSFS_ERR_FATAL:
					/* some fatal error. Theoretically, the stream is borked, so all
					 * further reads should fail. */
					err_flag = 1;
					break;
			}

			if (justread == 0) {
				break;
			}
		}

		efree(chunk_buf);

	} else {
		/* is there enough data in the buffer ? */
		if (stream->readbuf_avail < (off_t)size) {
			char *chunk_buf;
			size_t justread = 0;
			int is_persistent = php_stream_is_persistent(stream);

			chunk_buf = pemalloc(stream->chunk_size, is_persistent);
			justread = stream->ops->read(stream, chunk_buf, stream->chunk_size TSRMLS_CC);

			if (justread == (size_t)-1 || justread == 0) {
				pefree(chunk_buf, is_persistent);
			} else {
				php_stream_bucket *bucket;

				bucket = php_stream_bucket_new(stream, chunk_buf, justread, 1, is_persistent TSRMLS_CC);
				php_stream_bucket_append(&stream->readbuf, bucket TSRMLS_CC);
				stream->readbuf_avail += justread;
			}
		}
	}
}

PHPAPI size_t _php_stream_read(php_stream *stream, char *buf, size_t size TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t toread = 0, didread = 0;

	while (size > 0) {
		/* take from the read buffer first.
		 * It is possible that a buffered stream was switched to non-buffered, so we
		 * drain the remainder of the buffer before using the "raw" read mode for
		 * the excess */

		while (size > 0 && (bucket = stream->readbuf.head)) {
			if (bucket->is_unicode) {
				/* This is an string read func, convert to string first */
				php_stream_bucket_tostring(stream, &bucket, &stream->readbuf_ofs TSRMLS_CC);
			}
			toread = bucket->buf.str.len - stream->readbuf_ofs;
			if (toread > size) {
				toread = size;
			}
			memcpy(buf, bucket->buf.str.val + stream->readbuf_ofs, toread);
			stream->readbuf_ofs += toread;
			stream->readbuf_avail -= toread;
			if (stream->readbuf_ofs >= bucket->buf.str.len) {
				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
				stream->readbuf_ofs = 0;
			}
			size -= toread;
			buf += toread;
			didread += toread;
		}

		/* ignore eof here; the underlying state might have changed */
		if (size == 0) {
			break;
		}

		/* just break anyway, to avoid greedy read */
		if (didread > 0 && (stream->wrapper != &php_plain_files_wrapper)) {
			break;
		}

		if (!stream->readfilters.head && (stream->flags & PHP_STREAM_FLAG_NO_BUFFER || stream->chunk_size == 1)) {
			toread = stream->ops->read(stream, buf, size TSRMLS_CC);
			if (toread <= 0) {
				break;
			}
			buf += toread;
			size -= toread;
			didread += toread;
			continue;
		}

		php_stream_fill_read_buffer(stream, size TSRMLS_CC);
		if (stream->readbuf_avail <= 0) {
			/* EOF, or temporary end of data (for non-blocking mode). */
			break;
		}
	}

	if (didread > 0) {
		stream->position += didread;
	}
	return didread;
}

PHPAPI size_t _php_stream_read_unicode(php_stream *stream, UChar *buf, int32_t size TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t toread = 0, didread = 0;

	while (size > 0) {
		/* take from the read buffer first.
		 * It is possible that a buffered stream was switched to non-buffered, so we
		 * drain the remainder of the buffer before using the "raw" read mode for
		 * the excess */

		while (size > 0 && (bucket = stream->readbuf.head)) {
			UChar lastchar = 0;

			if (!bucket->is_unicode) {
				/* This is a unicode read func, convert to unicode first */
				php_stream_bucket_tounicode(stream, &bucket, &stream->readbuf_ofs TSRMLS_CC);
			}
			toread = bucket->buf.ustr.len - stream->readbuf_ofs;
			if (toread > size) {
				toread = size;
			}
			lastchar = *(bucket->buf.ustr.val + stream->readbuf_ofs + toread - 1);
			if (U16_IS_SURROGATE(lastchar) && U16_IS_SURROGATE_LEAD(lastchar)) {
				toread--;
				/* The only time we should encounter a split surrogate is when the buffer size is truncating the data
					In this case, reduce size along with toread to avoid getting stuck */
				size--;
			}
			memcpy(buf, bucket->buf.ustr.val + stream->readbuf_ofs, toread * sizeof(UChar));
			stream->readbuf_ofs += toread;
			stream->readbuf_avail -= toread;
			if (stream->readbuf_ofs >= bucket->buf.ustr.len) {
				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
				stream->readbuf_ofs = 0;
			}
			size -= toread;
			buf += toread;
			didread += toread;
		}

		/* ignore eof here; the underlying state might have changed */
		if (size == 0) {
			break;
		}

		/* just break anyway, to avoid greedy read */
		if (didread > 0 && (stream->wrapper != &php_plain_files_wrapper)) {
			break;
		}

		php_stream_fill_read_buffer(stream, size * sizeof(UChar) TSRMLS_CC);
		if (stream->readbuf_avail <= 0) {
			/* EOF, or temporary end of data (for non-blocking mode). */
			break;
		}
	}

	if (didread > 0) {
		stream->position += didread;
	}

	return didread;
}

/*	buf mabe NULL (in which case it will be allocated)
	num_bytes and num_chars must be initialized upon entry to maximum for each (-1 for no maximum)
	num_bytes/num_chars will be set on exit to actual contents of buf
	Will return unicode/string type dependent on the first character unit in the read buf
	Will return as many characters as possible (and permitted by max lengths) without changing unicode/string type
	Will not split surrogate pairs */
PHPAPI void *_php_stream_u_read(php_stream *stream, void *buf, int32_t *pnum_bytes, int32_t *pnum_chars, int *pis_unicode TSRMLS_DC)
{
	int grow_mode = 0;
	int32_t num_bytes = 0, num_chars = 0;
	int32_t max_bytes = *pnum_bytes, max_chars = *pnum_chars;
	int32_t buflen = buf ? max_bytes : 2048;
	int32_t bufpos = 0;
	int is_unicode;
	php_stream_bucket *bucket;

	/* It's possible that we have a readbuf, but that it's only half of a surrogate pair */
	if (!stream->readbuf.head ||
		(stream->readbuf.head == stream->readbuf.tail && stream->readbuf.head->is_unicode && 
		(stream->readbuf.head->buf.ustr.len - stream->readbuf_ofs) == 1 &&
		U16_IS_SURROGATE(stream->readbuf.head->buf.ustr.val[stream->readbuf.head->buf.ustr.len-1]))) {
		php_stream_fill_read_buffer(stream, max_bytes ? max_bytes : (max_chars ? max_chars : stream->chunk_size) TSRMLS_CC);
	}


	if (!stream->readbuf.head ||
		(stream->readbuf.head == stream->readbuf.tail && stream->readbuf.head->is_unicode && 
		(stream->readbuf.head->buf.ustr.len - stream->readbuf_ofs) == 1 &&
		U16_IS_SURROGATE(stream->readbuf.head->buf.ustr.val[stream->readbuf.head->buf.ustr.len-1]))) {
		/* Nothing to return */
		*pnum_bytes = 0;
		*pnum_chars = 0;
		*pis_unicode = 0;
		return NULL;
	}


	if (!buf) {
		grow_mode = 1;
		buf = emalloc(buflen);
	}

	is_unicode = stream->readbuf.head->is_unicode;
	if (is_unicode) {
		/* normalize byte boundary */
		if (max_bytes >= 0 && (max_bytes % sizeof(UChar))) {
			max_bytes -= (max_bytes % sizeof(UChar));
		}
		if (max_bytes >= 0 && max_bytes < UBYTES(max_chars)) {
			/* max_bytes needs to be at least twice max_chars when both are provided */
			max_chars = (max_bytes / sizeof(UChar));
		}
	} else {
		if (max_chars < 0 && max_bytes >= 0) {
			max_chars = max_bytes;
		} else if (max_chars >= 0 && grow_mode) {
			max_bytes = max_chars;
		}
	}

	for (;;) {
		if (buflen - bufpos < 1024 && max_bytes >= 0 && max_bytes > buflen) {
			buflen += 1024;
			if (buflen > max_bytes) {
				buflen = max_bytes;
			}
			buf = erealloc(buf, buflen);
		}

		if ((bucket = stream->readbuf.head)) {
			if ((bucket->is_unicode && !is_unicode) ||
				(!bucket->is_unicode && is_unicode)) {
				/* data type swap, exit now */
				break;
			}
			if (bucket->is_unicode) {
				UChar *s = bucket->buf.ustr.val + stream->readbuf_ofs, *p;
				int bytes_in_buf, chars_in_buf;
				int32_t ofs = 0;

				chars_in_buf = u_countChar32(s, bucket->buf.ustr.len - stream->readbuf_ofs);

				if (chars_in_buf > max_chars && max_chars >= 0) {
					chars_in_buf = max_chars;
				}
				/* u_countChar32 tells us that we won't overrun anyway */
				U16_FWD_N_UNSAFE(s, ofs, chars_in_buf);
				p = s + ofs;
				bytes_in_buf = UBYTES(ofs);
				if (bytes_in_buf > (max_bytes - num_bytes)) {
					bytes_in_buf = max_bytes - num_bytes;
					bytes_in_buf -= bytes_in_buf & 1; /* normalize */
					p = s + (bytes_in_buf >> 1);
					if (p > s && U16_IS_SURROGATE(p[-1]) && U16_IS_SURROGATE_LEAD(p[-1])) {
						/* Don't split surrogate pairs */
						p--;
						bytes_in_buf -= UBYTES(1);
					}
					if (bytes_in_buf <= 0) {
						/* No room to copy data (surrogate pair) */
						break;
					}
					chars_in_buf = u_countChar32(s, p - s);
				}
				memcpy((char *)buf + num_bytes, s, bytes_in_buf);
				num_bytes += bytes_in_buf;
				num_chars += chars_in_buf;
				stream->readbuf_ofs += p - s;
				stream->readbuf_avail -= p - s;
				if (stream->readbuf_ofs >= bucket->buf.ustr.len) {
					php_stream_bucket_unlink(bucket TSRMLS_CC);
					php_stream_bucket_delref(bucket TSRMLS_CC);
					stream->readbuf_ofs = 0;
				} else if (stream->readbuf_ofs == (bucket->buf.ustr.len - 1) && 
					U16_IS_SURROGATE(bucket->buf.ustr.val[bucket->buf.ustr.len - 1]) &&
					bucket->next && bucket->next->is_unicode) {
					/* Only one char left in the bucket, avoid already split surrogates getting "stuck" -- Should never happen thanks to fill_read_buffer */
					php_stream_bucket *next_bucket = bucket->next;

					bucket->buf.ustr.val = peurealloc(bucket->buf.ustr.val, next_bucket->buf.ustr.len + 1, bucket->is_persistent);
					bucket->buf.ustr.val[0] = bucket->buf.ustr.val[bucket->buf.ustr.len - 1];
					memcpy(bucket->buf.ustr.val + 1, next_bucket->buf.ustr.val, UBYTES(next_bucket->buf.ustr.len));
					php_stream_bucket_unlink(next_bucket TSRMLS_CC);
					php_stream_bucket_delref(next_bucket TSRMLS_CC);
					stream->readbuf_ofs = 0;
				} else {
					/* Reached max limits */
					break;
				}
			} else {
				int want = (max_chars < 0 || max_chars >= buflen) ? (buflen - num_bytes) : (max_chars - num_chars);
				int avail = bucket->buf.str.len - stream->readbuf_ofs;

				if (max_bytes >= 0 && want > max_bytes) {
					want = max_bytes;
				}

				if (want > avail) {
					want = avail;
				}

				memcpy((char *)buf + num_bytes, bucket->buf.str.val + stream->readbuf_ofs, want);
				stream->readbuf_ofs += want;
				stream->readbuf_avail -= want;
				num_bytes += want;
				num_chars += want;
				if (stream->readbuf_ofs >= bucket->buf.str.len) {
					php_stream_bucket_unlink(bucket TSRMLS_CC);
					php_stream_bucket_delref(bucket TSRMLS_CC);
					stream->readbuf_ofs = 0;
				} else {
					/* Reached max limit */
					break;
				}
			}
		} else {
			/* No more data */
			break;
		}
	}
	/* Successful exit */
	*pnum_bytes = num_bytes;
	*pnum_chars = num_chars;
	*pis_unicode = is_unicode;

	if (num_chars == 0 && grow_mode) {
		efree(buf);
		buf = NULL;
	}
	return buf;
}

PHPAPI int _php_stream_eof(php_stream *stream TSRMLS_DC)
{
	/* if there is data in the buffer, it's not EOF */
	if (stream->readbuf_avail > 0) {
		return 0;
	}

	/* use the configured timeout when checking eof */
	if (!stream->eof && PHP_STREAM_OPTION_RETURN_ERR ==
		   	php_stream_set_option(stream, PHP_STREAM_OPTION_CHECK_LIVENESS,
		   	0, NULL)) {
		stream->eof = 1;
	}

	return stream->eof;
}

PHPAPI int _php_stream_putc(php_stream *stream, int c TSRMLS_DC)
{
	unsigned char buf = c;

	if (php_stream_write(stream, &buf, 1) > 0) {
		return 1;
	}
	return EOF;
}

PHPAPI int _php_stream_getc(php_stream *stream TSRMLS_DC)
{
	char buf;

	if (php_stream_read(stream, &buf, 1) > 0) {
		return buf & 0xff;
	}
	return EOF;
}

PHPAPI int _php_stream_puts(php_stream *stream, char *buf TSRMLS_DC)
{
	int len;
	char newline[2] = "\n"; /* is this OK for Win? */
	len = strlen(buf);

	if (len > 0 && php_stream_write(stream, buf, len) && php_stream_write(stream, newline, 1)) {
		return 1;
	}
	return 0;
}

PHPAPI int _php_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	memset(ssb, 0, sizeof(*ssb));

	/* if the stream was wrapped, allow the wrapper to stat it */
	if (stream->wrapper && stream->wrapper->wops->stream_stat != NULL) {
		return stream->wrapper->wops->stream_stat(stream->wrapper, stream, ssb TSRMLS_CC);
	}

	/* if the stream doesn't directly support stat-ing, return with failure.
	 * We could try and emulate this by casting to a FD and fstat-ing it,
	 * but since the fd might not represent the actual underlying content
	 * this would give bogus results. */
	if (stream->ops->stat == NULL) {
		return -1;
	}

	return (stream->ops->stat)(stream, ssb TSRMLS_CC);
}

/* buf != NULL Still used by file() in ext/standard/file.c
   buf == NULL semantics no longer supported */
PHPAPI char *php_stream_locate_eol(php_stream *stream, char *buf, size_t buf_len TSRMLS_DC)
{
	size_t avail;
	char *cr, *lf, *eol = NULL;
	char *readptr;

	if (!buf) {
		return NULL;
	} else {
		readptr = buf;
		avail = buf_len;
	}	

	/* Look for EOL */
	if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL) {
		cr = memchr(readptr, '\r', avail);
		lf = memchr(readptr, '\n', avail);

		if (cr && lf != cr + 1 && !(lf && lf < cr)) {
			/* mac */
			stream->flags ^= PHP_STREAM_FLAG_DETECT_EOL;
			stream->flags |= PHP_STREAM_FLAG_EOL_MAC;
			eol = cr;
		} else if ((cr && lf && cr == lf - 1) || (lf)) {
			/* dos or unix endings */
			stream->flags ^= PHP_STREAM_FLAG_DETECT_EOL;
			eol = lf;
		}
	} else if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
		eol = memchr(readptr, '\r', avail);
	} else {
		/* unix (and dos) line endings */
		eol = memchr(readptr, '\n', avail);
	}

	return eol;
}

/* If buf == NULL, the buffer will be allocated automatically and will be of an
 * appropriate length to hold the line, regardless of the line length, memory
 * permitting -- returned string will be up to (maxlen-1), last byte holding terminating NULL */
PHPAPI char *_php_stream_get_line(php_stream *stream, char *buf, size_t maxlen,
		size_t *returned_len TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t total_copied = 0;
	int growmode = 0;

	if (!buf) {
		maxlen = stream->chunk_size + 1;
		buf = emalloc(maxlen);
		growmode = 1;
	}

	/* Leave room for NULL */
	maxlen--;

	for(;;) {
		/* Fill buf with buffered data
		   until no space is left in the buffer
		   or EOL is found */
		char lastchar = 0;

		/* consumed readbuf if possible */
		while ((bucket = stream->readbuf.head)) {
			char *eol;
			size_t tocopy;
			size_t wanted = maxlen - total_copied;
			int bucket_consumed = 0;

			if (bucket->is_unicode) {
				/* This is a string read func, convert to string first */
				php_stream_bucket_tostring(stream, &bucket, &stream->readbuf_ofs TSRMLS_CC);
			}

			if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL && lastchar == '\r') {
				/* Line ending was actually found in the last char of the last bucket
				   Since it was \r it could have been MAC or DOS */
				stream->flags ^= PHP_STREAM_FLAG_DETECT_EOL;
				if (bucket->buf.str.val[stream->readbuf_ofs] == '\n') {
					/* First byte here is a \n, put them together and you get DOS line endings */
					stream->readbuf_ofs++;
					stream->readbuf_avail--;
					buf[total_copied++] = '\n';
					/* unlikely -- It'd mean a one byte bucket -- possible though */
					if (stream->readbuf_ofs >= bucket->buf.str.len) {
						stream->readbuf_ofs = 0;
						php_stream_bucket_unlink(bucket TSRMLS_CC);
						php_stream_bucket_delref(bucket TSRMLS_CC);
					}
				} else {
					/* Seeing no \n in the first char of this bucket, we know it was MAC */
					stream->flags |= PHP_STREAM_FLAG_EOL_MAC;
				}
				goto exit_getline;
			} else if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL) {
				char *cr, *lf;
				lf = memchr(bucket->buf.str.val + stream->readbuf_ofs, '\n', bucket->buf.str.len - stream->readbuf_ofs);
				cr = memchr(bucket->buf.str.val + stream->readbuf_ofs, '\r', bucket->buf.str.len - stream->readbuf_ofs);
				eol = (cr && (!lf || cr < (lf - 1))) ? cr : lf;
			} else if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
				eol = memchr(bucket->buf.str.val + stream->readbuf_ofs, '\r', bucket->buf.str.len - stream->readbuf_ofs);
			} else {
				eol = memchr(bucket->buf.str.val + stream->readbuf_ofs, '\n', bucket->buf.str.len - stream->readbuf_ofs);
			}

			/* No \r or \n found in bucket -- grab it all */
			if (!eol) {
				eol = bucket->buf.str.val + bucket->buf.str.len - 1;
			}
			tocopy = eol - (bucket->buf.str.val + stream->readbuf_ofs) + 1;

			/* maxlen exceeded */
			if (tocopy > wanted && growmode) {
				if (tocopy - wanted > stream->chunk_size) {
					maxlen += tocopy - wanted;
				} else {
					maxlen += stream->chunk_size;
				}
				buf = erealloc(buf, maxlen + 1);
				wanted = maxlen - total_copied;
			}

			if (tocopy > wanted) {
				tocopy = wanted;
			}

			memcpy(buf + total_copied, bucket->buf.str.val + stream->readbuf_ofs, tocopy);
			total_copied += tocopy;
			stream->readbuf_ofs += tocopy;
			stream->readbuf_avail -= tocopy;
			lastchar = buf[total_copied-1];

			if (stream->readbuf_ofs >= bucket->buf.str.len) {
				stream->readbuf_ofs = 0;
				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
				bucket_consumed = 1;
			}

			if (total_copied >= maxlen) {
				goto exit_getline;
			}

			if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL &&
				bucket_consumed && lastchar == '\r') {
				/* Could be MAC, could be DOS...
				   Need to check the first char of the next bucket to be sure */
				continue;
			}

			if (lastchar == '\r' || lastchar == '\n') {
				stream->flags ^= PHP_STREAM_FLAG_DETECT_EOL;
				if (lastchar == '\r') {
					/* if there were a \n in this bucket after the \r, we would be looking at it */
					stream->flags |= PHP_STREAM_FLAG_EOL_MAC;
				}
				goto exit_getline;
			}
		}

		if (stream->eof) {
			if (total_copied == 0) {
				if (growmode) {
					efree(buf);
				}
				return NULL;
			}
			goto exit_getline;
		}

		if (maxlen - total_copied) {
			size_t bufneeded = maxlen - total_copied;

			if (growmode) {
				bufneeded = stream->chunk_size;
			}
			php_stream_fill_read_buffer(stream, bufneeded TSRMLS_CC);
		}

	}

 exit_getline:

	if (returned_len) {
		*returned_len = total_copied;
	}
	buf[total_copied] = 0;
	stream->position += total_copied;

	return buf;
}

/* If buf == NULL, the buffer will be allocated automatically and will be of an
 * appropriate length to hold the line, regardless of the line length, memory
 * permitting -- returned string will be up to (maxlen-1), last byte holding terminating NULL */
PHPAPI UChar *_php_stream_u_get_line(php_stream *stream, UChar *buf, int32_t *pmax_bytes, int32_t *pmax_chars, int *pis_unicode TSRMLS_DC)
{
	php_stream_bucket *bucket;
	int32_t num_bytes = 0, num_chars = 0;
	int32_t max_bytes = *pmax_bytes, max_chars = *pmax_chars;
	int growmode = 0, is_unicode;

	while (!stream->readbuf.head) {
		/* Nothing buffered, get an idea of the data type by polling */
		int32_t fillsize = (max_chars > 0) ? max_chars : ((max_bytes > 0) ? max_bytes : stream->chunk_size);

		php_stream_fill_read_buffer(stream, fillsize TSRMLS_CC);
		if (!stream->readbuf.head) {
			*pmax_bytes = 0;
			*pmax_chars = 0;
			*pis_unicode = 0;
			return NULL;
		}
	}

	*pis_unicode = is_unicode = stream->readbuf.head->is_unicode;

	if (!is_unicode) {
		/* Wrap normal get_line() */
		int returned_len;
		char *retbuf = php_stream_get_line(stream, (char*)buf, max_chars, &returned_len);

		*pmax_chars = returned_len;
		*pmax_bytes = returned_len;
		return (UChar*)retbuf;
	}

	/* Now act like php_stream_u_read(), but stopping at 000A, 000D, or 000D 000A */

	if (!buf) {
		max_bytes = UBYTES(257);
		buf = emalloc(max_bytes);
		growmode = 1;
	}

	/* Leave room for NULL */
	max_bytes -= UBYTES(1);

	for(;;) {
		/* Fill buf with buffered data
		   until no space is left in the buffer
		   or EOL is found */
		UChar lastchar = 0;

		/* consumed readbuf if possible */
		while ((bucket = stream->readbuf.head)) {
			UChar *eol, *s;
			int32_t want_chars = max_chars - num_chars;
			int32_t want_bytes = max_bytes - num_bytes;
			int32_t count_chars;
			int32_t count_bytes;
			int bucket_consumed = 0;

			if (!bucket->is_unicode) {
				/* Done with unicode data, bail as though EOL was reached (even though it wasn't) */
				goto exit_ugetline;
			}

			if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL && lastchar == '\r') {
				/* Line ending was actually found in the last char of the last bucket
				   Since it was \r it could have been MAC or DOS */
				stream->flags ^= PHP_STREAM_FLAG_DETECT_EOL;
				if (bucket->buf.ustr.val[stream->readbuf_ofs] == '\n') {
					/* First byte here is a \n, put them together and you get DOS line endings */
					stream->readbuf_ofs++;
					stream->readbuf_avail--;
					buf[num_bytes >> 1] = '\n';	/* Can't use num_chars here, surrogate pairs will foul it up */
					num_bytes += UBYTES(1);
					num_chars++;
					/* unlikely -- It'd mean a one UChar bucket -- possible though */
					if (stream->readbuf_ofs >= bucket->buf.ustr.len) {
						stream->readbuf_ofs = 0;
						php_stream_bucket_unlink(bucket TSRMLS_CC);
						php_stream_bucket_delref(bucket TSRMLS_CC);
					}
				} else {
					/* Seeing no \n in the first char of this bucket, we know it was MAC */
					stream->flags |= PHP_STREAM_FLAG_EOL_MAC;
				}
				goto exit_ugetline;
			} else if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL) {
				UChar *cr, *lf;
				lf  = u_memchr(bucket->buf.ustr.val + stream->readbuf_ofs, '\n', bucket->buf.ustr.len - stream->readbuf_ofs);
				cr  = u_memchr(bucket->buf.ustr.val + stream->readbuf_ofs, '\r', bucket->buf.ustr.len - stream->readbuf_ofs);
				eol = (cr && (!lf || cr < (lf - 1))) ? cr : lf;
			} else if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
				eol = u_memchr(bucket->buf.ustr.val + stream->readbuf_ofs, '\r', bucket->buf.ustr.len - stream->readbuf_ofs);
			} else {
				eol = u_memchr(bucket->buf.ustr.val + stream->readbuf_ofs, '\n', bucket->buf.ustr.len - stream->readbuf_ofs);
			}

			/* No \r or \n found in bucket -- grab it all */
			if (!eol) {
				eol = bucket->buf.ustr.val + bucket->buf.ustr.len - 1;
			}
			s = bucket->buf.ustr.val + stream->readbuf_ofs;

			count_bytes = UBYTES(eol - s + 1);
			if (count_bytes > want_bytes && growmode) {
				max_bytes = num_bytes + count_bytes + UBYTES(256);
				want_bytes = max_bytes - num_bytes;
				buf = erealloc(buf, max_bytes + UBYTES(1));
			} else if (count_bytes > want_bytes) {
				count_bytes = want_bytes;
			}
			if (U16_IS_SURROGATE(s[(count_bytes >> 1) - 1]) &&
				U16_IS_SURROGATE_LEAD(s[(count_bytes >> 1) - 1])) {
				count_bytes -= UBYTES(1);
			}
			if (count_bytes <= 0) {
				/* Not enough space in buffer, just break out */
				goto exit_ugetline;
			}
			count_chars = u_countChar32(s, count_bytes >> 1);

			if (max_chars >= 0 && count_chars > want_chars) {
				count_chars = want_chars;
				count_bytes = 0;
				U16_FWD_N_UNSAFE(s, count_bytes, count_chars);
				count_bytes <<= 1; /* translate U16 to bytes */
			}

			memcpy(buf + num_bytes, s, count_bytes);
			num_bytes += count_bytes;
			num_chars += count_chars;
			stream->readbuf_ofs += count_bytes >> 1;
			stream->readbuf_avail -= count_bytes >> 1;

			lastchar = buf[(num_bytes >> 1) - 1];

			if (stream->readbuf_ofs >= bucket->buf.ustr.len) {
				stream->readbuf_ofs = 0;
				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
				bucket_consumed = 1;
			}

			if ((max_bytes >= 0 && num_bytes >= max_bytes) || 
				(max_chars >= 0 && num_chars >= max_chars)) {
				goto exit_ugetline;
			}

			if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL &&
				bucket_consumed && lastchar == '\r') {
				/* Could be MAC, could be DOS...
				   Need to check the first char of the next bucket to be sure */
				continue;
			}

			if (lastchar == '\r' || lastchar == '\n') {
				stream->flags ^= PHP_STREAM_FLAG_DETECT_EOL;
				if (lastchar == '\r') {
					/* if there were a \n in this bucket after the \r, we would be looking at it */
					stream->flags |= PHP_STREAM_FLAG_EOL_MAC;
				}
				goto exit_ugetline;
			}
		}

		if (stream->eof) {
			if (num_bytes == 0) {
				if (growmode) {
					efree(buf);
				}
				buf = NULL;
			}
			goto exit_ugetline;
		}

		if (max_bytes - num_bytes) {
			int32_t want_bytes = max_bytes - num_bytes;

			if (growmode) {
				want_bytes = stream->chunk_size;
			}
			php_stream_fill_read_buffer(stream, want_bytes TSRMLS_CC);
		}

	}

 exit_ugetline:

	*pmax_chars = num_chars;
	*pmax_bytes = num_bytes;
	*pis_unicode = is_unicode;
	if (buf) {
		buf[num_bytes >> 1] = 0;
	}
	stream->position += num_bytes;

	return buf;
}

PHPAPI char *php_stream_get_record(php_stream *stream, size_t maxlen, size_t *returned_len, char *delim, size_t delim_len TSRMLS_DC)
{
	/* UTODO: Needs desperate rewriting for unicode conversion */
	return NULL;

#ifdef SMG_0
	char *e, *buf;
	size_t toread;
	int skip = 0;

	php_stream_fill_read_buffer(stream, maxlen TSRMLS_CC);

	if (delim_len == 0 || !delim) {
		toread = maxlen;
	} else {
		if (delim_len == 1) {
			e = memchr(stream->readbuf, *delim, stream->readbuf_len);
		} else {
			e = php_memnstr(stream->readbuf, delim, delim_len, (stream->readbuf + stream->readbuflen));
		}

		if (!e) {
			toread = maxlen;
		} else {
			toread = e - (char *) stream->readbuf;
			skip = 1;
		}
	}

	if (toread > maxlen && maxlen > 0) {
		toread = maxlen;
	}

	buf = emalloc(toread + 1);
	*returned_len = php_stream_read(stream, buf, toread);

	if (*returned_len >= 0) {
		if (skip) {
			stream->readpos += delim_len;
			stream->position += delim_len;
		}
		buf[*returned_len] = '\0';
		return buf;
	} else {
		efree(buf);
		return NULL;
	}
#endif
}

PHPAPI void _php_stream_flush_readbuf(php_stream *stream TSRMLS_DC)
{
	php_stream_bucket *bucket;

	while ((bucket = stream->readbuf.head)) {
		php_stream_bucket_unlink(bucket TSRMLS_CC);
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}
	stream->readbuf_ofs = stream->readbuf_avail = 0;
}

/* Writes a buffer directly to a stream, using multiple of the chunk size */
static size_t _php_stream_write_buffer(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	size_t didwrite = 0, towrite, justwrote;

 	/* if we have a seekable stream we need to ensure that data is written at the
 	 * current stream->position. This means invalidating the read buffer and then
	 * performing a low-level seek */
/* UTODO: FIX this
	if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0 && stream->readpos != stream->writepos) {
*/
	if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0) {
		php_stream_flush_readbuf(stream);

		stream->ops->seek(stream, stream->position, SEEK_SET, &stream->position TSRMLS_CC);
	}

 
	while (count > 0) {
		towrite = count;
		if (towrite > stream->chunk_size)
			towrite = stream->chunk_size;

		justwrote = stream->ops->write(stream, buf, towrite TSRMLS_CC);

		/* convert justwrote to an integer, since normally it is unsigned */
		if ((int)justwrote > 0) {
			buf += justwrote;
			count -= justwrote;
			didwrite += justwrote;
			
			/* Only screw with the buffer if we can seek, otherwise we lose data
			 * buffered from fifos and sockets */
			if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0) {
				stream->position += justwrote;
			}
		} else {
			break;
		}
	}
	return didwrite;

}

/* push some data through the write filter chain.
 * buf may be NULL, if flags are set to indicate a flush.
 * This may trigger a real write to the stream.
 * Returns the number of bytes consumed from buf by the first filter in the chain.
 * */
static size_t _php_stream_write_filtered(php_stream *stream, const char *buf, size_t count, int flags, int is_unicode TSRMLS_DC)
{
	size_t consumed = 0;
	php_stream_bucket *bucket;
	php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
	php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out, *brig_swap;
	php_stream_filter_status_t status = PSFS_ERR_FATAL;
	php_stream_filter *filter;

	if (buf) {
		if (is_unicode) {
			bucket = php_stream_bucket_new_unicode(stream, (UChar *)buf, count, 0, 0 TSRMLS_CC);
		} else {
			bucket = php_stream_bucket_new(stream, (char *)buf, count, 0, 0 TSRMLS_CC);
		}
		php_stream_bucket_append(brig_inp, bucket TSRMLS_CC);
	}

	for (filter = stream->writefilters.head; filter; filter = filter->next) {
		/* for our return value, we are interested in the number of bytes consumed from
		 * the first filter in the chain */
		status = filter->fops->filter(stream, filter, brig_inp, brig_outp, (filter == stream->writefilters.head) ? &consumed : NULL, flags TSRMLS_CC);
		if (status != PSFS_PASS_ON) {
			break;
		}
		/* brig_out becomes brig_in.
		 * brig_in will always be empty here, as the filter MUST attach any un-consumed buckets
		 * to its own brigade */
		brig_swap = brig_inp;
		brig_inp = brig_outp;
		brig_outp = brig_swap;
		memset(brig_outp, 0, sizeof(*brig_outp));
	}

	switch (status) {
		case PSFS_PASS_ON:
			/* filter chain generated some output; push it through to the
			 * underlying stream */
			while (brig_inp->head) {
				bucket = brig_inp->head;
				if (bucket->is_unicode) {
					_php_stream_write_buffer(stream, (char *)bucket->buf.ustr.val, UBYTES(bucket->buf.ustr.len) TSRMLS_CC);
				} else {
					_php_stream_write_buffer(stream, bucket->buf.str.val, bucket->buf.str.len TSRMLS_CC);
				}
				/* Potential error situation - eg: no space on device. Perhaps we should keep this brigade
				 * hanging around and try to write it later.
				 * At the moment, we just drop it on the floor
				 * */

				php_stream_bucket_unlink(bucket TSRMLS_CC);
				php_stream_bucket_delref(bucket TSRMLS_CC);
			}
			break;
		case PSFS_FEED_ME:
			/* need more data before we can push data through to the stream */
			break;

		case PSFS_ERR_FATAL:
			/* some fatal error.  Theoretically, the stream is borked, so all
			 * further writes should fail. */
			break;
	}

	return consumed;
}

PHPAPI int _php_stream_will_read_unicode(php_stream *stream TSRMLS_DC)
{
	php_stream_filter *filter;
	int inverted = 0;

	if (stream->readbuf.head) {
		/* If there are buckets available, what do they hold */
		return stream->readbuf.head->is_unicode;
	}

	if (!stream->readfilters.head) {
		/* Not filtered == reads as string */
		return 0;
	}

	for(filter = stream->readfilters.tail; filter; filter = filter->prev) {
		if (filter->flags & PSFO_FLAG_OUTPUTS_SAME) {
			continue;
		}
		if (filter->flags & PSFO_FLAG_OUTPUTS_OPPOSITE) {
			inverted ^= 1;
			continue;
		}
		if (filter->flags & PSFO_FLAG_OUTPUTS_ANY) {
			/* Indeterminate */
			return -1;
		}
		if (filter->flags & PSFO_FLAG_OUTPUTS_STRING) {
			/* If an inversion happens, it'll be unicode, otherwise string */
			return inverted;
		}
		if (filter->flags & PSFO_FLAG_OUTPUTS_UNICODE) {
			/* If an inversion happens, it'll be string, otherwise unicode */
			return inverted ^ 1;
		}
	}

	/* string comes from stream so apply same logic as filter outputting string */
	return inverted;
}

PHPAPI int _php_stream_flush(php_stream *stream, int closing TSRMLS_DC)
{
	int ret = 0;

	if (stream->writefilters.head) {
		_php_stream_write_filtered(stream, NULL, 0, closing ? PSFS_FLAG_FLUSH_CLOSE : PSFS_FLAG_FLUSH_INC, 0  TSRMLS_CC);
	}

	if (stream->ops->flush) {
		ret = stream->ops->flush(stream TSRMLS_CC);
	}

	return ret;
}

PHPAPI size_t _php_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	if (buf == NULL || count == 0 || stream->ops->write == NULL) {
		return 0;
	}

	if (stream->writefilters.head) {
		return _php_stream_write_filtered(stream, buf, count, PSFS_FLAG_NORMAL, 0 TSRMLS_CC);
	} else {
		return _php_stream_write_buffer(stream, buf, count TSRMLS_CC);
	}
}

PHPAPI size_t _php_stream_u_write(php_stream *stream, const UChar *buf, int32_t count TSRMLS_DC)
{
	if (buf == NULL || count == 0 || stream->ops->write == NULL) {
		return 0;
	}

	if (stream->writefilters.head) {
		return _php_stream_write_filtered(stream, (const char*)buf, count, PSFS_FLAG_NORMAL, 1 TSRMLS_CC);
	} else {
		int32_t ret;

		ret = _php_stream_write_buffer(stream, (const char*)buf, UBYTES(count) TSRMLS_CC);

		/* Return data points, not bytes */
		if (ret > 0) {
			ret >>= 1;
		}
		return ret;
	}
}

PHPAPI size_t _php_stream_printf(php_stream *stream TSRMLS_DC, const char *fmt, ...)
{
	size_t count;
	char *buf;
	va_list ap;

	va_start(ap, fmt);
	count = vspprintf(&buf, 0, fmt, ap);
	va_end(ap);

	if (!buf) {
		return 0; /* error condition */
	}

	count = php_stream_write(stream, buf, count);
	efree(buf);

	return count;
}

PHPAPI off_t _php_stream_tell(php_stream *stream TSRMLS_DC)
{
	return stream->position;
}

PHPAPI int _php_stream_seek(php_stream *stream, off_t offset, int whence TSRMLS_DC)
{
	/* handle the case where we are in the buffer */
	if ((stream->flags & PHP_STREAM_FLAG_NO_BUFFER) == 0) {
		switch(whence) {
			case SEEK_SET:
				if (offset < stream->position ||
					offset > stream->position + stream->readbuf_avail) {
					break;
				}
				/* act like SEEK_CUR */
				whence = SEEK_CUR;
				offset -= stream->position;
				/* fall through */
			case SEEK_CUR:
				if (offset == 0) {
					/* nothing to do */
					return 0;
				}

				if (offset > 0 && offset <= stream->readbuf_avail) {
					php_stream_bucket *bucket;

					while (offset && (bucket = stream->readbuf.head)) {
						int consume = bucket->buf.str.len - stream->readbuf_ofs;

						if (consume > offset) {
							/* seeking within this bucket */
							stream->readbuf_ofs += offset;
							stream->readbuf_avail -= offset;
							stream->position += offset;
							break;
						}

						/* consume the remaining bucket */
						stream->position += consume;
						stream->readbuf_ofs = 0;
						stream->readbuf_avail -= consume;
						offset -= consume;

						php_stream_bucket_unlink(bucket TSRMLS_CC);
						php_stream_bucket_delref(bucket TSRMLS_CC);
					}
					stream->eof = 0;
					return 0;
				}
				break;
		}
	}


	if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0) {
		int ret;
		
		if (stream->writefilters.head) {
			_php_stream_flush(stream, 0 TSRMLS_CC);
		}

		switch(whence) {
			case SEEK_CUR:
				offset = stream->position + offset;
				whence = SEEK_SET;
				break;
		}
		ret = stream->ops->seek(stream, offset, whence, &stream->position TSRMLS_CC);

		if (((stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0) || ret == 0) {
			if (ret == 0) {
				stream->eof = 0;
			}

			/* invalidate the buffer contents */
			php_stream_flush_readbuf(stream);

			return ret;
		}
		/* else the stream has decided that it can't support seeking after all;
		 * fall through to attempt emulation */
	}

	/* emulate forward moving seeks with reads */
	if (whence == SEEK_CUR && offset > 0) {
		char tmp[1024];
		while(offset >= sizeof(tmp)) {
			if (php_stream_read(stream, tmp, sizeof(tmp)) == 0) {
				return -1;
			}
			offset -= sizeof(tmp);
		}
		if (offset && (php_stream_read(stream, tmp, offset) == 0)) {
			return -1;
		}
		stream->eof = 0;
		return 0;
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream does not support seeking");

	return -1;
}

PHPAPI int _php_stream_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	int ret = PHP_STREAM_OPTION_RETURN_NOTIMPL;

	if (stream->ops->set_option) {
		ret = stream->ops->set_option(stream, option, value, ptrparam TSRMLS_CC);
	}

	if (ret == PHP_STREAM_OPTION_RETURN_NOTIMPL) {
		switch(option) {
			case PHP_STREAM_OPTION_SET_CHUNK_SIZE:
				ret = stream->chunk_size;
				stream->chunk_size = value;
				return ret;

			case PHP_STREAM_OPTION_READ_BUFFER:
				/* try to match the buffer mode as best we can */
				if (value == PHP_STREAM_BUFFER_NONE) {
					stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
				} else {
					stream->flags ^= PHP_STREAM_FLAG_NO_BUFFER;
				}
				ret = PHP_STREAM_OPTION_RETURN_OK;
				break;
				
			default:
				;
		}
	}

	return ret;
}

PHPAPI int _php_stream_truncate_set_size(php_stream *stream, size_t newsize TSRMLS_DC)
{
	return php_stream_set_option(stream, PHP_STREAM_OPTION_TRUNCATE_API, PHP_STREAM_TRUNCATE_SET_SIZE, &newsize);
}

PHPAPI size_t _php_stream_passthru(php_stream * stream STREAMS_DC TSRMLS_DC)
{
	size_t bcount = 0;
	char buf[8192];
	int b;

	if (php_stream_mmap_possible(stream)) {
		char *p;
		size_t mapped;

		p = php_stream_mmap_range(stream, php_stream_tell(stream), PHP_STREAM_COPY_ALL, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped);

		if (p) {
			PHPWRITE(p, mapped);

			php_stream_mmap_unmap(stream);

			return mapped;
		}
	}

	while ((b = php_stream_read(stream, buf, sizeof(buf))) > 0) {
		PHPWRITE(buf, b);
		bcount += b;
	}

	return bcount;
}


PHPAPI size_t _php_stream_copy_to_mem(php_stream *src, char **buf, size_t maxlen, int persistent STREAMS_DC TSRMLS_DC)
{
	size_t ret = 0;
	char *ptr;
	size_t len = 0, max_len;
	int step = CHUNK_SIZE;
	int min_room = CHUNK_SIZE / 4;
	php_stream_statbuf ssbuf;

	if (buf) { 
		*buf = NULL;
	}

	if (maxlen == 0) { 
		return 0;
	}

	if (maxlen == PHP_STREAM_COPY_ALL) {
		maxlen = 0;
	}

	if (php_stream_mmap_possible(src)) {
		char *p;
		size_t mapped;

		p = php_stream_mmap_range(src, php_stream_tell(src), maxlen, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped);

		if (p) {
			*buf = pemalloc_rel_orig(mapped + 1, persistent);

			if (*buf) {
				memcpy(*buf, p, mapped);
				(*buf)[mapped] = '\0';
			}

			php_stream_mmap_unmap(src);

			return mapped;
		}
	}

	if (maxlen > 0) {
		ptr = *buf = pemalloc_rel_orig(maxlen + 1, persistent);
		while ((len < maxlen) & !php_stream_eof(src)) {
			ret = php_stream_read(src, ptr, maxlen - len);
			len += ret;
			ptr += ret;
		}
		*ptr = '\0';
		return len;
	}

	/* avoid many reallocs by allocating a good sized chunk to begin with, if
	 * we can.  Note that the stream may be filtered, in which case the stat
	 * result may be inaccurate, as the filter may inflate or deflate the
	 * number of bytes that we can read.  In order to avoid an upsize followed
	 * by a downsize of the buffer, overestimate by the step size (which is
	 * 2K).  */
	if (php_stream_stat(src, &ssbuf) == 0 && ssbuf.sb.st_size > 0) {
		max_len = ssbuf.sb.st_size + step;
	} else {
		max_len = step;
	}

	ptr = *buf = pemalloc_rel_orig(max_len, persistent);

	while((ret = php_stream_read(src, ptr, max_len - len)))	{
		len += ret;
		if (len + min_room >= max_len) {
			*buf = perealloc_rel_orig(*buf, max_len + step, persistent);
			max_len += step;
			ptr = *buf + len;
		} else {
			ptr += ret;
		}
	}
	if (len) {
		*buf = perealloc_rel_orig(*buf, len + 1, persistent);
		(*buf)[len] = '\0';
	} else {
		pefree(*buf, persistent);
		*buf = NULL;
	}
	return len;
}

PHPAPI size_t _php_stream_copy_to_stream(php_stream *src, php_stream *dest, size_t maxlen STREAMS_DC TSRMLS_DC)
{
	char buf[CHUNK_SIZE];
	size_t readchunk;
	size_t haveread = 0;
	size_t didread;
	php_stream_statbuf ssbuf;

	if (maxlen == 0) {
		return 0;
	}

	if (maxlen == PHP_STREAM_COPY_ALL) {
		maxlen = 0;
	}

	if (php_stream_stat(src, &ssbuf) == 0) {
		/* in the event that the source file is 0 bytes, return 1 to indicate success
		 * because opening the file to write had already created a copy */
		if (ssbuf.sb.st_size == 0
#ifdef S_ISFIFO
		 && !S_ISFIFO(ssbuf.sb.st_mode)
#endif
#ifdef S_ISCHR
		 && !S_ISCHR(ssbuf.sb.st_mode)
#endif
		) {
			return 1;
		}
	}

	if (php_stream_mmap_possible(src)) {
		char *p;
		size_t mapped;

		p = php_stream_mmap_range(src, php_stream_tell(src), maxlen, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped);

		if (p) {
			haveread = php_stream_write(dest, p, mapped);

			php_stream_mmap_unmap(src);

			return mapped;
		}
	}

	while(1) {
		readchunk = sizeof(buf);

		if (maxlen && (maxlen - haveread) < readchunk)
			readchunk = maxlen - haveread;

		didread = php_stream_read(src, buf, readchunk);

		if (didread) {
			/* extra paranoid */
			size_t didwrite, towrite;
			char *writeptr;

			towrite = didread;
			writeptr = buf;
			haveread += didread;

			while(towrite) {
				didwrite = php_stream_write(dest, writeptr, towrite);
				if (didwrite == 0) {
					return 0;	/* error */
				}

				towrite -= didwrite;
				writeptr += didwrite;
			}
		} else {
			if (maxlen == 0) {
				return haveread;
			} else {
				return 0; /* error */
			}
		}

		if (maxlen - haveread == 0) {
			break;
		}
	}
	return haveread;

}
/* }}} */

/* {{{ wrapper init and registration */

static void stream_resource_regular_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_stream *stream = (php_stream*)rsrc->ptr;
	/* set the return value for pclose */
	FG(pclose_ret) = php_stream_free(stream, PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_RSRC_DTOR);
}

static void stream_resource_persistent_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_stream *stream = (php_stream*)rsrc->ptr;
	FG(pclose_ret) = php_stream_free(stream, PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_RSRC_DTOR);
}

void php_shutdown_stream_hashes(TSRMLS_D)
{
	if (FG(stream_wrappers)) {
		zend_hash_destroy(FG(stream_wrappers));
		efree(FG(stream_wrappers));
		FG(stream_wrappers) = NULL;
	}

	if (FG(stream_filters)) {
		zend_hash_destroy(FG(stream_filters));
		efree(FG(stream_filters));
		FG(stream_filters) = NULL;
	}
}

int php_init_stream_wrappers(int module_number TSRMLS_DC)
{
	le_stream = zend_register_list_destructors_ex(stream_resource_regular_dtor, NULL, "stream", module_number);
	le_pstream = zend_register_list_destructors_ex(NULL, stream_resource_persistent_dtor, "persistent stream", module_number);

	/* Filters are cleaned up by the streams they're attached to */
	le_stream_filter = zend_register_list_destructors_ex(NULL, NULL, "stream filter", module_number);

	return (
			zend_hash_init(&url_stream_wrappers_hash, 0, NULL, NULL, 1) == SUCCESS
			&& 
			zend_hash_init(php_get_stream_filters_hash_global(), 0, NULL, NULL, 1) == SUCCESS
			&&
			zend_hash_init(php_stream_xport_get_hash(), 0, NULL, NULL, 1) == SUCCESS
			&&
			php_stream_xport_register("tcp", php_stream_generic_socket_factory TSRMLS_CC) == SUCCESS
			&&
			php_stream_xport_register("udp", php_stream_generic_socket_factory TSRMLS_CC) == SUCCESS
#if defined(AF_UNIX) && !(defined(PHP_WIN32) || defined(__riscos__) || defined(NETWARE))
			&&
			php_stream_xport_register("unix", php_stream_generic_socket_factory TSRMLS_CC) == SUCCESS
			&&
			php_stream_xport_register("udg", php_stream_generic_socket_factory TSRMLS_CC) == SUCCESS
#endif
		) ? SUCCESS : FAILURE;
}

int php_shutdown_stream_wrappers(int module_number TSRMLS_DC)
{
	zend_hash_destroy(&url_stream_wrappers_hash);
	zend_hash_destroy(php_get_stream_filters_hash_global());
	zend_hash_destroy(php_stream_xport_get_hash());
	return SUCCESS;
}

/* Validate protocol scheme names during registration
 * Must conform to /^[a-zA-Z0-9+.-]+$/
 */
static inline int php_stream_wrapper_scheme_validate(char *protocol, int protocol_len)
{
	int i;

	for(i = 0; i < protocol_len; i++) {
		if (!isalnum((int)protocol[i]) &&
			protocol[i] != '+' &&
			protocol[i] != '-' &&
			protocol[i] != '.') {
			return FAILURE;
		}
	}

	return SUCCESS;
}

/* API for registering GLOBAL wrappers */
PHPAPI int php_register_url_stream_wrapper(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC)
{
	int protocol_len = strlen(protocol);

	if (php_stream_wrapper_scheme_validate(protocol, protocol_len) == FAILURE) {
		return FAILURE;
	}

	return zend_hash_add(&url_stream_wrappers_hash, protocol, protocol_len, wrapper, sizeof(*wrapper), NULL);
}

PHPAPI int php_unregister_url_stream_wrapper(char *protocol TSRMLS_DC)
{
	return zend_hash_del(&url_stream_wrappers_hash, protocol, strlen(protocol));
}

/* API for registering VOLATILE wrappers */
PHPAPI int php_register_url_stream_wrapper_volatile(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC)
{
	int protocol_len = strlen(protocol);

	if (php_stream_wrapper_scheme_validate(protocol, protocol_len) == FAILURE) {
		return FAILURE;
	}

	if (!FG(stream_wrappers)) {
		php_stream_wrapper tmpwrapper;

		ALLOC_HASHTABLE(FG(stream_wrappers));
		zend_hash_init(FG(stream_wrappers), 0, NULL, NULL, 1);
		zend_hash_copy(FG(stream_wrappers), &url_stream_wrappers_hash, NULL, &tmpwrapper, sizeof(php_stream_wrapper));
	}

	return zend_hash_add(FG(stream_wrappers), protocol, protocol_len, wrapper, sizeof(*wrapper), NULL);
}

PHPAPI int php_unregister_url_stream_wrapper_volatile(char *protocol TSRMLS_DC)
{
	if (!FG(stream_wrappers)) {
		php_stream_wrapper tmpwrapper;

		ALLOC_HASHTABLE(FG(stream_wrappers));
		zend_hash_init(FG(stream_wrappers), 0, NULL, NULL, 1);
		zend_hash_copy(FG(stream_wrappers), &url_stream_wrappers_hash, NULL, &tmpwrapper, sizeof(php_stream_wrapper));
	}

	return zend_hash_del(FG(stream_wrappers), protocol, strlen(protocol));
}
/* }}} */

/* {{{ php_stream_locate_url_wrapper */
PHPAPI php_stream_wrapper *php_stream_locate_url_wrapper(const char *path, char **path_for_open, int options TSRMLS_DC)
{
	HashTable *wrapper_hash = (FG(stream_wrappers) ? FG(stream_wrappers) : &url_stream_wrappers_hash);
	php_stream_wrapper *wrapper = NULL;
	const char *p, *protocol = NULL;
	int n = 0;

	if (path_for_open) {
		*path_for_open = (char*)path;
	}

	if (options & IGNORE_URL) {
		return (options & STREAM_LOCATE_WRAPPERS_ONLY) ? NULL : &php_plain_files_wrapper;
	}

	for (p = path; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; p++) {
		n++;
	}

	if ((*p == ':') && (n > 1) && !strncmp("://", p, 3)) {
		protocol = path;
	} else if (strncasecmp(path, "zlib:", 5) == 0) {
		/* BC with older php scripts and zlib wrapper */
		protocol = "compress.zlib";
		n = 13;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Use of \"zlib:\" wrapper is deprecated; please use \"compress.zlib://\" instead.");
	}

	if (protocol)	{
		if (FAILURE == zend_hash_find(wrapper_hash, (char*)protocol, n, (void**)&wrapper))	{
			char wrapper_name[32];

			if (n >= sizeof(wrapper_name))
				n = sizeof(wrapper_name) - 1;
			PHP_STRLCPY(wrapper_name, protocol, sizeof(wrapper_name), n);
			
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unable to find the wrapper \"%s\" - did you forget to enable it when you configured PHP?",
					wrapper_name);

			wrapper = NULL;
			protocol = NULL;
		}
	}
	/* TODO: curl based streams probably support file:// properly */
	if (!protocol || !strncasecmp(protocol, "file", n))	{
		if (protocol) {
			int localhost = 0;

			if (!strncasecmp(path, "file://localhost/", 17)) {
				localhost = 1;
			}

#ifdef PHP_WIN32
			if (localhost == 0 && path[n+3] != '\0' && path[n+3] != '/' && path[n+4] != ':')	{
#else
			if (localhost == 0 && path[n+3] != '/')	{
#endif
				if (options & REPORT_ERRORS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "remote host file access not supported, %s", path);
				}
				return NULL;
			}

			if (path_for_open) {
				/* skip past protocol and :/, but handle windows correctly */
				*path_for_open = (char*)path + n + 1;
				if (localhost == 1) {
					(*path_for_open) += 11;
				}
				while (*(++*path_for_open)=='/');
#ifdef PHP_WIN32
				if (*(*path_for_open + 1) != ':')
#endif
					(*path_for_open)--;
			}
		}

		if (options & STREAM_LOCATE_WRAPPERS_ONLY) {
			return NULL;
		}
		
		if (FG(stream_wrappers)) {
			/* The file:// wrapper may have been disabled/overridden */

			if (wrapper) {
				/* It was found so go ahead and provide it */
				return wrapper;
			}
			
			/* Check again, the original check might have not known the protocol name */
			if (zend_hash_find(wrapper_hash, "file", sizeof("file")-1, (void**)&wrapper) == SUCCESS) {
				return wrapper;
			}

			if (options & REPORT_ERRORS) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Plainfiles wrapper disabled");
			}
			return NULL;
		}

		/* fall back on regular file access */		
		return &php_plain_files_wrapper;
	}

	if (wrapper && wrapper->is_url && !PG(allow_url_fopen)) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "URL file-access is disabled in the server configuration");
		}
		return NULL;
	}

	return wrapper;
}
/* }}} */

/* {{{ _php_stream_mkdir
 */
PHPAPI int _php_stream_mkdir(char *path, int mode, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;

	wrapper = php_stream_locate_url_wrapper(path, NULL, ENFORCE_SAFE_MODE TSRMLS_CC);
	if (!wrapper || !wrapper->wops || !wrapper->wops->stream_mkdir) {
		return 0;
	}

	return wrapper->wops->stream_mkdir(wrapper, path, mode, options, context TSRMLS_CC);
}
/* }}} */

/* {{{ _php_stream_rmdir
 */
PHPAPI int _php_stream_rmdir(char *path, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;

	wrapper = php_stream_locate_url_wrapper(path, NULL, ENFORCE_SAFE_MODE TSRMLS_CC);
	if (!wrapper || !wrapper->wops || !wrapper->wops->stream_rmdir) {
		return 0;
	}

	return wrapper->wops->stream_rmdir(wrapper, path, options, context TSRMLS_CC);
}
/* }}} */

/* {{{ _php_stream_stat_path */
PHPAPI int _php_stream_stat_path(char *path, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open = path;
	int ret;

	/* Try to hit the cache first */
	if (flags & PHP_STREAM_URL_STAT_LINK) {
		if (BG(CurrentLStatFile) && strcmp(path, BG(CurrentLStatFile)) == 0) {
			memcpy(ssb, &BG(lssb), sizeof(php_stream_statbuf));
			return 0;
		}
	} else {
		if (BG(CurrentStatFile) && strcmp(path, BG(CurrentStatFile)) == 0) {
			memcpy(ssb, &BG(ssb), sizeof(php_stream_statbuf));
			return 0;
		}
	}

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, ENFORCE_SAFE_MODE TSRMLS_CC);
	if (wrapper && wrapper->wops->url_stat) {
		ret = wrapper->wops->url_stat(wrapper, path_to_open, flags, ssb, context TSRMLS_CC);
		if (ret == 0) {
			/* Drop into cache */
			if (flags & PHP_STREAM_URL_STAT_LINK) {
				if (BG(CurrentLStatFile)) {
					efree(BG(CurrentLStatFile));
				}
				BG(CurrentLStatFile) = estrdup(path);
				memcpy(&BG(lssb), ssb, sizeof(php_stream_statbuf));
			} else {
				if (BG(CurrentStatFile)) {
					efree(BG(CurrentStatFile));
				}
				BG(CurrentStatFile) = estrdup(path);
				memcpy(&BG(ssb), ssb, sizeof(php_stream_statbuf));
			}
		}
		return ret;
	}
	return -1;
}
/* }}} */

/* {{{ php_stream_opendir */
PHPAPI php_stream *_php_stream_opendir(char *path, int options,
		php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open;

	if (!path || !*path) {
		return NULL;
	}

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);

	if (wrapper && wrapper->wops->dir_opener) {
		stream = wrapper->wops->dir_opener(wrapper,
				path_to_open, "r", options ^ REPORT_ERRORS, NULL,
				context STREAMS_REL_CC TSRMLS_CC);

		if (stream) {
			stream->wrapper = wrapper;
			stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
		}
	} else if (wrapper) {
		php_stream_wrapper_log_error(wrapper, options ^ REPORT_ERRORS TSRMLS_CC, "not implemented");
	}
	if (stream == NULL && (options & REPORT_ERRORS)) {
		php_stream_display_wrapper_errors(wrapper, path, "failed to open dir" TSRMLS_CC);
	}
	php_stream_tidy_wrapper_error_log(wrapper TSRMLS_CC);

	return stream;
}
/* }}} */

/* {{{ _php_stream_readdir */
PHPAPI php_stream_dirent *_php_stream_readdir(php_stream *dirstream, php_stream_dirent *ent TSRMLS_DC)
{

	if (sizeof(php_stream_dirent) == php_stream_read(dirstream, (char*)ent, sizeof(php_stream_dirent))) {
		return ent;
	}

	return NULL;
}
/* }}} */

/* {{{ php_stream_open_wrapper_ex */
PHPAPI php_stream *_php_stream_open_wrapper_ex(char *path, char *mode, int options,
		char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open;
	int persistent = options & STREAM_OPEN_PERSISTENT;
	char *copy_of_path = NULL;
	int implicit_mode[16];
	int modelen = strlen(mode);
	
	if (opened_path) {
		*opened_path = NULL;
	}

	if (!path || !*path) {
		return NULL;
	}

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);
	if (options & STREAM_USE_URL && (!wrapper || !wrapper->is_url)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This function may only be used against URLs.");
		return NULL;
	}

	memcpy(implicit_mode, mode, modelen);
	if (context && context->default_mode && modelen < 15 && !strchr(mode, 't') && !strchr(mode, 'b')) {
		if (context->default_mode & PHP_FILE_BINARY) {
			implicit_mode[modelen++] = 'b';
		} else if (context->default_mode & PHP_FILE_TEXT) {
			implicit_mode[modelen++] = 't';
		}
		implicit_mode[modelen] = 0;
	}

	if (wrapper) {

		stream = wrapper->wops->stream_opener(wrapper,
				path_to_open, implicit_mode, options ^ REPORT_ERRORS,
				opened_path, context STREAMS_REL_CC TSRMLS_CC);

		/* if the caller asked for a persistent stream but the wrapper did not
		 * return one, force an error here */
		if (stream && (options & STREAM_OPEN_PERSISTENT) && !stream->is_persistent) {
			php_stream_wrapper_log_error(wrapper, options ^ REPORT_ERRORS TSRMLS_CC,
					"wrapper does not support persistent streams");
			php_stream_close(stream);
			stream = NULL;
		}
		
		if (stream) {
			stream->wrapper = wrapper;
			memcpy(stream->mode, implicit_mode, modelen + 1);
		}
	}

	if (stream) {
		if (stream->orig_path) {
			pefree(stream->orig_path, persistent);
		}
		copy_of_path = pestrdup(path, persistent);
		stream->orig_path = copy_of_path;
	}

	if (stream != NULL && (options & STREAM_MUST_SEEK)) {
		php_stream *newstream;

		switch(php_stream_make_seekable_rel(stream, &newstream,
					(options & STREAM_WILL_CAST)
						? PHP_STREAM_PREFER_STDIO : PHP_STREAM_NO_PREFERENCE)) {
			case PHP_STREAM_UNCHANGED:
				return stream;
			case PHP_STREAM_RELEASED:
				newstream->orig_path = pestrdup(path, persistent);
				return newstream;
			default:
				php_stream_close(stream);
				stream = NULL;
				if (options & REPORT_ERRORS) {
					char *tmp = estrdup(path);
					php_strip_url_passwd(tmp);
					php_error_docref1(NULL TSRMLS_CC, tmp, E_WARNING, "could not make seekable - %s",
							tmp);
					efree(tmp);

					options ^= REPORT_ERRORS;
				}
		}
	}

	if (stream && stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0 && strchr(mode, 'a') && stream->position == 0) {
		off_t newpos = 0;

		/* if opened for append, we need to revise our idea of the initial file position */
		if (0 == stream->ops->seek(stream, 0, SEEK_CUR, &newpos TSRMLS_CC)) {
			stream->position = newpos;
		}
	}

	/* Output encoding on text mode streams defaults to utf8 unless specified in context parameter */
	if (stream && strchr(implicit_mode, 't') && (strchr(implicit_mode, 'w') || strchr(implicit_mode, 'a') || strchr(implicit_mode, '+'))) {
		php_stream_filter *filter;
		char *encoding = (context && context->output_encoding) ? context->output_encoding : "utf8";
		char *filtername;
		int encoding_len = strlen(encoding);

		filtername = emalloc(encoding_len + sizeof("unicode.to."));
		memcpy(filtername, "unicode.to.", sizeof("unicode.to.") - 1);
		memcpy(filtername + sizeof("unicode.to.") - 1, encoding, encoding_len + 1);

		filter = php_stream_filter_create(filtername, NULL, persistent TSRMLS_CC);
		if (!filter) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Failed applying output encoding");
		} else {
			php_stream_filter_append(&stream->writefilters, filter);
		}
		efree(filtername);
	}

	if (stream && strchr(implicit_mode, 't') && (strchr(implicit_mode, 'r') || strchr(implicit_mode, '+'))) {
		php_stream_filter *filter;
		char *filtername;
		char *encoding = (context && context->input_encoding) ? context->input_encoding : "utf8";
		int input_encoding_len = strlen(encoding);

		filtername = emalloc(input_encoding_len + sizeof("unicode.from."));
		memcpy(filtername, "unicode.from.", sizeof("unicode.from.") - 1);
		memcpy(filtername + sizeof("unicode.from.") - 1, encoding, input_encoding_len + 1);

		filter = php_stream_filter_create(filtername, NULL, persistent TSRMLS_CC);
		if (!filter) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Failed applying input encoding");
		} else {
			php_stream_filter_append(&stream->readfilters, filter);
		}
		efree(filtername);
	}

	if (stream == NULL && (options & REPORT_ERRORS)) {
		php_stream_display_wrapper_errors(wrapper, path, "failed to open stream" TSRMLS_CC);
	}
	php_stream_tidy_wrapper_error_log(wrapper TSRMLS_CC);
#if ZEND_DEBUG
	if (stream == NULL && copy_of_path != NULL) {
		pefree(copy_of_path, persistent);
	}
#endif


	return stream;
}
/* }}} */

/* {{{ context API */
PHPAPI php_stream_context *php_stream_context_set(php_stream *stream, php_stream_context *context)
{
	php_stream_context *oldcontext = stream->context;
	stream->context = context;
	return oldcontext;
}

PHPAPI void php_stream_notification_notify(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC)
{
	if (context && context->notifier)
		context->notifier->func(context, notifycode, severity, xmsg, xcode, bytes_sofar, bytes_max, ptr TSRMLS_CC);
}

PHPAPI void php_stream_context_free(php_stream_context *context)
{
	if (context->options) {
		zval_ptr_dtor(&context->options);
		context->options = NULL;
	}
	if (context->notifier) {
		php_stream_notification_free(context->notifier);
		context->notifier = NULL;
	}
	if (context->input_encoding) {
		efree(context->input_encoding);
	}
	if (context->output_encoding) {
		efree(context->output_encoding);
	}
	if (context->links) {
		zval_ptr_dtor(&context->links);
		context->links = NULL;
	}
	efree(context);
}

PHPAPI php_stream_context *php_stream_context_alloc(void)
{
	php_stream_context *context;

	context = ecalloc(1, sizeof(php_stream_context));
	context->notifier = NULL;
	MAKE_STD_ZVAL(context->options);
	array_init(context->options);

	context->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, context, php_le_stream_context());
	return context;
}

PHPAPI php_stream_notifier *php_stream_notification_alloc(void)
{
	return ecalloc(1, sizeof(php_stream_notifier));
}

PHPAPI void php_stream_notification_free(php_stream_notifier *notifier)
{
	if (notifier->dtor) {
		notifier->dtor(notifier);
	}
	efree(notifier);
}

PHPAPI int php_stream_context_get_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval ***optionvalue)
{
	zval **wrapperhash;

	if (FAILURE == zend_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash)) {
		return FAILURE;
	}
	return zend_hash_find(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)optionvalue);
}

PHPAPI int php_stream_context_set_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval *optionvalue)
{
	zval **wrapperhash;
	zval *category, *copied_val;

	ALLOC_INIT_ZVAL(copied_val);
	*copied_val = *optionvalue;
	zval_copy_ctor(copied_val);
	INIT_PZVAL(copied_val);

	if (FAILURE == zend_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash)) {
		MAKE_STD_ZVAL(category);
		array_init(category);
		if (FAILURE == zend_hash_update(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&category, sizeof(zval *), NULL)) {
			return FAILURE;
		}

		wrapperhash = &category;
	}
	return zend_hash_update(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)&copied_val, sizeof(zval *), NULL);
}

PHPAPI int php_stream_context_get_link(php_stream_context *context,
        const char *hostent, php_stream **stream)
{
	php_stream **pstream;

	if (!stream || !hostent || !context || !(context->links)) {
		return FAILURE;
	}
	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1, (void**)&pstream)) {
		*stream = *pstream;
		return SUCCESS;
	}
	return FAILURE;
}

PHPAPI int php_stream_context_set_link(php_stream_context *context,
        const char *hostent, php_stream *stream)
{
	if (!context) {
		return FAILURE;
	}
	if (!context->links) {
		ALLOC_INIT_ZVAL(context->links);
		array_init(context->links);
	}
	if (!stream) {
		/* Delete any entry for <hostent> */
		return zend_hash_del(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1);
	}
	return zend_hash_update(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1, (void**)&stream, sizeof(php_stream *), NULL);
}

PHPAPI int php_stream_context_del_link(php_stream_context *context,
        php_stream *stream)
{
	php_stream **pstream;
	char *hostent;
	int ret = SUCCESS;

	if (!context || !context->links || !stream) {
		return FAILURE;
	}

	for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(context->links));
		SUCCESS == zend_hash_get_current_data(Z_ARRVAL_P(context->links), (void**)&pstream);
		zend_hash_move_forward(Z_ARRVAL_P(context->links))) {
		if (*pstream == stream) {
			if (SUCCESS == zend_hash_get_current_key(Z_ARRVAL_P(context->links), &hostent, NULL, 0)) {
				if (FAILURE == zend_hash_del(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1)) {
					ret = FAILURE;
				}
			} else {
				ret = FAILURE;
			}
		}
	}

	return ret;
}
/* }}} */

/* {{{ php_stream_dirent_alphasort
 */
PHPAPI int php_stream_dirent_alphasort(const char **a, const char **b)
{
	return strcoll(*a, *b);
}
/* }}} */

/* {{{ php_stream_dirent_alphasortr
 */
PHPAPI int php_stream_dirent_alphasortr(const char **a, const char **b)
{
	return strcoll(*b, *a);
}
/* }}} */

/* {{{ php_stream_scandir
 */
PHPAPI int _php_stream_scandir(char *dirname, char **namelist[], int flags, php_stream_context *context,
			  int (*compare) (const char **a, const char **b) TSRMLS_DC)
{
	php_stream *stream;
	php_stream_dirent sdp;
	char **vector = NULL;
	int vector_size = 0;
	int nfiles = 0;

	if (!namelist) {
		return FAILURE;
	}

	stream = php_stream_opendir(dirname, ENFORCE_SAFE_MODE | REPORT_ERRORS, context);
	if (!stream) {
		return FAILURE;
	}

	while (php_stream_readdir(stream, &sdp)) {
		if (nfiles == vector_size) {
			if (vector_size == 0) {
				vector_size = 10;
			} else {
				vector_size *= 2;
			}
			vector = (char **) erealloc(vector, vector_size * sizeof(char *));
		}

		vector[nfiles] = estrdup(sdp.d_name);

		nfiles++;
	}
	php_stream_closedir(stream);

	*namelist = vector;

	if (compare) {
		qsort(*namelist, nfiles, sizeof(char *), (int(*)(const void *, const void *))compare);
	}
	return nfiles;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
