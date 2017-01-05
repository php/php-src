/*
  +----------------------------------------------------------------------+
  | phar:// stream wrapper support                                       |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2017 The PHP Group                                |
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
void phar_dostat(phar_archive_data *phar, phar_entry_info *data, php_stream_statbuf *ssb, zend_bool is_dir);
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
static int phar_dir_close(php_stream *stream, int close_handle)  /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (data && data->u.flags) {
		zend_hash_destroy(data);
		data->u.flags = 0;
		FREE_HASHTABLE(data);
		stream->abstract = NULL;
	}

	return 0;
}
/* }}} */

/**
 * Used for seeking on a phar directory handle
 */
static int phar_dir_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset) /* {{{ */
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
static size_t phar_dir_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	size_t to_read;
	HashTable *data = (HashTable *)stream->abstract;
	zend_string *str_key;
	zend_ulong unused;

	if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key(data, &str_key, &unused)) {
		return 0;
	}

	zend_hash_move_forward(data);
	to_read = MIN(ZSTR_LEN(str_key), count);

	if (to_read == 0 || count < ZSTR_LEN(str_key)) {
		return 0;
	}

	memset(buf, 0, sizeof(php_stream_dirent));
	memcpy(((php_stream_dirent *) buf)->d_name, ZSTR_VAL(str_key), to_read);
	((php_stream_dirent *) buf)->d_name[to_read + 1] = '\0';

	return sizeof(php_stream_dirent);
}
/* }}} */

/**
 * Dummy: Used for writing to a phar directory (i.e. not used)
 */
static size_t phar_dir_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	return 0;
}
/* }}} */

/**
 * Dummy: Used for flushing writes to a phar directory (i.e. not used)
 */
static int phar_dir_flush(php_stream *stream) /* {{{ */
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
	zval dummy;

	ZVAL_NULL(&dummy);
	return (zend_hash_str_update(ht, arKey, nKeyLength, &dummy) != NULL) ? SUCCESS : FAILURE;
}
/* }}} */

/**
 * Used for sorting directories alphabetically
 */
