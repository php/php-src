/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
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
#include "zend_constants.h"
#include "php_phar.h"
#ifndef TRUE
 #       define TRUE 1
 #       define FALSE 0
#endif

ZEND_DECLARE_MODULE_GLOBALS(phar)

typedef struct _internal_phar_stream_data {
	phar_file_data *data;
	long	pointer; // relative position within file data
	char	*file;
	phar_manifest_entry	*internal_file;
} phar_internal_file_data;

/* True global resources - no need for thread safety here */

/* borrowed from ext/standard/pack.c */
static int machine_little_endian;
static int little_endian_long_map[4];
/* end borrowing */

static zend_class_entry *php_archive_entry_ptr;

static void destroy_phar_data(void *pDest)
{
	phar_file_data *data = (phar_file_data *) pDest;
	zend_hash_destroy(data->manifest);
}

static void destroy_phar_manifest(void *pDest)
{
	phar_manifest_entry *data = (phar_manifest_entry *)pDest;
	efree(data->filename);
}

static phar_internal_file_data *phar_get_filedata(char *alias, char *path)
{
	phar_file_data *data;
	phar_internal_file_data *ret;
	phar_manifest_entry *file_data;
	
	ret = NULL;
	if (SUCCESS == zend_hash_find(&PHAR_G(phar_data), alias, strlen(alias), (void **) &data)) {
		if (SUCCESS == zend_hash_find(data->manifest, path, strlen(path), (void **) &file_data)) {
			ret = (phar_internal_file_data *) emalloc(sizeof(phar_internal_file_data));
			ret->data = data;
			ret->internal_file = file_data;
			ret->pointer = 0;
		}
	}
	return ret;
}

/* {{{ phar_functions[]
 *
 * Every user visible function must have an entry in phar_functions[].
 */
function_entry phar_functions[] = {
	{NULL, NULL, NULL}	/* Must be the last line in phar_functions[] */
};
/* }}} */


/* {{{ php_archive_methods
 */
PHP_METHOD(PHP_Archive, mapPhar)
{
	char *fname, *alias, *buffer, *endbuffer, *unpack_var, *savebuf;
	phar_file_data *mydata;
	zend_bool compressed;
	phar_manifest_entry *entry;
	HashTable	*manifest;
	int alias_len, i;
	long halt_offset;
	php_uint32 manifest_len, manifest_count, manifest_index;
	zval *halt_constant, **unused1, **unused2;
	FILE *fp;
	struct stat st;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zsb|z", &unused1, &alias, &alias_len, &compressed, &unused2) == FAILURE) {
		return;
	}
#ifndef HAVE_ZLIB
	if (compressed) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error: zlib extension is required for compressed .phar files");
	}
#endif
	fname = zend_get_executed_filename(TSRMLS_C);
	if (!strcmp(fname, "[no active file]")) {
		return;
	}

	MAKE_STD_ZVAL(halt_constant);
	zend_get_constant("__COMPILER_HALT_OFFSET__", 24, halt_constant);
	halt_offset = Z_LVAL(*halt_constant);
	zval_dtor(halt_constant);

	// IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS
	fp = VCWD_FOPEN(fname, "rb");

	/* refuse to open anything that is not a regular file */
	if (fp && (0 > fstat(fileno(fp), &st) || !S_ISREG(st.st_mode))) {
		fclose(fp);
		fp = NULL;
	}
	if (!fp) {
		return;
	}

	// check for ?>\n and increment accordingly
	fseek(fp, halt_offset, SEEK_SET);
	if (FALSE == (buffer = (char *) emalloc(4))) goto MAPPHAR_ALLOC_FAILURE;
	if (3 != fread(buffer, 1, 3, fp)) {
MAPPHAR_FAILURE:
		efree(buffer);
MAPPHAR_ALLOC_FAILURE:
		fclose(fp);
		return;
	}
	if (*buffer == ' ' && *(buffer + 1) == '?' && *(buffer + 2) == '>') {
		halt_offset += 3;
		int nextchar;
		if (EOF == (nextchar = fgetc(fp))) goto MAPPHAR_FAILURE;
		if ((char) nextchar == '\r') {
			if (EOF == (nextchar = fgetc(fp))) goto MAPPHAR_FAILURE;
			halt_offset++;
		}
		if ((char) nextchar == '\n') {
			halt_offset++;
		}
	}
	// make sure we are at the right location to read the manifest
	fseek(fp, halt_offset, SEEK_SET);

	// read in manifest

	i = 0;
