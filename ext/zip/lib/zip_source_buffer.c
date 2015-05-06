/*
  zip_source_buffer.c -- create zip data source from buffer
  Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"

#ifndef WRITE_FRAGMENT_SIZE
#define WRITE_FRAGMENT_SIZE 64*1024
#endif

struct buffer {
    zip_uint64_t fragment_size;		/* size of each fragment */

    zip_uint8_t **fragments;		/* pointers to fragments */
    zip_uint64_t nfragments;		/* number of allocated fragments */
    zip_uint64_t fragments_capacity;	/* size of fragments (number of pointers) */
    zip_uint64_t size;			/* size of data in bytes */
    zip_uint64_t offset;		/* current offset */
    int free_data;
};

typedef struct buffer buffer_t;

struct read_data {
    zip_error_t error;
    time_t mtime;
    buffer_t *in;
    buffer_t *out;
};

static void buffer_free(buffer_t *buffer);
static buffer_t *buffer_new(zip_uint64_t fragment_size);
static buffer_t *buffer_new_read(const void *data, zip_uint64_t length, int free_data);
static buffer_t *buffer_new_write(zip_uint64_t fragment_size);
static zip_int64_t buffer_read(buffer_t *buffer, zip_uint8_t *data, zip_uint64_t length);
static int buffer_seek(buffer_t *buffer, void *data, zip_uint64_t len, zip_error_t *error);
static zip_int64_t buffer_write(buffer_t *buffer, const zip_uint8_t *data, zip_uint64_t length, zip_error_t *);

static zip_int64_t read_data(void *, void *, zip_uint64_t, zip_source_cmd_t);


ZIP_EXTERN zip_source_t *
zip_source_buffer(zip_t *za, const void *data, zip_uint64_t len, int freep)
{
    if (za == NULL)
	return NULL;

    return zip_source_buffer_create(data, len, freep, &za->error);
}


