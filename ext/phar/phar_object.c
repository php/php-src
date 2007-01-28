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

#include "phar_internal.h"

static zend_class_entry *phar_ce_archive;

#if HAVE_SPL
static zend_class_entry *phar_ce_entry;
#endif

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

/* {{{ proto mixed Phar::loadPhar(string filename [, string alias])
 * Loads any phar archive with an alias */
PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias = NULL;
	int fname_len, alias_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}
	RETURN_BOOL(phar_open_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto string apiVersion()
 * Returns the api version */
PHP_METHOD(Phar, apiVersion)
{
	RETURN_STRINGL(PHAR_VERSION_STR, sizeof(PHAR_VERSION_STR)-1, 1);
}
/* }}}*/

/* {{{ proto bool canCompress()
 * Returns whether phar extension supports compression using zlib/bzip2 */
PHP_METHOD(Phar, canCompress)
{
#if HAVE_ZLIB || HAVE_BZ2
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool canWrite()
 * Returns whether phar extension supports writing and creating phars */
PHP_METHOD(Phar, canWrite)
{
	RETURN_BOOL(PHAR_G(readonly));
}
/* }}} */

#if HAVE_SPL
/**
 * from spl_directory
 */
static void phar_spl_foreign_dtor(spl_filesystem_object *object TSRMLS_DC) /* {{{ */
{
	phar_archive_delref((phar_archive_data *) object->oth TSRMLS_CC);
	object->oth = NULL;
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

static spl_other_handler phar_spl_foreign_handler = {
       phar_spl_foreign_dtor,
       phar_spl_foreign_clone
};
#endif /* HAVE_SPL */

/* {{{ proto void Phar::__construct(string fname [, int flags [, string alias]])
 * Construct a Phar archive object
 */
PHP_METHOD(Phar, __construct)
{
#if !HAVE_SPL
	zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Cannot instantiate Phar object without SPL extension");
#else
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

	if (phar_open_or_create_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, &phar_data TSRMLS_CC) == FAILURE) {
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
#endif /* HAVE_SPL */
}
/* }}} */

#if HAVE_SPL

#define PHAR_ARCHIVE_OBJECT() \
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	if (!phar_obj->arc.archive) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Cannot call method on an uninitialized Phar object"); \
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

/* {{{ proto void Phar::begin()
 * Do not flush a writeable phar (save its contents) until explicitly requested
 */
PHP_METHOD(Phar, begin)
{
	PHAR_ARCHIVE_OBJECT();
	
	phar_obj->arc.archive->donotflush = 1;
}
/* }}} */

/* {{{ proto bool Phar::commit([string|stream stub [, int len]])
 * Save the contents of a modified phar, with an optional change to the stub
 */
PHP_METHOD(Phar, commit)
{
	zval *stub = NULL;
	long len = -1;
	php_stream *stream;
/*	phar_archive_data *temp;*/
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zl", &stub, &len) == FAILURE) {
		return;
	}

	phar_obj->arc.archive->donotflush = 0;

	if (stub && Z_TYPE_P(stub) == IS_STRING) {
		phar_flush(phar_obj->arc.archive, Z_STRVAL_P(stub), Z_STRLEN_P(stub) TSRMLS_CC);
	} else if (stub && Z_TYPE_P(stub) == IS_RESOURCE && (php_stream_from_zval_no_verify(stream, &stub))) {
		if (len > 0) {
			len = -len;
		} else {
			len = -1;
		}
		phar_flush(phar_obj->arc.archive, (char *) &stub, len TSRMLS_CC);
	} else {
		phar_flush(phar_obj->arc.archive, 0, 0 TSRMLS_CC);
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array|false Phar::getSignature()
 * Return signature or false
 */
PHP_METHOD(Phar, getSignature)
{
	PHAR_ARCHIVE_OBJECT();

	if (phar_obj->arc.archive->signature) {
		array_init(return_value);
		add_assoc_stringl(return_value, "hash", phar_obj->arc.archive->signature, phar_obj->arc.archive->sig_len, 1);
		switch(phar_obj->arc.archive->sig_flags) {
		case PHAR_SIG_MD5:
			add_assoc_stringl(return_value, "hash_type", "md5", 3, 1);
			break;
		case PHAR_SIG_SHA1:
			add_assoc_stringl(return_value, "hash_type", "sha1", 4, 1);
			break;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool Phar::getModified()
 * Return whether phar was modified
 */
PHP_METHOD(Phar, getModified)
{
	PHAR_ARCHIVE_OBJECT();

	RETURN_BOOL(phar_obj->arc.archive->is_modified);
}
/* }}} */

static int phar_set_compression(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;
	php_uint32 compress = *(php_uint32 *)argument;

	if (entry->is_deleted) {
		return ZEND_HASH_APPLY_KEEP;
	}
	entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry->flags |= compress;
	entry->is_modified = 1;
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int phar_test_compression(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;

	if (entry->is_deleted) {
		return ZEND_HASH_APPLY_KEEP;
	}
#if !HAVE_BZ2
	if (entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
		*(int *) argument = 0;
	}
#endif
#if !HAVE_ZLIB
	if (entry->flags & PHAR_ENT_COMPRESSED_GZ) {
		*(int *) argument = 0;
	}
#endif
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void pharobj_set_compression(HashTable *manifest, php_uint32 compress TSRMLS_DC) /* {{{ */
{
	zend_hash_apply_with_argument(manifest, phar_set_compression, &compress TSRMLS_CC);
}
/* }}} */

static int pharobj_cancompress(HashTable *manifest TSRMLS_DC) /* {{{ */
{
	int test;
	test = 1;
	zend_hash_apply_with_argument(manifest, phar_test_compression, &test TSRMLS_CC);
	return test;
}
/* }}} */

/* {{{ proto bool Phar::compressAllFilesGZ()
 * compress every file with GZip compression
 */
PHP_METHOD(Phar, compressAllFilesGZ)
{
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
	}
#if HAVE_ZLIB
	if (!pharobj_cancompress(&phar_obj->arc.archive->manifest TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress all files as Gzip, some are compressed as bzip2 and cannot be uncompressed");
	}
	pharobj_set_compression(&phar_obj->arc.archive->manifest, PHAR_ENT_COMPRESSED_GZ TSRMLS_CC);
	phar_obj->arc.archive->is_modified = 1;
	
	phar_flush(phar_obj->arc.archive, 0, 0 TSRMLS_CC);
#else
	zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
		"Cannot compress with Gzip compression, zlib extension is not enabled");
#endif
}
/* }}} */

/* {{{ proto bool Phar::compressAllFilesBZIP2()
 * compress every file with BZip2 compression
 */
PHP_METHOD(Phar, compressAllFilesBZIP2)
{
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
	}
#if HAVE_BZ2
	if (!pharobj_cancompress(&phar_obj->arc.archive->manifest TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress all files as Bzip2, some are compressed as gzip and cannot be uncompressed");
	}
	pharobj_set_compression(&phar_obj->arc.archive->manifest, PHAR_ENT_COMPRESSED_BZ2 TSRMLS_CC);
	phar_obj->arc.archive->is_modified = 1;
	
	phar_flush(phar_obj->arc.archive, 0, 0 TSRMLS_CC);
#else
	zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
		"Cannot compress with Bzip2 compression, bz2 extension is not enabled");
#endif
}
/* }}} */

/* {{{ proto bool Phar::uncompressAllFiles()
 * uncompress every file
 */
PHP_METHOD(Phar, uncompressAllFiles)
{
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
	}
	if (!pharobj_cancompress(&phar_obj->arc.archive->manifest TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot uncompress all files, some are compressed as bzip2 or gzip and cannot be uncompressed");
	}
	pharobj_set_compression(&phar_obj->arc.archive->manifest, PHAR_ENT_COMPRESSED_NONE TSRMLS_CC);
	phar_obj->arc.archive->is_modified = 1;
	
	phar_flush(phar_obj->arc.archive, 0, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto int Phar::offsetExists(string offset)
 * determines whether a file exists in the phar
 */
PHP_METHOD(Phar, offsetExists)
{
	char *fname;
	int fname_len;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				RETURN_FALSE;
			}
		}
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
		spl_instantiate_arg_ex1(phar_obj->spl.info_class, &return_value, 0, zfname TSRMLS_CC);
		zval_ptr_dtor(&zfname);
	}

}
/* }}} */

/* {{{ proto int Phar::offsetSet(string offset, string value)
 * set the contents of an internal file to those of an external file
 */
PHP_METHOD(Phar, offsetSet)
{
	char *fname;
	char *error;
	int fname_len;
	zval *contents;
	long contents_len = PHP_STREAM_COPY_ALL;
	phar_entry_data *data;
	php_stream *contents_file;
	long offset;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by INI setting");
		return;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &fname, &fname_len, &contents, &contents_len) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(contents) != IS_STRING && Z_TYPE_P(contents) != IS_RESOURCE) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", fname);
	}
	if (!(data = phar_get_or_create_entry_data(phar_obj->arc.archive->fname, phar_obj->arc.archive->fname_len, fname, fname_len, "w+b", &error TSRMLS_CC))) {
		if (error) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created: %s", fname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created", fname);
		}
	} else {
		if (error) {
			efree(error);
		}
		if (Z_TYPE_P(contents) == IS_STRING) {
			contents_len = php_stream_write(data->fp, Z_STRVAL_P(contents), Z_STRLEN_P(contents));
			if (contents_len != Z_STRLEN_P(contents)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", fname);
			}
		} else if (Z_TYPE_P(contents) == IS_RESOURCE) {
			if (!(php_stream_from_zval_no_verify(contents_file, &contents))) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", fname);
			}
			offset = php_stream_copy_to_stream(contents_file, data->fp, contents_len);
			if (contents_len != offset && contents_len != PHP_STREAM_COPY_ALL) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", fname);
			}
		}
		phar_flush(phar_obj->arc.archive, 0, 0 TSRMLS_CC);
		phar_entry_delref(data TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto int Phar::offsetUnset()
 * remove a file from a phar
 */
PHP_METHOD(Phar, offsetUnset)
{
	char *fname;
	int fname_len;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by INI setting");
		return;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				return;
			}
			entry->is_modified = 0;
			entry->is_deleted = 1;
			/* we need to "flush" the stream to save the newly deleted file on disk */
			phar_flush(phar_obj->arc.archive, 0, 0 TSRMLS_CC);
			RETURN_TRUE;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string Phar::getStub()
 * Get the pre-phar stub
 */
PHP_METHOD(Phar, getStub)
{
	char *buf;
	int len;
	php_stream *fp;
	PHAR_ARCHIVE_OBJECT();

	len = phar_obj->arc.archive->halt_offset;
	fp = phar_obj->arc.archive->fp;

	if (!fp) {
		 fp = php_stream_open_wrapper(phar_obj->arc.archive->fname, "rb", 0, NULL);
	}

	if (!fp)  {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Unable to read stub");
		return;
	}

	buf = emalloc(len+1);
	php_stream_rewind(fp);
	if (len != php_stream_read(fp, buf, len)) {
		if (fp != phar_obj->arc.archive->fp) {
			php_stream_close(fp);
		}
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Unable to read stub");
		efree(buf);
		return;
	}
	if (fp != phar_obj->arc.archive->fp) {
		php_stream_close(fp);
	}
	buf[len] = '\0';
	
	RETURN_STRINGL(buf, len, 0);
}
/* }}}*/

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

	if (phar_open_filename(arch, arch_len, NULL, 0, REPORT_ERRORS, &phar_data TSRMLS_CC) == FAILURE) {
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
			"Cannot call method on an uninitialized PharFileInfo object"); \
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

/* {{{ proto bool PharFileInfo::isCompressed()
 * Returns whether the entry is compressed
 */
PHP_METHOD(PharFileInfo, isCompressed)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSION_MASK);
}
/* }}} */

