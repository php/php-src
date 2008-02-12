/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  | utility functions                                                    |
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

/* retrieve a phar_entry_info's current file pointer for reading contents */
php_stream *phar_get_efp(phar_entry_info *entry)
{
	if (entry->fp_type == PHAR_FP) {
		if (!entry->phar->fp) {
			/* re-open just in time for cases where our refcount reached 0 on the phar archive */
			phar_open_archive_fp(entry->phar TSRMLS_CC);
		}
		return entry->phar->fp;
	} else if (entry->fp_type == PHAR_UFP) {
		return entry->phar->ufp;
	} else {
		return entry->fp;
	}
}

int phar_seek_efp(phar_entry_info *entry, off_t offset, int whence, off_t position TSRMLS_DC)
{
	php_stream *fp = phar_get_efp(entry);
	off_t temp;

	if (entry->is_dir) {
		return 0;
	}
	switch (whence) {
		case SEEK_END :
			temp = entry->offset + entry->uncompressed_filesize + offset;
			break;
		case SEEK_CUR :
			temp = entry->offset + position + offset;
			break;
		case SEEK_SET :
			temp = entry->offset + offset;
			break;
	}
	if (temp > entry->offset + (off_t) entry->uncompressed_filesize) {
		return -1;
	}
	if (temp < entry->offset) {
		return -1;
	}
	return php_stream_seek(fp, temp, SEEK_SET);
}

/**
 * Retrieve a copy of the file information on a single file within a phar, or null.
 * This also transfers the open file pointer, if any, to the entry.
 *
 * If the file does not already exist, this will fail.  Pre-existing files can be
 * appended, truncated, or read.  For read, if the entry is marked unmodified, it is
 * assumed that the file pointer, if present, is opened for reading
 */
int phar_get_entry_data(phar_entry_data **ret, char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry;
	int for_write  = mode[0] != 'r' || mode[1] == '+';
	int for_append = mode[0] == 'a';
	int for_create = mode[0] != 'r';
	int for_trunc  = mode[0] == 'w';

	if (!ret) {
		return FAILURE;
	}
	*ret = NULL;
	if (error) {
		*error = NULL;
	}
	if (for_write && PHAR_G(readonly)) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, disabled by ini setting", path, fname);
		}
		return FAILURE;
	}
	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return FAILURE;
	}
	if (!path_len) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"\" in phar \"%s\" cannot be empty", fname);
		}
		return FAILURE;
	}
	if (allow_dir) {
		if ((entry = phar_get_entry_info_dir(phar, path, path_len, 2, for_create && !PHAR_G(readonly) ? NULL : error TSRMLS_CC)) == NULL) {
			if (for_create && !PHAR_G(readonly)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	} else {
		if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) ? NULL : error TSRMLS_CC)) == NULL) {
			if (for_create && !PHAR_G(readonly)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	}
	if (entry->is_modified && !for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for reading, writable file pointers are open", path, fname);
		}
		return FAILURE;
	}
	if (entry->fp_refcount && for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, readable file pointers are open", path, fname);
		}
		return FAILURE;
	}
	if (entry->is_deleted) {
		if (!for_create) {
			return FAILURE;
		}
		entry->is_deleted = 0;
	}
	if (entry->is_dir) {
		*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
		(*ret)->position = 0;
		(*ret)->phar = phar;
		(*ret)->for_write = for_write;
		(*ret)->internal_file = entry;
		(*ret)->is_zip = entry->is_zip;
		(*ret)->is_tar = entry->is_tar;
		entry->phar->refcount++;
		entry->fp_refcount++;
		return SUCCESS;
	}
	if (entry->fp_type == PHAR_MOD) {
		if (for_trunc) {
			if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
				return FAILURE;
			}
		} else if (for_append) {
			phar_seek_efp(entry, 0, SEEK_END, 0 TSRMLS_CC);
		}
	} else {
		if (for_write) {
			if (for_trunc) {
				if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
					return FAILURE;
				}
			} else {
				if (FAILURE == phar_separate_entry_fp(entry, error TSRMLS_CC)) {
					return FAILURE;
				}
			}
		} else {
			if (FAILURE == phar_open_entry_fp(entry, error TSRMLS_CC)) {
				return FAILURE;
			}
		}
	}
	*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	(*ret)->position = 0;
	(*ret)->phar = phar;
	(*ret)->for_write = for_write;
	(*ret)->internal_file = entry;
	(*ret)->is_zip = entry->is_zip;
	(*ret)->is_tar = entry->is_tar;
	(*ret)->fp = phar_get_efp(entry);
	(*ret)->zero = entry->offset;
	entry->fp_refcount++;
	entry->phar->refcount++;
	return SUCCESS;
}
/* }}} */

