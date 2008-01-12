/*
  $NiH: zip_fopen_index.c,v 1.24 2005/05/20 21:54:53 wiz Exp $

  zip_fopen_index.c -- open file in zip archive for reading by index
  Copyright (C) 1999, 2004, 2005 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <nih@giga.or.at>

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

#include "zip.h"
#include "zipint.h"

static struct zip_file *_zip_file_new(struct zip *za);



PHPZIPAPI struct zip_file *
zip_fopen_index(struct zip *za, int fileno, int flags)
{
    int len, ret;
    int zfflags;
    struct zip_file *zf;

    if ((fileno < 0) || (fileno >= za->nentry)) {
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

    zfflags = 0;
    switch (za->cdir->entry[fileno].comp_method) {
    case ZIP_CM_STORE:
	zfflags |= ZIP_ZF_CRC;
	break;

    case ZIP_CM_DEFLATE:
	if ((flags & ZIP_FL_COMPRESSED) == 0)
	    zfflags |= ZIP_ZF_CRC | ZIP_ZF_DECOMP;
	break;
    default:
	if ((flags & ZIP_FL_COMPRESSED) == 0) {
	    _zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
	    return NULL;
	}
	break;
    }

    zf = _zip_file_new(za);

    zf->flags = zfflags;
    /* zf->name = za->cdir->entry[fileno].filename; */
    zf->method = za->cdir->entry[fileno].comp_method;
    zf->bytes_left = za->cdir->entry[fileno].uncomp_size;
    zf->cbytes_left = za->cdir->entry[fileno].comp_size;
    zf->crc_orig = za->cdir->entry[fileno].crc;

    if ((zf->fpos=_zip_file_get_offset(za, fileno)) == 0) {
	zip_fclose(zf);
	return NULL;
    }
    
    if ((zf->flags & ZIP_ZF_DECOMP) == 0)
	zf->bytes_left = zf->cbytes_left;
    else {
	if ((zf->buffer=(char *)malloc(BUFSIZE)) == NULL) {
	    _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	    zip_fclose(zf);
	    return NULL;
	}

	len = _zip_file_fillbuf(zf->buffer, BUFSIZE, zf);
	if (len <= 0) {
	    _zip_error_copy(&za->error, &zf->error);
	    zip_fclose(zf);
	return NULL;
	}

	if ((zf->zstr = (z_stream *)malloc(sizeof(z_stream))) == NULL) {
	    _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	    zip_fclose(zf);
	    return NULL;
	}
	zf->zstr->zalloc = Z_NULL;
	zf->zstr->zfree = Z_NULL;
	zf->zstr->opaque = NULL;
	zf->zstr->next_in = (Bytef *)zf->buffer;
	zf->zstr->avail_in = len;
	
	/* negative value to tell zlib that there is no header */
	if ((ret=inflateInit2(zf->zstr, -MAX_WBITS)) != Z_OK) {
	    _zip_error_set(&za->error, ZIP_ER_ZLIB, ret);
	    zip_fclose(zf);
	    return NULL;
	}
    }
    
    return zf;
}



PHPZIPAPI int
_zip_file_fillbuf(void *buf, size_t buflen, struct zip_file *zf)
{
    int i, j;

    if (zf->error.zip_err != ZIP_ER_OK)
	return -1;

    if ((zf->flags & ZIP_ZF_EOF) || zf->cbytes_left <= 0 || buflen <= 0)
	return 0;
    
    if (fseek(zf->za->zp, zf->fpos, SEEK_SET) < 0) {
	_zip_error_set(&zf->error, ZIP_ER_SEEK, errno);
	return -1;
    }
    if (buflen < zf->cbytes_left)
	i = buflen;
    else
	i = zf->cbytes_left;

    j = fread(buf, 1, i, zf->za->zp);
    if (j == 0) {
	_zip_error_set(&zf->error, ZIP_ER_EOF, 0);
	j = -1;
    }
    else if (j < 0)
	_zip_error_set(&zf->error, ZIP_ER_READ, errno);
    else {
	zf->fpos += j;
	zf->cbytes_left -= j;
    }

    return j;	
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
    zf->flags = 0;
    zf->crc = crc32(0L, Z_NULL, 0);
    zf->crc_orig = 0;
    zf->method = -1;
    zf->bytes_left = zf->cbytes_left = 0;
    zf->fpos = 0;
    zf->buffer = NULL;
    zf->zstr = NULL;

    return zf;
}
