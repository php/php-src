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

	zend_hash_apply_with_argument(&(PHAR_GLOBALS->phar_alias_map), phar_unalias_apply, phar_data TSRMLS_CC);
	if (--phar_data->refcount < 0) {
		phar_destroy_phar_data(phar_data TSRMLS_CC);
	}
}
/* }}}*/

/**
 * destructor for the manifest hash, frees each file's entry
 */
static void destroy_phar_manifest(void *pDest) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;
	TSRMLS_FETCH();

	if (entry->cfp) {
		php_stream_close(entry->cfp);
		entry->cfp = 0;
	}
	if (entry->fp) {
		php_stream_close(entry->fp);
		entry->fp = 0;
	}
	if (entry->metadata) {
		zval_ptr_dtor(&entry->metadata);
		entry->metadata = 0;
	}
	efree(entry->filename);
}
/* }}} */

/**
 * Looks up a phar archive in the filename map, connecting it to the alias
 * (if any) or returns null
 */
static int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *fd, **fd_ptr;

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
	if (fname && fname_len) {
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;
			if (alias && alias_len) {
				zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&fd,   sizeof(phar_archive_data*), NULL);
			}
			return SUCCESS;
		}
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), fname, fname_len, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			return SUCCESS;
		}
	}
	return FAILURE;
}
/* }}} */