#define PHAR_GET_VAL(var)			\
	if (buffer > endbuffer) goto MAPPHAR_FAILURE;\
	unpack_var = (char *) &var;		\
	var = 0;				\
	for (i = 0; i < 4; i++) {		\
		unpack_var[little_endian_long_map[i]] = *buffer++;\
	}

	if (4 != fread(buffer, 1, 4, fp)) goto MAPPHAR_FAILURE;
	endbuffer = buffer;
	PHAR_GET_VAL(manifest_len)
	buffer -= 4;
	if (FALSE == (buffer = (char *) erealloc(buffer, manifest_len)))
		goto MAPPHAR_ALLOC_FAILURE;
	savebuf = buffer;
	// set the test pointer
	endbuffer = buffer + manifest_len;
	// retrieve manifest
	if (manifest_len != fread(buffer, 1, manifest_len, fp)) goto MAPPHAR_FAILURE;
	// extract the number of entries
	PHAR_GET_VAL(manifest_count)
	// set up our manifest
	manifest = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(manifest, sizeof(phar_manifest_entry),
		zend_get_hash_value, destroy_phar_manifest, 0);
	for (manifest_index = 0; manifest_index < manifest_count; manifest_index++) {
		if (buffer > endbuffer) goto MAPPHAR_FAILURE;
		entry = (phar_manifest_entry *) emalloc(sizeof(phar_manifest_entry));
		PHAR_GET_VAL(entry->filename_len)
		entry->filename = (char *) emalloc(entry->filename_len + 1);
		memcpy(entry->filename, buffer, entry->filename_len);
		*(entry->filename + entry->filename_len) = '\0';
		buffer += entry->filename_len;
		PHAR_GET_VAL(entry->uncompressed_filesize)
		PHAR_GET_VAL(entry->timestamp)
		PHAR_GET_VAL(entry->offset_within_phar)
		PHAR_GET_VAL(entry->compressed_filesize)
		zend_hash_add(manifest, entry->filename, entry->filename_len, entry,
			sizeof(phar_manifest_entry), NULL);
	}
#undef PHAR_GET_VAL

	mydata = (phar_file_data *) emalloc(sizeof(phar_file_data));
	mydata->file = fname;
	mydata->alias = alias;
	mydata->alias_len = alias_len;
	mydata->internal_file_start = manifest_len + halt_offset + 4;
	mydata->is_compressed = compressed;
	mydata->manifest = manifest;
	zend_hash_add(&(PHAR_G(phar_data)), alias, alias_len, mydata,
		sizeof(phar_file_data), NULL);
	efree(savebuf);
	fclose(fp);
}

PHP_METHOD(PHP_Archive, apiVersion)
{
	RETURN_STRING("0.7", 3);
}

