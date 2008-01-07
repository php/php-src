/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2007 The PHP Group                                |
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

#define PHAR_MAIN
#include "phar_internal.h"
#include "SAPI.h"

ZEND_DECLARE_MODULE_GLOBALS(phar)

#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 50400 || PHP_VERSION_ID >= 60000)

static int _php_stream_unlink(char *url, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream_wrapper *wrapper = php_stream_locate_url_wrapper(url, NULL, options TSRMLS_CC);

	if (!wrapper || !wrapper->wops) {
		return 0;
	}

	if (!wrapper->wops->unlink) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
		return 0;
	}
	return wrapper->wops->unlink(wrapper, url, ENFORCE_SAFE_MODE | REPORT_ERRORS, context TSRMLS_CC);
}

#define php_stream_unlink(url, options, context) _php_stream_unlink((url), (options), (context) TSRMLS_CC)

#endif

/* if the original value is 0 (disabled), then allow setting/unsetting at will
   otherwise, only allow 1 (enabled), and error on disabling */
ZEND_INI_MH(phar_ini_modify_handler) /* {{{ */
{
	zend_bool old, ini;

	if (entry->name_length == 14) {
		old = PHAR_G(readonly_orig);
	} else {
		old = PHAR_G(require_hash_orig);
	} 

	if (new_value_length == 2 && !strcasecmp("on", new_value)) {
		ini = (zend_bool) 1;
	}
	else if (new_value_length == 3 && !strcasecmp("yes", new_value)) {
		ini = (zend_bool) 1;
	}
	else if (new_value_length == 4 && !strcasecmp("true", new_value)) {
		ini = (zend_bool) 1;
	}
	else {
		ini = (zend_bool) atoi(new_value);
	}

	/* do not allow unsetting in runtime */
	if (stage == ZEND_INI_STAGE_STARTUP) {
		if (entry->name_length == 14) {
			PHAR_G(readonly_orig) = ini; 
		} else {
			PHAR_G(require_hash_orig) = ini;
		} 
	} else if (old && !ini) {
		return FAILURE;
	}

	if (entry->name_length == 14) {
		PHAR_G(readonly) = ini; 
	} else {
		PHAR_G(require_hash) = ini;
	} 
	return SUCCESS;
}
/* }}}*/

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

static void phar_split_extract_list(TSRMLS_D)
{
	char *tmp = estrdup(PHAR_GLOBALS->extract_list);
	char *key;
	char *lasts;
	char *q;
	int keylen;

	zend_hash_clean(&(PHAR_GLOBALS->phar_plain_map));

	for (key = php_strtok_r(tmp, ",", &lasts);
			key;
			key = php_strtok_r(NULL, ",", &lasts))
	{
		char *val = strchr(key, '=');

		if (val) {	
			*val++ = '\0';
			for (q = key; *q; q++) {
				*q = tolower(*q);
			}
			keylen = q - key + 1;
			zend_hash_add(&(PHAR_GLOBALS->phar_plain_map), key, keylen, val, strlen(val)+1, NULL);
		}
	}
	efree(tmp);
}
/* }}} */

ZEND_INI_MH(phar_ini_extract_list) /* {{{ */
{
	PHAR_G(extract_list) = new_value;

	if (stage == ZEND_INI_STAGE_RUNTIME) {
		phar_split_extract_list(TSRMLS_C);
	}

	return SUCCESS;
}
/* }}} */

ZEND_INI_DISP(phar_ini_extract_list_disp) /*void name(zend_ini_entry *ini_entry, int type) {{{ */
{
	char *value;

	if (type==ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
	} else {
		value = NULL;
	}

	if (value) {
		char *tmp = strdup(value);
		char *key;
		char *lasts;
		char *q;
	
		if (!sapi_module.phpinfo_as_text) {
			php_printf("<ul>");
		}
		for (key = php_strtok_r(tmp, ",", &lasts);
				key;
				key = php_strtok_r(NULL, ",", &lasts))
		{
			char *val = strchr(key, '=');
	
			if (val) {	
				*val++ = '\0';
				for (q = key; *q; q++) {
					*q = tolower(*q);
				}
				if (sapi_module.phpinfo_as_text) {
					php_printf("%s => %s", key, val);
				} else {
					php_printf("<li>%s => %s</li>", key, val);
				}
			}
		}
		if (!sapi_module.phpinfo_as_text) {
			php_printf("</ul>");
		}
		free(tmp);
	}
}
/* }}} */

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN( "phar.readonly",     "1", PHP_INI_ALL, phar_ini_modify_handler, readonly,     zend_phar_globals, phar_globals)
	STD_PHP_INI_BOOLEAN( "phar.require_hash", "1", PHP_INI_ALL, phar_ini_modify_handler, require_hash, zend_phar_globals, phar_globals)
	STD_PHP_INI_ENTRY_EX("phar.extract_list", "",  PHP_INI_ALL, phar_ini_extract_list,   extract_list, zend_phar_globals, phar_globals, phar_ini_extract_list_disp)
PHP_INI_END()

/**
 * When all uses of a phar have been concluded, this frees the manifest
 * and the phar slot
 */
static void phar_destroy_phar_data(phar_archive_data *data TSRMLS_DC) /* {{{ */
{
#if HAVE_PHAR_ZIP
	if (data->zip) {
		_zip_free(data->zip);
		data->zip = 0;
	}
#endif
	if (data->alias && data->alias != data->fname) {
		efree(data->alias);
		data->alias = NULL;
	}
	if (data->fname) {
		efree(data->fname);
	}
	if (data->signature) {
		efree(data->signature);
	}
	if (data->manifest.arBuckets) {
		zend_hash_destroy(&data->manifest);
		data->manifest.arBuckets = NULL;
	}
	if (data->metadata) {
		zval_ptr_dtor(&data->metadata);
		data->metadata = 0;
	}
	if (data->fp) {
		php_stream_close(data->fp);
		data->fp = 0;
	}
	efree(data);
}
/* }}}*/

/**
 * Delete refcount and destruct if needed. On destruct return 1 else 0.
 */
int phar_archive_delref(phar_archive_data *phar TSRMLS_DC) /* {{{ */
{
	if (--phar->refcount < 0) {
		if (PHAR_GLOBALS->request_done
		|| zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), phar->fname, phar->fname_len) != SUCCESS) {
			phar_destroy_phar_data(phar TSRMLS_CC);
		}
		return 1;
	}
	return 0;
}
/* }}}*/

/**
 * Destroy phar's in shutdown, here we don't care about aliases
 */
static void destroy_phar_data_only(void *pDest) /* {{{ */
{
	phar_archive_data *phar_data = *(phar_archive_data **) pDest;
	TSRMLS_FETCH();

	if (EG(exception) || --phar_data->refcount < 0) {
		phar_destroy_phar_data(phar_data TSRMLS_CC);
	}
}
/* }}}*/

/**
 * Delete aliases to phar's that got kicked out of the global table
 */
static int phar_unalias_apply(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	return *(void**)pDest == argument ? ZEND_HASH_APPLY_REMOVE : ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/**
 * Filename map destructor
 */
static void destroy_phar_data(void *pDest) /* {{{ */
{
	phar_archive_data *phar_data = *(phar_archive_data **) pDest;
	TSRMLS_FETCH();

	if (PHAR_GLOBALS->request_ends) {
		destroy_phar_data_only(pDest);
		return;
	}
	zend_hash_apply_with_argument(&(PHAR_GLOBALS->phar_alias_map), phar_unalias_apply, phar_data TSRMLS_CC);
	if (--phar_data->refcount < 0) {
		phar_destroy_phar_data(phar_data TSRMLS_CC);
	}
}
/* }}}*/

/**
 * destructor for the manifest hash, frees each file's entry
 */
void destroy_phar_manifest(void *pDest) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;
	TSRMLS_FETCH();

	if (entry->cfp) {
		php_stream_close(entry->cfp);
		entry->cfp = 0;
	}
	if (entry->fp) {
		if (entry->fp != entry->phar->fp) {
			php_stream_close(entry->fp);
		}
		entry->fp = 0;
	}
	if (entry->metadata) {
		zval_ptr_dtor(&entry->metadata);
		entry->metadata = 0;
	}
	if (entry->metadata_str.c) {
		smart_str_free(&entry->metadata_str);
		entry->metadata_str.c = 0;
	}
	efree(entry->filename);
#if HAVE_PHAR_ZIP
	if (entry->zip) {
		zip_fclose(entry->zip);
		entry->zip = 0;
	}
#endif
	if (entry->link) {
		efree(entry->link);
		entry->link = 0;
	}
}
/* }}} */

/**
 * Looks up a phar archive in the filename map, connecting it to the alias
 * (if any) or returns null
 */
static int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error TSRMLS_DC) /* {{{ */
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
 * retrieve information on a file contained within a phar, or null if it ain't there
 */
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error TSRMLS_DC) /* {{{ */
{
	return phar_get_entry_info_dir(phar, path, path_len, 0, error TSRMLS_CC);
}
/* }}} */
/**
 * retrieve information on a file or directory contained within a phar, or null if none found
 */
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, int path_len, char dir, char **error TSRMLS_DC) /* {{{ */
{
	const char *pcr_error;
	phar_entry_info *entry;

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
	if (SUCCESS == zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
#if HAVE_PHAR_ZIP
		if (phar->is_zip) {
			/* construct phar_entry_info JIT */
			if (path[path_len - 1] == '/') {
				if (!dir) {
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
					}
					return NULL;
				}
		
				entry->is_dir = 1;
			}
		}
#endif
		if (entry->is_deleted) {
			/* entry is deleted, but has not been flushed to disk yet */
			return NULL;
		}
		return entry;
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
				entry->is_dir = 1;
				/* this next line tells PharFileInfo->__destruct() to efree the filename */
				entry->is_zip = entry->is_tar = 0;
				entry->filename = (char *) estrndup(path, path_len + 1);
				entry->filename_len = path_len;
				return entry;
			}
		}
	}
	return NULL;
}
/* }}} */

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

/* this is only called for non-zip-based phars */
static int phar_open_entry_file(phar_archive_data *phar, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
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
	return SUCCESS;
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
int phar_get_entry_data(phar_entry_data **ret, char *fname, int fname_len, char *path, int path_len, char *mode, char **error TSRMLS_DC) /* {{{ */
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
			spprintf(error, 0, "phar error: file \"%s\" cannot opened for writing, disabled by ini setting", fname);
		}
		return FAILURE;
	}
	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return FAILURE;
	}
	if (!path_len) {
		if (error) {
			spprintf(error, 0, "phar error: file \"\" cannot be empty");
		}
		return FAILURE;
	}
	if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) ? NULL : error TSRMLS_CC)) == NULL) {
		if (for_create && !PHAR_G(readonly)) {
			return SUCCESS;
		}
		return FAILURE;
	}
	if (entry->is_modified && !for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" cannot opened for reading, writable file pointers are open", fname);
		}
		return FAILURE;
	}
	if (entry->fp_refcount && for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" cannot opened for writing, readable file pointers are open", fname);
		}
		return FAILURE;
	}
	if (entry->is_deleted) {
		if (!for_create) {
			return FAILURE;
		}
#if HAVE_PHAR_ZIP
		if (entry->is_zip) {
			/* for new files, start with an empty string */
			struct zip_source *s = zip_source_buffer(entry->phar->zip, (void *)"", 0, 0);
			if (-1 == zip_replace(entry->phar->zip, entry->index, s)) {
				if (error) {
					spprintf(error, 4096, "phar error: zip-based phar \"%s\" entry \"%s\" error \"%s\"", entry->phar->fname, fname, zip_strerror(phar->zip));
				}
				zip_error_clear(phar->zip);
				return FAILURE;
			}
		}
#endif
		entry->is_deleted = 0;
	}
	*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	(*ret)->position = 0;
	(*ret)->zero = 0;
	(*ret)->phar = phar;
	(*ret)->for_write = for_write;
	(*ret)->internal_file = entry;
	(*ret)->is_zip = entry->is_zip;
	(*ret)->is_tar = entry->is_tar;
	if (entry->fp) {
		/* make a copy */
		if (for_trunc) {
			if (entry->fp == phar->fp) {
				/* duplicate entry if we are writing and are recycling the phar fp */
				if (FAILURE == phar_open_entry_file(phar, entry, error TSRMLS_CC)) {
					return FAILURE;
				}
				(*ret)->fp = entry->fp;
			}
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
						spprintf(error, 0, "phar error: file \"%s\" cannot opened for writing, no truncate support", fname);
					}
					return FAILURE;
				}
			} else if (php_stream_is(entry->fp, PHP_STREAM_IS_STDIO)) {
				php_stream_truncate_set_size(entry->fp, 0);
			} else {
				efree(*ret);
				*ret = NULL;
				if (error) {
					spprintf(error, 0, "phar error: file \"%s\" cannot opened for writing, no truncate support", fname);
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
		} else if (for_append) {
			php_stream_seek(entry->fp, 0, SEEK_END);
		}
		(*ret)->fp = entry->fp;
		if (entry->fp == phar->fp) {
			(*ret)->zero = entry->offset_within_phar + phar->internal_file_start;
		}
	} else {
		(*ret)->fp = 0;
		if (for_write) {
			if (FAILURE == phar_open_entry_file(phar, entry, error TSRMLS_CC)) {
				return FAILURE;
			}
			(*ret)->fp = entry->fp;
		}
	}
	entry->fp_refcount++;
	entry->phar->refcount++;
	return SUCCESS;
}
/* }}} */

int phar_entry_delref(phar_entry_data *idata TSRMLS_DC) /* {{{ */
{
	int ret = 0;

	if (idata->internal_file) {
		if (--idata->internal_file->fp_refcount < 0) {
			idata->internal_file->fp_refcount = 0;
		}
		if (idata->fp && idata->fp != idata->internal_file->fp) {
			php_stream_close(idata->fp);
		}
	}
	phar_archive_delref(idata->phar TSRMLS_CC);
	efree(idata);
	return ret;
}
/* }}} */

/**
 * Removes an entry, either by actually removing it or by marking it.
 */
void phar_entry_remove(phar_entry_data *idata, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;

	phar = idata->phar;
	if (idata->internal_file->fp_refcount < 2) {
		if (idata->fp && idata->fp != idata->phar->fp && idata->fp != idata->internal_file->fp) {
			php_stream_close(idata->fp);
		}
#if HAVE_PHAR_ZIP
		if (idata->internal_file->is_zip) {
			if (idata->internal_file->zip) {
				zip_fclose(idata->internal_file->zip);
				idata->internal_file->zip = 0;
			}
			zip_delete(phar->zip, idata->internal_file->index);
		}
#endif
		zend_hash_del(&idata->phar->manifest, idata->internal_file->filename, idata->internal_file->filename_len);
		idata->phar->refcount--;
		efree(idata);
	} else {
		idata->internal_file->is_deleted = 1;
		phar_entry_delref(idata TSRMLS_CC);
	}
	if (!phar->donotflush) {
		phar_flush(phar, 0, 0, error TSRMLS_CC);
	}
}
/* }}} */

/**
 * Create a new dummy file slot within a writeable phar for a newly created file
 */
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, char *mode, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, etemp;
	phar_entry_data *ret;
	const char *pcr_error;

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return NULL;
	}

	if (FAILURE == phar_get_entry_data(&ret, fname, fname_len, path, path_len, mode, error TSRMLS_CC)) {
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
	etemp.fp = php_stream_fopen_tmpfile();
	if (!etemp.fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		return NULL;
	}
	etemp.fp_refcount = 1;
	etemp.is_modified = 1;
	etemp.timestamp = time(0);
	etemp.offset_within_phar = -1;
	etemp.is_crc_checked = 1;
	etemp.flags = PHAR_ENT_PERM_DEF_FILE;
	etemp.old_flags = PHAR_ENT_PERM_DEF_FILE;
	etemp.phar = phar;
#if HAVE_PHAR_ZIP
	if (phar->is_zip) {
		int zindex;
		etemp.is_zip = 1;
		/* prevent attempts to check the CRC */
		etemp.is_crc_checked = 1;
		/* for new files, start with an empty string */
		struct zip_source *s = zip_source_buffer(phar->zip, (void *)"", 0, 0);
		if (-1 == (zindex = zip_add(phar->zip, path, s))) {
			if (error) {
				spprintf(error, 4096, "phar error: zip-based phar \"%s\" entry \"%s\" error \"%s\"", path, phar->fname, zip_strerror(phar->zip));
			}
			zip_error_clear(phar->zip);
			return NULL;
		}
		etemp.index = zindex;
		etemp.filename = (char *) zip_get_name(phar->zip, zindex, 0);
		if (NULL == etemp.filename) {
			if (error) {
				spprintf(error, 4096, "phar error: zip-based phar \"%s\" entry \"%s\" error \"%s\"", path, phar->fname, zip_strerror(phar->zip));
			}
			zip_error_clear(phar->zip);
			return NULL;
		}
		etemp.filename = estrndup(etemp.filename, strlen(etemp.filename));
	} else {
		etemp.is_tar = phar->is_tar;
		etemp.tar_type = '0';
		etemp.filename = estrndup(path, path_len);
	}
#else
	etemp.filename = estrndup(path, path_len);
#endif
	zend_hash_add(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info), NULL);
	/* retrieve the phar manifest copy */
	entry = phar_get_entry_info(phar, path, path_len, error TSRMLS_CC);
	
	if (!entry) {
		return NULL;
	}

	phar->refcount++;
	ret->phar = phar;
	ret->fp = entry->fp;
	ret->position = 0;
	ret->for_write = 1;
	ret->is_zip = entry->is_zip;
	ret->is_tar = entry->is_tar;
	ret->internal_file = entry;
	return ret;
}
/* }}} */

#define MAPPHAR_ALLOC_FAIL(msg) \
	php_stream_close(fp);\
	if (error) {\
		spprintf(error, 0, msg, fname);\
	}\
	return FAILURE;

