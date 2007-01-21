/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2006 The PHP Group                                |
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

ZEND_DECLARE_MODULE_GLOBALS(phar)

/* if the original value is 0 (disabled), then allow setting/unsetting at will
   otherwise, only allow 1 (enabled), and error on disabling */
ZEND_INI_MH(phar_ini_modify_handler)
{
	zend_bool *p, test;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (zend_bool *) (base+(size_t) mh_arg1);

	if (new_value_length==2 && strcasecmp("on", new_value)==0) {
		*p = (zend_bool) 1;
	}
	else if (new_value_length==3 && strcasecmp("yes", new_value)==0) {
		*p = (zend_bool) 1;
	}
	else if (new_value_length==4 && strcasecmp("true", new_value)==0) {
		*p = (zend_bool) 1;
	}
	else {
		*p = (zend_bool) atoi(new_value);
	}
	if (stage == ZEND_INI_STAGE_STARTUP && !entry->modified) {
		/* this is more efficient than processing orig_value every time */
		if (entry->name_length == 14) { /* phar.readonly */
			PHAR_G(readonly_orig) = *p; 
		} else { /* phar.require_hash */
			PHAR_G(require_hash_orig) = *p;
		} 
	}
	if (stage != ZEND_INI_STAGE_STARTUP) {
		if (entry->name_length == 14) { /* phar.readonly */
			test = PHAR_G(readonly_orig); 
		} else { /* phar.require_hash */
			test = PHAR_G(require_hash_orig);
		} 
		if (test && !*p) {
			/* do not allow unsetting in runtime */
			*p = (zend_bool) 1;
			return FAILURE;
		}
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("phar.readonly",     "1", PHP_INI_ALL, phar_ini_modify_handler, readonly,     zend_phar_globals, phar_globals)
	STD_PHP_INI_BOOLEAN("phar.require_hash", "1", PHP_INI_ALL, phar_ini_modify_handler, require_hash, zend_phar_globals, phar_globals)
PHP_INI_END()

/**
 * When all uses of a phar have been concluded, this frees the manifest
 * and the phar slot
 */
void phar_destroy_phar_data(phar_archive_data *data TSRMLS_DC) /* {{{ */
{
	if (data->alias && data->alias != data->fname) {
		efree(data->alias);
		data->alias = NULL;
	}
	efree(data->fname);
	if (data->signature) {
		efree(data->signature);
	}
	if (data->manifest.arBuckets) {
		zend_hash_destroy(&data->manifest);
	}
	if (data->fp) {
		php_stream_close(data->fp);
	}

	efree(data);
}
/* }}}*/

/**
 * Destroy phar's in shutdown, here we don't care about aliases
 */
static void destroy_phar_data_only(void *pDest) /* {{{ */
{
	phar_archive_data *phar_data = *(phar_archive_data **) pDest;
	TSRMLS_FETCH();

	if (--phar_data->refcount < 0) {
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

	if (entry->fp) {
		php_stream_close(entry->fp);
	}
	if (entry->temp_file) {
		php_stream_close(entry->temp_file);
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
static phar_archive_data * phar_get_archive(char *fname, int fname_len, char *alias, int alias_len TSRMLS_DC) /* {{{ */
{
	phar_archive_data *fd, **fd_ptr;

	if (alias && alias_len) {
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void**)&fd_ptr)) {
			if (fname && (fname_len != (*fd_ptr)->fname_len || strncmp(fname, (*fd_ptr)->fname, fname_len))) {
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
				return NULL;
			}
			return *fd_ptr;
		}
	}
	if (fname && fname_len) {
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void**)&fd_ptr)) {
			fd = *fd_ptr;
			if (alias && alias_len) {
				zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&fd,   sizeof(phar_archive_data*), NULL);
			}
			return fd;
		}
		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), fname, fname_len, (void**)&fd_ptr)) {
			return *fd_ptr;
		}
	}
	return NULL;
}
/* }}} */

