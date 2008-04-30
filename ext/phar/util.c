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
#if !defined(PHP_VERSION_ID) || PHP_VERSION_ID < 50300
extern php_stream_wrapper php_stream_phar_wrapper;
#endif

/* for links to relative location, prepend cwd of the entry */
static char *phar_get_link_location(phar_entry_info *entry TSRMLS_DC)
{
	char *tmp, *p, *ret = NULL;
	if (!entry->link) {
		return NULL;
	}
	if (entry->link[0] == '/') {
		return entry->link;
	}
	tmp = estrndup(entry->filename, entry->filename_len);
	p = strrchr(tmp, '/');
	if (p) {
		*p = '\0';
		spprintf(&ret, 0, "%s/%s", tmp, entry->link);
		efree(tmp);
		return ret;
	}
	efree(ret);
	return entry->link;
}

phar_entry_info *phar_get_link_source(phar_entry_info *entry TSRMLS_DC)
{
	phar_entry_info *link_entry;
	char *link = phar_get_link_location(entry TSRMLS_CC);

	if (!entry->link) {
		return entry;
	}

	if (SUCCESS == zend_hash_find(&(entry->phar->manifest), entry->link, strlen(entry->link), (void **)&link_entry) ||
		SUCCESS == zend_hash_find(&(entry->phar->manifest), link, strlen(link), (void **)&link_entry)) {
		if (link != entry->link) {
			efree(link);
		}
		return phar_get_link_source(link_entry TSRMLS_CC);
	} else {
		if (link != entry->link) {
			efree(link);
		}
		return NULL;
	}
}

/* retrieve a phar_entry_info's current file pointer for reading contents */
php_stream *phar_get_efp(phar_entry_info *entry, int follow_links TSRMLS_DC)
{
	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry TSRMLS_CC);

		if (link_entry && link_entry != entry) {
			return phar_get_efp(link_entry, 1 TSRMLS_CC);
		}
	}
	if (entry->fp_type == PHAR_FP) {
		if (!entry->phar->fp) {
			/* re-open just in time for cases where our refcount reached 0 on the phar archive */
			phar_open_archive_fp(entry->phar TSRMLS_CC);
		}
		return entry->phar->fp;
	} else if (entry->fp_type == PHAR_UFP) {
		return entry->phar->ufp;
	} else if (entry->fp_type == PHAR_MOD) {
		return entry->fp;
	} else {
		/* temporary manifest entry */
		if (!entry->fp) {
			entry->fp = php_stream_open_wrapper(entry->tmp, "rb", STREAM_MUST_SEEK|0, NULL);
		}
		return entry->fp;
	}
}

