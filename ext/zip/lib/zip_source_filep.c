/*
  zip_source_filep.c -- create data source from FILE *
  Copyright (C) 1999-2009 Dieter Baron and Thomas Klausner

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



#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"

struct read_file {
    char *fname;	/* name of file to copy from */
    FILE *f;		/* file to copy from */
    int closep;		/* close f */
    struct zip_stat st;	/* stat information passed in */

    zip_uint64_t off;	/* start offset of */
    zip_int64_t len;	/* length of data to copy */
    zip_int64_t remain;	/* bytes remaining to be copied */
    int e[2];		/* error codes */
};

static zip_int64_t read_file(void *state, void *data, zip_uint64_t len,
		     enum zip_source_cmd cmd);



ZIP_EXTERN struct zip_source *
zip_source_filep(struct zip *za, FILE *file, zip_uint64_t start,
		 zip_int64_t len)
{
    if (za == NULL)
	return NULL;

    if (file == NULL || len < -1) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    return _zip_source_file_or_p(za, NULL, file, start, len, 1, NULL);
}



struct zip_source *
_zip_source_file_or_p(struct zip *za, const char *fname, FILE *file,
		      zip_uint64_t start, zip_int64_t len, int closep,
		      const struct zip_stat *st)
{
    struct read_file *f;
    struct zip_source *zs;

    if (file == NULL && fname == NULL) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((f=(struct read_file *)malloc(sizeof(struct read_file))) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    f->fname = NULL;
    if (fname) {
	if ((f->fname=strdup(fname)) == NULL) {
	    _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	    free(f);
	    return NULL;
	}
    }
    f->f = file;
    f->off = start;
    f->len = (len ? len : -1);
    f->closep = f->fname ? 1 : closep;
    if (st)
	memcpy(&f->st, st, sizeof(f->st));
    else
	zip_stat_init(&f->st);

    if ((zs=zip_source_function(za, read_file, f)) == NULL) {
	free(f);
	return NULL;
    }

    return zs;
}



static zip_int64_t
read_file(void *state, void *data, zip_uint64_t len, enum zip_source_cmd cmd)
{
    struct read_file *z;
    char *buf;
    size_t i, n;

    z = (struct read_file *)state;
    buf = (char *)data;

    switch (cmd) {
    case ZIP_SOURCE_OPEN:
	if (z->fname) {
	    if ((z->f=fopen(z->fname, "rb")) == NULL) {
		z->e[0] = ZIP_ER_OPEN;
		z->e[1] = errno;
		return -1;
	    }
	}

	if (z->closep && z->off > 0) {
	    if (fseeko(z->f, (off_t)z->off, SEEK_SET) < 0) {
		z->e[0] = ZIP_ER_SEEK;
		z->e[1] = errno;
		return -1;
	    }
	}
	z->remain = z->len;
	return 0;
	
    case ZIP_SOURCE_READ:
	/* XXX: return INVAL if len > size_t max */
	if (z->remain != -1)
	    n = len > (zip_uint64_t)z->remain ? (zip_uint64_t)z->remain : len;
	else
	    n = len;

	if (!z->closep) {
	    /* we might share this file with others, so let's be safe */
	    if (fseeko(z->f, (off_t)(z->off + (zip_uint64_t)(z->len-z->remain)), SEEK_SET) < 0) {
		z->e[0] = ZIP_ER_SEEK;
		z->e[1] = errno;
		return -1;
	    }
	}

	if ((i=fread(buf, 1, n, z->f)) == 0) {
            if (ferror(z->f)) {
                z->e[0] = ZIP_ER_READ;
                z->e[1] = errno;
                return -1;
            }
	}

	if (z->remain != -1)
	    z->remain -= i;

	return (zip_int64_t)i;
	
    case ZIP_SOURCE_CLOSE:
	if (z->fname) {
	    fclose(z->f);
	    z->f = NULL;
	}
	return 0;

    case ZIP_SOURCE_STAT:
        {
	    if (len < sizeof(z->st))
		return -1;

	    if (z->st.valid != 0)
		memcpy(data, &z->st, sizeof(z->st));
	    else {
		struct zip_stat *st;
		struct stat fst;
		int err;
	    
		if (z->f)
		    err = fstat(fileno(z->f), &fst);
		else
		    err = stat(z->fname, &fst);

		if (err != 0) {
		    z->e[0] = ZIP_ER_READ; /* best match */
		    z->e[1] = errno;
		    return -1;
		}

		st = (struct zip_stat *)data;
		
		zip_stat_init(st);
		st->mtime = fst.st_mtime;
		st->valid |= ZIP_STAT_MTIME;
		if (z->len != -1) {
		    st->size = (zip_uint64_t)z->len;
		    st->valid |= ZIP_STAT_SIZE;
		}
		else if ((fst.st_mode&S_IFMT) == S_IFREG) {
		    st->size = (zip_uint64_t)fst.st_size;
		    st->valid |= ZIP_STAT_SIZE;
		}
	    }
	    return sizeof(z->st);
	}

    case ZIP_SOURCE_ERROR:
	if (len < sizeof(int)*2)
	    return -1;

	memcpy(data, z->e, sizeof(int)*2);
	return sizeof(int)*2;

    case ZIP_SOURCE_FREE:
	free(z->fname);
	if (z->closep && z->f)
	    fclose(z->f);
	free(z);
	return 0;

    default:
	;
    }

    return -1;
}
