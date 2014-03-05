/*
  +----------------------------------------------------------------------+
  | phar:// stream wrapper support                                       |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2014 The PHP Group                                |
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

#define PHAR_DIRSTREAM 1
#include "phar_internal.h"
#include "dirstream.h"

BEGIN_EXTERN_C()
void phar_dostat(phar_archive_data *phar, phar_entry_info *data, php_stream_statbuf *ssb, zend_bool is_dir TSRMLS_DC);
END_EXTERN_C()

php_stream_ops phar_dir_ops = {
	phar_dir_write, /* write */
	phar_dir_read,  /* read  */
	phar_dir_close, /* close */
	phar_dir_flush, /* flush */
	"phar dir",
	phar_dir_seek,  /* seek */
	NULL,           /* cast */
	NULL,           /* stat */
	NULL, /* set option */
};

/**
 * Used for closedir($fp) where $fp is an opendir('phar://...') directory handle
 */
static int phar_dir_close(php_stream *stream, int close_handle TSRMLS_DC)  /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (data && data->arBuckets) {
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

	if (!data) {
		return -1;
	}

	if (whence == SEEK_END) {
		whence = SEEK_SET;
		offset = zend_hash_num_elements(data) + offset;
	}

	if (whence == SEEK_SET) {
		zend_hash_internal_pointer_reset(data);
	}

	if (offset < 0) {
		return -1;
	} else {
		*newoffset = 0;
		while (*newoffset < offset && zend_hash_move_forward(data) == SUCCESS) {
			++(*newoffset);
		}
		return 0;
	}
}
/* }}} */

/**
 * Used for readdir() on an opendir()ed phar directory handle
 */
static size_t phar_dir_read(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	size_t to_read;
	HashTable *data = (HashTable *)stream->abstract;
	phar_zstr key;
	char *str_key;
	uint keylen;
	ulong unused;

	if (FAILURE == zend_hash_has_more_elements(data)) {
		return 0;
	}

	if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key_ex(data, &key, &keylen, &unused, 0, NULL)) {
		return 0;
	}

	PHAR_STR(key, str_key);
	zend_hash_move_forward(data);
	to_read = MIN(keylen, count);

	if (to_read == 0 || count < keylen) {
		PHAR_STR_FREE(str_key);
		return 0;
	}

	memset(buf, 0, sizeof(php_stream_dirent));
	memcpy(((php_stream_dirent *) buf)->d_name, str_key, to_read);
	PHAR_STR_FREE(str_key);
	((php_stream_dirent *) buf)->d_name[to_read + 1] = '\0';

	return sizeof(php_stream_dirent);
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

/**
 * Dummy: Used for flushing writes to a phar directory (i.e. not used)
 */
static int phar_dir_flush(php_stream *stream TSRMLS_DC) /* {{{ */
{
	return EOF;
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
	void *dummy = (char *) 1;

	return zend_hash_update(ht, arKey, nKeyLength, (void *) &dummy, sizeof(void *), NULL);
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
	result = zend_binary_strcmp(f->arKey, f->nKeyLength, s->arKey, s->nKeyLength);

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
	phar_zstr key;
	char *entry, *found, *save, *str_key;
	uint keylen;
	ulong unused;

	ALLOC_HASHTABLE(data);
	zend_hash_init(data, 64, zend_get_hash_value, NULL, 0);

	if ((*dir == '/' && dirlen == 1 && (manifest->nNumOfElements == 0)) || (dirlen >= sizeof(".phar")-1 && !memcmp(dir, ".phar", sizeof(".phar")-1))) {
		/* make empty root directory for empty phar */
		/* make empty directory for .phar magic directory */
		efree(dir);
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	}

	zend_hash_internal_pointer_reset(manifest);

	while (FAILURE != zend_hash_has_more_elements(manifest)) {
		if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key_ex(manifest, &key, &keylen, &unused, 0, NULL)) {
			break;
		}

		PHAR_STR(key, str_key);

		if (keylen <= (uint)dirlen) {
			if (keylen < (uint)dirlen || !strncmp(str_key, dir, dirlen)) {
				PHAR_STR_FREE(str_key);
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			}
		}

		if (*dir == '/') {
			/* root directory */
			if (keylen >= sizeof(".phar")-1 && !memcmp(str_key, ".phar", sizeof(".phar")-1)) {
				PHAR_STR_FREE(str_key);
				/* do not add any magic entries to this directory */
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			}

			if (NULL != (found = (char *) memchr(str_key, '/', keylen))) {
				/* the entry has a path separator and is a subdirectory */
				entry = (char *) safe_emalloc(found - str_key, 1, 1);
				memcpy(entry, str_key, found - str_key);
				keylen = found - str_key;
				entry[keylen] = '\0';
			} else {
				entry = (char *) safe_emalloc(keylen, 1, 1);
				memcpy(entry, str_key, keylen);
				entry[keylen] = '\0';
			}

			PHAR_STR_FREE(str_key);
			goto PHAR_ADD_ENTRY;
		} else {
			if (0 != memcmp(str_key, dir, dirlen)) {
				/* entry in directory not found */
				PHAR_STR_FREE(str_key);
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			} else {
				if (str_key[dirlen] != '/') {
					PHAR_STR_FREE(str_key);
					if (SUCCESS != zend_hash_move_forward(manifest)) {
						break;
					}
					continue;
				}
			}
		}

		save = str_key;
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
		PHAR_STR_FREE(str_key);
PHAR_ADD_ENTRY:
		if (keylen) {
			phar_add_empty(data, entry, keylen);
		}

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
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	}
}
/* }}}*/

