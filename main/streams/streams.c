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
   | Borrowed code from:                                                  |
   | Rasmus Lerdorf <rasmus@lerdorf.on.ca>                                |
   | Jim Winstead <jimw@php.net>                                          |
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
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <stddef.h>
#include <fcntl.h>
#include "php_streams_int.h"

/* {{{ resource and registration code */
static HashTable url_stream_wrappers_hash;
static int le_stream = FAILURE; /* true global */
static int le_pstream = FAILURE; /* true global */

PHPAPI int php_file_le_stream(void)
{
	return le_stream;
}

PHPAPI int php_file_le_pstream(void)
{
	return le_pstream;
}

PHPAPI HashTable *php_stream_get_url_stream_wrappers_hash()
{
	return &url_stream_wrappers_hash;
}

static int forget_persistent_resource_id_numbers(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_stream *stream;
	
	if (Z_TYPE_P(rsrc) != le_pstream)
		return 0;

	stream = (php_stream*)rsrc->ptr;

#if STREAM_DEBUG
fprintf(stderr, "forget_persistent: %s:%p\n", stream->ops->label, stream);
#endif
	
	stream->rsrc_id = FAILURE;

	return 0;
}

PHP_RSHUTDOWN_FUNCTION(streams)
{
	zend_hash_apply(&EG(persistent_list), (apply_func_t)forget_persistent_resource_id_numbers TSRMLS_CC);
	return SUCCESS;
}

PHPAPI int php_stream_from_persistent_id(const char *persistent_id, php_stream **stream TSRMLS_DC)
{
	list_entry *le;

	if (zend_hash_find(&EG(persistent_list), (char*)persistent_id, strlen(persistent_id)+1, (void*) &le) == SUCCESS) {
		if (Z_TYPE_P(le) == le_pstream) {
			if (stream) {
				*stream = (php_stream*)le->ptr;
				if ((*stream)->rsrc_id == FAILURE) {
					/* first access this request; give it a valid id */
					(*stream)->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, *stream, le_pstream);
				}
			}
			return PHP_STREAM_PERSISTENT_SUCCESS;
		}
		return PHP_STREAM_PERSISTENT_FAILURE;
	}
	return PHP_STREAM_PERSISTENT_NOT_EXIST;
}

/* }}} */

/* {{{ wrapper error reporting */
static void display_wrapper_errors(php_stream_wrapper *wrapper, const char *path, const char *caption TSRMLS_DC)
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
				if (i < wrapper->err_count - 1)
					l += brlen;
			}
			msg = emalloc(l + 1);
			msg[0] = '\0';
			for (i = 0; i < wrapper->err_count; i++) {
				strcat(msg, wrapper->err_stack[i]);
				if (i < wrapper->err_count - 1)
					strcat(msg, br);
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
	if (free_msg)
		efree(msg);
}

static void tidy_wrapper_error_log(php_stream_wrapper *wrapper TSRMLS_DC)
{
	if (wrapper) {
		/* tidy up the error stack */
		int i;

		for (i = 0; i < wrapper->err_count; i++)
			efree(wrapper->err_stack[i]);
		if (wrapper->err_stack)
			efree(wrapper->err_stack);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, buffer);
		efree(buffer);
	} else {
		/* append to stack */
		wrapper->err_stack = erealloc(wrapper->err_stack, (wrapper->err_count + 1) * sizeof(char *));
		if (wrapper->err_stack)
			wrapper->err_stack[wrapper->err_count++] = buffer;
	}
}


/* }}} */

