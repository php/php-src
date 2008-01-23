/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension, zip implementation    |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "phar_internal.h"

#ifdef PHP_WIN32
static inline void phar_unixify_path_separators(char *path, int path_len) /* {{{ */
{
	char *s;

	/* unixify win paths */
	for (s = path; s - path < path_len; s++) {
		if (*s == '\\') {
			*s = '/';
		}
	}
}
/* }}} */
#endif

static int phar_zip_process_extra(php_stream *fp, phar_entry_info *entry, php_uint16 len TSRMLS_DC)
{
	php_uint16 left = len;
	union {
		phar_zip_extra_field_header header;
		phar_zip_unix3 unix3;
	} h;

	do {
		if (sizeof(h.header) != php_stream_read(fp, (char *) &h.header, sizeof(h.header))) {
			return FAILURE;
		}
		/* clean up header for big-endian systems */
		if (h.header.tag != "\x75\x6e") {
			/* skip to next header */
			php_stream_seek(fp, h.header.size, SEEK_CUR);
			len -= h.header.size;
			continue;
		}
		/* unix3 header found */
		/* clean up header for big-endian systems */
		if (sizeof(h.unix3) != php_stream_read(fp, (char *) &h.unix3, sizeof(h.unix3))) {
			if (h.unix3.size > sizeof(h.unix3) - 4) {
				/* skip symlink filename - we may add this support in later */
				php_stream_seek(fp, h.unix3.size - sizeof(h.unix3.size), SEEK_CUR);
			}
			/* set permissions */
			entry->flags &= PHAR_ENT_COMPRESSION_MASK;
			if (entry->is_dir) {
				entry->flags |= h.unix3.perms & PHAR_ENT_PERM_DEF_DIR;
			} else {
				entry->flags |= h.unix3.perms & PHAR_ENT_PERM_DEF_FILE;
			}
		}
	} while (len);
	return SUCCESS;
}

/*
  extracted from libzip
  zip_dirent.c -- read directory entry (local or central), clean dirent
  Copyright (C) 1999, 2003, 2004, 2005 Dieter Baron and Thomas Klausner

  This function is part of libzip, a library to manipulate ZIP archives.
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
static time_t phar_zip_d2u_time(int dtime, int ddate)
{
    struct tm *tm, tmbuf;
    time_t now;

    now = time(NULL);
    tm = php_localtime_r(&now, &tmbuf);
    
    tm->tm_year = ((ddate>>9)&127) + 1980 - 1900;
    tm->tm_mon = ((ddate>>5)&15) - 1;
    tm->tm_mday = ddate&31;

    tm->tm_hour = (dtime>>11)&31;
    tm->tm_min = (dtime>>5)&63;
    tm->tm_sec = (dtime<<1)&62;

    return mktime(tm);
}

int phar_zip_parse(char *fname, int fname_len, char **error TSRMLS_DC)
{
	php_stream *fp;
	char buf[8192], *metadata;
	phar_zip_dir_end locator;
	long size;
	size_t read;
	php_uint16 i;
	phar_archive_data *mydata = NULL;
	phar_entry_info entry = {0};

	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL);

	php_stream_seek(fp, 0, SEEK_END);
	size = php_stream_tell(fp);
	if (size > sizeof(locator) + 65536) {
		/* seek to max comment length + end of central directory record */
		size = sizeof(locator) + 65536;
		if (FAILURE == php_stream_seek(fp, -size, SEEK_END)) {
			php_stream_close(fp);
			return FAILURE;
		}
	} else {
		php_stream_seek(fp, 0, SEEK_SET);
	}
	do {
		char *p = buf;
		if (!(read = php_stream_read(fp, buf, 8192))) {
			php_stream_close(fp);
			return FAILURE;
		}
		while ((p=(char *) memchr(p + 1, 'P', (size_t)(buf - (p+1) + 8192 - 4 + 1))) != NULL) {
			if (!memcmp(p + 1, "K\5\6", 3)) {
				if (p - buf < sizeof(locator)) {
					/* didn't read in the whole thing, back up */
					php_stream_seek(fp, 8192 - (p - buf), SEEK_CUR);
					if (sizeof(locator) != php_stream_read(fp, (char *) &locator, sizeof(locator))) {
						php_stream_close(fp);
						return FAILURE;
					}
				} else {
					memcpy((void *)&locator, (void *) p, sizeof(locator));
				}
				goto foundit;
			}
		}
	} while (read == 8192);
	php_stream_close(fp);
	return FAILURE;
