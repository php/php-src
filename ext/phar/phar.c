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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/url.h"
#include "ext/standard/crc32.h"
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_directory.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_interfaces.h"
#include "zend_operators.h"
#include "zend_qsort.h"
#include "php_phar.h"
#include "main/php_streams.h"
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifndef TRUE
 #       define TRUE 1
 #       define FALSE 0
#endif

#ifndef E_RECOVERABLE_ERROR
#define E_RECOVERABLE_ERROR E_ERROR
#endif

#define PHAR_VERSION_STR          "0.8.0"
/* x.y.z maps to 0xyz0 */
#define PHAR_API_VERSION          0x0800
#define PHAR_API_MAJORVERSION     0x0000
#define PHAR_API_MAJORVER_MASK    0xF000
#define PHAR_API_VER_MASK         0xFFF0

#define PHAR_HDR_ANY_COMPRESSED   0x0001
#define PHAR_HDR_SIGNATURE        0x0008

/* flags byte for each file adheres to these bitmasks.
   All unused values are reserved */
#define PHAR_ENT_COMPRESSION_MASK 0x0F
#define PHAR_ENT_COMPRESSED_NONE  0x00
#define PHAR_ENT_COMPRESSED_GZ    0x01
#define PHAR_ENT_COMPRESSED_BZ2   0x02

/* an entry has been marked as deleted from a writeable phar */
#define PHAR_ENT_DELETED	  0x10
/* an entry has been modified, and should be saved */
#define PHAR_ENT_MODIFIED	  0x20


ZEND_BEGIN_MODULE_GLOBALS(phar)
	HashTable   phar_fname_map;
	HashTable   phar_alias_map;
ZEND_END_MODULE_GLOBALS(phar)

ZEND_DECLARE_MODULE_GLOBALS(phar)

#ifndef php_uint16
# if SIZEOF_SHORT == 2
#  define php_uint16 unsigned short
# else
#  define php_uint16 uint16_t
# endif
#endif

typedef union _phar_archive_object  phar_archive_object;
typedef union _phar_entry_object    phar_entry_object;

/* entry for one file in a phar file */
typedef struct _phar_manifest_entry {
	php_uint32               filename_len;
	char                     *filename;
	php_uint32               uncompressed_filesize;
	php_uint32               timestamp;
	long                     offset_within_phar;
	php_uint32               compressed_filesize;
	php_uint32               crc32;
	char                     flags;
	zend_bool                crc_checked;
	php_stream               *fp;
	php_stream		 *temp_file;
} phar_entry_info;

/* information about a phar file (the archive itself) */
typedef struct _phar_archive_data {
	char                     *fname;
	int                      fname_len;
	char                     *alias;
	int                      alias_len;
	char                     version[12];
	size_t                   internal_file_start;
	size_t                   halt_offset;
	zend_bool                has_compressed_files;
	HashTable                manifest;
	php_uint32               min_timestamp;
	php_uint32               max_timestamp;
	php_stream               *fp;
	int                      refcount;
} phar_archive_data;

/* stream access data for one file entry in a phar file */
typedef struct _phar_entry_data {
	phar_archive_data        *phar;
	php_stream               *fp;
	phar_entry_info          *internal_file;
} phar_entry_data;

/* archive php object */
union _phar_archive_object {
	zend_object              std;
	spl_filesystem_object    spl;
	struct {
	    zend_object          std;
	    phar_archive_data    *archive;
	} arc;
};

/* entry php object */
union _phar_entry_object {
	zend_object              std;
	spl_filesystem_object    spl;
	struct {
	    zend_object          std;
	    phar_entry_info      *entry;
	} ent;
};

/* {{{ forward declarations */
static int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar TSRMLS_DC);

static php_stream *php_stream_phar_url_wrapper(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static int phar_close(php_stream *stream, int close_handle TSRMLS_DC);
static int phar_closedir(php_stream *stream, int close_handle TSRMLS_DC);
static int phar_seekdir(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
static size_t phar_read(php_stream *stream, char *buf, size_t count TSRMLS_DC);
static size_t phar_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int phar_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);

static size_t phar_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t phar_dirwrite(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static int phar_flush(php_stream *stream TSRMLS_DC);
static int phar_dirflush(php_stream *stream TSRMLS_DC);
static int phar_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);

static int phar_stream_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);
static php_stream *phar_opendir(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
/* }}} */

static zend_class_entry *phar_ce_archive;
static zend_class_entry *phar_ce_entry;

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
	efree(data->fname);
	if (data->manifest.arBuckets) {
		zend_hash_destroy(&data->manifest);
	}
	if (data->fp) {
		php_stream_close(data->fp);
	}
	data->fp = 0;
	efree(data);
}
/* }}}*/

/**
 * Filename map destructor
 */
static void destroy_phar_data(void *pDest) /* {{{ */
{
	phar_archive_data *phar_data = *(phar_archive_data **) pDest;
	TSRMLS_FETCH();

	if (--phar_data->refcount < 0) {
		phar_destroy_phar_data(phar_data TSRMLS_CC);
	}
}
/* }}}*/

/**
 * from spl_directory
 */
static void phar_spl_foreign_dtor(spl_filesystem_object *object TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar_data = (phar_archive_data *) object->oth;

	if (--phar_data->refcount < 0) {
		phar_destroy_phar_data(phar_data TSRMLS_CC);
	}
}
/* }}} */

/**
 * from spl_directory
 */
static void phar_spl_foreign_clone(spl_filesystem_object *src, spl_filesystem_object *dst TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar_data = (phar_archive_data *) dst->oth;

	phar_data->refcount++;
}
/* }}} */

/**
 * from spl_directory
 */
static spl_other_handler phar_spl_foreign_handler = {
	phar_spl_foreign_dtor,
	phar_spl_foreign_clone
};

