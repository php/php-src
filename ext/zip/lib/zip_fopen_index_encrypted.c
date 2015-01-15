/*
  zip_fopen_index_encrypted.c -- open file for reading by index w/ password
  Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

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



ZIP_EXTERN struct zip_file *
zip_fopen_index_encrypted(struct zip *za, zip_uint64_t index, zip_flags_t flags,
			  const char *password)
{
    struct zip_file *zf;
    struct zip_source *src;

    if ((src=_zip_source_zip_new(za, za, index, flags, 0, 0, password)) == NULL)
	return NULL;

    if (zip_source_open(src) < 0) {
	_zip_error_set_from_source(&za->error, src);
	zip_source_free(src);
	return NULL;
    }

    if ((zf=_zip_file_new(za)) == NULL) {
	zip_source_free(src);
	return NULL;
    }

    zf->src = src;

    return zf;
}



static struct zip_file *
_zip_file_new(struct zip *za)
{
    struct zip_file *zf, **file;

    if ((zf=(struct zip_file *)malloc(sizeof(struct zip_file))) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }
    
    if (za->nfile+1 >= za->nfile_alloc) {
	unsigned int n;
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
