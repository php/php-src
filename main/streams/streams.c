/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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

static int _php_stream_release_context(zend_rsrc_list_entry *le, void *pContext TSRMLS_DC)
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
			if (wrapper == &php_plain_files_wrapper) {
				msg = strerror(errno);
			} else {
				msg = "operation failed";
			}
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
	ret->readbuf_type = IS_STRING;

#if ZEND_DEBUG
	ret->open_filename = __zend_orig_filename ? __zend_orig_filename : __zend_filename;
	ret->open_lineno = __zend_orig_lineno ? __zend_orig_lineno : __zend_lineno;
#endif

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

static int _php_stream_free_persistent(zend_rsrc_list_entry *le, void *pStream TSRMLS_DC)
{
	return le->ptr == pStream;
}

PHPAPI int _php_stream_free(php_stream *stream, int close_options TSRMLS_DC) /* {{{ */
{
	int ret = 1;
	int remove_rsrc = 1;
	int preserve_handle = close_options & PHP_STREAM_FREE_PRESERVE_HANDLE ? 1 : 0;
	int release_cast = 1;
	php_stream_context *context = stream->context;

	if (stream->flags & PHP_STREAM_FLAG_NO_CLOSE) {
		preserve_handle = 1;
	}

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

		if (stream->readbuf.v) {
			pefree(stream->readbuf.v, stream->is_persistent);
			stream->readbuf.v = NULL;
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
			spprintf(&leakinfo, 0, __FILE__ "(%d) : Stream of type '%s' %p (path:%s) was not closed (opened in %s on line %d)\n", __LINE__, stream->ops->label, stream, stream->orig_path, stream->open_filename, stream->open_lineno);

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

	if (context) {
		zend_list_delete(context->rsrc_id);
	}

	return ret;
}
/* }}} */

/* {{{ generic stream operations */

/* size == full characters (char, UChar, or 2x UChar)
   TODO: Needs better handling of surrogate pairs */
static void php_stream_fill_read_buffer(php_stream *stream, size_t size TSRMLS_DC)
{
	if (stream->readpos == stream->writepos) {
		stream->readpos = stream->writepos = 0;
	}

	/* allocate/fill the buffer */

	if (stream->readfilters.head) {
		char *chunk_buf;
		int err_flag = 0;
		php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
		php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out, *brig_swap;

		/* Invalidate the existing cache, otherwise reads can fail, see note in
		   main/streams/filter.c::_php_stream_filter_append */
		stream->writepos = stream->readpos = 0;

		/* allocate a buffer for reading chunks */
		chunk_buf = emalloc(stream->chunk_size);

		while (!stream->eof && !err_flag && (stream->writepos - stream->readpos < (off_t)size)) {
			size_t justread = 0;
			int flags;
			php_stream_bucket *bucket;
			php_stream_filter_status_t status = PSFS_ERR_FATAL;
			php_stream_filter *filter;

			/* read a chunk into a bucket */
			justread = stream->ops->read(stream, chunk_buf, stream->chunk_size TSRMLS_CC);
			if (justread && justread != (size_t)-1) {
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
					while (brig_inp->head) {
						bucket = brig_inp->head;
						if (bucket->buf_type != stream->readbuf_type) {
							/* Stream expects different datatype than bucket has, convert slopily */
							php_stream_bucket_convert_notranscode(bucket, stream->readbuf_type);
						}
						/* Bucket type now matches stream type */

						/* grow buffer to hold this bucket
						 * TODO: this can fail for persistent streams */
						if (stream->readbuflen - stream->writepos < bucket->buflen) {
							stream->readbuflen += bucket->buflen;
							stream->readbuf.v = perealloc(stream->readbuf.v, PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->readbuflen), stream->is_persistent);
						}
						memcpy(stream->readbuf.s + PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->writepos), bucket->buf.s, PS_ULEN(stream->readbuf_type == IS_UNICODE, bucket->buflen));
						stream->writepos += bucket->buflen;

						php_stream_bucket_unlink(bucket TSRMLS_CC);
						php_stream_bucket_delref(bucket TSRMLS_CC);
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

			if (justread == 0 || justread == (size_t)-1) {
				break;
			}
		}

		efree(chunk_buf);
	} else {	/* Unfiltered Binary stream */
		/* reduce buffer memory consumption if possible, to avoid a realloc */
		if (stream->readbuf.s && stream->readbuflen - stream->writepos < stream->chunk_size) {
			memmove(stream->readbuf.s, stream->readbuf.s + stream->readpos, stream->writepos - stream->readpos);
			stream->writepos -= stream->readpos;
			stream->readpos = 0;
		}
		/* is there enough data in the buffer ? */
		while (stream->writepos - stream->readpos < (off_t)size) {
			size_t justread = 0;
			size_t toread;

			/* grow the buffer if required
			 * TODO: this can fail for persistent streams */
			if (stream->readbuflen - stream->writepos < stream->chunk_size) {
				stream->readbuflen += stream->chunk_size;
				stream->readbuf.s = (char*)perealloc(stream->readbuf.s, stream->readbuflen, stream->is_persistent);
			}

			toread = stream->readbuflen - stream->writepos;
			justread = stream->ops->read(stream, stream->readbuf.s + stream->writepos, toread TSRMLS_CC);

			if (justread != (size_t)-1) {
				stream->writepos += justread;
			}
			if (stream->eof || justread != toread) {
				break;
			}
		}
	}
}

/* Reads binary data from stream, if the stream is unicode (text), the raw unicode data will be returned */
PHPAPI size_t _php_stream_read(php_stream *stream, char *buf, size_t size TSRMLS_DC)
{
	size_t toread = 0, didread = 0;

	while (size > 0) {

		/* take from the read buffer first.
		 * It is possible that a buffered stream was switched to non-buffered, so we
		 * drain the remainder of the buffer before using the "raw" read mode for
		 * the excess */
		if (stream->writepos - stream->readpos > 0) {
			toread = PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->writepos - stream->readpos);

			if (toread > size) {
				toread = size;
			}

			if (stream->readbuf_type == IS_UNICODE) {
				/* Sloppy read, anyone using php_stream_read() on a unicode stream
				 * had better know what they're doing */
				
				memcpy(buf, stream->readbuf.u + stream->readpos, toread);
				stream->readpos += ceil(toread / UBYTES(1));
			} else {
				memcpy(buf, stream->readbuf.s + stream->readpos, toread);
				stream->readpos += toread;
			}

			size -= toread;
			buf += toread;
			didread += toread;
		}

		/* ignore eof here; the underlying state might have changed */
		if (size == 0) {
			break;
		}

		if (!stream->readfilters.head && (stream->flags & PHP_STREAM_FLAG_NO_BUFFER || stream->chunk_size == 1)) {
			toread = stream->ops->read(stream, buf, size TSRMLS_CC);
		} else {
			php_stream_fill_read_buffer(stream, size TSRMLS_CC);

			toread = stream->writepos - stream->readpos;
			if (toread > size) {
				toread = size;
			}

			if (toread > 0) {
				if (stream->readbuf_type == IS_UNICODE) {
					/* Sloppy read, anyone using php_stream_read() on a unicode stream
					 * had better know what they're doing */
				
					memcpy(buf, stream->readbuf.u + stream->readpos, toread);
					stream->readpos += ceil(toread / UBYTES(1));
				} else {
					memcpy(buf, stream->readbuf.s + stream->readpos, toread);
				}
				stream->readpos += toread;
			}
		}
		if (toread > 0) {
			didread += toread;
			buf += toread;
			size -= toread;
		} else {
			/* EOF, or temporary end of data (for non-blocking mode). */
			break;
		}
		/* just break anyway, to avoid greedy read */
		if (stream->wrapper != &php_plain_files_wrapper) {
			break;
		}
	}

	if (didread > 0) {
		stream->position += didread;
	}

	return didread;
}