/**
 * destructor for the manifest hash, frees each file's entry
 */
static void destroy_phar_manifest(void *pDest) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;

	if (entry->fp) {
		TSRMLS_FETCH();

		php_stream_close(entry->fp);
	}
	if (entry->temp_file) {
		TSRMLS_FETCH();

		php_stream_close(entry->temp_file);
	}
	entry->fp = 0;
	entry->temp_file = 0;
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
static phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len TSRMLS_DC) /* {{{ */
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
static phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len TSRMLS_DC) /* {{{ */
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
		return ret;
	}
	/* create an entry, this is a new file */
	if ((entry = phar_get_entry_info(phar, path, path_len TSRMLS_CC)) != NULL) {
		ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
		entry = (phar_entry_info *) emalloc(sizeof(phar_entry_info));
		etemp.filename_len = path_len;
		etemp.filename = estrndup(path, path_len);
		etemp.uncompressed_filesize = 0;
		etemp.compressed_filesize = 0;
		etemp.timestamp = time(0);
		etemp.offset_within_phar = -1;
		etemp.crc32 = 0;
		etemp.crc_checked = TRUE;
		etemp.fp = NULL;
		etemp.temp_file = 0;
		memcpy((void *) entry, (void *) &etemp, sizeof(phar_entry_info));
		zend_hash_add(&phar->manifest, etemp.filename, path_len, (void*)entry, sizeof(phar_entry_info), NULL);
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
		/* create atemporary stream for our new file */
		ret->internal_file->temp_file = php_stream_fopen_tmpfile();
		ret->internal_file->flags |= PHAR_ENT_MODIFIED;
	}
	return ret;
}
/* }}} */

/* {{{ proto string apiVersion()
 * Returns the api version */
PHP_METHOD(Phar, apiVersion)
{
	RETURN_STRINGL(PHAR_VERSION_STR, sizeof(PHAR_VERSION_STR)-1, 1);
}
/* }}}*/

/* {{{ proto bool canCompress()
 * Returns whether phar extension supports compression using zlib */
PHP_METHOD(Phar, canCompress)
{
#if HAVE_ZLIB || HAVE_BZ2
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

#define MAPPHAR_ALLOC_FAIL(msg) \
	php_stream_close(fp);\
	php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, msg, fname);\
	return FAILURE;

#define MAPPHAR_FAIL(msg) \
	efree(savebuf);\
	MAPPHAR_ALLOC_FAIL(msg)

#ifdef WORDS_BIGENDIAN
# define PHAR_GET_32(buffer, var) \
	var = ((unsigned char)buffer[3]) << 24 \
		+ ((unsigned char)buffer[2]) << 16 \
		+ ((unsigned char)buffer[1]) <<  8 \
		+ ((unsigned char)buffer[0]); \
	buffer += 4
#else
# define PHAR_GET_32(buffer, var) \
	var = *(php_uint32*)(buffer); \
	buffer += 4
#endif

/**
 * retrieve a previously opened phar manifest from the cache, or parse a new
 * one and add it to the cache, returning either SUCCESS or FAILURE, and setting
 * pphar to the pointer to the manifest entry
 * 
 * This is used by phar_open_filename to process the manifest, but can be called
 * directly.
 *
 * If reload is TRUE, this will overwrite the existing entry (essentially reload the information)
 */
static int phar_open_file(php_stream *fp, char *fname, int fname_len, char *alias, int alias_len, long halt_offset, phar_archive_data** pphar, zend_bool reload TSRMLS_DC) /* {{{ */
{
	char b32[4], *buffer, *endbuffer, *savebuf;
	phar_archive_data *mydata;
	phar_entry_info entry;
	php_uint32 manifest_len, manifest_count, manifest_index, tmp_len;
	php_uint16 manifest_tag;
	long offset;
	int compressed = 0;
	int register_alias;

	if (!reload && pphar) {
		*pphar = NULL;
	}

	if ((mydata = phar_get_archive(fname, fname_len, alias, alias_len TSRMLS_CC)) != NULL) {
		if (reload) {
			/* start over */
			if (mydata->manifest.arBuckets) {
				zend_hash_destroy(&mydata->manifest);
				mydata->manifest.arBuckets = 0;
			}
		} else {
			/* Overloading or reloading an archive would only be possible if we  */
			/* refcount everything to be sure no stream for any file in the */
			/* archive is open. */
			if (fname_len != mydata->fname_len || strncmp(fname, mydata->fname, fname_len)) {
				php_stream_close(fp);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, mydata->fname, fname);
				return FAILURE;
			} else {
				if (pphar) {
					*pphar = mydata;
				}		
				php_stream_close(fp);
				return SUCCESS;
			}
		}
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

	/* extract API version and global compressed flag */
	manifest_tag = (((unsigned char)buffer[0]) <<  8)
		+ ((unsigned char)buffer[1]);
	buffer += 2;
	if ((manifest_tag & PHAR_API_VER_MASK) < PHAR_API_VERSION ||
		    (manifest_tag & PHAR_API_MAJORVER_MASK) != PHAR_API_MAJORVERSION)
	{
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar \"%s\" is API version %1.u.%1.u.%1.u, and cannot be processed", fname, manifest_tag >> 12, (manifest_tag >> 8) & 0xF, (manifest_tag >> 4) & 0x0F);
		efree(savebuf);
		return FAILURE;
	}
	/* The lowest nibble contains the phar wide flags. The any compressed can */
	/* be ignored on reading because it is being generated anyways. */

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
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "cannot load phar \"%s\" with implicit alias \"%s\" under different alias \"%s\"", fname, buffer, alias);
			efree(savebuf);
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
	mydata = emalloc(sizeof(phar_archive_data));
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	offset = 0;
	mydata->min_timestamp = 0;
	mydata->max_timestamp = 0;
	for (manifest_index = 0; manifest_index < manifest_count; manifest_index++) {
		if (buffer + 4 > endbuffer) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest entry)")
		}
		PHAR_GET_32(buffer, entry.filename_len);
		if (entry.filename_len == 0) {
			MAPPHAR_FAIL("zero-length filename encountered in phar \"%s\"");
		}
		if (buffer + entry.filename_len + 16 + 1 > endbuffer) {
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
		entry.offset_within_phar = offset;
		offset += entry.compressed_filesize;
		entry.flags = *buffer++;
		switch (entry.flags & PHAR_ENT_COMPRESSION_MASK) {
		case PHAR_ENT_COMPRESSED_GZ:
#if !HAVE_ZLIB
			MAPPHAR_FAIL("zlib extension is required for gz compressed .phar file \"%s\"");
#endif
			compressed = 1;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
#if !HAVE_BZ2
			MAPPHAR_FAIL("bz2 extension is required for bzip2 compressed .phar file \"%s\"");
#endif
			compressed = 1;
			break;
		default:
			if (entry.uncompressed_filesize != entry.compressed_filesize) {
				MAPPHAR_FAIL("internal corruption of phar \"%s\" (compressed and uncompressed size does not match for uncompressed entry)");
			}
			break;
		}
		entry.crc_checked = 0;
		entry.fp = NULL;
		zend_hash_add(&mydata->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL);
	}

	mydata->fname = estrndup(fname, fname_len);
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	snprintf(mydata->version, sizeof(mydata->version), "%u.%u.%u", manifest_tag >> 12, (manifest_tag >> 8) & 0xF, (manifest_tag >> 4) & 0xF);
	mydata->internal_file_start = halt_offset + manifest_len + 4;
	mydata->halt_offset = halt_offset;
	mydata->has_compressed_files = compressed;
	mydata->fp = fp;
	mydata->refcount = 0;
	if (!reload) {
		zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void*)&mydata, sizeof(phar_archive_data),  NULL);
		if (register_alias) {
			zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
		}
	}
	efree(savebuf);
	
	if (!reload && pphar) {
		*pphar = mydata;
	}

	return SUCCESS;
}
/* }}} */

