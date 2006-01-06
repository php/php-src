/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005 The PHP Group                                     |
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
#include "zend_execute.h"
#include "zend_qsort.h"
#include "zend_constants.h"
#include "zend_operators.h"
#include "php_phar.h"
#include "main/php_streams.h"
#ifndef TRUE
 #       define TRUE 1
 #       define FALSE 0
#endif

ZEND_DECLARE_MODULE_GLOBALS(phar)

/* entry for one file in a phar file */
typedef struct _phar_manifest_entry {
	php_uint32	filename_len;
	char		*filename;
	php_uint32	uncompressed_filesize;
	php_uint32	timestamp;
	php_uint32	offset_within_phar;
	php_uint32	compressed_filesize;
	zend_bool	crc_checked;
	php_stream  *fp;
} phar_manifest_entry;

/* information about a phar file */
typedef struct _phar_file_data {
	char		*filename;
	int			filename_len;
	char		*alias;
	int			alias_len;
	size_t		internal_file_start;
	zend_bool	is_compressed;
	HashTable	manifest;
	php_stream	*fp;
} phar_file_data;

/* stream access data for one file entry in a phar file */
typedef struct _phar_internal_file_data {
	phar_file_data      *phar;
	php_stream          *fp;
	phar_manifest_entry	*internal_file;
} phar_internal_file_data;

/* {{{ forward declarations */
static php_stream *php_stream_phar_url_wrapper(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static int phar_close(php_stream *stream, int close_handle TSRMLS_DC);
static int phar_closedir(php_stream *stream, int close_handle TSRMLS_DC);
static int phar_seekdir(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
static size_t phar_read(php_stream *stream, char *buf, size_t count TSRMLS_DC);
static size_t phar_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int phar_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);

static size_t phar_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static int phar_flush(php_stream *stream TSRMLS_DC);
static int phar_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);

static int phar_stream_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);
static php_stream *phar_opendir(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
/* }}} */

static zend_class_entry *php_archive_entry_ptr;

static void destroy_phar_data(void *pDest) /* {{{ */
{
	TSRMLS_FETCH();

	phar_file_data *data = (phar_file_data *) pDest;
	efree(data->alias);
	zend_hash_destroy(&data->manifest);
	php_stream_close(data->fp);
}
/* }}}*/

static void destroy_phar_manifest(void *pDest) /* {{{ */
{
	phar_manifest_entry *entry = (phar_manifest_entry *)pDest;

	efree(entry->filename);
}
/* }}} */

static phar_internal_file_data *phar_get_filedata(char *alias, char *path TSRMLS_DC) /* {{{ */
{
	phar_file_data *phar;
	phar_internal_file_data *ret;
	phar_manifest_entry *internal_file;
	
	ret = NULL;
	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_data), alias, strlen(alias), (void **) &phar)) {
		if (SUCCESS == zend_hash_find(&phar->manifest, path, strlen(path), (void **) &internal_file)) {
			ret = (phar_internal_file_data *) emalloc(sizeof(phar_internal_file_data));
			ret->phar = phar;
			ret->internal_file = internal_file;
			if (internal_file->fp) {
				/* transfer ownership */
				ret->fp = internal_file->fp;
				php_stream_seek(ret->fp, 0, SEEK_SET);
				internal_file->fp = 0;
			} else {
				ret->fp = 0;
			}
		}
	}
	return ret;
}
/* }}} */

/* {{{ proto string apiVersion()
 * Returns the api version */
PHP_METHOD(Phar, apiVersion)
{
	RETURN_STRINGL("0.7.1", sizeof("0.7.1")-1, 1);
}
/* }}}*/

/* {{{ proto bool canCompress()
 * Returns whether phar extension supports compression using zlib */