static int phar_compare_dir_name(const void *a, const void *b)  /* {{{ */
{
	Bucket *f;
	Bucket *s;
	int result;

	f = (Bucket *) a;
	s = (Bucket *) b;
	result = zend_binary_strcmp(ZSTR_VAL(f->key), ZSTR_LEN(f->key), ZSTR_VAL(s->key), ZSTR_LEN(s->key));

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
static php_stream *phar_make_dirstream(char *dir, HashTable *manifest) /* {{{ */
{
	HashTable *data;
	int dirlen = strlen(dir);
	char *entry, *found, *save;
	zend_string *str_key;
	uint keylen;
	zend_ulong unused;

	ALLOC_HASHTABLE(data);
	zend_hash_init(data, 64, NULL, NULL, 0);

	if ((*dir == '/' && dirlen == 1 && (manifest->nNumOfElements == 0)) || (dirlen >= sizeof(".phar")-1 && !memcmp(dir, ".phar", sizeof(".phar")-1))) {
		/* make empty root directory for empty phar */
		/* make empty directory for .phar magic directory */
		efree(dir);
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	}

	zend_hash_internal_pointer_reset(manifest);

	while (FAILURE != zend_hash_has_more_elements(manifest)) {
		keylen = 0;
		if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key(manifest, &str_key, &unused)) {
			break;
		}

		keylen = ZSTR_LEN(str_key);
		if (keylen <= (uint)dirlen) {
			if (keylen == 0 || keylen < (uint)dirlen || !strncmp(ZSTR_VAL(str_key), dir, dirlen)) {
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			}
		}

		if (*dir == '/') {
			/* root directory */
			if (keylen >= sizeof(".phar")-1 && !memcmp(ZSTR_VAL(str_key), ".phar", sizeof(".phar")-1)) {
				/* do not add any magic entries to this directory */
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			}

			if (NULL != (found = (char *) memchr(ZSTR_VAL(str_key), '/', keylen))) {
				/* the entry has a path separator and is a subdirectory */
				entry = (char *) safe_emalloc(found - ZSTR_VAL(str_key), 1, 1);
				memcpy(entry, ZSTR_VAL(str_key), found - ZSTR_VAL(str_key));
				keylen = found - ZSTR_VAL(str_key);
				entry[keylen] = '\0';
			} else {
				entry = (char *) safe_emalloc(keylen, 1, 1);
				memcpy(entry, ZSTR_VAL(str_key), keylen);
				entry[keylen] = '\0';
			}

			goto PHAR_ADD_ENTRY;
		} else {
			if (0 != memcmp(ZSTR_VAL(str_key), dir, dirlen)) {
				/* entry in directory not found */
				if (SUCCESS != zend_hash_move_forward(manifest)) {
					break;
				}
				continue;
			} else {
				if (ZSTR_VAL(str_key)[dirlen] != '/') {
					if (SUCCESS != zend_hash_move_forward(manifest)) {
						break;
					}
					continue;
				}
			}
		}

		save = ZSTR_VAL(str_key);
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
		if (zend_hash_sort(data, phar_compare_dir_name, 0) == FAILURE) {
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
php_stream *phar_wrapper_open_dir(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC) /* {{{ */
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *error;
	zend_string *str_key;
	zend_ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry = NULL;
	uint host_len;

	if ((resource = phar_parse_url(wrapper, path, mode, options)) == NULL) {
		php_stream_wrapper_log_error(wrapper, options, "phar url \"%s\" is unknown", path);
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/ */
	if (!resource->scheme || !resource->host || !resource->path) {
		if (resource->host && !resource->path) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory (always use full path to a new phar)", path, resource->host);
			php_url_free(resource);
			return NULL;
		}
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url \"%s\", must have at least phar://%s/", path, path);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar url \"%s\"", path);
		return NULL;
	}

	host_len = strlen(resource->host);
	phar_request_initialize();
	internal_file = resource->path + 1; /* strip leading "/" */

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error)) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "%s", error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options, "phar file \"%s\" is unknown", resource->host);
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
		ret = phar_make_dirstream(internal_file, &phar->manifest);
		php_url_free(resource);
		return ret;
	}

	if (!phar->manifest.u.flags) {
		php_url_free(resource);
		return NULL;
	}

	if (NULL != (entry = zend_hash_str_find_ptr(&phar->manifest, internal_file, strlen(internal_file))) && !entry->is_dir) {
		php_url_free(resource);
		return NULL;
	} else if (entry && entry->is_dir) {
		if (entry->is_mounted) {
			php_url_free(resource);
			return php_stream_opendir(entry->tmp, options, context);
		}
		internal_file = estrdup(internal_file);
		php_url_free(resource);
		return phar_make_dirstream(internal_file, &phar->manifest);
	} else {
		int i_len = strlen(internal_file);

		/* search for directory */
		zend_hash_internal_pointer_reset(&phar->manifest);
		while (FAILURE != zend_hash_has_more_elements(&phar->manifest)) {
			if (HASH_KEY_NON_EXISTENT !=
					zend_hash_get_current_key(&phar->manifest, &str_key, &unused)) {
				if (ZSTR_LEN(str_key) > (uint)i_len && 0 == memcmp(ZSTR_VAL(str_key), internal_file, i_len)) {
					/* directory found */
					internal_file = estrndup(internal_file,
							i_len);
					php_url_free(resource);
					return phar_make_dirstream(internal_file, &phar->manifest);
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

/**
 * Make a new directory within a phar archive
 */
int phar_wrapper_mkdir(php_stream_wrapper *wrapper, const char *url_from, int mode, int options, php_stream_context *context) /* {{{ */
{
	phar_entry_info entry, *e;
	phar_archive_data *phar = NULL;
	char *error, *arch, *entry2;
	int arch_len, entry_len;
	php_url *resource = NULL;
	uint host_len;

	/* pre-readonly check, we need to know if this is a data phar */
	if (FAILURE == phar_split_fname(url_from, strlen(url_from), &arch, &arch_len, &entry2, &entry_len, 2, 2)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\", no phar archive specified", url_from);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL)) {
		phar = NULL;
	}

	efree(arch);
	efree(entry2);

	if (PHAR_G(readonly) && (!phar || !phar->is_data)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\", write operations disabled", url_from);
		return 0;
	}

	if ((resource = phar_parse_url(wrapper, url_from, "w", options)) == NULL) {
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url \"%s\"", url_from);
		return 0;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar stream url \"%s\"", url_from);
		return 0;
	}

	host_len = strlen(resource->host);

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", error retrieving phar information: %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if ((e = phar_get_entry_info_dir(phar, resource->path + 1, strlen(resource->path + 1), 2, &error, 1))) {
		/* directory exists, or is a subdirectory of an existing file */
		if (e->is_temp_dir) {
			efree(e->filename);
			efree(e);
		}
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", directory already exists", resource->path+1, resource->host);
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if (phar_get_entry_info_dir(phar, resource->path + 1, strlen(resource->path + 1), 0, &error, 1)) {
		/* entry exists as a file */
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", file already exists", resource->path+1, resource->host);
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
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

	if (NULL == zend_hash_str_add_mem(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info))) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", adding to manifest failed", entry.filename, phar->fname);
		efree(error);
		efree(entry.filename);
		return 0;
	}

	phar_flush(phar, 0, 0, 0, &error);

	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", entry.filename, phar->fname, error);
		zend_hash_str_del(&phar->manifest, entry.filename, entry.filename_len);
		efree(error);
		return 0;
	}

	phar_add_virtual_dirs(phar, entry.filename, entry.filename_len);
	return 1;
}
/* }}} */

