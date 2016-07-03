/*
  zip_close.c -- close zip archive and update changes
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



#include "zipint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif



/* max deflate size increase: size + ceil(size/16k)*5+6 */
#define MAX_DEFLATE_SIZE_32	4293656963u

static int add_data(struct zip *, struct zip_source *, struct zip_dirent *, FILE *);
static int copy_data(FILE *, zip_uint64_t, FILE *, struct zip_error *);
static int copy_source(struct zip *, struct zip_source *, FILE *);
static int write_cdir(struct zip *, const struct zip_filelist *, zip_uint64_t, FILE *);
static char *_zip_create_temp_output(struct zip *, FILE **);
static int _zip_torrentzip_cmp(const void *, const void *);



ZIP_EXTERN int
zip_close(struct zip *za)
{
    zip_uint64_t i, j, survivors;
    int error;
    char *temp;
    FILE *out;
#ifndef _WIN32
    mode_t mask;
#endif
    struct zip_filelist *filelist;
    int reopen_on_error;
    int new_torrentzip;
    int changed;

    reopen_on_error = 0;

    if (za == NULL)
	return -1;

    changed = _zip_changed(za, &survivors);

    /* don't create zip files with no entries */
    if (survivors == 0) {
	if (za->zn && ((za->open_flags & ZIP_TRUNCATE) || (changed && za->zp))) {
	    if (remove(za->zn) != 0) {
		_zip_error_set(&za->error, ZIP_ER_REMOVE, errno);
		return -1;
	    }
	}
	zip_discard(za);
	return 0;
    }	       

    if (!changed) {
	zip_discard(za);
	return 0;
    }

    if (survivors > za->nentry) {
        _zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }
    
    if ((filelist=(struct zip_filelist *)malloc(sizeof(filelist[0])*(size_t)survivors)) == NULL)
	return -1;

    /* archive comment is special for torrentzip */
    if (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0)) {
	/* TODO: use internal function when zip_set_archive_comment clears TORRENT flag */
	if (zip_set_archive_comment(za, TORRENT_SIG "XXXXXXXX", TORRENT_SIG_LEN + TORRENT_CRC_LEN) < 0) {
	    free(filelist);
	    return -1;
	}
    }
    /* TODO: if no longer torrentzip and archive comment not changed by user, delete it */


    /* create list of files with index into original archive  */
    for (i=j=0; i<za->nentry; i++) {
	if (za->entry[i].deleted)
	    continue;

        if (j >= survivors) {
            free(filelist);
            _zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
            return -1;
        }
        
	filelist[j].idx = i;
	filelist[j].name = zip_get_name(za, i, 0);
	j++;
    }
    if (j < survivors) {
        free(filelist);
        _zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }


    if ((temp=_zip_create_temp_output(za, &out)) == NULL) {
	free(filelist);
	return -1;
    }
    
    
    if (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0))
	qsort(filelist, (size_t)survivors, sizeof(filelist[0]),
	      _zip_torrentzip_cmp);

    new_torrentzip = (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0) == 1
		      && zip_get_archive_flag(za, ZIP_AFL_TORRENT,
					      ZIP_FL_UNCHANGED) == 0);
    error = 0;
    for (j=0; j<survivors; j++) {
	int new_data;
	struct zip_entry *entry;
	struct zip_dirent *de;

	i = filelist[j].idx;
	entry = za->entry+i;

	new_data = (ZIP_ENTRY_DATA_CHANGED(entry) || new_torrentzip || ZIP_ENTRY_CHANGED(entry, ZIP_DIRENT_COMP_METHOD));

	/* create new local directory entry */
	if (entry->changes == NULL) {
	    if ((entry->changes=_zip_dirent_clone(entry->orig)) == NULL) {
                _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                error = 1;
                break;
	    }
	}
	de = entry->changes;

	if (_zip_read_local_ef(za, i) < 0) {
	    error = 1;
	    break;
	}

	if (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0))
	    _zip_dirent_torrent_normalize(entry->changes);


	de->offset = (zip_uint64_t)ftello(out); /* TODO: check for errors */

	if (new_data) {
	    struct zip_source *zs;

	    zs = NULL;
	    if (!ZIP_ENTRY_DATA_CHANGED(entry)) {
		if ((zs=_zip_source_zip_new(za, za, i, ZIP_FL_UNCHANGED, 0, 0, NULL)) == NULL) {
		    error = 1;
		    break;
		}
	    }

	    /* add_data writes dirent */
	    if (add_data(za, zs ? zs : entry->source, de, out) < 0) {
		error = 1;
		if (zs)
		    zip_source_free(zs);
		break;
	    }
	    if (zs)
		zip_source_free(zs);
	}
	else {
	    zip_uint64_t offset;

	    /* when copying data, all sizes are known -> no data descriptor needed */
	    de->bitflags &= ~ZIP_GPBF_DATA_DESCRIPTOR;
	    if (_zip_dirent_write(de, out, ZIP_FL_LOCAL, &za->error) < 0) {
		error = 1;
		break;
	    }
	    if ((offset=_zip_file_get_offset(za, i, &za->error)) == 0) {
		error = 1;
		break;
	    }
	    if ((fseeko(za->zp, (off_t)offset, SEEK_SET) < 0)) {
		_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
		error = 1;
		break;
	    }
	    if (copy_data(za->zp, de->comp_size, out, &za->error) < 0) {
		error = 1;
		break;
	    }
	}
    }

    if (!error) {
	if (write_cdir(za, filelist, survivors, out) < 0)
	    error = 1;
    }

    free(filelist);

    if (error) {
	fclose(out);
	(void)remove(temp);
	free(temp);
	return -1;
    }

    if (fclose(out) != 0) {
	_zip_error_set(&za->error, ZIP_ER_CLOSE, errno);
	(void)remove(temp);
	free(temp);
	return -1;
    }
    
    if (za->zp) {
	fclose(za->zp);
	za->zp = NULL;
	reopen_on_error = 1;
    }
    if (_zip_rename(temp, za->zn) != 0) {
	_zip_error_set(&za->error, ZIP_ER_RENAME, errno);
	(void)remove(temp);
	free(temp);
	if (reopen_on_error) {
	    /* ignore errors, since we're already in an error case */
	    za->zp = fopen(za->zn, "rb");
	}
	return -1;
    }
