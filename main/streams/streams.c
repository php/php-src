/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

PHPAPI php_stream *php_stream_encloses(php_stream *enclosing, php_stream *enclosed)
{
	php_stream *orig = enclosed->enclosing_stream;

	php_stream_auto_cleanup(enclosed);
	enclosed->enclosing_stream = enclosing;
	return orig;
}

PHPAPI int php_stream_from_persistent_id(const char *persistent_id, php_stream **stream TSRMLS_DC)
{
	zend_rsrc_list_entry *le;

	if (zend_hash_find(&EG(persistent_list), (char*)persistent_id, strlen(persistent_id)+1, (void*) &le) == SUCCESS) {
		if (Z_TYPE_P(le) == le_pstream) {
			if (stream) {
				HashPosition pos;
				zend_rsrc_list_entry *regentry;
				ulong index = -1; /* intentional */

				/* see if this persistent resource already has been loaded to the
				 * regular list; allowing the same resource in several entries in the
				 * regular list causes trouble (see bug #54623) */
				zend_hash_internal_pointer_reset_ex(&EG(regular_list), &pos);
				while (zend_hash_get_current_data_ex(&EG(regular_list),
						(void **)&regentry, &pos) == SUCCESS) {
					if (regentry->ptr == le->ptr) {
						zend_hash_get_current_key_ex(&EG(regular_list), NULL, NULL,
							&index, 0, &pos);
						break;
					}
					zend_hash_move_forward_ex(&EG(regular_list), &pos);
				}
				
				*stream = (php_stream*)le->ptr;
				if (index == -1) { /* not found in regular list */
					le->refcount++;
					(*stream)->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, *stream, le_pstream);
				} else {
					regentry->refcount++;
					(*stream)->rsrc_id = index;
				}
			}
			return PHP_STREAM_PERSISTENT_SUCCESS;
		}
		return PHP_STREAM_PERSISTENT_FAILURE;
	}
	return PHP_STREAM_PERSISTENT_NOT_EXIST;
}

/* }}} */

static zend_llist *php_get_wrapper_errors_list(php_stream_wrapper *wrapper TSRMLS_DC)
{
    zend_llist *list = NULL;
    if (!FG(wrapper_errors)) {
        return NULL;
    } else {
        zend_hash_find(FG(wrapper_errors), (const char*)&wrapper,
            sizeof wrapper, (void**)&list);
        return list;
    }
}