zend_function_entry php_archive_methods[] = {
	PHP_ME(PHP_Archive, mapPhar, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(PHP_Archive, apiVersion, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	{NULL, NULL, NULL}
};
/* }}} */


/* {{{ phar_module_entry
 */
zend_module_entry phar_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"phar",
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

#ifdef COMPILE_DL_PHAR
ZEND_GET_MODULE(phar)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("phar.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_phar_globals, phar_globals)
    STD_PHP_INI_ENTRY("phar.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_phar_globals, phar_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_phar_init_globals
 */
static void php_phar_init_globals_module(zend_phar_globals *phar_globals)
{
	memset(phar_globals, 0, sizeof(zend_phar_globals));
}

/* }}} */

PHP_PHAR_API php_stream *php_stream_phar_url_wrapper(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
PHP_PHAR_API int phar_close(php_stream *stream, int close_handle TSRMLS_DC);
PHP_PHAR_API size_t phar_read(php_stream *stream, char *buf, size_t count TSRMLS_DC);
PHP_PHAR_API size_t phar_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC);
PHP_PHAR_API int phar_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);

PHP_PHAR_API size_t phar_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
PHP_PHAR_API int phar_flush(php_stream *stream TSRMLS_DC);
PHP_PHAR_API int phar_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);

PHP_PHAR_API int phar_stream_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);
PHP_PHAR_API php_stream *phar_opendir(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);

static php_stream_ops phar_ops = {
	phar_write, // write (does nothing)
	phar_read, //read
	phar_close, //close
	phar_flush, //flush (does nothing)
	"phar stream",
	NULL, // seek
	NULL, // cast
	phar_stat, // stat
	NULL, // set option
};

static php_stream_ops phar_dir_ops = {
	phar_write, // write (does nothing)
	phar_readdir, //read
	phar_close, //close
	phar_flush, //flush (does nothing)
	"phar stream",
	NULL, // seek
	NULL, // cast
	NULL, // stat
	NULL, // set option
};

static php_stream_wrapper_ops phar_stream_wops = {
    php_stream_phar_url_wrapper,
    NULL, /* stream_close */
    NULL, //php_stream_phar_stat,
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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(phar)
{
	ZEND_INIT_MODULE_GLOBALS(phar, php_phar_init_globals_module, NULL);
	zend_class_entry php_archive_entry;
	int machine_endian_check = 1;

	machine_little_endian = ((char *)&machine_endian_check)[0];

	if (machine_little_endian) {
		little_endian_long_map[0] = 0;
		little_endian_long_map[1] = 1;
		little_endian_long_map[2] = 2;
		little_endian_long_map[3] = 3;
	}
	else {
		zval val;
		int size = sizeof(Z_LVAL(val));
		Z_LVAL(val)=0; /*silence a warning*/

		little_endian_long_map[0] = size - 1;
		little_endian_long_map[1] = size - 2;
		little_endian_long_map[2] = size - 3;
		little_endian_long_map[3] = size - 4;
	}
	INIT_CLASS_ENTRY(php_archive_entry, "PHP_Archive", php_archive_methods);
	php_archive_entry_ptr = zend_register_internal_class(&php_archive_entry TSRMLS_CC);
	return php_register_url_stream_wrapper("phar", &php_stream_phar_wrapper TSRMLS_CC);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(phar)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return php_unregister_url_stream_wrapper("phar" TSRMLS_CC);
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(phar)
{
	zend_hash_init(&(PHAR_GLOBALS->phar_data), sizeof(phar_file_data),
		 zend_get_hash_value, destroy_phar_data, 0);
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
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
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
*/
static int phar_postprocess_file(char *contents, php_uint32 nr, unsigned long crc32, zend_bool read)
{
	unsigned int crc = ~0;
	php_uint32 i, actual_length;
	char *unpack_var;
	int len = 0;
	if (read) {
	#define PHAR_GET_VAL(var)			\
		unpack_var = (char *) &var;		\
		var = 0;				\
		for (i = 0; i < 4; i++) {		\
			unpack_var[little_endian_long_map[i]] = *contents++;\
		}
		PHAR_GET_VAL(crc32)
		PHAR_GET_VAL(actual_length)
		if (actual_length != nr) {
			return -1;
		}
	}


	for (len += nr; nr--; ++contents) {
	    CRC32(crc, *contents);
	}
	if (~crc == crc32) {
		return 0;
	} else {
		return -1;
	}
#undef PHAR_GET_VAL	
}

/* {{{ php_stream_phar_url_wrapper
 */
PHP_PHAR_API php_stream * php_stream_phar_url_wrapper(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	phar_internal_file_data *idata;
	php_stream *stream = NULL;
	char *internal_file;
	char *buffer;
	php_url *resource = NULL;
	FILE *fp;
	struct stat st;
#ifdef HAVE_ZLIB
	/* borrowed from zlib.c gzinflate() function */
	int status;
	unsigned long length;
	char *s1=NULL;
	z_stream zstream;
#endif

	resource = php_url_parse((char *) path);
	// we must have at the very least phar://alias.phar/internalfile.php
	if (!resource || !resource->scheme || !resource->host || !resource->path) {
		goto connect_errexit;
	}

	if (strcasecmp("phar", resource->scheme)) {
		goto connect_errexit;
	}

	internal_file = resource->path + 1; /* strip leading "/" */
	if (NULL == (idata = phar_get_filedata(resource->host, internal_file))) {
		return NULL;
	}

	php_url_free(resource);

	// IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS
	fp = VCWD_FOPEN(idata->data->file, "rb");

	/* refuse to open anything that is not a regular file */
	if (fp && (0 > fstat(fileno(fp), &st) || !S_ISREG(st.st_mode))) {
		fclose(fp);
		fp = NULL;
	}
	if (!fp) {
		efree(idata->file);
		efree(idata);
		return NULL;
	}

	/* seek to start of internal file and read it */
	fseek(fp, idata->data->internal_file_start + idata->internal_file->offset_within_phar, SEEK_SET);
	if (idata->data->is_compressed) {
#ifdef HAVE_ZLIB
		buffer = (char *) emalloc(idata->internal_file->compressed_filesize);
		if (idata->internal_file->compressed_filesize !=
				fread(buffer, 1, idata->internal_file->compressed_filesize, fp)) {
			fclose(fp);
			efree(buffer);
			efree(idata);
			return NULL;
		}
		fclose(fp);
		unsigned long crc32;
		php_uint32 actual_length, i;
		char *unpack_var, *savebuf;
		savebuf = buffer;
		#define PHAR_GET_VAL(var)			\
			unpack_var = (char *) &var;		\
			var = 0;				\
			for (i = 0; i < 4; i++) {		\
				unpack_var[little_endian_long_map[i]] = *buffer++;\
			}
		PHAR_GET_VAL(crc32)
		PHAR_GET_VAL(actual_length)
		#undef PHAR_GET_VAL

		/* borrowed from zlib.c gzinflate() function */
		zstream.zalloc = (alloc_func) Z_NULL;
		zstream.zfree = (free_func) Z_NULL;

		length = idata->internal_file->uncompressed_filesize;
		do {
			idata->file = (char *) erealloc(s1, length);

			if (!idata->file && s1) {
				efree(s1);
				efree(savebuf);
				efree(idata->file);
				efree(idata);
				return NULL;
			}

			zstream.next_in = (Bytef *) buffer;
			zstream.avail_in = (uInt) idata->internal_file->compressed_filesize;

			zstream.next_out = idata->file;
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
			s1 = idata->file;
			
		} while (status == Z_BUF_ERROR);

		if (status != Z_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
PHAR_ZLIB_ERROR:
			efree(savebuf);
			efree(idata->file);
			efree(idata);
			return NULL;
		}
		// check length
		if (actual_length != idata->internal_file->uncompressed_filesize) {
			goto PHAR_ZLIB_ERROR;
		}
		// check crc32
		if (-1 == phar_postprocess_file(idata->file, idata->internal_file->uncompressed_filesize, crc32, 0)) {
			goto PHAR_ZLIB_ERROR;
		}
#else
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "zlib extension must be enabled for compressed .phar files");
#endif
	} else {
		idata->file = (char *) emalloc(idata->internal_file->compressed_filesize);
		if (idata->internal_file->compressed_filesize !=
				fread(idata->file, 1, idata->internal_file->compressed_filesize, fp)) {
			fclose(fp);
			efree(idata->file);
			efree(idata);
			return NULL;
		}
		fclose(fp);
		// check length, crc32
		if (-1 == phar_postprocess_file(idata->file, idata->internal_file->uncompressed_filesize, 0, 1)) {
			efree(idata->file);
			efree(idata);
			return NULL;
		}
		memmove(idata->file, idata->file + 8, idata->internal_file->uncompressed_filesize);
	}

	stream = php_stream_alloc(&phar_ops, idata, NULL, mode);
	return stream;

 connect_errexit:
	if (resource) {
		php_url_free(resource);
	}
	return NULL;
}
/* }}} */

PHP_PHAR_API int phar_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;

	efree(data->file);
	efree(data);
	return 0;
}

PHP_PHAR_API size_t phar_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	size_t to_read;
	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;

	to_read = MIN(data->internal_file->uncompressed_filesize - data->pointer, count);
	if (to_read == 0) {
		return 0;
	}

	memcpy(buf, data->file + data->pointer, to_read);
	data->pointer += to_read;
	return to_read;
}

PHP_PHAR_API size_t phar_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	size_t to_read;
	phar_dir_data *data = (phar_dir_data *)stream->abstract;

	phar_dir_entry *ptr;
	to_read = MIN(strlen(data->current->entry), count);
	if (to_read == 0) {
		return 0;
	}
	ptr = data->current;
	data->current = data->current->next;
	memcpy(buf, ptr->entry, to_read);
	efree(ptr);

	return to_read;
}