/**
 * Create a new dummy file slot within a writeable phar for a newly created file
 */
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, etemp;
	phar_entry_data *ret;
	const char *pcr_error;
	char is_dir = (path_len > 0 && path != NULL) ? path[path_len - 1] == '/' : 0;

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return NULL;
	}

	if (FAILURE == phar_get_entry_data(&ret, fname, fname_len, path, path_len, mode, allow_dir, error TSRMLS_CC)) {
		return NULL;
	} else if (ret) {
		return ret;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 0, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	/* create a new phar data holder */
	ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));

	/* create an entry, this is a new file */
	memset(&etemp, 0, sizeof(phar_entry_info));
	etemp.filename_len = path_len;
	etemp.fp_type = PHAR_MOD;
	etemp.fp = php_stream_fopen_tmpfile();
	if (!etemp.fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		return NULL;
	}
	etemp.fp_refcount = 1;

	if (is_dir) {
		etemp.is_dir = 1;
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_DIR;
		etemp.filename_len--; /* strip trailing / */
		path_len--;
	} else {
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_FILE;
	}
	etemp.is_modified = 1;
	etemp.timestamp = time(0);
	etemp.is_crc_checked = 1;
	etemp.phar = phar;
	etemp.filename = estrndup(path, path_len);
	etemp.is_zip = phar->is_zip;
	if (phar->is_tar) {
		etemp.is_tar = phar->is_tar;
		etemp.tar_type = TAR_FILE;
	}
	if (FAILURE == zend_hash_add(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info), (void **) &entry)) {
		if (error) {
			spprintf(error, 0, "phar error: unable to add new entry \"%s\" to phar \"%s\"", etemp.filename, phar->fname);
		}
		return NULL;
	}
	
	if (!entry) {
		php_stream_close(etemp.fp);
		efree(etemp.filename);
		return NULL;
	}

	phar->refcount++;
	ret->phar = phar;
	ret->fp = entry->fp;
	ret->position = ret->zero = 0;
	ret->for_write = 1;
	ret->is_zip = entry->is_zip;
	ret->is_tar = entry->is_tar;
	ret->internal_file = entry;
	return ret;
}
/* }}} */

/* initialize a phar_archive_data's read-only fp for existing phar data */
int phar_open_archive_fp(phar_archive_data *phar TSRMLS_DC) 
{
	if (phar->fp) {
		return SUCCESS;
	}

#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(phar->fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return FAILURE;
	}
#endif
	
	if (php_check_open_basedir(phar->fname TSRMLS_CC)) {
		return FAILURE;
	}

	phar->fp = php_stream_open_wrapper(phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL);
	if (!phar->fp) {
		return FAILURE;
	}
	return SUCCESS;
}

/* copy file data from an existing to a new phar_entry_info that is not in the manifest */
int phar_copy_entry_fp(phar_entry_info *source, phar_entry_info *dest, char **error TSRMLS_DC)
{
	if (FAILURE == phar_open_entry_fp(source, error TSRMLS_CC)) {
		return FAILURE;
	}
	dest->fp_type = PHAR_MOD;
	dest->offset = 0;
	dest->is_modified = 1;
	dest->fp = php_stream_fopen_tmpfile();

	phar_seek_efp(source, 0, SEEK_SET, 0 TSRMLS_CC);
	if (source->uncompressed_filesize != php_stream_copy_to_stream(phar_get_efp(source), dest->fp, source->uncompressed_filesize)) {
		php_stream_close(dest->fp);
		dest->fp_type = PHAR_FP;
		if (error) {
			spprintf(error, 4096, "phar error: unable to copy contents of file \"%s\" to \"%s\" in phar archive \"%s\"", source->filename, dest->filename, source->phar->fname);
		}
		return FAILURE;
	}
	return SUCCESS;
}

