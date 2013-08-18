/*
  zip_open.c -- open zip archive by name
  Copyright (C) 1999-2011 Dieter Baron and Thomas Klausner

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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"

static void set_error(int *, struct zip_error *, int);
static struct zip *_zip_allocate_new(const char *, int *);
static int _zip_checkcons(FILE *, struct zip_cdir *, struct zip_error *);
static void _zip_check_torrentzip(struct zip *);
static struct zip_cdir *_zip_find_central_dir(FILE *, int, int *, off_t);
static int _zip_file_exists(const char *, int, int *);
static int _zip_headercomp(struct zip_dirent *, int,
			   struct zip_dirent *, int);
static unsigned char *_zip_memmem(const unsigned char *, int,
				  const unsigned char *, int);
static struct zip_cdir *_zip_readcdir(FILE *, off_t, unsigned char *, unsigned char *,
				 int, int, struct zip_error *);



ZIP_EXTERN(struct zip *)
zip_open(const char *fn, int flags, int *zep)
{
    FILE *fp;

    if (flags & ZIP_OVERWRITE) {
	return _zip_allocate_new(fn, zep);
    }

    switch (_zip_file_exists(fn, flags, zep)) {
    case -1:
			if (!(flags & ZIP_OVERWRITE)) {
				return NULL;
			}
    case 0:
	return _zip_allocate_new(fn, zep);
    default:
	break;
    }

    if ((fp=fopen(fn, "rb")) == NULL) {
	set_error(zep, NULL, ZIP_ER_OPEN);
	return NULL;
    }

    return _zip_open(fn, fp, flags, 0, zep);
}



struct zip *
_zip_open(const char *fn, FILE *fp, int flags, int aflags, int *zep)
{
    struct zip *za;
    struct zip_cdir *cdir;
    int i;
    off_t len;

    if (fseeko(fp, 0, SEEK_END) < 0) {
	*zep = ZIP_ER_SEEK;
	return NULL;
    }
    len = ftello(fp);

    /* treat empty files as empty archives */
    if (len == 0) {
	if ((za=_zip_allocate_new(fn, zep)) == NULL)
	    fclose(fp);
	else
	    za->zp = fp;
	return za;
    }

    cdir = _zip_find_central_dir(fp, flags, zep, len);
    if (cdir == NULL) {
	fclose(fp);
	return NULL;
    }

    if ((za=_zip_allocate_new(fn, zep)) == NULL) {
	_zip_cdir_free(cdir);
	fclose(fp);
	return NULL;
    }

    za->cdir = cdir;
    za->zp = fp;

    if ((za->entry=(struct zip_entry *)malloc(sizeof(*(za->entry))
					      * cdir->nentry)) == NULL) {
	set_error(zep, NULL, ZIP_ER_MEMORY);
	_zip_free(za);
	return NULL;
    }
    for (i=0; i<cdir->nentry; i++)
	_zip_entry_new(za);

    _zip_check_torrentzip(za);
    za->ch_flags = za->flags;

    return za;
}



static void
set_error(int *zep, struct zip_error *err, int ze)
{
    int se;

    if (err) {
	_zip_error_get(err, &ze, &se);
	if (zip_error_get_sys_type(ze) == ZIP_ET_SYS)
	    errno = se;
    }

    if (zep)
	*zep = ze;
}



/* _zip_readcdir:
   tries to find a valid end-of-central-directory at the beginning of
   buf, and then the corresponding central directory entries.
   Returns a struct zip_cdir which contains the central directory 
   entries, or NULL if unsuccessful. */

