/*
  +----------------------------------------------------------------------+
  | TAR archive support for Phar                                         |
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
  | Authors: Dmitry Stogov <dmitry@zend.com>                             |
  |          Gregory Beaver <cellog@php.net>                             |
  +----------------------------------------------------------------------+
*/

#include "phar_internal.h"

static php_uint32 phar_tar_number(char *buf, int len) /* {{{ */
{
	php_uint32 num = 0;
	int i = 0;

	while (i < len && buf[i] == ' ') {
		i++;
	}
	while (i < len &&
	       buf[i] >= '0' &&
	       buf[i] <= '7') {
		num = num * 8 + (buf[i] - '0');
		i++;
	}
	return num;
}
/* }}} */

/* adapted from format_octal() in libarchive
 * 
 * Copyright (c) 2003-2007 Tim Kientzle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
static int phar_tar_octal(char *buf, php_uint32 val, int len) /* {{{ */
{
	char *p = buf;
	int s = len;

	p += len;		/* Start at the end and work backwards. */
	while (s-- > 0) {
		*--p = (char)('0' + (val & 7));
		val >>= 3;
	}

	if (val == 0)
		return SUCCESS;

	/* If it overflowed, fill field with max value. */
	while (len-- > 0)
		*p++ = '7';

	return FAILURE;
}
/* }}} */

static php_uint32 phar_tar_checksum(char *buf, int len) /* {{{ */
{
	php_uint32 sum = 0;
	char *end = buf + len;

	while (buf != end) {
		sum += (unsigned char)*buf;
		buf++;
	}
	return sum;
}
/* }}} */

int phar_is_tar(char *buf)
{
	tar_header *header = (tar_header *) buf;
	php_uint32 checksum = phar_tar_number(header->checksum, sizeof(header->checksum));
	php_uint32 ret;
	char save[sizeof(header->checksum)];

	/* assume that the first filename in a tar won't begin with <?php */
	if (!strncmp(buf, "<?php", sizeof("<?php")-1)) {
		return 0;
	}

	memcpy(save, header->checksum, sizeof(header->checksum));
	memset(header->checksum, ' ', sizeof(header->checksum));
	ret = (checksum == phar_tar_checksum(buf, 512));
	memcpy(header->checksum, save, sizeof(header->checksum));
	return ret;
}

int phar_open_or_create_tar(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	int ret = phar_create_or_parse_filename(fname, fname_len, alias, alias_len, options, &phar, error TSRMLS_CC);

	if (pphar) {
		*pphar = phar;
	}
	if (FAILURE == ret) {
		return FAILURE;
	}
	if (phar->is_tar) {
		return ret;
	}
	if (phar->is_brandnew) {
		phar->is_tar = 1;
		phar->internal_file_start = 0;
		return SUCCESS;
	}

	/* we've reached here - the phar exists and is a regular phar */
	if (error) {
		spprintf(error, 4096, "phar zip error: phar \"%s\" already exists as a regular phar and must be deleted from disk prior to creating as a zip-based phar", fname);
	}
	return FAILURE;
}