/**
 * Open a directory handle within a phar archive
 */
php_stream *phar_wrapper_open_dir(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *error, *str_key;
	phar_zstr key;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry = NULL;
	uint host_len;

	if ((resource = phar_parse_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar url \"%s\" is unknown", path);
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
	internal_file = resource->path + 1; /* strip leading "/" */

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "%s", error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar file \"%s\" is unknown", resource->host);
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

	if (SUCCESS == zend_hash_find(&phar->manifest, internal_file, strlen(internal_file), (void**)&entry) && !entry->is_dir) {
		php_url_free(resource);
		return NULL;
	} else if (entry && entry->is_dir) {
		if (entry->is_mounted) {
			php_url_free(resource);
			return php_stream_opendir(entry->tmp, options, context);
		}
		internal_file = estrdup(internal_file);
		php_url_free(resource);
		return phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
	} else {
		int i_len = strlen(internal_file);

		/* search for directory */
		zend_hash_internal_pointer_reset(&phar->manifest);
		while (FAILURE != zend_hash_has_more_elements(&phar->manifest)) {
			if (HASH_KEY_NON_EXISTENT != 
					zend_hash_get_current_key_ex(
						&phar->manifest, &key, &keylen, &unused, 0, NULL)) {
				PHAR_STR(key, str_key);
				if (keylen > (uint)i_len && 0 == memcmp(str_key, internal_file, i_len)) {
					PHAR_STR_FREE(str_key);
					/* directory found */
					internal_file = estrndup(internal_file,
							i_len);
					php_url_free(resource);
					return phar_make_dirstream(internal_file, &phar->manifest TSRMLS_CC);
				}
				PHAR_STR_FREE(str_key);
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

/**
 * Make a new directory within a phar archive
 */
int phar_wrapper_mkdir(php_stream_wrapper *wrapper, char *url_from, int mode, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	phar_entry_info entry, *e;
	phar_archive_data *phar = NULL;
	char *error, *arch, *entry2;
	int arch_len, entry_len;
	php_url *resource = NULL;
	uint host_len;

	/* pre-readonly check, we need to know if this is a data phar */
	if (FAILURE == phar_split_fname(url_from, strlen(url_from), &arch, &arch_len, &entry2, &entry_len, 2, 2 TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\", no phar archive specified", url_from);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL TSRMLS_CC)) {
		phar = NULL;
	}

	efree(arch);
	efree(entry2);

	if (PHAR_G(readonly) && (!phar || !phar->is_data)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\", write operations disabled", url_from);
		return 0;
	}

	if ((resource = phar_parse_url(wrapper, url_from, "w", options TSRMLS_CC)) == NULL) {
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url_from);
		return 0;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url_from);
		return 0;
	}

	host_len = strlen(resource->host);

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", error retrieving phar information: %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if ((e = phar_get_entry_info_dir(phar, resource->path + 1, strlen(resource->path + 1), 2, &error, 1 TSRMLS_CC))) {
		/* directory exists, or is a subdirectory of an existing file */
		if (e->is_temp_dir) {
			efree(e->filename);
			efree(e);
		}
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", directory already exists", resource->path+1, resource->host);
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if (phar_get_entry_info_dir(phar, resource->path + 1, strlen(resource->path + 1), 0, &error, 1 TSRMLS_CC)) {
		/* entry exists as a file */
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", file already exists", resource->path+1, resource->host);
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	memset((void *) &entry, 0, sizeof(phar_entry_info));

	/* strip leading "/" */
	if (phar->is_zip) {
		entry.is_zip = 1;
	}

	entry.filename = estrdup(resource->path + 1);

	if (phar->is_tar) {
		entry.is_tar = 1;
		entry.tar_type = TAR_DIR;
	}

	entry.filename_len = strlen(resource->path + 1);
	php_url_free(resource);
	entry.is_dir = 1;
	entry.phar = phar;
	entry.is_modified = 1;
	entry.is_crc_checked = 1;
	entry.flags = PHAR_ENT_PERM_DEF_DIR;
	entry.old_flags = PHAR_ENT_PERM_DEF_DIR;

	if (SUCCESS != zend_hash_add(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", adding to manifest failed", entry.filename, phar->fname);
		efree(error);
		efree(entry.filename);
		return 0;
	}

	phar_flush(phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", entry.filename, phar->fname, error);
		zend_hash_del(&phar->manifest, entry.filename, entry.filename_len);
		efree(error);
		return 0;
	}

	phar_add_virtual_dirs(phar, entry.filename, entry.filename_len TSRMLS_CC);
	return 1;
}
/* }}} */