static struct zip_cdir *
_zip_readcdir(FILE *fp, off_t buf_offset, unsigned char *buf, unsigned char *eocd, int buflen,
	      int flags, struct zip_error *error)
{
    struct zip_cdir *cd;
    unsigned char *cdp, **bufp;
    int i, comlen, nentry;
    zip_uint32_t left;

    comlen = buf + buflen - eocd - EOCDLEN;
    if (comlen < 0) {
	/* not enough bytes left for comment */
	_zip_error_set(error, ZIP_ER_NOZIP, 0);
	return NULL;
    }

    /* check for end-of-central-dir magic */
    if (memcmp(eocd, EOCD_MAGIC, 4) != 0) {
	_zip_error_set(error, ZIP_ER_NOZIP, 0);
	return NULL;
    }

    if (memcmp(eocd+4, "\0\0\0\0", 4) != 0) {
	_zip_error_set(error, ZIP_ER_MULTIDISK, 0);
	return NULL;
    }

    cdp = eocd + 8;
    /* number of cdir-entries on this disk */
    i = _zip_read2(&cdp);
    /* number of cdir-entries */
    nentry = _zip_read2(&cdp);

    if ((cd=_zip_cdir_new(nentry, error)) == NULL)
	return NULL;

    cd->size = _zip_read4(&cdp);
    cd->offset = _zip_read4(&cdp);
    cd->comment = NULL;
    cd->comment_len = _zip_read2(&cdp);

    if (((zip_uint64_t)cd->offset)+cd->size > buf_offset + (eocd-buf)) {
	/* cdir spans past EOCD record */
	_zip_error_set(error, ZIP_ER_INCONS, 0);
	cd->nentry = 0;
	_zip_cdir_free(cd);
	return NULL;
    }

    if ((comlen < cd->comment_len) || (cd->nentry != i)) {
	_zip_error_set(error, ZIP_ER_NOZIP, 0);
	cd->nentry = 0;
	_zip_cdir_free(cd);
	return NULL;
    }
    if ((flags & ZIP_CHECKCONS) && comlen != cd->comment_len) {
	_zip_error_set(error, ZIP_ER_INCONS, 0);
	cd->nentry = 0;
	_zip_cdir_free(cd);
	return NULL;
    }

    if (cd->comment_len) {
	if ((cd->comment=(char *)_zip_memdup(eocd+EOCDLEN,
					     cd->comment_len, error))
	    == NULL) {
	    cd->nentry = 0;
	    _zip_cdir_free(cd);
	    return NULL;
	}
    }

    if (cd->offset >= buf_offset) {
	/* if buffer already read in, use it */
	cdp = buf + (cd->offset - buf_offset);
	bufp = &cdp;
    }
    else {
	/* go to start of cdir and read it entry by entry */
	bufp = NULL;
	clearerr(fp);
	fseeko(fp, cd->offset, SEEK_SET);
	/* possible consistency check: cd->offset =
	   len-(cd->size+cd->comment_len+EOCDLEN) ? */
	if (ferror(fp) || ((unsigned long)ftello(fp) != cd->offset)) {
	    /* seek error or offset of cdir wrong */
	    if (ferror(fp))
		_zip_error_set(error, ZIP_ER_SEEK, errno);
	    else
		_zip_error_set(error, ZIP_ER_NOZIP, 0);
	    cd->nentry = 0;
	    _zip_cdir_free(cd);
	    return NULL;
	}
    }

    left = cd->size;
    i=0;
    while (i<cd->nentry && left > 0) {
	if ((_zip_dirent_read(cd->entry+i, fp, bufp, &left, 0, error)) < 0) {
	    cd->nentry = i;
	    _zip_cdir_free(cd);
	    return NULL;
	}
	i++;
	
	if (i == cd->nentry && left > 0) {
	    /* Infozip extension for more than 64k entries:
	       nentries wraps around, size indicates correct EOCD */
	    if (_zip_cdir_grow(cd, cd->nentry+ZIP_UINT16_MAX, error) < 0) {
		cd->nentry = i;
		_zip_cdir_free(cd);
		return NULL;
	    }
	}
    }

    cd->nentry = i;
    
    return cd;
}



/* _zip_checkcons:
   Checks the consistency of the central directory by comparing central
   directory entries with local headers and checking for plausible
   file and header offsets. Returns -1 if not plausible, else the
   difference between the lowest and the highest fileposition reached */

static int
_zip_checkcons(FILE *fp, struct zip_cdir *cd, struct zip_error *error)
{
    int i;
    unsigned int min, max, j;
    struct zip_dirent temp;

    if (cd->nentry) {
	max = cd->entry[0].offset;
	min = cd->entry[0].offset;
    }
    else
	min = max = 0;

    for (i=0; i<cd->nentry; i++) {
	if (cd->entry[i].offset < min)
	    min = cd->entry[i].offset;
	if (min > cd->offset) {
	    _zip_error_set(error, ZIP_ER_NOZIP, 0);
	    return -1;
	}
	
	j = cd->entry[i].offset + cd->entry[i].comp_size
	    + cd->entry[i].filename_len + LENTRYSIZE;
	if (j > max)
	    max = j;
	if (max > cd->offset) {
	    _zip_error_set(error, ZIP_ER_NOZIP, 0);
	    return -1;
	}
	
	if (fseeko(fp, cd->entry[i].offset, SEEK_SET) != 0) {
	    _zip_error_set(error, ZIP_ER_SEEK, 0);
	    return -1;
	}
	
	if (_zip_dirent_read(&temp, fp, NULL, NULL, 1, error) == -1)
	    return -1;
	
	if (_zip_headercomp(cd->entry+i, 0, &temp, 1) != 0) {
	    _zip_error_set(error, ZIP_ER_INCONS, 0);
	    _zip_dirent_finalize(&temp);
	    return -1;
	}
	_zip_dirent_finalize(&temp);
    }

    return max - min;
}