int phar_open_tarfile(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, php_uint32 compression, char **error TSRMLS_DC) /* {{{ */
{
	char buf[512], *actual_alias = NULL;
	phar_entry_info entry = {0};
	size_t pos = 0, read;
	tar_header *hdr;
	php_uint32 sum1, sum2, size, old;
	phar_archive_data *myphar, **actual;

	if (error) {
		*error = NULL;
	}

	read = php_stream_read(fp, buf, sizeof(buf));
	if (read != sizeof(buf)) {
		if (error) {
			spprintf(error, 4096, "phar error: \"%s\" is not a tar file", fname);
		}
		php_stream_close(fp);
		return FAILURE;
	}
	hdr = (tar_header*)buf;
	old = (memcmp(hdr->magic, "ustar", sizeof("ustar")-1) != 0);

	myphar = (phar_archive_data *) ecalloc(1, sizeof(phar_archive_data));
	zend_hash_init(&myphar->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	myphar->is_tar = 1;
	/* remember whether this entire phar was compressed with gz/bzip2 */
	myphar->flags = compression;

	entry.is_tar = 1;
	entry.is_crc_checked = 1;
	entry.phar = myphar;
	do {
		if (read != sizeof(buf)) {
			if (error) {
				spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file", fname);
			}
			php_stream_close(fp);
			zend_hash_destroy(&myphar->manifest);
			myphar->manifest.arBuckets = 0;
			efree(myphar);
			return FAILURE;
		}
		pos += sizeof(buf);
		hdr = (tar_header*) buf;
		sum1 = phar_tar_number(hdr->checksum, sizeof(hdr->checksum));
		if (sum1 == 0 && phar_tar_checksum(buf, sizeof(buf)) == 0) {
			break;
		}
		memset(hdr->checksum, ' ', sizeof(hdr->checksum));
		sum2 = phar_tar_checksum(buf, old?sizeof(old_tar_header):sizeof(tar_header));
		if (sum1 != sum2) {
			if (error) {
				spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file", fname);
			}
			php_stream_close(fp);
			zend_hash_destroy(&myphar->manifest);
			myphar->manifest.arBuckets = 0;
			efree(myphar);
			return FAILURE;
		}

		size = entry.uncompressed_filesize = entry.compressed_filesize =
			phar_tar_number(hdr->size, sizeof(hdr->size));

		if (!old && hdr->prefix[0] != 0) {
			char name[256];

			strcpy(name, hdr->prefix);
			strcat(name, hdr->name);
			entry.filename_len = strlen(name);
			if (name[entry.filename_len - 1] == '/') {
				/* some tar programs store directories with trailing slash */
				entry.filename_len--;
			}
			entry.filename = estrndup(name, entry.filename_len);
		} else {
			entry.filename = estrdup(hdr->name);
			entry.filename_len = strlen(entry.filename);
			if (entry.filename[entry.filename_len - 1] == '/') {
				/* some tar programs store directories with trailing slash */
				entry.filename[entry.filename_len - 1] = '\0';
				entry.filename_len--;
			}
		}

		entry.tar_type = ((old & (hdr->typeflag == 0))?'0':hdr->typeflag);
		entry.offset = entry.offset_abs = pos; /* header_offset unused in tar */
		entry.fp_type = PHAR_FP;
		entry.flags = phar_tar_number(hdr->mode, sizeof(hdr->mode)) & PHAR_ENT_PERM_MASK;
		entry.timestamp = phar_tar_number(hdr->mtime, sizeof(hdr->mtime));

#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif
		if (old && entry.tar_type == TAR_FILE && S_ISDIR(entry.flags)) {
			entry.tar_type = TAR_DIR;
		}
		if (entry.tar_type == TAR_DIR) {
			entry.is_dir = 1;
		} else {
			entry.is_dir = 0;
		}

		entry.link = NULL;
		if (entry.tar_type == TAR_LINK) {
			if (!zend_hash_exists(&myphar->manifest, hdr->linkname, strlen(hdr->linkname))) {
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file - symbolic link to non-existent file", fname);
				}
				php_stream_close(fp);
				zend_hash_destroy(&myphar->manifest);
				myphar->manifest.arBuckets = 0;
				efree(myphar);
				return FAILURE;
			}
			entry.link = estrdup(hdr->linkname);
		} else if (entry.tar_type == TAR_SYMLINK) {
			entry.link = estrdup(hdr->linkname);
		}
		zend_hash_add(&myphar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL);
		if (!actual_alias && entry.filename_len == sizeof(".phar/alias.txt")-1 && !strncmp(entry.filename, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
			size_t read;
			/* found explicit alias */
			read = php_stream_read(fp, buf, size);
			if (read == size) {
				actual_alias = estrndup(buf, size);
				myphar->alias = actual_alias;
				myphar->alias_len = size;
				php_stream_seek(fp, pos, SEEK_SET);
			} else {
				if (error) {
					spprintf(error, 4096, "phar error: Unable to read alias from tar-based phar \"%s\"", fname);
				}
				php_stream_close(fp);
				zend_hash_destroy(&myphar->manifest);
				myphar->manifest.arBuckets = 0;
				efree(myphar);
				return FAILURE;
			}
		}
		size = (size+511)&~511;
		pos += size;
		if (((hdr->typeflag == 0) || (hdr->typeflag == TAR_FILE)) && size > 0) {
			php_stream_seek(fp, size, SEEK_CUR);
		}
		read = php_stream_read(fp, buf, sizeof(buf));
	} while (read != 0);
	myphar->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(myphar->fname, fname_len);
#endif
	myphar->fname_len = fname_len;
	myphar->fp = fp;
	phar_request_initialize(TSRMLS_C);
	if (SUCCESS != zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), myphar->fname, fname_len, (void*)&myphar, sizeof(phar_archive_data*), (void **)&actual)) {
		if (error) {
			spprintf(error, 4096, "phar error: Unable to add tar-based phar \"%s\" to phar registry", fname);
		}
		php_stream_close(fp);
		zend_hash_destroy(&myphar->manifest);
		myphar->manifest.arBuckets = 0;
		efree(myphar);
		return FAILURE;
	}
	myphar = *actual;
	if (actual_alias) {
		myphar->is_explicit_alias = 1;
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), actual_alias, myphar->alias_len, (void*)&myphar, sizeof(phar_archive_data*), NULL);
	} else {
		myphar->alias = estrndup(myphar->fname, fname_len);
		myphar->alias_len = fname_len;
		myphar->is_explicit_alias = 0;
	}
	if (pphar) {
		*pphar = myphar;
	}
	return SUCCESS;
}
/* }}} */

