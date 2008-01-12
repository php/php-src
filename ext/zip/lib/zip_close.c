/*
  $NiH: zip_close.c,v 1.60 2006/05/09 17:21:47 wiz Exp $

  zip_close.c -- close zip archive and update changes
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "zip.h"
#include "zipint.h"

static int add_data(struct zip *, int, struct zip_dirent *, FILE *);
static int add_data_comp(zip_source_callback, void *, struct zip_stat *,
			 FILE *, struct zip_error *);
static int add_data_uncomp(zip_source_callback, void *, struct zip_stat *,
			   FILE *, struct zip_error *);
static void ch_set_error(struct zip_error *, zip_source_callback, void *);
static int copy_data(FILE *, off_t, FILE *, struct zip_error *);
static int _zip_cdir_set_comment(struct zip_cdir *, struct zip *);
static int _zip_changed(struct zip *, int *);
static char *_zip_create_temp_output(struct zip *, FILE **);

PHPZIPAPI int
zip_close(struct zip *za)
{
    int survivors;
    int i, j, error;
    char *temp;
    FILE *out;
    mode_t mask;
    struct zip_cdir *cd;
    struct zip_dirent de;
		int rename_error = 0;

    if (!_zip_changed(za, &survivors)) {
	_zip_free(za);
	return 0;
    }

    /* don't create zip files with no entries */
    if (survivors == 0) {
	if (za->zn) {
	    if (remove(za->zn) != 0) {
		_zip_error_set(&za->error, ZIP_ER_REMOVE, errno);
		return -1;
	    }
	}
	_zip_free(za);
	return 0;
    }

    if ((cd=_zip_cdir_new(survivors, &za->error)) == NULL)
	return -1;

    for (i=0; i<survivors; i++)
	_zip_dirent_init(&cd->entry[i]);

    if (_zip_cdir_set_comment(cd, za) == -1) {
	_zip_cdir_free(cd);
	return -1;
    }

    if ((temp=_zip_create_temp_output(za, &out)) == NULL) {
	_zip_cdir_free(cd);
	return -1;
    }

    error = 0;
    for (i=j=0; i<za->nentry; i++) {
	if (za->entry[i].state == ZIP_ST_DELETED)
	    continue;

	/* create new local directory entry */
	if (ZIP_ENTRY_DATA_CHANGED(za->entry+i)) {
	    _zip_dirent_init(&de);
	    /* use it as central directory entry */
	    memcpy(cd->entry+j, &de, sizeof(cd->entry[j]));

	    /* set/update file name */
	    if (za->entry[i].ch_filename == NULL) {
		if (za->entry[i].state == ZIP_ST_REPLACED) {
		    de.filename = strdup(za->cdir->entry[i].filename);
		    de.filename_len = strlen(de.filename);
		    cd->entry[j].filename = za->cdir->entry[i].filename;
		    cd->entry[j].filename_len = de.filename_len;
		}
		else {
		    de.filename = strdup("-");
		    de.filename_len = 1;
		    cd->entry[j].filename = "-";
		    cd->entry[j].filename_len = de.filename_len;
		}
	    }
	}
	else {
	    /* copy existing directory entries */
	    if (fseek(za->zp, za->cdir->entry[i].offset, SEEK_SET) != 0) {
		_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
		error = 1;
		break;
	    }
	    if (_zip_dirent_read(&de, za->zp, NULL, 0, 1, &za->error) != 0) {
		error = 1;
		break;
	    }

		if (de.bitflags & ZIP_GPBF_USE_DATA_DESCRIPTOR) {
			de.crc = (za->cdir->entry+i)->crc;
			de.comp_size = (za->cdir->entry+i)->comp_size;
			de.uncomp_size = (za->cdir->entry+i)->uncomp_size;
		}
	    memcpy(cd->entry+j, za->cdir->entry+i, sizeof(cd->entry[j]));
	}

	if (za->entry[i].ch_filename) {
	    free(de.filename);
	    if ((de.filename=strdup(za->entry[i].ch_filename)) == NULL) {
		error = 1;
		break;
	    }
	    de.filename_len = strlen(de.filename);
	    cd->entry[j].filename = za->entry[i].ch_filename;
	    cd->entry[j].filename_len = de.filename_len;
	}

	if (za->entry[i].ch_comment_len != -1) {
	    /* as the rest of cd entries, its malloc/free is done by za */
	    cd->entry[j].comment = za->entry[i].ch_comment;
	    cd->entry[j].comment_len = za->entry[i].ch_comment_len;
	}

	cd->entry[j].offset = ftell(out);

	if (ZIP_ENTRY_DATA_CHANGED(za->entry+i)) {
	    if (add_data(za, i, &de, out) < 0) {
		error = 1;
		break;
	    }

	    cd->entry[j].last_mod = de.last_mod;
	    cd->entry[j].comp_method = de.comp_method;
	    cd->entry[j].comp_size = de.comp_size;
	    cd->entry[j].uncomp_size = de.uncomp_size;
	    cd->entry[j].crc = de.crc;
	}
	else {
	    if (_zip_dirent_write(&de, out, 1, &za->error) < 0) {
		error = 1;
		break;
	    }
	    /* we just read the local dirent, file is at correct position */
	    if (copy_data(za->zp, de.comp_size, out, &za->error) < 0) {
		error = 1;
		break;
	    }
	}

	j++;

	_zip_dirent_finalize(&de);
    }

    if (!error) {
	if (_zip_cdir_write(cd, out, &za->error) < 0)
	    error = 1;
    }

    /* pointers in cd entries are owned by za */
    cd->nentry = 0;
    _zip_cdir_free(cd);

    if (error) {
	_zip_dirent_finalize(&de);
	fclose(out);
	remove(temp);
	free(temp);
	return -1;
    }

    if (fclose(out) != 0) {
	_zip_error_set(&za->error, ZIP_ER_CLOSE, errno);
	remove(temp);
	free(temp);
	return -1;
    }

   if (za->zp) {
	fclose(za->zp);
	za->zp = NULL;
    }