/* open and decompress a compressed phar entry
 */
int phar_open_entry_fp(phar_entry_info *entry, char **error TSRMLS_DC) 
{
	php_stream_filter *filter;
	phar_archive_data *phar = entry->phar;
	char *filtername;
	off_t loc;

	if (entry->fp_type != PHAR_FP) {
		/* either newly created or already modified */
		return SUCCESS;
	}
	if (!phar->fp) {
		if (FAILURE == phar_open_archive_fp(phar TSRMLS_CC)) {
			spprintf(error, 4096, "phar error: Cannot open phar archive \"%s\" for reading", phar->fname);
			return FAILURE;
		}
	}
	if (!(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
		return SUCCESS;
	}
	if (!phar->ufp) {
		phar->ufp = php_stream_fopen_tmpfile();
		if (!phar->ufp) {
			spprintf(error, 4096, "phar error: Cannot open temporary file for decompressing phar archive \"%s\" file \"%s\"", phar->fname, entry->filename);
			return FAILURE;
		}
	}

	if ((filtername = phar_decompress_filter(entry, 0)) != NULL) {
		filter = php_stream_filter_create(filtername, NULL, php_stream_is_persistent(phar->ufp) TSRMLS_CC);
	} else {
		filter = NULL;
	}
	if (!filter) {
		spprintf(error, 4096, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", phar->fname, phar_decompress_filter(entry, 1), entry->filename);
		return FAILURE;
	}
	/* now we can safely use proper decompression */
	/* save the new offset location within ufp */
	php_stream_seek(phar->ufp, 0, SEEK_END);
	loc = php_stream_tell(phar->ufp);
	php_stream_filter_append(&phar->ufp->writefilters, filter);
	php_stream_seek(phar->fp, entry->offset, SEEK_SET);
	if (php_stream_copy_to_stream(phar->fp, phar->ufp, entry->compressed_filesize) != entry->compressed_filesize) {
		spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		return FAILURE;
	}
	php_stream_filter_flush(filter, 1);
	php_stream_flush(phar->ufp);
	php_stream_filter_remove(filter, 1 TSRMLS_CC);
	if (php_stream_tell(phar->ufp) - loc != (off_t) entry->uncompressed_filesize) {
		spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
		return FAILURE;
	}

	entry->old_flags = entry->flags;
	entry->fp_type = PHAR_UFP;
	/* this is now the new location of the file contents within this fp */
	entry->offset = loc;

	return SUCCESS;
}

#if defined(PHP_VERSION_ID) && PHP_VERSION_ID < 50202
typedef struct {
	char        *data;
	size_t      fpos;
	size_t      fsize;
	size_t      smax;
	int         mode;
	php_stream  **owner_ptr;
} php_stream_memory_data;
#endif

int phar_create_writeable_entry(phar_archive_data *phar, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	if (entry->fp_type == PHAR_MOD) {
		/* already newly created, truncate */
#if PHP_VERSION_ID >= 50202
		php_stream_truncate_set_size(entry->fp, 0);
#else
		if (php_stream_is(entry->fp, PHP_STREAM_IS_TEMP)) {
			if (php_stream_is(*(php_stream**)entry->fp->abstract, PHP_STREAM_IS_MEMORY)) {
				php_stream *inner = *(php_stream**)entry->fp->abstract;
				php_stream_memory_data *memfp = (php_stream_memory_data*)inner->abstract;
				memfp->fpos = 0;
				memfp->fsize = 0;
			} else if (php_stream_is(*(php_stream**)entry->fp->abstract, PHP_STREAM_IS_STDIO)) {
				php_stream_truncate_set_size(*(php_stream**)entry->fp->abstract, 0);
			} else {
				efree(*ret);
				*ret = NULL;
				if (error) {
					spprintf(error, 0, "phar error: file \"%s\" cannot be opened for writing, no truncate support", fname);
				}
				return FAILURE;
			}
		} else if (php_stream_is(entry->fp, PHP_STREAM_IS_STDIO)) {
			php_stream_truncate_set_size(entry->fp, 0);
		} else {
			efree(*ret);
			*ret = NULL;
			if (error) {
				spprintf(error, 0, "phar error: file \"%s\" cannot be opened for writing, no truncate support", fname);
			}
			return FAILURE;
		}
#endif
		entry->old_flags = entry->flags;
		entry->is_modified = 1;
		phar->is_modified = 1;
		/* reset file size */
		entry->uncompressed_filesize = 0;
		entry->compressed_filesize = 0;
		entry->crc32 = 0;
		entry->flags = PHAR_ENT_PERM_DEF_FILE;
		entry->fp_type = PHAR_MOD;
		entry->offset = 0;
		return SUCCESS;
	}
	if (error) {
		*error = NULL;
	}
	/* open a new temp file for writing */
	entry->fp = php_stream_fopen_tmpfile();
	if (!entry->fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		return FAILURE;
	}
	entry->old_flags = entry->flags;
	entry->is_modified = 1;
	phar->is_modified = 1;
	/* reset file size */
	entry->uncompressed_filesize = 0;
	entry->compressed_filesize = 0;
	entry->crc32 = 0;
	entry->flags = PHAR_ENT_PERM_DEF_FILE;
	entry->fp_type = PHAR_MOD;
	entry->offset = 0;
	return SUCCESS;
}
/* }}} */

int phar_separate_entry_fp(phar_entry_info *entry, char **error TSRMLS_DC)
{
	php_stream *fp;

	if (FAILURE == phar_open_entry_fp(entry, error TSRMLS_CC)) {
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		return SUCCESS;
	}

	fp = php_stream_fopen_tmpfile();
	phar_seek_efp(entry, 0, SEEK_SET, 0 TSRMLS_CC);
	if (entry->uncompressed_filesize != php_stream_copy_to_stream(phar_get_efp(entry), fp, entry->uncompressed_filesize)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot separate entry file \"%s\" contents in phar archive \"%s\" for write access", entry->filename, entry->phar->fname);
		}
		return FAILURE;
	}

	entry->offset = 0;
	entry->fp = fp;
	entry->fp_type = PHAR_MOD;
	entry->is_modified = 1;
	return SUCCESS;
}