/**
 * Create or open a phar for writing
 */
static int phar_create_or_open_filename(char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
{
	phar_archive_data *mydata;
	int register_alias;
	php_stream *fp;
	phar_archive_data *phar;

	if (pphar) {
		*pphar = NULL;
	}
	
	if ((phar = phar_get_archive(fname, fname_len, alias, alias_len TSRMLS_CC)) != NULL) {
		if (fname_len != phar->fname_len || strncmp(fname, phar->fname, fname_len)) {
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, phar->fname, fname);
			return FAILURE;
		} else {
			if (pphar) {
				*pphar = phar;
			}
			return SUCCESS;
		}
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

	if (fp) {
		/* open an existing phar */
		php_stream_close(fp);
		return phar_open_filename(fname, fname_len, alias, alias_len, pphar TSRMLS_CC);
	}
	/* set up our manifest */
	mydata = emalloc(sizeof(phar_archive_data));
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	mydata->min_timestamp = 0;
	mydata->max_timestamp = 0;
	mydata->fname = estrndup(fname, fname_len);
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	snprintf(mydata->version, sizeof(mydata->version), "%s", PHAR_VERSION_STR);
	mydata->internal_file_start = -1;
	mydata->halt_offset = 0;
	mydata->has_compressed_files = 0;
	mydata->fp = fp;
	mydata->refcount = 0;
	if (!alias_len || !alias) {
		/* if we neither have an explicit nor an implicit alias, we use the filename */
		alias = NULL;
		alias_len = 0;
		register_alias = 0;
	} else {
		register_alias = 1;
	}
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void*)&mydata, sizeof(phar_archive_data),  NULL);
	if (register_alias) {
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	}
	return SUCCESS;
}

/**
 * Scan a phar file for the required __HALT_COMPILER(); ?> token and verify
 * that the manifest is proper, then pass it to phar_open_file().  SUCCESS
 * or FAILURE is returned and pphar is set to a pointer to the phar's manifest
 */
