/*
  zip_source_deflate.c -- deflate (de)compressoin routines
  Copyright (C) 2009 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include <stdlib.h>
#include <string.h>

#include "zipint.h"

struct deflate {
    int e[2];

    int eof;
    int mem_level;
    zip_uint64_t size;
    char buffer[BUFSIZE];
    z_stream zstr;
};

static zip_int64_t compress_read(struct zip_source *, struct deflate *,
				 void *, zip_uint64_t);
static zip_int64_t decompress_read(struct zip_source *, struct deflate *,
				   void *, zip_uint64_t);
static zip_int64_t deflate_compress(struct zip_source *, void *, void *,
				    zip_uint64_t, enum zip_source_cmd);
static zip_int64_t deflate_decompress(struct zip_source *, void *, void *,
				      zip_uint64_t, enum zip_source_cmd);
static void deflate_free(struct deflate *);



struct zip_source *
zip_source_deflate(struct zip *za, struct zip_source *src,
		   zip_int32_t cm, int flags)
{
    struct deflate *ctx;
    struct zip_source *s2;

    if (src == NULL || (cm != ZIP_CM_DEFLATE && !ZIP_CM_IS_DEFAULT(cm))) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((ctx=(struct deflate *)malloc(sizeof(*ctx))) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    ctx->e[0] = ctx->e[1] = 0;
    ctx->eof = 0;
    if (flags & ZIP_CODEC_ENCODE) {
	if (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0))
	    ctx->mem_level = TORRENT_MEM_LEVEL;
	else
	    ctx->mem_level = MAX_MEM_LEVEL;
    }

    if ((s2=zip_source_layered(za, src,
			       ((flags & ZIP_CODEC_ENCODE)
				? deflate_compress : deflate_decompress),
			       ctx)) == NULL) {
	deflate_free(ctx);
	return NULL;
    }

    return s2;
}



static zip_int64_t
compress_read(struct zip_source *src, struct deflate *ctx,
	      void *data, zip_uint64_t len)
{
    int end, ret;
    zip_int64_t n;

    if (ctx->e[0] != 0)
	return -1;

    if (len == 0)
	return 0;

    ctx->zstr.next_out = (Bytef *)data;
    ctx->zstr.avail_out = (uInt)len; /* TODO: check for overflow */

    end = 0;
    while (!end) {
	ret = deflate(&ctx->zstr, ctx->eof ? Z_FINISH : 0);

	switch (ret) {
	case Z_OK:
	case Z_STREAM_END:
	    /* all ok */

	    if (ctx->zstr.avail_out == 0
		|| (ctx->eof && ctx->zstr.avail_in == 0))
		end = 1;
	    break;

	case Z_BUF_ERROR:
	    if (ctx->zstr.avail_in == 0) {
		if (ctx->eof) {
		    end = 1;
		    break;
		}

		if ((n=zip_source_read(src, ctx->buffer, sizeof(ctx->buffer))) < 0) {
		    zip_source_error(src, ctx->e, ctx->e+1);
		    end = 1;
		    break;
		}
		else if (n == 0) {
		    ctx->eof = 1;
		    ctx->size = ctx->zstr.total_in;
		    /* TODO: check against stat of src? */
		}
		else {
		    ctx->zstr.next_in = (Bytef *)ctx->buffer;
		    ctx->zstr.avail_in = (uInt)n;
		}
		continue;
	    }
	    /* fallthrough */
	case Z_NEED_DICT:
	case Z_DATA_ERROR:
	case Z_STREAM_ERROR:
	case Z_MEM_ERROR:
	    ctx->e[0] = ZIP_ER_ZLIB;
	    ctx->e[1] = ret;

	    end = 1;
	    break;
	}
    }

    if (ctx->zstr.avail_out < len)
	return (zip_int64_t)(len - ctx->zstr.avail_out);

    return (ctx->e[0] == 0) ? 0 : -1;
}



static zip_int64_t
decompress_read(struct zip_source *src, struct deflate *ctx,
		void *data, zip_uint64_t len)
{
    int end, ret;
    zip_int64_t n;

    if (ctx->e[0] != 0)
	return -1;

    if (len == 0)
	return 0;

    ctx->zstr.next_out = (Bytef *)data;
    ctx->zstr.avail_out = (uInt)len; /* TODO: check for overflow */

    end = 0;
    while (!end && ctx->zstr.avail_out) {
	ret = inflate(&ctx->zstr, Z_SYNC_FLUSH);

	switch (ret) {
	case Z_OK:
	    break;

	case Z_STREAM_END:
	    ctx->eof = 1;
	    end = 1;
	    break;

	case Z_BUF_ERROR:
	    if (ctx->zstr.avail_in == 0) {
		if (ctx->eof) {
		    end = 1;
		    break;
		}

		if ((n=zip_source_read(src, ctx->buffer, sizeof(ctx->buffer))) < 0) {
		    zip_source_error(src, ctx->e, ctx->e+1);
		    end = 1;
		    break;
		}
		else if (n == 0)
		    ctx->eof = 1;
		else {
		    ctx->zstr.next_in = (Bytef *)ctx->buffer;
		    ctx->zstr.avail_in = (uInt)n;
		}
		continue;
	    }
	    /* fallthrough */
	case Z_NEED_DICT:
	case Z_DATA_ERROR:
	case Z_STREAM_ERROR:
	case Z_MEM_ERROR:
	    ctx->e[0] = ZIP_ER_ZLIB;
	    ctx->e[1] = ret;
	    end = 1;
	    break;
	}
    }

    if (ctx->zstr.avail_out < len)
	return (zip_int64_t)(len - ctx->zstr.avail_out);

    return (ctx->e[0] == 0) ? 0 : -1;
}