struct _phar_pass_tar_info {
	php_stream *old;
	php_stream *new;
	char **error;
};

int phar_tar_writeheaders(void *pDest, void *argument TSRMLS_DC)
{
	tar_header header;
	size_t pos;
	phar_entry_info *entry = (phar_entry_info *) pDest;
	struct _phar_pass_tar_info *fp = (struct _phar_pass_tar_info *)argument;
	char padding[512];

	if (entry->is_deleted) {
		return ZEND_HASH_APPLY_REMOVE;
	}

	memset((char *) &header, 0, sizeof(header));
	if (entry->filename_len > 100) {
		if (entry->filename_len > 255) {
			if (fp->error) {
				spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, filename \"%s\" is too long for tar file format", entry->phar->fname, entry->filename);
			}
			return ZEND_HASH_APPLY_STOP;
		}
		memcpy(header.prefix, entry->filename+100, entry->filename_len - 100);
		memcpy(header.name, entry->filename, 100);
	} else {
		memcpy(header.name, entry->filename, entry->filename_len);
	}
	phar_tar_octal(header.mode, entry->flags & PHAR_ENT_PERM_MASK, sizeof(header.mode));
	if (FAILURE == phar_tar_octal(header.size, entry->uncompressed_filesize, sizeof(header.size))) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, filename \"%s\" is too large for tar file format", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}
	if (FAILURE == phar_tar_octal(header.mtime, entry->timestamp, sizeof(header.mtime))) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, file modification time of file \"%s\" is too large for tar file format", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}
	/* calc checksum */
	header.typeflag = entry->tar_type;
	if (entry->link) {
		strcpy(header.linkname, entry->link);
	}
	strcpy(header.magic, "ustar");
	strcpy(header.version, "00");
	strcpy(header.checksum, "        ");
	entry->crc32 = phar_tar_checksum((char *)&header, sizeof(header));
	if (FAILURE == phar_tar_octal(header.checksum, entry->crc32, sizeof(header.checksum))) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, checksum of file \"%s\" is too large for tar file format", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}

	/* write header */
	entry->header_offset = php_stream_tell(fp->new);
	if (sizeof(header) != php_stream_write(fp->new, (char *) &header, sizeof(header))) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, header for  file \"%s\" could not be written", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}
	pos = php_stream_tell(fp->new); /* save start of file within tar */

	/* write contents */
	if (FAILURE == phar_open_entry_fp(entry, fp->error TSRMLS_CC)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0 TSRMLS_CC)) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, contents of file \"%s\" could not be written, seek failed", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}
	if (entry->uncompressed_filesize != php_stream_copy_to_stream(phar_get_efp(entry), fp->new, entry->uncompressed_filesize)) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, contents of file \"%s\" could not be written", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}

	memset(padding, 0, 512);
	php_stream_write(fp->new, padding, ((entry->uncompressed_filesize +511)&~511) - entry->uncompressed_filesize);

	entry->is_modified = 0;
	if (entry->fp_type == PHAR_MOD && entry->fp != entry->phar->fp && entry->fp != entry->phar->ufp) {
		if (!entry->fp_refcount) {
			php_stream_close(entry->fp);
		}
		entry->fp = NULL;
	}
	entry->fp_type = PHAR_FP;

	/* note new location within tar */
	entry->offset = entry->offset_abs = pos;
	return ZEND_HASH_APPLY_KEEP;
}