static int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar TSRMLS_DC) /* {{{ */
{
	const char token[] = "__HALT_COMPILER();";
	char *pos, buffer[1024 + sizeof(token)];
	const long readsize = sizeof(buffer) - sizeof(token);
	const long tokenlen = sizeof(token) - 1;
	long halt_offset;
	php_stream *fp;
	phar_archive_data *phar;
	
	if ((phar = phar_get_archive(fname, fname_len, alias, alias_len TSRMLS_CC)) != NULL) {
		if (fname_len != phar->fname_len || strncmp(fname, phar->fname, fname_len)) {
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, phar->fname, fname);
			return FAILURE;
		} else {
			if (pphar) {
				*pphar = phar;
			}
			return SUCCESS;
		}
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
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open phar for reading \"%s\"", fname);
		return FAILURE;
	}

	/* Maybe it's better to compile the file instead of just searching,  */
	/* but we only want the offset. So we want a .re scanner to find it. */

	if (-1 == php_stream_seek(fp, 0, SEEK_SET)) {
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
			return phar_open_file(fp, fname, fname_len, alias, alias_len, halt_offset, pphar, FALSE TSRMLS_CC);
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
static int phar_split_fname(char *filename, int filename_len, char **arch, int *arch_len, char **entry, int *entry_len TSRMLS_DC) /* {{{ */
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
		if (strcmp(mode, "wb") == 0 || strcmp(mode, "w") == 0) {
			if (phar_create_or_open_filename(resource->host, arch_len, NULL, 0, NULL TSRMLS_CC) == FAILURE)
			{
				php_url_free(resource);
				return NULL;
			}
		} else {
			if (phar_open_filename(resource->host, arch_len, NULL, 0, NULL TSRMLS_CC) == FAILURE)
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
static int phar_open_compiled_file(char *alias, int alias_len TSRMLS_DC) /* {{{ */
{
	char *fname;
	long halt_offset;
	zval *halt_constant;
	php_stream *fp;

	fname = zend_get_executed_filename(TSRMLS_C);

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
	zval_ptr_dtor(&halt_constant);
	
	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open phar for reading \"%s\"", fname);
		return FAILURE;
	}

	return phar_open_file(fp, fname, strlen(fname), alias, alias_len, halt_offset, NULL, FALSE TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Phar::mapPhar([string alias])
 * Reads the currently executed file (a phar) and registers its manifest */
PHP_METHOD(Phar, mapPhar)
{
	char * alias = NULL;
	int alias_len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &alias, &alias_len) == FAILURE) {
		return;
	}

	RETURN_BOOL(phar_open_compiled_file(alias, alias_len TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto mixed Phar::loadPhar(string url [, string alias])
 * Loads any phar archive with an alias */
PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias = NULL;
	int fname_len, alias_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}
	RETURN_BOOL(phar_open_filename(fname, fname_len, alias, alias_len, NULL TSRMLS_CC) == SUCCESS);
} /* }}} */

static php_stream_ops phar_ops = {
	phar_write, /* write */
	phar_read, /* read */
	phar_close, /* close */
	phar_flush, /* flush */
	"phar stream",
	phar_seek, /* seek */
	NULL, /* cast */
	phar_stat, /* stat */
	NULL, /* set option */
};

static php_stream_ops phar_dir_ops = {
	phar_dirwrite, /* write (does nothing) */
	phar_readdir, /* read */
	phar_closedir, /* close */
	phar_dirflush, /* flush (does nothing) */
	"phar stream",
	phar_seekdir, /* seek */
	NULL, /* cast */
	phar_stat, /* stat */
	NULL, /* set option */
};

static php_stream_wrapper_ops phar_stream_wops = {
    php_stream_phar_url_wrapper,
    NULL, /* stream_close */
    NULL, /* php_stream_phar_stat, */
    phar_stream_stat, /* stat_url */
    phar_opendir, /* opendir */
    "phar",
    NULL, /* unlink */
    NULL, /* rename */
    NULL, /* create directory */
    NULL /* remove directory */
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
	unsigned int crc = ~0;
	int len = idata->internal_file->uncompressed_filesize;
	char c;

	php_stream_seek(idata->fp, 0, SEEK_SET);
	
	while (len--) { 
		php_stream_read(idata->fp, &c, 1);
		CRC32(crc, c);
	}
	php_stream_seek(idata->fp, 0, SEEK_SET);
	if (~crc == crc32) {
		return SUCCESS;
	} else {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (crc32 mismatch on file \"%s\")", idata->phar->fname, idata->internal_file->filename);
		return FAILURE;
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
static php_stream * php_stream_phar_url_wrapper(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
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
	if (strcmp(mode, "wb") == 0 || strcmp(mode, "w") == 0) {
		if (NULL == (idata = phar_get_or_create_entry_data(resource->host, strlen(resource->host), internal_file, strlen(internal_file) TSRMLS_CC))) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: file \"%s\" could not be created in phar \"%s\"", internal_file, resource->host);
			efree(internal_file);
			php_url_free(resource);
			return NULL;
		}
		fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
		idata->phar->refcount++;
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
		;
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
		/* bytes being consumed on the stream. Therefor use a consumed filter. */ 
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
		php_stream_close(idata->fp);
		efree(idata);
		efree(internal_file);
		return NULL;
	}
	idata->internal_file->crc_checked = 1;

	fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
	idata->phar->refcount++;
	efree(internal_file);
	return fpf;
}
/* }}} */

/**
 * Used for fclose($fp) where $fp is a phar archive
 */
static int phar_close(php_stream *stream, int close_handle TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	/* data->fp is the temporary memory stream containing this file's data */
	if (data->fp) {
		/* only close if we have a cached temp memory stream */
		if (data->internal_file->fp) {
			php_stream_close(data->fp);
		} else {
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
static int phar_closedir(php_stream *stream, int close_handle TSRMLS_DC)  /* {{{ */
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
static int phar_seekdir(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
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
static size_t phar_read(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	size_t got = php_stream_read(data->fp, buf, count);
	
	if (data->fp->eof) {
		stream->eof = 1;
	}
	
	return got;
}
/* }}} */

/**
 * Used for readdir() on an opendir()ed phar directory handle
 */
static size_t phar_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
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
static int phar_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
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
static size_t phar_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *) stream->abstract;

	if (data->internal_file->temp_file == 0) {
		/* create a new temporary stream for our new file */
		data->internal_file->temp_file = php_stream_fopen_tmpfile();
	}
	if (count != php_stream_write(data->internal_file->temp_file, buf, count)) {
		php_stream_close(data->internal_file->temp_file);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "phar error: Could not write %d characters to \"%s\" in phar \"%s\"", (int) count, data->internal_file->filename, data->phar->fname);
		return -1;
	}
	data->internal_file->uncompressed_filesize += count;
	data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize; 
	data->internal_file->flags |= PHAR_ENT_MODIFIED;
	return 0;
}
/* }}} */

/**
 * Dummy: Used for writing to a phar directory (i.e. not used)
 */
static size_t phar_dirwrite(php_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
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

/**
 * Used to save work done on a writeable phar
 */
static int phar_flush(php_stream *stream TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;
	phar_entry_info *entry;
	char *buffer;
	char *manifest;
	off_t manifest_ftell, bufsize, file_ftell;
	php_uint32 copy, loc, newcrc32;
	php_stream *file, *newfile, *compressedfile;
	php_stream_filter *filter;

	if (strcmp(stream->mode, "wb") != 0 && strcmp(stream->mode, "w") != 0) {
		return EOF;
	}
	newfile = php_stream_fopen_tmpfile();
	if (!data->fp) {
		data->fp = php_stream_open_wrapper(data->phar->fname, "rb", 0, NULL);
	} else {
		php_stream_rewind(data->fp);
	}
	filter = 0;

	if (data->phar->halt_offset) {
		if (data->phar->halt_offset != php_stream_copy_to_stream(data->fp, newfile, data->phar->halt_offset))
		{
			php_stream_close(data->fp);
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to copy prologue of old phar to new phar \"%s\"", data->phar->fname);
		}
	}
	manifest_ftell = php_stream_tell(newfile);
	buffer = (char *) emalloc(300);
	bufsize = 300;
	/*  4: manifest length, 4: manifest entry count, 2: phar version,
	    4: alias length, the rest is the alias itself
	*/
	manifest = (char *) emalloc(14 + data->phar->alias_len);
	/* use dummy value until we know the actual length */
	phar_set_32(manifest, 0); /* manifest length */
	phar_set_32(manifest+4, data->phar->manifest.nNumOfElements);
	*(manifest + 8) = (unsigned char) (((PHAR_API_VERSION) >> 8) & 0xFF);
	*(manifest + 9) = (unsigned char) ((PHAR_API_VERSION) & 0xFF);
	phar_set_32(manifest+10, data->phar->alias_len);
	memcpy(manifest + 14, data->phar->alias, data->phar->alias_len);

	/* write the manifest header */
	if (14 + data->phar->alias_len != php_stream_write(newfile, manifest, 14 + data->phar->alias_len)) {
		efree(buffer);
		efree(manifest);
		php_stream_close(data->fp);
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write manifest meta-data of new phar \"%s\"", data->phar->fname);
	}

	for (zend_hash_internal_pointer_reset(&data->phar->manifest);
	    zend_hash_has_more_elements(&data->phar->manifest) == SUCCESS;
	    zend_hash_move_forward(&data->phar->manifest)) {
		if (zend_hash_get_current_data(&data->phar->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->flags & PHAR_ENT_DELETED) {
			/* remove this from the new phar */
			continue;
		}
		phar_set_32(buffer, entry->filename_len);
		memcpy(buffer + 4, entry->filename, entry->filename_len);
		if (4 + entry->filename_len != php_stream_write(newfile, buffer, 4 + entry->filename_len)) {
			efree(buffer);
			efree(manifest);
			php_stream_close(data->fp);
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write filename of file \"%s\" to manifest of new phar \"%s\"", entry->filename, data->phar->fname);
		}
		if (entry->flags & PHAR_ENT_MODIFIED) {
			if (!entry->temp_file) {
				/* nothing to do here */
				continue;
			}
		} else {
			/* set the manifest meta-data:
			   4: uncompressed filesize
			   4: creation timestamp
			   4: compressed filesize
			   4: crc32
			   1: flags (compression or not)
			*/
			copy = time(NULL);
			phar_set_32(buffer, entry->uncompressed_filesize);
			phar_set_32(buffer+4, copy);
			phar_set_32(buffer+8, entry->compressed_filesize);
			phar_set_32(buffer+12, entry->crc32);
			buffer[16] = (char) entry->flags;
			if (17 != php_stream_write(newfile, buffer, 17)) {
				efree(buffer);
				efree(manifest);
				php_stream_close(data->fp);
				php_stream_close(newfile);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write temporary manifest of file \"%s\" to manifest of new phar \"%s\"", entry->filename, data->phar->fname);
			}
		}
	}
	/* write the actual manifest size */
	file_ftell = php_stream_tell(newfile);
	copy = file_ftell - manifest_ftell + 13; /* compensate for manifest header size */
	phar_set_32(manifest, copy);
	if (-1 == php_stream_seek(newfile, manifest_ftell, SEEK_SET)) {
		efree(buffer);
		efree(manifest);
		php_stream_close(data->fp);
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to manifest of new phar \"%s\"", data->phar->fname);
	}
	/* write the manifest header */
	if (2 != php_stream_write(newfile, manifest, 2)) {
		efree(buffer);
		efree(manifest);
		php_stream_close(data->fp);
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write out manifest size of new phar \"%s\"", data->phar->fname);
	}
	if (-1 == php_stream_seek(newfile, file_ftell, SEEK_SET)) {
		efree(buffer);
		efree(manifest);
		php_stream_close(data->fp);
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to file location of new phar \"%s\"", data->phar->fname);
	}

	compressedfile = php_stream_fopen_tmpfile();
	/* now copy the actual file data to the new phar */
	for (zend_hash_internal_pointer_reset(&data->phar->manifest);
	    zend_hash_has_more_elements(&data->phar->manifest) == SUCCESS;
	    zend_hash_move_forward(&data->phar->manifest)) {
		if (zend_hash_get_current_data(&data->phar->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->flags & PHAR_ENT_DELETED) {
			/* remove this from the new phar */
			continue;
		}
		if (entry->flags & PHAR_ENT_MODIFIED) {
			if (!entry->temp_file) {
				/* nothing to do here */
				continue;
			}
			php_stream_rewind(entry->temp_file);
			file = entry->temp_file;
			copy = entry->uncompressed_filesize;
		} else {
			if (-1 == php_stream_seek(data->fp, entry->offset_within_phar + data->phar->internal_file_start, SEEK_SET)) {
				efree(buffer);
				efree(manifest);
				php_stream_close(data->fp);
				php_stream_close(newfile);
				php_stream_close(compressedfile);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to start of file \"%s\" while creating new phar \"%s\"", entry->filename, data->phar->fname);
			}
			file = data->fp;
			copy = entry->uncompressed_filesize;
		}
		if (entry->flags & PHAR_ENT_COMPRESSED_GZ) {
			filter = php_stream_filter_create("zlib.deflate", NULL, 0 TSRMLS_CC);
			if (!filter) {
				efree(buffer);
				efree(manifest);
				php_stream_close(data->fp);
				php_stream_close(newfile);
				php_stream_close(compressedfile);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to zlib compress file \"%s\" to new phar \"%s\"", entry->filename, data->phar->fname);
			}
			php_stream_filter_append(&file->readfilters, filter);
			if (bufsize < entry->uncompressed_filesize) {
				buffer = (char *) erealloc((void *)buffer, entry->uncompressed_filesize);
			}
			copy = php_stream_read(file, buffer, entry->uncompressed_filesize);
			entry->compressed_filesize = copy;
			newcrc32 = 0;
			for (loc = 0;loc < copy; loc++) {
				CRC32(newcrc32, *(buffer + loc));
			}
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			php_stream_rewind(compressedfile);
			php_stream_write(compressedfile, buffer, copy); 
			file = compressedfile;
			entry->crc32 = newcrc32;
		} else if (entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
			filter = php_stream_filter_create("bzip2.compress", NULL, 0 TSRMLS_CC);
			if (!filter) {
				efree(buffer);
				efree(manifest);
				php_stream_close(data->fp);
				php_stream_close(newfile);
				php_stream_close(compressedfile);
				php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to bzip2 compress file \"%s\" to new phar \"%s\"", entry->filename, data->phar->fname);
			}
			php_stream_filter_append(&file->readfilters, filter);
			if (bufsize < entry->uncompressed_filesize) {
				buffer = (char *) erealloc((void *)buffer, entry->uncompressed_filesize);
			}
			copy = php_stream_read(data->fp, buffer, entry->uncompressed_filesize);
			entry->compressed_filesize = copy;
			newcrc32 = 0;
			for (loc = 0;loc < copy; loc++) {
				CRC32(newcrc32, *(buffer + loc));
			}
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			php_stream_rewind(compressedfile);
			php_stream_write(compressedfile, buffer, copy); 
			file = compressedfile;
			entry->crc32 = newcrc32;
		}
		if (copy != php_stream_copy_to_stream(file, newfile, copy)) {
			efree(buffer);
			efree(manifest);
			php_stream_close(data->fp);
			php_stream_close(newfile);
			php_stream_close(compressedfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write contents of file \"%s\" to new phar \"%s\"", entry->filename, data->phar->fname);
		}
		/* close the temporary file, no longer needed */
		if (entry->flags & PHAR_ENT_MODIFIED) {
			if (entry->temp_file) {
				php_stream_close(entry->temp_file);
				entry->temp_file = 0;
				entry->flags &= ~PHAR_ENT_MODIFIED;
			}
		}
	}
	php_stream_close(compressedfile);
	/* now that we know the actual file sizes and CRC32s, re-write the manifest */
	if (-1 == php_stream_seek(newfile, data->phar->halt_offset + 14 + data->phar->alias_len, SEEK_SET)) {
		efree(buffer);
		efree(manifest);
		php_stream_close(data->fp);
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to set manifest length of new phar \"%s\"", data->phar->fname);
	}
	for (zend_hash_internal_pointer_reset(&data->phar->manifest);
	    zend_hash_has_more_elements(&data->phar->manifest) == SUCCESS;
	    zend_hash_move_forward(&data->phar->manifest)) {
		if (zend_hash_get_current_data(&data->phar->manifest, (void **)&entry) == FAILURE) {
			continue;
		}
		if (entry->flags & PHAR_ENT_DELETED) {
			/* already removed from the new phar */
			continue;
		}
		/* skip already written filename */
		if (-1 == php_stream_seek(newfile, 4 + entry->filename_len, SEEK_CUR)) {
			efree(buffer);
			efree(manifest);
			php_stream_close(data->fp);
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to manifest of file \"%s\" in new phar \"%s\"", entry->filename, data->phar->fname);
		}
		/* set the actual manifest meta-data:
			4: uncompressed filesize
			4: creation timestamp
			4: compressed filesize
			4: crc32
			1: flags (compression or not)
		*/
		copy = time(NULL);
		phar_set_32(buffer, entry->uncompressed_filesize);
		phar_set_32(buffer+4, copy);
		phar_set_32(buffer+8, entry->compressed_filesize);
		phar_set_32(buffer+12, entry->crc32);
		buffer[16] = (char) entry->flags;
		if (17 != php_stream_write(newfile, buffer, 17)) {
			efree(buffer);
			efree(manifest);
			php_stream_close(data->fp);
			php_stream_close(newfile);
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to write manifest of file \"%s\" in new phar \"%s\"", entry->filename, data->phar->fname);
		}
	}
	/* finally, close the temp file, rename the original phar,
	   move the temp to the old phar, unlink the old phar, and reload it into memory
	*/
	php_stream_rewind(newfile);
	file = php_stream_open_wrapper(data->phar->fname, "wb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
	if (!file) {
		efree(buffer);
		efree(manifest);
		php_stream_close(data->fp);
		php_stream_close(newfile);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open new phar \"%s\" for writing", data->phar->fname);
	}
	php_stream_copy_to_stream(newfile, file, PHP_STREAM_COPY_ALL);
	php_stream_close(newfile);
	php_stream_close(file);
	efree(buffer);
	efree(manifest);
	file = 0;