/* {{{ proto bool PharFileInfo::isCompressedGZ()
 * Returns whether the entry is compressed using gz
 */
PHP_METHOD(PharFileInfo, isCompressedGZ)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ);
}
/* }}} */

/* {{{ proto bool PharFileInfo::isCompressedBZIP2()
 * Returns whether the entry is compressed using bzip2
 */
PHP_METHOD(PharFileInfo, isCompressedBZIP2)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2);
}
/* }}} */

/* {{{ proto int PharFileInfo::getCRC32()
 * Returns CRC32 code or throws an exception if not CRC checked
 */
PHP_METHOD(PharFileInfo, getCRC32)
{
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->is_crc_checked) {
		RETURN_LONG(entry_obj->ent.entry->crc32);
	} else {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry was not CRC checked"); \
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::isCRCChecked()
 * Returns whether file entry is CRC checked
 */
PHP_METHOD(PharFileInfo, isCRCChecked)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_BOOL(entry_obj->ent.entry->is_crc_checked);
}
/* }}} */

/* {{{ proto int PharFileInfo::getPharFlags()
 * Returns the Phar file entry flags
 */
PHP_METHOD(PharFileInfo, getPharFlags)
{
	PHAR_ENTRY_OBJECT();
	
	RETURN_LONG(entry_obj->ent.entry->flags & ~(PHAR_ENT_PERM_MASK|PHAR_ENT_COMPRESSION_MASK));
}
/* }}} */