#define MAPPHAR_FAIL(msg) \
	efree(savebuf);\
	if (mydata) {\
		phar_destroy_phar_data(mydata TSRMLS_CC);\
	}\
	if (signature) {\
		efree(signature);\
	}\
	MAPPHAR_ALLOC_FAIL(msg)

#ifdef WORDS_BIGENDIAN
# define PHAR_GET_32(buffer, var) \
	var = ((((unsigned char*)(buffer))[3]) << 24) \
		| ((((unsigned char*)(buffer))[2]) << 16) \
		| ((((unsigned char*)(buffer))[1]) <<  8) \
		| (((unsigned char*)(buffer))[0]); \
	(buffer) += 4
# define PHAR_GET_16(buffer, var) \
	var = ((((unsigned char*)(buffer))[1]) <<  8) \
		| (((unsigned char*)(buffer))[0]); \
	(buffer) += 2
#else
# define PHAR_GET_32(buffer, var) \
	var = *(php_uint32*)(buffer); \
	buffer += 4
# define PHAR_GET_16(buffer, var) \
	var = *(php_uint16*)(buffer); \
	buffer += 2
#endif

/**
 * Open an already loaded phar
 */
int phar_open_loaded(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
#ifdef PHP_WIN32
	char *unixfname;
#endif

	if (error) {
		*error = NULL;
	}
#ifdef PHP_WIN32
	unixfname = estrndup(fname, fname_len);
	phar_unixify_path_separators(unixfname, fname_len);
	if (SUCCESS == phar_get_archive(&phar, unixfname, fname_len, alias, alias_len, error TSRMLS_CC)
		&& ((alias && fname_len == phar->fname_len
		&& !strncmp(unixfname, phar->fname, fname_len)) || !alias)
	) {
		efree(unixfname);
#else
	if (SUCCESS == phar_get_archive(&phar, fname, fname_len, alias, alias_len, error TSRMLS_CC)
		&& ((alias && fname_len == phar->fname_len
		&& !strncmp(fname, phar->fname, fname_len)) || !alias)
	) {
#endif
		/* logic above is as follows:
		   If an explicit alias was requested, ensure the filename passed in
		   matches the phar's filename.
		   If no alias was passed in, then it can match either and be valid
		 */
		if (pphar) {
			*pphar = phar;
		}
		return SUCCESS;
	} else {
#ifdef PHP_WIN32
		efree(unixfname);
#endif
		if (pphar) {
			*pphar = NULL;
		}
		if (phar && alias && (options & REPORT_ERRORS)) {
			if (error) {
				spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, phar->fname, fname);
			}
		}
		return FAILURE;
	}
}
/* }}}*/

/**
 * Parse out metadata from the manifest for a single file
 *
 * Meta-data is in this format:
 * [len32][data...]
 * 
 * data is the serialized zval
 */
static int phar_parse_metadata(char **buffer, zval **metadata, int is_zip TSRMLS_DC) /* {{{ */
{
	const unsigned char *p;
	php_uint32 buf_len;
	php_unserialize_data_t var_hash;

	if (!is_zip) {
		PHAR_GET_32(*buffer, buf_len);
	} else {
		buf_len = is_zip;
	}
	
	if (buf_len) {
		ALLOC_INIT_ZVAL(*metadata);
		p = (const unsigned char*) *buffer;
		PHP_VAR_UNSERIALIZE_INIT(var_hash);
		if (!php_var_unserialize(metadata, &p, p + buf_len,  &var_hash TSRMLS_CC)) {
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			zval_ptr_dtor(metadata);
			*metadata = NULL;
			return FAILURE;
		}
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	} else {
		*metadata = NULL;
	}
	*buffer += buf_len;
	return SUCCESS;
}
/* }}}*/

static const char hexChars[] = "0123456789ABCDEF";

static int phar_hex_str(const char *digest, size_t digest_len, char ** signature)
{
	int pos = -1;
	size_t len;

	*signature = (char*)safe_emalloc(digest_len, 2, 1);

	for(len = 0; len < digest_len; ++len) {
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] >> 4];
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] & 0x0F];
	}
	(*signature)[++pos] = '\0';
	return pos;
}

/**
 * Does not check for a previously opened phar in the cache.
 *
 * Parse a new one and add it to the cache, returning either SUCCESS or 
 * FAILURE, and setting pphar to the pointer to the manifest entry
 * 
 * This is used by phar_open_fp to process a zip-based phar, but can be called
 * directly.
 */
int phar_open_zipfile(char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
#if HAVE_PHAR_ZIP
	struct zip *zip;
	int ziperror, i, phar_alias_index, register_alias, metadata_len;
	phar_entry_info entry = {0};
	struct zip_stat zs;
	char tmp_buf[1024], *metadata;

	if (error) {
		*error = NULL;
	}

	zip = zip_open(fname, 0, &ziperror);
	if (!zip) {
		if (error) {
			/* now for the stupid hoops libzip forces... */
			char *tmp;
			int tmp_len;
			tmp_len = zip_error_to_str(NULL, 0, ziperror, ziperror);
			if (!(tmp = emalloc((tmp_len + 1) * sizeof(char)))) {
				spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\"", fname);
			} else {
				if (!zip_error_to_str(tmp, tmp_len + 1, ziperror, ziperror)) {
					spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\"", fname);
				} else {
					spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\": %s", fname, tmp);
					efree(tmp);
				}
			}
		}
		return FAILURE;
	}
	phar_archive_data *mydata = NULL;
	mydata = ecalloc(sizeof(phar_archive_data), 1);
	mydata->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	mydata->fname_len = fname_len;
	if (-1 != (phar_alias_index = zip_name_locate(zip, "./phar/alias.txt", 0))) {
		struct zip_file *zf = zip_fopen_index(zip, phar_alias_index, 0);
		int tmp_len;

		tmp_len = zip_fread(zf, tmp_buf, 1024);
		zip_fclose(zf);
		/* if the alias is stored we enforce it (implicit overrides explicit) */
		if (tmp_len != -1 && alias && alias_len && (alias_len != tmp_len || strncmp(alias, tmp_buf, tmp_len)))
		{
			if (error) {
				spprintf(error, 0, "cannot load phar \"%s\" with implicit alias \"%s\" under different alias \"%s\"", fname, tmp_buf, alias);
			}
			return FAILURE;
		}
		if (tmp_len != -1) {
			/* use implicit alias */
			alias = tmp_buf;
			alias_len = tmp_len;
			register_alias = 0;
		}
	} else {
		register_alias = alias ? 1 : 0;
	}
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	mydata->is_zip = 1;
	mydata->zip = zip;

	phar_request_initialize(TSRMLS_C);

	/* read in phar metadata (zip file comment) */
	metadata = (char *) zip_get_archive_comment(zip, &metadata_len, 0);
	if (metadata) {
		if (phar_parse_metadata(&metadata, &mydata->metadata, metadata_len TSRMLS_CC) == FAILURE) {
			/* if not valid serialized data, it is a regular string */
			ALLOC_INIT_ZVAL(mydata->metadata);
			Z_STRVAL_P(mydata->metadata) = estrndup(metadata, metadata_len);
			Z_STRLEN_P(mydata->metadata) = metadata_len;
			Z_TYPE_P(mydata->metadata) = IS_STRING;
		}
	} else {
		mydata->metadata = NULL;
	}
	/* set up our manifest */
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	entry.phar = mydata;
	entry.is_zip = 1;
	/* prevent CRC checking */
	entry.is_crc_checked = 1;
	for (i = 0; i < zip_get_num_files(zip); i++) {
		char *name;
		name = (char *) zip_get_name(zip, i, 0);
		if (name) {
			/* get file stat */
			if (-1 != zip_stat_index(zip, i, 0, &zs)) {
				entry.compressed_filesize = zs.comp_size;
				entry.uncompressed_filesize = zs.size;
				entry.crc32 = zs.crc;
				entry.timestamp = (php_uint32) zs.mtime;
				entry.flags = 0;
				switch (zs.comp_method) {
					case ZIP_CM_DEFLATE :
						/* if we have zip, we have zlib decompression */
						entry.flags |= PHAR_ENT_COMPRESSED_GZ;
						break;
					case ZIP_CM_BZIP2 :
#if !HAVE_BZ2
						if (mydata->metadata) {
							zval_dtor(mydata->metadata);
						}
						efree(mydata->fname);
						if (mydata->alias) {
							efree(mydata->alias);
						}
						zip_close(zip);
						zend_hash_destroy(&(mydata->manifest));
						mydata->manifest.arBuckets = NULL;
						efree(mydata);
						if (error) {
							spprintf(error, 0, "bz2 extension is required for Bzip2 compressed zip-based .phar file \"%s\"", fname);
						}
						return FAILURE;
#else
						if (!phar_has_bz2) {
							if (mydata->metadata) {
								zval_dtor(mydata->metadata);
							}
							efree(mydata->fname);
							if (mydata->alias) {
								efree(mydata->alias);
							}
							zip_close(zip);
							zend_hash_destroy(&(mydata->manifest));
							mydata->manifest.arBuckets = NULL;
							efree(mydata);
							if (error) {
								spprintf(error, 0, "bz2 extension is required for gz compressed zip-based .phar file \"%s\"", fname);
							}
							return FAILURE;
						}
#endif
						entry.flags |= PHAR_ENT_COMPRESSED_BZ2;
						break;
				}
			}
			entry.index = i;
			entry.filename_len = strlen(name);
			entry.filename = estrndup(name, entry.filename_len);
			/* get file metadata */
			metadata = (char *) zip_get_file_comment(zip, i, &metadata_len, 0);
			if (metadata) {
				if (phar_parse_metadata(&metadata, &(entry.metadata), metadata_len TSRMLS_CC) == FAILURE) {
					/* if not valid serialized data, it is a regular string */
					ALLOC_INIT_ZVAL(entry.metadata);
					Z_STRVAL_P(entry.metadata) = estrndup(metadata, metadata_len);
					Z_STRLEN_P(entry.metadata) = metadata_len;
					Z_TYPE_P(entry.metadata) = IS_STRING;
				}
			} else {
				entry.metadata = NULL;
			}
			zend_hash_add(&mydata->manifest, name, strlen(name), (void *)&entry,sizeof(phar_entry_info), NULL);
		} 
	}
	/* ignore all errors in loading up manifest */
	zip_error_clear(zip);

	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), mydata->fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	if (register_alias) {
		mydata->is_explicit_alias = 1;
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	} else {
		mydata->is_explicit_alias = 0;
	}
	if (pphar) {
		*pphar = mydata;
	}
	return SUCCESS;
#else
	spprintf(error, 4096, "Error: Cannot open zip-based phar \"%s\"", fname);
	return FAILURE;
#endif
}
/* }}} */

/**
 * Does not check for a previously opened phar in the cache.
 *
 * Parse a new one and add it to the cache, returning either SUCCESS or 
 * FAILURE, and setting pphar to the pointer to the manifest entry
 * 
 * This is used by phar_open_filename to process the manifest, but can be called
 * directly.
 */
int phar_open_file(php_stream *fp, char *fname, int fname_len, char *alias, int alias_len, long halt_offset, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	char b32[4], *buffer, *endbuffer, *savebuf;
	phar_archive_data *mydata = NULL;
	phar_entry_info entry;
	php_uint32 manifest_len, manifest_count, manifest_flags, manifest_index, tmp_len, sig_flags;
	php_uint16 manifest_ver;
	long offset;
	int register_alias, sig_len;
	char *signature = NULL;

	if (pphar) {
		*pphar = NULL;
	}
	if (error) {
		*error = NULL;
	}

	/* check for ?>\n and increment accordingly */
	if (-1 == php_stream_seek(fp, halt_offset, SEEK_SET)) {
		MAPPHAR_ALLOC_FAIL("cannot seek to __HALT_COMPILER(); location in phar \"%s\"")
	}

	buffer = b32;
	if (3 != php_stream_read(fp, buffer, 3)) {
		MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest at stub end)")
	}
	if ((*buffer == ' ' || *buffer == '\n') && *(buffer + 1) == '?' && *(buffer + 2) == '>') {
		int nextchar;
		halt_offset += 3;
		if (EOF == (nextchar = php_stream_getc(fp))) {
			MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest at stub end)")
		}
		if ((char) nextchar == '\r') {
			/* if we have an \r we require an \n as well */
			if (EOF == (nextchar = php_stream_getc(fp)) || (char)nextchar != '\n') {
				MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest at stub end)")
			}
			halt_offset++;
		}
		if ((char) nextchar == '\n') {
			halt_offset++;
		}
	}
	/* make sure we are at the right location to read the manifest */
	if (-1 == php_stream_seek(fp, halt_offset, SEEK_SET)) {
		MAPPHAR_ALLOC_FAIL("cannot seek to __HALT_COMPILER(); location in phar \"%s\"")
	}

	/* read in manifest */
	buffer = b32;
	if (4 != php_stream_read(fp, buffer, 4)) {
		MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest at manifest length)")
	}
	PHAR_GET_32(buffer, manifest_len);
	if (manifest_len > 1048576) {
		/* prevent serious memory issues by limiting manifest to at most 1 MB in length */
		MAPPHAR_ALLOC_FAIL("manifest cannot be larger than 1 MB in phar \"%s\"")
	}
	buffer = (char *)emalloc(manifest_len);
	savebuf = buffer;
	endbuffer = buffer + manifest_len;
	if (manifest_len < 10 || manifest_len != php_stream_read(fp, buffer, manifest_len)) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest header)")
	}

	/* extract the number of entries */
	PHAR_GET_32(buffer, manifest_count);
	if (manifest_count == 0) {
		MAPPHAR_FAIL("in phar \"%s\", manifest claims to have zero entries.  Phars must have at least 1 entry");
	}

	/* extract API version, lowest nibble currently unused */
	manifest_ver = (((unsigned char)buffer[0]) <<  8)
	             + ((unsigned char)buffer[1]);
	buffer += 2;
	if ((manifest_ver & PHAR_API_VER_MASK) < PHAR_API_MIN_READ) {
		efree(savebuf);
		php_stream_close(fp);
		if (error) {
			spprintf(error, 0, "phar \"%s\" is API version %1.u.%1.u.%1.u, and cannot be processed", fname, manifest_ver >> 12, (manifest_ver >> 8) & 0xF, (manifest_ver >> 4) & 0x0F);
		}
		return FAILURE;
	}

	PHAR_GET_32(buffer, manifest_flags);

	manifest_flags &= ~PHAR_HDR_COMPRESSION_MASK; 

	/* The lowest nibble contains the phar wide flags. The compression flags can */
	/* be ignored on reading because it is being generated anyways. */
	if (manifest_flags & PHAR_HDR_SIGNATURE) {
		unsigned char buf[1024];
		int read_size, len;
		char sig_buf[8], *sig_ptr = sig_buf;
		off_t read_len;

		if (-1 == php_stream_seek(fp, -8, SEEK_END)
		|| (read_len = php_stream_tell(fp)) < 20
		|| 8 != php_stream_read(fp, sig_buf, 8) 
		|| memcmp(sig_buf+4, "GBMB", 4)) {
			efree(savebuf);
			php_stream_close(fp);
			if (error) {
				spprintf(error, 0, "phar \"%s\" has a broken signature", fname);
			}
			return FAILURE;
		}
		PHAR_GET_32(sig_ptr, sig_flags);
		switch(sig_flags) {
#if HAVE_HASH_EXT
		case PHAR_SIG_SHA512: {
			unsigned char digest[64], saved[64];
			PHP_SHA512_CTX  context;

			php_stream_rewind(fp);
			PHP_SHA512Init(&context);
			read_len -= sizeof(digest);
			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}
			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA512Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}
			PHP_SHA512Final(digest, &context);

			if (read_len > 0
			|| php_stream_read(fp, (char*)saved, sizeof(saved)) != sizeof(saved)
			|| memcmp(digest, saved, sizeof(digest))) {
				efree(savebuf);
				php_stream_close(fp);
				if (error) {
					spprintf(error, 0, "phar \"%s\" has a broken signature", fname);
				}
				return FAILURE;
			}

			sig_len = phar_hex_str((const char*)digest, sizeof(digest), &signature);
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32], saved[32];
			PHP_SHA256_CTX  context;

			php_stream_rewind(fp);
			PHP_SHA256Init(&context);
			read_len -= sizeof(digest);
			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}
			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA256Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}
			PHP_SHA256Final(digest, &context);

			if (read_len > 0
			|| php_stream_read(fp, (char*)saved, sizeof(saved)) != sizeof(saved)
			|| memcmp(digest, saved, sizeof(digest))) {
				efree(savebuf);
				php_stream_close(fp);
				if (error) {
					spprintf(error, 0, "phar \"%s\" has a broken signature", fname);
				}
				return FAILURE;
			}

			sig_len = phar_hex_str((const char*)digest, sizeof(digest), &signature);
			break;
		}
#else
		case PHAR_SIG_SHA512:
		case PHAR_SIG_SHA256:
			efree(savebuf);
			php_stream_close(fp);
			if (error) {
				spprintf(error, 0, "phar \"%s\" has a unsupported signature", fname);
			}
			return FAILURE;