/* allocate a new stream for a particular ops */
PHPAPI php_stream *_php_stream_alloc(php_stream_ops *ops, void *abstract, const char *persistent_id, const char *mode STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_stream *ret;
	
	ret = (php_stream*) pemalloc_rel_orig(sizeof(php_stream), persistent_id ? 1 : 0);

	memset(ret, 0, sizeof(php_stream));

#if STREAM_DEBUG
fprintf(stderr, "stream_alloc: %s:%p persistent=%s\n", ops->label, ret, persistent_id);
#endif
	
	ret->ops = ops;
	ret->abstract = abstract;
	ret->is_persistent = persistent_id ? 1 : 0;
	ret->chunk_size = FG(def_chunk_size);
	
	if (FG(auto_detect_line_endings))
		ret->flags |= PHP_STREAM_FLAG_DETECT_EOL;

	if (persistent_id) {
		list_entry le;

		Z_TYPE(le) = le_pstream;
		le.ptr = ret;

		if (FAILURE == zend_hash_update(&EG(persistent_list), (char *)persistent_id,
					strlen(persistent_id) + 1,
					(void *)&le, sizeof(list_entry), NULL)) {
			
			pefree(ret, 1);
			return NULL;
		}
	}
	
	ret->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, ret, persistent_id ? le_pstream : le_stream);
	strlcpy(ret->mode, mode, sizeof(ret->mode));
	
	return ret;
}
/* }}} */

PHPAPI int _php_stream_free(php_stream *stream, int close_options TSRMLS_DC) /* {{{ */
{
	int ret = 1;
	int remove_rsrc = 1;
	int preserve_handle = close_options & PHP_STREAM_FREE_PRESERVE_HANDLE ? 1 : 0;
	int release_cast = 1;

#if STREAM_DEBUG
fprintf(stderr, "stream_free: %s:%p[%s] in_free=%d opts=%08x\n", stream->ops->label, stream, stream->__orig_path, stream->in_free, close_options);
#endif
	
	/* recursion protection */
	if (stream->in_free)
		return 1;

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
		stream->ops->label, stream, stream->__orig_path, preserve_handle, release_cast, remove_rsrc);
