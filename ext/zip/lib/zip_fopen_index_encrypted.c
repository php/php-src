/*
  zip_fopen_index_encrypted.c -- open file for reading by index w/ password
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



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "zipint.h"

static struct zip_file *_zip_file_new(struct zip *za);



ZIP_EXTERN(struct zip_file *)
zip_fopen_index_encrypted(struct zip *za, zip_uint64_t fileno, int flags,
			  const char *password)
{
    struct zip_file *zf;
    zip_compression_implementation comp_impl;
    zip_encryption_implementation enc_impl;
    struct zip_source *src, *s2;
    zip_uint64_t start;
    struct zip_stat st;

    if (fileno >= za->nentry) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((flags & ZIP_FL_UNCHANGED) == 0
	&& ZIP_ENTRY_DATA_CHANGED(za->entry+fileno)) {
	_zip_error_set(&za->error, ZIP_ER_CHANGED, 0);
	return NULL;
    }

    if (fileno >= za->cdir->nentry) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if (flags & ZIP_FL_ENCRYPTED)
	flags |= ZIP_FL_COMPRESSED;

    zip_stat_index(za, fileno, flags, &st);

    enc_impl = NULL;
    if ((flags & ZIP_FL_ENCRYPTED) == 0) {
	if (st.encryption_method != ZIP_EM_NONE) {
	    if (password == NULL) {
		_zip_error_set(&za->error, ZIP_ER_NOPASSWD, 0);
		return NULL;
	    }
	    if ((enc_impl=zip_get_encryption_implementation(
		     st.encryption_method)) == NULL) {
		_zip_error_set(&za->error, ZIP_ER_ENCRNOTSUPP, 0);
		return NULL;
	    }
	}
    }

    comp_impl = NULL;
    if ((flags & ZIP_FL_COMPRESSED) == 0) {
	if (st.comp_method != ZIP_CM_STORE) {
	    if ((comp_impl=zip_get_compression_implementation(
		     st.comp_method)) == NULL) {
		_zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
		return NULL;
	    }
	}
    }

    if ((start=_zip_file_get_offset(za, fileno)) == 0)
	return NULL;

    if (st.comp_size == 0) {
	if ((src=zip_source_buffer(za, NULL, 0, 0)) == NULL)
	    return NULL;
    }
    else {
	if ((src=_zip_source_file_or_p(za, NULL, za->zp, start, st.comp_size,
				       0, &st)) == NULL)
	    return NULL;
	if (enc_impl) {
	    if ((s2=enc_impl(za, src, ZIP_EM_TRAD_PKWARE, 0,
			     password)) == NULL) {
		zip_source_free(src);
		/* XXX: set error (how?) */
		return NULL;
	    }
	    src = s2;
	}
	if (comp_impl) {
	    if ((s2=comp_impl(za, src, za->cdir->entry[fileno].comp_method,
			      0)) == NULL) {
		zip_source_free(src);
		/* XXX: set error (how?) */
		return NULL;
	    }
	    src = s2;
	}
	if ((flags & ZIP_FL_COMPRESSED) == 0
	    || st.comp_method == ZIP_CM_STORE ) {
	    if ((s2=zip_source_crc(za, src, 1)) == NULL) {
		zip_source_free(src);
		/* XXX: set error (how?) */
		return NULL;
	    }
	    src = s2;
	}
    }

    if (zip_source_open(src) < 0) {
	_zip_error_set_from_source(&za->error, src);
	zip_source_free(src);
	return NULL;
    }

    zf = _zip_file_new(za);

    zf->src = src;

    return zf;
}



static struct zip_file *
_zip_file_new(struct zip *za)
{
    struct zip_file *zf, **file;
    int n;

    if ((zf=(struct zip_file *)malloc(sizeof(struct zip_file))) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }
    
    if (za->nfile >= za->nfile_alloc-1) {
	n = za->nfile_alloc + 10;
	file = (struct zip_file **)realloc(za->file,
					   n*sizeof(struct zip_file *));
	if (file == NULL) {
	    _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	    free(zf);
	    return NULL;
	}
	za->nfile_alloc = n;
	za->file = file;
    }

    za->file[za->nfile++] = zf;

    zf->za = za;
    _zip_error_init(&zf->error);
    zf->eof = 0;
    zf->src = NULL;

    return zf;
}