#endif
		case PHAR_SIG_SHA1: {
			unsigned char digest[20], saved[20];
			PHP_SHA1_CTX  context;

			php_stream_rewind(fp);
			PHP_SHA1Init(&context);
			read_len -= sizeof(digest);
			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}
			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA1Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}
			PHP_SHA1Final(digest, &context);

			if (read_len > 0
			|| php_stream_read(fp, (char*)saved, sizeof(saved)) != sizeof(saved)
			|| memcmp(digest, saved, sizeof(digest))) {
				efree(savebuf);
				php_stream_close(fp);
				if (error) {
					spprintf(error, 0, "phar \"%s\" has a broken signature", fname);
				}
				return FAILURE;
			}

			sig_len = phar_hex_str((const char*)digest, sizeof(digest), &signature);
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16], saved[16];
			PHP_MD5_CTX   context;

			php_stream_rewind(fp);
			PHP_MD5Init(&context);
			read_len -= sizeof(digest);
			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}
			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_MD5Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}
			PHP_MD5Final(digest, &context);

			if (read_len > 0
			|| php_stream_read(fp, (char*)saved, sizeof(saved)) != sizeof(saved)
			|| memcmp(digest, saved, sizeof(digest))) {
				efree(savebuf);
				php_stream_close(fp);
				if (error) {
					spprintf(error, 0, "phar \"%s\" has a broken signature", fname);
				}
				return FAILURE;
			}

			sig_len = phar_hex_str((const char*)digest, sizeof(digest), &signature);
			break;
		}
		default:
			efree(savebuf);
			php_stream_close(fp);
			if (error) {
				spprintf(error, 0, "phar \"%s\" has a broken or unsupported signature", fname);
			}
			return FAILURE;
		}
	} else if (PHAR_G(require_hash)) {
		efree(savebuf);
		php_stream_close(fp);
		if (error) {
			spprintf(error, 0, "phar \"%s\" does not have a signature", fname);
		}
		return FAILURE;
	} else {
		sig_flags = 0;
		sig_len = 0;
	}

	/* extract alias */
	PHAR_GET_32(buffer, tmp_len);
	if (buffer + tmp_len > endbuffer) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (buffer overrun)");
	}
	if (manifest_len < 10 + tmp_len) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest header)")
	}
	/* tmp_len = 0 says alias length is 0, which means the alias is not stored in the phar */
	if (tmp_len) {
		/* if the alias is stored we enforce it (implicit overrides explicit) */
		if (alias && alias_len && (alias_len != (int)tmp_len || strncmp(alias, buffer, tmp_len)))
		{
			buffer[tmp_len] = '\0';
			efree(savebuf);
			php_stream_close(fp);
			if (signature) {
				efree(signature);
			}
			if (error) {
				spprintf(error, 0, "cannot load phar \"%s\" with implicit alias \"%s\" under different alias \"%s\"", fname, buffer, alias);
			}
			return FAILURE;
		}
		alias_len = tmp_len;
		alias = buffer;
		buffer += tmp_len;
		register_alias = 1;
	} else if (!alias_len || !alias) {
		/* if we neither have an explicit nor an implicit alias, we use the filename */
		alias = NULL;
		alias_len = 0;
		register_alias = 0;
	} else {
		register_alias = 1;
	}
	
	/* we have 5 32-bit items plus 1 byte at least */
	if (manifest_count > ((manifest_len - 10 - tmp_len) / (5 * 4 + 1))) {
		/* prevent serious memory issues */
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (too many manifest entries for size of manifest)")
	}

	mydata = ecalloc(sizeof(phar_archive_data), 1);

	/* check whether we have meta data, zero check works regardless of byte order */
	if (phar_parse_metadata(&buffer, &mydata->metadata, 0 TSRMLS_CC) == FAILURE) {
		MAPPHAR_FAIL("unable to read phar metadata in .phar file \"%s\"");
	}

	/* set up our manifest */
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	offset = 0;
	for (manifest_index = 0; manifest_index < manifest_count; manifest_index++) {
		if (buffer + 4 > endbuffer) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest entry)")
		}
		memset(&entry, 0, sizeof(phar_entry_info));
		PHAR_GET_32(buffer, entry.filename_len);
		if (entry.filename_len == 0) {
			MAPPHAR_FAIL("zero-length filename encountered in phar \"%s\"");
		}
		if (buffer + entry.filename_len + 20 > endbuffer) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest entry)");
		}
		entry.filename = estrndup(buffer, entry.filename_len);
		buffer += entry.filename_len;
		PHAR_GET_32(buffer, entry.uncompressed_filesize);
		PHAR_GET_32(buffer, entry.timestamp);
		if (offset == 0) {
			mydata->min_timestamp = entry.timestamp;
			mydata->max_timestamp = entry.timestamp;
		} else {
			if (mydata->min_timestamp > entry.timestamp) {
				mydata->min_timestamp = entry.timestamp;
			} else if (mydata->max_timestamp < entry.timestamp) {
				mydata->max_timestamp = entry.timestamp;
			}
		}
		PHAR_GET_32(buffer, entry.compressed_filesize);
		PHAR_GET_32(buffer, entry.crc32);
		PHAR_GET_32(buffer, entry.flags);
		if (phar_parse_metadata(&buffer, &entry.metadata, 0 TSRMLS_CC) == FAILURE) {
			efree(entry.filename);
			MAPPHAR_FAIL("unable to read file metadata in .phar file \"%s\"");
		}
		entry.offset_within_phar = offset;
		offset += entry.compressed_filesize;
		switch (entry.flags & PHAR_ENT_COMPRESSION_MASK) {
		case PHAR_ENT_COMPRESSED_GZ:
#if !HAVE_ZLIB
			if (entry.metadata) {
				zval_ptr_dtor(&entry.metadata);
			}
			efree(entry.filename);
			MAPPHAR_FAIL("zlib extension is required for gz compressed .phar file \"%s\"");
#else
			if (!phar_has_zlib) {
				if (entry.metadata) {
					zval_ptr_dtor(&entry.metadata);
				}
				efree(entry.filename);
				MAPPHAR_FAIL("zlib extension is required for gz compressed .phar file \"%s\"");
			}
#endif
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
#if !HAVE_BZ2
			if (entry.metadata) {
				zval_ptr_dtor(&entry.metadata);
			}
			efree(entry.filename);
			MAPPHAR_FAIL("bz2 extension is required for bzip2 compressed .phar file \"%s\"");
#else
			if (!phar_has_bz2) {
				if (entry.metadata) {
					zval_ptr_dtor(&entry.metadata);
				}
				efree(entry.filename);
				MAPPHAR_FAIL("bz2 extension is required for bzip2 compressed .phar file \"%s\"");
			}
#endif
			break;
		default:
			if (entry.uncompressed_filesize != entry.compressed_filesize) {
				if (entry.metadata) {
					zval_ptr_dtor(&entry.metadata);
				}
				efree(entry.filename);
				MAPPHAR_FAIL("internal corruption of phar \"%s\" (compressed and uncompressed size does not match for uncompressed entry)");
			}
			break;
		}
		manifest_flags |= (entry.flags & PHAR_ENT_COMPRESSION_MASK);
		/* if signature matched, no need to check CRC32 for each file */
		entry.is_crc_checked = (manifest_flags & PHAR_HDR_SIGNATURE ? 1 : 0);
		entry.fp = NULL;
		entry.phar = mydata;
		zend_hash_add(&mydata->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL);
	}

	snprintf(mydata->version, sizeof(mydata->version), "%u.%u.%u", manifest_ver >> 12, (manifest_ver >> 8) & 0xF, (manifest_ver >> 4) & 0xF);
	mydata->internal_file_start = halt_offset + manifest_len + 4;
	mydata->halt_offset = halt_offset;
	mydata->flags = manifest_flags;
	mydata->fp = fp;
	mydata->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	mydata->sig_flags = sig_flags;
	mydata->sig_len = sig_len;
	mydata->signature = signature;
	phar_request_initialize(TSRMLS_C);
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), mydata->fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*),  NULL);
	if (register_alias) {
		mydata->is_explicit_alias = 1;
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	} else {
		mydata->is_explicit_alias = 0;
	}
	efree(savebuf);
	
	if (pphar) {
		*pphar = mydata;
	}

	return SUCCESS;
}
/* }}} */

/**
 * Create or open a phar for writing
 */
int phar_open_or_create_zip(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
#if HAVE_PHAR_ZIP
	phar_archive_data *phar;
	int ret = phar_create_or_parse_filename(fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC);
	int register_alias;

	if (FAILURE == ret) {
		return FAILURE;
	}
	if ((*pphar)->is_zip) {
		return ret;
	}

	phar = *pphar;
	if (phar->is_brandnew) {
		int *errorp;
		phar->is_zip = 1;
		if (phar->fp) {
			php_stream_close(phar->fp);
			phar->fp = NULL;
			php_stream_unlink(phar->fname, 0, NULL);
		}
		phar->zip = zip_open(fname, 0 | ZIP_CREATE | ZIP_EXCL, errorp);
		if (NULL != phar->zip) {
			return SUCCESS;
		}
		/* fail - free newly created manifest entry */
		if (register_alias) {
			zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len);
		}
		zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len);
		efree(phar->fname);
		efree(phar->alias);
		efree(phar);
		*pphar = NULL;

		if (error) {
			/* now for the stupid hoops libzip forces... */
			char *tmp;
			int tmp_len;
			tmp_len = zip_error_to_str(NULL, 0, *errorp, *errorp);
			if (!(tmp = emalloc((tmp_len + 1) * sizeof(char)))) {
				spprintf(error, 4096, "phar zip error: cannot create zip-based phar \"%s\"", fname);
			} else {
				if (!zip_error_to_str(tmp, tmp_len + 1, *errorp, *errorp)) {
					spprintf(error, 4096, "phar zip error: cannot create zip-based phar \"%s\"", fname);
				} else {
					spprintf(error, 4096, "phar zip error: cannot create zip-based phar \"%s\": %s", fname, tmp);
					efree(tmp);
				}
			}
		}
		return FAILURE;
	}

	/* we've reached here - the phar exists and is a regular phar */
	if (error) {
		spprintf(error, 4096, "phar zip error: phar \"%s\" already exists as a regular phar and must be deleted from disk prior to creating as a zip-based phar", fname);
	}
	return FAILURE;
#else
	if (error) {
		spprintf(error, 4096, "phar zip error: phar \"%s\" cannot be created as zip-based phar, zip-based phars are disabled", fname);
	}
	return FAILURE;
#endif /* #if HAVE_PHAR_ZIP */
}

/**
 * Create or open a phar for writing
 */
int phar_open_or_create_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	char *ext_str;
	int ext_len;

	if (error) {
		*error = NULL;
	}

	if (phar_open_loaded(fname, fname_len, alias, alias_len, options, pphar, 0 TSRMLS_CC) == SUCCESS) {
		if (pphar && !PHAR_G(readonly)) {
			(*pphar)->is_writeable = 1;
		}
		return SUCCESS;
	}

	if (phar_detect_phar_fname_ext(fname, 1, &ext_str, &ext_len) == SUCCESS) {
		if (ext_len >= sizeof(".phar.zip")-1 && !memcmp((void *) ext_str, (void *) ".phar.zip", sizeof(".phar.zip")-1)) {
			/* assume zip-based phar */
			return phar_open_or_create_zip(fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC);
		}
		if (ext_len >= sizeof(".phar.tar")-1 && !memcmp((void *) ext_str, (void *) ".phar.tar", sizeof(".phar.tar")-1)) {
			/* assume tar-based phar */
			return phar_open_or_create_tar(fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC);
		}
	}
	return phar_create_or_parse_filename(fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC);
}

int phar_create_or_parse_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *mydata;
	char *my_realpath;
	int register_alias;
	php_stream *fp;

	if (!pphar) {
		pphar = &mydata;
	}
#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return FAILURE;
	}
#endif

	if (php_check_open_basedir(fname TSRMLS_CC)) {
		return FAILURE;
	}

	/* first open readonly so it won't be created if not present */
	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL);

	if (fp) {
		if (phar_open_fp(fp, fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC) == SUCCESS) {
			if (!PHAR_G(readonly)) {
				(*pphar)->is_writeable = 1;
			}
			return SUCCESS;
		} else {
			/* file exists, but is either corrupt or not a phar archive */
			php_stream_close(fp);
			return FAILURE;
		}
		php_stream_close(fp);
	}


	if (PHAR_G(readonly)) {
		if (options & REPORT_ERRORS) {
			if (error) {
				spprintf(error, 0, "creating archive \"%s\" disabled by INI setting", fname);
			}
		}
		return FAILURE;
	}
	/* re-open for writing */
	fp = php_stream_open_wrapper(fname, "r+b", IGNORE_URL|STREAM_MUST_SEEK|0, NULL);

	/* set up our manifest */
	mydata = ecalloc(sizeof(phar_archive_data), 1);
	if (pphar) {
		*pphar = mydata;
	}
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	my_realpath = NULL;
	my_realpath = expand_filepath(fname, my_realpath TSRMLS_CC);
	if (my_realpath) {
		fname_len = strlen(my_realpath);
#ifdef PHP_WIN32
		phar_unixify_path_separators(my_realpath, fname_len);
#endif
		fname = my_realpath;
		mydata->fname = my_realpath;
	} else {
		mydata->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
		phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	}
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	snprintf(mydata->version, sizeof(mydata->version), "%s", PHAR_API_VERSION_STR);
	mydata->is_explicit_alias = alias ? 1 : 0;
	mydata->internal_file_start = -1;
	mydata->fp = fp;
	mydata->is_writeable = 1;
	mydata->is_brandnew = 1;
	if (!alias_len || !alias) {
		/* if we neither have an explicit nor an implicit alias, we use the filename */
		alias = NULL;
		alias_len = 0;
		register_alias = 0;
	} else {
		register_alias = 1;
	}
	phar_request_initialize(TSRMLS_C);
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), mydata->fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*),  NULL);
	if (register_alias) {
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	}
	return SUCCESS;
}
/* }}}*/

/**
 * Return an already opened filename.
 *
 * Or scan a phar file for the required __HALT_COMPILER(); ?> token and verify
 * that the manifest is proper, then pass it to phar_open_file().  SUCCESS
 * or FAILURE is returned and pphar is set to a pointer to the phar's manifest
 */
int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	php_stream *fp;

	if (error) {
		*error = NULL;
	}
	
	if (phar_open_loaded(fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	} else if (error && *error) {
		return FAILURE;
	}

#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return FAILURE;
	}
#endif

	if (php_check_open_basedir(fname TSRMLS_CC)) {
		return FAILURE;
	}

	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK, NULL);
	
	if (!fp) {
		if (options & REPORT_ERRORS) {
			if (error) {
				spprintf(error, 0, "unable to open phar for reading \"%s\"", fname);
			}
		}
		return FAILURE;
	}

	return phar_open_fp(fp, fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC);
}
/* }}}*/

/**
 * Scan an open fp for the required __HALT_COMPILER(); ?> token and verify
 * that the manifest is proper, then pass it to phar_open_file().  SUCCESS
 * or FAILURE is returned and pphar is set to a pointer to the phar's manifest
 */
static int phar_open_fp(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	const char token[] = "__HALT_COMPILER();";
	const char zip_magic[] = "PK\x03\x04";
	char *pos, buffer[1024 + sizeof(token)], test = '\0';
	const long readsize = sizeof(buffer) - sizeof(token);
	const long tokenlen = sizeof(token) - 1;
	long halt_offset;
	size_t got;

	/* Maybe it's better to compile the file instead of just searching,  */
	/* but we only want the offset. So we want a .re scanner to find it. */

	if (error) {
		*error = NULL;
	}
	if (-1 == php_stream_rewind(fp)) {
		MAPPHAR_ALLOC_FAIL("cannot rewind phar \"%s\"")
	}

	buffer[sizeof(buffer)-1] = '\0';
	memset(buffer, 32, sizeof(token));
	halt_offset = 0;
	while(!php_stream_eof(fp)) {
		if ((got = php_stream_read(fp, buffer+tokenlen, readsize)) < (size_t) tokenlen) {
			MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated entry)")
		}
		if (!test) {
			test = '\1';
			pos = buffer+tokenlen;
			if (!memcmp(pos, zip_magic, 4)) {
				php_stream_close(fp);
				return phar_open_zipfile(fname, fname_len, alias, alias_len, pphar, error TSRMLS_CC);
			}
			if (got > 512) {
				if (phar_is_tar(pos)) {
					return phar_open_tarfile(fp, fname, fname_len, alias, alias_len, options, pphar, error TSRMLS_CC);
				}
			}
		}
		if ((pos = strstr(buffer, token)) != NULL) {
			halt_offset += (pos - buffer); /* no -tokenlen+tokenlen here */
			return phar_open_file(fp, fname, fname_len, alias, alias_len, halt_offset, pphar, error TSRMLS_CC);
		}

		halt_offset += got;
		memmove(buffer, buffer + tokenlen, got + 1);
	}
	
	MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (__HALT_COMPILER(); not found)")
}
/* }}} */

