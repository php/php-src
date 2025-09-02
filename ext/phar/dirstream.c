/*
  +----------------------------------------------------------------------+
  | phar:// stream wrapper support                                       |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
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

void phar_dostat(phar_archive_data *phar, phar_entry_info *data, php_stream_statbuf *ssb, bool is_dir);

static const php_stream_ops phar_dir_ops = {
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

	if (data) {
		zend_hash_destroy(data);
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
static ssize_t phar_dir_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;
	zend_string *str_key;
	zend_ulong unused;

	if (count != sizeof(php_stream_dirent)) {
		return -1;
	}

	if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key(data, &str_key, &unused)) {
		return 0;
	}

	zend_hash_move_forward(data);

	php_stream_dirent *dirent = (php_stream_dirent *) buf;

	if (sizeof(dirent->d_name) <= ZSTR_LEN(str_key)) {
		return 0;
	}

	memset(dirent, 0, sizeof(php_stream_dirent));
	PHP_STRLCPY(dirent->d_name, ZSTR_VAL(str_key), sizeof(dirent->d_name), ZSTR_LEN(str_key));

	return sizeof(php_stream_dirent);
}
/* }}} */

/**
 * Dummy: Used for writing to a phar directory (i.e. not used)
 */
static ssize_t phar_dir_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	return -1;
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
 * Used for sorting directories alphabetically
 */
static int phar_compare_dir_name(Bucket *f, Bucket *s)  /* {{{ */
{
	int result = zend_binary_strcmp(
		ZSTR_VAL(f->key), ZSTR_LEN(f->key), ZSTR_VAL(s->key), ZSTR_LEN(s->key));
	return ZEND_NORMALIZE_BOOL(result);
}
/* }}} */

/**
 * Create a opendir() directory stream handle by iterating over each of the
 * files in a phar and retrieving its relative path.  From this, construct
 * a list of files/directories that are "in" the directory represented by dir
 */