#ifdef PHP_WIN32 
	if (!MoveFileEx(temp, za->zn, MOVEFILE_REPLACE_EXISTING)) {
		rename_error = -1;
	}
#else
	if (rename(temp, za->zn) != 0) {
		rename_error = -1;
	}
#endif

	if (rename_error < 0) {
		_zip_error_set(&za->error, ZIP_ER_RENAME, errno);
		remove(temp);
		free(temp);
		return -1;
	}

    mask = umask(0);
    umask(mask);
    chmod(za->zn, 0666&~mask);

    _zip_free(za);
	free(temp);
    return 0;
}



static int
add_data(struct zip *za, int idx, struct zip_dirent *de, FILE *ft)
{
    off_t offstart, offend;
    zip_source_callback cb;
    void *ud;
    struct zip_stat st;

    cb = za->entry[idx].source->f;
    ud = za->entry[idx].source->ud;

    if (cb(ud, &st, sizeof(st), ZIP_SOURCE_STAT) < (ssize_t)sizeof(st)) {
	ch_set_error(&za->error, cb, ud);
	return -1;
    }

    if (cb(ud, NULL, 0, ZIP_SOURCE_OPEN) < 0) {
	ch_set_error(&za->error, cb, ud);
	return -1;
    }

    offstart = ftell(ft);

    if (_zip_dirent_write(de, ft, 1, &za->error) < 0)
	return -1;

    if (st.comp_method != ZIP_CM_STORE) {
	if (add_data_comp(cb, ud, &st, ft, &za->error) < 0)
	    return -1;
    }
    else {
	if (add_data_uncomp(cb, ud, &st, ft, &za->error) < 0)
	    return -1;
    }

    if (cb(ud, NULL, 0, ZIP_SOURCE_CLOSE) < 0) {
	ch_set_error(&za->error, cb, ud);
	return -1;
    }

    offend = ftell(ft);

    if (fseek(ft, offstart, SEEK_SET) < 0) {
	_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
	return -1;
    }

    de->comp_method = st.comp_method;
    de->last_mod = st.mtime;
    de->crc = st.crc;
    de->uncomp_size = st.size;
    de->comp_size = st.comp_size;

    if (_zip_dirent_write(de, ft, 1, &za->error) < 0)
	return -1;

    if (fseek(ft, offend, SEEK_SET) < 0) {
	_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
	return -1;
    }

    return 0;
}



static int
add_data_comp(zip_source_callback cb, void *ud, struct zip_stat *st,FILE *ft,
	      struct zip_error *error)
{
    char buf[BUFSIZE];
    ssize_t n;

    st->comp_size = 0;
    while ((n=cb(ud, buf, sizeof(buf), ZIP_SOURCE_READ)) > 0) {
	if (fwrite(buf, 1, n, ft) != (size_t)n) {
	    _zip_error_set(error, ZIP_ER_WRITE, errno);
	    return -1;
	}

	st->comp_size += n;
    }
    if (n < 0) {
	ch_set_error(error, cb, ud);
	return -1;
    }

    return 0;
}