/**
 * Remove a directory within a phar archive
 */
int phar_wrapper_rmdir(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry;
	phar_archive_data *phar = NULL;
	char *error, *arch, *entry2;
	int arch_len, entry_len;
	php_url *resource = NULL;
	uint host_len;
	phar_zstr key;
	char *str_key;
	uint key_len;
	ulong unused;
	uint path_len;

	/* pre-readonly check, we need to know if this is a data phar */
	if (FAILURE == phar_split_fname(url, strlen(url), &arch, &arch_len, &entry2, &entry_len, 2, 2 TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\", no phar archive specified, or phar archive does not exist", url);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL TSRMLS_CC)) {
		phar = NULL;
	}

	efree(arch);
	efree(entry2);

	if (PHAR_G(readonly) && (!phar || !phar->is_data)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot rmdir directory \"%s\", write operations disabled", url);
		return 0;
	}

	if ((resource = phar_parse_url(wrapper, url, "w", options TSRMLS_CC)) == NULL) {
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

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error TSRMLS_CC)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", error retrieving phar information: %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	path_len = strlen(resource->path+1);

	if (!(entry = phar_get_entry_info_dir(phar, resource->path + 1, path_len, 2, &error, 1 TSRMLS_CC))) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", directory does not exist", resource->path+1, resource->host);
		}
		php_url_free(resource);
		return 0;
	}

	if (!entry->is_deleted) {
		for (zend_hash_internal_pointer_reset(&phar->manifest);
		HASH_KEY_NON_EXISTENT != zend_hash_get_current_key_ex(&phar->manifest, &key, &key_len, &unused, 0, NULL);
		zend_hash_move_forward(&phar->manifest)) {

			PHAR_STR(key, str_key);

			if (key_len > path_len && 
				memcmp(str_key, resource->path+1, path_len) == 0 && 
				IS_SLASH(str_key[path_len])) {
				PHAR_STR_FREE(str_key);
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
			PHAR_STR_FREE(str_key);
		}

		for (zend_hash_internal_pointer_reset(&phar->virtual_dirs);
			HASH_KEY_NON_EXISTENT != zend_hash_get_current_key_ex(&phar->virtual_dirs, &key, &key_len, &unused, 0, NULL);
			zend_hash_move_forward(&phar->virtual_dirs)) {
	
			PHAR_STR(key, str_key);
	
			if (key_len > path_len && 
				memcmp(str_key, resource->path+1, path_len) == 0 && 
				IS_SLASH(str_key[path_len])) {
				PHAR_STR_FREE(str_key);
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
			PHAR_STR_FREE(str_key);
		}
	}

	if (entry->is_temp_dir) {
		zend_hash_del(&phar->virtual_dirs, resource->path+1, path_len);
		efree(entry->filename);
		efree(entry);
	} else {
		entry->is_deleted = 1;
		entry->is_modified = 1;
		phar_flush(phar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", entry->filename, phar->fname, error);
			php_url_free(resource);
			efree(error);
			return 0;
		}
	}

	php_url_free(resource);
	return 1;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