/**
 * retrieve information on a file contained within a phar, or null if it ain't there
 */
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error TSRMLS_DC) /* {{{ */
{
	const char *pcr_error;
	phar_entry_info *entry;

	if (error) {
		*error = NULL;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 0, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (!&phar->manifest.arBuckets) {
		return NULL;
	}
	if (SUCCESS == zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
		if (entry->is_deleted) {
			/* entry is deleted, but has not been flushed to disk yet */
			return NULL;
		}
		return entry;
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
	int			mode;
	php_stream  **owner_ptr;
} php_stream_memory_data;
#endif

/**
 * Retrieve a copy of the file information on a single file within a phar, or null.
 * This also transfers the open file pointer, if any, to the entry.
 */
static int phar_get_entry_data(phar_entry_data **ret, char *fname, int fname_len, char *path, int path_len, char *mode, char **error TSRMLS_DC) /* {{{ */
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
	if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) ? NULL : error TSRMLS_CC)) == NULL) {
		if (for_create && !PHAR_G(readonly)) {
			return SUCCESS;
		}
		return FAILURE;
	}
	if (entry->is_modified && !for_write) {
		if (error) {
			spprintf(error, 0, "phar error: file \"%s\" cannot opened for reading, writable file pointers are open", fname);
		}
		return FAILURE;
	}
	if (entry->fp_refcount && for_write) {
		if (error) {
			spprintf(error, 0, "phar error: file \"%s\" cannot opened for writing, readable file pointers are open", fname);
		}
		return FAILURE;
	}
	if (entry->is_deleted) {
		if (!for_create) {
			return FAILURE;
		}
		entry->is_deleted = 0;
	}
	*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	(*ret)->position = 0;
	(*ret)->phar = phar;
	(*ret)->internal_file = entry;
	if (entry->fp) {
		/* make a copy */
		if (for_trunc) {
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
	} else {
		(*ret)->fp = 0;
		if (for_write) {
			/* open a new temp file for writing */
			entry->fp = php_stream_fopen_tmpfile();
			if (!entry->fp) {
				if (error) {
					spprintf(error, 0, "phar error: unable to create temprary file");
				}
				return FAILURE;
			}
			(*ret)->fp = entry->fp;
			entry->is_modified = 1;
			phar->is_modified = 1;
			/* reset file size */
			entry->uncompressed_filesize = 0;
			entry->compressed_filesize = 0;
			entry->crc32 = 0;
			entry->flags = PHAR_ENT_PERM_DEF_FILE;
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

	if (--idata->internal_file->fp_refcount <= 0) {
		idata->internal_file->fp_refcount = 0;
	}
	if (idata->fp && idata->fp != idata->internal_file->fp) {
		php_stream_close(idata->fp);
	}
	phar_archive_delref(idata->internal_file->phar TSRMLS_CC);
	efree(idata);
	return ret;
}
/* }}} */

/**
 * Removes an entry, either by actually removingit or by marking it.
 */
void phar_entry_remove(phar_entry_data *idata, char **error TSRMLS_DC) /* {{{ */
{
	if (!idata->phar->donotflush) {
		phar_flush(idata->internal_file->phar, 0, 0, error TSRMLS_CC);
	}
	if (idata->internal_file->fp_refcount < 2) {
		if (idata->fp && idata->fp != idata->internal_file->fp) {
			php_stream_close(idata->fp);
		}
		zend_hash_del(&idata->phar->manifest, idata->internal_file->filename, idata->internal_file->filename_len);
		idata->phar->refcount--;
		efree(idata);
	} else {
		idata->internal_file->is_deleted = 1;
		phar_entry_delref(idata TSRMLS_CC);
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
			spprintf(error, 0, "phar error: unable to create temorary file");
		}
		return NULL;
	}
	etemp.fp_refcount = 1;
	etemp.is_modified = 1;
	etemp.filename = estrndup(path, path_len);
	etemp.timestamp = time(0);
	etemp.offset_within_phar = -1;
	etemp.is_crc_checked = 1;
	etemp.flags = PHAR_ENT_PERM_DEF_FILE;
	etemp.phar = phar;
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
static int phar_open_loaded(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;

	if (error) {
		*error = NULL;
	}
	if (SUCCESS == phar_get_archive(&phar, fname, fname_len, alias, alias_len, error TSRMLS_CC)
		&& fname_len == phar->fname_len
		&& !strncmp(fname, phar->fname, fname_len)
	) {
		if (pphar) {
			*pphar = phar;
		}
		return SUCCESS;
	} else {
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
static int phar_parse_metadata(php_stream *fp, char **buffer, char *endbuffer, zval **metadata TSRMLS_DC) /* {{{ */
{
	const unsigned char *p;
	php_uint32 buf_len;
	php_unserialize_data_t var_hash;

	PHAR_GET_32(*buffer, buf_len);
	
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
		MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	if ((*buffer == ' ' || *buffer == '\n') && *(buffer + 1) == '?' && *(buffer + 2) == '>') {
		int nextchar;
		halt_offset += 3;
		if (EOF == (nextchar = php_stream_getc(fp))) {
			MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
		}
		if ((char) nextchar == '\r') {
			if (EOF == (nextchar = php_stream_getc(fp))) {
				MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
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
		MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	PHAR_GET_32(buffer, manifest_len);
	if (manifest_len > 1048576) {
		/* prevent serious memory issues by limiting manifest to at most 1 MB in length */
		MAPPHAR_ALLOC_FAIL("manifest cannot be larger than 1 MB in phar \"%s\"")
	}
	buffer = (char *)emalloc(manifest_len);
	savebuf = buffer;
	endbuffer = buffer + manifest_len;
	if (manifest_len != php_stream_read(fp, buffer, manifest_len)) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	if (manifest_len < 10) {
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
				spprintf(error, 0, "phar \"%s\" has a broken signature", fname);
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
	if (phar_parse_metadata(fp, &buffer, endbuffer, &mydata->metadata TSRMLS_CC) == FAILURE) {
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
		if (phar_parse_metadata(fp, &buffer, endbuffer, &entry.metadata TSRMLS_CC) == FAILURE) {
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
#endif
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
#if !HAVE_BZ2
			if (entry.metadata) {
				zval_ptr_dtor(&entry.metadata);
			}
			efree(entry.filename);
			MAPPHAR_FAIL("bz2 extension is required for bzip2 compressed .phar file \"%s\"");
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
		entry.is_crc_checked = 0;
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
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	mydata->sig_flags = sig_flags;
	mydata->sig_len = sig_len;
	mydata->signature = signature;
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*),  NULL);
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
int phar_open_or_create_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *mydata;
	int register_alias;
	php_stream *fp;

	if (!pphar) {
		pphar = &mydata;
	}
	if (error) {
		*error = NULL;
	}

	if (phar_open_loaded(fname, fname_len, alias, alias_len, options, pphar, 0 TSRMLS_CC) == SUCCESS) {
		if (!PHAR_G(readonly)) {
			(*pphar)->is_writeable = 1;
		}
		return SUCCESS;
	}

#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return FAILURE;
	}
#endif

	if (php_check_open_basedir(fname TSRMLS_CC)) {
		return FAILURE;
	}

	fp = php_stream_open_wrapper(fname, PHAR_G(readonly)?"rb":"r+b", IGNORE_URL|STREAM_MUST_SEEK|0, NULL);

	if (fp && phar_open_fp(fp, fname, fname_len, alias, alias_len, options, pphar, 0 TSRMLS_CC) == SUCCESS) {
		if (!PHAR_G(readonly)) {
			(*pphar)->is_writeable = 1;
		}
		return SUCCESS;
	}

	if (PHAR_G(readonly)) {
		if (options & REPORT_ERRORS) {
			if (error) {
				spprintf(error, 0, "creating archive \"%s\" disabled by INI setting", fname);
			}
		}
		return FAILURE;
	}

	/* set up our manifest */
	mydata = ecalloc(sizeof(phar_archive_data), 1);
	if (pphar) {
		*pphar = mydata;
	}
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	mydata->fname = estrndup(fname, fname_len);
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
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*),  NULL);
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
	} else if (*error) {
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

	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
	
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
	char *pos, buffer[1024 + sizeof(token)];
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
		if ((got = php_stream_read(fp, buffer+tokenlen, readsize)) < tokenlen) {
			MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
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

/**
 * Process a phar stream name, ensuring we can handle any of:
 * 
 * - phar://whatever.phar
 * - whatever.phar
 * - whatever.phar.gz
 * - whatever.phar.bz2
 *
 * This is used by phar_open_url()
 */
int phar_split_fname(char *filename, int filename_len, char **arch, int *arch_len, char **entry, int *entry_len TSRMLS_DC) /* {{{ */
{
	char *pos_p, *pos_z, *pos_b, *ext_str;
	int ext_len;

	if (!strncasecmp(filename, "phar://", 7)) {
		filename += 7;
		filename_len -= 7;
	}

	pos_p = strstr(filename, ".phar.php");
	pos_z = strstr(filename, ".phar.gz");
	pos_b = strstr(filename, ".phar.bz2");
	if (pos_p) {
		if (pos_z) {
			return FAILURE;
		}
		ext_str = pos_p;
		ext_len = 9;
	} else if (pos_z) {
		ext_str = pos_z;
		ext_len = 8;
	} else if (pos_b) {
		ext_str = pos_b;
		ext_len = 9;
	} else if ((pos_p = strstr(filename, ".phar")) != NULL) {
		ext_str = pos_p;
		ext_len = 5;
	} else {
		return FAILURE;
	}
	*arch_len = ext_str - filename + ext_len;
	*arch = estrndup(filename, *arch_len);
	if (ext_str[ext_len]) {
		*entry_len = filename_len - *arch_len;
		*entry = estrndup(ext_str+ext_len, *entry_len);
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
	char *arch, *entry, *error;
	int arch_len, entry_len;

	if (!strncasecmp(filename, "phar://", 7)) {
		if (mode[0] == 'a') {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: open mode append not supported");
			return NULL;			
		}		
		if (phar_split_fname(filename, strlen(filename), &arch, &arch_len, &entry, &entry_len TSRMLS_CC) == FAILURE) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\" (cannot contain .phar.php and .phar.gz/.phar.bz2)", filename);
			efree(arch);
			efree(entry);
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

	if (alias && phar_open_loaded(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL, 0 TSRMLS_CC) == SUCCESS) {
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

	php_stream_seek(fp, 0, SEEK_SET);	
	while (len--) { 
		CRC32(crc, php_stream_getc(fp));
	}
	php_stream_seek(fp, 0, SEEK_SET);
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
	switch (entry->flags & PHAR_ENT_COMPRESSION_MASK) {
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
 * used for fopen('phar://...') and company
 */
static php_stream * phar_wrapper_open_url(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	phar_entry_data *idata;
	char *internal_file;
	char *buffer;
	char *error;
	char *filter_name;
	char *plain_map;
	char tmpbuf[8];
	HashTable *pharcontext;
	php_url *resource = NULL;
	php_stream *fp, *fpf;
	php_stream_filter *filter/*,  *consumed */;
	php_uint32 offset, read, total, toread;
	zval **pzoption, *metadata;
	uint host_len;

	resource = php_url_parse(path);

	if (!resource && (resource = phar_open_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
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
	if (zend_hash_find(&(PHAR_GLOBALS->phar_plain_map), resource->host, host_len+1, (void **)&plain_map) == SUCCESS) {
		spprintf(&internal_file, 0, "%s%s", plain_map, resource->path);
		fp = php_stream_open_wrapper_ex(internal_file, mode, options, opened_path, context);
		if (!fp) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" extracted from \"%s\" could not be opened", internal_file, resource->host);
		}
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

	if (!fp) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot open phar \"%s\"", idata->phar->fname);
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}

	/* seek to start of internal file and read it */
	offset = idata->phar->internal_file_start + idata->internal_file->offset_within_phar;
	if (-1 == php_stream_seek(fp, offset, SEEK_SET)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (cannot seek to start of file \"%s\" at offset \"%d\")",
			idata->phar->fname, internal_file, offset);
		phar_entry_delref(idata TSRMLS_CC);
		efree(internal_file);
		return NULL;
	}

	if ((idata->internal_file->flags & PHAR_ENT_COMPRESSION_MASK) != 0) {
		if ((filter_name = phar_decompress_filter(idata->internal_file, 0)) != NULL) {
			filter = php_stream_filter_create(phar_decompress_filter(idata->internal_file, 0), NULL, php_stream_is_persistent(fp) TSRMLS_CC);
		} else {
			filter = NULL;
		}
		if (!filter) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", idata->phar->fname, phar_decompress_filter(idata->internal_file, 1), internal_file);
			phar_entry_delref(idata TSRMLS_CC);
			efree(internal_file);
			return NULL;			
		}
		buffer = (char *) emalloc(8192);
		read = 0;
		total = 0;

		idata->fp = php_stream_temp_new();
		idata->internal_file->fp = idata->fp;
		php_stream_filter_append(&idata->fp->writefilters, filter);
		do {
			if ((total + 8192) > idata->internal_file->compressed_filesize) {
				toread = idata->internal_file->compressed_filesize - total;
			} else {
				toread = 8192;
			}
			read = php_stream_read(fp, buffer, toread);
			if (read) {
				total += read;
				if (read != php_stream_write(idata->fp, buffer, read)) {
					efree(buffer);
					php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
					phar_entry_delref(idata TSRMLS_CC);
					efree(internal_file);
					return NULL;
				}
				if (total == idata->internal_file->compressed_filesize) {
					read = 0;
				} 
			}
		} while (read);
		efree(buffer);
		php_stream_filter_flush(filter, 1);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		if (php_stream_tell(fp) != (off_t)(offset + idata->internal_file->compressed_filesize)) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
			phar_entry_delref(idata TSRMLS_CC);
			efree(internal_file);
			return NULL;
		}
		if (php_stream_tell(idata->fp) != (off_t)idata->internal_file->uncompressed_filesize) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
			phar_entry_delref(idata TSRMLS_CC);
			efree(internal_file);
			return NULL;
		}
		php_stream_seek(fp, offset + idata->internal_file->compressed_filesize, SEEK_SET);
	} else { /* from here is for non-compressed */
		buffer = &tmpbuf[0];
		/* bypass to temp stream */
		idata->fp = php_stream_temp_new();
		idata->internal_file->fp = idata->fp;
		if (php_stream_copy_to_stream(fp, idata->fp, idata->internal_file->uncompressed_filesize) != idata->internal_file->uncompressed_filesize) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
			phar_entry_delref(idata TSRMLS_CC);
			efree(internal_file);
			return NULL;
		}
	}

	/* check length, crc32 */
	if (phar_postprocess_file(wrapper, options, idata, idata->internal_file->crc32 TSRMLS_CC) != SUCCESS) {
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
	php_stream_seek(data->fp, data->position, SEEK_SET);
	
	got = php_stream_read(data->fp, buf, count);
	if (data->fp->eof) {
		stream->eof = 1;
	}

	/* note the position, and restore the stream for the next fp */
	data->position = php_stream_tell(data->fp);

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

/**
 * Save phar contents to disk
 *
 * user_stub contains either a string, or a resource pointer, if len is a negative length.
 * user_stub and len should be both 0 if the default or existing stub should be used
 */
int phar_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC) /* {{{ */
{
	static const char newstub[] = "<?php __HALT_COMPILER();";
	phar_entry_info *entry;
	int halt_offset, restore_alias_len, global_flags = 0, closeoldfile;
	char *buf, *pos;
	char manifest[18], entry_buffer[24];
	off_t manifest_ftell;
	long offset;
	size_t wrote, read;
	php_uint32 manifest_len, mytime, loc, new_manifest_count;
	php_uint32 newcrc32;
	php_stream *file, *oldfile, *newfile, *stubfile;
	php_stream_filter *filter;
	php_serialize_data_t metadata_hash;
	smart_str metadata_str = {0};

	if (error) {
		*error = NULL;
	}

	if (PHAR_G(readonly)) {
		return EOF;
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
					spprintf(error, 0, "unable to read resource to copy stub to new phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			if (len == -1) {
				len = PHP_STREAM_COPY_ALL;
			} else {
				len = -len;
			}
			offset = php_stream_copy_to_stream(stubfile, newfile, len);
			if (len != offset && len != PHP_STREAM_COPY_ALL) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to copy stub from resource to new phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			archive->halt_offset = offset;
		} else {
			if ((pos = strstr(user_stub, "__HALT_COMPILER();")) == NULL)
			{
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "illegal stub for phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			if ((size_t)len != php_stream_write(newfile, user_stub, len)) {
				if (closeoldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (error) {
					spprintf(error, 0, "unable to create stub from string in new phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			archive->halt_offset = len;
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
	metadata_str.c = 0;
	if (archive->metadata) {
		PHP_VAR_SERIALIZE_INIT(metadata_hash);
		php_var_serialize(&metadata_str, &archive->metadata, &metadata_hash TSRMLS_CC);
		PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
	} else {
		metadata_str.len = 0;
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
		offset += 4 + entry->filename_len + sizeof(entry_buffer);

		/* compress and rehash as necessary */
		if (oldfile && !entry->is_modified) {
			continue;
		}
		if (!entry->fp) {
			/* this should never happen */
			continue;
		}
		file = entry->fp;
		php_stream_rewind(file);
		newcrc32 = ~0;
		mytime = entry->uncompressed_filesize;
		for (loc = 0;loc < mytime; loc++) {
			CRC32(newcrc32, php_stream_getc(file));
		}
		entry->crc32 = ~newcrc32;
		entry->is_crc_checked = 1;
		if (!(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
			/* not compressed */
			continue;
		}
		php_stream_rewind(file);
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
		php_stream_filter_append(&file->readfilters, filter);

		/* create new file that holds the compressed version */
		/* work around inability to specify freedom in write and strictness
		in read count */
		entry->compressed_filesize = 0;
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
		do {
			read = php_stream_read(file, buf, 8192);
			if (read) {
				if (read != php_stream_write(entry->cfp, buf, read)) {
					if (error) {
						spprintf(error, 0, "unable to write to file \"%s\" while creating new phar \"%s\"", entry->filename, archive->fname);
					}
					efree(buf);
					php_stream_filter_remove(filter, 1 TSRMLS_CC);
					php_stream_close(entry->cfp);
					entry->cfp = 0;
					if (closeoldfile) {
						php_stream_close(oldfile);
					}
					php_stream_close(newfile);
					return EOF;
				}
				entry->compressed_filesize += read;
			}
		} while (read);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		/* generate crc on compressed file */
		php_stream_rewind(entry->cfp);
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

	manifest_len = offset + archive->alias_len + sizeof(manifest) + metadata_str.len;
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
	
	metadata_str.c = 0;
	phar_set_32(manifest, metadata_str.len);
	if (metadata_str.len) {
		if (4 != php_stream_write(newfile, manifest, 4) ||
		metadata_str.len != php_stream_write(newfile, metadata_str.c, metadata_str.len)) {
			smart_str_free(&metadata_str);
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
			smart_str_free(&metadata_str);
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
	smart_str_free(&metadata_str);
	
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
		metadata_str.c = 0;
		if (entry->metadata) {
			PHP_VAR_SERIALIZE_INIT(metadata_hash);
			php_var_serialize(&metadata_str, &entry->metadata, &metadata_hash TSRMLS_CC);
			PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
		}
		mytime = time(NULL);
		phar_set_32(entry_buffer, entry->uncompressed_filesize);
		phar_set_32(entry_buffer+4, mytime);
		phar_set_32(entry_buffer+8, entry->compressed_filesize);
		phar_set_32(entry_buffer+12, entry->crc32);
		phar_set_32(entry_buffer+16, entry->flags);
		phar_set_32(entry_buffer+20, metadata_str.len);
		if (sizeof(entry_buffer) != php_stream_write(newfile, entry_buffer, sizeof(entry_buffer))
		|| metadata_str.len != php_stream_write(newfile, metadata_str.c, metadata_str.len)) {
			smart_str_free(&metadata_str);
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			if (error) {
				spprintf(error, 0, "unable to write temporary manifest of file \"%s\" to manifest of new phar \"%s\"", entry->filename, archive->fname);
			}
			return EOF;
		}
		smart_str_free(&metadata_str);
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
			php_stream_rewind(file);
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
		
		switch(PHAR_SIG_USE) {
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

	if (!is_dir) {
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
	char *internal_file, *key, *error;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry;

	resource = php_url_parse(url);

	if (!resource && (resource = phar_open_url(wrapper, url, "r", 0 TSRMLS_CC)) == NULL) {
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
	char *internal_file;
	char *error;
	phar_entry_data *idata;

	resource = php_url_parse(url);

	if (!resource && (resource = phar_open_url(wrapper, url, "rb", options TSRMLS_CC)) == NULL) {
		return FAILURE;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url);
		return FAILURE;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url);
		return FAILURE;
	}

	if (PHAR_G(readonly)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: write operations disabled by INI setting");
		return FAILURE;
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
		return FAILURE;
	}
	if (error) {
		efree(error);
	}
	if (!idata) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" is not a file in phar \"%s\", cannot unlink", internal_file, resource->host);
		efree(internal_file);
		php_url_free(resource);
		return FAILURE;
	}
	if (idata->internal_file->fp_refcount > 1) {
		/* more than just our fp resource is open for this file */ 
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" in phar \"%s\", has open file pointers, cannot unlink", internal_file, resource->host);
		efree(internal_file);
		php_url_free(resource);
		phar_entry_delref(idata TSRMLS_CC);
		return FAILURE;
	}
	php_url_free(resource);
	efree(internal_file);
	phar_entry_remove(idata, &error TSRMLS_CC);
	if (error) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, error);
		efree(error);
	}
	return SUCCESS;
}
/* }}} */

static int phar_wrapper_rename(php_stream_wrapper *wrapper, char *url_from, char *url_to, int options, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	php_url *resource_from, *resource_to;
	char *from_file, *to_file;
	char *error;
	phar_entry_data *fromdata, *todata;

	if (PHAR_G(readonly)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: write operations disabled by INI setting");
		return FAILURE;
	}

	resource_from = php_url_parse(url_from);
	resource_to = php_url_parse(url_from);

	if (!resource_from && (resource_from = phar_open_url(wrapper, url_from, "r+b", options TSRMLS_CC)) == NULL) {
		return FAILURE;
	}
	
	if (!resource_to && (resource_to = phar_open_url(wrapper, url_to, "wb", options TSRMLS_CC)) == NULL) {
		php_url_free(resource_from);
		return FAILURE;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource_from->scheme || !resource_from->host || !resource_from->path) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url_from);
		return FAILURE;
	}
	
	if (!resource_to->scheme || !resource_to->host || !resource_to->path) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", url_to);
		return FAILURE;
	}

	if (strcasecmp("phar", resource_from->scheme)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url_from);
		return FAILURE;
	}

	if (strcasecmp("phar", resource_to->scheme)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", url_to);
		return FAILURE;
	}

	if (strcmp(resource_from->host, resource_to->host)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot rename \"%s\" to \"%s\", not within the same phar archive", url_from, url_to);
		return FAILURE;
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
		return FAILURE;
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
		return FAILURE;
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
		return FAILURE;
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
		return FAILURE;
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
		return FAILURE;
	}

	php_stream_seek(fromdata->internal_file->fp, 0, SEEK_SET);
	if (fromdata->internal_file->uncompressed_filesize != php_stream_copy_to_stream(fromdata->internal_file->fp, todata->internal_file->fp, PHP_STREAM_COPY_ALL)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: rename failed \"%s\" to \"%s\"", url_from, url_to);
		efree(from_file);
		efree(to_file);
		php_url_free(resource_from);
		php_url_free(resource_to);
		phar_entry_delref(fromdata TSRMLS_CC);
		phar_entry_delref(todata TSRMLS_CC);
		return FAILURE;
	}
	phar_entry_delref(fromdata TSRMLS_CC);
	phar_entry_delref(todata TSRMLS_CC);
	efree(from_file);
	efree(to_file);
	php_url_free(resource_from);
	php_url_free(resource_to);
	phar_wrapper_unlink(wrapper, url_from, 0, 0 TSRMLS_CC);
	return SUCCESS;
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
	char *internal_file, *key, *error;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry;

	resource = php_url_parse(path);

	if (!resource && (resource = phar_open_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
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

	internal_file = resource->path + 1; /* strip leading "/" */
	if (FAILURE == phar_get_archive(&phar, resource->host, strlen(resource->host), NULL, 0, &error TSRMLS_CC)) {
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

PHP_MINIT_FUNCTION(phar) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(phar, php_phar_init_globals_module, NULL);
	REGISTER_INI_ENTRIES();

	phar_object_init(TSRMLS_C);

	return php_register_url_stream_wrapper("phar", &php_stream_phar_wrapper TSRMLS_CC);
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(phar) /* {{{ */
{
	return php_unregister_url_stream_wrapper("phar" TSRMLS_CC);
}
/* }}} */

PHP_RINIT_FUNCTION(phar) /* {{{ */
{
	PHAR_GLOBALS->request_done = 0;
	zend_hash_init(&(PHAR_GLOBALS->phar_fname_map), sizeof(phar_archive_data*), zend_get_hash_value, destroy_phar_data, 0);
	zend_hash_init(&(PHAR_GLOBALS->phar_alias_map), sizeof(phar_archive_data*), zend_get_hash_value, NULL, 0);
	zend_hash_init(&(PHAR_GLOBALS->phar_plain_map), sizeof(const char *), zend_get_hash_value, NULL, 0);
	phar_split_extract_list(TSRMLS_C);
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(phar) /* {{{ */
{
	zend_hash_destroy(&(PHAR_GLOBALS->phar_alias_map));
	PHAR_GLOBALS->phar_fname_map.pDestructor = destroy_phar_data_only;
	zend_hash_destroy(&(PHAR_GLOBALS->phar_fname_map));
	zend_hash_destroy(&(PHAR_GLOBALS->phar_plain_map));
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
	php_info_print_table_row(2, "gzip compression", 
#if HAVE_ZLIB
		"enabled");
#else
		"disabled");
#endif
	php_info_print_table_row(2, "bzip2 compression", 
#if HAVE_BZ2
		"enabled");
#else
		"disabled");
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
	ZEND_MOD_REQUIRED("zlib")
#endif
#if HAVE_BZ2
	ZEND_MOD_REQUIRED("bz2")
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
	PHP_RINIT(phar),
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