PHP_METHOD(Phar, canCompress)
{
#ifdef HAVE_PHAR_ZLIB
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

#define MAPPHAR_ALLOC_FAIL(msg) \
	php_stream_close(fp);\
	php_error_docref(NULL TSRMLS_CC, E_ERROR, msg, fname);\
	return FAILURE;

#define MAPPHAR_FAIL(msg) \
	efree(savebuf);\
	MAPPHAR_ALLOC_FAIL(msg)

#ifdef WORDS_BIGENDIAN
# define PHAR_GET_VAL(buffer, var) \
	var = ((unsigned char)buffer[3]) << 12 \
		+ ((unsigned char)buffer[2]) <<  8 \
		+ ((unsigned char)buffer[1]) <<  4 \
		+ ((unsigned char)buffer[0]); \
	buffer += 4
#else
# define PHAR_GET_VAL(buffer, var) \
	var = *(php_uint32*)(buffer); \
	buffer += 4
#endif

static int phar_open_file(php_stream *fp, char *fname, int fname_len, char *alias, int alias_len, zend_bool compressed, long halt_offset TSRMLS_DC) /* {{{ */
{
	char *buffer, *endbuffer, *savebuf;
	phar_file_data mydata, *phar_data;
	phar_manifest_entry entry;
	php_uint32 manifest_len, manifest_count, manifest_index;

	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_data), alias, alias_len, (void **) &phar_data)) {
		/* Overloading or reloading an archive would only be possible if we  */
		/* refcount everything to be sure no stream for any file in the */
		/* archive is open. */
		if (fname_len != phar_data->filename_len || strncmp(fname, phar_data->filename, fname_len)) {
			php_stream_close(fp);
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, phar_data->filename, fname);
		} else {
			php_stream_close(fp);
			return SUCCESS;
		}
	}

	/* check for ?>\n and increment accordingly */
	if (-1 == php_stream_seek(fp, halt_offset, SEEK_SET)) {
		MAPPHAR_ALLOC_FAIL("cannot seek to __HALT_COMPILER(); location in phar \"%s\"")
	}

	if (FALSE == (buffer = (char *) emalloc(4))) {
		MAPPHAR_ALLOC_FAIL("memory allocation failed in phar \"%s\"")
	}
	savebuf = buffer;
	if (3 != php_stream_read(fp, buffer, 3)) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	if (*buffer == ' ' && *(buffer + 1) == '?' && *(buffer + 2) == '>') {
		int nextchar;
		halt_offset += 3;
		if (EOF == (nextchar = php_stream_getc(fp))) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
		}
		if ((char) nextchar == '\r') {
			if (EOF == (nextchar = php_stream_getc(fp))) {
				MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
			}
			halt_offset++;
		}
		if ((char) nextchar == '\n') {
			halt_offset++;
		}
	}
	/* make sure we are at the right location to read the manifest */
	if (-1 == php_stream_seek(fp, halt_offset, SEEK_SET)) {
		MAPPHAR_FAIL("cannot seek to __HALT_COMPILER(); location in phar \"%s\"")
	}

	/* read in manifest */
	if (4 != php_stream_read(fp, buffer, 4)) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	endbuffer = buffer + 5;
	PHAR_GET_VAL(buffer, manifest_len);
	buffer -= 4;
	if (manifest_len > 1048576) {
		/* prevent serious memory issues by limiting manifest to at most 1 MB in length */
		MAPPHAR_FAIL("manifest cannot be larger than 1 MB in phar \"%s\"")
	}
	if (FALSE == (buffer = (char *) erealloc(buffer, manifest_len))) {
		MAPPHAR_FAIL("memory allocation failed in phar \"%s\"")
	}
	savebuf = buffer;
	/* set the test pointer */
	endbuffer = buffer + manifest_len;
	/* retrieve manifest */
	if (manifest_len != php_stream_read(fp, buffer, manifest_len)) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
	}
	/* extract the number of entries */
	if (buffer + 4 > endbuffer) {
		MAPPHAR_FAIL("internal corruption of phar \"%s\" (buffer overrun)");
	}
	PHAR_GET_VAL(buffer, manifest_count);
	/* we have 5 32-bit items at least */
	if (manifest_count > (manifest_len / (4 * 5))) {
		/* prevent serious memory issues */
		MAPPHAR_FAIL("too many manifest entries for size of manifest in phar \"%s\"")
	}
	/* set up our manifest */
	zend_hash_init(&mydata.manifest, sizeof(phar_manifest_entry),
		zend_get_hash_value, destroy_phar_manifest, 0);
	for (manifest_index = 0; manifest_index < manifest_count; manifest_index++) {
		if (buffer + 4 > endbuffer) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (truncated manifest)")
		}
		PHAR_GET_VAL(buffer, entry.filename_len);
		if (buffer + entry.filename_len + 16 > endbuffer) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (buffer overrun)");
		}
		entry.filename = estrndup(buffer, entry.filename_len);
		buffer += entry.filename_len;
		PHAR_GET_VAL(buffer, entry.uncompressed_filesize);
		PHAR_GET_VAL(buffer, entry.timestamp);
		PHAR_GET_VAL(buffer, entry.offset_within_phar);
		PHAR_GET_VAL(buffer, entry.compressed_filesize);
		if (entry.uncompressed_filesize + 8 != entry.compressed_filesize) {
#ifndef HAVE_PHAR_ZLIB
			if (!compressed) {
				MAPPHAR_FAIL("zlib extension is required for compressed .phar file \"%s\"");
			}
#endif
			compressed = 1;
		}
		entry.crc_checked = 0;
		entry.fp = NULL;
		if (entry.compressed_filesize < 9) {
			MAPPHAR_FAIL("internal corruption of phar \"%s\" (file size in phar is not large enough)")
		}
		zend_hash_add(&mydata.manifest, entry.filename, entry.filename_len, &entry,
			sizeof(phar_manifest_entry), NULL);
	}

	mydata.filename = estrndup(fname, fname_len);
	mydata.filename_len = fname_len;
	/* alias is auto-efreed after returning, so we must dupe it */
	mydata.alias = estrndup(alias, alias_len);
	mydata.alias_len = alias_len;
	mydata.internal_file_start = manifest_len + halt_offset + 4;
	mydata.is_compressed = compressed;
	mydata.fp = fp;
	zend_hash_add(&(PHAR_GLOBALS->phar_data), alias, alias_len, &mydata,
		sizeof(phar_file_data), NULL);
	efree(savebuf);
	
	return SUCCESS;
}
/* }}} */