#if PHP_MAJOR_VERSION < 6
	if (PG(safe_mode) && (!php_checkuid(data->phar->fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		php_stream_close(data->fp);
		return EOF;
	}
#endif

	if (php_check_open_basedir(data->phar->fname TSRMLS_CC)) {
		php_stream_close(data->fp);
		return EOF;
	}

	file = php_stream_open_wrapper(data->phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!file) {
		php_stream_close(data->fp);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to open phar for reading \"%s\"", data->phar->fname);
		return EOF;
	}

	if (-1 == php_stream_seek(file, data->phar->halt_offset, SEEK_SET)) {
		php_stream_close(data->fp);
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "unable to seek to __HALT_COMPILER(); in new phar \"%s\"", data->phar->fname);
	}
	
	php_stream_close(data->fp);
	phar_open_file(file, data->phar->fname, data->phar->fname_len, data->phar->alias, data->phar->alias_len, data->phar->halt_offset, &data->phar, TRUE TSRMLS_CC);
	return EOF;
}
/* }}} */

/**
 * Dummy: Used for flushing writes to a phar directory (i.e. not used)
 */
static int phar_dirflush(php_stream *stream TSRMLS_DC) /* {{{ */
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
	/* read-only across the board */
	ssb->sb.st_mode = 0444;

	if (!is_dir) {
		ssb->sb.st_size = data->uncompressed_filesize;
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
static int phar_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	phar_entry_data *data;
	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) return -1;

	data = (phar_entry_data *)stream->abstract;
	phar_dostat(data->phar, data->internal_file, ssb, 0, data->phar->alias, data->phar->alias_len TSRMLS_CC);
	return 0;
}
/* }}} */