/**
 * Remove a directory within a phar archive
 */
int phar_wrapper_rmdir(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context) /* {{{ */
{
	phar_entry_info *entry;
	phar_archive_data *phar = NULL;
	char *error, *arch, *entry2;
	int arch_len, entry_len;
	php_url *resource = NULL;
	uint host_len;
	zend_string *str_key;
	zend_ulong unused;
	uint path_len;

	/* pre-readonly check, we need to know if this is a data phar */
	if (FAILURE == phar_split_fname(url, strlen(url), &arch, &arch_len, &entry2, &entry_len, 2, 2)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\", no phar archive specified, or phar archive does not exist", url);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL)) {
		phar = NULL;
	}

	efree(arch);
	efree(entry2);

	if (PHAR_G(readonly) && (!phar || !phar->is_data)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot rmdir directory \"%s\", write operations disabled", url);
		return 0;
	}

	if ((resource = phar_parse_url(wrapper, url, "w", options)) == NULL) {
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url \"%s\"", url);
		return 0;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar stream url \"%s\"", url);
		return 0;
	}

	host_len = strlen(resource->host);

	if (FAILURE == phar_get_archive(&phar, resource->host, host_len, NULL, 0, &error)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", error retrieving phar information: %s", resource->path+1, resource->host, error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	path_len = strlen(resource->path+1);

	if (!(entry = phar_get_entry_info_dir(phar, resource->path + 1, path_len, 2, &error, 1))) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", resource->path+1, resource->host, error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", directory does not exist", resource->path+1, resource->host);
		}
		php_url_free(resource);
		return 0;
	}

	if (!entry->is_deleted) {
		for (zend_hash_internal_pointer_reset(&phar->manifest);
			HASH_KEY_NON_EXISTENT != zend_hash_get_current_key(&phar->manifest, &str_key, &unused);
			zend_hash_move_forward(&phar->manifest)
		) {
			if (ZSTR_LEN(str_key) > path_len &&
				memcmp(ZSTR_VAL(str_key), resource->path+1, path_len) == 0 &&
				IS_SLASH(ZSTR_VAL(str_key)[path_len])) {
				php_stream_wrapper_log_error(wrapper, options, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
		}

		for (zend_hash_internal_pointer_reset(&phar->virtual_dirs);
			HASH_KEY_NON_EXISTENT != zend_hash_get_current_key(&phar->virtual_dirs, &str_key, &unused);
			zend_hash_move_forward(&phar->virtual_dirs)) {

			if (ZSTR_LEN(str_key) > path_len &&
				memcmp(ZSTR_VAL(str_key), resource->path+1, path_len) == 0 &&
				IS_SLASH(ZSTR_VAL(str_key)[path_len])) {
				php_stream_wrapper_log_error(wrapper, options, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
		}
	}

	if (entry->is_temp_dir) {
		zend_hash_str_del(&phar->virtual_dirs, resource->path+1, path_len);
		efree(entry->filename);
		efree(entry);
	} else {
		entry->is_deleted = 1;
		entry->is_modified = 1;
		phar_flush(phar, 0, 0, 0, &error);

		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", entry->filename, phar->fname, error);
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