int phar_tar_flush(phar_archive_data *phar, char *user_stub, long len, char **error TSRMLS_DC) /* {{{ */
{
	phar_entry_info entry = {0};
	static const char newstub[] = "<?php // tar-based phar archive stub file\n__HALT_COMPILER();";
	php_stream *oldfile, *newfile, *stubfile;
	int closeoldfile, free_user_stub;
	struct _phar_pass_tar_info pass;
	char *buf;

	entry.flags = PHAR_ENT_PERM_DEF_FILE;
	entry.timestamp = time(NULL);
	entry.is_modified = 1;
	entry.is_crc_checked = 1;
	entry.is_tar = 1;
	entry.tar_type = '0';
	entry.phar = phar;
	entry.fp_type = PHAR_MOD;
	/* set alias */
	if (phar->is_explicit_alias) {
		entry.filename = estrndup(".phar/alias.txt", sizeof(".phar/alias.txt")-1);
		entry.filename_len = sizeof(".phar/alias.txt")-1;
		entry.fp = php_stream_fopen_tmpfile();
		entry.crc32 = phar_tar_checksum(phar->alias, phar->alias_len);
		php_stream_write(entry.fp, phar->alias, phar->alias_len);
		entry.uncompressed_filesize = phar->alias_len;
		zend_hash_update(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL);
	}

	/* set stub */
	if (user_stub) {
		char *pos;
		if (len < 0) {
			/* resource passed in */
			if (!(php_stream_from_zval_no_verify(stubfile, (zval **)user_stub))) {
				if (error) {
					spprintf(error, 0, "unable to access resource to copy stub to new tar-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
			if (len == -1) {
				len = PHP_STREAM_COPY_ALL;
			} else {
				len = -len;
			}
			user_stub = 0;
			if (!(len = php_stream_copy_to_mem(stubfile, &user_stub, len, 0)) || !user_stub) {
				if (error) {
					spprintf(error, 0, "unable to read resource to copy stub to new tar-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
			free_user_stub = 1;
		} else {
			free_user_stub = 0;
		}
		if ((pos = strstr(user_stub, "__HALT_COMPILER();")) == NULL)
		{
			if (error) {
				spprintf(error, 0, "illegal stub for tar-based phar \"%s\"", phar->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		len = pos - user_stub + 18;
		entry.fp = php_stream_fopen_tmpfile();
		entry.uncompressed_filesize = len + 5;

		if ((size_t)len != php_stream_write(entry.fp, user_stub, len)
		||            5 != php_stream_write(entry.fp, " ?>\r\n", 5)) {
			if (error) {
				spprintf(error, 0, "unable to create stub from string in new tar-based phar \"%s\"", phar->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			php_stream_close(entry.fp);
			return EOF;
		}
		entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
		entry.filename_len = sizeof(".phar/stub.php")-1;
		zend_hash_update(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL);
		if (free_user_stub) {
			efree(user_stub);
		}
	} else {
		if (!zend_hash_exists(&phar->manifest, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
			/* this is a brand new phar */
			entry.fp = php_stream_fopen_tmpfile();
			if (sizeof(newstub)-1 != php_stream_write(entry.fp, newstub, sizeof(newstub)-1)) {
				if (error) {
					spprintf(error, 0, "unable to create stub in new tar-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
			entry.uncompressed_filesize = sizeof(newstub) - 1;
			entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
			entry.filename_len = sizeof(".phar/stub.php")-1;
			zend_hash_add(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL);
		}
	}

	if (phar->fp && !phar->is_brandnew) {
		oldfile = phar->fp;
		closeoldfile = 0;
		php_stream_rewind(oldfile);
	} else {
		oldfile = php_stream_open_wrapper(phar->fname, "rb", 0, NULL);
		closeoldfile = oldfile != NULL;
	}
	newfile = php_stream_fopen_tmpfile();
	if (!newfile) {
		if (error) {
			spprintf(error, 0, "unable to create temporary file");
		}
		if (closeoldfile) {
			php_stream_close(oldfile);
		}
		return EOF;
	}

	pass.old = oldfile;
	pass.new = newfile;
	pass.error = error;

	zend_hash_apply_with_argument(&phar->manifest, (apply_func_arg_t) phar_tar_writeheaders, (void *) &pass TSRMLS_CC);

	/* add final zero blocks */
	buf = (char *) ecalloc(1024, 1);
	php_stream_write(newfile, buf, 1024);
	efree(buf);

	if (closeoldfile) {
		php_stream_close(oldfile);
	}
	/* on error in the hash iterator above, error is set */
	if (error && *error) {
		php_stream_close(newfile);
		return EOF;
	}
	if (phar->fp) {
		php_stream_close(phar->fp);
	}
	if (phar->ufp) {
		php_stream_close(phar->ufp);
		phar->ufp = NULL;
	}

	phar->is_brandnew = 0;

	php_stream_rewind(newfile);

	if (phar->donotflush) {
		/* deferred flush */
		phar->fp = newfile;
	} else {
		phar->fp = php_stream_open_wrapper(phar->fname, "w+b", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
		if (!phar->fp) {
			phar->fp = newfile;
			if (error) {
				spprintf(error, 0, "unable to open new phar \"%s\" for writing", phar->fname);
			}
			return EOF;
		}
		if (phar->flags & PHAR_FILE_COMPRESSED_GZ) {
			php_stream_filter *filter;
			/* to properly compress, we have to tell zlib to add a zlib header */
			zval filterparams;

			array_init(&filterparams);
			/* ext/zlib zval_dtors a separated zval, so we have to make sure it doesn't destroy ours */
#if PHP_VERSION_ID < 50300
			filterparams->refcount = 26;
#else
			Z_SET_REFCOUNT(filterparams, 26);
#endif
/* this is defined in zlib's zconf.h */
#ifndef MAX_WBITS
#define MAX_WBITS 15
#endif
			add_assoc_long(&filterparams, "window", MAX_WBITS + 16);
			filter = php_stream_filter_create("zlib.deflate", &filterparams, php_stream_is_persistent(phar->fp) TSRMLS_CC);
			zval_dtor(&filterparams);
			if (!filter) {
				/* copy contents uncompressed rather than lose them */
				php_stream_copy_to_stream(newfile, phar->fp, PHP_STREAM_COPY_ALL);
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 4096, "unable to compress all contents of phar \"%s\" using zlib, PHP versions older than 5.2.6 have a buggy zlib", phar->fname);
				}
				return EOF;
			}
			php_stream_filter_append(&phar->fp->writefilters, filter);
			php_stream_copy_to_stream(newfile, phar->fp, PHP_STREAM_COPY_ALL);
			php_stream_filter_flush(filter, 1);
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			php_stream_close(phar->fp);
			/* use the temp stream as our base */
			phar->fp = newfile;
		} else if (phar->flags & PHAR_FILE_COMPRESSED_BZ2) {
			php_stream_filter *filter;

			filter = php_stream_filter_create("bzip2.compress", NULL, php_stream_is_persistent(phar->fp) TSRMLS_CC);
			php_stream_filter_append(&phar->fp->writefilters, filter);
			php_stream_copy_to_stream(newfile, phar->fp, PHP_STREAM_COPY_ALL);
			php_stream_filter_flush(filter, 1);
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			php_stream_close(phar->fp);
			/* use the temp stream as our base */
			phar->fp = newfile;
		} else {
			php_stream_copy_to_stream(newfile, phar->fp, PHP_STREAM_COPY_ALL);
			/* we could also reopen the file in "rb" mode but there is no need for that */
			php_stream_close(newfile);
		}
	}
	return EOF;
}
/* }}} */