/* {{{ proto int PharFileInfo::getMetaData()
 * Returns the metadata of the entry
 */
PHP_METHOD(PharFileInfo, getMetadata)
{
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->metadata) {
		RETURN_ZVAL(entry_obj->ent.entry->metadata, 1, 0);
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::setMetaData(mixed $metadata)
 * Returns the metadata of the entry
 */
PHP_METHOD(PharFileInfo, setMetadata)
{
	zval *metadata;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &metadata) == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->metadata) {
		zval_ptr_dtor(&entry_obj->ent.entry->metadata);
		entry_obj->ent.entry->metadata = NULL;
	}

	MAKE_STD_ZVAL(entry_obj->ent.entry->metadata);
	ZVAL_ZVAL(entry_obj->ent.entry->metadata, metadata, 1, 0);
}
/* }}} */

/* {{{ proto int PharFileInfo::setCompressedGZ()
 * Instructs the Phar class to compress the current file using zlib
 */
PHP_METHOD(PharFileInfo, setCompressedGZ)
{
#if HAVE_ZLIB
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) {
		RETURN_TRUE;
		return;
	}
	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
	}
	if (entry_obj->ent.entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress deleted file");
	}
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_GZ;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	phar_flush(entry_obj->ent.entry->phar, 0, 0 TSRMLS_CC);
	RETURN_TRUE;