static int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, zend_bool compressed TSRMLS_DC) /* {{{ */
{
	const char token[] = "__HALT_COMPILER();";
	char *pos, buffer[1024 + sizeof(token)];
	const long readsize = sizeof(buffer) - sizeof(token);
	const long tokenlen = sizeof(token) - 1;
	int result;
	long halt_offset;
	php_stream *fp;

	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return FAILURE;
	}

	if (php_check_open_basedir(fname TSRMLS_CC)) {
		return FAILURE;
	}

	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "unable to open phar for reading \"%s\"", fname);
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
			result = phar_open_file(fp, fname, fname_len, alias, alias_len, compressed, halt_offset TSRMLS_CC);
			if (result == FAILURE) {
				php_stream_close(fp);
			}
			return result;
		}

		halt_offset += readsize;
		memmove(buffer, buffer + tokenlen, readsize + 1);
	}
	
	MAPPHAR_ALLOC_FAIL("internal corruption of phar \"%s\" (__HALT_COMPILER(); not found)")
	php_stream_close(fp);
	return FAILURE;
}
/* }}} */

static php_url* phar_open_url(php_stream_wrapper *wrapper, char *filename, char *mode, int options TSRMLS_DC) /* {{{ */
{
	php_url *resource;
	char *pos_p, *pos_z, *ext_str, *alias;
	int len, ext_len;

	if (!strncasecmp(filename, "phar://", 7)) {
		filename += 7;
		pos_p = strstr(filename, ".phar.php");
		pos_z = strstr(filename, ".phar.gz");
		if (pos_p) {
			if (pos_z) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\" (cannot contain .phar.php and .phar.gz)", filename);
				return NULL;
			}
			ext_str = pos_p;
			ext_len = 9;
		} else if (pos_z) {
			ext_str = pos_z;
			ext_len = 8;
		} else if ((pos_p = strstr(filename, ".phar")) != NULL) {
			ext_str = pos_p;
			ext_len = 5;
		} else {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\" (filename extension must be .phar.php, .phar.gz or .phar)", filename);
			return NULL;
		}
		resource = emalloc(sizeof(php_url));
		resource->scheme = estrndup("phar", 4);
		len = ext_str - filename + ext_len;
		alias = resource->host = estrndup(filename, len);
		if (ext_str[ext_len]) {
			resource->path = estrdup(ext_str+ext_len);
		} else {
			resource->path = estrndup("/", 1);
		}