#ifndef _WIN32
    mask = umask(0);
    umask(mask);
    chmod(za->zn, 0666&~mask);
#endif

    zip_discard(za);
    free(temp);
    
    return 0;
}



static int
add_data(struct zip *za, struct zip_source *src, struct zip_dirent *de, FILE *ft)
{
    off_t offstart, offdata, offend;
    struct zip_stat st;
    struct zip_source *s2;
    int ret;
    int is_zip64;
    zip_flags_t flags;
    
    if (zip_source_stat(src, &st) < 0) {
	_zip_error_set_from_source(&za->error, src);
	return -1;
    }

    if ((st.valid & ZIP_STAT_COMP_METHOD) == 0) {
	st.valid |= ZIP_STAT_COMP_METHOD;
	st.comp_method = ZIP_CM_STORE;
    }

    if (ZIP_CM_IS_DEFAULT(de->comp_method) && st.comp_method != ZIP_CM_STORE)
	de->comp_method = st.comp_method;
    else if (de->comp_method == ZIP_CM_STORE && (st.valid & ZIP_STAT_SIZE)) {
	st.valid |= ZIP_STAT_COMP_SIZE;
	st.comp_size = st.size;
    }
    else {
	/* we'll recompress */
	st.valid &= ~ZIP_STAT_COMP_SIZE;
    }


    flags = ZIP_EF_LOCAL;

    if ((st.valid & ZIP_STAT_SIZE) == 0)
	flags |= ZIP_FL_FORCE_ZIP64;
    else {
	de->uncomp_size = st.size;
	
	if ((st.valid & ZIP_STAT_COMP_SIZE) == 0) {
	    if (( ((de->comp_method == ZIP_CM_DEFLATE || ZIP_CM_IS_DEFAULT(de->comp_method)) && st.size > MAX_DEFLATE_SIZE_32)
		 || (de->comp_method != ZIP_CM_STORE && de->comp_method != ZIP_CM_DEFLATE && !ZIP_CM_IS_DEFAULT(de->comp_method))))
		flags |= ZIP_FL_FORCE_ZIP64;
	}
	else
	    de->comp_size = st.comp_size;
    }


    offstart = ftello(ft);

    /* as long as we don't support non-seekable output, clear data descriptor bit */
    de->bitflags &= ~ZIP_GPBF_DATA_DESCRIPTOR;
    if ((is_zip64=_zip_dirent_write(de, ft, flags, &za->error)) < 0)
	return -1;


    if (st.comp_method == ZIP_CM_STORE || (ZIP_CM_IS_DEFAULT(de->comp_method) && st.comp_method != de->comp_method)) {
	struct zip_source *s_store, *s_crc;
	zip_compression_implementation comp_impl;
	
	if (st.comp_method != ZIP_CM_STORE) {
	    if ((comp_impl=_zip_get_compression_implementation(st.comp_method)) == NULL) {
		_zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
		return -1;
	    }
	    if ((s_store=comp_impl(za, src, st.comp_method, ZIP_CODEC_DECODE)) == NULL) {
		/* error set by comp_impl */
		return -1;
	    }
	}
	else
	    s_store = src;

	if ((s_crc=zip_source_crc(za, s_store, 0)) == NULL) {
	    if (s_store != src)
		zip_source_pop(s_store);
	    return -1;
	}

	/* TODO: deflate 0-byte files for torrentzip? */
	if (de->comp_method != ZIP_CM_STORE && ((st.valid & ZIP_STAT_SIZE) == 0 || st.size != 0)) {
	    if ((comp_impl=_zip_get_compression_implementation(de->comp_method)) == NULL) {
		_zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
		zip_source_pop(s_crc);
		if (s_store != src)
		    zip_source_pop(s_store);
		return -1;
	    }
	    if ((s2=comp_impl(za, s_crc, de->comp_method, ZIP_CODEC_ENCODE)) == NULL) {
		zip_source_pop(s_crc);
		if (s_store != src)
		    zip_source_pop(s_store);
		return -1;
	    }
	}
	else
	    s2 = s_crc;
    }
    else
	s2 = src;

    offdata = ftello(ft);
	
    ret = copy_source(za, s2, ft);
	
    if (zip_source_stat(s2, &st) < 0)
	ret = -1;
    
    while (s2 != src) {
	if ((s2=zip_source_pop(s2)) == NULL) {
	    /* TODO: set erorr */
	    ret = -1;
	    break;
	}
    }

    if (ret < 0)
	return -1;

    offend = ftello(ft);

    if (fseeko(ft, offstart, SEEK_SET) < 0) {
	_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
	return -1;
    }

    if ((st.valid & (ZIP_STAT_COMP_METHOD|ZIP_STAT_CRC|ZIP_STAT_SIZE)) != (ZIP_STAT_COMP_METHOD|ZIP_STAT_CRC|ZIP_STAT_SIZE)) {
	_zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
	return -1;
    }

    if (st.valid & ZIP_STAT_MTIME)
	de->last_mod = st.mtime;
    else
	time(&de->last_mod);
    de->comp_method = st.comp_method;
    de->crc = st.crc;
    de->uncomp_size = st.size;
    de->comp_size = (zip_uint64_t)(offend - offdata);

    if (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0))
	_zip_dirent_torrent_normalize(de);

    if ((ret=_zip_dirent_write(de, ft, flags, &za->error)) < 0)
	return -1;
 
    if (is_zip64 != ret) {
	/* Zip64 mismatch between preliminary file header written before data and final file header written afterwards */
	_zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
	return -1;
    }

   
    if (fseeko(ft, offend, SEEK_SET) < 0) {
	_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
	return -1;
    }

    return 0;
}



