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
    zip_uint64_t start;
    zip_uint64_t end;
    zip_uint64_t offset;
    zip_stat_t stat;
    zip_error_t error;
    zip_int64_t supports;
    bool needs_seek;
};

static zip_int64_t window_read(zip_source_t *, void *, void *, zip_uint64_t, zip_source_cmd_t);


zip_source_t *
zip_source_window(zip_t *za, zip_source_t *src, zip_uint64_t start, zip_uint64_t len)
{
    return _zip_source_window_new(src, start, len, NULL, &za->error);
}


zip_source_t *
_zip_source_window_new(zip_source_t *src, zip_uint64_t start, zip_uint64_t length, zip_stat_t *st, zip_error_t *error)
{
    struct window *ctx;

    if (src == NULL || start + length < start) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx=(struct window *)malloc(sizeof(*ctx))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    ctx->start = start;
    ctx->end = start + length;
    zip_stat_init(&ctx->stat);
    zip_error_init(&ctx->error);
    ctx->supports = (zip_source_supports(src) & ZIP_SOURCE_SUPPORTS_SEEKABLE) | (zip_source_make_command_bitmap(ZIP_SOURCE_SUPPORTS, ZIP_SOURCE_TELL, -1));
    ctx->needs_seek = (ctx->supports & ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK)) ? true : false;

    if (st) {
        if (_zip_stat_merge(&ctx->stat, st, error) < 0) {
            free(ctx);
            return NULL;
        }
    }

    return zip_source_layered_create(src, window_read, ctx, error);
}


int
_zip_source_set_source_archive(zip_source_t *src, zip_t *za)
{
    src->source_archive = za;
    return _zip_register_source(za, src);
}


/* called by zip_discard to avoid operating on file from closed archive */
void
_zip_source_invalidate(zip_source_t *src)
{
    src->source_closed = 1;

    if (zip_error_code_zip(&src->error) == ZIP_ER_OK) {
        zip_error_set(&src->error, ZIP_ER_ZIPCLOSED, 0);
    }
}


static zip_int64_t
window_read(zip_source_t *src, void *_ctx, void *data, zip_uint64_t len, zip_source_cmd_t cmd)
{
    struct window *ctx;
    zip_int64_t ret;
    zip_uint64_t n, i;
    char b[8192];

    ctx = (struct window *)_ctx;

    switch (cmd) {
        case ZIP_SOURCE_CLOSE:
            return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            free(ctx);
            return 0;

        case ZIP_SOURCE_OPEN:
            if (!ctx->needs_seek) {
                for (n=0; n<ctx->start; n+=(zip_uint64_t)ret) {
                    i = (ctx->start-n > sizeof(b) ? sizeof(b) : ctx->start-n);
                    if ((ret=zip_source_read(src, b, i)) < 0) {
                        _zip_error_set_from_source(&ctx->error, src);
                        return -1;
                    }
                    if (ret==0) {
                        zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
                        return -1;
                    }
                }

            }
	    ctx->offset = ctx->start;
            return 0;

        case ZIP_SOURCE_READ:
            if (len > ctx->end - ctx->offset)
                len = ctx->end - ctx->offset;

            if (len == 0)
                return 0;

            if (ctx->needs_seek) {
                if (zip_source_seek(src, (zip_int64_t)ctx->offset, SEEK_SET) < 0) {
                    _zip_error_set_from_source(&ctx->error, src);
                    return -1;
                }
            }

            if ((ret=zip_source_read(src, data, len)) < 0) {
                zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
                return -1;
            }

            ctx->offset += (zip_uint64_t)ret;

            if (ret == 0) {
                if (ctx->offset < ctx->end) {
                    zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
                    return -1;
                }
            }
            return ret;

        case ZIP_SOURCE_SEEK:
        {
            zip_int64_t new_offset = zip_source_seek_compute_offset(ctx->offset - ctx->start, ctx->end - ctx->start, data, len, &ctx->error);

            if (new_offset < 0) {
                return -1;
            }

            ctx->offset = (zip_uint64_t)new_offset + ctx->start;
            return 0;
        }

       case ZIP_SOURCE_STAT:
        {
            zip_stat_t *st;

	    st = (zip_stat_t *)data;

            if (_zip_stat_merge(st, &ctx->stat, &ctx->error) < 0) {
                return -1;
            }
            return 0;
        }

        case ZIP_SOURCE_SUPPORTS:
            return ctx->supports;

        case ZIP_SOURCE_TELL:
            return (zip_int64_t)(ctx->offset - ctx->start);

       default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}


void
_zip_deregister_source(zip_t *za, zip_source_t *src)
{
    unsigned int i;

    for (i=0; i<za->nopen_source; i++) {
        if (za->open_source[i] == src) {
            za->open_source[i] = za->open_source[za->nopen_source-1];
            za->nopen_source--;
            break;
        }
    }
}


int
_zip_register_source(zip_t *za, zip_source_t *src)
{
    zip_source_t **open_source;

    if (za->nopen_source+1 >= za->nopen_source_alloc) {
        unsigned int n;
        n = za->nopen_source_alloc + 10;
        open_source = (zip_source_t **)realloc(za->open_source, n*sizeof(zip_source_t *));
        if (open_source == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
        za->nopen_source_alloc = n;
        za->open_source = open_source;
    }

    za->open_source[za->nopen_source++] = src;

    return 0;
}