/**
 * Stream wrapper stat implementation of stat()
 */
static int phar_stream_stat(php_stream_wrapper *wrapper, char *url, int flags,
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
static int compare_dir_name(const void *a, const void *b TSRMLS_DC)  /* {{{ */
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
		if (zend_hash_sort(data, zend_qsort, compare_dir_name, 0 TSRMLS_CC) == FAILURE) {
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
 * Open a directory handle within a phar archive
 */
static php_stream *phar_opendir(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *key;
	uint keylen;
	ulong unused;
	phar_archive_data *phar;
	phar_entry_info *entry;

	resource = php_url_parse(filename);

	if (!resource && (resource = phar_open_url(wrapper, filename, mode, options TSRMLS_CC)) == NULL) {
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/ */
	if (!resource->scheme || !resource->host || !resource->path) {
		if (resource->host && !resource->path) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory", filename, resource->host);
			php_url_free(resource);
			return NULL;
		}
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\", must have at least phar://%s/", filename, filename);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar url \"%s\"", filename);
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

/* {{{ proto void Phar::__construct(string fname [, int flags [, string alias]])
 * Construct a Phar archive object
 */
PHP_METHOD(Phar, __construct)
{
	char *fname, *alias = NULL;
	int fname_len, alias_len = 0;
	long flags = 0;
	phar_archive_object *phar_obj;
	phar_archive_data   *phar_data;
	zval *zobj = getThis(), arg1, arg2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &fname, &fname_len, &flags, &alias, &alias_len) == FAILURE) {
		return;
	}

	phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (phar_obj->arc.archive) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot call constructor twice");
		return;
	}

	if (phar_open_filename(fname, fname_len, alias, alias_len, &phar_data TSRMLS_CC) == FAILURE) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot open phar file '%s' with alias '%s'", fname, alias);
		return;
	}

	phar_data->refcount++;
	phar_obj->arc.archive = phar_data;
	phar_obj->spl.oth_handler = &phar_spl_foreign_handler;

	fname_len = spprintf(&fname, 0, "phar://%s", fname);

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, fname, fname_len, 0);

	if (ZEND_NUM_ARGS() > 1) {
		INIT_PZVAL(&arg2);
		ZVAL_LONG(&arg2, flags);
		zend_call_method_with_2_params(&zobj, Z_OBJCE_P(zobj), 
			&spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg1, &arg2);
	} else {
		zend_call_method_with_1_params(&zobj, Z_OBJCE_P(zobj), 
			&spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg1);
	}

	phar_obj->spl.info_class = phar_ce_entry;

	efree(fname);
}
/* }}} */