int phar_detect_phar_fname_ext(const char *filename, int check_length, char **ext_str, int *ext_len) /* {{{ */
{
	char end;
	char *pos_p = strstr(filename, ".phar.php");
	char *pos_zi = strstr(filename, ".phar.zip");
	char *pos_zi2 = strstr(filename, ".phar.zip.php");
	char *pos_t = strstr(filename, ".phar.tar");
	char *pos_t2 = strstr(filename, ".phar.tar.php");
	char *pos_z = strstr(filename, ".phar.gz");
	char *pos_b = strstr(filename, ".phar.bz2");

	if (pos_p) {
		if (pos_z) {
			return FAILURE;
		}
		*ext_str = pos_p;
		*ext_len = 9;
	} else if (pos_z) {
		*ext_str = pos_z;
		*ext_len = 8;
	} else if (pos_b) {
		*ext_str = pos_b;
		*ext_len = 9;
	} else if (pos_zi2) {
		*ext_str = pos_zi2;
		*ext_len = 13;
	} else if (pos_zi) {
		*ext_str = pos_zi;
		*ext_len = 9;
	} else if (pos_t2) {
		*ext_str = pos_t2;
		*ext_len = 13;
	} else if (pos_t) {
		*ext_str = pos_t;
		*ext_len = 9;
	} else if ((pos_p = strstr(filename, ".phar")) != NULL && pos_p[4] != '\0') {
		*ext_str = pos_p;
		*ext_len = 5;
	} else if ((pos_p = strstr(filename, ".php")) != NULL && pos_p[4] != '\0') {
		*ext_str = pos_p;
		*ext_len = 4;
	} else {
		/* We have an alias with no extension, so locate the first / and fail */
		*ext_str = strstr(filename, "/");
		if (*ext_str) {
			*ext_len = -1;
		}
		return FAILURE;
	}
	if (check_length && (*ext_str)[*ext_len] != '\0') {
		return FAILURE;
	}
	end = (*ext_str)[*ext_len];
	if (end != '\0' && end != '/' && end != '\\') {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static int php_check_dots(const char *element, int n) 
{
	for(n--; n >= 0; --n) {
		if (element[n] != '.') {
			return 1;
		}
	}
	return 0;
}

#define IS_DIRECTORY_UP(element, len) \
	(len >= 2 && !php_check_dots(element, len))

#define IS_DIRECTORY_CURRENT(element, len) \
	(len == 1 && element[0] == '.')

#define IS_BACKSLASH(c)     ((c) == '/')

#ifdef PHP_WIN32
/* stupid-ass non-extern declaration in tsrm_strtok.h breaks dumbass MS compiler */
static inline int in_character_class(char ch, const char *delim)
{
	while (*delim) {
		if (*delim == ch) {
			return 1;
		}
		delim++;
	}
	return 0;
}

char *tsrm_strtok_r(char *s, const char *delim, char **last)
{
	char *token;

	if (s == NULL) {
		s = *last;
	}

	while (*s && in_character_class(*s, delim)) {
		s++;
	}
	if (!*s) {
		return NULL;
	}

	token = s;

	while (*s && !in_character_class(*s, delim)) {
		s++;
	}
	if (!*s) {
		*last = s;
	} else {
		*s = '\0';
		*last = s + 1;
	}
	return token;
}
#endif

/**
 * Remove .. and . references within a phar filename
 */
static char *phar_fix_filepath(char *path, int *new_len) /* {{{ */
{
	char *ptr, *free_path, *new_phar;
	char *tok;
	int ptr_length, new_phar_len = 1, path_length = *new_len;

	free_path = path;
	new_phar = estrndup("/\0", 2);
	tok = NULL;
	ptr = tsrm_strtok_r(path, "/", &tok);
	while (ptr) {
		ptr_length = strlen(ptr);

		if (IS_DIRECTORY_UP(ptr, ptr_length)) {
			char save;

			save = '/';

#define PREVIOUS new_phar[new_phar_len - 1]

			while (new_phar_len > 1 &&
					!IS_BACKSLASH(PREVIOUS)) {
				save = PREVIOUS;
				PREVIOUS = '\0';
				new_phar_len--;
			}

			if (new_phar[0] != '/') {
				new_phar[new_phar_len++] = save;
				new_phar[new_phar_len] = '\0';
			} else if (new_phar_len > 1) {
				PREVIOUS = '\0';
				new_phar_len--;
			}
		} else if (!IS_DIRECTORY_CURRENT(ptr, ptr_length)) {
			if (new_phar_len > 1) {
				new_phar = (char *) erealloc(new_phar, new_phar_len+ptr_length+1+1);
				new_phar[new_phar_len++] = '/';
				memcpy(&new_phar[new_phar_len], ptr, ptr_length+1);
			} else {
				new_phar = (char *) erealloc(new_phar, new_phar_len+ptr_length+1);
				memcpy(&new_phar[new_phar_len], ptr, ptr_length+1);
			}

			new_phar_len += ptr_length;
		}
		ptr = tsrm_strtok_r(NULL, "/", &tok);
	}

	efree(free_path);

	if (path[path_length-1] == '/' && new_phar_len > 1) {
		new_phar = (char*)erealloc(new_phar, new_phar_len + 2);
		new_phar[new_phar_len++] = '/';
		new_phar[new_phar_len] = 0;
	}

	if (new_phar_len == 0) {
		new_phar = (char *) erealloc(new_phar, new_phar_len+1+1);
		new_phar[new_phar_len] = '/';
		new_phar[new_phar_len+1] = '\0';
		new_phar_len++;
	}
	*new_len = new_phar_len;
	return new_phar;
}
/* }}} */

/**
 * Process a phar stream name, ensuring we can handle any of:
 * 
 * - whatever.phar
 * - whatever.phar.gz
 * - whatever.phar.bz2
 * - whatever.phar.php
 *
 * Optionally the name might start with 'phar://'
 *
 * This is used by phar_open_url()
 */
int phar_split_fname(char *filename, int filename_len, char **arch, int *arch_len, char **entry, int *entry_len TSRMLS_DC) /* {{{ */
{
	char *ext_str;
	int ext_len;

	if (!strncasecmp(filename, "phar://", 7)) {
		filename += 7;
		filename_len -= 7;
	}

	ext_len = 0;
	if (phar_detect_phar_fname_ext(filename, 0, &ext_str, &ext_len) == FAILURE) {
		if (ext_len != -1) {
			if (!ext_str) {
				/* no / detected, restore arch for error message */
				*arch = filename;
			}
			return FAILURE;
		}
		ext_len = 0;
		/* no extension detected - instead we are dealing with an alias */
	}

	*arch_len = ext_str - filename + ext_len;
	*arch = estrndup(filename, *arch_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(*arch, *arch_len);
#endif
	if (ext_str[ext_len]) {
		*entry_len = filename_len - *arch_len;
		*entry = estrndup(ext_str+ext_len, *entry_len);
#ifdef PHP_WIN32
		phar_unixify_path_separators(*entry, *entry_len);
#endif
		*entry = phar_fix_filepath(*entry, entry_len);
	} else {
		*entry_len = 1;
		*entry = estrndup("/", 1);
	}
	return SUCCESS;
}
/* }}} */
	
/**
 * Open a phar file for streams API
 */
static php_url* phar_open_url(php_stream_wrapper *wrapper, char *filename, char *mode, int options TSRMLS_DC) /* {{{ */
{
	php_url *resource;
	char *arch = NULL, *entry = NULL, *error;
	int arch_len, entry_len;

	if (!strncasecmp(filename, "phar://", 7)) {
		if (mode[0] == 'a') {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: open mode append not supported");
			return NULL;			
		}		
		if (phar_split_fname(filename, strlen(filename), &arch, &arch_len, &entry, &entry_len TSRMLS_CC) == FAILURE) {
			if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
				if (arch && !entry) {
					php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory (always use full path to a new phar)", filename, arch);
					arch = NULL;
				} else {
					php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\" (cannot contain .phar.php and .phar.gz/.phar.bz2)", filename);
				}
			}
			if (arch) {
				efree(arch);
			}
			if (entry) {
				efree(entry);
			}
			return NULL;
		}
		resource = ecalloc(1, sizeof(php_url));
		resource->scheme = estrndup("phar", 4);
		resource->host = arch;

		resource->path = entry;
#if MBO_0
		if (resource) {
			fprintf(stderr, "Alias:     %s\n", alias);
			fprintf(stderr, "Scheme:    %s\n", resource->scheme);
/*			fprintf(stderr, "User:      %s\n", resource->user);*/
/*			fprintf(stderr, "Pass:      %s\n", resource->pass ? "***" : NULL);*/
			fprintf(stderr, "Host:      %s\n", resource->host);
/*			fprintf(stderr, "Port:      %d\n", resource->port);*/
			fprintf(stderr, "Path:      %s\n", resource->path);
/*			fprintf(stderr, "Query:     %s\n", resource->query);*/
/*			fprintf(stderr, "Fragment:  %s\n", resource->fragment);*/
		}
#endif
		if (PHAR_GLOBALS->request_init && PHAR_GLOBALS->phar_plain_map.arBuckets && zend_hash_exists(&(PHAR_GLOBALS->phar_plain_map), arch, arch_len+1)) {
			return resource;
		}
		if (mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+')) {
			if (PHAR_G(readonly)) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: write operations disabled by INI setting");
				php_url_free(resource);
				return NULL;
			}
			if (phar_open_or_create_filename(resource->host, arch_len, NULL, 0, options, NULL, &error TSRMLS_CC) == FAILURE)
			{
				if (error) {
					php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
					efree(error);
				}
				php_url_free(resource);
				return NULL;
			}
		} else {
			if (phar_open_filename(resource->host, arch_len, NULL, 0, options, NULL, &error TSRMLS_CC) == FAILURE)
			{
				if (error) {
					php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
					efree(error);
				}
				php_url_free(resource);
				return NULL;
			}
		}	
		return resource;
	}

	return NULL;
}
/* }}} */

/**
 * Invoked when a user calls Phar::mapPhar() from within an executing .phar
 * to set up its manifest directly
 */
int phar_open_compiled_file(char *alias, int alias_len, char **error TSRMLS_DC) /* {{{ */
{
	char *fname;
	long halt_offset;
	zval *halt_constant;
	php_stream *fp;
	int fname_len;

	if (error) {
		*error = NULL;
	}
	fname = zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

	if (strstr(fname, "phar://") && strstr(fname, ".phar/stub.php")) {
		char *arch, *entry;
		int arch_len, entry_len;
		phar_archive_data *phar;

		if (SUCCESS != phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len TSRMLS_CC)) {
			efree(entry);
			efree(arch);
			goto regular_error;
		}
		if (strcmp(entry, "/.phar/stub.php")) {
			efree(entry);
			efree(arch);
			goto regular_error;
		}

		/* we're running from inside a zip-based phar */
		efree(entry);
		entry = fname;
		fname = arch;
		fname_len = arch_len;
		if (SUCCESS == phar_open_loaded(fname, fname_len, alias, alias_len, 0, &phar, 0 TSRMLS_CC) && phar && (phar->is_zip || phar->is_tar)) {
			efree(arch);
			return SUCCESS;
		}
		/* restore original value for error purpose */
		fname = entry;
	}
regular_error:

	if (phar_open_loaded(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL, 0 TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	if (!strcmp(fname, "[no active file]")) {
		if (error) {
			spprintf(error, 0, "cannot initialize a phar outside of PHP execution");
		}
		return FAILURE;
	}

	MAKE_STD_ZVAL(halt_constant);
	if (0 == zend_get_constant("__COMPILER_HALT_OFFSET__", 24, halt_constant TSRMLS_CC)) {
		FREE_ZVAL(halt_constant);
		if (error) {
			spprintf(error, 0, "__HALT_COMPILER(); must be declared in a phar");
		}
		return FAILURE;
	}
	halt_offset = Z_LVAL(*halt_constant);
	FREE_ZVAL(halt_constant);
	
	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!fp) {
		if (error) {
			spprintf(error, 0, "unable to open phar for reading \"%s\"", fname);
		}
		return FAILURE;
	}

	return phar_open_file(fp, fname, fname_len, alias, alias_len, halt_offset, NULL, error TSRMLS_CC);
}
/* }}} */

static php_stream_ops phar_ops = {
	phar_stream_write, /* write */
	phar_stream_read,  /* read */
	phar_stream_close, /* close */
	phar_stream_flush, /* flush */
	"phar stream",
	phar_stream_seek,  /* seek */
	NULL,              /* cast */
	phar_stream_stat,  /* stat */
	NULL, /* set option */
};

static php_stream_ops phar_dir_ops = {
	phar_dir_write, /* write */
	phar_dir_read,  /* read  */
	phar_dir_close, /* close */
	phar_dir_flush, /* flush */
	"phar dir",
	phar_dir_seek,  /* seek */
	NULL,           /* cast */
	phar_dir_stat,  /* stat */
	NULL, /* set option */
};

static php_stream_wrapper_ops phar_stream_wops = {
    phar_wrapper_open_url,
    NULL,                  /* phar_wrapper_close */
    NULL,                  /* phar_wrapper_stat, */
    phar_wrapper_stat,     /* stat_url */
    phar_wrapper_open_dir, /* opendir */
    "phar",
    phar_wrapper_unlink,   /* unlink */
    phar_wrapper_rename,   /* rename */
    NULL,                  /* create directory */
    NULL,                  /* remove directory */
};

php_stream_wrapper php_stream_phar_wrapper =  {
    &phar_stream_wops,
    NULL,
    0 /* is_url */
};

/**
 * Validate the CRC32 of a file opened from within the phar
 */
static int phar_postprocess_file(php_stream_wrapper *wrapper, int options, phar_entry_data *idata, php_uint32 crc32 TSRMLS_DC) /* {{{ */
{
	php_uint32 crc = ~0;
	int len = idata->internal_file->uncompressed_filesize;
	php_stream *fp = idata->fp;

	php_stream_seek(fp, 0 + idata->zero, SEEK_SET);	
	while (len--) { 
		CRC32(crc, php_stream_getc(fp));
	}
	php_stream_seek(fp, 0 + idata->zero, SEEK_SET);
	if (~crc == crc32) {
		return SUCCESS;
	} else {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (crc32 mismatch on file \"%s\")", idata->phar->fname, idata->internal_file->filename);
		return FAILURE;
	}
}
/* }}} */

/**
 * Determine which stream compression filter (if any) we need to read this file
 */
static char * phar_compress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
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
static char * phar_decompress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
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
 * helper function to open an internal file's fp just-in-time
 */
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, php_stream *fp,
				      char **error, int for_write TSRMLS_DC)
{
	php_uint32 offset, flags;
	php_stream_filter *filter/*,  *consumed */;
	char *filter_name;
	char *buffer;

	if (error) {
		*error = NULL;
	}
#if HAVE_PHAR_ZIP
	if (entry->is_zip) {
		char readbuf[8192];
		int got;
		if (entry->fp) {
			return entry;
		}
		if (!entry->zip) {
			if (entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
# if HAVE_BZ2
				char *filter_name;
				php_stream_filter *filter;
				php_stream *fp;
				/* we have to decompress this by hand */

				if (!phar_has_bz2) {
					if (error) {
						spprintf(error, 4096, "phar error, cannot decompress bzip2-compressed entry"); 
					}
					return NULL;
				}

				/* first, read into a temp stream */
				fp = php_stream_temp_new();
				entry->zip = zip_fopen_index(phar->zip, entry->index, ZIP_FL_COMPRESSED);
				if (!entry->zip) {
					if (error) {
						spprintf(error, 4096, "phar error: %s", zip_strerror(phar->zip));
					}
					zip_error_clear(phar->zip);
					return NULL;
				}
				do {
					size_t check;
					got = zip_fread(entry->zip, (void *)readbuf, 8192);
					if (-1 == got) break;
					check = got;
					if (check != php_stream_write(fp, readbuf, got)) {
						if (error) {
							spprintf(error, 4096, "phar error: could not copy full zip file contents of entry \"%s\"", entry->filename);
						}
						php_stream_close(fp);
						entry->fp = NULL;
						zip_fclose(entry->zip);
						entry->zip = NULL;
						return NULL;
					}
				} while (got == 8192);
				zip_fclose(entry->zip);
				php_stream_seek(fp, 0, SEEK_SET);

				/* now use a decompression filter to inflate into our temp file */
				if ((filter_name = phar_decompress_filter(entry, 0)) != NULL) {
					filter = php_stream_filter_create(filter_name, NULL, php_stream_is_persistent(fp) TSRMLS_CC);
				} else {
					filter = NULL;
				}
				if (!filter) {
					spprintf(error, 0, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", entry->phar->fname, phar_decompress_filter(entry, 1), entry->filename);
					return NULL;			
				}

				/* now we can safely use proper decompression */
				entry->fp = php_stream_temp_new();
				php_stream_filter_append(&entry->fp->writefilters, filter);
				if (php_stream_copy_to_stream(fp, entry->fp, entry->compressed_filesize) != entry->compressed_filesize || php_stream_tell(entry->fp) != (off_t) entry->uncompressed_filesize) {
					spprintf(error, 0, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
					php_stream_filter_remove(filter, 1 TSRMLS_CC);
					return NULL;
				}
				php_stream_filter_flush(filter, 1);
				php_stream_filter_remove(filter, 1 TSRMLS_CC);
				php_stream_close(fp);
				return entry;
# else /* #if HAVE_BZ2 */
				if (error) {
					spprintf(error, 4096, "phar error, cannot decompress bzip2-compressed entry"); 
				}
				return NULL;
# endif /* #if HAVE_BZ2 */
			} else {
				/* uncompressed or zlib-compressed */
				entry->zip = zip_fopen_index(phar->zip, entry->index, 0);
				if (!entry->zip) {
					if (error) {
						spprintf(error, 4096, "phar error: %s", zip_strerror(phar->zip));
					}
					zip_error_clear(phar->zip);
					return NULL;
				}
			}
		}

		/* load contents of zip file to temp stream */
		entry->fp = php_stream_temp_new();
		do {
			size_t check;
			got = zip_fread(entry->zip, (void *)readbuf, 8192);
			if (-1 == got) break;
			check = got;
			if (check != php_stream_write(entry->fp, readbuf, got)) {
				if (error) {
					spprintf(error, 4096, "phar error: could not copy full zip file contents of entry \"%s\"", entry->filename);
				}
				php_stream_close(entry->fp);
				entry->fp = NULL;
				zip_fclose(entry->zip);
				entry->zip = NULL;
				return NULL;
			}
		} while (got == 8192);
		zip_fclose(entry->zip);
		entry->zip = NULL;
		return entry;
	}
#endif /* #if HAVE_PHAR_ZIP */
	/* seek to start of internal file and read it */
	offset = phar->internal_file_start + entry->offset_within_phar;
	if (-1 == php_stream_seek(fp, offset, SEEK_SET)) {
		spprintf(error, 0, "phar error: internal corruption of phar \"%s\" (cannot seek to start of file \"%s\" at offset \"%d\")",
			phar->fname, entry->filename, offset);
		return NULL;
	}

	if (entry->is_modified) {
		flags = entry->old_flags;
	} else {
		flags = entry->flags;
	}

	if ((flags & PHAR_ENT_COMPRESSION_MASK) != 0) {
		if ((filter_name = phar_decompress_filter(entry, 0)) != NULL) {
			filter = php_stream_filter_create(phar_decompress_filter(entry, 0), NULL, php_stream_is_persistent(fp) TSRMLS_CC);
		} else {
			filter = NULL;
		}
		if (!filter) {
			spprintf(error, 0, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", phar->fname, phar_decompress_filter(entry, 1), entry->filename);
			return NULL;			
		}
		/* now we can safely use proper decompression */
		entry->old_flags = entry->flags;
		buffer = (char *) emalloc(8192);

		entry->fp = php_stream_temp_new();
		php_stream_filter_append(&entry->fp->writefilters, filter);
		if (php_stream_copy_to_stream(fp, entry->fp, entry->compressed_filesize) != entry->compressed_filesize || php_stream_tell(entry->fp) != (off_t) entry->uncompressed_filesize) {
			efree(buffer);
			spprintf(error, 0, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			return NULL;
		}
		efree(buffer);
		php_stream_filter_flush(filter, 1);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		php_stream_seek(fp, offset + entry->compressed_filesize, SEEK_SET);
	} else { /* from here is for non-compressed */
		if (!for_write && !entry->is_modified) {
			/* recycle the phar fp */
			entry->fp = phar->fp;
			return entry;
		}
		/* bypass to temp stream */
		entry->fp = php_stream_temp_new();
		if (php_stream_copy_to_stream(fp, entry->fp, entry->uncompressed_filesize) != entry->uncompressed_filesize) {
			spprintf(error, 0, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
			return NULL;
		}
	}
	return entry;
}

/**
 * used for fopen('phar://...') and company
 */
static php_stream * phar_wrapper_open_url(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	phar_entry_data *idata;
	char *internal_file;
	char *error;
	char *plain_map;
	HashTable *pharcontext;
	php_url *resource = NULL;
	php_stream *fp, *fpf;
	zval **pzoption, *metadata;
	uint host_len;

	if ((resource = phar_open_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", path);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", path);
		return NULL;
	}

	host_len = strlen(resource->host);
	phar_request_initialize(TSRMLS_C);
	if (zend_hash_find(&(PHAR_GLOBALS->phar_plain_map), resource->host, host_len+1, (void **)&plain_map) == SUCCESS) {
		spprintf(&internal_file, 0, "%s%s", plain_map, resource->path);
		fp = php_stream_open_wrapper_ex(internal_file, mode, options, opened_path, context);
		if (!fp) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" extracted from \"%s\" could not be opened", internal_file, resource->host);
		}
		php_url_free(resource);
		efree(internal_file);
		return fp;
	}

	/* strip leading "/" */
	internal_file = estrdup(resource->path + 1);
	if (mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+')) {
		if (NULL == (idata = phar_get_or_create_entry_data(resource->host, host_len, internal_file, strlen(internal_file), mode, &error TSRMLS_CC))) {
			if (error) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
				efree(error);
			} else {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" could not be created in phar \"%s\"", internal_file, resource->host);
			}
			efree(internal_file);
			php_url_free(resource);
			return NULL;
		}
		if (error) {
			efree(error);
		}
		fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
		php_url_free(resource);
		efree(internal_file);
		if (context && context->options && zend_hash_find(HASH_OF(context->options), "phar", sizeof("phar"), (void**)&pzoption) == SUCCESS) {
			pharcontext = HASH_OF(*pzoption);
			if (idata->internal_file->uncompressed_filesize == 0
				&& idata->internal_file->compressed_filesize == 0
				&& zend_hash_find(pharcontext, "compress", sizeof("compress"), (void**)&pzoption) == SUCCESS
				&& Z_TYPE_PP(pzoption) == IS_LONG
				&& (Z_LVAL_PP(pzoption) & ~PHAR_ENT_COMPRESSION_MASK) == 0
			) {
				idata->internal_file->flags &= ~PHAR_ENT_COMPRESSION_MASK;
				idata->internal_file->flags |= Z_LVAL_PP(pzoption);
			}
			if (zend_hash_find(pharcontext, "metadata", sizeof("metadata"), (void**)&pzoption) == SUCCESS) {
				if (idata->internal_file->metadata) {
					zval_ptr_dtor(&idata->internal_file->metadata);
					idata->internal_file->metadata = NULL;
				}

				MAKE_STD_ZVAL(idata->internal_file->metadata);
				metadata = *pzoption;
				ZVAL_ZVAL(idata->internal_file->metadata, metadata, 1, 0);
				idata->phar->is_modified = 1;
			}
		}
		return fpf;
	} else {
		if ((FAILURE == phar_get_entry_data(&idata, resource->host, host_len, internal_file, strlen(internal_file), "r", &error TSRMLS_CC)) || !idata) {
			if (error) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
				efree(error);
			}
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" is not a file in phar \"%s\"", internal_file, resource->host);
			efree(internal_file);
			php_url_free(resource);
			return NULL;
		}
	}
	php_url_free(resource);
	