/* _zip_check_torrentzip:
   check whether ZA has a valid TORRENTZIP comment, i.e. is torrentzipped */

static void
_zip_check_torrentzip(struct zip *za)
{
    uLong crc_got, crc_should;
    char buf[8+1];
    char *end;

    if (za->zp == NULL || za->cdir == NULL)
	return;

    if (za->cdir->comment_len != TORRENT_SIG_LEN+8
	|| strncmp(za->cdir->comment, TORRENT_SIG, TORRENT_SIG_LEN) != 0)
	return;

    memcpy(buf, za->cdir->comment+TORRENT_SIG_LEN, 8);
    buf[8] = '\0';
    errno = 0;
    crc_should = strtoul(buf, &end, 16);
    if ((crc_should == UINT_MAX && errno != 0) || (end && *end))
	return;

    if (_zip_filerange_crc(za->zp, za->cdir->offset, za->cdir->size,
			   &crc_got, NULL) < 0)
	return;

    if (crc_got == crc_should)
	za->flags |= ZIP_AFL_TORRENT;
}




/* _zip_headercomp:
   compares two headers h1 and h2; if they are local headers, set
   local1p or local2p respectively to 1, else 0. Return 0 if they
   are identical, -1 if not. */

static int
_zip_headercomp(struct zip_dirent *h1, int local1p, struct zip_dirent *h2,
	   int local2p)
{
    if ((h1->version_needed != h2->version_needed)
#if 0
	/* some zip-files have different values in local
	   and global headers for the bitflags */
	|| (h1->bitflags != h2->bitflags)
#endif
	|| (h1->comp_method != h2->comp_method)
	|| (h1->last_mod != h2->last_mod)
	|| (h1->filename_len != h2->filename_len)
	|| !h1->filename || !h2->filename
	|| strcmp(h1->filename, h2->filename))
	return -1;

    /* check that CRC and sizes are zero if data descriptor is used */
    if ((h1->bitflags & ZIP_GPBF_DATA_DESCRIPTOR) && local1p
	&& (h1->crc != 0
	    || h1->comp_size != 0
	    || h1->uncomp_size != 0))
	return -1;
    if ((h2->bitflags & ZIP_GPBF_DATA_DESCRIPTOR) && local2p
	&& (h2->crc != 0
	    || h2->comp_size != 0
	    || h2->uncomp_size != 0))
	return -1;
    
    /* check that CRC and sizes are equal if no data descriptor is used */
    if (((h1->bitflags & ZIP_GPBF_DATA_DESCRIPTOR) == 0 || local1p == 0)
	&& ((h2->bitflags & ZIP_GPBF_DATA_DESCRIPTOR) == 0 || local2p == 0)) {
	if ((h1->crc != h2->crc)
	    || (h1->comp_size != h2->comp_size)
	    || (h1->uncomp_size != h2->uncomp_size))
	    return -1;
    }
    
    if ((local1p == local2p)
	&& ((h1->extrafield_len != h2->extrafield_len)
	    || (h1->extrafield_len && h2->extrafield
		&& memcmp(h1->extrafield, h2->extrafield,
			  h1->extrafield_len))))
	return -1;

    /* if either is local, nothing more to check */
    if (local1p || local2p)
	return 0;

    if ((h1->version_madeby != h2->version_madeby)
	|| (h1->disk_number != h2->disk_number)
	|| (h1->int_attrib != h2->int_attrib)
	|| (h1->ext_attrib != h2->ext_attrib)
	|| (h1->offset != h2->offset)
	|| (h1->comment_len != h2->comment_len)
	|| (h1->comment_len && h2->comment
	    && memcmp(h1->comment, h2->comment, h1->comment_len)))
	return -1;

    return 0;
}