#define PHAR_ARCHIVE_OBJECT() \
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	if (!phar_obj->arc.archive) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Cannot call method on an uninitialzed Phar object"); \
		return; \
	}

/* {{{ proto int Phar::count()
 * Returns the number of entries in the Phar archive
 */
PHP_METHOD(Phar, count)
{
	PHAR_ARCHIVE_OBJECT();
	
	RETURN_LONG(zend_hash_num_elements(&phar_obj->arc.archive->manifest));
}
/* }}} */

/* {{{ proto string Phar::getVersion()
 * Return version info of Phar archive
 */
PHP_METHOD(Phar, getVersion)
{
	PHAR_ARCHIVE_OBJECT();
	
	RETURN_STRING(phar_obj->arc.archive->version, 1);
}
/* }}} */

/* {{{ proto int Phar::offsetExists(string offset)
 * determines whether a file exists in the phar
 */
PHP_METHOD(Phar, offsetExists)
{
	char *fname;
	int fname_len;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int Phar::offsetGet(string offset)
 * get a PharFileInfo object for a specific file
 */
PHP_METHOD(Phar, offsetGet)
{
	char *fname;
	int fname_len;
	zval *zfname;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}
	
	if (!phar_get_entry_info(phar_obj->arc.archive, fname, fname_len TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist", fname);
	} else {
		fname_len = spprintf(&fname, 0, "phar://%s/%s", phar_obj->arc.archive->fname, fname);
		MAKE_STD_ZVAL(zfname);
		ZVAL_STRINGL(zfname, fname, fname_len, 0);
		spl_instantiate_arg_ex1(phar_obj->spl.file_class, &return_value, 0, zfname TSRMLS_CC);
		zval_ptr_dtor(&zfname);
	}

}
/* }}} */

/* {{{ proto int Phar::offsetSet(string offset, string value)
 * set the contents of an internal file to those of an external file
 */
PHP_METHOD(Phar, offsetSet)
{
	zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Operation currently not supported");
}
/* }}} */

/* {{{ proto int Phar::offsetUnset()
 * remove a file from a phar
 */
PHP_METHOD(Phar, offsetUnset)
{
	zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Operation currently not supported");
}
/* }}} */

/* {{{ proto void PharFileInfo::__construct(string entry)
 * Construct a Phar entry object
 */
PHP_METHOD(PharFileInfo, __construct)
{
	char *fname, *arch, *entry;
	int fname_len, arch_len, entry_len;
	phar_entry_object  *entry_obj;
	phar_entry_info    *entry_info;
	phar_archive_data *phar_data;
	zval *zobj = getThis(), arg1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}
	
	entry_obj = (phar_entry_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (entry_obj->ent.entry) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot call constructor twice");
		return;
	}

	if (phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len TSRMLS_CC) == FAILURE) {
		efree(arch);
		efree(entry);
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot access phar file entry '%s'", fname);
		return;
	}

	if (phar_open_filename(arch, arch_len, NULL, 0, &phar_data TSRMLS_CC) == FAILURE) {
		efree(arch);
		efree(entry);
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot open phar file '%s'", fname);
		return;
	}

	if ((entry_info = phar_get_entry_info(phar_data, entry, entry_len TSRMLS_CC)) == NULL) {
		efree(arch);
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot access phar file entry '%s' in archive '%s'", entry, arch);
		efree(entry);
		return;
	}

	efree(arch);
	efree(entry);

	entry_obj->ent.entry = entry_info;

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, fname, fname_len, 0);

	zend_call_method_with_1_params(&zobj, Z_OBJCE_P(zobj), 
		&spl_ce_SplFileInfo->constructor, "__construct", NULL, &arg1);
}
/* }}} */