/**
 * retrieve information on a file contained within a phar, or null if it ain't there
 */
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry;

	if (path && *path == '/') {
		path++;
		path_len--;
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

/**
 * Retrieve a copy of the file information on a single file within a phar, or null.
 * This also transfers the open file pointer, if any, to the entry.
 */
static phar_entry_data *phar_get_entry_data(char *fname, int fname_len, char *path, int path_len TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry;
	phar_entry_data *ret;
	
	ret = NULL;
	if ((phar = phar_get_archive(fname, fname_len, NULL, 0 TSRMLS_CC)) != NULL) {
		if ((entry = phar_get_entry_info(phar, path, path_len TSRMLS_CC)) != NULL) {
			ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
			ret->phar = phar;
			ret->internal_file = entry;
			if (entry->fp) {
				/* transfer ownership */
				ret->fp = entry->fp;
				php_stream_seek(ret->fp, 0, SEEK_SET);
				entry->fp = 0;
			} else {
				ret->fp = 0;
			}
		}
	}
	return ret;
}
/* }}} */

/**
 * Create a new dummy file slot within a writeable phar for a newly created file
 */
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, etemp;
	phar_entry_data *ret;

	if ((phar = phar_get_archive(fname, fname_len, NULL, 0 TSRMLS_CC)) == NULL) {
		return NULL;
	}

	if (NULL != (ret = phar_get_entry_data(fname, fname_len, path, path_len TSRMLS_CC))) {
		/* reset file size */
		ret->internal_file->uncompressed_filesize = 0;
		ret->internal_file->compressed_filesize = 0;
		ret->internal_file->crc32 = 0;
		ret->internal_file->flags = PHAR_ENT_PERM_DEF_FILE;
		return ret;
	}

	if (PHAR_G(readonly)) {
		/*no need to issue an error here */
		/*php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar error: write operations disabled by INI setting");*/
		return NULL;
	}
	
	/* create a new phar data holder */
	ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	if ((entry = phar_get_entry_info(phar, path, path_len TSRMLS_CC)) == NULL) {
		/* create an entry, this is a new file */
		memset(&etemp, 0, sizeof(phar_entry_info));
		etemp.filename_len = path_len;
		etemp.filename = estrndup(path, path_len);
		etemp.timestamp = time(0);
		etemp.offset_within_phar = -1;
		etemp.is_crc_checked = 1;
		etemp.flags = PHAR_ENT_PERM_DEF_FILE;
		zend_hash_add(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info), NULL);
		/* retrieve the phar manifest copy */
		entry = phar_get_entry_info(phar, path, path_len TSRMLS_CC);
	}
	ret->phar = phar;
	ret->internal_file = entry;
	if (entry->fp) {
		/* transfer ownership */
		ret->fp = entry->fp;
		php_stream_seek(ret->fp, 0, SEEK_SET);
		entry->fp = 0;
	} else {
		ret->fp = 0;
	}
	if (ret->internal_file->temp_file == 0) {
		/* create a temporary stream for our new file */
		ret->internal_file->temp_file = php_stream_fopen_tmpfile();
		ret->internal_file->is_modified = 1;
		ret->phar->is_modified = 1;
	}
	return ret;
}
/* }}} */

#define MAPPHAR_ALLOC_FAIL(msg) \
	php_stream_close(fp);\
	php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, msg, fname);\
	return FAILURE;

#define MAPPHAR_FAIL(msg) \
	efree(savebuf);\
	if (mydata) {\
		efree(mydata);\
	}\
	if (signature) {\
		efree(signature);\
	}\
	MAPPHAR_ALLOC_FAIL(msg)

#ifdef WORDS_BIGENDIAN
# define PHAR_GET_32(buffer, var) \
	var = ((unsigned char)(buffer)[3]) << 24 \
		+ ((unsigned char)(buffer)[2]) << 16 \
		+ ((unsigned char)(buffer)[1]) <<  8 \
		+ ((unsigned char)(buffer)[0]); \
	(buffer) += 4
# define PHAR_GET_16(buffer, var) \
	var = ((unsigned char)(buffer)[1]) <<  8 \
		+ ((unsigned char)(buffer)[0]); \
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
static int phar_open_loaded(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;

	if ((phar = phar_get_archive(fname, fname_len, alias, alias_len TSRMLS_CC)) != NULL
	&& fname_len == phar->fname_len
	&& !strncmp(fname, phar->fname, fname_len)) {
		if (pphar) {
			*pphar = phar;
		}
		return SUCCESS;
	} else {
		if (pphar) {
			*pphar = NULL;
		}
		if (phar && alias && (options & REPORT_ERRORS)) {
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, phar->fname, fname);
		}
		return FAILURE;
	}
}
/* }}}*/

/**
 * Parse out metadata from the manifest for a single file
 *
 * Meta-data is in this format:
 * [type32][len16][data...]
 * 
 * where type32 is a 32-bit type indicator, len16 is a 16-bit length indicator,
 * and data is the actual meta-data.
 */
static int phar_parse_metadata(php_stream *fp, char **buffer, char *endbuffer, zval *metadata TSRMLS_DC) /* {{{ */
{
	zval *dataarray, *found;
	php_uint32 datatype;
	php_uint16 len;
	char *data;

	do {
		/* for each meta-data, add an array index to the metadata array
		   if the index already exists, convert to a sub-array */
		PHAR_GET_32(*buffer, datatype);
		PHAR_GET_16(*buffer, len);
		data = (char *) emalloc(len+1);
		if (endbuffer - *buffer < len) {
			efree(data);
			return FAILURE;
		} else {
			memcpy(data, *buffer, len);
			data[len] = '\0';
		}
		if (SUCCESS == zend_hash_index_find(HASH_OF(metadata), datatype, (void**)&found)) {
			if (Z_TYPE_P(found) == IS_ARRAY) {
				dataarray = found;
			} else {
				MAKE_STD_ZVAL(dataarray);
				array_init(dataarray);
				add_next_index_zval(dataarray, found);
			}
		} else {
			dataarray = metadata;
		}
		add_index_stringl(dataarray, datatype, data, len, 0);
	} while (*(php_uint32 *) *buffer && *buffer < endbuffer);
	*buffer += 4;
	return SUCCESS;
}
/* }}}*/

/**
 * Does not check for a previously opened phar in the cache.
 *
 * Parse a new one and add it to the cache, returning either SUCCESS or 
 * FAILURE, and setting pphar to the pointer to the manifest entry
 * 
 * This is used by phar_open_filename to process the manifest, but can be called
 * directly.
 */