ZIP_EXTERN zip_source_t *
zip_source_buffer_create(const void *data, zip_uint64_t len, int freep, zip_error_t *error)
{
    struct read_data *ctx;
    zip_source_t *zs;

    if (data == NULL && len > 0) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((ctx=(struct read_data *)malloc(sizeof(*ctx))) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    if ((ctx->in = buffer_new_read(data, len, freep)) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	free(ctx);
	return NULL;
    }

    ctx->out = NULL;
    ctx->mtime = time(NULL);
    zip_error_init(&ctx->error);
    
    if ((zs=zip_source_function_create(read_data, ctx, error)) == NULL) {
	buffer_free(ctx->in);
	free(ctx);
	return NULL;
    }

    return zs;
}


static zip_int64_t
read_data(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd)
{
    struct read_data *ctx = (struct read_data *)state;

    switch (cmd) {
        case ZIP_SOURCE_BEGIN_WRITE:
	    if ((ctx->out = buffer_new_write(WRITE_FRAGMENT_SIZE)) == NULL) {
		zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
		return -1;
	    }
	    return 0;

        case ZIP_SOURCE_CLOSE:
            return 0;
            
        case ZIP_SOURCE_COMMIT_WRITE:
	    buffer_free(ctx->in);
	    ctx->in = ctx->out;
	    ctx->out = NULL;
	    return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);
            
        case ZIP_SOURCE_FREE:
	    buffer_free(ctx->in);
	    buffer_free(ctx->out);
            free(ctx);
            return 0;
            
        case ZIP_SOURCE_OPEN:
	    ctx->in->offset = 0;
            return 0;
	
        case ZIP_SOURCE_READ:
	    if (len > ZIP_INT64_MAX) {
		zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
		return -1;
	    }
            return buffer_read(ctx->in, data, len);
	
        case ZIP_SOURCE_REMOVE:
	{
	    buffer_t *empty = buffer_new_read(NULL, 0, 0);
	    if (empty == 0) {
		zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
		return -1;
	    }
	
	    buffer_free(ctx->in);
	    ctx->in = empty;
	    return 0;
	}

        case ZIP_SOURCE_ROLLBACK_WRITE:
	    buffer_free(ctx->out);
	    ctx->out = NULL;
	    return 0;

        case ZIP_SOURCE_SEEK:
	    return buffer_seek(ctx->in, data, len, &ctx->error);

        case ZIP_SOURCE_SEEK_WRITE:
	    return buffer_seek(ctx->out, data, len, &ctx->error);
       
        case ZIP_SOURCE_STAT:
        {
            zip_stat_t *st;
	    
	    if (len < sizeof(*st)) {
                zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
		return -1;
	    }

	    st = (zip_stat_t *)data;

	    zip_stat_init(st);
	    st->mtime = ctx->mtime;
	    st->size = ctx->in->size;
	    st->comp_size = st->size;
	    st->comp_method = ZIP_CM_STORE;
	    st->encryption_method = ZIP_EM_NONE;
	    st->valid = ZIP_STAT_MTIME|ZIP_STAT_SIZE|ZIP_STAT_COMP_SIZE|ZIP_STAT_COMP_METHOD|ZIP_STAT_ENCRYPTION_METHOD;
	    
	    return sizeof(*st);
	}

        case ZIP_SOURCE_SUPPORTS:
	    return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT, ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, ZIP_SOURCE_SEEK, ZIP_SOURCE_TELL, ZIP_SOURCE_BEGIN_WRITE, ZIP_SOURCE_COMMIT_WRITE, ZIP_SOURCE_REMOVE, ZIP_SOURCE_ROLLBACK_WRITE, ZIP_SOURCE_SEEK_WRITE, ZIP_SOURCE_TELL_WRITE, ZIP_SOURCE_WRITE, -1);
            
        case ZIP_SOURCE_TELL:
            if (ctx->in->offset > ZIP_INT64_MAX) {
		zip_error_set(&ctx->error, ZIP_ER_TELL, EOVERFLOW);
		return -1;
	    }
	    return (zip_int64_t)ctx->in->offset;
		
            
        case ZIP_SOURCE_TELL_WRITE:
            if (ctx->out->offset > ZIP_INT64_MAX) {
		zip_error_set(&ctx->error, ZIP_ER_TELL, EOVERFLOW);
		return -1;
	    }
	    return (zip_int64_t)ctx->out->offset;

        case ZIP_SOURCE_WRITE:
	    if (len > ZIP_INT64_MAX) {
		zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
		return -1;
	    }
	    return buffer_write(ctx->out, data, len, &ctx->error);

        default:
	    zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}


static void
buffer_free(buffer_t *buffer)
{
    if (buffer == NULL) {
	return; 
    }

    if (buffer->free_data) {
	zip_uint64_t i;

	for (i=0; i < buffer->nfragments; i++) {
	    free(buffer->fragments[i]);
	}
    }
    free(buffer->fragments);
    free(buffer);
}


static buffer_t *
buffer_new(zip_uint64_t fragment_size)
{
    buffer_t *buffer;

    if ((buffer = malloc(sizeof(*buffer))) == NULL) {
	return NULL;
    }

    buffer->fragment_size = fragment_size;
    buffer->offset = 0;
    buffer->free_data = 0;
    buffer->nfragments = 0;
    buffer->fragments_capacity = 0;
    buffer->fragments = NULL;
    buffer->size = 0;

    return buffer;
}


static buffer_t *
buffer_new_read(const void *data, zip_uint64_t length, int free_data)
{
    buffer_t *buffer;

    if ((buffer = buffer_new(length)) == NULL) {
	return NULL;
    }

    buffer->size = length;

    if (length > 0) {
	if ((buffer->fragments = malloc(sizeof(*(buffer->fragments)))) == NULL) {
	    buffer_free(buffer);
	    return NULL;
	}
	buffer->fragments_capacity = 1;

	buffer->nfragments = 1;
	buffer->fragments[0] = (zip_uint8_t *)data;
	buffer->free_data = free_data;
    }

    return buffer;
}