static php_stream *phar_make_dirstream(const char *dir, size_t dirlen, const HashTable *manifest) /* {{{ */
{
	HashTable *data;
	char *entry;

	ALLOC_HASHTABLE(data);
	zend_hash_init(data, 64, NULL, NULL, 0);

	if ((*dir == '/' && dirlen == 1 && (manifest->nNumOfElements == 0)) || (dirlen >= sizeof(".phar")-1 && !memcmp(dir, ".phar", sizeof(".phar")-1))) {
		/* make empty root directory for empty phar */
		/* make empty directory for .phar magic directory */
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	}

	zend_string *str_key;
	ZEND_HASH_MAP_FOREACH_STR_KEY(manifest, str_key) {
		size_t keylen = ZSTR_LEN(str_key);
		if (keylen <= dirlen) {
			if (keylen == 0 || keylen < dirlen || !strncmp(ZSTR_VAL(str_key), dir, dirlen)) {
				continue;
			}
		}

		if (*dir == '/') {
			/* root directory */
			if (zend_string_starts_with_literal(str_key, ".phar")) {
				/* do not add any magic entries to this directory */
				continue;
			}

			const char *has_slash = memchr(ZSTR_VAL(str_key), '/', keylen);
			if (has_slash) {
				/* the entry has a path separator and is a subdirectory */
				keylen = has_slash - ZSTR_VAL(str_key);
			}
			entry = safe_emalloc(keylen, 1, 1);
			memcpy(entry, ZSTR_VAL(str_key), keylen);
			entry[keylen] = '\0';

			goto PHAR_ADD_ENTRY;
		} else {
			if (0 != memcmp(ZSTR_VAL(str_key), dir, dirlen)) {
				/* entry in directory not found */
				continue;
			} else {
				if (ZSTR_VAL(str_key)[dirlen] != '/') {
					continue;
				}
			}
		}

		const char *save = ZSTR_VAL(str_key);
		save += dirlen + 1; /* seek to just past the path separator */

		const char *has_slash = memchr(save, '/', keylen - dirlen - 1);
		if (has_slash) {
			/* is subdirectory */
			save -= dirlen + 1;
			entry = safe_emalloc(has_slash - save + dirlen, 1, 1);
			memcpy(entry, save + dirlen + 1, has_slash - save - dirlen - 1);
			keylen = has_slash - save - dirlen - 1;
			entry[keylen] = '\0';
		} else {
			/* is file */
			save -= dirlen + 1;
			entry = safe_emalloc(keylen - dirlen, 1, 1);
			memcpy(entry, save + dirlen + 1, keylen - dirlen - 1);
			entry[keylen - dirlen - 1] = '\0';
			keylen = keylen - dirlen - 1;
		}
PHAR_ADD_ENTRY:
		if (keylen) {
			/**
			 * Add an empty element to avoid duplicates
			 *
			 * This is used to get a unique listing of virtual directories within a phar,
			 * for iterating over opendir()ed phar directories.
			 */
			zval dummy;

			ZVAL_NULL(&dummy);
			zend_hash_str_update(data, entry, keylen, &dummy);
		}

		efree(entry);
	} ZEND_HASH_FOREACH_END();

	if (FAILURE != zend_hash_has_more_elements(data)) {
		zend_hash_sort(data, phar_compare_dir_name, 0);
		return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
	} else {
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
	char *error;
	phar_archive_data *phar;

	if ((resource = phar_parse_url(wrapper, path, mode, options)) == NULL) {
		php_stream_wrapper_log_error(wrapper, options, "phar url \"%s\" is unknown", path);
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/ */
	if (!resource->scheme || !resource->host || !resource->path) {
		if (resource->host && !resource->path) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory (always use full path to a new phar)", path, ZSTR_VAL(resource->host));
			php_url_free(resource);
			return NULL;
		}
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url \"%s\", must have at least phar://%s/", path, path);
		return NULL;
	}

	if (!zend_string_equals_literal_ci(resource->scheme, "phar")) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar url \"%s\"", path);
		return NULL;
	}

	phar_request_initialize();

	if (FAILURE == phar_get_archive(&phar, ZSTR_VAL(resource->host), ZSTR_LEN(resource->host), NULL, 0, &error)) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "%s", error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options, "phar file \"%s\" is unknown", ZSTR_VAL(resource->host));
		}
		php_url_free(resource);
		return NULL;
	}

	if (error) {
		efree(error);
	}

	if (zend_string_equals(resource->path, ZSTR_CHAR('/'))) {
		/* root directory requested */
		php_url_free(resource);
		return phar_make_dirstream("/", strlen("/"), &phar->manifest);
	}

	if (!HT_IS_INITIALIZED(&phar->manifest)) {
		php_url_free(resource);
		return NULL;
	}

	const char *internal_file = ZSTR_VAL(resource->path) + 1; /* strip leading "/" */
	size_t internal_file_len = ZSTR_LEN(resource->path) - 1;
	phar_entry_info *entry = zend_hash_str_find_ptr(&phar->manifest, internal_file, internal_file_len);
	php_stream *ret;

	if (NULL != entry && !entry->is_dir) {
		php_url_free(resource);
		return NULL;
	} else if (entry && entry->is_dir) {
		if (entry->is_mounted) {
			ret = php_stream_opendir(entry->tmp, options, context);
			php_url_free(resource);
			return ret;
		}
		ret = phar_make_dirstream(internal_file, internal_file_len, &phar->manifest);
		php_url_free(resource);
		return ret;
	} else {
		zend_string *str_key;

		/* search for directory */
		ZEND_HASH_MAP_FOREACH_STR_KEY(&phar->manifest, str_key) {
			if (zend_string_starts_with_cstr(str_key, internal_file, internal_file_len)) {
				/* directory found */
				ret = phar_make_dirstream(internal_file, internal_file_len, &phar->manifest);
				php_url_free(resource);
				return ret;
			}
		} ZEND_HASH_FOREACH_END();
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
	size_t arch_len, entry_len;
	php_url *resource = NULL;

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

	if (!zend_string_equals_literal_ci(resource->scheme, "phar")) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar stream url \"%s\"", url_from);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, ZSTR_VAL(resource->host), ZSTR_LEN(resource->host), NULL, 0, &error)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", error retrieving phar information: %s", ZSTR_VAL(resource->path) + 1, ZSTR_VAL(resource->host), error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if ((e = phar_get_entry_info_dir(phar, ZSTR_VAL(resource->path) + 1, ZSTR_LEN(resource->path) - 1, 2, &error, 1))) {
		/* directory exists, or is a subdirectory of an existing file */
		if (e->is_temp_dir) {
			zend_string_efree(e->filename);
			efree(e);
		}
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", directory already exists", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host));
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host), error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	if (phar_get_entry_info_dir(phar, ZSTR_VAL(resource->path) + 1, ZSTR_LEN(resource->path) - 1, 0, &error, 1)) {
		/* entry exists as a file */
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", file already exists", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host));
		php_url_free(resource);
		return 0;
	}

	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host), error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	memset((void *) &entry, 0, sizeof(phar_entry_info));

	/* strip leading "/" */
	if (phar->is_zip) {
		entry.is_zip = 1;
	}

	entry.filename = zend_string_init(ZSTR_VAL(resource->path) + 1, ZSTR_LEN(resource->path) - 1, false);

	if (phar->is_tar) {
		entry.is_tar = 1;
		entry.tar_type = TAR_DIR;
	}

	php_url_free(resource);
	entry.is_dir = 1;
	entry.phar = phar;
	entry.is_modified = 1;
	entry.is_crc_checked = 1;
	entry.flags = PHAR_ENT_PERM_DEF_DIR;
	entry.old_flags = PHAR_ENT_PERM_DEF_DIR;

	if (NULL == zend_hash_add_mem(&phar->manifest, entry.filename, &entry, sizeof(phar_entry_info))) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", adding to manifest failed", ZSTR_VAL(entry.filename), phar->fname);
		efree(error);
		zend_string_efree(entry.filename);
		return 0;
	}

	phar_flush(phar, &error);

	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot create directory \"%s\" in phar \"%s\", %s", ZSTR_VAL(entry.filename), phar->fname, error);
		zend_hash_del(&phar->manifest, entry.filename);
		efree(error);
		return 0;
	}

	phar_add_virtual_dirs(phar, ZSTR_VAL(entry.filename), ZSTR_LEN(entry.filename));
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
	size_t arch_len, entry_len;
	php_url *resource = NULL;

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

	if (!zend_string_equals_literal_ci(resource->scheme, "phar")) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar stream url \"%s\"", url);
		return 0;
	}

	if (FAILURE == phar_get_archive(&phar, ZSTR_VAL(resource->host), ZSTR_LEN(resource->host), NULL, 0, &error)) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", error retrieving phar information: %s", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host), error);
		efree(error);
		php_url_free(resource);
		return 0;
	}

	size_t path_len = ZSTR_LEN(resource->path) - 1;

	if (!(entry = phar_get_entry_info_dir(phar, ZSTR_VAL(resource->path) + 1, path_len, 2, &error, 1))) {
		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host), error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", directory does not exist", ZSTR_VAL(resource->path)+1, ZSTR_VAL(resource->host));
		}
		php_url_free(resource);
		return 0;
	}

	if (!entry->is_deleted) {
		zend_string *str_key;

		ZEND_HASH_MAP_FOREACH_STR_KEY(&phar->manifest, str_key) {
			if (
				zend_string_starts_with_cstr(str_key, ZSTR_VAL(resource->path)+1, path_len)
				&& IS_SLASH(ZSTR_VAL(str_key)[path_len])
			) {
				php_stream_wrapper_log_error(wrapper, options, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					zend_string_efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_MAP_FOREACH_STR_KEY(&phar->virtual_dirs, str_key) {
			ZEND_ASSERT(str_key);
			if (
				zend_string_starts_with_cstr(str_key, ZSTR_VAL(resource->path)+1, path_len)
				&& IS_SLASH(ZSTR_VAL(str_key)[path_len])
			) {
				php_stream_wrapper_log_error(wrapper, options, "phar error: Directory not empty");
				if (entry->is_temp_dir) {
					zend_string_efree(entry->filename);
					efree(entry);
				}
				php_url_free(resource);
				return 0;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (entry->is_temp_dir) {
		zend_hash_str_del(&phar->virtual_dirs, ZSTR_VAL(resource->path)+1, path_len);
		zend_string_efree(entry->filename);
		efree(entry);
	} else {
		entry->is_deleted = 1;
		entry->is_modified = 1;
		phar_flush(phar, &error);

		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: cannot remove directory \"%s\" in phar \"%s\", %s", ZSTR_VAL(entry->filename), phar->fname, error);
			php_url_free(resource);
			efree(error);
			return 0;
		}
	}

	php_url_free(resource);
	return 1;
}
/* }}} */