/* Read unicode data from a stream.  Returns failure (-1) if not a unicode stream */
PHPAPI size_t _php_stream_read_unicode(php_stream *stream, UChar *buf, int size, int maxchars TSRMLS_DC)
{
	size_t toread = 0, didread = 0, string_length = 0;

	if (stream->readbuf_type != IS_UNICODE) {
		return -1;
	}

	while (size > 0) {
		/* take from the read buffer first.
		 * It is possible that a buffered stream was switched to non-buffered, so we
		 * drain the remainder of the buffer before using the "raw" read mode for
		 * the excess */

		while (size > 0 && (toread = (stream->writepos - stream->readpos)) &&
              (toread > 1 ||
			   !U16_IS_SURROGATE(stream->readbuf.u[stream->readpos]) ||
			   !U16_IS_SURROGATE_LEAD(stream->readbuf.u[stream->readpos]) )) {
			int length;

			if (toread > size) {
				toread = size;
			}

			if (U16_IS_SURROGATE(stream->readbuf.u[stream->readpos + toread - 1]) &&
				U16_IS_SURROGATE_LEAD(stream->readbuf.u[stream->readpos + toread - 1])) {
				/* Don't split surrogates */
				toread--;
				if (!toread) {
					break;
				}
			}

			if (maxchars > -1) {
				length = u_countChar32(stream->readbuf.u + stream->readpos, toread);
				if (string_length + length > maxchars) {
					/* Don't read more U32 points than the caller asked for */
					toread = 0;
					length = size - string_length;
					U16_FWD_N(stream->readbuf.u + stream->readpos, toread, stream->writepos - stream->readpos, length);
				}
				string_length += length;
			}

			memcpy(buf, stream->readbuf.u + stream->readpos, UBYTES(toread));
			stream->readpos += toread;
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
		if (stream->writepos - stream->readpos <= 0) {
			/* EOF, or temporary end of data (for non-blocking mode). */
			break;
		}
	}

	if (didread > 0) {
		stream->position += didread;
	}

	return didread;
}

PHPAPI UChar *_php_stream_read_unicode_chars(php_stream *stream, int *pchars TSRMLS_DC)
{
	int size = *pchars;
	UChar *bufstart, *buf;
	int buflen = size;
	size_t toread = 0, didread = 0, string_length = 0;

	if (stream->readbuf_type != IS_UNICODE) {
		return NULL;
	}

	/* Allocate for ideal size first, add more later if needed */
	bufstart = buf = eumalloc(buflen + 1);

	while (size > 0) {
		/* take from the read buffer first.
		 * It is possible that a buffered stream was switched to non-buffered, so we
		 * drain the remainder of the buffer before using the "raw" read mode for
		 * the excess */

		while (size > 0 && (toread = (stream->writepos - stream->readpos)) &&
              (toread > 1 || 
			   !U16_IS_SURROGATE(stream->readbuf.u[stream->readpos]) ||
			   !U16_IS_SURROGATE_LEAD(stream->readbuf.u[stream->readpos]) )) {
			int length;

			if (toread > size) {
				toread = size;
			}

			if (U16_IS_SURROGATE(stream->readbuf.u[stream->readpos + toread - 1]) &&
				U16_IS_SURROGATE_LEAD(stream->readbuf.u[stream->readpos + toread - 1])) {
				/* Don't split surrogates */
				toread--;
				if (!toread) {
					break;
				}
			}

			length = u_countChar32(stream->readbuf.u + stream->readpos, toread);
			if (string_length + length > size) {
				/* Don't read more U32 points than the caller asked for */
				toread = 0;
				length = size - string_length;
				U16_FWD_N(stream->readbuf.u + stream->readpos, toread, stream->writepos - stream->readpos, length);
			}

			if (toread > (buflen - didread)) {
				/* We know we're in surrogated territory at this point, allocate aggressively */
				int ofs = buf - bufstart;
				buflen += size * 2;
				bufstart = eurealloc(bufstart, buflen + 1);
				buf = bufstart + ofs;
			}

			memcpy(buf, stream->readbuf.u + stream->readpos, UBYTES(toread));
			stream->readpos += toread;
			size -= toread;
			buf += toread;
			didread += toread;
			string_length += length;
		}

		/* ignore eof here; the underlying state might have changed */
		if (size == 0) {
			break;
		}

		/* just break anyway, to avoid greedy read */
		if (didread > 0 && (stream->wrapper != &php_plain_files_wrapper)) {
			break;
		}

		php_stream_fill_read_buffer(stream, UBYTES(size) TSRMLS_CC);
		if (stream->writepos - stream->readpos <= 0) {
			/* EOF, or temporary end of data (for non-blocking mode). */
			break;
		}
	}

	if (didread > 0) {
		stream->position += didread;
	}

	*pchars = string_length;
	buf[0] = 0;
	return bufstart;
}

PHPAPI int _php_stream_eof(php_stream *stream TSRMLS_DC)
{
	/* if there is data in the buffer, it's not EOF */
	if (stream->writepos - stream->readpos > 0) {
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

PHPAPI void *php_stream_locate_eol(php_stream *stream, zstr zbuf, int buf_len TSRMLS_DC)
{
	size_t avail;
	char *cr, *lf, *eol = NULL;
	char *readptr, *buf = zbuf.s;

	if (!buf) {
		readptr = stream->readbuf.s + PS_ULEN(stream->readbuf_type == IS_UNICODE, stream->readpos);
		avail = stream->writepos - stream->readpos;
	} else {
		readptr = zbuf.s;
		avail = buf_len;
	}

	if (stream->flags & PHP_STREAM_FLAG_DETECT_EOL) {
		if (stream->readbuf_type == IS_UNICODE) {
			cr = (char*)u_memchr((UChar*)readptr, '\r', avail);
			lf = (char*)u_memchr((UChar*)readptr, '\n', avail);
		} else {
			cr = memchr(readptr, '\r', avail);
			lf = memchr(readptr, '\n', avail);
		}

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
		eol = (stream->readbuf_type == IS_UNICODE) ? u_memchr((UChar*)readptr, '\r', avail) : memchr(readptr, '\r', avail);
	} else {
		/* unix (and dos) line endings */
		eol = (stream->readbuf_type == IS_UNICODE) ? u_memchr((UChar*)readptr, '\n', avail) : memchr(readptr, '\n', avail);
	}

	return (void*)eol;
}

/* If buf == NULL, the buffer will be allocated automatically and will be of an
 * appropriate length to hold the line, regardless of the line length, memory
 * permitting -- returned string will be up to (maxlen-1) units of (maxchars) characters, last byte holding terminating NULL
 * Like php_stream_read(), this will (UTODO) treat unicode streams as ugly binary data (use with caution) */
PHPAPI void *_php_stream_get_line(php_stream *stream, int buf_type, zstr buf, size_t maxlen, size_t maxchars, size_t *returned_len TSRMLS_DC)
{
	size_t avail = 0;
	size_t current_buf_size = 0;
	size_t total_copied = 0;
	int grow_mode = 0;
	int is_unicode = stream->readbuf_type == IS_UNICODE;
	int split_surrogate = 0;
	zstr bufstart = buf;

	if ((buf_type == IS_STRING && is_unicode) ||
		(buf_type == IS_UNICODE && !is_unicode)) {
		/* UTODO: Allow sloppy conversion */
		return NULL;
	}

	if (buf.v == NULL) {
		grow_mode = 1;
	} else if (maxlen == 0) {
		return NULL;
	}

	/*
	 * If the underlying stream operations block when no new data is readable,
	 * we need to take extra precautions.
	 *
	 * If there is buffered data available, we check for a EOL. If it exists,
	 * we pass the data immediately back to the caller. This saves a call
	 * to the read implementation and will not block where blocking
	 * is not necessary at all.
	 *
	 * If the stream buffer contains more data than the caller requested,
	 * we can also avoid that costly step and simply return that data.
	 */

	for (;;) {
		avail = stream->writepos - stream->readpos;

		if (!split_surrogate && avail > 0) {
			size_t cpysz = avail;
			zstr readptr;
			int done = 0;

			if (is_unicode) {
				UChar *eol;
				readptr.u = stream->readbuf.u + stream->readpos;

				eol = php_stream_locate_eol(stream, NULL_ZSTR, 0 TSRMLS_CC);
				if (eol) {
					cpysz = eol - readptr.u + 1;
					done = 1;
				}

				if (U16_IS_SURROGATE(readptr.u[cpysz - 1]) &&
					U16_IS_SURROGATE_LEAD(readptr.u[cpysz - 1])) {
					/* Don't orphan */
					cpysz--;
					if (!cpysz) {
						/* Force the loop to land on fill_read_buffer */
						split_surrogate = 1; /* must specifically be 1 */
						continue;
					}
				}
			} else {
				char *eol;
				readptr.s = stream->readbuf.s + stream->readpos;
				eol = php_stream_locate_eol(stream, NULL_ZSTR, 0 TSRMLS_CC);
				if (eol) {
					cpysz = eol - readptr.s + 1;
					done = 1;
				}
			}

			if (grow_mode) {
				/* allow room for a NUL. If this realloc is really a realloc
				 * (ie: second time around), we get an extra byte. In most
				 * cases, with the default chunk size of 8K, we will only
				 * incur that overhead once.  When people have lines longer
				 * than 8K, we waste 1 byte per additional 8K or so.
				 * That seems acceptable to me, to avoid making this code
				 * hard to follow */
				bufstart.s = erealloc(bufstart.s, PS_ULEN(stream->readbuf_type == IS_UNICODE, current_buf_size + cpysz + 1));
				buf.s = bufstart.s + PS_ULEN(stream->readbuf_type == IS_UNICODE, total_copied);
				current_buf_size += cpysz + 1;
			} else {
				if (cpysz >= maxlen - 1) {
					cpysz = maxlen - 1;
					done = 1;
				}
			}

			if (is_unicode) {
				if (maxchars) {
					int ulen = u_countChar32(readptr.u, cpysz);
	
					if (ulen > maxchars) {
						int32_t i = 0;
	
						ulen = maxchars;
						U16_FWD_N(readptr.u, i, cpysz, ulen);
						cpysz = i;
					}
					maxchars -= ulen;
				}
				memcpy(buf.u, readptr.u, UBYTES(cpysz));
				buf.u += cpysz;
			} else {
				if (maxchars && cpysz > maxchars) {
					cpysz = maxchars;
				}
				memcpy(buf.s, readptr.s, cpysz);
				buf.s += cpysz;
			}

			stream->position += cpysz;
			stream->readpos += cpysz;
			maxlen -= cpysz;
			total_copied += cpysz;

			if (done) {
				break;
			}
		} else if (stream->eof) {
			break;
		} else {
			/* XXX: Should be fine to always read chunk_size */
			size_t toread;
			
			if (grow_mode) {
				toread = stream->chunk_size;
			} else {
				toread = maxlen - 1;
				if (toread > stream->chunk_size) {
					toread = stream->chunk_size;
				}
			}

			php_stream_fill_read_buffer(stream, toread TSRMLS_CC);

			if (stream->writepos - stream->readpos <= split_surrogate) {
				break;
			}
			split_surrogate = 0;
		}
	}

	if (returned_len) {
		*returned_len = total_copied;
	}

	if (total_copied == 0) {
		assert(stream->eof || !grow_mode ||
				(grow_mode && bufstart.v == NULL));
		return NULL;
	}

	if (is_unicode) {
		buf.u[0] = 0;
	} else {
		buf.s[0] = 0;
	}

	return bufstart.s;
}

PHPAPI char *php_stream_get_record(php_stream *stream, size_t maxlen, size_t *returned_len, char *delim, size_t delim_len TSRMLS_DC)
{
	char *e, *buf;
	size_t toread;
	int skip = 0;

	php_stream_fill_read_buffer(stream, maxlen TSRMLS_CC);

	if (delim_len == 0 || !delim) {
		toread = maxlen;
	} else {
		size_t seek_len;

		seek_len = stream->writepos - stream->readpos;
		if (seek_len > maxlen) {
			seek_len = maxlen;
		}

		if (delim_len == 1) {
			e = memchr(stream->readbuf.s + stream->readpos, *delim, seek_len);
		} else {
			e = php_memnstr(stream->readbuf.s + stream->readpos, delim, delim_len, (stream->readbuf.s + stream->readpos + seek_len));
		}

		if (!e) {
			if (seek_len < maxlen && !stream->eof) {
				return NULL;
			}
			toread = maxlen;
		} else {
			toread = e - stream->readbuf.s - stream->readpos;
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
}

PHPAPI UChar *php_stream_get_record_unicode(php_stream *stream, size_t maxlen, size_t maxchars, size_t *returned_len, UChar *delim, size_t delim_len TSRMLS_DC)
{
	UChar *e, *buf;
	size_t toread;
	int skip = 0;

	if (stream->readbuf_type != IS_UNICODE) {
		return NULL;
	}

	php_stream_fill_read_buffer(stream, maxlen TSRMLS_CC);

	if (delim_len == 0 || !delim) {
		toread = maxlen;
	} else {
		size_t seek_len;

		seek_len = stream->writepos - stream->readpos;
		if (seek_len > maxlen) {
			seek_len = maxlen;
		}

		if (delim_len == 1) {
			e = u_memchr(stream->readbuf.u + stream->readpos, *delim, seek_len);
		} else {
			e = u_strFindFirst(stream->readbuf.u + stream->readpos, stream->readbuf.u + stream->readpos + seek_len, delim, delim_len);
		}

		if (!e) {
			toread = maxlen;
		} else {
			toread = e - (stream->readbuf.u + stream->readpos);
			skip = 1;
		}
	}

	if (toread > maxlen && maxlen > 0) {
		toread = maxlen;
	}

	if (U16_IS_SURROGATE(stream->readbuf.u[stream->readpos + toread - 1]) &&
		U16_IS_SURROGATE_LEAD(stream->readbuf.u[stream->readpos + toread - 1])) {
		/* Don't orphan */
		toread--;
	}

	if (maxchars > 0) {
		size_t ulen = u_countChar32(stream->readbuf.u + stream->readpos, toread);

		if (maxchars > ulen) {
			int i = 0;
			UChar *s = stream->readbuf.u + stream->readpos;

			U16_FWD_N(s, i, toread, maxchars);
			toread = i;
		}
	}

	buf = eumalloc(toread + 1);
	*returned_len = php_stream_read_unicode(stream, buf, toread);

	if (*returned_len >= 0) {
		if (skip) {
			stream->readpos += delim_len;
			stream->position += delim_len;
		}
		buf[*returned_len] = 0;
		return buf;
	} else {
		efree(buf);
		return NULL;
	}
}

/* Writes a buffer directly to a stream, using multiple of the chunk size */
static size_t _php_stream_write_buffer(php_stream *stream, int buf_type, zstr buf, int buflen TSRMLS_DC)
{
	size_t didwrite = 0, towrite, justwrote, shouldwrite;
	char *freeme = NULL;
	void *buf_orig = buf.v;
	int buflen_orig = buflen, conv_err = 0;

 	/* if we have a seekable stream we need to ensure that data is written at the
 	 * current stream->position. This means invalidating the read buffer and then
	 * performing a low-level seek */
	if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0 && stream->readpos != stream->writepos) {
		stream->readpos = stream->writepos = 0;

		stream->ops->seek(stream, stream->position, SEEK_SET, &stream->position TSRMLS_CC);
	}

	if (buf_type == IS_UNICODE) {
		int len, num_conv, ulen = u_countChar32(buf.u, buflen);
		char *str;
		UErrorCode status = U_ZERO_ERROR;

		/* Use runtime_encoding to map to binary */
		num_conv = zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &str, &len, buf.u, buflen, &status);
		if (U_FAILURE(status)) {
			zend_raise_conversion_error_ex("Unable to convert data to be written", ZEND_U_CONVERTER(UG(runtime_encoding_conv)),
									ZEND_FROM_UNICODE, num_conv TSRMLS_CC);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%d character unicode buffer downcoded for binary stream runtime_encoding", ulen);
		}

		if (num_conv < buflen) {
			conv_err = 1;
		}

		freeme = buf.s = str;
		buflen = len;
	}

	shouldwrite = buflen;

	while (buflen > 0) {
		towrite = buflen;
		if (towrite > stream->chunk_size) {
			towrite = stream->chunk_size;
		}

		justwrote = stream->ops->write(stream, buf.s, towrite TSRMLS_CC);

		/* convert justwrote to an integer, since normally it is unsigned */
		if ((int)justwrote > 0) {
			buf.s += justwrote;
			buflen -= justwrote;
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

	if (buf_type == IS_UNICODE) {
		/* Map bytes written back to UChars written */

		if (shouldwrite == didwrite && !conv_err) {
			/* wrote it all */
			didwrite = buflen_orig;
		} else {
			/* Figure out how didwrite corresponds to the input buffer */
			char *tmp = emalloc(didwrite + 1), *t = tmp;
			const UChar *s = buf_orig;
			UErrorCode status = U_ZERO_ERROR;

			ucnv_resetFromUnicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)));
			ucnv_fromUnicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &t, t + didwrite, &s, s + buflen_orig, NULL, TRUE, &status);

			didwrite = s - ((UChar*)buf_orig);
			efree(tmp);
		}
	}

	if (freeme) {
		efree(freeme);
	}

	return didwrite;
}

/* push some data through the write filter chain.
 * buf may be NULL, if flags are set to indicate a flush.
 * This may trigger a real write to the stream.
 * Returns the number of bytes consumed from buf by the first filter in the chain.
 * */
static size_t _php_stream_write_filtered(php_stream *stream, int buf_type, zstr buf, int count, int flags TSRMLS_DC)
{
	size_t consumed = 0;
	php_stream_bucket *bucket;
	php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
	php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out, *brig_swap;
	php_stream_filter_status_t status = PSFS_ERR_FATAL;
	php_stream_filter *filter;

	if (buf.v) {
		if (buf_type == IS_UNICODE) {
			bucket = php_stream_bucket_new_unicode(stream, buf.u, count, 0, 0 TSRMLS_CC);
		} else {
			bucket = php_stream_bucket_new(stream, buf.s, count, 0, 0 TSRMLS_CC);
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
				_php_stream_write_buffer(stream, bucket->buf_type, bucket->buf, bucket->buflen TSRMLS_CC);
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

PHPAPI int _php_stream_flush(php_stream *stream, int closing TSRMLS_DC)
{
	int ret = 0;

	if (stream->writefilters.head) {
		_php_stream_write_filtered(stream, IS_STRING, NULL_ZSTR, 0, closing ? PSFS_FLAG_FLUSH_CLOSE : PSFS_FLAG_FLUSH_INC  TSRMLS_CC);
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
		return _php_stream_write_filtered(stream, IS_STRING, ZSTR((void*)buf), count, PSFS_FLAG_NORMAL TSRMLS_CC);
	} else {
		return _php_stream_write_buffer(stream, IS_STRING, ZSTR((void*)buf), count TSRMLS_CC);
	}
}

PHPAPI size_t _php_stream_write_unicode(php_stream *stream, const UChar *buf, int count TSRMLS_DC)
{
	int32_t ret;
	
	if (buf == NULL || count == 0 || stream->ops->write == NULL) {
		return 0;
	}

	if (stream->writefilters.head) {
		ret = _php_stream_write_filtered(stream, IS_UNICODE, ZSTR((void*)buf), count, PSFS_FLAG_NORMAL TSRMLS_CC);
	} else {
		ret = _php_stream_write_buffer(stream, IS_UNICODE, ZSTR((void*)buf), count TSRMLS_CC);
	}

	return ret;
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
			case SEEK_CUR:
				if (offset > 0 && offset < stream->writepos - stream->readpos) {
					stream->readpos += offset;
					stream->position += offset;
					stream->eof = 0;
					return 0;
				}
				break;
			case SEEK_SET:
				if (offset > stream->position &&
						offset < stream->position + stream->writepos - stream->readpos) {
					stream->readpos += offset - stream->position;
					stream->position = offset;
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
			stream->readpos = stream->writepos = 0;

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
	size_t count = 0;

	if (php_stream_mmap_possible(stream)) {
		/* mmap_possible == non-filtered stream == binary stream */
		char *p;
		size_t mapped;

		p = php_stream_mmap_range(stream, php_stream_tell(stream), PHP_STREAM_COPY_ALL, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped);

		if (p) {
			PHPWRITE(p, mapped);

			php_stream_mmap_unmap(stream);

			return mapped;
		}
	}

	if (stream->readbuf_type == IS_UNICODE) {
		UChar inbuf_start[8192];
		UConverter *conv = ZEND_U_CONVERTER(UG(output_encoding_conv));
		int outbuflen = UCNV_GET_MAX_BYTES_FOR_STRING(8192, ucnv_getMaxCharSize(conv));
		char *outbuf_start = emalloc(outbuflen + 1);
		int b;

		ucnv_resetFromUnicode(conv);

		while ((b = php_stream_read_unicode(stream, inbuf_start, 8192)) > 0) {
			char *outbuf = outbuf_start;
			const UChar *inbuf = inbuf_start;
			UErrorCode status = U_ZERO_ERROR;
			int len;

			ucnv_fromUnicode(conv, &outbuf, outbuf + outbuflen, &inbuf, inbuf + b, NULL, TRUE, &status);
			len = u_countChar32(inbuf_start, inbuf - inbuf_start);
			if (U_FAILURE(status)) {
				/* Memory overflow isn't a problem becuase MAX_BYTES_FOR_STRING was allocated,
				   anything else is a more serious problem */
				zend_raise_conversion_error_ex("Unable to convert Unicode character using output_encoding, at least one character was lost",
											   conv, ZEND_FROM_UNICODE, len TSRMLS_CC);
			}
			if (outbuf > outbuf_start) {
				PHPWRITE(outbuf_start, outbuf - outbuf_start);
				count += len;
			}
		}
		efree(outbuf_start);
	} else {
		char buf[8192];
		int b;

		while ((b = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			PHPWRITE(buf, b);
			count += b;
		}
	}

	return count;
}


PHPAPI size_t _php_stream_copy_to_mem_ex(php_stream *src, zend_uchar rettype, void **buf, size_t maxlen, size_t maxchars, int persistent STREAMS_DC TSRMLS_DC)
{
	size_t ret = 0;
	zstr ptr;
	size_t len = 0, max_len;
	int step = CHUNK_SIZE;
	int min_room = CHUNK_SIZE / 4;
	php_stream_statbuf ssbuf;

	if (rettype != src->readbuf_type) {
		/* UTODO: Introduce sloppy buffer conversion */
		return 0;
	}

	if (maxlen == 0) { 
		return 0;
	}

	if (maxlen == PHP_STREAM_COPY_ALL) {
		maxlen = 0;
	}

	if (maxlen > 0) {
		if (rettype == IS_UNICODE) {
			ptr.u = *buf = pemalloc_rel_orig(UBYTES(maxlen + 1), persistent);
			while ((len < maxlen) && !php_stream_eof(src)) {
				int ulen;

				ret = php_stream_read_unicode_ex(src, ptr.u, maxlen - len, maxchars);
				ulen = u_countChar32(ptr.u, ret);
				len += ret;
				ptr.u += ret;
				maxchars -= ret;
			}
			*(ptr.u) = 0;
			return len;
		} else {
			ptr.s = *buf = pemalloc_rel_orig(maxlen + 1, persistent);
			while ((len < maxlen) && !php_stream_eof(src)) {
				ret = php_stream_read(src, ptr.s, maxlen - len);
				len += ret;
				ptr.s += ret;
			}
			*(ptr.s) = 0;
			return len;
		}
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

	if (rettype == IS_UNICODE) {
		ptr.u = *buf = pemalloc_rel_orig(UBYTES(max_len + 1), persistent);

		while((ret = php_stream_read_unicode_ex(src, ptr.u, max_len - len, maxchars)))	{
			int ulen = u_countChar32(ptr.u, ret);

			len += ret;
			if (len + min_room >= max_len) {
				*buf = perealloc_rel_orig(*buf, UBYTES(max_len + step), persistent);
				max_len += step;
				ptr.u = ((UChar*)(*buf)) + len;
			} else {
				ptr.u += ret;
			}
			maxchars -= ulen;
		}
	} else {
		ptr.s = *buf = pemalloc_rel_orig(max_len + 1, persistent);

		while((ret = php_stream_read(src, ptr.s, max_len - len)))	{
			len += ret;
			if (len + min_room >= max_len) {
				*buf = perealloc_rel_orig(*buf, max_len + step, persistent);
				max_len += step;
				ptr.s = ((char*)(*buf)) + len;
			} else {
				ptr.s += ret;
			}
		}
	}

	if (len) {
		if (rettype == IS_UNICODE) {
			if ((max_len - len) > (2 * step)) {
				*buf = perealloc_rel_orig(*buf, UBYTES(len + 1), persistent);
			}
			((UChar*)(*buf))[len] = 0;
		} else {
			if ((max_len - len) > (2 * step)) {
				*buf = perealloc_rel_orig(*buf, len + 1, persistent);
			}
			((char*)(*buf))[len] = 0;
		}
	} else {
		pefree(*buf, persistent);
		*buf = NULL;
	}
	return len;
}

/* Designed for copying UChars (taking into account both maxlen and maxchars) */
PHPAPI size_t _php_stream_ucopy_to_stream(php_stream *src, php_stream *dest, size_t maxlen, size_t maxchars STREAMS_DC TSRMLS_DC)
{
	size_t haveread = 0;
	php_stream_statbuf ssbuf;

	if (src->readbuf_type == IS_STRING) {
		/* Called incorrectly, don't do that. */
		return _php_stream_copy_to_stream(src, dest, maxlen STREAMS_CC TSRMLS_CC);
	}

	if (maxlen == 0 || maxchars == 0) {
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

	while(1) {
		UChar buf[CHUNK_SIZE];
		size_t readchunk = CHUNK_SIZE;
		size_t didread;

		if (maxlen && (maxlen - haveread) < readchunk) {
			readchunk = maxlen - haveread;
		}

		didread = php_stream_read_unicode_ex(src, buf, readchunk, maxchars);

		if (didread) {
			/* extra paranoid */
			size_t didwrite, towrite;
			UChar *writeptr;

			if (maxchars > 0) {
				/* Determine number of chars in this buf */
				maxchars -= u_countChar32(buf, didread);
			}

			towrite = didread;
			writeptr = buf;
			haveread += didread;

			while(towrite) {
				didwrite = php_stream_write_unicode(dest, writeptr, towrite);
				if (didwrite == 0) {
					return 0;	/* error */
				}

				towrite -= didwrite;
				writeptr += didwrite;
			}
		} else {
			return haveread;
		}

		if (maxchars == 0 || maxlen - haveread == 0) {
			break;
		}
	}

	return haveread;
}

/* Optimized for copying octets from source stream */
PHPAPI size_t _php_stream_copy_to_stream(php_stream *src, php_stream *dest, size_t maxlen STREAMS_DC TSRMLS_DC)
{
	size_t haveread = 0;
	php_stream_statbuf ssbuf;

	if (src->readbuf_type == IS_UNICODE) {
		/* Called incorrectly, don't do that. */
		return _php_stream_ucopy_to_stream(src, dest, maxlen, -1 STREAMS_CC TSRMLS_CC);
	}

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
			mapped = php_stream_write(dest, p, mapped);

			php_stream_mmap_unmap(src);

			return mapped;
		}
	}

	while(1) {
		char buf[CHUNK_SIZE];
		size_t readchunk = sizeof(buf);
		size_t didread;

		if (maxlen && (maxlen - haveread) < readchunk) {
			readchunk = maxlen - haveread;
		}

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
			return haveread;
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

	return zend_hash_add(&url_stream_wrappers_hash, protocol, protocol_len + 1, &wrapper, sizeof(wrapper), NULL);
}

PHPAPI int php_unregister_url_stream_wrapper(char *protocol TSRMLS_DC)
{
	return zend_hash_del(&url_stream_wrappers_hash, protocol, strlen(protocol) + 1);
}

static void clone_wrapper_hash(TSRMLS_D)
{
	php_stream_wrapper *tmp;

	ALLOC_HASHTABLE(FG(stream_wrappers));
	zend_hash_init(FG(stream_wrappers), zend_hash_num_elements(&url_stream_wrappers_hash), NULL, NULL, 1);
	zend_hash_copy(FG(stream_wrappers), &url_stream_wrappers_hash, NULL, &tmp, sizeof(tmp));
}

/* API for registering VOLATILE wrappers */
PHPAPI int php_register_url_stream_wrapper_volatile(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC)
{
	int protocol_len = strlen(protocol);

	if (php_stream_wrapper_scheme_validate(protocol, protocol_len) == FAILURE) {
		return FAILURE;
	}

	if (!FG(stream_wrappers)) {
		clone_wrapper_hash(TSRMLS_C);
	}

	return zend_hash_add(FG(stream_wrappers), protocol, protocol_len + 1, &wrapper, sizeof(wrapper), NULL);
}

PHPAPI int php_unregister_url_stream_wrapper_volatile(char *protocol TSRMLS_DC)
{
	if (!FG(stream_wrappers)) {
		clone_wrapper_hash(TSRMLS_C);
	}

	return zend_hash_del(FG(stream_wrappers), protocol, strlen(protocol) + 1);
}
/* }}} */

/* {{{ php_stream_locate_url_wrapper */
PHPAPI php_stream_wrapper *php_stream_locate_url_wrapper(const char *path, char **path_for_open, int options TSRMLS_DC)
{
	HashTable *wrapper_hash = (FG(stream_wrappers) ? FG(stream_wrappers) : &url_stream_wrappers_hash);
	php_stream_wrapper **wrapperpp = NULL;
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

	if ((*p == ':') && (n > 1) && (!strncmp("//", p+1, 2) || (n == 4 && !memcmp("data:", path, 5)))) {
		protocol = path;
	} else if (n == 5 && strncasecmp(path, "zlib:", 5) == 0) {
		/* BC with older php scripts and zlib wrapper */
		protocol = "compress.zlib";
		n = 13;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Use of \"zlib:\" wrapper is deprecated; please use \"compress.zlib://\" instead");
	}

	if (protocol) {
		char *tmp = estrndup(protocol, n);
		if (FAILURE == zend_hash_find(wrapper_hash, (char*)tmp, n + 1, (void**)&wrapperpp)) {
			php_strtolower(tmp, n);
			if (FAILURE == zend_hash_find(wrapper_hash, (char*)tmp, n + 1, (void**)&wrapperpp)) {
				char wrapper_name[32];

				if (n >= sizeof(wrapper_name)) {
					n = sizeof(wrapper_name) - 1;
				}
				PHP_STRLCPY(wrapper_name, protocol, sizeof(wrapper_name), n);

				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find the wrapper \"%s\" - did you forget to enable it when you configured PHP?", wrapper_name);

				wrapperpp = NULL;
				protocol = NULL;
			}
		}
		efree(tmp);
	}
	/* TODO: curl based streams probably support file:// properly */
	if (!protocol || !strncasecmp(protocol, "file", n))	{
		/* fall back on regular file access */
		php_stream_wrapper *plain_files_wrapper = &php_plain_files_wrapper;

		if (protocol) {
			int localhost = 0;

			if (!strncasecmp(path, "file://localhost/", 17)) {
				localhost = 1;
			}

#ifdef PHP_WIN32
			if (localhost == 0 && path[n+3] != '\0' && path[n+3] != '/' && path[n+4] != ':')	{
#else
			if (localhost == 0 && path[n+3] != '\0' && path[n+3] != '/') {
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

		/* The file:// wrapper may have been disabled/overridden */
 		if (FG(stream_wrappers)) {
			if (zend_hash_find(wrapper_hash, "file", sizeof("file"), (void**)&wrapperpp) == FAILURE) {
				if (options & REPORT_ERRORS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Plainfiles wrapper disabled");
				}
				return NULL;
			} else {
				/* Handles overridden plain files wrapper */
				plain_files_wrapper = *wrapperpp;
			}
		}

		if (!php_stream_allow_url_fopen("file", sizeof("file") - 1) ||
			((options & STREAM_OPEN_FOR_INCLUDE) && !php_stream_allow_url_include("file", sizeof("file") - 1)) ) {
			if (options & REPORT_ERRORS) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "file:// wrapper is disabled in the server configuration");
			}
			return NULL;
		}
		
		return plain_files_wrapper;
	}

	if (((options & STREAM_DISABLE_URL_PROTECTION) == 0) &&
	    (!php_stream_allow_url_fopen(protocol, n) ||
		 ((options & STREAM_OPEN_FOR_INCLUDE) && !php_stream_allow_url_include(protocol, n)))) {
		if (options & REPORT_ERRORS) {
			/* protocol[n] probably isn't '\0' */
			char *protocol_dup = estrndup(protocol, n);
			if (!php_stream_allow_url_fopen(protocol, n)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s:// wrapper is disabled in the server configuration by allow_url_fopen=0", protocol_dup);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s:// wrapper is disabled in the server configuration by allow_url_include=0", protocol_dup);
			}
			efree(protocol_dup);
		}
		return NULL;
	}

	return *wrapperpp;
}
/* }}} */

/* {{{ _php_stream_mkdir
 */
PHPAPI int _php_stream_mkdir(char *path, int mode, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;

	wrapper = php_stream_locate_url_wrapper(path, NULL, 0 TSRMLS_CC);
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

	wrapper = php_stream_locate_url_wrapper(path, NULL, 0 TSRMLS_CC);
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

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, 0 TSRMLS_CC);
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
			stream->flags |= PHP_STREAM_FLAG_NO_BUFFER | PHP_STREAM_FLAG_IS_DIR;
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

PHPAPI php_stream *_php_stream_u_opendir(zend_uchar type, zstr path, int path_len, int options, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	char *filename;
	int filename_len;
	php_stream *stream;

	if (type == IS_STRING) {
		return php_stream_opendir(path.s, options, context);
	}

	/* type == IS_UNICODE */
	if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, path.u, path_len, options, context)) {
		return NULL;
	}

	stream = php_stream_opendir(filename, options, context);
	efree(filename);
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

/* {{{ php_stream_fix_encoding
 * Sets read/write encoding on a stream based on the fopen mode, context options, and INI setting */
PHPAPI void php_stream_fix_encoding(php_stream *stream, const char *mode, php_stream_context *context TSRMLS_DC)
{
	/* Output encoding on text mode streams defaults to utf8 unless specified in context parameter */
	if (stream && strchr(mode, 't') && UG(unicode)) {
		/* Only apply implicit unicode.to. filter if the wrapper didn't do it for us */
		if ((php_stream_filter_product(&stream->writefilters, IS_UNICODE) == IS_UNICODE) && 
			(strchr(mode, 'w') || strchr(mode, 'a') || strchr(mode, '+'))) {
			char *encoding = (context && context->output_encoding) ? context->output_encoding : UG(stream_encoding);

			/* UTODO: (Maybe?) Allow overriding the default error handlers on a per-stream basis via context params */
			php_stream_encoding_apply(stream, 1, encoding, UG(from_error_mode), UG(from_subst_char));
		}

		/* Only apply implicit unicode.from. filter if the wrapper didn't do it for us */
		if ((stream->readbuf_type == IS_STRING) && (strchr(mode, 'r') || strchr(mode, '+'))) {
			char *encoding = (context && context->input_encoding) ? context->input_encoding : UG(stream_encoding);

			/* UTODO: (Maybe?) Allow overriding the default error handlers on a per-stream basis via context params */
			php_stream_encoding_apply(stream, 0, encoding, UG(to_error_mode), NULL);
		}
	}
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
	char *resolved_path = NULL;
	char *copy_of_path = NULL;
	char implicit_mode[16];
	int modelen = strlen(mode);
	
	if (opened_path) {
		*opened_path = NULL;
	}

	if (!path || !*path) {
		return NULL;
	}

	if (options & USE_PATH) {
		resolved_path = zend_resolve_path(path, strlen(path) TSRMLS_CC);
		if (resolved_path) {
			path = resolved_path;
			/* we've found this file, don't re-check include_path or run realpath */
			options |= STREAM_ASSUME_REALPATH;
			options &= ~USE_PATH;
		}
	}

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);
	if (options & STREAM_USE_URL && (!wrapper || !wrapper->is_url)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This function may only be used against URLs");
		if (resolved_path) {
			efree(resolved_path);
		}
		return NULL;
	}

	strlcpy(implicit_mode, mode, sizeof(implicit_mode));
	if (context && context->default_mode && modelen < 15 && !strchr(mode, 't') && !strchr(mode, 'b')) {
		if (context->default_mode & PHP_FILE_BINARY) {
			implicit_mode[modelen++] = 'b';
		} else if (context->default_mode & PHP_FILE_TEXT) {
			implicit_mode[modelen++] = 't';
		}
		implicit_mode[modelen] = '\0';
	}

	if (wrapper) {
		if (!wrapper->wops->stream_opener) {
			php_stream_wrapper_log_error(wrapper, options ^ REPORT_ERRORS TSRMLS_CC,
					"wrapper does not support stream open");
		} else {
			/* refcount++ to make sure the context doesn't get destroyed 
			 * if open() fails and stream is closed */
			if (context) {
				zend_list_addref(context->rsrc_id);
			}

			stream = wrapper->wops->stream_opener(wrapper,
				path_to_open, implicit_mode, options ^ REPORT_ERRORS,
				opened_path, context STREAMS_REL_CC TSRMLS_CC);

			/* if open() succeeded and context was not used, do refcount--
			 * XXX if a wrapper didn't actually use context (no way to know that)
			 * and open() failed, refcount will stay increased */
			if (context && stream && !stream->context) {
				zend_list_delete(context->rsrc_id);
			}
		}

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
			strlcpy(stream->mode, implicit_mode, sizeof(stream->mode));
		}
	}

	if (stream) {
		if (opened_path && !*opened_path && resolved_path) {
			*opened_path = resolved_path;
			resolved_path = NULL;
		}
		if (stream->orig_path) {
			pefree(stream->orig_path, persistent);
		}
		copy_of_path = pestrdup(path, persistent);
		stream->orig_path = copy_of_path;
#if ZEND_DEBUG
		stream->open_filename = __zend_orig_filename ? __zend_orig_filename : __zend_filename;
		stream->open_lineno = __zend_orig_lineno ? __zend_orig_lineno : __zend_lineno;
#endif
	}

	if (stream != NULL && (options & STREAM_MUST_SEEK)) {
		php_stream *newstream;

		switch(php_stream_make_seekable_rel(stream, &newstream,
					(options & STREAM_WILL_CAST)
						? PHP_STREAM_PREFER_STDIO : PHP_STREAM_NO_PREFERENCE)) {
			case PHP_STREAM_UNCHANGED:
				if (resolved_path) {
					efree(resolved_path);
				}
				return stream;
			case PHP_STREAM_RELEASED:
				if (newstream->orig_path) {
					pefree(newstream->orig_path, persistent);
				}
				newstream->orig_path = pestrdup(path, persistent);
				if (resolved_path) {
					efree(resolved_path);
				}
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


	php_stream_fix_encoding(stream, implicit_mode, context TSRMLS_CC);

	if (stream == NULL && (options & REPORT_ERRORS)) {
		php_stream_display_wrapper_errors(wrapper, path, "failed to open stream" TSRMLS_CC);
		if (opened_path && *opened_path) {
			efree(*opened_path);
			*opened_path = NULL;
		}
	}
	php_stream_tidy_wrapper_error_log(wrapper TSRMLS_CC);
#if ZEND_DEBUG
	if (stream == NULL && copy_of_path != NULL) {
		pefree(copy_of_path, persistent);
	}
#endif

	if (resolved_path) {
		efree(resolved_path);
	}
	return stream;
}
/* }}} */

/* {{{ _php_stream_u_open_wrapper */
PHPAPI php_stream *_php_stream_u_open_wrapper(zend_uchar type, zstr path, int path_len,
			char *mode, int options, zstr *opened_path, int *opened_path_len,
			php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	char *filename = NULL;
	int filename_len;

	if (opened_path) {
		opened_path->v = NULL;
	}
	if (opened_path_len) {
		*opened_path_len = 0;
	}

	if (type == IS_STRING) {
		stream = php_stream_open_wrapper_ex(path.s, mode, options, (char**)opened_path, context);

		if (opened_path_len && opened_path && opened_path->s) {
			*opened_path_len = strlen(opened_path->s);
		}

		return stream;
	}

	/* type == IS_UNICODE */
	if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, path.u, path_len, options, context)) {
		return NULL;
	}

	stream = php_stream_open_wrapper_ex(filename, mode, options, (char**)opened_path, context);
	efree(filename);

	if (opened_path && opened_path->s) {
		UChar *upath;
		int upath_len;

		if (SUCCESS == php_stream_path_decode(NULL, &upath, &upath_len, opened_path->s, strlen(opened_path->s), options, context)) {
			efree(opened_path->s);
			opened_path->u = upath;
			if (opened_path_len) {
				*opened_path_len = upath_len;
			}
		} else {
			/* Shouldn't happen */
			efree(opened_path->s);
			opened_path->s = NULL;
		}
	}

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

	if (FAILURE == zend_rt_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash)) {
		return FAILURE;
	}
	return zend_rt_hash_find(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)optionvalue);
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

	if (FAILURE == zend_rt_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash)) {
		MAKE_STD_ZVAL(category);
		array_init(category);
		if (FAILURE == zend_rt_hash_update(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&category, sizeof(zval *), NULL)) {
			return FAILURE;
		}

		wrapperhash = &category;
	}
	return zend_rt_hash_update(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)&copied_val, sizeof(zval *), NULL);
}