#if MBO_0
		if (resource) {
			fprintf(stderr, "Alias:     %s\n", alias);
			fprintf(stderr, "Compressed:%s\n", pos_z ? "yes" : "no");
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
		if (phar_open_filename(resource->host, len, alias, len, pos_z ? 1 : 0 TSRMLS_CC) == FAILURE)
		{
			php_url_free(resource);
			return NULL;
		}
		
		return resource;
	}

	return NULL;
}
/* }}} */

static int phar_open_compiled_file(char *alias, int alias_len, zend_bool compressed TSRMLS_DC) /* {{{ */
{
	char *fname;
	int result;
	long halt_offset;
	zval *halt_constant;
	php_stream *fp;

	fname = zend_get_executed_filename(TSRMLS_C);

	if (!strcmp(fname, "[no active file]")) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "cannot initialize a phar outside of PHP execution");
		return FAILURE;
	}

	MAKE_STD_ZVAL(halt_constant);
	if (0 == zend_get_constant("__COMPILER_HALT_OFFSET__", 24, halt_constant TSRMLS_CC)) {
	FREE_ZVAL(halt_constant);
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "__HALT_COMPILER(); must be declared in a phar");
		return FAILURE;
	}
	halt_offset = Z_LVAL(*halt_constant);
	zval_ptr_dtor(&halt_constant);
	
	fp = php_stream_open_wrapper(fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);

	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "unable to open phar for reading \"%s\"", fname);
		return FAILURE;
	}

	result = phar_open_file(fp, fname, strlen(fname), alias, alias_len, compressed, halt_offset TSRMLS_CC);
	if (result == FAILURE) {
		php_stream_close(fp);
	}
	return result;
}
/* }}} */

/* {{{ proto mixed Phar::mapPhar(string alias, mixed compressed, [mixed unused [, mixed unused]])
 * Maps the currently executed file (a phar) with the given alias */
PHP_METHOD(Phar, mapPhar)
{
	char *alias;
	zend_bool compressed;
	int alias_len;
	zval **unused1, **unused2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb|zz", &alias, &alias_len, &compressed, &unused1, &unused2) == FAILURE) {
		return;
	}
#ifndef HAVE_PHAR_ZLIB
	if (compressed) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "zlib extension is required for compressed .phar files");
		return;
	}
#endif

	RETURN_BOOL(phar_open_compiled_file(alias, alias_len, compressed TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto mixed Phar::loadPhar(string url, string alias)
 * Loads a phar archive with an alias */
PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias;
	int fname_len, alias_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}
	RETURN_BOOL(phar_open_filename(fname, fname_len, alias, alias_len, 0 TSRMLS_CC) == SUCCESS);
} /* }}} */

static php_stream_ops phar_ops = {
	phar_write, /* write (does nothing) */
	phar_read, /* read */
	phar_close, /* close */
	phar_flush, /* flush (does nothing) */
	"phar stream",
	phar_seek, /* seek */
	NULL, /* cast */
	phar_stat, /* stat */
	NULL, /* set option */
};

static php_stream_ops phar_dir_ops = {
	phar_write, /* write (does nothing) */
	phar_readdir, /* read */
	phar_closedir, /* close */
	phar_flush, /* flush (does nothing) */
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

static int phar_postprocess_file(php_stream_wrapper *wrapper, int options, phar_internal_file_data *idata, unsigned long crc32 TSRMLS_DC) /* {{{ */
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
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (crc32 mismatch on file \"%s\")", idata->phar->filename, idata->internal_file->filename);
		return FAILURE;
	}
}
/* }}} */