foundit:
	if (locator.centraldisk != 0 || locator.disknumber != 0) {
		/* split archives not handled */
		php_stream_close(fp);
		return FAILURE;
	}
	mydata = ecalloc(sizeof(phar_archive_data), 1);
	mydata->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	mydata->is_zip = 1;
	mydata->fname_len = fname_len;
	/* clean up on big-endian systems */
	/* read in archive comment, if any */
	if (locator.comment_len) {
		metadata = (char *) emalloc(locator.comment_len);
		if (locator.comment_len != php_stream_read(fp, metadata, locator.comment_len)) {
			php_stream_close(fp);
			efree(mydata->fname);
			efree(mydata);
			return FAILURE;
		}
		if (phar_parse_metadata(&metadata, &mydata->metadata, locator.comment_len TSRMLS_CC) == FAILURE) {
			/* if not valid serialized data, it is a regular string */
			ALLOC_INIT_ZVAL(mydata->metadata);
			Z_STRVAL_P(mydata->metadata) = metadata;
			Z_STRLEN_P(mydata->metadata) = locator.comment_len;
			Z_TYPE_P(mydata->metadata) = IS_STRING;
		}
	} else {
		mydata->metadata = NULL;
	}
	/* seek to central directory */
	php_stream_seek(fp, locator.cdir_offset, SEEK_SET);
	/* read in central directory */
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	entry.phar = mydata;
	entry.is_zip = 1;