#if MBO_0
		fprintf(stderr, "Pharname:   %s\n", idata->phar->filename);
		fprintf(stderr, "Filename:   %s\n", internal_file);
		fprintf(stderr, "Entry:      %s\n", idata->internal_file->filename);
		fprintf(stderr, "Size:       %u\n", idata->internal_file->uncompressed_filesize);
		fprintf(stderr, "Compressed: %u\n", idata->internal_file->flags);
		fprintf(stderr, "Offset:     %u\n", idata->internal_file->offset_within_phar);
		fprintf(stderr, "Cached:     %s\n", idata->internal_file->filedata ? "yes" : "no");
#endif

	/* do we have the data already? */
	if (idata->fp) {
		fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
		efree(internal_file);
		return fpf;
	}

#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(idata->phar->fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}
#endif
	
	if (php_check_open_basedir(idata->phar->fname TSRMLS_CC)) {
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}

	fp = idata->phar->fp;

	if (!idata->phar->is_zip && !fp) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot open phar \"%s\"", idata->phar->fname);
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}

	idata->internal_file = phar_open_jit(idata->phar, idata->internal_file, fp, &error, idata->for_write TSRMLS_CC);
	if (!idata->internal_file) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
		efree(error);
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}
	idata->fp = idata->internal_file->fp;
	if (idata->fp == idata->phar->fp) {
		idata->zero = idata->internal_file->offset_within_phar + idata->phar->internal_file_start;
	}

	/* check length, crc32 */
	if (!idata->internal_file->is_crc_checked && phar_postprocess_file(wrapper, options, idata, idata->internal_file->crc32 TSRMLS_CC) != SUCCESS) {
		/* already issued the error */
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}
	idata->internal_file->is_crc_checked = 1;

	fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
	efree(internal_file);
	return fpf;
}
/* }}} */

/**
 * Used for fclose($fp) where $fp is a phar archive
 */
static int phar_stream_close(php_stream *stream, int close_handle TSRMLS_DC) /* {{{ */
{
	phar_entry_delref((phar_entry_data *)stream->abstract TSRMLS_CC);

	return 0;
}
/* }}} */

/**
 * Used for closedir($fp) where $fp is an opendir('phar://...') directory handle
 */
static int phar_dir_close(php_stream *stream, int close_handle TSRMLS_DC)  /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (data && data->arBuckets)
	{
		zend_hash_destroy(data);
		data->arBuckets = 0;
		FREE_HASHTABLE(data);
		stream->abstract = NULL;
	}
	return 0;
}
/* }}} */

/**
 * Used for seeking on a phar directory handle
 */
static int phar_dir_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (data)
	{
		if (whence == SEEK_END) {
			whence = SEEK_SET;
			offset = zend_hash_num_elements(data) + offset;
		}
		if (whence == SEEK_SET) {
			zend_hash_internal_pointer_reset(data);
		}

		if (offset < 0) {
			php_stream_wrapper_log_error(stream->wrapper, stream->flags TSRMLS_CC, "phar error: cannot seek because the resulting seek is negative");
			return -1;
		} else {
			*newoffset = 0;
			while (*newoffset < offset && zend_hash_move_forward(data) == SUCCESS) {
				(*newoffset)++;
			}
			return 0;
		}
	}
	return -1;
}
/* }}} */

/**
 * used for fread($fp) and company on a fopen()ed phar file handle
 */
static size_t phar_stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;
	size_t got;
	
	if (data->internal_file->is_deleted) {
		stream->eof = 1;
		return 0;
	}

	/* use our proxy position */
	php_stream_seek(data->fp, data->position + data->zero, SEEK_SET);

	if (!data->zero) {
		got = php_stream_read(data->fp, buf, count);
		if (data->fp->eof) {
			stream->eof = 1;
		}
		/* note the position, and restore the stream for the next fp */
		data->position = php_stream_tell(data->fp);
	} else {
		got = php_stream_read(data->fp, buf, MIN(count, data->internal_file->uncompressed_filesize - data->position));
		data->position = php_stream_tell(data->fp) - data->zero;
		stream->eof = (data->position == (off_t) data->internal_file->uncompressed_filesize);
	}


	return got;
}
/* }}} */

/**
 * Used for readdir() on an opendir()ed phar directory handle
 */
static size_t phar_dir_read(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	size_t to_read;
	HashTable *data = (HashTable *)stream->abstract;
	char *key;
	uint keylen;
	ulong unused;

	if (FAILURE == zend_hash_has_more_elements(data)) {
		return 0;
	}
	if (HASH_KEY_NON_EXISTANT == zend_hash_get_current_key_ex(data, &key, &keylen, &unused, 0, NULL)) {
		return 0;
	}
	zend_hash_move_forward(data);
	to_read = MIN(keylen, count);
	if (to_read == 0 || count < keylen) {
		return 0;
	}
	memset(buf, 0, sizeof(php_stream_dirent));
	memcpy(((php_stream_dirent *) buf)->d_name, key, to_read);
	((php_stream_dirent *) buf)->d_name[to_read + 1] = '\0';

	return sizeof(php_stream_dirent);
}
/* }}} */

/**
 * Used for fseek($fp) on a phar file handle
 */
static int phar_stream_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	int res;
	if (data->zero) {
		off_t temp;
		switch (whence) {
			case SEEK_END :
				temp = data->zero + data->internal_file->uncompressed_filesize + offset;
				break;
			case SEEK_CUR :
				temp = data->zero + data->position + offset;
				break;
			case SEEK_SET :
				temp = data->zero + offset;
				break;
		}
		if (temp > data->zero + (off_t) data->internal_file->uncompressed_filesize) {
			*newoffset = -1;
			return -1;
		}
		if (temp < data->zero) {
			*newoffset = -1;
			return -1;
		}
		res = php_stream_seek(data->fp, temp, SEEK_SET);
		*newoffset = php_stream_tell(data->fp) - data->zero;
		data->position = *newoffset;
		return res;
	}
	if (whence != SEEK_SET) {
		/* use our proxy position, so the relative stuff works */
		php_stream_seek(data->fp, data->position, SEEK_SET);
	}
	/* now do the actual seek */
	res = php_stream_seek(data->fp, offset, whence);
	*newoffset = php_stream_tell(data->fp);
	data->position = *newoffset;
	return res;
}
/* }}} */

/**
 * Used for writing to a phar file
 */
static size_t phar_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *) stream->abstract;

	php_stream_seek(data->fp, data->position, SEEK_SET);
	if (count != php_stream_write(data->fp, buf, count)) {
		php_stream_wrapper_log_error(stream->wrapper, stream->flags TSRMLS_CC, "phar error: Could not write %d characters to \"%s\" in phar \"%s\"", (int) count, data->internal_file->filename, data->phar->fname);
		return -1;
	}
	data->position = php_stream_tell(data->fp);
	if (data->position > (off_t)data->internal_file->uncompressed_filesize) {
		data->internal_file->uncompressed_filesize = data->position;
	}
	data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize;
	data->internal_file->old_flags = data->internal_file->flags;
	data->internal_file->is_modified = 1;
	return count;
}
/* }}} */

/**
 * Dummy: Used for writing to a phar directory (i.e. not used)
 */
static size_t phar_dir_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	return 0;
}
/* }}} */

static inline void phar_set_32(char *buffer, int var) /* {{{ */
{
#ifdef WORDS_BIGENDIAN
	*((buffer) + 3) = (unsigned char) (((var) >> 24) & 0xFF);
	*((buffer) + 2) = (unsigned char) (((var) >> 16) & 0xFF);
	*((buffer) + 1) = (unsigned char) (((var) >> 8) & 0xFF);
	*((buffer) + 0) = (unsigned char) ((var) & 0xFF);
#else
	*(php_uint32 *)(buffer) = (php_uint32)(var);
#endif
} /* }}} */

/**
 * The only purpose of this is to store the API version, which was stored bigendian for some reason
 * in the original PHP_Archive, so we will do the same
 */
static inline void phar_set_16(char *buffer, int var) /* {{{ */
{
#ifdef WORDS_BIGENDIAN
	*((buffer) + 1) = (unsigned char) (((var) >> 8) & 0xFF); \
	*(buffer) = (unsigned char) ((var) & 0xFF);
#else
	*(php_uint16 *)(buffer) = (php_uint16)(var);
#endif
} /* }}} */

static int phar_flush_clean_deleted_apply(void *data TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)data;

	if (entry->fp_refcount <= 0 && entry->is_deleted) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

#if HAVE_PHAR_ZIP
static ssize_t phar_zip_source(void *state, void *data, size_t len, enum zip_source_cmd cmd)
{
	char *buf = (char *) data;
	phar_entry_info *entry = (phar_entry_info *) state;
	size_t read;
	struct zip_stat *sb = (struct zip_stat *) data;
	TSRMLS_FETCH();

	switch (cmd) {
		case ZIP_SOURCE_OPEN :
			php_stream_seek(entry->fp, 0, SEEK_SET);
			return 0;
		case ZIP_SOURCE_READ :
			read = php_stream_read(entry->fp, buf, len);
			if (read < 0) return 0;
			return read;
		case ZIP_SOURCE_STAT :
			zip_stat_init(sb);
			sb->mtime = time(NULL);
			sb->size = entry->uncompressed_filesize;
			return sizeof(struct zip_stat);
		default:
			return len;
	}
}

/* reconstruct the zip index of each manifest entry */
static int phar_zip_reconstruct_apply(void *data TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)data;
	if (entry->is_deleted) {
		entry->index = -1;
		if (entry->fp_refcount <= 0) {
			return ZEND_HASH_APPLY_REMOVE;
		} else {
			/* we can't delete this in-memory until it is closed */
			return ZEND_HASH_APPLY_KEEP;
		}
	}
	entry->index = zip_name_locate(entry->phar->zip, entry->filename, 0);
	return ZEND_HASH_APPLY_KEEP;
}