/**
 * helper function to open an internal file's fp just-in-time
 */
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, php_stream *fp,
				      char **error, int for_write TSRMLS_DC)
{
	if (error) {
		*error = NULL;
	}
	/* seek to start of internal file and read it */
	if (FAILURE == phar_open_entry_fp(entry, error TSRMLS_CC)) {
		return NULL;
	}
	if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0 TSRMLS_CC)) {
		spprintf(error, 4096, "phar error: cannot seek to start of file \"%s\" in phar \"%s\"", entry->filename, phar->fname);
		return NULL;
	}
	return entry;
}

/**
 * Looks up a phar archive in the filename map, connecting it to the alias
 * (if any) or returns null
 */
int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *fd, **fd_ptr;
	char *my_realpath, *save;
	int save_len;

	phar_request_initialize(TSRMLS_C);

	if (error) {
		*error = NULL;
	}
	*archive = NULL;
	if (alias && alias_len) {
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void**)&fd_ptr)) {
			if (fname && (fname_len != (*fd_ptr)->fname_len || strncmp(fname, (*fd_ptr)->fname, fname_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
				}
				return FAILURE;
			}
			*archive = *fd_ptr;
			return SUCCESS;
		}
	}
	my_realpath = NULL;
	save = fname;
	save_len = fname_len;
	if (fname && fname_len) {
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;
			if (alias && alias_len) {
				zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&fd,   sizeof(phar_archive_data*), NULL);
			}
			return SUCCESS;
		}
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), save, save_len, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			return SUCCESS;
		}

		/* not found, try converting \ to / */
		my_realpath = expand_filepath(fname, my_realpath TSRMLS_CC);
		if (my_realpath) {
			fname_len = strlen(my_realpath);
			fname = my_realpath;
		} else {
			return FAILURE;
		}