static php_stream * php_stream_phar_url_wrapper(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	phar_internal_file_data *idata;
	php_stream *stream = NULL;
	char *internal_file;
	char *buffer;
	char *filedata;
	char tmpbuf[8];
	php_url *resource = NULL;
	php_stream *fp;
	int status;
#ifdef HAVE_PHAR_ZLIB
	unsigned long crc32;
	php_uint32 actual_length;
	char *savebuf;
	/* borrowed from zlib.c gzinflate() function */
	php_uint32 offset;
	unsigned long length;
	char *s1=NULL;
	z_stream zstream;
#endif

	resource = php_url_parse(path);

	if (!resource && (resource = phar_open_url(wrapper, path, mode, options TSRMLS_CC)) == NULL) {
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource || !resource->scheme || !resource->host || !resource->path) {
		if (resource) {
			php_url_free(resource);
		}
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\"", path);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		if (resource) {
			php_url_free(resource);
		}
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar stream url \"%s\"", path);
		return NULL;
	}

	/* strip leading "/" */
	internal_file = estrdup(resource->path + 1);
	if (NULL == (idata = phar_get_filedata(resource->host, internal_file TSRMLS_CC))) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: \"%s\" is not a file in phar \"%s\"", internal_file, resource->host);
		efree(internal_file);
		php_url_free(resource);
		return NULL;
	}
	
#if MBO_0
		fprintf(stderr, "Pharname: %s\n", idata->phar->filename);
		fprintf(stderr, "Filename: %s\n", internal_file);
		fprintf(stderr, "Entry:    %s\n", idata->internal_file->filename);
		fprintf(stderr, "Size:     %u\n", idata->internal_file->uncompressed_filesize);
		fprintf(stderr, "Offset:   %u\n", idata->internal_file->offset_within_phar);
		fprintf(stderr, "Cached:   %s\n", idata->internal_file->filedata ? "yes" : "no");
#endif

	/* do we have the data already? */
	if (idata->fp) {
		stream = php_stream_alloc(&phar_ops, idata, NULL, mode);
		efree(internal_file);
		return stream;
	}

	if (PG(safe_mode) && (!php_checkuid(idata->phar->filename, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		efree(idata);
		efree(internal_file);
		return NULL;
	}
	
	if (php_check_open_basedir(idata->phar->filename TSRMLS_CC)) {
		efree(idata);
		efree(internal_file);
		return NULL;
	}

	fp = idata->phar->fp;

	if (!fp) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: cannot open phar \"%s\"", idata->phar->filename);
		efree(idata);
		efree(internal_file);
		return NULL;
	}

	/* seek to start of internal file and read it */
	offset = idata->phar->internal_file_start + idata->internal_file->offset_within_phar;
	if (-1 == php_stream_seek(fp, offset, SEEK_SET)) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (cannot seek to start of file \"%s\" at offset \"%d\")",
			idata->phar->filename, internal_file, offset);
		efree(idata);
		efree(internal_file);
		return NULL;
	}

	if (idata->phar->is_compressed) {
#ifdef HAVE_PHAR_ZLIB
		buffer = (char *) emalloc(idata->internal_file->compressed_filesize);
		if (idata->internal_file->compressed_filesize !=
				php_stream_read(fp, buffer, idata->internal_file->compressed_filesize)) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (filesize mismatch on file \"%s\")", idata->phar->filename, internal_file);
			efree(idata);
			efree(internal_file);
			efree(buffer);
			return NULL;
		}
		savebuf = buffer;
		PHAR_GET_VAL(buffer, crc32);
		PHAR_GET_VAL(buffer, actual_length);

		/* borrowed from zlib.c gzinflate() function */
		zstream.zalloc = (alloc_func) Z_NULL;
		zstream.zfree = (free_func) Z_NULL;

		length = idata->internal_file->uncompressed_filesize;
		do {
			filedata = (char *) erealloc(s1, length);

			if (!filedata && s1) {
				php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (corrupted zlib compression of file \"%s\")", idata->phar->filename, internal_file);
				efree(s1);
				efree(savebuf);
				efree(idata);
				efree(internal_file);
				return NULL;
			}

			zstream.next_in = (Bytef *) buffer;
			zstream.avail_in = (uInt) idata->internal_file->compressed_filesize;

			zstream.next_out = filedata;
			zstream.avail_out = (uInt) length;

			/* init with -MAX_WBITS disables the zlib internal headers */
			status = inflateInit2(&zstream, -MAX_WBITS);
			if (status == Z_OK) {
				status = inflate(&zstream, Z_FINISH);
				if (status != Z_STREAM_END) {
					inflateEnd(&zstream);
					if (status == Z_OK) {
						status = Z_BUF_ERROR;
					}
				} else {
					status = inflateEnd(&zstream);
				}
			}
			s1 = filedata;
			
		} while (status == Z_BUF_ERROR);

		if (status != Z_OK) {
			efree(savebuf);
			efree(filedata);
			efree(idata);
			efree(internal_file);
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: decompression failed");
			return NULL;
		}

		efree(savebuf);
		/* check length */
		if (actual_length != idata->internal_file->uncompressed_filesize) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (filesize mismatch on file \"%s\")", idata->phar->filename, internal_file);
			efree(internal_file);
			efree(filedata);
			efree(idata);
			return NULL;
		}
		/* need to copy filedata to stream now */
		idata->fp = php_stream_temp_open(0, PHP_STREAM_MAX_MEM, filedata, actual_length);
		efree(filedata);
		/* check crc32/filesize */
		if (!idata->internal_file->crc_checked 
		&& phar_postprocess_file(wrapper, options, idata, crc32 TSRMLS_CC) != SUCCESS) {
			efree(idata);
			efree(internal_file);
		}
		idata->internal_file->crc_checked = 1;