static int
add_data_uncomp(zip_source_callback cb, void *ud, struct zip_stat *st,
		FILE *ft, struct zip_error *error)
{
    char b1[BUFSIZE], b2[BUFSIZE];
    int end, flush, ret;
    ssize_t n;
    size_t n2;
    z_stream zstr;

    st->comp_method = ZIP_CM_DEFLATE;
    st->comp_size = st->size = 0;
    st->crc = crc32(0, NULL, 0);

    zstr.zalloc = Z_NULL;
    zstr.zfree = Z_NULL;
    zstr.opaque = NULL;
    zstr.avail_in = 0;
    zstr.avail_out = 0;

    /* -15: undocumented feature of zlib to _not_ write a zlib header */
    deflateInit2(&zstr, Z_BEST_COMPRESSION, Z_DEFLATED, -15, 9,
		 Z_DEFAULT_STRATEGY);

    zstr.next_out = (Bytef *)b2;
    zstr.avail_out = sizeof(b2);
    zstr.avail_in = 0;

    flush = 0;
    end = 0;
    while (!end) {
	if (zstr.avail_in == 0 && !flush) {
	    if ((n=cb(ud, b1, sizeof(b1), ZIP_SOURCE_READ)) < 0) {
		ch_set_error(error, cb, ud);
		deflateEnd(&zstr);
		return -1;
	    }
	    if (n > 0) {
		zstr.avail_in = n;
		zstr.next_in = (Bytef *)b1;
		st->size += n;
		st->crc = crc32(st->crc, (Bytef *)b1, n);
	    }
	    else
		flush = Z_FINISH;
	}

	ret = deflate(&zstr, flush);
	if (ret != Z_OK && ret != Z_STREAM_END) {
	    _zip_error_set(error, ZIP_ER_ZLIB, ret);
	    return -1;
	}

	if (zstr.avail_out != sizeof(b2)) {
	    n2 = sizeof(b2) - zstr.avail_out;

	    if (fwrite(b2, 1, n2, ft) != n2) {
		_zip_error_set(error, ZIP_ER_WRITE, errno);
		return -1;
	    }

	    zstr.next_out = (Bytef *)b2;
	    zstr.avail_out = sizeof(b2);
	    st->comp_size += n2;
	}

	if (ret == Z_STREAM_END) {
	    deflateEnd(&zstr);
	    end = 1;
	}
    }

    return 0;
}



static void
ch_set_error(struct zip_error *error, zip_source_callback cb, void *ud)
{
    int e[2];

    if ((cb(ud, e, sizeof(e), ZIP_SOURCE_ERROR)) < (ssize_t)sizeof(e)) {
	error->zip_err = ZIP_ER_INTERNAL;
	error->sys_err = 0;
    }
    else {
	error->zip_err = e[0];
	error->sys_err = e[1];
    }
}



static int
copy_data(FILE *fs, off_t len, FILE *ft, struct zip_error *error)
{
    char buf[BUFSIZE];
    int n, nn;

    if (len == 0)
	return 0;

    while (len > 0) {
	nn = len > sizeof(buf) ? sizeof(buf) : len;
	if ((n=fread(buf, 1, nn, fs)) < 0) {
	    _zip_error_set(error, ZIP_ER_READ, errno);
	    return -1;
	}
	else if (n == 0) {
	    _zip_error_set(error, ZIP_ER_EOF, 0);
	    return -1;
	}

	if (fwrite(buf, 1, n, ft) != (size_t)n) {
	    _zip_error_set(error, ZIP_ER_WRITE, errno);
	    return -1;
	}

	len -= n;
    }

    return 0;
}



static int
_zip_cdir_set_comment(struct zip_cdir *dest, struct zip *src)
{
    if (src->ch_comment_len != -1) {
	dest->comment = _zip_memdup(src->ch_comment,
				    src->ch_comment_len, &src->error);
	if (dest->comment == NULL)
	    return -1;
	dest->comment_len = src->ch_comment_len;
    } else {
	if (src->cdir && src->cdir->comment) {
	    dest->comment = _zip_memdup(src->cdir->comment,
					src->cdir->comment_len, &src->error);
	    if (dest->comment == NULL)
		return -1;
	    dest->comment_len = src->cdir->comment_len;
	}
    }

    return 0;
}



static int
_zip_changed(struct zip *za, int *survivorsp)
{
    int changed, i, survivors;

    changed = survivors = 0;

    if (za->ch_comment_len != -1)
	changed = 1;

    for (i=0; i<za->nentry; i++) {
	if ((za->entry[i].state != ZIP_ST_UNCHANGED)
	    || (za->entry[i].ch_comment_len != -1))
	    changed = 1;
	if (za->entry[i].state != ZIP_ST_DELETED)
	    survivors++;
    }

    *survivorsp = survivors;

    return changed;
}



static char *
_zip_create_temp_output(struct zip *za, FILE **outp)
{
    char *temp;
    int tfd;
    FILE *tfp;
    int len = strlen(za->zn) + 8;

    if ((temp=(char *)malloc(len)) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    snprintf(temp, len, "%s.XXXXXX", za->zn);

    if ((tfd=mkstemp(temp)) == -1) {
	_zip_error_set(&za->error, ZIP_ER_TMPOPEN, errno);
	free(temp);
	return NULL;
    }

    if ((tfp=fdopen(tfd, "r+b")) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_TMPOPEN, errno);
	close(tfd);
	remove(temp);
	free(temp);
	return NULL;
    }
#ifdef PHP_WIN32
	_setmode(_fileno(tfp), _O_BINARY );
#endif

    *outp = tfp;
    return temp;
}