#ifdef PHP_WIN32
		phar_unixify_path_separators(fname, fname_len);
#endif
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;
			if (alias && alias_len) {
				zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&fd,   sizeof(phar_archive_data*), NULL);
			}
			efree(my_realpath);
			return SUCCESS;
		}
		efree(my_realpath);
	}
	return FAILURE;
}
/* }}} */

/**
 * Determine which stream compression filter (if any) we need to read this file
 */
char * phar_compress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	switch (entry->flags & PHAR_ENT_COMPRESSION_MASK) {
	case PHAR_ENT_COMPRESSED_GZ:
		return "zlib.deflate";
	case PHAR_ENT_COMPRESSED_BZ2:
		return "bzip2.compress";
	default:
		return return_unknown ? "unknown" : NULL;
	}
}
/* }}} */

/**
 * Determine which stream decompression filter (if any) we need to read this file
 */
char * phar_decompress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	php_uint32 flags;

	if (entry->is_modified) {
		flags = entry->old_flags;
	} else {
		flags = entry->flags;
	}
	switch (flags & PHAR_ENT_COMPRESSION_MASK) {
	case PHAR_ENT_COMPRESSED_GZ:
		return "zlib.inflate";
	case PHAR_ENT_COMPRESSED_BZ2:
		return "bzip2.decompress";
	default:
		return return_unknown ? "unknown" : NULL;
	}
}
/* }}} */

/**
 * retrieve information on a file contained within a phar, or null if it ain't there
 */
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error TSRMLS_DC) /* {{{ */
{
	return phar_get_entry_info_dir(phar, path, path_len, 0, error TSRMLS_CC);
}
/* }}} */
/**
 * retrieve information on a file or directory contained within a phar, or null if none found
 * allow_dir is 0 for none, 1 for both empty directories in the phar and temp directories, and 2 for only
 * valid pre-existing empty directory entries
 */
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, int path_len, char dir, char **error TSRMLS_DC) /* {{{ */
{
	const char *pcr_error;
	phar_entry_info *entry;
	char is_dir = (path[path_len - 1] == '/');

	if (error) {
		*error = NULL;
	}

	if (!path_len && !dir) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" must not be empty", path);
		}
		return NULL;
	}
	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (!&phar->manifest.arBuckets) {
		return NULL;
	}
	if (is_dir) {
		path_len--;
	}
	if (SUCCESS == zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
		if (entry->is_deleted) {
			/* entry is deleted, but has not been flushed to disk yet */
			return NULL;
		}
		if (entry->is_dir && !dir) {
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
			}
			return NULL;
		}
		if (!entry->is_dir && is_dir) {
			/* user requested a directory, we must return one */
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
			}
			return NULL;
		}
		return entry;
	}
	if (dir == 1) {
		/* try to find a directory */
		HashTable *manifest;
		char *key;
		uint keylen;
		ulong unused;

		if (!path_len) {
			path = "/";
		}
		manifest = &phar->manifest;
		zend_hash_internal_pointer_reset(manifest);
		while (FAILURE != zend_hash_has_more_elements(manifest)) {
			if (HASH_KEY_NON_EXISTANT == zend_hash_get_current_key_ex(manifest, &key, &keylen, &unused, 0, NULL)) {
				break;
			}
			if (0 != memcmp(key, path, path_len)) {
				/* entry in directory not found */
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			} else {
				if (key[path_len] != '/') {
					if (SUCCESS != zend_hash_move_forward(manifest)) {
						break;
					}
					continue;
				}
				/* found a file in this path */
				entry = (phar_entry_info *) ecalloc(1, sizeof(phar_entry_info));
				/* this next line tells PharFileInfo->__destruct() to efree the filename */
				entry->is_temp_dir = entry->is_dir = 1;
				entry->filename = (char *) estrndup(path, path_len + 1);
				entry->filename_len = path_len;
				return entry;
			}
		}
	}
	return NULL;
}
/* }}} */