#else
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "zlib extension must be enabled for compressed .phar files");
		efree(internal_file);
		return NULL;
#endif
	} else { /* from here is for non-compressed */
		buffer = &tmpbuf[0];
		php_stream_read(fp, buffer, 8);
		PHAR_GET_VAL(buffer, crc32);
		PHAR_GET_VAL(buffer, actual_length);
		if (actual_length != idata->internal_file->uncompressed_filesize) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->filename, internal_file);
			efree(idata);
			efree(internal_file);
			return NULL;
		}
		/* bypass to temp stream */
		idata->fp = php_stream_temp_new();
		if (php_stream_copy_to_stream(fp, idata->fp, actual_length) != actual_length) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", idata->phar->filename, internal_file);
			php_stream_close(idata->fp);
			efree(idata);
			efree(internal_file);
			return NULL;
		}
		/* check length, crc32 */
		if (!idata->internal_file->crc_checked
		&& phar_postprocess_file(wrapper, options, idata, crc32 TSRMLS_CC) != SUCCESS) {
			php_stream_close(idata->fp);
			efree(idata);
			efree(internal_file);
			return NULL;
		}
		idata->internal_file->crc_checked = 1;
	}

	stream = php_stream_alloc(&phar_ops, idata, NULL, mode);
	efree(internal_file);
	return stream;
}
/* }}} */

static int phar_close(php_stream *stream, int close_handle TSRMLS_DC) /* {{{ */
{
	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;

	if (data->fp) {
		if (data->internal_file->fp) {
			php_stream_close(data->fp);
		} else {
			data->internal_file->fp = data->fp;
		}
	}

	efree(data);
	return 0;
}
/* }}} */

static int phar_closedir(php_stream *stream, int close_handle TSRMLS_DC)  /* {{{ */
{
	HashTable *data = (HashTable *)stream->abstract;

	if (data)
	{
		zend_hash_destroy(data);
		FREE_HASHTABLE(data);
		stream->abstract = NULL;
	}
	return 0;
}
/* }}} */

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

static size_t phar_read(php_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;

	return php_stream_read(data->fp, buf, count);
}
/* }}} */

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

static int phar_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
{
	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;

	int res = php_stream_seek(data->fp, offset, whence);
	*newoffset = php_stream_tell(data->fp);
	return res;
}
/* }}} */

static size_t phar_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	return 0;
}
/* }}} */

static int phar_flush(php_stream *stream TSRMLS_DC) /* {{{ */
{
	return EOF;
}
/* }}} */

static void phar_dostat(phar_manifest_entry *data, php_stream_statbuf *ssb, zend_bool is_dir, char *alias,
			int alias_len TSRMLS_DC);