#else
	zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
		"Cannot compress with Gzip compression, zlib extension is not enabled");
#endif
}
/* }}} */

/* {{{ proto int PharFileInfo::setCompressedBZIP2()
 * Instructs the Phar class to compress the current file using bzip2
 */
PHP_METHOD(PharFileInfo, setCompressedBZIP2)
{
#if HAVE_BZ2
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
		RETURN_TRUE;
		return;
	}
	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
	}
	if (entry_obj->ent.entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress deleted file");
	}
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_BZ2;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	phar_flush(entry_obj->ent.entry->phar, 0, 0 TSRMLS_CC);
	RETURN_TRUE;
#else
	zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
		"Cannot compress with Bzip2 compression, bzip2 extension is not enabled");
#endif
}
/* }}} */

/* {{{ proto int PharFileInfo::setUncompressed()
 * Instructs the Phar class to uncompress the current file
 */
PHP_METHOD(PharFileInfo, setUncompressed)
{
	char *fname;
	int fname_len;
	PHAR_ENTRY_OBJECT();

	if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSION_MASK) == 0) {
		RETURN_TRUE;
		return;
	}
	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
	}
	if (entry_obj->ent.entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress deleted file");
	}
#if !HAVE_ZLIB
	if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot uncompress Gzip-compressed file, zlib extension is not enabled");
	}
#endif
#if !HAVE_BZ2
	if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot uncompress Bzip2-compressed file, bzip2 extension is not enabled");
	}