PHP_PHAR_API int phar_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC)
{
	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;
	switch (whence) {
		case SEEK_SET :
			if (offset < 0 || offset > data->internal_file->uncompressed_filesize) {
				*newoffset = (off_t) - 1;
				return -1;
			}
			data->pointer = offset;
			*newoffset = offset;
			return 0;
		case SEEK_CUR :
			if (data->pointer + offset < 0 || data->pointer + offset
					> data->internal_file->uncompressed_filesize) {
				*newoffset = (off_t) - 1;
				return -1;
			}
			data->pointer += offset;
			*newoffset = data->pointer;
			return 0;
		case SEEK_END :
			if (offset > 0 || -1 * offset > data->internal_file->uncompressed_filesize) {
				*newoffset = (off_t) - 1;
				return -1;
			}
			data->pointer = data->internal_file->uncompressed_filesize + offset;
			*newoffset = data->pointer;
			return 0;
		default :
			*newoffset = (off_t) - 1;
			return -1;
	}
}

PHP_PHAR_API size_t phar_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	return 0;
}

PHP_PHAR_API int phar_flush(php_stream *stream TSRMLS_DC)
{
	return EOF;
}

static void phar_dostat(phar_manifest_entry *data, php_stream_statbuf *ssb, zend_bool is_dir TSRMLS_DC);