/* perform final modification of zip contents for each file in the manifest before saving */
static int phar_zip_changed_apply(void *data TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)data;

	if (entry->is_deleted) {
		if (entry->fp_refcount <= 0) {
			return ZEND_HASH_APPLY_REMOVE;
		} else {
			/* we can't delete this in-memory until it is closed */
			return ZEND_HASH_APPLY_KEEP;
		}
	}
	if (entry->is_modified) {
		ssize_t (*cb)(void *state, void *data, size_t len, enum zip_source_cmd cmd) = phar_zip_source;
		if (entry->fp) {
			struct zip_source *s = zip_source_function(entry->phar->zip, cb, entry);
			if (-1 == zip_replace(entry->phar->zip, entry->index, s)) {
				zip_error_clear(entry->phar->zip);
				return ZEND_HASH_APPLY_REMOVE;
			}
		}

		/* set file metadata */
		if (entry->metadata) {
			php_serialize_data_t metadata_hash;
			if (entry->metadata_str.c) {
				smart_str_free(&entry->metadata_str);
			}
			entry->metadata_str.c = 0;
			entry->metadata_str.len = 0;
			PHP_VAR_SERIALIZE_INIT(metadata_hash);
			php_var_serialize(&entry->metadata_str, &entry->metadata, &metadata_hash TSRMLS_CC);
			PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
			if (-1 == zip_set_file_comment(entry->phar->zip, entry->index, entry->metadata_str.c, entry->metadata_str.len)) {
				return ZEND_HASH_APPLY_STOP;
			}
		} else {
			zip_set_file_comment(entry->phar->zip, entry->index, NULL, 0);
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

int phar_zip_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC) /* {{{ */
{
	char *pos;
	smart_str main_metadata_str = {0};
	int ziperrint;
	static const char newstub[] = "<?php // zip-based phar archive stub file\n__HALT_COMPILER();";
	php_stream *stubfile;
	php_serialize_data_t metadata_hash;
	int free_user_stub;
	int phar_stub_index, phar_alias_index;

	/* save modified files to the zip */
	zend_hash_apply(&archive->manifest, phar_zip_changed_apply TSRMLS_CC);
	if (archive->zip->error.str) {
		if (error) {
			spprintf(error, 4096, "phar zip flush of \"%s\" failed: %s", archive->fname, zip_strerror(archive->zip));
		}
		return EOF;
	}

	/* set phar metadata */
	main_metadata_str.c = 0;
	if (archive->metadata) {
		PHP_VAR_SERIALIZE_INIT(metadata_hash);
		php_var_serialize(&main_metadata_str, &archive->metadata, &metadata_hash TSRMLS_CC);
		PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
		zip_set_archive_comment(archive->zip, main_metadata_str.c, main_metadata_str.len);
		smart_str_free(&main_metadata_str);
	} else {
		zip_set_archive_comment(archive->zip, NULL, 0);
	}

	/* set alias */
	if (archive->is_explicit_alias) {
		struct zip_source *source;
		phar_alias_index = zip_name_locate(archive->zip, ".phar/alias.txt", 0);
		if (NULL == (source = zip_source_buffer(archive->zip, archive->alias, archive->alias_len, 0)) ||
				(-1 == phar_alias_index && -1 == zip_add(archive->zip, ".phar/alias.txt", source)) ||
				 (-1 != phar_alias_index && -1 == zip_replace(archive->zip, phar_alias_index, source))) {
			if (error) {
				spprintf(error, 0, "unable to set alias in new zip-based phar \"%s\": %s", archive->fname, zip_strerror(archive->zip));
			}
			zip_error_clear(archive->zip);
			return EOF;
		}
	}
	/* set stub */
	phar_stub_index = zip_name_locate(archive->zip, ".phar/stub.php", 0);
	zip_error_clear(archive->zip);
	if (user_stub) {
		struct zip_source *source;
		if (len < 0) {
			/* resource passed in */
			if (!(php_stream_from_zval_no_verify(stubfile, (zval **)user_stub))) {
				if (error) {
					spprintf(error, 0, "unable to access resource to copy stub to new phar \"%s\"", archive->fname);
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
					spprintf(error, 0, "unable to read resource to copy stub to new phar \"%s\"", archive->fname);
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
				spprintf(error, 0, "illegal stub for phar \"%s\"", archive->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		len = pos - user_stub + 18;
		if (NULL == (source = zip_source_buffer(archive->zip, user_stub, len, 0)) ||
				(-1 == phar_stub_index && -1 == zip_add(archive->zip, ".phar/stub.php", source)) ||
				(-1 != phar_stub_index && -1 == zip_replace(archive->zip, phar_stub_index, source))) {
			if (error) {
				spprintf(error, 0, "unable to create stub from string in new zip-based phar \"%s\": %s", archive->fname, zip_strerror(archive->zip));
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			zip_error_clear(archive->zip);
			return EOF;
		}
		if (free_user_stub) {
			efree(user_stub);
		}
	} else {
		if (-1 != phar_stub_index) {
			struct zip_source *source;
			/* this is a brand new phar, add the stub */
			if (NULL == (source = zip_source_buffer(archive->zip, newstub, sizeof(newstub) - 1, 0)) || -1 == zip_add(archive->zip, ".phar/stub.php", source)) {
				if (error) {
					spprintf(error, 0, "unable to create stub in new zip-based phar \"%s\": %s", archive->fname, zip_strerror(archive->zip));
				}
				zip_error_clear(archive->zip);
				return EOF;
			}
		}
	}

	/* save zip */
	if (-1 == zip_close(archive->zip)) {
		if (error) {
			spprintf(error, 4096, "saving of zip-based phar \"%s\" failed: %s", archive->fname, zip_strerror(archive->zip));
		}
		return EOF;
	}

	/* re-open */
	archive->zip = zip_open(archive->fname, 0, &ziperrint);
	if (!archive->zip) {
		if (error) {
			/* now for the stupid hoops libzip forces... */
			char *tmp;
			int tmp_len;
			tmp_len = zip_error_to_str(NULL, 0, ziperrint, ziperrint);
			if (!(tmp = emalloc((tmp_len + 1) * sizeof(char)))) {
				spprintf(error, 4096, "phar zip error: cannot re-open zip-based phar \"%s\"", archive->fname);
			} else {
				if (!zip_error_to_str(tmp, tmp_len + 1, ziperrint, ziperrint)) {
					spprintf(error, 4096, "phar zip error: cannot re-open zip-based phar \"%s\"", archive->fname);
				} else {
					spprintf(error, 4096, "phar zip error: cannot re-open zip-based phar \"%s\": %s", archive->fname, tmp);
					efree(tmp);
				}
			}
		}
		return EOF;
	}

	/* reconstruct manifest zip index map */
	zend_hash_apply(&archive->manifest, phar_zip_reconstruct_apply TSRMLS_CC);
	return EOF;
}
/* }}} */
#endif /* if HAVE_PHAR_ZIP */

/**
 * Save phar contents to disk
 *
 * user_stub contains either a string, or a resource pointer, if len is a negative length.
 * user_stub and len should be both 0 if the default or existing stub should be used
 */
int phar_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC) /* {{{ */
{
	static const char newstub[] = "<?php __HALT_COMPILER(); ?>\r\n";
	phar_entry_info *entry, *newentry;
	int halt_offset, restore_alias_len, global_flags = 0, closeoldfile;
	char *buf, *pos;
	char manifest[18], entry_buffer[24];
	off_t manifest_ftell;
	long offset;
	size_t wrote;
	php_uint32 manifest_len, mytime, loc, new_manifest_count;
	php_uint32 newcrc32;
	php_stream *file, *oldfile, *newfile, *stubfile;
	php_stream_filter *filter;
	php_serialize_data_t metadata_hash;
	smart_str main_metadata_str = {0};
	int free_user_stub;

	if (error) {
		*error = NULL;
	}

	if (PHAR_G(readonly)) {
		return EOF;
	}

#if HAVE_PHAR_ZIP
	if (archive->is_zip) {
		return phar_zip_flush(archive, user_stub, len, error TSRMLS_CC);
	}
#endif
	if (archive->is_tar) {
		return phar_tar_flush(archive, user_stub, len, error TSRMLS_CC);
	}
	if (archive->fp && !archive->is_brandnew) {
		oldfile = archive->fp;
		closeoldfile = 0;
		php_stream_rewind(oldfile);
	} else {
		oldfile = php_stream_open_wrapper(archive->fname, "rb", 0, NULL);
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

	if (user_stub) {
		if (len < 0) {
			/* resource passed in */
			if (!(php_stream_from_zval_no_verify(stubfile, (zval **)user_stub))) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to access resource to copy stub to new phar \"%s\"", archive->fname);
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
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to read resource to copy stub to new phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			free_user_stub = 1;
		} else {
			free_user_stub = 0;
		}
		if ((pos = strstr(user_stub, "__HALT_COMPILER();")) == NULL)
		{
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "illegal stub for phar \"%s\"", archive->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		len = pos - user_stub + 18;
		if ((size_t)len != php_stream_write(newfile, user_stub, len)
		||            5 != php_stream_write(newfile, " ?>\r\n", 5)) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "unable to create stub from string in new phar \"%s\"", archive->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		archive->halt_offset = len + 5;
		if (free_user_stub) {
			efree(user_stub);
		}
	} else {
		if (archive->halt_offset && oldfile && !archive->is_brandnew) {
			if (archive->halt_offset != php_stream_copy_to_stream(oldfile, newfile, archive->halt_offset)) {	
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to copy stub of old phar to new phar \"%s\"", archive->fname);
				}
				return EOF;
			}
		} else {
			/* this is a brand new phar */
			archive->halt_offset = sizeof(newstub)-1;
			if (sizeof(newstub)-1 != php_stream_write(newfile, newstub, sizeof(newstub)-1)) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to create stub in new phar \"%s\"", archive->fname);
				}
				return EOF;
			}
		}
	}
	manifest_ftell = php_stream_tell(newfile);
	halt_offset = manifest_ftell;

	/* Check whether we can get rid of some of the deleted entries which are
	 * unused. However some might still be in use so even after this clean-up
	 * we need to skip entries marked is_deleted. */
	zend_hash_apply(&archive->manifest, phar_flush_clean_deleted_apply TSRMLS_CC);

	/* compress as necessary, calculate crcs, serialize meta-data, manifest size, and file sizes */
	main_metadata_str.c = 0;
	if (archive->metadata) {
		PHP_VAR_SERIALIZE_INIT(metadata_hash);
		php_var_serialize(&main_metadata_str, &archive->metadata, &metadata_hash TSRMLS_CC);
		PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
	} else {
		main_metadata_str.len = 0;
	}
	new_manifest_count = 0;
	offset = 0;
	buf = (char *) emalloc(8192);
	for (zend_hash_internal_pointer_reset(&archive->manifest);
	    zend_hash_has_more_elements(&archive->manifest) == SUCCESS;
	    zend_hash_move_forward(&archive->manifest)) {
		if (zend_hash_get_current_data(&archive->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->cfp) {
			/* did we forget to get rid of cfp last time? */
			php_stream_close(entry->cfp);
			entry->cfp = 0;
		}
		if (entry->is_deleted) {
			/* remove this from the new phar */
			continue;
		}
		/* after excluding deleted files, calculate manifest size in bytes and number of entries */
		++new_manifest_count;

		if (entry->metadata) {
			if (entry->metadata_str.c) {
				smart_str_free(&entry->metadata_str);
			}
			entry->metadata_str.c = 0;
			entry->metadata_str.len = 0;
			PHP_VAR_SERIALIZE_INIT(metadata_hash);
			php_var_serialize(&entry->metadata_str, &entry->metadata, &metadata_hash TSRMLS_CC);
			PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
		} else {
			entry->metadata_str.c = 0;
			entry->metadata_str.len = 0;
		}

		offset += 4 + entry->filename_len + sizeof(entry_buffer) + entry->metadata_str.len;

		/* compress and rehash as necessary */
		if (oldfile && !entry->is_modified) {
			continue;
		}
		if (!entry->fp || (entry->is_modified && entry->fp == archive->fp)) {
			/* re-open internal file pointer just-in-time */
			newentry = phar_open_jit(archive, entry, oldfile, error, 0 TSRMLS_CC);
			if (!newentry) {
				/* major problem re-opening, so we ignore this file and the error */
				efree(*error);
				*error = NULL;
				continue;
			}
			entry = newentry;
		}
		file = entry->fp;
		if (file == archive->fp) {
			if (-1 == php_stream_seek(file, entry->offset_within_phar + archive->internal_file_start, SEEK_SET)) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, archive->fname);
				}
				return EOF;
			}
		} else {
			php_stream_rewind(file);
		}
		newcrc32 = ~0;
		mytime = entry->uncompressed_filesize;
		for (loc = 0;loc < mytime; loc++) {
			CRC32(newcrc32, php_stream_getc(file));
		}
		entry->crc32 = ~newcrc32;
		entry->is_crc_checked = 1;
		if (!(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
			/* not compressed */
			entry->compressed_filesize = entry->uncompressed_filesize;
			continue;
		}
		filter = php_stream_filter_create(phar_compress_filter(entry, 0), NULL, 0 TSRMLS_CC);
		if (!filter) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (entry->flags & PHAR_ENT_COMPRESSED_GZ) {
				if (error) {
					spprintf(error, 0, "unable to gzip compress file \"%s\" to new phar \"%s\"", entry->filename, archive->fname);
				}
			} else {
				if (error) {
					spprintf(error, 0, "unable to bzip2 compress file \"%s\" to new phar \"%s\"", entry->filename, archive->fname);
				}
			}
			efree(buf);
			return EOF;
		}

		/* create new file that holds the compressed version */
		/* work around inability to specify freedom in write and strictness
		in read count */
		entry->cfp = php_stream_fopen_tmpfile();
		if (!entry->cfp) {
			if (error) {
				spprintf(error, 0, "unable to create temporary file");
			}
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			efree(buf);
			return EOF;
		}
		php_stream_flush(file);
		if (file == archive->fp) {
			if (-1 == php_stream_seek(file, entry->offset_within_phar + archive->internal_file_start, SEEK_SET)) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, archive->fname);
				}
				return EOF;
			}
		} else {
			php_stream_rewind(file);
		}
		php_stream_filter_append((&file->readfilters), filter);
		entry->compressed_filesize = (php_uint32) php_stream_copy_to_stream(file, entry->cfp, entry->uncompressed_filesize+8192);
		php_stream_filter_flush(filter, 1);
		entry->compressed_filesize += (php_uint32) php_stream_copy_to_stream(file, entry->cfp, entry->uncompressed_filesize+8192);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		/* generate crc on compressed file */
		php_stream_rewind(entry->cfp);
		entry->old_flags = entry->flags;
		entry->is_modified = 1;
		global_flags |= (entry->flags & PHAR_ENT_COMPRESSION_MASK);
	}
	efree(buf);
	global_flags |= PHAR_HDR_SIGNATURE;

	/* write out manifest pre-header */
	/*  4: manifest length
	 *  4: manifest entry count
	 *  2: phar version
	 *  4: phar global flags
	 *  4: alias length
	 *  ?: the alias itself
	 *  4: phar metadata length
	 *  ?: phar metadata
	 */
	restore_alias_len = archive->alias_len;
	if (!archive->is_explicit_alias) {
		archive->alias_len = 0;
	}

	manifest_len = offset + archive->alias_len + sizeof(manifest) + main_metadata_str.len;
	phar_set_32(manifest, manifest_len);
	phar_set_32(manifest+4, new_manifest_count);
	*(manifest + 8) = (unsigned char) (((PHAR_API_VERSION) >> 8) & 0xFF);
	*(manifest + 9) = (unsigned char) (((PHAR_API_VERSION) & 0xF0));
	phar_set_32(manifest+10, global_flags);
	phar_set_32(manifest+14, archive->alias_len);

	/* write the manifest header */
	if (sizeof(manifest) != php_stream_write(newfile, manifest, sizeof(manifest))
	|| (size_t)archive->alias_len != php_stream_write(newfile, archive->alias, archive->alias_len)) {
		if (closeoldfile) {
			php_stream_close(oldfile);
		}
		php_stream_close(newfile);
		archive->alias_len = restore_alias_len;
		if (error) {
			spprintf(error, 0, "unable to write manifest header of new phar \"%s\"", archive->fname);
		}
		return EOF;
	}
	
	archive->alias_len = restore_alias_len;
	
	phar_set_32(manifest, main_metadata_str.len);
	if (main_metadata_str.len) {
		if (4 != php_stream_write(newfile, manifest, 4) ||
		main_metadata_str.len != php_stream_write(newfile, main_metadata_str.c, main_metadata_str.len)) {
			smart_str_free(&main_metadata_str);
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			archive->alias_len = restore_alias_len;
			if (error) {
				spprintf(error, 0, "unable to write manifest meta-data of new phar \"%s\"", archive->fname);
			}
			return EOF;
		} 
	} else {
		if (4 != php_stream_write(newfile, manifest, 4)) {
			smart_str_free(&main_metadata_str);
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			archive->alias_len = restore_alias_len;
			if (error) {
				spprintf(error, 0, "unable to write manifest header of new phar \"%s\"", archive->fname);
			}
			return EOF;
		}
	}
	smart_str_free(&main_metadata_str);
	
	/* re-calculate the manifest location to simplify later code */
	manifest_ftell = php_stream_tell(newfile);
	
	/* now write the manifest */
	for (zend_hash_internal_pointer_reset(&archive->manifest);
	    zend_hash_has_more_elements(&archive->manifest) == SUCCESS;
	    zend_hash_move_forward(&archive->manifest)) {
		if (zend_hash_get_current_data(&archive->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->is_deleted) {
			/* remove this from the new phar */
			continue;
		}
		phar_set_32(entry_buffer, entry->filename_len);
		if (4 != php_stream_write(newfile, entry_buffer, 4)
		|| entry->filename_len != php_stream_write(newfile, entry->filename, entry->filename_len)) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "unable to write filename of file \"%s\" to manifest of new phar \"%s\"", entry->filename, archive->fname);
			}
			return EOF;
		}
		/* set the manifest meta-data:
			4: uncompressed filesize
			4: creation timestamp
			4: compressed filesize
			4: crc32
			4: flags
			4: metadata-len
			+: metadata
		*/
		mytime = time(NULL);
		phar_set_32(entry_buffer, entry->uncompressed_filesize);
		phar_set_32(entry_buffer+4, mytime);
		phar_set_32(entry_buffer+8, entry->compressed_filesize);
		phar_set_32(entry_buffer+12, entry->crc32);
		phar_set_32(entry_buffer+16, entry->flags);
		phar_set_32(entry_buffer+20, entry->metadata_str.len);
		if (sizeof(entry_buffer) != php_stream_write(newfile, entry_buffer, sizeof(entry_buffer))
		|| entry->metadata_str.len != php_stream_write(newfile, entry->metadata_str.c, entry->metadata_str.len)) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "unable to write temporary manifest of file \"%s\" to manifest of new phar \"%s\"", entry->filename, archive->fname);
			}
			return EOF;
		}
	}
	
	/* now copy the actual file data to the new phar */
	offset = 0;
	for (zend_hash_internal_pointer_reset(&archive->manifest);
	    zend_hash_has_more_elements(&archive->manifest) == SUCCESS;
	    zend_hash_move_forward(&archive->manifest)) {
		if (zend_hash_get_current_data(&archive->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->is_deleted) {
			continue;
		}
		if (entry->cfp) {
			file = entry->cfp;
			php_stream_rewind(file);
		} else if (entry->fp && (entry->is_modified || !oldfile)) {
			file = entry->fp;
			if (file == archive->fp) {
				if (-1 == php_stream_seek(file, entry->offset_within_phar + archive->internal_file_start, SEEK_SET)) {
					if (closeoldfile) {
						php_stream_close(oldfile);
					}
					php_stream_close(newfile);
					if (error) {
						spprintf(error, 0, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, archive->fname);
					}
					return EOF;
				}
			} else {
				php_stream_rewind(file);
			}
		} else {
			if (!oldfile) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, archive->fname);
				}
				return EOF;
			}
			if (-1 == php_stream_seek(oldfile, entry->offset_within_phar + archive->internal_file_start, SEEK_SET)) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, archive->fname);
				}
				return EOF;
			}
			file = oldfile;
		}
		/* this will have changed for all files that have either
		   changed compression or been modified */
		entry->offset_within_phar = offset;
		offset += entry->compressed_filesize;
		wrote = php_stream_copy_to_stream(file, newfile, entry->compressed_filesize);
		if (entry->compressed_filesize != wrote) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "unable to write contents of file \"%s\" to new phar \"%s\"", entry->filename, archive->fname);
			}
			return EOF;
		}
		entry->is_modified = 0;
		if (entry->cfp) {
			php_stream_close(entry->cfp);
			entry->cfp = 0;
		}
		if (entry->fp && entry->fp_refcount == 0) {
			if (entry->fp != archive->fp) {
				php_stream_close(entry->fp);
			}
			entry->fp = 0;
		}
	}

	/* append signature */
	if (global_flags & PHAR_HDR_SIGNATURE) {
		unsigned char buf[1024];
		int  sig_flags = 0, sig_len;
		char sig_buf[4];

		php_stream_rewind(newfile);
		
		if (archive->signature) {
			efree(archive->signature);
		}
		
		switch(archive->sig_flags) {
#if HAVE_HASH_EXT
		case PHAR_SIG_SHA512: {
			unsigned char digest[64];
			PHP_SHA512_CTX  context;

			PHP_SHA512Init(&context);
			while ((sig_len = php_stream_read(newfile, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA512Update(&context, buf, sig_len);
			}
			PHP_SHA512Final(digest, &context);
			php_stream_write(newfile, (char *) digest, sizeof(digest));
			sig_flags |= PHAR_SIG_SHA512;
			archive->sig_len = phar_hex_str((const char*)digest, sizeof(digest), &archive->signature);
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32];
			PHP_SHA256_CTX  context;

			PHP_SHA256Init(&context);
			while ((sig_len = php_stream_read(newfile, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA256Update(&context, buf, sig_len);
			}
			PHP_SHA256Final(digest, &context);
			php_stream_write(newfile, (char *) digest, sizeof(digest));
			sig_flags |= PHAR_SIG_SHA256;
			archive->sig_len = phar_hex_str((const char*)digest, sizeof(digest), &archive->signature);
			break;
		}
#else
		case PHAR_SIG_SHA512:
		case PHAR_SIG_SHA256:
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "unable to write contents of file \"%s\" to new phar \"%s\" with requested hash type", entry->filename, archive->fname);
			}
			return EOF;
#endif
		case PHAR_SIG_PGP:
			/* TODO: currently fall back to sha1,later do both */
		default:
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			PHP_SHA1Init(&context);
			while ((sig_len = php_stream_read(newfile, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA1Update(&context, buf, sig_len);
			}
			PHP_SHA1Final(digest, &context);
			php_stream_write(newfile, (char *) digest, sizeof(digest));
			sig_flags |= PHAR_SIG_SHA1;
			archive->sig_len = phar_hex_str((const char*)digest, sizeof(digest), &archive->signature);
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			PHP_MD5Init(&context);
			while ((sig_len = php_stream_read(newfile, (char*)buf, sizeof(buf))) > 0) {
				PHP_MD5Update(&context, buf, sig_len);
			}
			PHP_MD5Final(digest, &context);
			php_stream_write(newfile, (char *) digest, sizeof(digest));
			sig_flags |= PHAR_SIG_MD5;
			archive->sig_len = phar_hex_str((const char*)digest, sizeof(digest), &archive->signature);
			break;
		}
		}
		phar_set_32(sig_buf, sig_flags);
		php_stream_write(newfile, sig_buf, 4);
		php_stream_write(newfile, "GBMB", 4);
		archive->sig_flags = sig_flags;
	}

	/* finally, close the temp file, rename the original phar,
	   move the temp to the old phar, unlink the old phar, and reload it into memory
	*/
	if (archive->fp) {
		php_stream_close(archive->fp);
	}
	if (closeoldfile) {
		php_stream_close(oldfile);
	}

	archive->internal_file_start = halt_offset + manifest_len + 4;
	archive->is_brandnew = 0;

	php_stream_rewind(newfile);

	if (archive->donotflush) {
		/* deferred flush */
		archive->fp = newfile;
	} else {
		archive->fp = php_stream_open_wrapper(archive->fname, "w+b", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
		if (!archive->fp) {
			archive->fp = newfile;
			if (error) {
				spprintf(error, 0, "unable to open new phar \"%s\" for writing", archive->fname);
			}
			return EOF;
		}
		php_stream_copy_to_stream(newfile, archive->fp, PHP_STREAM_COPY_ALL);
		php_stream_close(newfile);
		/* we could also reopen the file in "rb" mode but there is no need for that */
	}

	if (-1 == php_stream_seek(archive->fp, archive->halt_offset, SEEK_SET)) {
		if (error) {
			spprintf(error, 0, "unable to seek to __HALT_COMPILER(); in new phar \"%s\"", archive->fname);
		}
		return EOF;
	}

	return EOF;
}
/* }}} */

/**
 * Used to save work done on a writeable phar
 */
static int phar_stream_flush(php_stream *stream TSRMLS_DC) /* {{{ */
{
	char *error;
	int ret;
	if (stream->mode[0] == 'w' || (stream->mode[0] == 'r' && stream->mode[1] == '+')) {
		ret = phar_flush(((phar_entry_data *)stream->abstract)->internal_file->phar, 0, 0, &error TSRMLS_CC);
		if (error) {
			php_stream_wrapper_log_error(stream->wrapper, REPORT_ERRORS TSRMLS_CC, error);
			efree(error);
		}
		return ret;
	} else {
		return EOF;
	}
}
/* }}} */

/**
 * Dummy: Used for flushing writes to a phar directory (i.e. not used)
 */
static int phar_dir_flush(php_stream *stream TSRMLS_DC) /* {{{ */
{
	return EOF;
}
/* }}} */

 /* {{{ phar_dostat */
/**
 * stat an opened phar file handle stream, used by phar_stat()
 */
static void phar_dostat(phar_archive_data *phar, phar_entry_info *data, php_stream_statbuf *ssb, 
			zend_bool is_dir, char *alias, int alias_len TSRMLS_DC)
{
	char *tmp;
	int tmp_len;
	memset(ssb, 0, sizeof(php_stream_statbuf));

	if (!is_dir && !data->is_dir) {
		ssb->sb.st_size = data->uncompressed_filesize;
		ssb->sb.st_mode = data->flags & PHAR_ENT_PERM_MASK;
		ssb->sb.st_mode |= S_IFREG; /* regular file */
		/* timestamp is just the timestamp when this was added to the phar */
#ifdef NETWARE
		ssb->sb.st_mtime.tv_sec = data->timestamp;
		ssb->sb.st_atime.tv_sec = data->timestamp;
		ssb->sb.st_ctime.tv_sec = data->timestamp;
#else
		ssb->sb.st_mtime = data->timestamp;
		ssb->sb.st_atime = data->timestamp;
		ssb->sb.st_ctime = data->timestamp;
#endif
	} else if (!is_dir && data->is_dir && (data->is_tar || data->is_zip)) {
		ssb->sb.st_size = 0;
		ssb->sb.st_mode = data->flags & PHAR_ENT_PERM_MASK;
		ssb->sb.st_mode |= S_IFDIR; /* regular directory */
		/* timestamp is just the timestamp when this was added to the phar */
#ifdef NETWARE
		ssb->sb.st_mtime.tv_sec = data->timestamp;
		ssb->sb.st_atime.tv_sec = data->timestamp;
		ssb->sb.st_ctime.tv_sec = data->timestamp;
#else
		ssb->sb.st_mtime = data->timestamp;
		ssb->sb.st_atime = data->timestamp;
		ssb->sb.st_ctime = data->timestamp;
#endif
	} else {
		ssb->sb.st_size = 0;
		ssb->sb.st_mode = 0777;
		ssb->sb.st_mode |= S_IFDIR; /* regular directory */
#ifdef NETWARE
		ssb->sb.st_mtime.tv_sec = phar->max_timestamp;
		ssb->sb.st_atime.tv_sec = phar->max_timestamp;
		ssb->sb.st_ctime.tv_sec = phar->max_timestamp;
#else
		ssb->sb.st_mtime = phar->max_timestamp;
		ssb->sb.st_atime = phar->max_timestamp;
		ssb->sb.st_ctime = phar->max_timestamp;
#endif
	}
	if (!phar->is_writeable) {
		ssb->sb.st_mode = (ssb->sb.st_mode & 0555) | (ssb->sb.st_mode & ~0777);
	}

	ssb->sb.st_nlink = 1;
	ssb->sb.st_rdev = -1;
	if (data) {
		tmp_len = data->filename_len + alias_len;
	} else {
		tmp_len = alias_len + 1;
	}
	tmp = (char *) emalloc(tmp_len);
	memcpy(tmp, alias, alias_len);
	if (data) {
		memcpy(tmp + alias_len, data->filename, data->filename_len);
	} else {
		*(tmp+alias_len) = '/';
	}
	/* this is only for APC, so use /dev/null device - no chance of conflict there! */
	ssb->sb.st_dev = 0xc;
	/* generate unique inode number for alias/filename, so no phars will conflict */
	ssb->sb.st_ino = (unsigned short)zend_get_hash_value(tmp, tmp_len);
	efree(tmp);
#ifndef PHP_WIN32
	ssb->sb.st_blksize = -1;
	ssb->sb.st_blocks = -1;
#endif
}
/* }}}*/

/**
 * Stat an opened phar file handle
 */
static int phar_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) {
		return -1;
	}

	phar_dostat(data->phar, data->internal_file, ssb, 0, data->phar->alias, data->phar->alias_len TSRMLS_CC);
	return 0;
}
/* }}} */