int phar_seek_efp(phar_entry_info *entry, off_t offset, int whence, off_t position, int follow_links TSRMLS_DC)
{
	php_stream *fp = phar_get_efp(entry, follow_links TSRMLS_CC);
	off_t temp;

	if (!fp) {
		return -1;
	}
	if (follow_links) {
		phar_entry_info *t;
		t = phar_get_link_source(entry TSRMLS_CC);
		if (t) {
			entry = t;
		}
	}
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

/* mount an absolute path or uri to a path internal to the phar archive */
int phar_mount_entry(phar_archive_data *phar, char *filename, int filename_len, char *path, int path_len TSRMLS_DC)
{
	phar_entry_info entry = {0};
	php_stream_statbuf ssb;
	int is_phar;
	const char *err;

	if (phar_path_check(&path, &path_len, &err) > pcr_is_ok) {
		return FAILURE;
	}

	is_phar = (filename_len > 7 && !memcmp(filename, "phar://", 7));

	entry.phar = phar;
	entry.filename = estrndup(path, path_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(entry.filename, path_len);
#endif
	entry.filename_len = path_len;
	if (is_phar) {
		entry.tmp = estrndup(filename, filename_len);
	} else {
		entry.tmp = expand_filepath(filename, NULL TSRMLS_CC);
		if (!entry.tmp) {
			entry.tmp = estrndup(filename, filename_len);
		}
	}
#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && !is_phar && (!php_checkuid(entry.tmp, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		efree(entry.tmp);
		efree(entry.filename);
		return FAILURE;
	}
#endif

	filename_len = strlen(entry.tmp);
	filename = entry.tmp;
	/* only check openbasedir for files, not for phar streams */
	if (!is_phar && php_check_open_basedir(filename TSRMLS_CC)) {
		efree(entry.tmp);
		efree(entry.filename);
		return FAILURE;
	}
	entry.is_mounted = 1;
	entry.is_crc_checked = 1;
	entry.fp_type = PHAR_TMP;

	if (SUCCESS != php_stream_stat_path(filename, &ssb)) {
		efree(entry.tmp);
		efree(entry.filename);
		return FAILURE;
	}
	if (ssb.sb.st_mode & S_IFDIR) {
		entry.is_dir = 1;
		if (SUCCESS != zend_hash_add(&phar->mounted_dirs, entry.filename, path_len, (void *)&(entry.filename), sizeof(char *), NULL)) {
			/* directory already mounted */
			efree(entry.tmp);
			efree(entry.filename);
			return FAILURE;
		}
	} else {
		entry.is_dir = 0;
		entry.uncompressed_filesize = entry.compressed_filesize = ssb.sb.st_size;
	}
	entry.flags = ssb.sb.st_mode;
	if (SUCCESS == zend_hash_add(&phar->manifest, entry.filename, path_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
		return SUCCESS;
	}
	efree(entry.tmp);
	efree(entry.filename);
	return FAILURE;
}

char *phar_find_in_include_path(char *filename, int filename_len, phar_archive_data **pphar TSRMLS_DC) /* {{{ */
{
#if PHP_VERSION_ID >= 50300
	char *path, *fname, *arch, *entry, *ret, *test;
	int arch_len, entry_len;

	if (pphar) {
		*pphar = NULL;
	}

	if (!zend_is_executing(TSRMLS_C) || !PHAR_G(cwd)) {
		return phar_save_resolve_path(filename, filename_len TSRMLS_CC);
	}
	fname = zend_get_executed_filename(TSRMLS_C);
	if (SUCCESS != phar_split_fname(fname, strlen(fname), &arch, &arch_len, &entry, &entry_len, 0, 0 TSRMLS_CC)) {
		return phar_save_resolve_path(filename, filename_len TSRMLS_CC);
	}
	if (*filename == '.') {
		int try_len;

		if (SUCCESS != (zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **) &pphar))) {
			efree(arch);
			efree(entry);
			return phar_save_resolve_path(filename, filename_len TSRMLS_CC);
		}
		efree(entry);
		try_len = filename_len;
		test = phar_fix_filepath(estrndup(filename, filename_len), &try_len, 1 TSRMLS_CC);
		if (*test == '/') {
			if (zend_hash_exists(&((*pphar)->manifest), test + 1, try_len - 1)) {
				spprintf(&ret, 0, "phar://%s%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		} else {
			if (zend_hash_exists(&((*pphar)->manifest), test, try_len)) {
				spprintf(&ret, 0, "phar://%s/%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		}
	}
	efree(entry);
	spprintf(&path, MAXPATHLEN, "phar://%s/%s%c%s", arch, PHAR_G(cwd), DEFAULT_DIR_SEPARATOR, PG(include_path));
	efree(arch);
	ret = php_resolve_path(filename, filename_len, path TSRMLS_CC);
	efree(path);
	if (ret && strlen(ret) > 8 && !strncmp(ret, "phar://", 7)) {
		char *arch, *entry;
		int arch_len, entry_len, ret_len;

		ret_len = strlen(ret);
		/* found phar:// */

		if (SUCCESS != phar_split_fname(ret, ret_len, &arch, &arch_len, &entry, &entry_len, 0, 0 TSRMLS_CC)) {
			return ret;
		}
		zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **) &pphar);
		efree(arch);
		efree(entry);
	}
	return ret;
#else /* PHP 5.2 */
	char resolved_path[MAXPATHLEN];
	char trypath[MAXPATHLEN];
	char *ptr, *end, *path = PG(include_path);
	php_stream_wrapper *wrapper;
	const char *p;
	int n = 0;
	char *fname, *arch, *entry, *ret, *test;
	int arch_len, entry_len;

	if (!filename) {
		return NULL;
	}

	if (!zend_is_executing(TSRMLS_C) || !PHAR_G(cwd)) {
		goto doit;
	}
	fname = zend_get_executed_filename(TSRMLS_C);
	if (SUCCESS != phar_split_fname(fname, strlen(fname), &arch, &arch_len, &entry, &entry_len, 0, 0 TSRMLS_CC)) {
		goto doit;
	}

	if (*filename == '.') {
		phar_archive_data **pphar;
		int try_len;

		if (SUCCESS != (zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **) &pphar))) {
			efree(arch);
			efree(entry);
			goto doit;
		}
		efree(entry);
		try_len = filename_len;
		test = phar_fix_filepath(estrndup(filename, filename_len), &try_len, 1 TSRMLS_CC);
		if (zend_hash_exists(&((*pphar)->manifest), test + 1, try_len - 1)) {
			spprintf(&ret, 0, "phar://%s%s", arch, test);
			efree(arch);
			efree(test);
			return ret;
		}
		efree(test);
	}
	efree(arch);

doit:
	if (*filename == '.' ||
	    IS_ABSOLUTE_PATH(filename, filename_len) ||
	    !path ||
	    !*path) {
		if (tsrm_realpath(filename, resolved_path TSRMLS_CC)) {
			return estrdup(resolved_path);
		} else {
			return NULL;
		}
	}
	/* test for stream wrappers and return */
	for (p = filename; p - filename < filename_len && (isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'); ++p, ++n);
	if (n < filename_len - 3 && (*p == ':') && (!strncmp("//", p+1, 2) || ( filename_len > 4 && !memcmp("data", filename, 4)))) {
		/* found stream wrapper, this is an absolute path until stream wrappers implement realpath */
		return estrndup(filename, filename_len);
	}

	ptr = (char *) path;
	while (ptr && *ptr) {
		int len, is_stream_wrapper = 0, maybe_stream = 1;

		end = strchr(ptr, DEFAULT_DIR_SEPARATOR);
#ifndef PHP_WIN32
		/* search for stream wrapper */
		if (end - ptr  <= 1) {
			maybe_stream = 0;
			goto not_stream;
		}
		for (p = ptr, n = 0; p < end && (isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'); ++p, ++n);

		if (n == end - ptr && *p && !strncmp("//", p+1, 2)) {
			is_stream_wrapper = 1;
			/* seek to real end of include_path portion */
			end = strchr(end + 1, DEFAULT_DIR_SEPARATOR);
		} else {
			maybe_stream = 0;
		}
not_stream:
#endif
		if (end) {
			if ((end-ptr) + 1 + filename_len + 1 >= MAXPATHLEN) {
				ptr = end + 1;
				continue;
			}
			memcpy(trypath, ptr, end-ptr);
			len = end-ptr;
			trypath[end-ptr] = '/';
			memcpy(trypath+(end-ptr)+1, filename, filename_len+1);
			ptr = end+1;
		} else {
			len = strlen(ptr);

			if (len + 1 + filename_len + 1 >= MAXPATHLEN) {
				break;
			}
			memcpy(trypath, ptr, len);
			trypath[len] = '/';
			memcpy(trypath+len+1, filename, filename_len+1);
			ptr = NULL;
		}

		if (!is_stream_wrapper && maybe_stream) {
			/* search for stream wrapper */
			for (p = trypath, n = 0; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; ++p, ++n);
		}

		if (is_stream_wrapper || (n < len - 3 && (*p == ':') && (n > 1) && (!strncmp("//", p+1, 2) || !memcmp("data", trypath, 4)))) {
			char *actual;

			wrapper = php_stream_locate_url_wrapper(trypath, &actual, STREAM_OPEN_FOR_INCLUDE TSRMLS_CC);
			if (wrapper == &php_plain_files_wrapper) {
				strncpy(trypath, actual, MAXPATHLEN);
			} else if (!wrapper) {
				/* if wrapper is NULL, there was a mal-formed include_path stream wrapper, so skip this ptr */
				continue;
			} else {
				if (wrapper->wops->url_stat) {
					php_stream_statbuf ssb;

					if (SUCCESS == wrapper->wops->url_stat(wrapper, trypath, 0, &ssb, NULL TSRMLS_CC)) {
						if (wrapper == &php_stream_phar_wrapper) {
							char *arch, *entry;
							int arch_len, entry_len, ret_len;
					
							ret_len = strlen(trypath);
							/* found phar:// */

							if (SUCCESS != phar_split_fname(trypath, ret_len, &arch, &arch_len, &entry, &entry_len, 0, 0 TSRMLS_CC)) {
								return estrndup(trypath, ret_len);
							}
							zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **) &pphar);
							efree(arch);
							efree(entry);
							return estrndup(trypath, ret_len);
						}
						return estrdup(trypath);
					}
				}
				continue;
			}
		}

		if (tsrm_realpath(trypath, resolved_path TSRMLS_CC)) {
			return estrdup(resolved_path);
		}
	} /* end provided path */

	/* check in calling scripts' current working directory as a fall back case
	 */
	if (zend_is_executing(TSRMLS_C)) {
		char *exec_fname = zend_get_executed_filename(TSRMLS_C);
		int exec_fname_length = strlen(exec_fname);
		const char *p;
		int n = 0;

		while ((--exec_fname_length >= 0) && !IS_SLASH(exec_fname[exec_fname_length]));
		if (exec_fname && exec_fname[0] != '[' &&
		    exec_fname_length > 0 &&
		    exec_fname_length + 1 + filename_len + 1 < MAXPATHLEN) {
			memcpy(trypath, exec_fname, exec_fname_length + 1);
			memcpy(trypath+exec_fname_length + 1, filename, filename_len+1);

			/* search for stream wrapper */
			for (p = trypath; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; ++p, ++n);
			if (n < exec_fname_length - 3 && (*p == ':') && (n > 1) && (!strncmp("//", p+1, 2) || !memcmp("data", trypath, 4))) {
				char *actual;
	
				wrapper = php_stream_locate_url_wrapper(trypath, &actual, STREAM_OPEN_FOR_INCLUDE TSRMLS_CC);
				if (wrapper == &php_plain_files_wrapper) {
					/* this should never technically happen, but we'll leave it here for completeness */
					strncpy(trypath, actual, MAXPATHLEN);
				} else if (!wrapper) {
					/* if wrapper is NULL, there was a mal-formed include_path stream wrapper
					   this also should be impossible */
					return NULL;
				} else {
					return estrdup(trypath);
				}
			}
			if (tsrm_realpath(trypath, resolved_path TSRMLS_CC)) {
				return estrdup(resolved_path);
			}
		}
	}

	return NULL;
#endif /* PHP 5.2 */
}
/* }}} */

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
	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return FAILURE;
	}
	if (for_write && PHAR_G(readonly) && !phar->is_data) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, disabled by ini setting", path, fname);
		}
		return FAILURE;
	}
	if (!path_len) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"\" in phar \"%s\" cannot be empty", fname);
		}
		return FAILURE;
	}
	if (allow_dir) {
		if ((entry = phar_get_entry_info_dir(phar, path, path_len, allow_dir, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error TSRMLS_CC)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	} else {
		if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error TSRMLS_CC)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
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
		(*ret)->fp = NULL;
		(*ret)->phar = phar;
		(*ret)->for_write = for_write;
		(*ret)->internal_file = entry;
		(*ret)->is_zip = entry->is_zip;
		(*ret)->is_tar = entry->is_tar;
		++(entry->phar->refcount);
		++(entry->fp_refcount);
		return SUCCESS;
	}
	if (entry->fp_type == PHAR_MOD) {
		if (for_trunc) {
			if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
				return FAILURE;
			}
		} else if (for_append) {
			phar_seek_efp(entry, 0, SEEK_END, 0, 0 TSRMLS_CC);
		}
	} else {
		if (entry->link) {
			efree(entry->link);
			entry->link = NULL;
			entry->tar_type = (entry->tar_type ? TAR_FILE : 0);
		}
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
			if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
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
	(*ret)->fp = phar_get_efp(entry, 1 TSRMLS_CC);
	(*ret)->zero = entry->offset;
	++(entry->fp_refcount);
	++(entry->phar->refcount);
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
	char is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len > 0 && path != NULL) ? path[path_len - 1] == '/' : 0;

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

	if (allow_dir == 2) {
		etemp.is_dir = 1;
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_DIR;
		if (is_dir) {
			etemp.filename_len--; /* strip trailing / */
			path_len--;
		}
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

	++(phar->refcount);
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
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(source, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}
	if (dest->link) {
		efree(dest->link);
		dest->link = NULL;
		dest->tar_type = (dest->tar_type ? TAR_FILE : 0);
	}
	dest->fp_type = PHAR_MOD;
	dest->offset = 0;
	dest->is_modified = 1;
	dest->fp = php_stream_fopen_tmpfile();

	phar_seek_efp(source, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	link = phar_get_link_source(source TSRMLS_CC);
	if (!link) {
		link = source;
	}
	if (link->uncompressed_filesize != php_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), dest->fp, link->uncompressed_filesize)) {
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
int phar_open_entry_fp(phar_entry_info *entry, char **error, int follow_links TSRMLS_DC) 
{
	php_stream_filter *filter;
	phar_archive_data *phar = entry->phar;
	char *filtername;
	off_t loc;

	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry TSRMLS_CC);

		if (link_entry && link_entry != entry) {
			return phar_open_entry_fp(link_entry, error, 1 TSRMLS_CC);
		}
	}
	if (entry->fp_type == PHAR_TMP) {
		if (!entry->fp) {
			entry->fp = php_stream_open_wrapper(entry->tmp, "rb", STREAM_MUST_SEEK|0, NULL);
		}
		return SUCCESS;
	}
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
	if ((entry->old_flags && !(entry->old_flags & PHAR_ENT_COMPRESSION_MASK)) || !(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
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
				if (error) {
					spprintf(error, 0, "phar error: file \"%s\" cannot be opened for writing, no truncate support", phar->fname);
				}
				return FAILURE;
			}
		} else if (php_stream_is(entry->fp, PHP_STREAM_IS_STDIO)) {
			php_stream_truncate_set_size(entry->fp, 0);
		} else {
			if (error) {
				spprintf(error, 0, "phar error: file \"%s\" cannot be opened for writing, no truncate support", phar->fname);
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
	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->tar_type ? TAR_FILE : 0);
	}
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
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		return SUCCESS;
	}

	fp = php_stream_fopen_tmpfile();
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	link = phar_get_link_source(entry TSRMLS_CC);
	if (!link) {
		link = entry;
	}
	if (link->uncompressed_filesize != php_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), fp, link->uncompressed_filesize)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot separate entry file \"%s\" contents in phar archive \"%s\" for write access", entry->filename, entry->phar->fname);
		}
		return FAILURE;
	}

	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->tar_type ? TAR_FILE : 0);
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
	if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
		return NULL;
	}
	if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC)) {
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
	char is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = path_len && (path[path_len - 1] == '/');

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

	if (!phar->manifest.arBuckets) {
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
		if (!entry->is_dir && dir == 2) {
			/* user requested a directory, we must return one */
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
			}
			return NULL;
		}
		return entry;
	} else if (phar->mounted_dirs.arBuckets && zend_hash_num_elements(&phar->mounted_dirs)) {
		char *key;
		ulong unused;
		uint keylen;

		zend_hash_internal_pointer_reset(&phar->mounted_dirs);
		while (FAILURE != zend_hash_has_more_elements(&phar->mounted_dirs)) {
			if (HASH_KEY_NON_EXISTANT == zend_hash_get_current_key_ex(&phar->mounted_dirs, &key, &keylen, &unused, 0, NULL)) {
				break;
			}
			if ((int)keylen >= path_len || strncmp(key, path, keylen)) {
				continue;
			} else {
				char *test;
				int test_len;
				phar_entry_info *entry;
				php_stream_statbuf ssb;

				if (SUCCESS != zend_hash_find(&phar->manifest, key, keylen, (void **) &entry)) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" could not be retrieved from manifest", key);
					}
					return NULL;
				}
				if (!entry->tmp || !entry->is_mounted) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" is not properly initialized as a mounted path", key);
					}
					return NULL;
				}
				test_len = spprintf(&test, MAXPATHLEN, "%s%s", entry->tmp, path + keylen);
				if (SUCCESS != php_stream_stat_path(test, &ssb)) {
					efree(test);
					return NULL;
				}
				if (ssb.sb.st_mode & S_IFDIR && !dir) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
					}
					return NULL;
				}
				if ((ssb.sb.st_mode & S_IFDIR) == 0 && dir) {
					efree(test);
					/* user requested a directory, we must return one */
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
					}
					return NULL;
				}
				/* mount the file just in time */
				if (SUCCESS != phar_mount_entry(phar, test, test_len, path, path_len TSRMLS_CC)) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be mounted", path, test);
					}
					return NULL;
				}
				efree(test);
				if (SUCCESS != zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be retrieved after being mounted", path, test);
					}
					return NULL;
				}
				return entry;
			}
		}
	}
	if (dir) {
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
				entry->phar = phar;
				return entry;
			}
		}
	}
	return NULL;
}
/* }}} */
