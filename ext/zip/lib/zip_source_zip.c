/*
  zip_source_zip.c -- create data source from zip file
  Copyright (C) 1999-2007 Dieter Baron and Thomas Klausner

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

struct read_zip {
    struct zip_file *zf;
    struct zip_stat st;
    off_t off, len;
};

static ssize_t read_zip(void *st, void *data, size_t len,
			enum zip_source_cmd cmd);



ZIP_EXTERN(struct zip_source *)
zip_source_zip(struct zip *za, struct zip *srcza, int srcidx, int flags,
	       off_t start, off_t len)
{
    struct zip_error error;
    struct zip_source *zs;
    struct read_zip *p;

    /* XXX: ZIP_FL_RECOMPRESS */

    if (za == NULL)
	return NULL;

    if (srcza == NULL || start < 0 || len < -1 || srcidx < 0 || srcidx >= srcza->nentry) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((flags & ZIP_FL_UNCHANGED) == 0
	&& ZIP_ENTRY_DATA_CHANGED(srcza->entry+srcidx)) {
	_zip_error_set(&za->error, ZIP_ER_CHANGED, 0);
	return NULL;
    }

    if (len == 0)
	len = -1;

    if (start == 0 && len == -1 && (flags & ZIP_FL_RECOMPRESS) == 0)
	flags |= ZIP_FL_COMPRESSED;
    else
	flags &= ~ZIP_FL_COMPRESSED;

    if ((p=(struct read_zip *)malloc(sizeof(*p))) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }
	
    _zip_error_copy(&error, &srcza->error);
	
    if (zip_stat_index(srcza, srcidx, flags, &p->st) < 0
	|| (p->zf=zip_fopen_index(srcza, srcidx, flags)) == NULL) {
	free(p);
	_zip_error_copy(&za->error, &srcza->error);
	_zip_error_copy(&srcza->error, &error);
	
	return NULL;
    }
    p->off = start;
    p->len = len;

    if ((flags & ZIP_FL_COMPRESSED) == 0) {
	p->st.size = p->st.comp_size = len;
	p->st.comp_method = ZIP_CM_STORE;
	p->st.crc = 0;
    }
    
    if ((zs=zip_source_function(za, read_zip, p)) == NULL) {
	free(p);
	return NULL;
    }

    return zs;
}



static ssize_t
read_zip(void *state, void *data, size_t len, enum zip_source_cmd cmd)
{
    struct read_zip *z;
    char b[8192], *buf;
    int i, n;

    z = (struct read_zip *)state;
    buf = (char *)data;

    switch (cmd) {
    case ZIP_SOURCE_OPEN:
	for (n=0; n<z->off; n+= i) {
	    i = (z->off-n > sizeof(b) ? sizeof(b) : z->off-n);
	    if ((i=zip_fread(z->zf, b, i)) < 0) {
		zip_fclose(z->zf);
		z->zf = NULL;
		return -1;
	    }
	}
	return 0;
	
    case ZIP_SOURCE_READ:
	if (z->len != -1)
	    n = len > z->len ? z->len : len;
	else
	    n = len;
	

	if ((i=zip_fread(z->zf, buf, n)) < 0)
	    return -1;

	if (z->len != -1)
	    z->len -= i;

	return i;
	
    case ZIP_SOURCE_CLOSE:
	return 0;

    case ZIP_SOURCE_STAT:
	if (len < sizeof(z->st))
	    return -1;
	len = sizeof(z->st);

	memcpy(data, &z->st, len);
	return len;

    case ZIP_SOURCE_ERROR:
	{
	    int *e;

	    if (len < sizeof(int)*2)
		return -1;

	    e = (int *)data;
	    zip_file_error_get(z->zf, e, e+1);
	}
	return sizeof(int)*2;

    case ZIP_SOURCE_FREE:
	zip_fclose(z->zf);
	free(z);
	return 0;

    default:
	;
    }

    return -1;
}