static int phar_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	phar_internal_file_data *data;
	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) return -1;

	data = (phar_internal_file_data *)stream->abstract;
	phar_dostat(data->internal_file, ssb, 0, data->phar->alias, data->phar->alias_len TSRMLS_CC);
	return 0;
}
/* }}} */

static void phar_dostat(phar_manifest_entry *data, php_stream_statbuf *ssb, zend_bool is_dir, char *alias,
			int alias_len TSRMLS_DC) /* {{{ */
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
		ssb->sb.st_mtime.tv_sec = 0;
		ssb->sb.st_atime.tv_sec = 0;
		ssb->sb.st_ctime.tv_sec = 0;
#else
		ssb->sb.st_mtime = 0;
		ssb->sb.st_atime = 0;
		ssb->sb.st_ctime = 0;
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

static int phar_stream_stat(php_stream_wrapper *wrapper, char *url, int flags,
				  php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	char *internal_file, *key;
	uint keylen;
	ulong unused;
	phar_file_data *data;
	phar_manifest_entry *file_data;

	resource = php_url_parse(url);

	if (!resource && (resource = phar_open_url(wrapper, url, "r", 0 TSRMLS_CC)) == NULL) {
		return -1;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource || !resource->scheme || !resource->host || !resource->path) {
		if (resource) {
			php_url_free(resource);
		}
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
	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_data), resource->host, strlen(resource->host), (void **) &data)) {
		if (*internal_file == '\0') {
			/* root directory requested */
			phar_dostat(NULL, ssb, 1, data->alias, data->alias_len TSRMLS_CC);
			php_url_free(resource);
			return 0;
		}
		/* search through the manifest of files, and if we have an exact match, it's a file */
		if (SUCCESS == zend_hash_find(&data->manifest, internal_file, strlen(internal_file), (void **) &file_data)) {
			phar_dostat(file_data, ssb, 0, data->alias, data->alias_len TSRMLS_CC);
		} else {
			/* search for directory (partial match of a file) */
			zend_hash_internal_pointer_reset(&data->manifest);
			while (HASH_KEY_NON_EXISTANT != zend_hash_has_more_elements(&data->manifest)) {
				if (HASH_KEY_NON_EXISTANT !=
						zend_hash_get_current_key_ex(
							&data->manifest, &key, &keylen, &unused, 0, NULL)) {
					if (0 == memcmp(internal_file, key, strlen(internal_file))) {
						/* directory found, all dirs have the same stat */
						if (key[strlen(internal_file)] == '/') {
							phar_dostat(NULL, ssb, 1, data->alias, data->alias_len TSRMLS_CC);
							break;
						}
					}
				}
				if (SUCCESS != zend_hash_move_forward(&data->manifest)) {
					break;
				}
			}
		}
	}

	php_url_free(resource);
	return 0;
}
/* }}} */

/* add an empty element with a char * key to a hash table, avoiding duplicates */
static int phar_add_empty(HashTable *ht, char *arKey, uint nKeyLength)  /* {{{ */
{
	void *dummy = (void *) 1;

	return zend_hash_update(ht, arKey, nKeyLength, &dummy, sizeof(void *), NULL);
}
/* }}} */