int phar_open_file(php_stream *fp, char *fname, int fname_len, char *alias, int alias_len, long halt_offset, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
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

	/* check for ?>\n and increment accordingly */
	if (-1 == php_stream_seek(fp, halt_offset, SEEK_SET)) {
		MAPPHAR_ALLOC_FAIL("cannot seek to __HALT_COMPILER(); location in phar \"%s\"")
	}

	buffer = b32;
	if (3 != php_stream_read(fp, buffer, 3)) {
		MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	if (*buffer == ' ' && *(buffer + 1) == '?' && *(buffer + 2) == '>') {
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" is API version %1.u.%1.u.%1.u, and cannot be processed", fname, manifest_ver >> 12, (manifest_ver >> 8) & 0xF, (manifest_ver >> 4) & 0x0F);
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
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" has a broken signature", fname);
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
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" has a broken signature", fname);
				return FAILURE;
			}

			sig_len = sizeof(digest) * 2;
			signature = (char*)safe_emalloc(sizeof(digest), 2, 1);
			for(len = 0; len < sizeof(digest); ++len)
			{
				sprintf(signature+len+len, "%02X", digest[len]);
			}
			signature[sizeof(digest) * 2] = '\0';
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
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" has a broken signature", fname);
				return FAILURE;
			}

			sig_len = sizeof(digest) * 2;
			signature = (char*)safe_emalloc(sizeof(digest), 2, 1);
			for(len = 0; len < sizeof(digest); ++len)
			{
				sprintf(signature+len+len, "%02X", digest[len]);
			}
			signature[sizeof(digest) * 2] = '\0';
			break;
		}
		default:
			efree(savebuf);
			php_stream_close(fp);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" has a broken signature", fname);
			return FAILURE;
		}
	} else if (PHAR_G(require_hash)) {
		efree(savebuf);
		php_stream_close(fp);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" does not have a signature", fname);
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
		if (alias && alias_len && (alias_len != tmp_len || strncmp(alias, buffer, tmp_len)))
		{
			buffer[tmp_len] = '\0';
			efree(savebuf);
			php_stream_close(fp);
			if (signature) {
				efree(signature);
			}
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "cannot load phar \"%s\" with implicit alias \"%s\" under different alias \"%s\"", fname, buffer, alias);
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

	/* set up our manifest */
	entry.temp_file = 0;
	mydata = ecalloc(sizeof(phar_archive_data), 1);
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
		if (*(php_uint32 *) buffer) {
			MAKE_STD_ZVAL(entry.metadata);
			array_init(entry.metadata);
			phar_parse_metadata(fp, &buffer, endbuffer, entry.metadata TSRMLS_CC);
		} else {
			buffer += 4;
		}
		entry.offset_within_phar = offset;
		offset += entry.compressed_filesize;
		switch (entry.flags & PHAR_ENT_COMPRESSION_MASK) {
		case PHAR_ENT_COMPRESSED_GZ:
#if !HAVE_ZLIB
			MAPPHAR_FAIL("zlib extension is required for gz compressed .phar file \"%s\"");
#endif
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
#if !HAVE_BZ2
			MAPPHAR_FAIL("bz2 extension is required for bzip2 compressed .phar file \"%s\"");
#endif
			break;
		default:
			if (entry.uncompressed_filesize != entry.compressed_filesize) {
				MAPPHAR_FAIL("internal corruption of phar \"%s\" (compressed and uncompressed size does not match for uncompressed entry)");
			}
			break;
		}
		manifest_flags |= (entry.flags & PHAR_ENT_COMPRESSION_MASK);
		entry.is_crc_checked = 0;
		entry.fp = NULL;
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
static int phar_open_or_create_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
{
	phar_archive_data *mydata;
	int register_alias;
	php_stream *fp;

	if (!pphar) {
		pphar = &mydata;
	}

	if (phar_open_loaded(fname, fname_len, alias, alias_len, options, pphar TSRMLS_CC) == SUCCESS) {
		(*pphar)->is_writeable = 1;
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

	fp = php_stream_open_wrapper(fname, "r+b", IGNORE_URL|STREAM_MUST_SEEK|0, NULL);

	if (fp && phar_open_fp(fp, fname, fname_len, alias, alias_len, options, pphar TSRMLS_CC) == SUCCESS) {
		(*pphar)->is_writeable = 1;
		return SUCCESS;
	}

	if (PHAR_G(readonly)) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "creating archive \"%s\" disabled by INI setting", fname);
		}
		return FAILURE;
	}

	/* set up our manifest */
	mydata = ecalloc(sizeof(phar_archive_data), 1);
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	mydata->fname = estrndup(fname, fname_len);
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	snprintf(mydata->version, sizeof(mydata->version), "%s", PHAR_VERSION_STR);
	mydata->is_explicit_alias = alias ? 1 : 0;
	mydata->internal_file_start = -1;
	mydata->fp = fp;
	mydata->is_writeable = 1;
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

/**
 * Return an already opened filename.
 *
 * Or scan a phar file for the required __HALT_COMPILER(); ?> token and verify
 * that the manifest is proper, then pass it to phar_open_file().  SUCCESS
 * or FAILURE is returned and pphar is set to a pointer to the phar's manifest
 */