static int
copy_data(FILE *fs, zip_uint64_t len, FILE *ft, struct zip_error *error)
{
    char buf[BUFSIZE];
    size_t n, nn;

    if (len == 0)
	return 0;

    while (len > 0) {
	nn = len > sizeof(buf) ? sizeof(buf) : len > SIZE_MAX ? SIZE_MAX : (size_t)len;
	if ((n=fread(buf, 1, nn, fs)) == 0) {
            if (ferror(fs)) {
                _zip_error_set(error, ZIP_ER_READ, errno);
                return -1;
            }
            else {
                _zip_error_set(error, ZIP_ER_EOF, 0);
                return -1;
            }
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
copy_source(struct zip *za, struct zip_source *src, FILE *ft)
{
    char buf[BUFSIZE];
    zip_int64_t n;
    int ret;

    if (zip_source_open(src) < 0) {
	_zip_error_set_from_source(&za->error, src);
	return -1;
    }

    ret = 0;
    while ((n=zip_source_read(src, buf, sizeof(buf))) > 0) {
	if (fwrite(buf, 1, (size_t)n, ft) != (size_t)n) {
	    _zip_error_set(&za->error, ZIP_ER_WRITE, errno);
	    ret = -1;
	    break;
	}
    }
    
    if (n < 0) {
	if (ret == 0)
	    _zip_error_set_from_source(&za->error, src);
	ret = -1;
    }	

    zip_source_close(src);
    
    return ret;
}



static int
write_cdir(struct zip *za, const struct zip_filelist *filelist, zip_uint64_t survivors, FILE *out)
{
    off_t cd_start, end;
    zip_int64_t size;
    uLong crc;
    char buf[TORRENT_CRC_LEN+1];
    
    cd_start = ftello(out);

    if ((size=_zip_cdir_write(za, filelist, survivors, out)) < 0)
	return -1;
    
    end = ftello(out);

    if (zip_get_archive_flag(za, ZIP_AFL_TORRENT, 0) == 0)
	return 0;


    /* fix up torrentzip comment */

    if (_zip_filerange_crc(out, cd_start, size, &crc, &za->error) < 0)
	return -1;

    snprintf(buf, sizeof(buf), "%08lX", (long)crc);

    if (fseeko(out, end-TORRENT_CRC_LEN, SEEK_SET) < 0) {
	_zip_error_set(&za->error, ZIP_ER_SEEK, errno);
	return -1;
    }

    if (fwrite(buf, TORRENT_CRC_LEN, 1, out) != 1) {
	_zip_error_set(&za->error, ZIP_ER_WRITE, errno);
	return -1;
    }

    return 0;
}



int
_zip_changed(const struct zip *za, zip_uint64_t *survivorsp)
{
    int changed;
    zip_uint64_t i, survivors;

    changed = 0;
    survivors = 0;

    if (za->comment_changed || za->ch_flags != za->flags)
	changed = 1;

    for (i=0; i<za->nentry; i++) {
	if (za->entry[i].deleted || za->entry[i].source || (za->entry[i].changes && za->entry[i].changes->changed != 0))
	    changed = 1;
	if (!za->entry[i].deleted)
	    survivors++;
    }

    if (survivorsp)
	*survivorsp = survivors;

    return changed;
}



static char *
_zip_create_temp_output(struct zip *za, FILE **outp)
{
    char *temp;
    int tfd;
    FILE *tfp;
    
    if (za->tempdir) {
        if ((temp=(char *)malloc(strlen(za->tempdir)+13)) == NULL) {
            _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return NULL;
        }
        sprintf(temp, "%s/.zip.XXXXXX", za->tempdir);
    }
    else {
        if ((temp=(char *)malloc(strlen(za->zn)+8)) == NULL) {
            _zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return NULL;
        }
        sprintf(temp, "%s.XXXXXX", za->zn);
    }

    if ((tfd=mkstemp(temp)) == -1) {
	_zip_error_set(&za->error, ZIP_ER_TMPOPEN, errno);
	free(temp);
	return NULL;
    }
    
    if ((tfp=fdopen(tfd, "r+b")) == NULL) {
	_zip_error_set(&za->error, ZIP_ER_TMPOPEN, errno);
	close(tfd);
	(void)remove(temp);
	free(temp);
	return NULL;
    }

#ifdef _WIN32
    /*
      According to Pierre Joye, Windows in some environments per
      default creates text files, so force binary mode.
    */
    _setmode(_fileno(tfp), _O_BINARY );
#endif

    *outp = tfp;
    return temp;
}



static int
_zip_torrentzip_cmp(const void *a, const void *b)
{
    const char *aname = ((const struct zip_filelist *)a)->name;
    const char *bname = ((const struct zip_filelist *)b)->name;

    if (aname == NULL)
	return (bname != NULL) * -1;
    else if (bname == NULL)
	return 1;

    return strcasecmp(aname, bname);
}