/* {{{ wrapper error reporting */
void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper, const char *path, const char *caption TSRMLS_DC)
{
	char *tmp = estrdup(path);
	char *msg;
	int free_msg = 0;

	if (wrapper) {
		zend_llist *err_list = php_get_wrapper_errors_list(wrapper TSRMLS_CC);
		if (err_list) {
			size_t l = 0;
			int brlen;
			int i;
			int count = zend_llist_count(err_list);
			const char *br;
			const char **err_buf_p;
			zend_llist_position pos;

			if (PG(html_errors)) {
				brlen = 7;
				br = "<br />\n";
			} else {
				brlen = 1;
				br = "\n";
			}

			for (err_buf_p = zend_llist_get_first_ex(err_list, &pos), i = 0;
					err_buf_p;
					err_buf_p = zend_llist_get_next_ex(err_list, &pos), i++) {
				l += strlen(*err_buf_p);
				if (i < count - 1) {
					l += brlen;
				}
			}
			msg = emalloc(l + 1);
			msg[0] = '\0';
			for (err_buf_p = zend_llist_get_first_ex(err_list, &pos), i = 0;
					err_buf_p;
					err_buf_p = zend_llist_get_next_ex(err_list, &pos), i++) {
				strcat(msg, *err_buf_p);
				if (i < count - 1) {
					strcat(msg, br);
				}
			}

			free_msg = 1;
		} else {
			if (wrapper == &php_plain_files_wrapper) {
				msg = strerror(errno); /* TODO: not ts on linux */
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
	if (wrapper && FG(wrapper_errors)) {
		zend_hash_del(FG(wrapper_errors), (const char*)&wrapper, sizeof wrapper);
	}
}

static void wrapper_error_dtor(void *error)
{
	efree(*(char**)error);
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
		zend_llist *list = NULL;
		if (!FG(wrapper_errors)) {
			ALLOC_HASHTABLE(FG(wrapper_errors));
			zend_hash_init(FG(wrapper_errors), 8, NULL,
					(dtor_func_t)zend_llist_destroy, 0);
		} else {
			zend_hash_find(FG(wrapper_errors), (const char*)&wrapper,
				sizeof wrapper, (void**)&list);
		}

		if (!list) {
			zend_llist new_list;
			zend_llist_init(&new_list, sizeof buffer, wrapper_error_dtor, 0);
			zend_hash_update(FG(wrapper_errors), (const char*)&wrapper,
				sizeof wrapper, &new_list, sizeof new_list, (void**)&list);
		}

		/* append to linked list */
		zend_llist_add_element(list, &buffer);
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

	ret->wrapper          = NULL;
	ret->wrapperthis      = NULL;
	ret->wrapperdata      = NULL;
	ret->stdiocast        = NULL;
	ret->orig_path        = NULL;
	ret->context          = NULL;
	ret->readbuf          = NULL;
	ret->enclosing_stream = NULL;

	return ret;
}
/* }}} */

PHPAPI int _php_stream_free_enclosed(php_stream *stream_enclosed, int close_options TSRMLS_DC) /* {{{ */
{
	return _php_stream_free(stream_enclosed,
		close_options | PHP_STREAM_FREE_IGNORE_ENCLOSING TSRMLS_CC);
}
/* }}} */

#if STREAM_DEBUG
static const char *_php_stream_pretty_free_options(int close_options, char *out)
{
	if (close_options & PHP_STREAM_FREE_CALL_DTOR)
		strcat(out, "CALL_DTOR, ");
	if (close_options & PHP_STREAM_FREE_RELEASE_STREAM)
		strcat(out, "RELEASE_STREAM, ");
	if (close_options & PHP_STREAM_FREE_PRESERVE_HANDLE)
		strcat(out, "PREVERSE_HANDLE, ");
	if (close_options & PHP_STREAM_FREE_RSRC_DTOR)
		strcat(out, "RSRC_DTOR, ");
	if (close_options & PHP_STREAM_FREE_PERSISTENT)
		strcat(out, "PERSISTENT, ");
	if (close_options & PHP_STREAM_FREE_IGNORE_ENCLOSING)
		strcat(out, "IGNORE_ENCLOSING, ");
	if (out[0] != '\0')
		out[strlen(out) - 2] = '\0';
	return out;
}
#endif

static int _php_stream_free_persistent(zend_rsrc_list_entry *le, void *pStream TSRMLS_DC)
{
	return le->ptr == pStream;
}


PHPAPI int _php_stream_free(php_stream *stream, int close_options TSRMLS_DC) /* {{{ */
{
	int ret = 1;
	int preserve_handle = close_options & PHP_STREAM_FREE_PRESERVE_HANDLE ? 1 : 0;
	int release_cast = 1;
	php_stream_context *context = NULL;

	/* on an resource list destruction, the context, another resource, may have
	 * already been freed (if it was created after the stream resource), so
	 * don't reference it */
	if (EG(active)) {
		context = stream->context;
	}

	if (stream->flags & PHP_STREAM_FLAG_NO_CLOSE) {
		preserve_handle = 1;
	}

#if STREAM_DEBUG
	{
		char out[200] = "";
		fprintf(stderr, "stream_free: %s:%p[%s] in_free=%d opts=%s\n",
			stream->ops->label, stream, stream->orig_path, stream->in_free, _php_stream_pretty_free_options(close_options, out));
	}
	
#endif

	if (stream->in_free) {
		/* hopefully called recursively from the enclosing stream; the pointer was NULLed below */
		if ((stream->in_free == 1) && (close_options & PHP_STREAM_FREE_IGNORE_ENCLOSING) && (stream->enclosing_stream == NULL)) {
			close_options |= PHP_STREAM_FREE_RSRC_DTOR; /* restore flag */
		} else {
			return 1; /* recursion protection */
		}
	}

	stream->in_free++;

	/* force correct order on enclosing/enclosed stream destruction (only from resource
	 * destructor as in when reverse destroying the resource list) */
	if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) &&
			!(close_options & PHP_STREAM_FREE_IGNORE_ENCLOSING) &&
			(close_options & (PHP_STREAM_FREE_CALL_DTOR | PHP_STREAM_FREE_RELEASE_STREAM)) && /* always? */
			(stream->enclosing_stream != NULL)) {
		php_stream *enclosing_stream = stream->enclosing_stream;
		stream->enclosing_stream = NULL;
		/* we force PHP_STREAM_CALL_DTOR because that's from where the
		 * enclosing stream can free this stream. We remove rsrc_dtor because
		 * we want the enclosing stream to be deleted from the resource list */
		return _php_stream_free(enclosing_stream,
			(close_options | PHP_STREAM_FREE_CALL_DTOR) & ~PHP_STREAM_FREE_RSRC_DTOR TSRMLS_CC);
	}

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
		stream->ops->label, stream, stream->orig_path, preserve_handle, release_cast,
		(close_options & PHP_STREAM_FREE_RSRC_DTOR) == 0);
#endif

	/* make sure everything is saved */
	_php_stream_flush(stream, 1 TSRMLS_CC);

	/* If not called from the resource dtor, remove the stream from the resource list. */
	if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) == 0) {
		/* zend_list_delete actually only decreases the refcount; if we're
		 * releasing the stream, we want to actually delete the resource from
		 * the resource list, otherwise the resource will point to invalid memory.
		 * In any case, let's always completely delete it from the resource list,
		 * not only when PHP_STREAM_FREE_RELEASE_STREAM is set */
		while (zend_list_delete(stream->rsrc_id) == SUCCESS) {}
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

		if (stream->readbuf) {
			pefree(stream->readbuf, stream->is_persistent);
			stream->readbuf = NULL;
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

	if (context) {
		zend_list_delete(context->rsrc_id);
	}

	return ret;
}
/* }}} */

/* {{{ generic stream operations */

static void php_stream_fill_read_buffer(php_stream *stream, size_t size TSRMLS_DC)
{
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
						/* grow buffer to hold this bucket
						 * TODO: this can fail for persistent streams */
						if (stream->readbuflen - stream->writepos < bucket->buflen) {
							stream->readbuflen += bucket->buflen;
							stream->readbuf = perealloc(stream->readbuf, stream->readbuflen,
									stream->is_persistent);
						}
						memcpy(stream->readbuf + stream->writepos, bucket->buf, bucket->buflen);
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

	} else {
		/* is there enough data in the buffer ? */
		if (stream->writepos - stream->readpos < (off_t)size) {
			size_t justread = 0;

			/* reduce buffer memory consumption if possible, to avoid a realloc */
			if (stream->readbuf && stream->readbuflen - stream->writepos < stream->chunk_size) {
				memmove(stream->readbuf, stream->readbuf + stream->readpos, stream->readbuflen - stream->readpos);
				stream->writepos -= stream->readpos;
				stream->readpos = 0;
			}

			/* grow the buffer if required
			 * TODO: this can fail for persistent streams */
			if (stream->readbuflen - stream->writepos < stream->chunk_size) {
				stream->readbuflen += stream->chunk_size;
				stream->readbuf = perealloc(stream->readbuf, stream->readbuflen,
						stream->is_persistent);
			}

			justread = stream->ops->read(stream, stream->readbuf + stream->writepos,
					stream->readbuflen - stream->writepos
					TSRMLS_CC);

			if (justread != (size_t)-1) {
				stream->writepos += justread;
			}
		}
	}
}