int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
{
	php_stream *fp;
	
	if (phar_open_loaded(fname, fname_len, alias, alias_len, options, pphar TSRMLS_CC) == SUCCESS) {
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

	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
	
	if (!fp) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open phar for reading \"%s\"", fname);
		}
		return FAILURE;
	}

	return phar_open_fp(fp, fname, fname_len, alias, alias_len, options, pphar TSRMLS_CC);
}
/* }}}*/

/**
 * Scan an open fp for the required __HALT_COMPILER(); ?> token and verify
 * that the manifest is proper, then pass it to phar_open_file().  SUCCESS
 * or FAILURE is returned and pphar is set to a pointer to the phar's manifest
 */
static int phar_open_fp(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
{
	const char token[] = "__HALT_COMPILER();";
	char *pos, buffer[1024 + sizeof(token)];
	const long readsize = sizeof(buffer) - sizeof(token);
	const long tokenlen = sizeof(token) - 1;
	long halt_offset;

	/* Maybe it's better to compile the file instead of just searching,  */
	/* but we only want the offset. So we want a .re scanner to find it. */

	if (-1 == php_stream_rewind(fp)) {
		MAPPHAR_ALLOC_FAIL("cannot rewind phar \"%s\"")
	}

	buffer[sizeof(buffer)-1] = '\0';
	memset(buffer, 32, sizeof(token));
	halt_offset = 0;
	while(!php_stream_eof(fp)) {
		if (php_stream_read(fp, buffer+tokenlen, readsize) < 0) {
			MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
		}
		if ((pos = strstr(buffer, token)) != NULL) {
			halt_offset += (pos - buffer); /* no -tokenlen+tokenlen here */
			return phar_open_file(fp, fname, fname_len, alias, alias_len, halt_offset, pphar TSRMLS_CC);
		}

		halt_offset += readsize;
		memmove(buffer, buffer + tokenlen, readsize + 1);
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
	char *arch, *entry;
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
			if (phar_open_or_create_filename(resource->host, arch_len, NULL, 0, options, NULL TSRMLS_CC) == FAILURE)
			{
				php_url_free(resource);
				return NULL;
			}
		} else {
			if (phar_open_filename(resource->host, arch_len, NULL, 0, options, NULL TSRMLS_CC) == FAILURE)
			{
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
int phar_open_compiled_file(char *alias, int alias_len TSRMLS_DC) /* {{{ */
{
	char *fname;
	long halt_offset;
	zval *halt_constant;
	php_stream *fp;
	int fname_len;

	fname = zend_get_executed_filename(TSRMLS_C);
	
	fname_len = strlen(fname);

	if (alias && phar_open_loaded(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	if (!strcmp(fname, "[no active file]")) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "cannot initialize a phar outside of PHP execution");
		return FAILURE;
	}

	MAKE_STD_ZVAL(halt_constant);
	if (0 == zend_get_constant("__COMPILER_HALT_OFFSET__", 24, halt_constant TSRMLS_CC)) {
		FREE_ZVAL(halt_constant);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "__HALT_COMPILER(); must be declared in a phar");
		return FAILURE;
	}
	halt_offset = Z_LVAL(*halt_constant);
	FREE_ZVAL(halt_constant);
	
	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open phar for reading \"%s\"", fname);
		return FAILURE;
	}

	return phar_open_file(fp, fname, fname_len, alias, alias_len, halt_offset, NULL TSRMLS_CC);
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
    NULL,                  /* rename */
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
	char *filter_name;
	char tmpbuf[8];
	php_url *resource = NULL;
	php_stream *fp, *fpf;
	php_stream_filter *filter, *consumed;
	php_uint32 offset;

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

	/* strip leading "/" */
	internal_file = estrdup(resource->path + 1);
	if (mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+')) {
		if (NULL == (idata = phar_get_or_create_entry_data(resource->host, strlen(resource->host), internal_file, strlen(internal_file) TSRMLS_CC))) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" could not be created in phar \"%s\"", internal_file, resource->host);
			efree(internal_file);
			php_url_free(resource);
			return NULL;
		}
		fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
		idata->phar->refcount++;
		php_url_free(resource);
		efree(internal_file);
		return fpf;
	} else {
		if (NULL == (idata = phar_get_entry_data(resource->host, strlen(resource->host), internal_file, strlen(internal_file) TSRMLS_CC))) {
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
		idata->phar->refcount++;
		efree(internal_file);
		return fpf;
	}

#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(idata->phar->fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		efree(idata);
		efree(internal_file);
		return NULL;
	}