/**
 * Stat a dir in a phar
 */
static int phar_dir_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) {
		return -1;
	}

	phar_dostat(data->phar, data->internal_file, ssb, 0, data->phar->alias, data->phar->alias_len TSRMLS_CC);
	return 0;
}
/* }}} */

/**
 * Stream wrapper stat implementation of stat()
 */
static int phar_wrapper_stat(php_stream_wrapper *wrapper, char *url, int flags,
				  php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	char *internal_file, *key, *error, *plain_map;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry;
	uint host_len;
	int retval;

	if ((resource = phar_open_url(wrapper, url, "r", flags TSRMLS_CC)) == NULL) {
		return -1;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, flags TSRMLS_CC, "phar error: invalid url \"%s\"", url);
		return -1;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, flags TSRMLS_CC, "phar error: not a phar url \"%s\"", url);
		return -1;
	}

	host_len = strlen(resource->host);
	phar_request_initialize(TSRMLS_C);
	if (zend_hash_find(&(PHAR_GLOBALS->phar_plain_map), resource->host, host_len+1, (void **)&plain_map) == SUCCESS) {
		spprintf(&internal_file, 0, "%s%s", plain_map, resource->path);
		retval = php_stream_stat_path_ex(internal_file, flags, ssb, context);
  		if (retval == -1) {
			php_stream_wrapper_log_error(wrapper, 0/* TODO:options */ TSRMLS_CC, "phar error: file \"%s\" extracted from \"%s\" could not be opened", internal_file, resource->host);
		}
		php_url_free(resource);
		efree(internal_file);
		return retval;
	}

	internal_file = resource->path + 1; /* strip leading "/" */
	/* find the phar in our trusty global hash indexed by alias (host of phar://blah.phar/file.whatever) */
	if (FAILURE == phar_get_archive(&phar, resource->host, strlen(resource->host), NULL, 0, &error TSRMLS_CC)) {
		php_url_free(resource);
		if (error) {
			php_stream_wrapper_log_error(wrapper, flags TSRMLS_CC, error);
			efree(error);
		}
		return 0;
	}
	if (error) {
		efree(error);
	}
	if (*internal_file == '\0') {
		/* root directory requested */
		phar_dostat(phar, NULL, ssb, 1, phar->alias, phar->alias_len TSRMLS_CC);
		php_url_free(resource);
		return 0;
	}
	if (!phar->manifest.arBuckets) {
		php_url_free(resource);
		return 0;
	}
	/* search through the manifest of files, and if we have an exact match, it's a file */
	if (SUCCESS == zend_hash_find(&phar->manifest, internal_file, strlen(internal_file), (void**)&entry)) {
		phar_dostat(phar, entry, ssb, 0, phar->alias, phar->alias_len TSRMLS_CC);
	} else {
		/* search for directory (partial match of a file) */
		zend_hash_internal_pointer_reset(&phar->manifest);
		while (FAILURE != zend_hash_has_more_elements(&phar->manifest)) {
			if (HASH_KEY_NON_EXISTANT !=
					zend_hash_get_current_key_ex(
						&phar->manifest, &key, &keylen, &unused, 0, NULL)) {
				if (0 == memcmp(internal_file, key, strlen(internal_file))) {
					/* directory found, all dirs have the same stat */
					if (key[strlen(internal_file)] == '/') {
						phar_dostat(phar, NULL, ssb, 1, phar->alias, phar->alias_len TSRMLS_CC);
						break;
					}
				}
			}
			if (SUCCESS != zend_hash_move_forward(&phar->manifest)) {
				break;
			}
		}
	}

	php_url_free(resource);
	return 0;
}
/* }}} */

/**
 * add an empty element with a char * key to a hash table, avoiding duplicates
 *
 * This is used to get a unique listing of virtual directories within a phar,
 * for iterating over opendir()ed phar directories.
 */
static int phar_add_empty(HashTable *ht, char *arKey, uint nKeyLength)  /* {{{ */
{
	void *dummy = (void *) 1;

	return zend_hash_update(ht, arKey, nKeyLength, &dummy, sizeof(void *), NULL);
}
/* }}} */

/**
 * Used for sorting directories alphabetically
 */
static int phar_compare_dir_name(const void *a, const void *b TSRMLS_DC)  /* {{{ */
{
	Bucket *f;
	Bucket *s;
	int result;
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);

#if (PHP_MAJOR_VERSION < 6)
	result = zend_binary_strcmp(f->arKey, f->nKeyLength, s->arKey, s->nKeyLength);
#else
	result = zend_binary_strcmp(f->key.arKey.s, f->nKeyLength, s->key.arKey.s, s->nKeyLength);
#endif

	if (result < 0) {
		return -1;
	} else if (result > 0) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

/**
 * Create a opendir() directory stream handle by iterating over each of the
 * files in a phar and retrieving its relative path.  From this, construct
 * a list of files/directories that are "in" the directory represented by dir
 */
static php_stream *phar_make_dirstream(char *dir, HashTable *manifest TSRMLS_DC) /* {{{ */
{
	HashTable *data;
	int dirlen = strlen(dir);
	char *save, *found, *key;
	uint keylen;
	ulong unused;
	char *entry;
	ALLOC_HASHTABLE(data);
	zend_hash_init(data, 64, zend_get_hash_value, NULL, 0);

	if (*dir == '/' && dirlen == 1 && (manifest->nNumOfElements == 0)) {
		/* make empty root directory for empty phar */
		efree(dir);
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	}
	zend_hash_internal_pointer_reset(manifest);
	while (FAILURE != zend_hash_has_more_elements(manifest)) {
		if (HASH_KEY_NON_EXISTANT == zend_hash_get_current_key_ex(manifest, &key, &keylen, &unused, 0, NULL)) {
			break;
		}
		if (*dir == '/') {
			/* root directory */
			if (NULL != (found = (char *) memchr(key, '/', keylen))) {
				/* the entry has a path separator and is a subdirectory */
				entry = (char *) safe_emalloc(found - key, 1, 1);
				memcpy(entry, key, found - key);
				keylen = found - key;
				entry[keylen] = '\0';
			} else {
				entry = (char *) safe_emalloc(keylen, 1, 1);
				memcpy(entry, key, keylen);
				entry[keylen] = '\0';
			}
			goto PHAR_ADD_ENTRY;
		} else {
			if (0 != memcmp(key, dir, dirlen)) {
				/* entry in directory not found */
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			} else {
				if (key[dirlen] != '/') {
					if (SUCCESS != zend_hash_move_forward(manifest)) {
						break;
					}
					continue;
				}
			}
		}
		save = key;
		save += dirlen + 1; /* seek to just past the path separator */
		if (NULL != (found = (char *) memchr(save, '/', keylen - dirlen - 1))) {
			/* is subdirectory */
			save -= dirlen + 1;
			entry = (char *) safe_emalloc(found - save + dirlen, 1, 1);
			memcpy(entry, save + dirlen + 1, found - save - dirlen - 1);
			keylen = found - save - dirlen - 1;
			entry[keylen] = '\0';
		} else {
			/* is file */
			save -= dirlen + 1;
			entry = (char *) safe_emalloc(keylen - dirlen, 1, 1);
			memcpy(entry, save + dirlen + 1, keylen - dirlen - 1);
			entry[keylen - dirlen - 1] = '\0';
			keylen = keylen - dirlen - 1;
		}
PHAR_ADD_ENTRY:
		phar_add_empty(data, entry, keylen);
		efree(entry);
		if (SUCCESS != zend_hash_move_forward(manifest)) {
			break;
		}
	}
	if (FAILURE != zend_hash_has_more_elements(data)) {
		efree(dir);
		if (zend_hash_sort(data, zend_qsort, phar_compare_dir_name, 0 TSRMLS_CC) == FAILURE) {
			FREE_HASHTABLE(data);
			return NULL;
		}
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	} else {
		efree(dir);
		FREE_HASHTABLE(data);
		return NULL;
	}
}
/* }}}*/

/**
 * Unlink a file within a phar archive
 */
static int phar_wrapper_unlink(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	php_url *resource;
	char *internal_file, *error, *plain_map;
	phar_entry_data *idata;
	uint host_len;
	int retval;

	if ((resource = phar_open_url(wrapper, url, "rb", options TSRMLS_CC)) == NULL) {
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url);
		return 0;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url);
		return 0;
	}

	host_len = strlen(resource->host);
	phar_request_initialize(TSRMLS_C);
	if (zend_hash_find(&(PHAR_GLOBALS->phar_plain_map), resource->host, host_len+1, (void **)&plain_map) == SUCCESS) {
		spprintf(&internal_file, 0, "%s%s", plain_map, resource->path);
		retval = php_stream_unlink(internal_file, options, context);
		if (!retval) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" extracted from \"%s\" could not be opened", internal_file, resource->host);
		}
		php_url_free(resource);
		efree(internal_file);
		return retval;
	}

	if (PHAR_G(readonly)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: write operations disabled by INI setting");
		return 0;
	}

	/* need to copy to strip leading "/", will get touched again */
	internal_file = estrdup(resource->path + 1);
	if (FAILURE == phar_get_entry_data(&idata, resource->host, strlen(resource->host), internal_file, strlen(internal_file), "r", &error TSRMLS_CC)) {
		/* constraints of fp refcount were not met */
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
			efree(error);
		}
		efree(internal_file);
		php_url_free(resource);
		return 0;
	}
	if (error) {
		efree(error);
	}
	if (!idata) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" is not a file in phar \"%s\", cannot unlink", internal_file, resource->host);
		efree(internal_file);
		php_url_free(resource);
		return 0;
	}
	if (idata->internal_file->fp_refcount > 1) {
		/* more than just our fp resource is open for this file */ 
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" in phar \"%s\", has open file pointers, cannot unlink", internal_file, resource->host);
		efree(internal_file);
		php_url_free(resource);
		phar_entry_delref(idata TSRMLS_CC);
		return 0;
	}
	php_url_free(resource);
	efree(internal_file);
	phar_entry_remove(idata, &error TSRMLS_CC);
	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
		efree(error);
	}
	return 1;
}
/* }}} */

