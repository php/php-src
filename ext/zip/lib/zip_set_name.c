/*
  zip_set_name.c -- rename helper function
  Copyright (C) 1999-2012 Dieter Baron and Thomas Klausner

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



int
_zip_set_name(struct zip *za, zip_uint64_t idx, const char *name, zip_flags_t flags)
{
    struct zip_entry *e;
    struct zip_string *str;
    int changed;
    zip_int64_t i;

    if (idx >= za->nentry) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return -1;
    }

    if (ZIP_IS_RDONLY(za)) {
	_zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
	return -1;
    }

    if (name && name[0] != '\0') {
        /* TODO: check for string too long */
	if ((str=_zip_string_new((const zip_uint8_t *)name, (zip_uint16_t)strlen(name), flags, &za->error)) == NULL)
	    return -1;
	if ((flags & ZIP_FL_ENCODING_ALL) == ZIP_FL_ENC_GUESS && _zip_guess_encoding(str, ZIP_ENCODING_UNKNOWN) == ZIP_ENCODING_UTF8_GUESSED)
	    str->encoding = ZIP_ENCODING_UTF8_KNOWN;
    }
    else
	str = NULL;

    /* TODO: encoding flags needed for CP437? */
    if ((i=_zip_name_locate(za, name, 0, NULL)) >= 0 && (zip_uint64_t)i != idx) {
	_zip_string_free(str);
	_zip_error_set(&za->error, ZIP_ER_EXISTS, 0);
	return -1;
    }

    /* no effective name change */
    if (i>=0 && (zip_uint64_t)i == idx) {
	_zip_string_free(str);
	return 0;
    }

    e = za->entry+idx;

    if (e->changes) {
	_zip_string_free(e->changes->filename);
	e->changes->filename = NULL;
	e->changes->changed &= ~ZIP_DIRENT_FILENAME;
    }

    if (e->orig)
	changed = !_zip_string_equal(e->orig->filename, str);
    else
	changed = 1;

    if (changed) {
        if (e->changes == NULL) {
            if ((e->changes=_zip_dirent_clone(e->orig)) == NULL) {
                _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
		_zip_string_free(str);
                return -1;
            }
        }
        e->changes->filename = str;
        e->changes->changed |= ZIP_DIRENT_FILENAME;
    }
    else {
	_zip_string_free(str);
	if (e->changes && e->changes->changed == 0) {
	    _zip_dirent_free(e->changes);
	    e->changes = NULL;
	}
    }

    return 0;
}