#endif
	
	if (php_check_open_basedir(idata->phar->fname TSRMLS_CC)) {
		efree(idata);
		efree(internal_file);
		return NULL;
	}

	fp = idata->phar->fp;

	if (!fp) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot open phar \"%s\"", idata->phar->fname);
		efree(idata);
		efree(internal_file);
		return NULL;
	}

	/* seek to start of internal file and read it */
	offset = idata->phar->internal_file_start + idata->internal_file->offset_within_phar;
	if (-1 == php_stream_seek(fp, offset, SEEK_SET)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (cannot seek to start of file \"%s\" at offset \"%d\")",
			idata->phar->fname, internal_file, offset);
		efree(idata);
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
			efree(idata);
			efree(internal_file);
			return NULL;			
		}
		/* Unfortunately we cannot check the read position of fp after getting */
		/* uncompressed data because the new stream posiition is being changed */
		/* by the number of bytes read throughthe filter not by the raw number */
		/* bytes being consumed on the stream. Therefore use a consumed filter. */ 
		consumed = php_stream_filter_create("consumed", NULL, php_stream_is_persistent(fp) TSRMLS_CC);
		php_stream_filter_append(&fp->readfilters, consumed);
		php_stream_filter_append(&fp->readfilters, filter);

		idata->fp = php_stream_temp_new();
		if (php_stream_copy_to_stream(fp, idata->fp, idata->internal_file->uncompressed_filesize) != idata->internal_file->uncompressed_filesize) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
			php_stream_close(idata->fp);
			efree(idata);
			efree(internal_file);
			return NULL;
		}
		php_stream_filter_flush(filter, 1);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
		php_stream_filter_flush(consumed, 1);
		php_stream_filter_remove(consumed, 1 TSRMLS_CC);
		if (offset + idata->internal_file->compressed_filesize != php_stream_tell(fp)) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
			php_stream_close(idata->fp);
			efree(idata);
			efree(internal_file);
			return NULL;
		}
		php_stream_seek(fp, offset + idata->internal_file->compressed_filesize, SEEK_SET);
	} else { /* from here is for non-compressed */
		buffer = &tmpbuf[0];
		/* bypass to temp stream */
		idata->fp = php_stream_temp_new();
		if (php_stream_copy_to_stream(fp, idata->fp, idata->internal_file->uncompressed_filesize) != idata->internal_file->uncompressed_filesize) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->fname, internal_file);
			php_stream_close(idata->fp);
			efree(idata);
			efree(internal_file);
			return NULL;
		}
	}

	/* check length, crc32 */
	if (phar_postprocess_file(wrapper, options, idata, idata->internal_file->crc32 TSRMLS_CC) != SUCCESS) {
		/* already issued the error */
		php_stream_close(idata->fp);
		efree(idata);
		efree(internal_file);
		return NULL;
	}
	idata->internal_file->is_crc_checked = 1;

	fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
	idata->phar->refcount++;
	efree(internal_file);
	return fpf;
}
/* }}} */

/**
 * Used for fclose($fp) where $fp is a phar archive
 */
static int phar_stream_close(php_stream *stream, int close_handle TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	/* data->fp is the temporary memory stream containing this file's data */
	if (data->fp) {
		/* only close if we have a cached temp memory stream */
		if (data->internal_file->fp) {
			php_stream_close(data->fp);
			data->fp = 0;
		} else {
			/* else transfer ownership back */
			data->internal_file->fp = data->fp;
		}
	}
	if (--data->phar->refcount < 0) {
		phar_destroy_phar_data(data->phar TSRMLS_CC);
	}

	efree(data);
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
	got = php_stream_read(data->fp, buf, count);
	
	if (data->fp->eof) {
		stream->eof = 1;
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

	int res = php_stream_seek(data->fp, offset, whence);
	*newoffset = php_stream_tell(data->fp);
	return res;
}
/* }}} */

/**
 * Used for writing to a phar file
 */
static size_t phar_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *) stream->abstract;

	if (data->internal_file->temp_file == 0) {
		/* create a new temporary stream for our new file */
		data->internal_file->temp_file = php_stream_fopen_tmpfile();
	}
	if (count != php_stream_write(data->internal_file->temp_file, buf, count)) {
		php_stream_close(data->internal_file->temp_file);
		data->internal_file->temp_file = 0;
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar error: Could not write %d characters to \"%s\" in phar \"%s\"", (int) count, data->internal_file->filename, data->phar->fname);
		return -1;
	}
	data->internal_file->uncompressed_filesize += count;
	data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize; 
	data->internal_file->is_modified = 1;
	data->phar->is_modified = 1;
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