PHPAPI int php_stream_context_get_link(php_stream_context *context,
        const char *hostent, php_stream **stream)
{
	php_stream **pstream;

	if (!stream || !hostent || !context || !(context->links)) {
		return FAILURE;
	}
	if (SUCCESS == zend_rt_hash_find(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1, (void**)&pstream)) {
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
		return zend_rt_hash_del(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1);
	}
	return zend_rt_hash_update(Z_ARRVAL_P(context->links), (char*)hostent, strlen(hostent)+1, (void**)&stream, sizeof(php_stream *), NULL);
}

PHPAPI int php_stream_context_del_link(php_stream_context *context,
        php_stream *stream)
{
	php_stream **pstream;
	zend_uchar type;
	zstr hostent;
	unsigned int hostent_len;
	int ret = SUCCESS;

	if (!context || !context->links || !stream) {
		return FAILURE;
	}

	for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(context->links));
		SUCCESS == zend_hash_get_current_data(Z_ARRVAL_P(context->links), (void**)&pstream);
		zend_hash_move_forward(Z_ARRVAL_P(context->links))) {
		if (*pstream == stream) {
			type = zend_hash_get_current_key_ex(Z_ARRVAL_P(context->links), &hostent, &hostent_len, NULL, 0, NULL);
			if (type == HASH_KEY_IS_STRING || type == HASH_KEY_IS_UNICODE) {
				if (FAILURE == zend_u_hash_del(Z_ARRVAL_P(context->links), type, hostent, hostent_len)) {
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

	stream = php_stream_opendir(dirname, REPORT_ERRORS, context);
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

/* {{{ php_stream_path_encode
Encode a filepath to the appropriate characterset.
If the wrapper supports its own encoding rules it will be dispatched to wrapper->wops->path_encode()
Otherwise the INI defined filesystem_encoding converter will be used
If wrapper == NULL, the path will be explored to locate the correct wrapper
*/
PHPAPI int _php_stream_path_encode(php_stream_wrapper *wrapper,
				char **pathenc, int *pathenc_len, const UChar *path, int path_len,
				int options, php_stream_context *context TSRMLS_DC)
{
	UErrorCode status = U_ZERO_ERROR;
	int num_conv;

	if (!wrapper) {
		UChar *p;
		U_STRING_DECL(delim, "://", 3);
		int delim_len = 3;

		U_STRING_INIT(delim, "://", 3);

		p = u_strFindFirst(path, path_len, delim, delim_len);
		if (p) {
			char *scheme = NULL;
			int scheme_len = 0;

			/* Convert just the scheme using utf8 in order to look it up in the registry */
			num_conv = zend_unicode_to_string_ex(UG(utf8_conv), &scheme, &scheme_len, path, (p - path) + delim_len, &status);
			if (U_FAILURE(status)) {
				if (options & REPORT_ERRORS) {
					zend_raise_conversion_error_ex("Unable to convert filepath", UG(utf8_conv), ZEND_FROM_UNICODE, num_conv TSRMLS_CC);
				}
				*pathenc = NULL;
				*pathenc_len = 0;

				return FAILURE;
			}
			wrapper = php_stream_locate_url_wrapper(scheme, NULL, options | STREAM_DISABLE_URL_PROTECTION TSRMLS_CC);
			efree(scheme);
			if (!wrapper) {
				*pathenc = NULL;
				*pathenc_len = 0;

				return FAILURE;
			}			
		} else {
			wrapper = &php_plain_files_wrapper;
		}
	}

	if (wrapper->wops->path_encode) {
		if (wrapper->wops->path_encode(wrapper, pathenc, pathenc_len, path, path_len, options, context TSRMLS_CC) == FAILURE) {
			*pathenc = NULL;
			*pathenc_len = 0;

			return FAILURE;
		}

		return SUCCESS;
	}

	/* Otherwise, fallback on filesystem_encoding */
	status = U_ZERO_ERROR;

	num_conv = zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(filesystem_encoding_conv)),
				pathenc, pathenc_len, path, path_len, &status);
	if (U_FAILURE(status)) {
		if (options & REPORT_ERRORS) {
			zend_raise_conversion_error_ex("Unable to convert filepath", ZEND_U_CONVERTER(UG(filesystem_encoding_conv)),
							ZEND_FROM_UNICODE, num_conv TSRMLS_CC);
		}

		*pathenc = NULL;
		*pathenc_len = 0;

		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */


/* {{{ php_stream_path_decode
Decode a filepath from its character set to unicode
If the wrapper supports its own decoding rules it will be dispatched to wrapper->wops->path_encode()
Otherwise (or if wrapper == NULL) the INI defined filesystem_encoding converter will be used.
*/
PHPAPI int _php_stream_path_decode(php_stream_wrapper *wrapper,
				UChar **pathdec, int *pathdec_len, const char *path, int path_len,
				int options, php_stream_context *context TSRMLS_DC)
{
	int num_conv;
	UErrorCode status = U_ZERO_ERROR;

	if (wrapper && wrapper->wops->path_decode) {
		if (wrapper->wops->path_decode(wrapper, pathdec, pathdec_len, path, path_len, options, context TSRMLS_CC) == FAILURE) {
			*pathdec = NULL;
			*pathdec_len = 0;

			return FAILURE;
		}
		return SUCCESS;
	}

	/* Otherwise fallback on filesystem_encoding */
	num_conv = zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(filesystem_encoding_conv)),
				pathdec, pathdec_len, path, path_len, &status);
	if (U_FAILURE(status)) {
		if (options & REPORT_ERRORS) {
			zend_raise_conversion_error_ex("Unable to convert filepath", ZEND_U_CONVERTER(UG(filesystem_encoding_conv)),
							ZEND_TO_UNICODE, num_conv TSRMLS_CC);
		}

		*pathdec = NULL;
		*pathdec_len = 0;

		return FAILURE;
	}

	return SUCCESS;

}
/* }}} */

/* {{{ allow_url_fopen / allow_url_include Handlers */

PHPAPI int php_stream_wrapper_is_allowed(const char *wrapper, int wrapper_len, const char *setting TSRMLS_DC)
{
	HashTable *wrapper_hash = (FG(stream_wrappers) ? FG(stream_wrappers) : &url_stream_wrappers_hash);
	php_stream_wrapper **wrapperpp;
	int setting_len = setting ? strlen(setting) : 0;
	const char *s = setting, *e = s + setting_len;
	char *wrapper_dup;

	/* BC: allow_url_* == on */
	if (setting_len == 1 && *setting == '*') {
		/* "*" means everything is allowed */
		return 1;
	}

	if (wrapper_len == (sizeof("zlib") - 1) && strncasecmp("zlib", wrapper, sizeof("zlib") - 1) == 0) {
		wrapper = "compress.zlib";
		wrapper_len = sizeof("compress.zlib") - 1;
	}

	wrapper_dup = estrndup(wrapper, wrapper_len);
	php_strtolower(wrapper_dup, wrapper_len);
	if (FAILURE == zend_hash_find(wrapper_hash, wrapper_dup, wrapper_len + 1, (void**)&wrapperpp)) {
		/* Wrapper does not exist, assume disallow */
		efree(wrapper_dup);
		return 0;
	}
	efree(wrapper_dup);

	/* BC: allow_url_* == off */
	if (!setting || !setting_len) {
		/* NULL or empty indicates that only is_url == 0 wrappers are allowed */

		if (wrapper_len == (sizeof("file") - 1) && strncasecmp("file", wrapper, sizeof("file") - 1) == 0) {
			/* file:// is non-url */
			return 1;
		}

		if ((*wrapperpp)->is_url) {
			/* is_url types are disabled, but this is an is_url wrapper, disallow */
			return 0;
		}

		/* Wrapper is not is_url, allow it */
		return 1;
	}

	/* Otherwise, scan list */
	while (s < e) {
		const char *p = php_memnstr((char*)s, ":", 1, (char*)e);

		if (!p) {
			p = e;
		}

		if (wrapper_len == (p - s) &&
			strncasecmp(s, wrapper, p - s) == 0) {
			/* wrapper found in list */
			return 1;
		}

		if ((*wrapperpp)->wops == php_stream_user_wrapper_ops &&
			(sizeof("user") - 1) == (p - s) &&
			strncasecmp(s, "user", sizeof("user") - 1) == 0) {
			/* Wrapper is userspace wrapper and meta-wrapper "user" is enabled */
			return 1;
		}

		s = p + 1;
	}

	return 0;
}

/* allow_url_*_list accepts:
 *
 * 1/on to enable all URL prefixes
 * 0/off to disable all is_url=1 wrappers
 * A colon delimited list of wrappers to allow (wildcards allowed)
 * e.g.    file:gzip:compress.*:php
 */
PHP_INI_MH(OnUpdateAllowUrl)
{
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif
        char **allow = (char **) (base+(size_t) mh_arg1);

	/* BC Enable */
	if ((new_value_length == 1 && *new_value == '1') ||
		(new_value_length == (sizeof("on") - 1) && strncasecmp(new_value, "on", sizeof("on") - 1) == 0) ) {

		if (*allow && strcmp(*allow, "*") == 0) {
			/* Turning on, but that's no change from current, so leave it alone */
			return SUCCESS;
		}
			
		if (stage != PHP_INI_STAGE_STARTUP) {
			/* Not already on, and not in SYSTEM context, fail */
			return FAILURE;
		}

		/* Otherwise, turn on setting */
		if (*allow) {
			free(*allow);
		}

		*allow = zend_strndup("*", 1);

		return SUCCESS;
	}

	/* BC disable */
	if ((new_value_length == 1 && *new_value == '0') ||
		(new_value_length == (sizeof("off") - 1) && strncasecmp(new_value, "off", sizeof("off") - 1) == 0) ) {

		/* Always permit shutting off allowurl settings */
		if (*allow) {
			free(*allow);
		}
		*allow = NULL;

		return SUCCESS;
	}

	/* Specify as list */
	if (stage == PHP_INI_STAGE_STARTUP) {
		/* Always allow new settings in startup stage */
		if (*allow) {
			free(*allow);
		}
		*allow = zend_strndup(new_value, new_value_length);

		return SUCCESS;
	}

	/* In PERDIR/RUNTIME context, do more work to ensure we're only tightening the restriction */

	if (*allow && strcmp(*allow, "*") == 0) {
		/* Currently allowing everying, so whatever we set it to will be more restrictive */
		free(*allow);
		*allow = zend_strndup(new_value, new_value_length);

		return SUCCESS;
	}

	if (!*allow) {
		/* Currently allowing anything with is_url == 0
		 * So long as this list doesn't contain any is_url == 1, allow it
		 */
		HashTable *wrapper_hash = (FG(stream_wrappers) ? FG(stream_wrappers) : &url_stream_wrappers_hash);
		char *s = new_value, *e = new_value + new_value_length;

		while (s < e) {
			php_stream_wrapper **wrapper;
			char *p = php_memnstr(s, ":", 1, e);
			char *scan;
			int scan_len;

			if (!p) {
				p = e;
			}

			/* file:// is never a URL */
			if ( (p - s) == (sizeof("file") - 1) && strncasecmp(s, "file", sizeof("file") - 1) == 0 ) {
				/* file is not a URL */
				s = p + 1;
				continue;
			}

			if ( (p - s) == (sizeof("zlib") - 1) && strncasecmp(s, "zlib", sizeof("zlib") - 1) == 0 ) {
				/* Wastful since we know that compress.zlib is already lower cased, but forgivable */
				scan = estrndup("compress.zlib", sizeof("compress.zlib") - 1);
				scan_len = sizeof("compress.zlib") - 1;
			} else {
				scan = estrndup(s, p - s);;
				scan_len = p - s;
				php_strtolower(scan, scan_len);
			}

			if (FAILURE == zend_hash_find(wrapper_hash, scan, scan_len + 1, (void**) &wrapper)) {
				/* Unknown wrapper, not allowed in this context */
				efree(scan);
				return FAILURE;
			}
			efree(scan);

			if ((*wrapper)->is_url) {
				/* Disallowed is_url wrapper specified when trying to escape is_url == 0 context */
				return FAILURE;
			}

			/* Seems alright so far... */
			s = p+1;
		}

		/* All tests passed, allow it */
		*allow = zend_strndup(new_value, new_value_length);

		return SUCCESS;
	}

	/* The current allows are restricted to a specific list,
	 * Make certain that our new list is a subset of that list
	 */
	{
		char *s = new_value, *e = new_value + new_value_length;

		while (s < e) {
			char *p = php_memnstr(s, ":", 1, e);

			if (!p) {
				p = e;
			}

			if (!php_stream_wrapper_is_allowed(s, p - s, *allow TSRMLS_CC)) {
				/* Current settings don't allow this wrapper, deny */
				return FAILURE;
			}

			s = p + 1;
		}

		free(*allow);
		*allow = zend_strndup(new_value, new_value_length);

		return SUCCESS;
	}
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