static zip_int64_t
deflate_compress(struct zip_source *src, void *ud, void *data,
		 zip_uint64_t len, enum zip_source_cmd cmd)
{
    struct deflate *ctx;
    int ret;

    ctx = (struct deflate *)ud;

    switch (cmd) {
    case ZIP_SOURCE_OPEN:
	ctx->zstr.zalloc = Z_NULL;
	ctx->zstr.zfree = Z_NULL;
	ctx->zstr.opaque = NULL;
	ctx->zstr.avail_in = 0;
	ctx->zstr.next_in = NULL;
	ctx->zstr.avail_out = 0;
	ctx->zstr.next_out = NULL;

	/* negative value to tell zlib not to write a header */
	if ((ret=deflateInit2(&ctx->zstr, Z_BEST_COMPRESSION, Z_DEFLATED,
			      -MAX_WBITS, ctx->mem_level,
			      Z_DEFAULT_STRATEGY)) != Z_OK) {
	    ctx->e[0] = ZIP_ER_ZLIB;
	    ctx->e[1] = ret;
	    return -1;
	}

	return 0;

    case ZIP_SOURCE_READ:
	return compress_read(src, ctx, data, len);

    case ZIP_SOURCE_CLOSE:
	deflateEnd(&ctx->zstr);
	return 0;

    case ZIP_SOURCE_STAT:
    	{
	    struct zip_stat *st;

	    st = (struct zip_stat *)data;

	    st->comp_method = ZIP_CM_DEFLATE;
	    st->valid |= ZIP_STAT_COMP_METHOD;
	    if (ctx->eof) {
		st->comp_size = ctx->size;
		st->valid |= ZIP_STAT_COMP_SIZE;
	    }
	    else
		st->valid &= ~ZIP_STAT_COMP_SIZE;
	}
	return 0;

    case ZIP_SOURCE_ERROR:
	memcpy(data, ctx->e, sizeof(int)*2);
	return sizeof(int)*2;

    case ZIP_SOURCE_FREE:
	deflate_free(ctx);
	return 0;

    default:
	ctx->e[0] = ZIP_ER_INVAL;
	ctx->e[1] = 0;
	return -1;
    }
}



static zip_int64_t
deflate_decompress(struct zip_source *src, void *ud, void *data,
		   zip_uint64_t len, enum zip_source_cmd cmd)
{
    struct deflate *ctx;
    zip_int64_t n;
    int ret;

    ctx = (struct deflate *)ud;

    switch (cmd) {
    case ZIP_SOURCE_OPEN:
	if ((n=zip_source_read(src, ctx->buffer, sizeof(ctx->buffer))) < 0)
	    return ZIP_SOURCE_ERR_LOWER;

	ctx->zstr.zalloc = Z_NULL;
	ctx->zstr.zfree = Z_NULL;
	ctx->zstr.opaque = NULL;
	ctx->zstr.next_in = (Bytef *)ctx->buffer;
	ctx->zstr.avail_in = (uInt)n /* TODO: check for overflow */;

	/* negative value to tell zlib that there is no header */
	if ((ret=inflateInit2(&ctx->zstr, -MAX_WBITS)) != Z_OK) {
	    ctx->e[0] = ZIP_ER_ZLIB;
	    ctx->e[1] = ret;

	    return -1;
	}
	return 0;

    case ZIP_SOURCE_READ:
	return decompress_read(src, ctx, data, len);

    case ZIP_SOURCE_CLOSE:
	inflateEnd(&ctx->zstr);
	return 0;

    case ZIP_SOURCE_STAT:
	{
	    struct zip_stat *st;

	    st = (struct zip_stat *)data;

	    st->comp_method = ZIP_CM_STORE;
	    if (st->comp_size > 0 && st->size > 0)
		st->comp_size = st->size;
	}
	return 0;

    case ZIP_SOURCE_ERROR:
	if (len < sizeof(int)*2)
	    return -1;

	memcpy(data, ctx->e, sizeof(int)*2);
	return sizeof(int)*2;

    case ZIP_SOURCE_FREE:
	/* TODO: inflateEnd if close was not called */
	free(ctx);
	return 0;

    default:
	ctx->e[0] = ZIP_ER_INVAL;
	ctx->e[1] = 0;
	return -1;
    }

}



static void
deflate_free(struct deflate *ctx)
{
    /* TODO: deflateEnd if close was not called */
    free(ctx);
}