#define PHAR_ENTRY_OBJECT() \
	phar_entry_object *entry_obj = (phar_entry_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	if (!entry_obj->ent.entry) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Cannot call method on an uninitialzed PharFileInfo object"); \
		return; \
	}

/* {{{ proto int PharFileInfo::getCompressedSize()
 * Returns the compressed size
 */
PHP_METHOD(PharFileInfo, getCompressedSize)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_LONG(entry_obj->ent.entry->compressed_filesize);
}
/* }}} */

/* {{{ proto int PharFileInfo::getCRC32()
 * Returns CRC32 code or throws an exception if not CRC checked
 */
PHP_METHOD(PharFileInfo, getCRC32)
{
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->crc_checked) {
		RETURN_LONG(entry_obj->ent.entry->crc32);
	} else {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry was not CRC checked"); \
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::getPharFlags()
 * Returns the Phar file entry flags
 */
PHP_METHOD(PharFileInfo, getPharFlags)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_LONG(entry_obj->ent.entry->flags);
}
/* }}} */

/* {{{ proto int PharFileInfo::isCRCChecked()
 * Returns whether file entry is CRC checked
 */
PHP_METHOD(PharFileInfo, isCRCChecked)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_BOOL(entry_obj->ent.entry->crc_checked);
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
	{NULL, NULL, NULL}	/* Must be the last line in phar_functions[] */
};

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_mapPhar, 0, 0, 0)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_loadPhar, 0, 0, 1)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

zend_function_entry php_archive_methods[] = {
	PHP_ME(Phar, __construct,   arginfo_phar___construct,  0)
	PHP_ME(Phar, count,         NULL,                      0)
	PHP_ME(Phar, getVersion,    NULL,                      0)
	PHP_ME(Phar, offsetGet,     arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetSet,     arginfo_phar_offsetSet,    ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetUnset,   arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetExists,  arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	/* static member functions */
	PHP_ME(Phar, apiVersion,    NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canCompress,   NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mapPhar,       arginfo_phar_mapPhar,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, loadPhar,      arginfo_phar_loadPhar,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_entry___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

zend_function_entry php_entry_methods[] = {
	PHP_ME(PharFileInfo, __construct,        arginfo_entry___construct,  0)
	PHP_ME(PharFileInfo, getCompressedSize,  NULL,                       0)
	PHP_ME(PharFileInfo, getCRC32,           NULL,                       0)
	PHP_ME(PharFileInfo, getPharFlags,       NULL,                       0)
	PHP_ME(PharFileInfo, isCRCChecked,       NULL,                       0)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ php_phar_init_globals
 */
static void php_phar_init_globals_module(zend_phar_globals *phar_globals)
{
	memset(phar_globals, 0, sizeof(zend_phar_globals));
}
/* }}} */

PHP_MINIT_FUNCTION(phar) /* {{{ */
{
	zend_class_entry ce;

	ZEND_INIT_MODULE_GLOBALS(phar, php_phar_init_globals_module, NULL);

	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator, NULL  TSRMLS_CC);

	zend_class_implements(phar_ce_archive TSRMLS_CC, 2, spl_ce_Countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharFileInfo", php_entry_methods);
	phar_ce_entry = zend_register_internal_class_ex(&ce, spl_ce_SplFileInfo, NULL  TSRMLS_CC);

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
	zend_hash_init(&(PHAR_GLOBALS->phar_fname_map), sizeof(phar_archive_data),  zend_get_hash_value, destroy_phar_data, 0);
	zend_hash_init(&(PHAR_GLOBALS->phar_alias_map), sizeof(phar_archive_data*), zend_get_hash_value, NULL, 0);
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(phar) /* {{{ */
{
	zend_hash_destroy(&(PHAR_GLOBALS->phar_alias_map));
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
	ZEND_MOD_REQUIRED("spl")
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