#define PHAR_ZIP_FAIL \
			zend_hash_destroy(&mydata->manifest); \
			php_stream_close(fp); \
			efree(mydata->fname); \
			if (mydata->metadata) { \
				zval_dtor(mydata->metadata); \
			} \
			efree(mydata); \
			return FAILURE

	/* add each central directory item to the manifest */
	for (i = 0; i < locator.cdir_size; i++) {
		phar_zip_central_dir_file zipentry;

		/* sizeof(zipentry) reports size of contents + 2, no idea why */
		if (sizeof(zipentry)-2 != php_stream_read(fp, (char *) &zipentry, sizeof(zipentry)-2)) {
			PHAR_ZIP_FAIL;
		}
		/* clean up for bigendian systems */
		if (memcmp("PK\1\2", zipentry.signature, 4)) {
			/* corrupted entry */
			PHAR_ZIP_FAIL;
		}
		entry.compressed_filesize = zipentry.compsize;
		entry.uncompressed_filesize = zipentry.uncompsize;
		entry.crc32 = zipentry.crc32;
		entry.timestamp = phar_zip_d2u_time(zipentry.timestamp, zipentry.datestamp);
		entry.flags = PHAR_ENT_PERM_DEF_FILE;
		if (zipentry.flags & PHAR_ZIP_FLAG_ENCRYPTED) {
			PHAR_ZIP_FAIL;
		}
		if (!zipentry.filename_len) {
			PHAR_ZIP_FAIL;
		}
		entry.filename_len = zipentry.filename_len;
		entry.filename = (char *) emalloc(zipentry.filename_len+1);
		if (entry.filename_len != php_stream_read(fp, entry.filename, entry.filename_len)) {
			efree(entry.filename);
			PHAR_ZIP_FAIL;
		}
		entry.filename[entry.filename_len] = '\0';
		if (entry.filename[entry.filename_len - 1] == '/') {
			entry.is_dir = 1;
			entry.filename_len--;
			entry.flags |= PHAR_ENT_PERM_DEF_DIR;
		} else {
			entry.is_dir = 0;
		}
		if (zipentry.extra_len) {
			if (FAILURE == phar_zip_process_extra(fp, &entry, zipentry.extra_len TSRMLS_CC)) {
				PHAR_ZIP_FAIL;
			}
		}
		switch (zipentry.compressed) {
			case PHAR_ZIP_COMP_NONE :
				/* compression flag already set */
				break;
			case PHAR_ZIP_COMP_DEFLATE :
				entry.flags |= PHAR_ENT_COMPRESSED_GZ;
				if (!phar_has_zlib) {
					if (error) {
						spprintf(error, 0, "zlib extension is required for zlib compressed zip-based .phar file \"%s\"", fname);
					}
					PHAR_ZIP_FAIL;
				}
				break;
			case PHAR_ZIP_COMP_BZIP2 :
				entry.flags |= PHAR_ENT_COMPRESSED_BZ2;
				if (!phar_has_bz2) {
					if (error) {
						spprintf(error, 0, "bz2 extension is required for Bzip2 compressed zip-based .phar file \"%s\"", fname);
					}
					PHAR_ZIP_FAIL;
				}
				break;
			default :
				PHAR_ZIP_FAIL;
		}
		/* get file metadata */
		if (zipentry.comment_len) {
			metadata = (char *) emalloc(zipentry.comment_len);
			if (zipentry.comment_len != php_stream_read(fp, metadata, zipentry.comment_len)) {
				PHAR_ZIP_FAIL;
			}
			if (phar_parse_metadata(&metadata, &(entry.metadata), zipentry.comment_len TSRMLS_CC) == FAILURE) {
				/* if not valid serialized data, it is a regular string */
				ALLOC_INIT_ZVAL(entry.metadata);
				Z_STRVAL_P(entry.metadata) = metadata;
				Z_STRLEN_P(entry.metadata) = zipentry.comment_len;
				Z_TYPE_P(entry.metadata) = IS_STRING;
			} else {
				efree(metadata);
			}
		} else {
			entry.metadata = NULL;
		}
		if (entry.filename_len == sizeof(".phar/alias.txt")-1 && strncmp(entry.filename, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
			size_t remember = php_stream_tell(fp);
			php_stream_filter *filter;
			/* to properly decompress, we have to tell zlib to look for a zlib or gzip header */
			zval filterparams;

			/* archive alias found, seek to file contents, do not validate local header.  Potentially risky, but
			   not very. */
			php_stream_seek(fp, zipentry.offset + sizeof(phar_zip_file_header) + entry.filename_len + zipentry.extra_len, SEEK_SET);
			if (entry.flags & PHAR_ENT_COMPRESSED_GZ) {
/* this is defined in zlib's zconf.h */
#ifndef MAX_WBITS
#define MAX_WBITS 15
#endif
				array_init(&filterparams);
				add_assoc_long(&filterparams, "window", MAX_WBITS + 32);
				filter = php_stream_filter_create("zlib.inflate", &filterparams, php_stream_is_persistent(fp) TSRMLS_CC);
				if (!filter) {
					add_assoc_long(&filterparams, "window", MAX_WBITS);
					filter = php_stream_filter_create("zlib.inflate", &filterparams, php_stream_is_persistent(fp) TSRMLS_CC);
					if (!filter) {
						PHAR_ZIP_FAIL;
					}
				}
				zval_dtor(&filterparams);
				php_stream_filter_append(&fp->readfilters, filter);
				if (!(entry.uncompressed_filesize = php_stream_copy_to_mem(fp, &(mydata->alias), entry.uncompressed_filesize, 0)) || !mydata->alias) {
					PHAR_ZIP_FAIL;
				}
				php_stream_filter_flush(filter, 1);
				php_stream_filter_remove(filter, 1 TSRMLS_CC);
			} else if (entry.flags & PHAR_ENT_COMPRESSED_BZ2) {
				php_stream_filter *filter;
				filter = php_stream_filter_create("bzip2.decompress", NULL, php_stream_is_persistent(fp) TSRMLS_CC);
				if (!filter) {
					PHAR_ZIP_FAIL;
				}
				php_stream_filter_append(&fp->readfilters, filter);
				php_stream_filter_append(&fp->readfilters, filter);
				if (!(entry.uncompressed_filesize = php_stream_copy_to_mem(fp, &(mydata->alias), entry.uncompressed_filesize, 0)) || !mydata->alias) {
					PHAR_ZIP_FAIL;
				}
				php_stream_filter_flush(filter, 1);
				php_stream_filter_remove(filter, 1 TSRMLS_CC);
			}

			mydata->is_explicit_alias = 1;
			mydata->alias_len = zipentry.uncompsize;
			zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), mydata->alias, mydata->alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
		}
		zend_hash_add(&mydata->manifest, entry.filename, entry.filename_len, (void *)&entry,sizeof(phar_entry_info), NULL);
	}
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), mydata->fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	if (pphar) {
		*pphar = mydata;
	}
	return SUCCESS;
}