PHP_PHAR_API int phar_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) return -1;

	phar_internal_file_data *data = (phar_internal_file_data *)stream->abstract;
	phar_dostat(data->internal_file, ssb, 0);
	return 0;
}

static void phar_dostat(phar_manifest_entry *data, php_stream_statbuf *ssb, zend_bool is_dir TSRMLS_DC)
{
	memset(ssb, 0, sizeof(php_stream_statbuf));
	ssb->sb.st_mode = 0444;

	if (!is_dir) {
		ssb->sb.st_size = data->uncompressed_filesize;
		ssb->sb.st_mode |= S_IFREG;
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
		ssb->sb.st_mode |= S_IFDIR;
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
	ssb->sb.st_blksize = -1;
	ssb->sb.st_blocks = -1;
}

PHP_PHAR_API int phar_stream_stat(php_stream_wrapper *wrapper, char *url, int flags,
				  php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC)

{
	php_url *resource = NULL;
	char *internal_file, *key;
	uint keylen;
	ulong unused;
	phar_file_data *data;
	phar_manifest_entry *file_data;

	resource = php_url_parse(url);
	// we must have at the very least phar://alias.phar/internalfile.php
	if (!resource || !resource->scheme || !resource->host || !resource->path) {
errexit:
		php_url_free(resource);
		return -1;
	}

	if (strcasecmp("phar", resource->scheme)) {
		goto errexit;
	}

	internal_file = resource->path + 1; /* strip leading "/" */
	if (SUCCESS == zend_hash_find(&PHAR_G(phar_data), resource->host, strlen(resource->host), (void **) &data)) {
		if (*internal_file == '\0') {
			// root directory requested
			phar_dostat(NULL, ssb, 1);
			php_url_free(resource);
			return 0;
		}
		if (SUCCESS == zend_hash_find(data->manifest, internal_file, strlen(internal_file), (void **) &file_data)) {
			phar_dostat(file_data, ssb, 0);
		} else {
			// search for directory
			zend_hash_internal_pointer_reset(data->manifest);
			while (zend_hash_has_more_elements(data->manifest)) {
				if (zend_hash_get_current_key_ex(data->manifest, &key, &keylen, &unused, 0, NULL)) {
					if (0 == memcmp(key, internal_file, keylen)) {
						// directory found
						phar_dostat(NULL, ssb, 1);
						break;
					}
				}
				zend_hash_move_forward(data->manifest);
			}
		}
	}

	php_url_free(resource);
	return 0;
}

static php_stream *phar_make_dirstream(char *dir, HashTable *manifest)
{
	phar_dir_data *data;
	int dirlen = strlen(dir);
	char *save, *found, *key;
	uint keylen;
	ulong unused;
	phar_dir_entry *entry = NULL, *prev = NULL;
	data = (phar_dir_data *) emalloc(sizeof(phar_dir_data));

	zend_hash_internal_pointer_reset(manifest);
	while (zend_hash_has_more_elements(manifest)) {
		if (!zend_hash_get_current_key_ex(manifest, &key, &keylen, &unused, 0, NULL)) {
			return NULL;
		}
		if (*dir == '/') {
			// root directory
			if (memchr(key, '/', keylen)) {
				// the entry has a path separator and is not in /
				zend_hash_move_forward(manifest);
				continue;
			}
		} else {
			if (0 != memcmp(key, dir, dirlen)) {
				// entry in directory not found
				zend_hash_move_forward(manifest);
				continue;
			}
		}
		entry = (phar_dir_entry *) emalloc(sizeof(phar_dir_entry));
		save = key;
		save += dirlen + 1; // seek to just past the path separator
		if (NULL != (found = (char *) memchr(save, '/', keylen - dirlen - 1))) {
			// is subdirectory
			entry->entry = (char *) emalloc (found - save + 1);
			memcpy(entry->entry, save, found - save);
			entry->entry[found - save + 1] = '\0';
		} else {
			// is file
			entry->entry = (char *) emalloc (keylen - dirlen - 1);
			memcpy(entry->entry, save, keylen - dirlen - 1);
			entry->entry[keylen - dirlen - 1] = '\0';
		}
		if (prev) {
			prev->next = entry;
		} else {
			data->current = entry;
		}
		prev = entry;
		zend_hash_move_forward(manifest);
	}
	return php_stream_alloc(&phar_dir_ops, data, NULL, "r");
}

PHP_PHAR_API php_stream *phar_opendir(php_stream_wrapper *wrapper, char *filename, char *mode,
			int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_url *resource = NULL;
	php_stream *ret;
	char *internal_file, *key;
	uint keylen;
	ulong unused;
	phar_file_data *data;
	phar_manifest_entry *file_data;

	resource = php_url_parse(filename);
	// we must have at the very least phar://alias.phar/internalfile.php
	if (!resource || !resource->scheme || !resource->host || !resource->path) {
		goto PHAR_DIR_ERROR;
	}

	if (strcasecmp("phar", resource->scheme)) {
		goto PHAR_DIR_ERROR;
	}

	internal_file = resource->path + 1; /* strip leading "/" */
	if (SUCCESS == zend_hash_find(&PHAR_G(phar_data), resource->host, strlen(resource->host), (void **) &data)) {
		if (*internal_file == '\0') {
			// root directory requested
			ret = phar_make_dirstream("/", data->manifest);
			php_url_free(resource);
			return ret;
		}
		if (SUCCESS == zend_hash_find(data->manifest, internal_file, strlen(internal_file), (void **) &file_data)) {
PHAR_DIR_ERROR:
			php_url_free(resource);
			return NULL;
		} else {
			// search for directory
			zend_hash_internal_pointer_reset(data->manifest);
			while (zend_hash_has_more_elements(data->manifest)) {
				if (zend_hash_get_current_key_ex(data->manifest, &key, &keylen, &unused, 0, NULL)) {
					if (0 == memcmp(key, internal_file, keylen)) {
						// directory found
						php_url_free(resource);
						return phar_make_dirstream(internal_file, data->manifest);
					}
				}
				zend_hash_move_forward(data->manifest);
			}
		}
	}

	php_url_free(resource);
	return NULL;
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