int phar_flush(phar_entry_data *data TSRMLS_DC) /* {{{ */
{
	static const char newprologue[] = "<?php __HALT_COMPILER();";
	phar_entry_info *entry;
	int alias_len, fname_len, halt_offset, restore_alias_len, global_flags = 0;
	char *fname, *alias;
	char manifest[18], entry_buffer[24];
	off_t manifest_ftell;
	long offset;
	php_uint32 copy, loc, new_manifest_count;
	php_uint32 newcrc32;
	php_stream *file, *oldfile, *newfile, *compfile;
	php_stream_filter *filter;

	if (PHAR_G(readonly)) {
		return EOF;
	}

	oldfile = php_stream_open_wrapper(data->phar->fname, "rb", 0, NULL);
	newfile = php_stream_fopen_tmpfile();
	filter  = 0;

	if (data->phar->halt_offset && oldfile) {
		if (data->phar->halt_offset != php_stream_copy_to_stream(oldfile, newfile, data->phar->halt_offset)) {
			if (oldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to copy prologue of old phar to new phar \"%s\"", data->phar->fname);
			return EOF;
		}
	} else {
		/* this is a brand new phar */
		data->phar->halt_offset = sizeof(newprologue)-1;
		if (sizeof(newprologue)-1 != php_stream_write(newfile, newprologue, sizeof(newprologue)-1)) {
			if (oldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to create prologue in new phar \"%s\"", data->phar->fname);
			return EOF;
		}
	}
	manifest_ftell = php_stream_tell(newfile);
	
	/* compress as necessary, calculate crcs, manifest size, and file sizes */
	new_manifest_count = 0;
	offset = 0;
	for (zend_hash_internal_pointer_reset(&data->phar->manifest);
	    zend_hash_has_more_elements(&data->phar->manifest) == SUCCESS;
	    zend_hash_move_forward(&data->phar->manifest)) {
		if (zend_hash_get_current_data(&data->phar->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->is_deleted) {
			/* remove this from the new phar */
			continue;
		}
		/* after excluding deleted files, calculate manifest size in bytes and number of entries */
		++new_manifest_count;
		offset += 4 + entry->filename_len + sizeof(entry_buffer);

		/* compress as necessary */
		if (entry->is_modified) {
			if (!entry->temp_file) {
				/* nothing to do here */
				continue;
			}
			php_stream_rewind(entry->temp_file);
			file = entry->temp_file;
			copy = entry->uncompressed_filesize;
			if (!(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
				php_stream_rewind(file);
				newcrc32 = ~0;
				for (loc = 0;loc < copy; loc++) {
					CRC32(newcrc32, php_stream_getc(file));
				}
				entry->crc32 = ~newcrc32;
				entry->is_crc_checked = 1;
			}
		} else {
			if (-1 == php_stream_seek(oldfile, entry->offset_within_phar + data->phar->internal_file_start, SEEK_SET)) {
				if (oldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, data->phar->fname);
				return EOF;
			}
			file = oldfile;
			copy = entry->uncompressed_filesize;
		}
		if (entry->flags & PHAR_ENT_COMPRESSION_MASK) {
			filter = php_stream_filter_create(phar_compress_filter(entry, 0), NULL, 0 TSRMLS_CC);
			if (!filter) {
				if (oldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				if (entry->flags & PHAR_ENT_COMPRESSED_GZ) {
					php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to gzip compress file \"%s\" to new phar \"%s\"", entry->filename, data->phar->fname);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to bzip2 compress file \"%s\" to new phar \"%s\"", entry->filename, data->phar->fname);
				}
				return EOF;
			}
			php_stream_filter_append(&file->readfilters, filter);

			/* create new file that holds the compressed version */
			compfile = php_stream_fopen_tmpfile();
			entry->compressed_filesize = php_stream_copy_to_stream(file, compfile, entry->uncompressed_filesize);
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			/* generate crc on compressed file */
			php_stream_rewind(compfile);
			newcrc32 = ~0;
			for (loc = entry->compressed_filesize; loc > 0; --loc) {
				CRC32(newcrc32, php_stream_getc(compfile));
			}
			entry->crc32 = ~newcrc32;
			entry->is_crc_checked = 1;
			if (entry->temp_file) {
				/* no longer need the uncompressed contents */
				php_stream_close(entry->temp_file);
			}
			/* use temp_file to store the newly compressed data */
			entry->temp_file = compfile;
			entry->compressed_filesize = copy;
			entry->is_modified = 1;
			global_flags |= (entry->flags & PHAR_ENT_COMPRESSION_MASK);
		}
	}
	
	global_flags |= PHAR_HDR_SIGNATURE;

	/* write out manifest pre-header */
	/*  4: manifest length
	 *  4: manifest entry count
	 *  2: phar version
	 *  4: phar global flags
	 *  4: alias length, the rest is the alias itself
	 */
	restore_alias_len = data->phar->alias_len;
	if (!data->phar->is_explicit_alias) {
		data->phar->alias_len = 0;
	}

	phar_set_32(manifest, offset + data->phar->alias_len + sizeof(manifest) - 4); /* manifest length */
	phar_set_32(manifest+4, new_manifest_count);
	*(manifest + 8) = (unsigned char) (((PHAR_API_VERSION) >> 8) & 0xFF);
	*(manifest + 9) = (unsigned char) (((PHAR_API_VERSION) & 0xF0));
	phar_set_32(manifest+10, global_flags);
	phar_set_32(manifest+14, data->phar->alias_len);

	/* write the manifest header */
	if (sizeof(manifest) != php_stream_write(newfile, manifest, sizeof(manifest))
	|| data->phar->alias_len != php_stream_write(newfile, data->phar->alias, data->phar->alias_len)) {
		if (oldfile) {
			php_stream_close(oldfile);
		}
		php_stream_close(newfile);
		data->phar->alias_len = restore_alias_len;
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write manifest meta-data of new phar \"%s\"", data->phar->fname);
		return EOF;
	}
	data->phar->alias_len = restore_alias_len;
	
	/* re-calculate the manifest location to simplify later code */
	manifest_ftell = php_stream_tell(newfile);
	
	/* now write the manifest */
	for (zend_hash_internal_pointer_reset(&data->phar->manifest);
	    zend_hash_has_more_elements(&data->phar->manifest) == SUCCESS;
	    zend_hash_move_forward(&data->phar->manifest)) {
		if (zend_hash_get_current_data(&data->phar->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->is_deleted) {
			/* remove this from the new phar */
			continue;
		}
		phar_set_32(entry_buffer, entry->filename_len);
		if (4 != php_stream_write(newfile, entry_buffer, 4)
		|| entry->filename_len != php_stream_write(newfile, entry->filename, entry->filename_len)) {
			if (oldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write filename of file \"%s\" to manifest of new phar \"%s\"", entry->filename, data->phar->fname);
			return EOF;
		}
		/* set the manifest meta-data:
			4: uncompressed filesize
			4: creation timestamp
			4: compressed filesize
			4: crc32
			4: flags
			4+: metadata TODO: copy the actual metadata, 0 for now
		*/
		copy = time(NULL);
		phar_set_32(entry_buffer, entry->uncompressed_filesize);
		phar_set_32(entry_buffer+4, copy);
		phar_set_32(entry_buffer+8, entry->compressed_filesize);
		phar_set_32(entry_buffer+12, entry->crc32);
		phar_set_32(entry_buffer+16, entry->flags);
		copy = 0;
		phar_set_32(entry_buffer+20, 0);
		if (sizeof(entry_buffer) != php_stream_write(newfile, entry_buffer, sizeof(entry_buffer))) {
			if (oldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write temporary manifest of file \"%s\" to manifest of new phar \"%s\"", entry->filename, data->phar->fname);
			return EOF;
		}
	}
	
	/* now copy the actual file data to the new phar */
	offset = 0;
	for (zend_hash_internal_pointer_reset(&data->phar->manifest);
	    zend_hash_has_more_elements(&data->phar->manifest) == SUCCESS;
	    zend_hash_move_forward(&data->phar->manifest)) {
		if (zend_hash_get_current_data(&data->phar->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->is_deleted) {
			/* remove this from the new phar */
			continue;
		}
		if (entry->is_modified && entry->temp_file) {
			php_stream_rewind(entry->temp_file);
			file = entry->temp_file;
		} else {
			if (-1 == php_stream_seek(oldfile, entry->offset_within_phar + data->phar->internal_file_start, SEEK_SET)) {
				if (oldfile) {
					php_stream_close(oldfile);
				}
				php_stream_close(newfile);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, data->phar->fname);
				return EOF;
			}
			file = oldfile;
		}
		/* this will have changed for all files that have either
		   changed compression or been modified */
		entry->offset_within_phar = offset;
		offset += entry->compressed_filesize;
		if (entry->compressed_filesize != php_stream_copy_to_stream(file, newfile, entry->compressed_filesize)) {
			if (oldfile) {
				php_stream_close(oldfile);
			}
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write contents of file \"%s\" to new phar \"%s\"", entry->filename, data->phar->fname);
			return EOF;
		}
		/* close the temporary file, no longer needed */
		if (entry->is_modified) {
			if (entry->temp_file) {
				php_stream_close(entry->temp_file);
				entry->temp_file = 0;
				entry->is_modified = 0;
			}
		}
	}

	/* append signature */
	if (global_flags & PHAR_HDR_SIGNATURE) {
		unsigned char buf[1024];
		int  sig_flags = 0, len;
		char sig_buf[4];

		php_stream_rewind(newfile);
		
		switch(PHAR_SIG_USE) {
		case PHAR_SIG_PGP:
			/* TODO: currently fall back to sha1,later do both */
		default:
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			PHP_SHA1Init(&context);
			while ((len = php_stream_read(newfile, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA1Update(&context, buf, len);
			}
			PHP_SHA1Final(digest, &context);
			php_stream_write(newfile, (char *) digest, sizeof(digest));
			sig_flags |= PHAR_SIG_SHA1;
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			PHP_MD5Init(&context);
			while ((len = php_stream_read(newfile, (char*)buf, sizeof(buf))) > 0) {
				PHP_MD5Update(&context, buf, len);
			}
			PHP_MD5Final(digest, &context);
			php_stream_write(newfile, (char *) digest, sizeof(digest));
			sig_flags |= PHAR_SIG_MD5;
			break;
		}
		}
		phar_set_32(sig_buf, sig_flags);
		php_stream_write(newfile, sig_buf, 4);
		php_stream_write(newfile, "GBMB", 4);
	}

	/* finally, close the temp file, rename the original phar,
	   move the temp to the old phar, unlink the old phar, and reload it into memory
	*/
	php_stream_rewind(newfile);
	file = php_stream_open_wrapper(data->phar->fname, "wb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
	if (!file) {
		if (oldfile) {
			php_stream_close(oldfile);
		}
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open new phar \"%s\" for writing", data->phar->fname);
		return EOF;
	}
	php_stream_copy_to_stream(newfile, file, PHP_STREAM_COPY_ALL);
	php_stream_close(newfile);
	php_stream_close(file);

	file = php_stream_open_wrapper(data->phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!file) {
		if (oldfile) {
			php_stream_close(oldfile);
		}
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open new phar \"%s\" for reading", data->phar->fname);
		return EOF;
	}

	if (-1 == php_stream_seek(file, data->phar->halt_offset, SEEK_SET)) {
		if (oldfile) {
			php_stream_close(oldfile);
		}
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to __HALT_COMPILER(); in new phar \"%s\"", data->phar->fname);
		return EOF;
	}

	if (oldfile) {
		php_stream_close(oldfile);
	}
	if (data->fp) {
		php_stream_close(data->fp);
		data->fp = 0;
	}
	if (data->phar->fp) {
		/* we will re-open this later */
		php_stream_close(data->phar->fp);
		data->phar->fp = 0;
	}

	fname = estrndup(data->phar->fname, data->phar->fname_len);
	fname_len = data->phar->fname_len;
	if (data->phar->is_explicit_alias) {
		alias = estrndup(data->phar->alias, data->phar->alias_len);
	} else {
		alias = 0;
	}
	alias_len = data->phar->alias_len;
	halt_offset = data->phar->halt_offset;
	zend_hash_apply_with_argument(&(PHAR_GLOBALS->phar_alias_map), phar_unalias_apply, data->phar TSRMLS_CC);
	zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len);
	phar_open_file(file, fname, fname_len, alias, alias_len, halt_offset, NULL TSRMLS_CC);
	efree(fname);
	if (alias) {
		efree(alias);
	}
	return EOF;
}
/* }}} */