static int compare_dir_name(const void *a, const void *b TSRMLS_DC)  /* {{{ */
{
	Bucket *f;
	Bucket *s;
	int result;
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);

	result = zend_binary_strcmp(f->arKey, f->nKeyLength,
				    s->arKey, s->nKeyLength);

	if (result == 0) {
		return 0;
	} 

	if (result < 0) {
		return -1;
	} else if (result > 0) {
		return 1;
	} else {
		return 0;
	}

	return 0;
}
/* }}} */

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
	while (HASH_KEY_NON_EXISTANT != zend_hash_has_more_elements(manifest)) {
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
	if (HASH_KEY_NON_EXISTANT != zend_hash_has_more_elements(data)) {
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

static php_stream *phar_opendir(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *key;
	uint keylen;
	ulong unused;
	phar_file_data *data;
	phar_manifest_entry *file_data;

	resource = php_url_parse(filename);

	if (!resource && (resource = phar_open_url(wrapper, filename, mode, options TSRMLS_CC)) == NULL) {
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/ */
	if (!resource || !resource->scheme || !resource->host || !resource->path) {
		if (resource && resource->host && !resource->path) {
			php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory", filename, resource->host);
			php_url_free(resource);
			return NULL;
		}
		if (resource) {
			php_url_free(resource);
		}
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: invalid url \"%s\", must have at least phar://%s/", filename, filename);
		return NULL;
	}

	if (strcasecmp("phar", resource->scheme)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "phar error: not a phar url \"%s\"", filename);
		return NULL;
	}

	internal_file = resource->path + 1; /* strip leading "/" */
	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_data), resource->host, strlen(resource->host), (void **) &data)) {
		if (*internal_file == '\0') {
			/* root directory requested */
			internal_file = estrndup(internal_file - 1, 1);
			ret = phar_make_dirstream(internal_file, &data->manifest TSRMLS_CC);
			php_url_free(resource);
			return ret;
		}
		if (SUCCESS == zend_hash_find(&data->manifest, internal_file, strlen(internal_file), (void **) &file_data)) {
			php_url_free(resource);
			return NULL;
		} else {
			/* search for directory */
			zend_hash_internal_pointer_reset(&data->manifest);
			while (HASH_KEY_NON_EXISTANT != zend_hash_has_more_elements(&data->manifest)) {
				if (HASH_KEY_NON_EXISTANT != 
						zend_hash_get_current_key_ex(
							&data->manifest, &key, &keylen, &unused, 0, NULL)) {
					if (0 == memcmp(key, internal_file, strlen(internal_file))) {
						/* directory found */
						internal_file = estrndup(internal_file,
								strlen(internal_file));
						php_url_free(resource);
						return phar_make_dirstream(internal_file, &data->manifest TSRMLS_CC);
					}
				}
				if (SUCCESS != zend_hash_move_forward(&data->manifest)) {
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
	{NULL, NULL, NULL}	/* Must be the last line in phar_functions[] */
};

static
ZEND_BEGIN_ARG_INFO(arginfo_phar_mapPhar, 0)
	ZEND_ARG_INFO(0, alias)
	ZEND_ARG_INFO(0, compressed)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_phar_loadPhar, 0)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO();

zend_function_entry php_archive_methods[] = {
	PHP_ME(Phar, apiVersion,    NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canCompress,   NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mapPhar,       arginfo_phar_mapPhar,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, loadPhar,      arginfo_phar_loadPhar,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(phar)
{
	zend_class_entry php_archive_entry;
	ZEND_INIT_MODULE_GLOBALS(phar, php_phar_init_globals_module, NULL);

	INIT_CLASS_ENTRY(php_archive_entry, "Phar", php_archive_methods);
	php_archive_entry_ptr = zend_register_internal_class(&php_archive_entry TSRMLS_CC);

	return php_register_url_stream_wrapper("phar", &php_stream_phar_wrapper TSRMLS_CC);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(phar)
{
	return php_unregister_url_stream_wrapper("phar" TSRMLS_CC);
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(phar)
{
	zend_hash_init(&(PHAR_GLOBALS->phar_data), sizeof(phar_file_data),
		 zend_get_hash_value, destroy_phar_data, 0);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(phar)
{
	zend_hash_destroy(&(PHAR_GLOBALS->phar_data));
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(phar)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "phar PHP Archive support", "enabled");
	php_info_print_table_row(2, "phar API version", "0.7.1");
	php_info_print_table_row(2, "CVS revision", "$Revision$");
	php_info_print_table_row(2, "compressed phar support", 
#ifdef HAVE_PHAR_ZLIB
		"enabled");
#else
		"disabled");
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ phar_module_entry
 */
zend_module_entry phar_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"Phar",
	phar_functions,
	PHP_MINIT(phar),
	PHP_MSHUTDOWN(phar),
	PHP_RINIT(phar),
	PHP_RSHUTDOWN(phar),
	PHP_MINFO(phar),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1.0", /* Replace with version number for your extension */
#endif
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
