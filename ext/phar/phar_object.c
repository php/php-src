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

	if (phar_open_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, &phar_data TSRMLS_CC) == FAILURE) {
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
	int fname_len;
	char *contents;
	int contents_len;
	phar_entry_data *data;
	php_stream *fp;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by INI setting");
		return;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &fname, &fname_len, &contents, &contents_len) == FAILURE) {
		return;
	}

	if (!(data = phar_get_or_create_entry_data(phar_obj->arc.archive->fname, phar_obj->arc.archive->fname_len, fname, fname_len TSRMLS_CC))) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created", fname);
	} else {
		fname_len = spprintf(&fname, 0, "phar://%s/%s", phar_obj->arc.archive->fname, fname);
		fp = php_stream_open_wrapper(fname, "w+b", STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
		if (contents_len != php_stream_write(fp, contents, contents_len)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", fname);
		}
		php_stream_close(fp);
		efree(fname);
		efree(data);
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
	phar_entry_data *data;
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
			data = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
			data->phar = phar_obj->arc.archive;
			data->fp = 0;
			/* internal_file is unused in phar_flush, so we won't set it */
			phar_flush(data, 0, 0 TSRMLS_CC);
			efree(data);
			RETURN_TRUE;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int Phar::setStub(string|stream stub [, int len])
 * set the pre-phar stub for the current writeable phar
 */
PHP_METHOD(Phar, setStub)
{
	zval *stub;
	phar_entry_data *idata;
	long len = -1;
	php_stream *stream;
	PHAR_ARCHIVE_OBJECT();
	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot set stub, phar is read-only");
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &stub, &len) == FAILURE) {
		return;
	}

	idata = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	idata->phar = phar_obj->arc.archive;
	idata->fp = 0;
	idata->internal_file = 0;

	if (Z_TYPE_P(stub) == IS_STRING) {
		phar_flush(idata, Z_STRVAL_P(stub), Z_STRLEN_P(stub) TSRMLS_CC);
		efree(idata);
	} else if (Z_TYPE_P(stub) == IS_RESOURCE && (php_stream_from_zval_no_verify(stream, &stub))) {
		if (len > 0) {
			len = -len;
		}
		phar_flush(idata, (char *) &stub, len TSRMLS_CC);
		efree(idata);
	} else {
		efree(idata);
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot only set stub to a string or read from a stream resource");
	}
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
	php_stream *stream, *parent;
	phar_entry_data *idata;
	char *fname;
	int fname_len;
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
	stream = php_stream_fopen_tmpfile();
	if (!entry_obj->ent.entry->temp_file) {
		fname_len = spprintf(&fname, 0, "phar://%s/%s", entry_obj->ent.entry->phar->fname, entry_obj->ent.entry->filename);
		parent = php_stream_open_wrapper_ex(fname, "rb", 0, 0, 0);
		efree(fname);
		php_stream_copy_to_stream(parent, stream, PHP_STREAM_COPY_ALL);
		entry_obj->ent.entry->temp_file = stream;
		php_stream_close(parent);
	}
	if (entry_obj->ent.entry->fp) {
		php_stream_close(entry_obj->ent.entry->fp);
		entry_obj->ent.entry->fp = 0;
	}
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_GZ;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	idata = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	idata->fp = 0;
	idata->internal_file = 0;
	idata->phar = entry_obj->ent.entry->phar;
	phar_flush(idata, 0, 0 TSRMLS_CC);
	efree(idata);
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
	php_stream *stream, *parent;
	phar_entry_data *idata;
	char *fname;
	int fname_len;
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
	stream = php_stream_fopen_tmpfile();
	if (!entry_obj->ent.entry->temp_file) {
		fname_len = spprintf(&fname, 0, "phar://%s/%s", entry_obj->ent.entry->phar->fname, entry_obj->ent.entry->filename);
		parent = php_stream_open_wrapper_ex(fname, "rb", 0, 0, 0);
		efree(fname);
		php_stream_copy_to_stream(parent, stream, PHP_STREAM_COPY_ALL);
		entry_obj->ent.entry->temp_file = stream;
		php_stream_close(parent);
	}
	if (entry_obj->ent.entry->fp) {
		php_stream_close(entry_obj->ent.entry->fp);
		entry_obj->ent.entry->fp = 0;
	}
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_BZ2;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	idata = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	idata->fp = 0;
	idata->internal_file = 0;
	idata->phar = entry_obj->ent.entry->phar;
	phar_flush(idata, 0, 0 TSRMLS_CC);
	efree(idata);
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
	php_stream *stream, *parent;
	phar_entry_data *idata;
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
	stream = php_stream_fopen_tmpfile();
	if (!entry_obj->ent.entry->temp_file) {
		fname_len = spprintf(&fname, 0, "phar://%s/%s", entry_obj->ent.entry->phar->fname, entry_obj->ent.entry->filename);
		parent = php_stream_open_wrapper_ex(fname, "rb", 0, 0, 0);
		efree(fname);
		php_stream_copy_to_stream(parent, stream, PHP_STREAM_COPY_ALL);
		entry_obj->ent.entry->compressed_filesize = entry_obj->ent.entry->uncompressed_filesize;
		entry_obj->ent.entry->temp_file = stream;
		php_stream_close(parent);
	}
	if (entry_obj->ent.entry->fp) {
		php_stream_close(entry_obj->ent.entry->fp);
		entry_obj->ent.entry->fp = 0;
	}
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	idata = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	idata->fp = 0;
	idata->internal_file = 0;
	idata->phar = entry_obj->ent.entry->phar;
	phar_flush(idata, 0, 0 TSRMLS_CC);
	efree(idata);
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_setStub, 0, 0, 0)
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
	PHP_ME(Phar, __construct,   arginfo_phar___construct,  ZEND_ACC_PRIVATE)
#else
	PHP_ME(Phar, __construct,   arginfo_phar___construct,  0)
	PHP_ME(Phar, count,         NULL,                      0)
	PHP_ME(Phar, getVersion,    NULL,                      0)
	PHP_ME(Phar, getSignature,  NULL,                      0)
	PHP_ME(Phar, getModified,   NULL,                      0)
	PHP_ME(Phar, setStub,       arginfo_phar_setStub,      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetGet,     arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetSet,     arginfo_phar_offsetSet,    ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetUnset,   arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetExists,  arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
#endif
	/* static member functions */
	PHP_ME(Phar, apiVersion,    NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canCompress,   NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canWrite,      NULL,                      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mapPhar,       arginfo_phar_mapPhar,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, loadPhar,      arginfo_phar_loadPhar,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
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