static struct zip *
_zip_allocate_new(const char *fn, int *zep)
{
    struct zip *za;
    struct zip_error error;

    if ((za=_zip_new(&error)) == NULL) {
	set_error(zep, &error, 0);
	return NULL;
    }

    if (fn == NULL)
	za->zn = NULL;
    else {
	za->zn = strdup(fn);
	if (!za->zn) {
	    _zip_free(za);
	    set_error(zep, NULL, ZIP_ER_MEMORY);
	    return NULL;
	}
    }
    return za;
}



static int
_zip_file_exists(const char *fn, int flags, int *zep)
{
    struct stat st;

    if (fn == NULL) {
	set_error(zep, NULL, ZIP_ER_INVAL);
	return -1;
    }
    
    if (stat(fn, &st) != 0) {
	if (flags & ZIP_CREATE || flags & ZIP_OVERWRITE)
	    return 0;
	else {
	    set_error(zep, NULL, ZIP_ER_OPEN);
	    return -1;
	}
    }
    else if ((flags & ZIP_EXCL)) {
	set_error(zep, NULL, ZIP_ER_EXISTS);
	return -1;
    }
    /* ZIP_CREATE gets ignored if file exists and not ZIP_EXCL,
       just like open() */

    return 1;
}



static struct zip_cdir *
_zip_find_central_dir(FILE *fp, int flags, int *zep, off_t len)
{
    struct zip_cdir *cdir, *cdirnew;
    unsigned char *buf, *match;
    off_t buf_offset;
    int a, best, buflen, i;
    struct zip_error zerr;

    i = fseeko(fp, -(len < CDBUFSIZE ? len : CDBUFSIZE), SEEK_END);
    if (i == -1 && errno != EFBIG) {
	/* seek before start of file on my machine */
	set_error(zep, NULL, ZIP_ER_SEEK);
	return NULL;
    }
    buf_offset = ftello(fp);
    
    /* 64k is too much for stack */
    if ((buf=(unsigned char *)malloc(CDBUFSIZE)) == NULL) {
	set_error(zep, NULL, ZIP_ER_MEMORY);
	return NULL;
    }

    clearerr(fp);
    buflen = fread(buf, 1, CDBUFSIZE, fp);

    if (ferror(fp)) {
	set_error(zep, NULL, ZIP_ER_READ);
	free(buf);
	return NULL;
    }
    
    best = -1;
    cdir = NULL;
    match = buf;
    _zip_error_set(&zerr, ZIP_ER_NOZIP, 0);

    while ((match=_zip_memmem(match, buflen-(match-buf)-18,
			      (const unsigned char *)EOCD_MAGIC, 4))!=NULL) {
	/* found match -- check, if good */
	/* to avoid finding the same match all over again */
	match++;
	if ((cdirnew=_zip_readcdir(fp, buf_offset, buf, match-1, buflen, flags,
				   &zerr)) == NULL)
	    continue;

	if (cdir) {
	    if (best <= 0)
		best = _zip_checkcons(fp, cdir, &zerr);
	    a = _zip_checkcons(fp, cdirnew, &zerr);
	    if (best < a) {
		_zip_cdir_free(cdir);
		cdir = cdirnew;
		best = a;
	    }
	    else
		_zip_cdir_free(cdirnew);
	}
	else {
	    cdir = cdirnew;
	    if (flags & ZIP_CHECKCONS)
		best = _zip_checkcons(fp, cdir, &zerr);
	    else
		best = 0;
	}
	cdirnew = NULL;
    }

    free(buf);
    
    if (best < 0) {
	set_error(zep, &zerr, 0);
	_zip_cdir_free(cdir);
	return NULL;
    }

    return cdir;
}



static unsigned char *
_zip_memmem(const unsigned char *big, int biglen, const unsigned char *little, 
       int littlelen)
{
    const unsigned char *p;
    
    if ((biglen < littlelen) || (littlelen == 0))
	return NULL;
    p = big-1;
    while ((p=(const unsigned char *)
	        memchr(p+1, little[0], (size_t)(big-(p+1)+biglen-littlelen+1)))
	   != NULL) {
	if (memcmp(p+1, little+1, littlelen-1)==0)
	    return (unsigned char *)p;
    }

    return NULL;
}
