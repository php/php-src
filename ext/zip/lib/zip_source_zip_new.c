/*
  zip_source_zip_new.c -- prepare data structures for zip_fopen/zip_source_zip
  Copyright (C) 2012 Dieter Baron and Thomas Klausner

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

#include "zipint.h"



struct zip_source *
_zip_source_zip_new(struct zip *za, struct zip *srcza, zip_uint64_t srcidx, zip_flags_t flags,
		    zip_uint64_t start, zip_uint64_t len, const char *password)
{
    zip_compression_implementation comp_impl;
    zip_encryption_implementation enc_impl;
    struct zip_source *src, *s2;
    zip_uint64_t offset;
    struct zip_stat st;

    if (za == NULL)
	return NULL;

    if (srcza == NULL ||  srcidx >= srcza->nentry) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((flags & ZIP_FL_UNCHANGED) == 0
	&& (ZIP_ENTRY_DATA_CHANGED(srcza->entry+srcidx) || srcza->entry[srcidx].deleted)) {
	_zip_error_set(&za->error, ZIP_ER_CHANGED, 0);
	return NULL;
    }

    if (zip_stat_index(srcza, srcidx, flags|ZIP_FL_UNCHANGED, &st) < 0) {
	_zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
	return NULL;
    }

    if (flags & ZIP_FL_ENCRYPTED)
	flags |= ZIP_FL_COMPRESSED;

    if ((start > 0 || len > 0) && (flags & ZIP_FL_COMPRESSED)) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    /* overflow or past end of file */
    if ((start > 0 || len > 0) && (start+len < start || start+len > st.size)) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    enc_impl = NULL;
    if (((flags & ZIP_FL_ENCRYPTED) == 0) && (st.encryption_method != ZIP_EM_NONE)) {
	if (password == NULL) {
	    _zip_error_set(&za->error, ZIP_ER_NOPASSWD, 0);
	    return NULL;
	}
	if ((enc_impl=_zip_get_encryption_implementation(st.encryption_method)) == NULL) {
	    _zip_error_set(&za->error, ZIP_ER_ENCRNOTSUPP, 0);
	    return NULL;
	}
    }

    comp_impl = NULL;
    if ((flags & ZIP_FL_COMPRESSED) == 0) {
	if (st.comp_method != ZIP_CM_STORE) {
	    if ((comp_impl=_zip_get_compression_implementation(st.comp_method)) == NULL) {
		_zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
		return NULL;
	    }
	}
    }

    if ((offset=_zip_file_get_offset(srcza, srcidx, &za->error)) == 0)
	return NULL;

    if (st.comp_size == 0) {
	if ((src=zip_source_buffer(za, NULL, 0, 0)) == NULL)
	    return NULL;
    }
    else {
	if (start+len > 0 && enc_impl == NULL && comp_impl == NULL) {
	    struct zip_stat st2;

	    st2.size = len ? len : st.size-start;
	    st2.comp_size = st2.size;
	    st2.comp_method = ZIP_CM_STORE;
	    st2.mtime = st.mtime;
	    st2.valid = ZIP_STAT_SIZE|ZIP_STAT_COMP_SIZE|ZIP_STAT_COMP_METHOD|ZIP_STAT_MTIME;

            /* TODO: check for overflow of st2.size */
	    if ((src=_zip_source_file_or_p(za, NULL, srcza->zp, offset+start, (zip_int64_t)st2.size, 0, &st2)) == NULL)
		return NULL;
	}
	else {
            /* TODO: check for overflow of st.comp_size */
	    if ((src=_zip_source_file_or_p(za, NULL, srcza->zp, offset, (zip_int64_t)st.comp_size, 0, &st)) == NULL)
		return NULL;
	}

	if (enc_impl) {
	    if ((s2=enc_impl(za, src, st.encryption_method, 0, password)) == NULL) {
		zip_source_free(src);
		/* TODO: set error (how?) */
		return NULL;
	    }
	    src = s2;
	}
	if (comp_impl) {
	    if ((s2=comp_impl(za, src, st.comp_method, 0)) == NULL) {
		zip_source_free(src);
		/* TODO: set error (how?) */
		return NULL;
	    }
	    src = s2;
	}
	if (((flags & ZIP_FL_COMPRESSED) == 0 || st.comp_method == ZIP_CM_STORE)
	    && (len == 0 || len == st.comp_size)) {
	    /* when reading the whole file, check for crc errors */
	    if ((s2=zip_source_crc(za, src, 1)) == NULL) {
		zip_source_free(src);
		/* TODO: set error (how?) */
		return NULL;
	    }
	    src = s2;
	}

	if (start+len > 0 && (comp_impl || enc_impl)) {
	    if ((s2=zip_source_window(za, src, start, len ? len : st.size-start)) == NULL) {
		zip_source_free(src);
		/* TODO: set error (how?) (why?) */
		return NULL;
	    }
	    src = s2;
	}
    }

    return src;
}
