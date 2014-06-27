/*
  zip_source_window.c -- return part of lower source
  Copyright (C) 2012-2014 Dieter Baron and Thomas Klausner

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

struct window {
    zip_uint64_t skip;
    zip_uint64_t len;
    zip_uint64_t left;
    int e[2];
};

static zip_int64_t window_read(struct zip_source *, void *, void *,
			       zip_uint64_t, enum zip_source_cmd);



struct zip_source *
zip_source_window(struct zip *za, struct zip_source *src, zip_uint64_t start, zip_uint64_t len)
{
    struct window *ctx;

    if (src == NULL) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((ctx=(struct window *)malloc(sizeof(*ctx))) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    ctx->skip = start;
    ctx->len = len;
    ctx->left = len;

    return zip_source_layered(za, src, window_read, ctx);
}



static zip_int64_t
window_read(struct zip_source *src, void *_ctx, void *data,
	    zip_uint64_t len, enum zip_source_cmd cmd)
{
    struct window *ctx;
    zip_int64_t ret;
    zip_uint64_t n, i;
    char b[8192];

    ctx = (struct window *)_ctx;

    switch (cmd) {
    case ZIP_SOURCE_OPEN:
	for (n=0; n<ctx->skip; n+=(zip_uint64_t)ret) {
	    i = (ctx->skip-n > sizeof(b) ? sizeof(b) : ctx->skip-n);
	    if ((ret=zip_source_read(src, b, i)) < 0)
		return ZIP_SOURCE_ERR_LOWER;
	    if (ret==0) {
		ctx->e[0] = ZIP_ER_EOF;
		ctx->e[1] = 0;
		return -1;
	    }
	}
	return 0;

    case ZIP_SOURCE_READ:
	if (len > ctx->left)
	    len = ctx->left;
	
	if (len == 0)
	    return 0;

	if ((ret=zip_source_read(src, data, len)) < 0)
	    return ZIP_SOURCE_ERR_LOWER;

	ctx->left -= (zip_uint64_t)ret;

        if (ret == 0) {
	    if (ctx->left > 0) {
		ctx->e[0] = ZIP_ER_EOF;
		ctx->e[1] = 0;
		return -1;
	    }
	}
	return ret;

    case ZIP_SOURCE_CLOSE:
	return 0;

    case ZIP_SOURCE_STAT:
	{
	    struct zip_stat *st;

	    st = (struct zip_stat *)data;

	    st->size = ctx->len;
	    st->valid |= ZIP_STAT_SIZE;
	    st->valid &= ~(ZIP_STAT_CRC|ZIP_STAT_COMP_SIZE);
	}
	return 0;
	
    case ZIP_SOURCE_ERROR:
	memcpy(data, ctx->e, sizeof(ctx->e));
	return 0;

    case ZIP_SOURCE_FREE:
	free(ctx);
	return 0;

    default:
	return -1;
    }
    
}