/**
 * Used to save work done on a writeable phar
 */
static int phar_stream_flush(php_stream *stream TSRMLS_DC) /* {{{ */
{
	if (stream->mode[0] == 'w' || (stream->mode[0] == 'r' && stream->mode[1] == '+')) {
		return phar_flush((phar_entry_data *)stream->abstract TSRMLS_CC);
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
	ssb->sb.st_ino = zend_get_hash_value(tmp, tmp_len);
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
	char *internal_file, *key;
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
	if ((phar = phar_get_archive(resource->host, strlen(resource->host), NULL, 0 TSRMLS_CC)) != NULL) {
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

	zend_hash_internal_pointer_reset(manifest);
	while (FAILURE != zend_hash_has_more_elements(manifest)) {
		if (HASH_KEY_NON_EXISTANT == zend_hash_get_current_key_ex(manifest, &key, &keylen, &unused, 0, NULL)) {
			break;
		}
		if (*dir == '/') {
			/* root directory */
			if (NULL != (found = (char *) memchr(key, '/', keylen))) {
				/* the entry has a path separator and is a subdirectory */
				entry = (char *) emalloc (found - key + 1);
				memcpy(entry, key, found - key);
				keylen = found - key;
				entry[keylen] = '\0';
			} else {
				entry = (char *) emalloc (keylen + 1);
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
			entry = (char *) emalloc (found - save + dirlen + 1);
			memcpy(entry, save + dirlen + 1, found - save - dirlen - 1);
			keylen = found - save - dirlen - 1;
			entry[keylen] = '\0';
		} else {
			/* is file */
			save -= dirlen + 1;
			entry = (char *) emalloc (keylen - dirlen + 1);
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
	if (NULL == (idata = phar_get_entry_data(resource->host, strlen(resource->host), internal_file, strlen(internal_file) TSRMLS_CC))) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" is not a file in phar \"%s\", cannot unlink", internal_file, resource->host);
		efree(internal_file);
		php_url_free(resource);
		return FAILURE;
	}
	if (idata->internal_file->is_modified) {
		idata->internal_file->is_modified = 0;
		if (idata->internal_file->temp_file) {
			php_stream_close(idata->internal_file->temp_file);
			idata->internal_file->temp_file = 0;
		}
	}
	idata->internal_file->is_deleted = 1;
	efree(internal_file);
	if (idata->fp) {
		php_stream_close(idata->fp);
	}
	efree(idata);
	/* we need to "flush" the stream to save the newly deleted file on disk */
	idata = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	idata->fp = 0;
	idata->phar = phar_get_archive(resource->host, strlen(resource->host), 0, 0 TSRMLS_CC);
	phar_flush(idata TSRMLS_CC);
	php_url_free(resource);
	efree(idata);
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
	char *internal_file, *key;
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
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory", path, resource->host);
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
	if ((phar = phar_get_archive(resource->host, strlen(resource->host), NULL, 0 TSRMLS_CC)) != NULL) {
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
	zend_hash_init(&(PHAR_GLOBALS->phar_fname_map), sizeof(phar_archive_data*), zend_get_hash_value, destroy_phar_data, 0);
	zend_hash_init(&(PHAR_GLOBALS->phar_alias_map), sizeof(phar_archive_data*), zend_get_hash_value, NULL, 0);
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(phar) /* {{{ */
{
	zend_hash_destroy(&(PHAR_GLOBALS->phar_alias_map));
	PHAR_GLOBALS->phar_fname_map. pDestructor = destroy_phar_data_only;
	zend_hash_destroy(&(PHAR_GLOBALS->phar_fname_map));
	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(phar) /* {{{ */
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Phar: PHP Archive support", "enabled");
	php_info_print_table_row(2, "Phar API version", PHAR_VERSION_STR);
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
	php_info_print_table_row(1, "Phar based on pear/PHP_Archive, original concept by Davey Shafik and fully realized by Gregory Beaver");
	php_info_print_table_end();
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
	PHAR_VERSION_STR,
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