PHPAPI size_t _php_stream_read(php_stream *stream, char *buf, size_t size TSRMLS_DC)
{
	size_t toread = 0, didread = 0;

	while (size > 0) {

		/* take from the read buffer first.
		 * It is possible that a buffered stream was switched to non-buffered, so we
		 * drain the remainder of the buffer before using the "raw" read mode for
		 * the excess */
		if (stream->writepos > stream->readpos) {

			toread = stream->writepos - stream->readpos;
			if (toread > size) {
				toread = size;
			}

			memcpy(buf, stream->readbuf + stream->readpos, toread);
			stream->readpos += toread;
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
				memcpy(buf, stream->readbuf + stream->readpos, toread);
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

PHPAPI char *php_stream_locate_eol(php_stream *stream, char *buf, size_t buf_len TSRMLS_DC)
{
	size_t avail;
	char *cr, *lf, *eol = NULL;
	char *readptr;

	if (!buf) {
		readptr = stream->readbuf + stream->readpos;
		avail = stream->writepos - stream->readpos;
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
 * permitting */
PHPAPI char *_php_stream_get_line(php_stream *stream, char *buf, size_t maxlen,
		size_t *returned_len TSRMLS_DC)
{
	size_t avail = 0;
	size_t current_buf_size = 0;
	size_t total_copied = 0;
	int grow_mode = 0;
	char *bufstart = buf;

	if (buf == NULL) {
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

		if (avail > 0) {
			size_t cpysz = 0;
			char *readptr;
			char *eol;
			int done = 0;

			readptr = stream->readbuf + stream->readpos;
			eol = php_stream_locate_eol(stream, NULL, 0 TSRMLS_CC);

			if (eol) {
				cpysz = eol - readptr + 1;
				done = 1;
			} else {
				cpysz = avail;
			}

			if (grow_mode) {
				/* allow room for a NUL. If this realloc is really a realloc
				 * (ie: second time around), we get an extra byte. In most
				 * cases, with the default chunk size of 8K, we will only
				 * incur that overhead once.  When people have lines longer
				 * than 8K, we waste 1 byte per additional 8K or so.
				 * That seems acceptable to me, to avoid making this code
				 * hard to follow */
				bufstart = erealloc(bufstart, current_buf_size + cpysz + 1);
				current_buf_size += cpysz + 1;
				buf = bufstart + total_copied;
			} else {
				if (cpysz >= maxlen - 1) {
					cpysz = maxlen - 1;
					done = 1;
				}
			}

			memcpy(buf, readptr, cpysz);

			stream->position += cpysz;
			stream->readpos += cpysz;
			buf += cpysz;
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

			if (stream->writepos - stream->readpos == 0) {
				break;
			}
		}
	}

	if (total_copied == 0) {
		if (grow_mode) {
			assert(bufstart == NULL);
		}
		return NULL;
	}

	buf[0] = '\0';
	if (returned_len) {
		*returned_len = total_copied;
	}

	return bufstart;
}

#define STREAM_BUFFERED_AMOUNT(stream) \
	((size_t)(((stream)->writepos) - (stream)->readpos))

static char *_php_stream_search_delim(php_stream *stream,
									  size_t maxlen,
									  size_t skiplen,
									  char *delim, /* non-empty! */
									  size_t delim_len TSRMLS_DC)
{
	size_t	seek_len;

	/* set the maximum number of bytes we're allowed to read from buffer */
	seek_len = MIN(STREAM_BUFFERED_AMOUNT(stream), maxlen);
	if (seek_len <= skiplen) {
		return NULL;
	}

	if (delim_len == 1) {
		return memchr(&stream->readbuf[stream->readpos + skiplen],
			delim[0], seek_len - skiplen);
	} else {
		return php_memnstr((char*)&stream->readbuf[stream->readpos + skiplen],
				delim, delim_len,
				(char*)&stream->readbuf[stream->readpos + seek_len]);
	}
}

PHPAPI char *php_stream_get_record(php_stream *stream, size_t maxlen, size_t *returned_len, char *delim, size_t delim_len TSRMLS_DC)
{
	char	*ret_buf,				/* returned buffer */
			*found_delim = NULL;
	size_t	buffered_len,
			tent_ret_len;			/* tentative returned length */
	int		has_delim	 = delim_len > 0;

	if (maxlen == 0) {
		return NULL;
	}

	if (has_delim) {
		found_delim = _php_stream_search_delim(
			stream, maxlen, 0, delim, delim_len TSRMLS_CC);
	}

	buffered_len = STREAM_BUFFERED_AMOUNT(stream);
	/* try to read up to maxlen length bytes while we don't find the delim */
	while (!found_delim && buffered_len < maxlen) {
		size_t	just_read,
				to_read_now;

		to_read_now = MIN(maxlen - buffered_len, stream->chunk_size);

		php_stream_fill_read_buffer(stream, buffered_len + to_read_now TSRMLS_CC);

		just_read = STREAM_BUFFERED_AMOUNT(stream) - buffered_len;

		/* Assume the stream is temporarily or permanently out of data */
		if (just_read == 0) {
			break;
		}

		if (has_delim) {
			/* search for delimiter, but skip buffered_len (the number of bytes
			 * buffered before this loop iteration), as they have already been
			 * searched for the delimiter.
			 * The left part of the delimiter may still remain in the buffer,
			 * so subtract up to <delim_len - 1> from buffered_len, which is
			 * the ammount of data we skip on this search  as an optimization
			 */
			found_delim = _php_stream_search_delim(
				stream, maxlen,
				buffered_len >= (delim_len - 1)
						? buffered_len - (delim_len - 1)
						: 0,
				delim, delim_len TSRMLS_CC);
			if (found_delim) {
				break;
			}
		}
		buffered_len += just_read;
	}

	if (has_delim && found_delim) {
		tent_ret_len = found_delim - (char*)&stream->readbuf[stream->readpos];
	} else if (!has_delim && STREAM_BUFFERED_AMOUNT(stream) >= maxlen) {
		tent_ret_len = maxlen;
	} else {
		/* return with error if the delimiter string (if any) was not found, we
		 * could not completely fill the read buffer with maxlen bytes and we
		 * don't know we've reached end of file. Added with non-blocking streams
		 * in mind, where this situation is frequent */
		if (STREAM_BUFFERED_AMOUNT(stream) < maxlen && !stream->eof) {
			return NULL;
		} else if (STREAM_BUFFERED_AMOUNT(stream) == 0 && stream->eof) {
			/* refuse to return an empty string just because by accident
			 * we knew of EOF in a read that returned no data */
			return NULL;
		} else {
			tent_ret_len = MIN(STREAM_BUFFERED_AMOUNT(stream), maxlen);
		}
	}

	ret_buf = emalloc(tent_ret_len + 1);
	/* php_stream_read will not call ops->read here because the necessary
	 * data is guaranteedly buffered */
	*returned_len = php_stream_read(stream, ret_buf, tent_ret_len);

	if (found_delim) {
		stream->readpos += delim_len;
		stream->position += delim_len;
	}
	ret_buf[*returned_len] = '\0';
	return ret_buf;
}

/* Writes a buffer directly to a stream, using multiple of the chunk size */
static size_t _php_stream_write_buffer(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	size_t didwrite = 0, towrite, justwrote;

 	/* if we have a seekable stream we need to ensure that data is written at the
 	 * current stream->position. This means invalidating the read buffer and then
	 * performing a low-level seek */
	if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0 && stream->readpos != stream->writepos) {
		stream->readpos = stream->writepos = 0;

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
static size_t _php_stream_write_filtered(php_stream *stream, const char *buf, size_t count, int flags TSRMLS_DC)
{
	size_t consumed = 0;
	php_stream_bucket *bucket;
	php_stream_bucket_brigade brig_in = { NULL, NULL }, brig_out = { NULL, NULL };
	php_stream_bucket_brigade *brig_inp = &brig_in, *brig_outp = &brig_out, *brig_swap;
	php_stream_filter_status_t status = PSFS_ERR_FATAL;
	php_stream_filter *filter;

	if (buf) {
		bucket = php_stream_bucket_new(stream, (char *)buf, count, 0, 0 TSRMLS_CC);
		php_stream_bucket_append(&brig_in, bucket TSRMLS_CC);
	}

	for (filter = stream->writefilters.head; filter; filter = filter->next) {
		/* for our return value, we are interested in the number of bytes consumed from
		 * the first filter in the chain */
		status = filter->fops->filter(stream, filter, brig_inp, brig_outp,
				filter == stream->writefilters.head ? &consumed : NULL, flags TSRMLS_CC);

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
				_php_stream_write_buffer(stream, bucket->buf, bucket->buflen TSRMLS_CC);
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
		_php_stream_write_filtered(stream, NULL, 0, closing ? PSFS_FLAG_FLUSH_CLOSE : PSFS_FLAG_FLUSH_INC  TSRMLS_CC);
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
		return _php_stream_write_filtered(stream, buf, count, PSFS_FLAG_NORMAL TSRMLS_CC);
	} else {
		return _php_stream_write_buffer(stream, buf, count TSRMLS_CC);
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
	if (stream->fclose_stdiocast == PHP_STREAM_FCLOSE_FOPENCOOKIE) {
		/* flush to commit data written to the fopencookie FILE* */
		fflush(stream->stdiocast);
	}

	/* handle the case where we are in the buffer */
	if ((stream->flags & PHP_STREAM_FLAG_NO_BUFFER) == 0) {
		switch(whence) {
			case SEEK_CUR:
				if (offset > 0 && offset <= stream->writepos - stream->readpos) {
					stream->readpos += offset; /* if offset = ..., then readpos = writepos */
					stream->position += offset;
					stream->eof = 0;
					return 0;
				}
				break;
			case SEEK_SET:
				if (offset > stream->position &&
						offset <= stream->position + stream->writepos - stream->readpos) {
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
	if (whence == SEEK_CUR && offset >= 0) {
		char tmp[1024];
		size_t didread;
		while(offset > 0) {
			if ((didread = php_stream_read(stream, tmp, MIN(offset, sizeof(tmp)))) == 0) {
				return -1;
			}
			offset -= didread;
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
				} else if (stream->flags & PHP_STREAM_FLAG_NO_BUFFER) {
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

		p = php_stream_mmap_range(stream, php_stream_tell(stream), PHP_STREAM_MMAP_ALL, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped);

		if (p) {
			PHPWRITE(p, mapped);

			php_stream_mmap_unmap_ex(stream, mapped);

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

	if (maxlen == 0) {
		return 0;
	}

	if (maxlen == PHP_STREAM_COPY_ALL) {
		maxlen = 0;
	}

	if (maxlen > 0) {
		ptr = *buf = pemalloc_rel_orig(maxlen + 1, persistent);
		while ((len < maxlen) && !php_stream_eof(src)) {
			ret = php_stream_read(src, ptr, maxlen - len);
			if (!ret) {
				break;
			}
			len += ret;
			ptr += ret;
		}
		if (len) {
			*ptr = '\0';
		} else {
			pefree(*buf, persistent);
			*buf = NULL;
		}
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

/* Returns SUCCESS/FAILURE and sets *len to the number of bytes moved */
PHPAPI int _php_stream_copy_to_stream_ex(php_stream *src, php_stream *dest, size_t maxlen, size_t *len STREAMS_DC TSRMLS_DC)
{
	char buf[CHUNK_SIZE];
	size_t readchunk;
	size_t haveread = 0;
	size_t didread, didwrite, towrite;
	size_t dummy;
	php_stream_statbuf ssbuf;

	if (!len) {
		len = &dummy;
	}

	if (maxlen == 0) {
		*len = 0;
		return SUCCESS;
	}

	if (maxlen == PHP_STREAM_COPY_ALL) {
		maxlen = 0;
	}

	if (php_stream_stat(src, &ssbuf) == 0) {
		if (ssbuf.sb.st_size == 0
#ifdef S_ISREG
			&& S_ISREG(ssbuf.sb.st_mode)
#endif
		) {
			*len = 0;
			return SUCCESS;
		}
	}

	if (php_stream_mmap_possible(src)) {
		char *p;
		size_t mapped;

		p = php_stream_mmap_range(src, php_stream_tell(src), maxlen, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped);

		if (p) {
			didwrite = php_stream_write(dest, p, mapped);

			php_stream_mmap_unmap_ex(src, mapped);

			*len = didwrite;

			/* we've got at least 1 byte to read
			 * less than 1 is an error
			 * AND read bytes match written */
			if (mapped > 0 && mapped == didwrite) {
				return SUCCESS;
			}
			return FAILURE;
		}
	}

	while(1) {
		readchunk = sizeof(buf);

		if (maxlen && (maxlen - haveread) < readchunk) {
			readchunk = maxlen - haveread;
		}

		didread = php_stream_read(src, buf, readchunk);

		if (didread) {
			/* extra paranoid */
			char *writeptr;

			towrite = didread;
			writeptr = buf;
			haveread += didread;

			while(towrite) {
				didwrite = php_stream_write(dest, writeptr, towrite);
				if (didwrite == 0) {
					*len = haveread - (didread - towrite);
					return FAILURE;
				}

				towrite -= didwrite;
				writeptr += didwrite;
			}
		} else {
			break;
		}

		if (maxlen - haveread == 0) {
			break;
		}
	}

	*len = haveread;

	/* we've got at least 1 byte to read.
	 * less than 1 is an error */

	if (haveread > 0 || src->eof) {
		return SUCCESS;
	}
	return FAILURE;
}

/* Returns the number of bytes moved.
 * Returns 1 when source len is 0.
 * Deprecated in favor of php_stream_copy_to_stream_ex() */
ZEND_ATTRIBUTE_DEPRECATED
PHPAPI size_t _php_stream_copy_to_stream(php_stream *src, php_stream *dest, size_t maxlen STREAMS_DC TSRMLS_DC)
{
	size_t len;
	int ret = _php_stream_copy_to_stream_ex(src, dest, maxlen, &len STREAMS_REL_CC TSRMLS_CC);
	if (ret == SUCCESS && len == 0 && maxlen != 0) {
		return 1;
	}
	return len;
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
    
    if (FG(wrapper_errors)) {
		zend_hash_destroy(FG(wrapper_errors));
		efree(FG(wrapper_errors));
		FG(wrapper_errors) = NULL;
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

		if (FG(stream_wrappers)) {
		/* The file:// wrapper may have been disabled/overridden */

			if (wrapperpp) {
				/* It was found so go ahead and provide it */
				return *wrapperpp;
			}

			/* Check again, the original check might have not known the protocol name */
			if (zend_hash_find(wrapper_hash, "file", sizeof("file"), (void**)&wrapperpp) == SUCCESS) {
				return *wrapperpp;
			}

			if (options & REPORT_ERRORS) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "file:// wrapper is disabled in the server configuration");
			}
			return NULL;
		}

		return plain_files_wrapper;
	}

	if (wrapperpp && (*wrapperpp)->is_url &&
        (options & STREAM_DISABLE_URL_PROTECTION) == 0 &&
	    (!PG(allow_url_fopen) ||
	     (((options & STREAM_OPEN_FOR_INCLUDE) ||
	       PG(in_user_include)) && !PG(allow_url_include)))) {
		if (options & REPORT_ERRORS) {
			/* protocol[n] probably isn't '\0' */
			char *protocol_dup = estrndup(protocol, n);
			if (!PG(allow_url_fopen)) {
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
	char *resolved_path = NULL;
	char *copy_of_path = NULL;

	if (opened_path) {
		*opened_path = NULL;
	}

	if (!path || !*path) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename cannot be empty");
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

	if (wrapper) {
		if (!wrapper->wops->stream_opener) {
			php_stream_wrapper_log_error(wrapper, options ^ REPORT_ERRORS TSRMLS_CC,
					"wrapper does not support stream open");
		} else {
			stream = wrapper->wops->stream_opener(wrapper,
				path_to_open, mode, options ^ REPORT_ERRORS,
				opened_path, context STREAMS_REL_CC TSRMLS_CC);
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

/* {{{ context API */
PHPAPI php_stream_context *php_stream_context_set(php_stream *stream, php_stream_context *context)
{
	php_stream_context *oldcontext = stream->context;
	TSRMLS_FETCH();

	stream->context = context;

	if (context) {
		zend_list_addref(context->rsrc_id);
	}
	if (oldcontext) {
		zend_list_delete(oldcontext->rsrc_id);
	}

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
	efree(context);
}

PHPAPI php_stream_context *php_stream_context_alloc(TSRMLS_D)
{
	php_stream_context *context;

	context = ecalloc(1, sizeof(php_stream_context));
	context->notifier = NULL;
	MAKE_STD_ZVAL(context->options);
	array_init(context->options);

	context->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, context, php_le_stream_context(TSRMLS_C));
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
	unsigned int vector_size = 0;
	unsigned int nfiles = 0;

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
				if(vector_size*2 < vector_size) {
					/* overflow */
					efree(vector);
					return FAILURE;
				}
				vector_size *= 2;
			}
			vector = (char **) safe_erealloc(vector, vector_size, sizeof(char *), 0);
		}

		vector[nfiles] = estrdup(sdp.d_name);

		nfiles++;
		if(vector_size < 10 || nfiles == 0) {
			/* overflow */
			efree(vector);
			return FAILURE;
		}
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