static int phar_wrapper_rename(php_stream_wrapper *wrapper, char *url_from, char *url_to, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	php_url *resource_from, *resource_to;
	char *from_file, *to_file, *error, *plain_map;
	phar_entry_data *fromdata, *todata;
	uint host_len;

	if (PHAR_G(readonly)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: write operations disabled by INI setting");
		return 0;
	}

	if ((resource_from = phar_open_url(wrapper, url_from, "r+b", options TSRMLS_CC)) == NULL) {
		return 0;
	}
	
	if ((resource_to = phar_open_url(wrapper, url_to, "wb", options TSRMLS_CC)) == NULL) {
		php_url_free(resource_from);
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource_from->scheme || !resource_from->host || !resource_from->path) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url_from);
		return 0;
	}
	
	if (!resource_to->scheme || !resource_to->host || !resource_to->path) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url_to);
		return 0;
	}

	if (strcasecmp("phar", resource_from->scheme)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url_from);
		return 0;
	}

	if (strcasecmp("phar", resource_to->scheme)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url_to);
		return 0;
	}

	if (strcmp(resource_from->host, resource_to->host)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot rename \"%s\" to \"%s\", not within the same phar archive", url_from, url_to);
		return 0;
	}

	host_len = strlen(resource_from->host);
	phar_request_initialize(TSRMLS_C);
	if (zend_hash_find(&(PHAR_GLOBALS->phar_plain_map), resource_from->host, host_len+1, (void **)&plain_map) == SUCCESS) {
		/*TODO:use php_stream_rename() once available*/
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot rename \"%s\" to \"%s\" from extracted phar archive", url_from, url_to);
		return 0;
	}

	/* need to copy to strip leading "/", will get touched again */
	from_file = estrdup(resource_from->path + 1);
	to_file = estrdup(resource_to->path + 1);
	if (FAILURE == phar_get_entry_data(&fromdata, resource_from->host, strlen(resource_from->host), from_file, strlen(from_file), "r", &error TSRMLS_CC)) {
		/* constraints of fp refcount were not met */
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
			efree(error);
		}
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		return 0;
	}
	if (error) {
		efree(error);
	}
	if (!fromdata) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" is not a file in phar \"%s\", cannot rename", from_file, resource_from->host);
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		return 0;
	}
	if (!(todata = phar_get_or_create_entry_data(resource_to->host, strlen(resource_to->host), to_file, strlen(to_file), "w", &error TSRMLS_CC))) {
		/* constraints of fp refcount were not met */
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
			efree(error);
		}
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		return 0;
	}
	if (error) {
		efree(error);
	}
	if (fromdata->internal_file->fp_refcount > 1) {
		/* more than just our fp resource is open for this file */ 
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" in phar \"%s\", has open file pointers, cannot rename", from_file, resource_from->host);
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		phar_entry_delref(fromdata TSRMLS_CC);
		phar_entry_delref(todata TSRMLS_CC);
		return 0;
	}
	if (todata->internal_file->fp_refcount > 1) {
		/* more than just our fp resource is open for this file */ 
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" in phar \"%s\", has open file pointers, cannot rename", to_file, resource_to->host);
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		phar_entry_delref(fromdata TSRMLS_CC);
		phar_entry_delref(todata TSRMLS_CC);
		return 0;
	}

	php_stream_seek(fromdata->internal_file->fp, 0 + fromdata->zero, SEEK_SET);
	if (fromdata->internal_file->uncompressed_filesize != php_stream_copy_to_stream(fromdata->internal_file->fp, todata->internal_file->fp, PHP_STREAM_COPY_ALL)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: rename failed \"%s\" to \"%s\"", url_from, url_to);
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		phar_entry_delref(fromdata TSRMLS_CC);
		phar_entry_delref(todata TSRMLS_CC);
		return 0;
	}
	todata->internal_file->uncompressed_filesize = todata->internal_file->compressed_filesize = fromdata->internal_file->uncompressed_filesize;
	phar_entry_delref(fromdata TSRMLS_CC);
	phar_entry_delref(todata TSRMLS_CC);
	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
		efree(error);
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		return 0;
	}
	efree(from_file);
	efree(to_file);
	php_url_free(resource_from);
	php_url_free(resource_to);
	phar_wrapper_unlink(wrapper, url_from, 0, 0 TSRMLS_CC);
	return 1;
}
/* }}} */

/**
 * Open a directory handle within a phar archive
 */
static php_stream *phar_wrapper_open_dir(php_stream_wrapper *wrapper, char *path, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *key, *error, *plain_map;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry;
	uint host_len;

	if ((resource = phar_open_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/ */
	if (!resource->scheme || !resource->host || !resource->path) {
		if (resource->host && !resource->path) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory (always use full path to a new phar)", path, resource->host);
			php_url_free(resource);
			return NULL;
		}
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\", must have at least phar://%s/", path, path);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar url \"%s\"", path);
		return NULL;
	}

	host_len = strlen(resource->host);
	phar_request_initialize(TSRMLS_C);
	if (zend_hash_find(&(PHAR_GLOBALS->phar_plain_map), resource->host, host_len+1, (void **)&plain_map) == SUCCESS) {
		spprintf(&internal_file, 0, "%s%s", plain_map, resource->path);
		ret = php_stream_opendir(internal_file, options, context);
		if (!ret) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" extracted from \"%s\" could not be opened", internal_file, resource->host);
		}
		php_url_free(resource);
		efree(internal_file);
		return ret;
	}

	internal_file = resource->path + 1; /* strip leading "/" */
	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
			efree(error);
		}
		php_url_free(resource);
		return NULL;
	}
	if (error) {
		efree(error);
	}
	if (*internal_file == '\0') {
		/* root directory requested */
		internal_file = estrndup(internal_file - 1, 1);
		ret = phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
		php_url_free(resource);
		return ret;
	}
	if (!phar->manifest.arBuckets) {
		php_url_free(resource);
		return NULL;
	}
	if (SUCCESS == zend_hash_find(&phar->manifest, internal_file, strlen(internal_file), (void**)&entry)) {
		php_url_free(resource);
		return NULL;
	} else {
		/* search for directory */
		zend_hash_internal_pointer_reset(&phar->manifest);
		while (FAILURE != zend_hash_has_more_elements(&phar->manifest)) {
			if (HASH_KEY_NON_EXISTANT != 
					zend_hash_get_current_key_ex(
						&phar->manifest, &key, &keylen, &unused, 0, NULL)) {
				if (0 == memcmp(key, internal_file, strlen(internal_file))) {
					/* directory found */
					internal_file = estrndup(internal_file,
							strlen(internal_file));
					php_url_free(resource);
					return phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
				}
			}
			if (SUCCESS != zend_hash_move_forward(&phar->manifest)) {
				break;
			}
		}
	}

	php_url_free(resource);
	return NULL;
}
/* }}} */

#ifdef COMPILE_DL_PHAR
ZEND_GET_MODULE(phar)
#endif

/* {{{ phar_functions[]
 *
 * Every user visible function must have an entry in phar_functions[].
 */
function_entry phar_functions[] = {
       {NULL, NULL, NULL}      /* Must be the last line in phar_functions[] */
};
/* }}}*/

/* {{{ php_phar_init_globals
 */
static void php_phar_init_globals_module(zend_phar_globals *phar_globals)
{
	memset(phar_globals, 0, sizeof(zend_phar_globals));
	phar_globals->readonly = 1;
}
/* }}} */

static zend_op_array *phar_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC) /* {{{ */
{
	zend_op_array *res;
	char *name = NULL;
	char *fname = NULL;
	int fname_len, failed;
	zend_op_array *(*save)(zend_file_handle *file_handle, int type TSRMLS_DC);
	phar_archive_data *phar;

	save = zend_compile_file; /* restore current handler or we cause trouble */
	zend_compile_file = phar_orig_compile_file;

	fname = zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);
	if (strstr(file_handle->filename, ".phar") && !strstr(file_handle->filename, ":\\")) {
		if (SUCCESS == phar_open_filename(file_handle->filename, strlen(file_handle->filename), NULL, 0, 0, &phar, NULL TSRMLS_CC)) {
			if (phar->is_zip || phar->is_tar) {
				/* zip-based phar */
				spprintf(&name, 4096, "phar://%s/%s", file_handle->filename, ".phar/stub.php");
				file_handle->type = ZEND_HANDLE_FILENAME;
				file_handle->free_filename = 1;
				file_handle->filename = name;
				if (file_handle->opened_path) {
					efree(file_handle->opened_path);
				}
				goto skip_phar;
			}
		}
	}
	if (zend_hash_num_elements(&(PHAR_GLOBALS->phar_fname_map))) {
		char *arch, *entry;
		int arch_len, entry_len;
		if (strncasecmp(fname, "phar://", 7)) {
			goto skip_phar;
		}
		fname_len = strlen(fname);
		if (SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len TSRMLS_CC)) {
			efree(entry);
			entry = file_handle->filename;
			/* include within phar, if :// is not in the url, then prepend phar://<archive>/ */
			if (strstr(entry, "://")) {
				efree(arch);
				goto skip_phar;
			}
			entry_len = strlen(entry);
			/* auto-convert to phar:// */
			spprintf(&name, 4096, "phar://%s/%s", arch, entry);
			efree(arch);
			file_handle->type = ZEND_HANDLE_FILENAME;
			file_handle->free_filename = 1;
			file_handle->filename = name;
		}
	}
skip_phar:
	zend_try {
		failed = 0;
		res = zend_compile_file(file_handle, type TSRMLS_CC);
	} zend_catch {
		failed = 1;
	} zend_end_try();
	if (name) {
		efree(name);
	}
	zend_compile_file = save;
	if (failed) {
		zend_bailout();
	}
	return res;
}
/* }}} */

static void phar_fopen(INTERNAL_FUNCTION_PARAMETERS)
{
	char *filename, *mode;
	int filename_len, mode_len;
	zend_bool use_include_path = 0;
	zval *zcontext = NULL;
	php_stream *stream;

	if (ht < 3 || !zend_hash_num_elements(&(PHAR_GLOBALS->phar_fname_map))) {
		/* no need to check, include_path not even specified in fopen/ no active phars */
		goto skip_phar;
	}
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ss|br", &filename, &filename_len, &mode, &mode_len, &use_include_path, &zcontext) == FAILURE || !use_include_path) {
		goto skip_phar;
	}
	{
		char *arch, *entry, *fname;
		int arch_len, entry_len, fname_len;
		php_stream_context *context = NULL;
		fname = zend_get_executed_filename(TSRMLS_C);
		if (strncasecmp(fname, "phar://", 7)) {
			goto skip_phar;
		}
		fname_len = strlen(fname);
		if (SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len TSRMLS_CC)) {
			char *s, *name;

			efree(entry);
			entry = filename;
			/* fopen within phar, if :// is not in the url, then prepend phar://<archive>/ */
			entry_len = filename_len;
			for (s = entry; s < (entry + entry_len - 4); s++) {
				if (*s == ':' && *(s + 1) == '/' && *(s + 2) == '/') {
					efree(arch);
					goto skip_phar;
				}
			}
			/* auto-convert to phar:// */
			spprintf(&name, 4096, "phar://%s/%s", arch, entry);
			efree(arch);
			context = php_stream_context_from_zval(zcontext, 0);
			stream = php_stream_open_wrapper_ex(name, mode, 0 | REPORT_ERRORS, NULL, context);
			efree(name);
			php_stream_to_zval(stream, return_value);
			return;
		}
	}
skip_phar:
	PHAR_G(orig_fopen)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	return;
}

PHP_MINIT_FUNCTION(phar) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(phar, php_phar_init_globals_module, NULL);
	REGISTER_INI_ENTRIES();

	PHAR_G(orig_fopen) = NULL;
	phar_has_gnupg = zend_hash_exists(&module_registry, "gnupg", sizeof("gnupg"));
	phar_has_bz2 = zend_hash_exists(&module_registry, "bz2", sizeof("bz2"));
	phar_has_zlib = zend_hash_exists(&module_registry, "zlib", sizeof("zlib"));

	phar_orig_compile_file = zend_compile_file;
	zend_compile_file = phar_compile_file;

	phar_object_init(TSRMLS_C);

	return php_register_url_stream_wrapper("phar", &php_stream_phar_wrapper TSRMLS_CC);
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(phar) /* {{{ */
{
	return php_unregister_url_stream_wrapper("phar" TSRMLS_CC);
	if (zend_compile_file == phar_compile_file) {
		zend_compile_file = phar_orig_compile_file;
	}
}
/* }}} */

void phar_request_initialize(TSRMLS_D) /* {{{ */
{
	if (!PHAR_GLOBALS->request_init)
	{
		zend_function *orig;
		PHAR_GLOBALS->request_init = 1;
		PHAR_GLOBALS->request_ends = 0;
		PHAR_GLOBALS->request_done = 0;
		zend_hash_init(&(PHAR_GLOBALS->phar_fname_map), sizeof(phar_archive_data*), zend_get_hash_value, destroy_phar_data,  0);
		zend_hash_init(&(PHAR_GLOBALS->phar_alias_map), sizeof(phar_archive_data*), zend_get_hash_value, NULL, 0);
		zend_hash_init(&(PHAR_GLOBALS->phar_plain_map), sizeof(const char *),       zend_get_hash_value, NULL, 0);
		zend_hash_init(&(PHAR_GLOBALS->phar_SERVER_mung_list), sizeof(const char *),       zend_get_hash_value, NULL, 0);
		phar_split_extract_list(TSRMLS_C);
		if (SUCCESS == zend_hash_find(CG(function_table), "fopen", 6, (void **)&orig)) {
			PHAR_G(orig_fopen) = orig->internal_function.handler;
			orig->internal_function.handler = phar_fopen;
		}
	}
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(phar) /* {{{ */
{
	PHAR_GLOBALS->request_ends = 1;
	if (PHAR_GLOBALS->request_init)
	{
		zend_function *orig;
		if (PHAR_G(orig_fopen) && SUCCESS == zend_hash_find(CG(function_table), "fopen", 6, (void **)&orig)) {
			orig->internal_function.handler = PHAR_G(orig_fopen);
		}
		zend_hash_destroy(&(PHAR_GLOBALS->phar_alias_map));
		PHAR_GLOBALS->phar_alias_map.arBuckets = NULL;
		zend_hash_destroy(&(PHAR_GLOBALS->phar_fname_map));
		PHAR_GLOBALS->phar_fname_map.arBuckets = NULL;
		zend_hash_destroy(&(PHAR_GLOBALS->phar_plain_map));
		PHAR_GLOBALS->phar_plain_map.arBuckets = NULL;
		zend_hash_destroy(&(PHAR_GLOBALS->phar_SERVER_mung_list));
		PHAR_GLOBALS->phar_SERVER_mung_list.arBuckets = NULL;
		PHAR_GLOBALS->request_init = 0;
	}
	PHAR_GLOBALS->request_done = 1;
	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(phar) /* {{{ */
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Phar: PHP Archive support", "enabled");
	php_info_print_table_row(2, "Phar EXT version", PHAR_EXT_VERSION_STR);
	php_info_print_table_row(2, "Phar API version", PHAR_API_VERSION_STR);
	php_info_print_table_row(2, "CVS revision", "$Revision$");
	php_info_print_table_row(2, "Phar-based phar archives", "enabled");
	php_info_print_table_row(2, "Tar-based phar archives", "enabled");
#if HAVE_PHAR_ZIP
	php_info_print_table_row(2, "ZIP-based phar archives", "enabled");
#else
	php_info_print_table_row(2, "ZIP-based phar archives", "unavailable");
#endif
#if HAVE_ZLIB
	if (phar_has_zlib) {
		php_info_print_table_row(2, "gzip compression", "enabled");
	} else {
		php_info_print_table_row(2, "gzip compression", "disabled");
	}
#else
	php_info_print_table_row(2, "gzip compression", "unavailable");
#endif
#if HAVE_BZ2
	if (phar_has_bz2) {
		php_info_print_table_row(2, "bzip2 compression", "enabled");
	} else {
		php_info_print_table_row(2, "bzip2 compression", "disabled (install pecl/bz2)");
	}
#else
	php_info_print_table_row(2, "bzip2 compression", "unavailable (install pecl/bz2)");
#endif
#if HAVE_GNUPGLIB
  if (phar_has_gnupg) {
		php_info_print_table_row(2, "GPG signature", "enabled");
	} else {
		php_info_print_table_row(2, "GPG signature", "disabled (install pecl/gnupg)");
	}
#else
	php_info_print_table_row(2, "GPG signature", "unavailable (install pecl/gnupg)");
#endif
	php_info_print_table_end();

	php_info_print_box_start(0);
	PUTS("Phar based on pear/PHP_Archive, original concept by Davey Shafik.");
	PUTS(!sapi_module.phpinfo_as_text?"<br />":"\n");	
	PUTS("Phar fully realized by Gregory Beaver and Marcus Boerger.");
	php_info_print_box_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ phar_module_entry
 */
static zend_module_dep phar_deps[] = {
#if HAVE_ZLIB
	ZEND_MOD_OPTIONAL("zlib")
#endif
#if HAVE_BZ2
	ZEND_MOD_OPTIONAL("bz2")
#endif
#if HAVE_GNUPGLIB
	ZEND_MOD_OPTIONAL("gnupg")
#endif
#if HAVE_SPL
	ZEND_MOD_REQUIRED("spl")
#endif
	{NULL, NULL, NULL}
};

zend_module_entry phar_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	phar_deps,
	"Phar",
	phar_functions,
	PHP_MINIT(phar),
	PHP_MSHUTDOWN(phar),
	NULL,
	PHP_RSHUTDOWN(phar),
	PHP_MINFO(phar),
	PHAR_EXT_VERSION_STR,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