#endif
	
	/* make sure everything is saved */
	_php_stream_flush(stream, 1 TSRMLS_CC);
		
	/* If not called from the resource dtor, remove the stream
     * from the resource list.
	 * */
	if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) == 0 && remove_rsrc) {
		zend_list_delete(stream->rsrc_id);
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
		
#if ZEND_DEBUG
		if ((close_options & PHP_STREAM_FREE_RSRC_DTOR) && (stream->__exposed == 0) && (EG(error_reporting) & E_WARNING)) {
			/* it leaked: Lets deliberately NOT pefree it so that the memory manager shows it
			 * as leaked; it will log a warning, but lets help it out and display what kind
			 * of stream it was. */
			char leakbuf[512];
			snprintf(leakbuf, sizeof(leakbuf), __FILE__ "(%d) : Stream of type '%s' 0x%08X (path:%s) was not closed\n", __LINE__, stream->ops->label, (unsigned int)stream, stream->__orig_path);

			STR_FREE(stream->__orig_path);
			
# if defined(PHP_WIN32)
			OutputDebugString(leakbuf);
# else
			fprintf(stderr, leakbuf);
# endif
		} else {
			STR_FREE(stream->__orig_path);
			pefree(stream, stream->is_persistent);
		}
#else
		pefree(stream, stream->is_persistent);
#endif
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

		/* allocate a buffer for reading chunks */
		chunk_buf = emalloc(stream->chunk_size);

		while (!err_flag && (stream->writepos - stream->readpos < (off_t)size)) {
			size_t justread = 0;
			int flags;
			php_stream_bucket *bucket;
			php_stream_filter_status_t status;
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

			if (justread == 0) {
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
			if (toread > size)
				toread = size;

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
			if (toread > size)
				toread = size;

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
	}

	if (didread > 0)
		stream->position += didread;

	return didread;
}

PHPAPI int _php_stream_eof(php_stream *stream TSRMLS_DC)
{
	/* if there is data in the buffer, it's not EOF */
	if (stream->writepos - stream->readpos > 0)
		return 0;

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

	if (buf == NULL)
		grow_mode = 1;
	else if (maxlen == 0)
		return NULL;

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
				if (toread > stream->chunk_size)
					toread = stream->chunk_size;
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
	if (returned_len)
		*returned_len = total_copied;

	return bufstart;
}

PHPAPI char *php_stream_get_record(php_stream *stream, size_t maxlen, size_t *returned_len, char *delim, size_t delim_len TSRMLS_DC)
{
	char *e, *buf;
	size_t toread;

	php_stream_fill_read_buffer(stream, maxlen TSRMLS_CC);

	if (delim_len == 0) {
		toread = maxlen;
	} else {
		if (delim_len == 1) {
			e = memchr(stream->readbuf, *delim, stream->readbuflen);
		} else {
			e = php_memnstr(stream->readbuf, delim, delim_len, (stream->readbuf + stream->readbuflen));
		}

		if (!e) {
			toread = maxlen;
		} else {
			toread = e - (char *) stream->readbuf;
		}
	}

	buf = emalloc(toread + 1);
	*returned_len = php_stream_read(stream, buf, toread);
	                
	if (*returned_len >= 0) {
		return buf;
	} else {
		efree(buf);
		return NULL;
	}
}

/* Writes a buffer directly to a stream, using multiple of the chunk size */
static size_t _php_stream_write_buffer(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	size_t didwrite = 0, towrite, justwrote;

	while (count > 0) {
		towrite = count;
		if (towrite > stream->chunk_size)
			towrite = stream->chunk_size;
	
		justwrote = stream->ops->write(stream, buf, towrite TSRMLS_CC);

		if (justwrote > 0) {
			buf += justwrote;
			count -= justwrote;
			didwrite += justwrote;
			
			/* Only screw with the buffer if we can seek, otherwise we lose data
			 * buffered from fifos and sockets */
			if (stream->ops->seek && (stream->flags & PHP_STREAM_FLAG_NO_SEEK) == 0 && !php_stream_is_filtered(stream)) {
				stream->position += justwrote;
				stream->writepos = 0;
				stream->readpos = 0;
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
	php_stream_filter_status_t status;
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
	if (buf == NULL || count == 0 || stream->ops->write == NULL)
		return 0;

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
	
	if (!buf)
		return 0; /* error condition */
	
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
	/* not moving anywhere */
	if ((offset == 0 && whence == SEEK_CUR) || (offset == stream->position && whence == SEEK_SET))
		return 0;

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
	
	/* invalidate the buffer contents */
	stream->readpos = stream->writepos = 0;

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
			if (ret == 0)
				stream->eof = 0;
			return ret;
		}
		/* else the stream has decided that it can't support seeking after all;
		 * fall through to attempt emulation */
	}

	/* emulate forward moving seeks with reads */
	if (whence == SEEK_CUR && offset > 0) {
		char tmp[1024];
		while(offset >= sizeof(tmp)) {
			if (php_stream_read(stream, tmp, sizeof(tmp)) == 0)
				return -1;
			offset -= sizeof(tmp);
		}
		if (offset)	{
			if (php_stream_read(stream, tmp, offset) == 0)
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
				ret = PHP_STREAM_OPTION_RETURN_ERR;
		}
	}
	
	return ret;
}

PHPAPI size_t _php_stream_passthru(php_stream * stream STREAMS_DC TSRMLS_DC)
{
	size_t bcount = 0;
	int ready = 0;
	char buf[8192];
#ifdef HAVE_MMAP
	int fd;
#endif

#ifdef HAVE_MMAP
	if (!php_stream_is(stream, PHP_STREAM_IS_SOCKET)
			&& !php_stream_is_filtered(stream)
			&& php_stream_tell(stream) == 0
			&& SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD, (void*)&fd, 0))
	{
		struct stat sbuf;
		off_t off;
		void *p;
		size_t len;

		fstat(fd, &sbuf);

		if (sbuf.st_size > sizeof(buf)) {
			off = php_stream_tell(stream);
			len = sbuf.st_size - off;
			p = mmap(0, len, PROT_READ, MAP_SHARED, fd, off);
			if (p != (void *) MAP_FAILED) {
				BG(mmap_file) = p;
				BG(mmap_len) = len;
				PHPWRITE(p, len);
				BG(mmap_file) = NULL;
				munmap(p, len);
				bcount += len;
				ready = 1;
			}
		}
	}
#endif
	if(!ready) {
		int b;

		while ((b = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			PHPWRITE(buf, b);
			bcount += b;
		}
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
#if HAVE_MMAP
	int srcfd;
#endif

	if (buf)
		*buf = NULL;

	if (maxlen == 0)
		return 0;

	if (maxlen == PHP_STREAM_COPY_ALL)
		maxlen = 0;

#if HAVE_MMAP
	/* try and optimize the case where we are copying from the start of a plain file.
	 * We could probably make this work in more situations, but I don't trust the stdio
	 * buffering layer.
	 * */
	if ( php_stream_is(src, PHP_STREAM_IS_STDIO) &&
			!php_stream_is_filtered(src) &&
			php_stream_tell(src) == 0 &&
			SUCCESS == php_stream_cast(src, PHP_STREAM_AS_FD, (void**)&srcfd, 0))
	{
		struct stat sbuf;

		if (fstat(srcfd, &sbuf) == 0) {
			void *srcfile;

#if STREAM_DEBUG
			fprintf(stderr, "mmap attempt: maxlen=%d filesize=%ld\n", maxlen, sbuf.st_size);
#endif
	
			if (maxlen > sbuf.st_size || maxlen == 0)
				maxlen = sbuf.st_size;
#if STREAM_DEBUG
			fprintf(stderr, "mmap attempt: will map maxlen=%d\n", maxlen);
#endif
		
			srcfile = mmap(NULL, maxlen, PROT_READ, MAP_SHARED, srcfd, 0);
			if (srcfile != (void*)MAP_FAILED) {

				*buf = pemalloc_rel_orig(maxlen + 1, persistent);

				if (*buf)	{
					memcpy(*buf, srcfile, maxlen);
					(*buf)[maxlen] = '\0';
					ret = maxlen;
				}

				munmap(srcfile, maxlen);

				return ret;
			}
		}
		/* fall through - we might be able to copy in smaller chunks */
	}
#endif

	ptr = *buf = pemalloc_rel_orig(step, persistent);
	max_len = step;

	while((ret = php_stream_read(src, ptr, max_len - len)))	{
		len += ret;
		if (len + min_room >= max_len) {
			*buf = perealloc_rel_orig(*buf, max_len + step, persistent);
			max_len += step;
			ptr = *buf + len;
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
#if HAVE_MMAP
	int srcfd;
#endif

	if (maxlen == 0)
		return 0;

	if (maxlen == PHP_STREAM_COPY_ALL)
		maxlen = 0;

#if HAVE_MMAP
	/* try and optimize the case where we are copying from the start of a plain file.
	 * We could probably make this work in more situations, but I don't trust the stdio
	 * buffering layer.
	 * */
	if ( php_stream_is(src, PHP_STREAM_IS_STDIO) &&
			!php_stream_is_filtered(src) &&
			php_stream_tell(src) == 0 &&
			SUCCESS == php_stream_cast(src, PHP_STREAM_AS_FD, (void**)&srcfd, 0))
	{
		struct stat sbuf;

		if (fstat(srcfd, &sbuf) == 0) {
			void *srcfile;

			/* in the event that the source file is 0 bytes, return 1 to indicate success
			 * because opening the file to write had already created a copy */

			if(sbuf.st_size ==0)
				return 1;

			if (maxlen > sbuf.st_size || maxlen == 0)
				maxlen = sbuf.st_size;

			srcfile = mmap(NULL, maxlen, PROT_READ, MAP_SHARED, srcfd, 0);
			if (srcfile != (void*)MAP_FAILED) {
				haveread = php_stream_write(dest, srcfile, maxlen);
				munmap(srcfile, maxlen);
				return haveread;
			}
		}
		/* fall through - we might be able to copy in smaller chunks */
	}
#endif

	if (php_stream_stat(src, &ssbuf) == 0) {
		/* in the event that the source file is 0 bytes, return 1 to indicate success
		 * because opening the file to write had already created a copy */
		if (ssbuf.sb.st_size == 0) {
			return 1;
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
				if (didwrite == 0)
					return 0;	/* error */

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

int php_init_stream_wrappers(int module_number TSRMLS_DC)
{
	le_stream = zend_register_list_destructors_ex(stream_resource_regular_dtor, NULL, "stream", module_number);
	le_pstream = zend_register_list_destructors_ex(NULL, stream_resource_persistent_dtor, "persistent stream", module_number);

	return (
			zend_hash_init(&url_stream_wrappers_hash, 0, NULL, NULL, 1) == SUCCESS
			&& 
			zend_hash_init(php_get_stream_filters_hash(), 0, NULL, NULL, 1) == SUCCESS
		) ? SUCCESS : FAILURE;
}

int php_shutdown_stream_wrappers(int module_number TSRMLS_DC)
{
	zend_hash_destroy(&url_stream_wrappers_hash);
	zend_hash_destroy(php_get_stream_filters_hash());
	return SUCCESS;
}

PHPAPI int php_register_url_stream_wrapper(char *protocol, php_stream_wrapper *wrapper TSRMLS_DC)
{
	return zend_hash_add(&url_stream_wrappers_hash, protocol, strlen(protocol), wrapper, sizeof(*wrapper), NULL);
}

PHPAPI int php_unregister_url_stream_wrapper(char *protocol TSRMLS_DC)
{
	return zend_hash_del(&url_stream_wrappers_hash, protocol, strlen(protocol));
}
/* }}} */

/* {{{ php_stream_locate_url_wrapper */
PHPAPI php_stream_wrapper *php_stream_locate_url_wrapper(const char *path, char **path_for_open, int options TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;
	const char *p, *protocol = NULL;
	int n = 0;

	if (path_for_open)
		*path_for_open = (char*)path;

	if (options & IGNORE_URL)
		return (options & STREAM_LOCATE_WRAPPERS_ONLY) ? NULL : &php_plain_files_wrapper;
	
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
		if (FAILURE == zend_hash_find(&url_stream_wrappers_hash, (char*)protocol, n, (void**)&wrapper))	{
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
		if (protocol && path[n+1] == '/' && path[n+2] == '/')	{
			if (options & REPORT_ERRORS)
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "remote host file access not supported, %s", path);
			return NULL;
		}
		if (protocol && path_for_open)
			*path_for_open = (char*)path + n + 1;
		
		/* fall back on regular file access */
		return (options & STREAM_LOCATE_WRAPPERS_ONLY) ? NULL : &php_plain_files_wrapper;
	}

	if (wrapper && wrapper->is_url && !PG(allow_url_fopen)) {
		if (options & REPORT_ERRORS)
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "URL file-access is disabled in the server configuration");
		return NULL;
	}
	
	return wrapper;
}
/* }}} */

/* {{{ _php_stream_stat_path */
PHPAPI int _php_stream_stat_path(char *path, php_stream_statbuf *ssb TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;
	char *path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, ENFORCE_SAFE_MODE TSRMLS_CC);
	if (wrapper && wrapper->wops->url_stat) {
		return wrapper->wops->url_stat(wrapper, path_to_open, ssb TSRMLS_CC);
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
	
	if (!path || !*path)
		return NULL;

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);

	if (wrapper && wrapper->wops->dir_opener)	{
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
		display_wrapper_errors(wrapper, path, "failed to open dir" TSRMLS_CC);
	}
	tidy_wrapper_error_log(wrapper TSRMLS_CC);

	return stream;
}
/* }}} */

/* {{{ _php_stream_readdir */
PHPAPI php_stream_dirent *_php_stream_readdir(php_stream *dirstream, php_stream_dirent *ent TSRMLS_DC)
{

	if (sizeof(php_stream_dirent) == php_stream_read(dirstream, (char*)ent, sizeof(php_stream_dirent)))
		return ent;
	
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
#if ZEND_DEBUG
	char *copy_of_path = NULL;
#endif
	
	
	if (opened_path)
		*opened_path = NULL;

	if (!path || !*path)
		return NULL;

	path_to_open = path;

	wrapper = php_stream_locate_url_wrapper(path, &path_to_open, options TSRMLS_CC);
	if (options & STREAM_USE_URL && (!wrapper || !wrapper->is_url)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This function may only be used against URLs.");
		return NULL;
	}

	if (wrapper)	{

		/* prepare error stack */
		wrapper->err_count = 0;
		wrapper->err_stack = NULL;
		
		stream = wrapper->wops->stream_opener(wrapper,
				path_to_open, mode, options ^ REPORT_ERRORS,
				opened_path, context STREAMS_REL_CC TSRMLS_CC);
		if (stream)
			stream->wrapper = wrapper;
	}

#if ZEND_DEBUG
	if (stream) {
		copy_of_path = estrdup(path);
		stream->__orig_path = copy_of_path;
	}
#endif
	
	if (stream != NULL && (options & STREAM_MUST_SEEK)) {
		php_stream *newstream;

		switch(php_stream_make_seekable_rel(stream, &newstream,
					(options & STREAM_WILL_CAST)
						? PHP_STREAM_PREFER_STDIO : PHP_STREAM_NO_PREFERENCE)) {
			case PHP_STREAM_UNCHANGED:
				return stream;
			case PHP_STREAM_RELEASED:
#if ZEND_DEBUG
				newstream->__orig_path = estrdup(path);
#endif
				return newstream;
			default:
				php_stream_close(stream);
				stream = NULL;
				if (options & REPORT_ERRORS) {
					char *tmp = estrdup(path);
					php_strip_url_passwd(tmp);
					php_error_docref1(NULL TSRMLS_CC, tmp, E_WARNING, "could not make seekable - %s",
							tmp, strerror(errno));
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
		display_wrapper_errors(wrapper, path, "failed to open stream" TSRMLS_CC);
	}
	tidy_wrapper_error_log(wrapper TSRMLS_CC);
#if ZEND_DEBUG
	if (stream == NULL && copy_of_path != NULL) {
		efree(copy_of_path);
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
	zval_ptr_dtor(&context->options);
	efree(context);
}

PHPAPI php_stream_context *php_stream_context_alloc(void)
{
	php_stream_context *context;
	
	context = ecalloc(1, sizeof(php_stream_context));
	MAKE_STD_ZVAL(context->options);
	array_init(context->options);

	return context;
}

PHPAPI php_stream_notifier *php_stream_notification_alloc(void)
{
	return ecalloc(1, sizeof(php_stream_notifier));
}

PHPAPI void php_stream_notification_free(php_stream_notifier *notifier)
{
	efree(notifier);
}

PHPAPI int php_stream_context_get_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval ***optionvalue)
{
	zval **wrapperhash;
	
	if (FAILURE == zend_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash))
		return FAILURE;
	return zend_hash_find(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)optionvalue);
}

PHPAPI int php_stream_context_set_option(php_stream_context *context,
		const char *wrappername, const char *optionname, zval *optionvalue)
{
	zval **wrapperhash;
	zval *category;

	if (FAILURE == zend_hash_find(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&wrapperhash)) {
		
		MAKE_STD_ZVAL(category);
		array_init(category);
		if (FAILURE == zend_hash_update(Z_ARRVAL_P(context->options), (char*)wrappername, strlen(wrappername)+1, (void**)&category, sizeof(zval *), NULL))
			return FAILURE;

		ZVAL_ADDREF(optionvalue);
		wrapperhash = &category;
	}
	return zend_hash_update(Z_ARRVAL_PP(wrapperhash), (char*)optionname, strlen(optionname)+1, (void**)&optionvalue, sizeof(zval *), NULL);
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
