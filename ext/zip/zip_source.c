/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Tim Starling <tstarling@wikimedia.org>                       |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif
#include "php.h"
#include "php_zip.h"

typedef struct _php_zip_string_source {
	/* The current string being read from */
	zend_string *in_str;
	/* The offset into in_str of the current read position */
	size_t in_offset;
	/* The modification time returned in stat calls */
	time_t mtime;
	/* The current string being written to */
	zend_string *out_str;
	/* The offset into out_str of the current write position */
	size_t out_offset;
	/* A place to copy the result to when the archive is closed, or NULL */
	zend_string **dest;
	/* The error to be returned when libzip asks for the last error code */
	zip_error_t error;
} php_zip_string_source;

/* The source callback function, see https://libzip.org/documentation/zip_source_function.html
 * This is similar to read_data() in libzip's zip_source_buffer.c */
static zip_int64_t php_zip_string_cb(void *userdata, void *data, zip_uint64_t len, zip_source_cmd_t cmd)
{
	php_zip_string_source *ctx = userdata;
	switch (cmd) {
		case ZIP_SOURCE_SUPPORTS:
			return zip_source_make_command_bitmap(
				ZIP_SOURCE_FREE,
#if LIBZIP_VERSION_MAJOR > 1 || LIBZIP_VERSION_MINOR >= 10
				ZIP_SOURCE_SUPPORTS_REOPEN,
#endif
				ZIP_SOURCE_OPEN,
				ZIP_SOURCE_READ,
				ZIP_SOURCE_CLOSE,
				ZIP_SOURCE_STAT,
				ZIP_SOURCE_ERROR,
				ZIP_SOURCE_SEEK,
				ZIP_SOURCE_TELL,
				ZIP_SOURCE_BEGIN_WRITE,
				ZIP_SOURCE_WRITE,
				ZIP_SOURCE_COMMIT_WRITE,
				ZIP_SOURCE_ROLLBACK_WRITE,
				ZIP_SOURCE_SEEK_WRITE,
				ZIP_SOURCE_TELL_WRITE,
				ZIP_SOURCE_REMOVE,
				-1
			);

		case ZIP_SOURCE_FREE:
			zend_string_release(ctx->out_str);
			zend_string_release(ctx->in_str);
			efree(ctx);
			return 0;

		/* Read ops */

		case ZIP_SOURCE_OPEN:
			ctx->in_offset = 0;
			return 0;

		case ZIP_SOURCE_READ: {
			size_t remaining = ZSTR_LEN(ctx->in_str) - ctx->in_offset;
			len = MIN(len, remaining);
			if (len) {
				memcpy(data, ZSTR_VAL(ctx->in_str) + ctx->in_offset, len);
				ctx->in_offset += len;
			}
			return len;
		}

		case ZIP_SOURCE_CLOSE:
			return 0;

		case ZIP_SOURCE_STAT: {
			zip_stat_t *st;
			if (len < sizeof(*st)) {
				zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
				return -1;
			}

			st = (zip_stat_t *)data;
			zip_stat_init(st);
			st->mtime = ctx->mtime;
			st->size = ZSTR_LEN(ctx->in_str);
			st->comp_size = st->size;
			st->comp_method = ZIP_CM_STORE;
			st->encryption_method = ZIP_EM_NONE;
			st->valid = ZIP_STAT_MTIME | ZIP_STAT_SIZE | ZIP_STAT_COMP_SIZE | ZIP_STAT_COMP_METHOD | ZIP_STAT_ENCRYPTION_METHOD;

			return sizeof(*st);
		}

		case ZIP_SOURCE_ERROR:
			return zip_error_to_data(&ctx->error, data, len);

		/* Seekable read ops */

		case ZIP_SOURCE_SEEK: {
			zip_int64_t new_offset = zip_source_seek_compute_offset(
					ctx->in_offset, ZSTR_LEN(ctx->in_str), data, len, &ctx->error);
			if (new_offset < 0) {
				return -1;
			}
			ctx->in_offset = (size_t)new_offset;
			return 0;
		}

		case ZIP_SOURCE_TELL:
			if (ctx->in_offset > ZIP_INT64_MAX) {
				zip_error_set(&ctx->error, ZIP_ER_TELL, EOVERFLOW);
				return -1;
			}
			return (zip_int64_t)ctx->in_offset;

		/* Write ops */

		case ZIP_SOURCE_BEGIN_WRITE:
			zend_string_release(ctx->out_str);
			ctx->out_str = ZSTR_EMPTY_ALLOC();
			return 0;

		case ZIP_SOURCE_WRITE:
			if (ctx->out_offset > SIZE_MAX - len) {
				zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
				return -1;
			}
			if (ctx->out_offset + len > ZSTR_LEN(ctx->out_str)) {
				ctx->out_str = zend_string_realloc(ctx->out_str, ctx->out_offset + len, false);
			}
			memcpy(ZSTR_VAL(ctx->out_str) + ctx->out_offset, data, len);
			ctx->out_offset += len;
			return len;

		case ZIP_SOURCE_COMMIT_WRITE:
			ZSTR_VAL(ctx->out_str)[ZSTR_LEN(ctx->out_str)] = '\0';
			zend_string_release(ctx->in_str);
			ctx->in_str = ctx->out_str;
			ctx->out_str = ZSTR_EMPTY_ALLOC();
			if (ctx->dest) {
				*(ctx->dest) = zend_string_copy(ctx->in_str);
			}
			return 0;

		case ZIP_SOURCE_ROLLBACK_WRITE:
			zend_string_release(ctx->out_str);
			ctx->out_str = ZSTR_EMPTY_ALLOC();
			return 0;

		case ZIP_SOURCE_SEEK_WRITE: {
			zip_int64_t new_offset = zip_source_seek_compute_offset(
					ctx->out_offset, ZSTR_LEN(ctx->out_str), data, len, &ctx->error);
			if (new_offset < 0) {
				return -1;
			}
			ctx->out_offset = new_offset;
			return 0;
		}

		case ZIP_SOURCE_TELL_WRITE:
			if (ctx->out_offset > ZIP_INT64_MAX) {
				zip_error_set(&ctx->error, ZIP_ER_TELL, EOVERFLOW);
				return -1;
			}
			return (zip_int64_t)ctx->out_offset;

		case ZIP_SOURCE_REMOVE:
			zend_string_release(ctx->in_str);
			ctx->in_str = ZSTR_EMPTY_ALLOC();
			ctx->in_offset = 0;
			return 0;

		default:
			zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
			return -1;
	}
}

zip_source_t * php_zip_create_string_source(zend_string *str, zend_string **dest, zip_error_t *err)
{
	php_zip_string_source *ctx = ecalloc(1, sizeof(php_zip_string_source));
	ctx->in_str = zend_string_copy(str);
	ctx->out_str = ZSTR_EMPTY_ALLOC();
	ctx->dest = dest;
	ctx->mtime = time(NULL);
	return zip_source_function_create(php_zip_string_cb, (void*)ctx, err);
}