#endif
	if (!entry_obj->ent.entry->fp) {
		fname_len = spprintf(&fname, 0, "phar://%s/%s", entry_obj->ent.entry->phar->fname, entry_obj->ent.entry->filename);
		entry_obj->ent.entry->fp = php_stream_open_wrapper_ex(fname, "rb", 0, 0, 0);
		efree(fname);
	}
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	phar_flush(entry_obj->ent.entry->phar, 0, 0 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_SPL */

/* {{{ phar methods */

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
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_setStub, 0, 0, 1)
	ZEND_ARG_INFO(0, newstub)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_loadPhar, 0, 0, 1)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO();

#if HAVE_SPL
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();
#endif

zend_function_entry php_archive_methods[] = {
#if !HAVE_SPL
	PHP_ME(Phar, __construct,           arginfo_phar___construct,  ZEND_ACC_PRIVATE)
#else
	PHP_ME(Phar, __construct,           arginfo_phar___construct,  ZEND_ACC_PUBLIC)
	PHP_ME(Phar, count,                 NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getModified,           NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getSignature,          NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getStub,               NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getVersion,            NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, begin,                 NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, compressAllFilesGZ,    NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, compressAllFilesBZIP2, NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, uncompressAllFiles,    NULL,                      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, commit,                arginfo_phar_setStub,      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetExists,          arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetGet,             arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetSet,             arginfo_phar_offsetSet,    ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetUnset,           arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
#endif
	/* static member functions */
	PHP_ME(Phar, apiVersion,            NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canCompress,           NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canWrite,              NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, loadPhar,              arginfo_phar_loadPhar,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mapPhar,               arginfo_phar_mapPhar,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	{NULL, NULL, NULL}
};

#if HAVE_SPL
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_entry___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_entry_setMetadata, 0, 0, 1)
	ZEND_ARG_INFO(0, metadata)
ZEND_END_ARG_INFO();

zend_function_entry php_entry_methods[] = {
	PHP_ME(PharFileInfo, __construct,        arginfo_entry___construct,  0)
	PHP_ME(PharFileInfo, getCompressedSize,  NULL,                       0)
	PHP_ME(PharFileInfo, isCompressed,       NULL,                       0)
	PHP_ME(PharFileInfo, isCompressedGZ,     NULL,                       0)
	PHP_ME(PharFileInfo, isCompressedBZIP2,  NULL,                       0)
	PHP_ME(PharFileInfo, setUncompressed,    NULL,                       0)
	PHP_ME(PharFileInfo, setCompressedGZ,    NULL,                       0)
	PHP_ME(PharFileInfo, setCompressedBZIP2, NULL,                       0)
	PHP_ME(PharFileInfo, getCRC32,           NULL,                       0)
	PHP_ME(PharFileInfo, isCRCChecked,       NULL,                       0)
	PHP_ME(PharFileInfo, getPharFlags,       NULL,                       0)
	PHP_ME(PharFileInfo, getMetadata,        NULL,                       0)
	PHP_ME(PharFileInfo, setMetadata,        arginfo_entry_setMetadata,  0)
	{NULL, NULL, NULL}
};
#endif
/* }}} */

#define REGISTER_PHAR_CLASS_CONST_LONG(class_name, const_name, value) \
	zend_declare_class_constant_long(class_name, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

void phar_object_init(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

#if HAVE_SPL
	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator, NULL  TSRMLS_CC);

	zend_class_implements(phar_ce_archive TSRMLS_CC, 2, spl_ce_Countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharFileInfo", php_entry_methods);
	phar_ce_entry = zend_register_internal_class_ex(&ce, spl_ce_SplFileInfo, NULL  TSRMLS_CC);
#else
	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class(&ce TSRMLS_CC);
	phar_ce_archive->ce_flags |= ZEND_ACC_FINAL_CLASS;
#endif

	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "COMPRESSED", PHAR_ENT_COMPRESSION_MASK)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "GZ", PHAR_ENT_COMPRESSED_GZ)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "BZ2", PHAR_ENT_COMPRESSED_BZ2)
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