static buffer_t *
buffer_new_write(zip_uint64_t fragment_size)
{
    buffer_t *buffer;

    if ((buffer = buffer_new(fragment_size)) == NULL) {
	return NULL;
    }

    if ((buffer->fragments = malloc(sizeof(*(buffer->fragments)))) == NULL) {
	buffer_free(buffer);
	return NULL;
    }
    buffer->fragments_capacity = 1;
    buffer->nfragments = 0;
    buffer->free_data = 1;

    return buffer;
}


static zip_int64_t
buffer_read(buffer_t *buffer, zip_uint8_t *data, zip_uint64_t length)
{
    zip_uint64_t n, i, fragment_offset;

    length = ZIP_MIN(length, buffer->size - buffer->offset);

    if (length == 0) {
	return 0;
    }
    if (length > ZIP_INT64_MAX) {
	return -1;
    }

    i = buffer->offset / buffer->fragment_size;
    fragment_offset = buffer->offset % buffer->fragment_size;
    n = 0;
    while (n < length) {
	zip_uint64_t left = ZIP_MIN(length - n, buffer->fragment_size - fragment_offset);
	
	memcpy(data + n, buffer->fragments[i] + fragment_offset, left);

	n += left;
	i++;
	fragment_offset = 0;
    }

    buffer->offset += n;
    return (zip_int64_t)n;
}


static int
buffer_seek(buffer_t *buffer, void *data, zip_uint64_t len, zip_error_t *error)
{
    zip_int64_t new_offset = zip_source_seek_compute_offset(buffer->offset, buffer->size, data, len, error);
    
    if (new_offset < 0) {
        return -1;
    }
    
    buffer->offset = (zip_uint64_t)new_offset;
    return 0;
}


static zip_int64_t
buffer_write(buffer_t *buffer, const zip_uint8_t *data, zip_uint64_t length, zip_error_t *error)
{
    zip_uint64_t n, i, fragment_offset;

    if (buffer->offset + length + buffer->fragment_size - 1 < length) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return -1;
    }

    /* grow buffer if needed */
    if (buffer->offset + length > buffer->nfragments * buffer->fragment_size) {
	zip_uint64_t needed_fragments = (buffer->offset + length + buffer->fragment_size - 1) / buffer->fragment_size;
	
	if (needed_fragments > buffer->fragments_capacity) {
	    zip_uint64_t new_capacity = buffer->fragments_capacity;
	    zip_uint8_t **fragments;

	    while (new_capacity < needed_fragments) {
		new_capacity *= 2;
	    }

	    fragments = realloc(buffer->fragments, new_capacity * sizeof(*fragments));

	    if (fragments == NULL) {
		zip_error_set(error, ZIP_ER_MEMORY, 0);
		return -1;
	    }

	    buffer->fragments = fragments;
	    buffer->fragments_capacity = new_capacity;
	}

	while (buffer->nfragments < needed_fragments) {
	    if ((buffer->fragments[buffer->nfragments] = malloc(buffer->fragment_size)) == NULL) {
		zip_error_set(error, ZIP_ER_MEMORY, 0);
		return -1;
	    }
	    buffer->nfragments++;
	}
    }

    i = buffer->offset / buffer->fragment_size;
    fragment_offset = buffer->offset % buffer->fragment_size;
    n = 0;
    while (n < length) {
	zip_uint64_t left = ZIP_MIN(length - n, buffer->fragment_size - fragment_offset);
		
	memcpy(buffer->fragments[i] + fragment_offset, data + n, left);

	n += left;
	i++;
	fragment_offset = 0;
    }

    buffer->offset += n;
    if (buffer->offset > buffer->size) {
	buffer->size = buffer->offset;
    }

    return (zip_int64_t)n;
}
