/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

#include "php.h"
#include "php_ini.h"
#include "php_zip.h"

#if HAVE_ZZIPLIB

#include "ext/standard/info.h"
#include <zziplib.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

static int le_zip_dir;
#define le_zip_dir_name "Zip Directory"
static int le_zip_entry;
#define le_zip_entry_name "Zip Entry"

/* {{{ zip_functions[]
 */
function_entry zip_functions[] = {
	PHP_FE(zip_open,                    NULL)
	PHP_FE(zip_read,                    NULL)
	PHP_FE(zip_close,                   NULL)
	PHP_FE(zip_entry_name,              NULL)
	PHP_FE(zip_entry_compressedsize,    NULL)
	PHP_FE(zip_entry_filesize,          NULL)
	PHP_FE(zip_entry_compressionmethod, NULL)
	PHP_FE(zip_entry_open,              NULL)
	PHP_FE(zip_entry_read,              NULL)
	PHP_FE(zip_entry_close,             NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ zip_module_entry
 */
zend_module_entry zip_module_entry = {
	STANDARD_MODULE_HEADER,
	"zip",
	zip_functions,
	PHP_MINIT(zip),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(zip),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ZIP
ZEND_GET_MODULE(zip)
#endif

/* {{{ php_zip_free_dir
 */
static void php_zip_free_dir(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ZZIP_DIR *z_dir = (ZZIP_DIR *) rsrc->ptr;
	zzip_closedir(z_dir);
}
/* }}} */

/* {{{ php_zip_free_entry
 */
static void php_zip_free_entry(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_zzip_dirent *entry = (php_zzip_dirent *) rsrc->ptr;

	if (entry->fp)
		zzip_close(entry->fp);

	efree(entry);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(zip)
{
	le_zip_dir   = zend_register_list_destructors_ex(php_zip_free_dir,   NULL, le_zip_dir_name,   module_number);
	le_zip_entry = zend_register_list_destructors_ex(php_zip_free_entry, NULL, le_zip_entry_name, module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(zip)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Zip support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource zip_open(string filename)
   Open a new zip archive for reading */
PHP_FUNCTION(zip_open)
{
	char     *filename;
	ZZIP_DIR *archive_p = NULL;
	int       filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_ALLOW_FILE_NOT_EXISTS))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(filename TSRMLS_CC)) {
		RETURN_FALSE;
	}

	archive_p = zzip_opendir(filename);
	if (archive_p == NULL) {
		php_error(E_WARNING, "%s() Cannot open zip archive %s", 
				  get_active_function_name(TSRMLS_C), filename);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, archive_p, le_zip_dir);
}
/* }}} */

/* {{{ proto resource zip_read(resource zip)
   Returns the next file in the archive */
PHP_FUNCTION(zip_read)
{
	zval             *zzip_dp;
	ZZIP_DIR         *archive_p = NULL;
	php_zzip_dirent  *entry = NULL;
	int               ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zzip_dp) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(archive_p, ZZIP_DIR *, &zzip_dp, -1, le_zip_dir_name, le_zip_dir);

	entry = emalloc(sizeof(php_zzip_dirent));
	ret = zzip_dir_read(archive_p, &entry->dirent);
	if (ret == 0) {
		efree(entry);
		RETURN_FALSE;
	}
	entry->fp = NULL;

	ZEND_REGISTER_RESOURCE(return_value, entry, le_zip_entry);
}
/* }}} */

/* {{{ proto void zip_close(resource zip)
   Close a Zip archive */
PHP_FUNCTION(zip_close)
{
	zval      *zzip_dp;
	ZZIP_DIR  *archive_p = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zzip_dp) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(archive_p, ZZIP_DIR *, &zzip_dp, -1, le_zip_dir_name, le_zip_dir);

	zend_list_delete(Z_LVAL_P(zzip_dp));
}
/* }}} */

/* {{{ php_zzip_get_entry
 */
static void php_zzip_get_entry(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval             *zzip_ent;
	php_zzip_dirent  *entry = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zzip_ent) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(entry, php_zzip_dirent *, &zzip_ent, -1, le_zip_entry_name, le_zip_entry);

	switch (opt) {
	case 0:
		RETURN_STRING(entry->dirent.d_name, 1);
		break;
	case 1:
		RETURN_LONG(entry->dirent.d_csize);
		break;
	case 2:
		RETURN_LONG(entry->dirent.st_size);
		break;
	case 3:
		RETURN_STRING((char *) zzip_compr_str(entry->dirent.d_compr), 1);
		break;
	}

}
/* }}} */

/* {{{ proto string zip_entry_name(resource zip_entry)
   Return the name given a ZZip entry */
PHP_FUNCTION(zip_entry_name)
{
	php_zzip_get_entry(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int zip_entry_compressedsize(resource zip_entry)
   Return the compressed size of a ZZip entry */
PHP_FUNCTION(zip_entry_compressedsize)
{
	php_zzip_get_entry(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int zip_entry_filesize(resource zip_entry)
   Return the actual filesize of a ZZip entry */
PHP_FUNCTION(zip_entry_filesize)
{
	php_zzip_get_entry(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto string zip_entry_compressionmethod(resource zip_entry)
   Return a string containing the compression method used on a particular entry */
PHP_FUNCTION(zip_entry_compressionmethod)
{
	php_zzip_get_entry(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto bool zip_entry_open(resource zip_dp, resource zip_entry, string mode)
   Open a Zip File, pointed by the resource entry */
PHP_FUNCTION(zip_entry_open)
{
	zval             *zzip_dp;
	zval             *zzip_ent;
	ZZIP_DIR         *archive_p = NULL;
	php_zzip_dirent  *entry = NULL;
	char             *mode;
	int               mode_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|s", &zzip_dp, &zzip_ent, &mode, &mode_len) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(archive_p, ZZIP_DIR *,        &zzip_dp,  -1, le_zip_dir_name,   le_zip_dir);
	ZEND_FETCH_RESOURCE(entry,     php_zzip_dirent *, &zzip_ent, -1, le_zip_entry_name, le_zip_entry);

	entry->fp = zzip_file_open(archive_p, entry->dirent.d_name, O_RDONLY | O_BINARY);

	RETURN_BOOL((int) entry->fp);
}
/* }}} */

/* {{{ proto string zip_entry_read(resource zip_ent)
   Read X bytes from an opened zip entry */
PHP_FUNCTION(zip_entry_read)
{
	zval             *zzip_ent;
	php_zzip_dirent  *entry = NULL;
	char             *buf   = NULL;
	int               len   = 1024;
	int               ret   = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zzip_ent, &len) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(entry, php_zzip_dirent *, &zzip_ent, -1, le_zip_entry_name, le_zip_entry);

	buf = emalloc(len + 1);
	ret = zzip_read(entry->fp, buf, len);
	buf[ret] = 0;
	if (ret == 0) {
		RETURN_FALSE;
	} else {
		RETURN_STRINGL(buf, ret, 0);
	}
}
/* }}} */

/* {{{ proto void zip_entry_close(resource zip_ent)
   Close a zip entry */
PHP_FUNCTION(zip_entry_close)
{
	zval             *zzip_ent;
	php_zzip_dirent  *entry = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zzip_ent) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(entry, php_zzip_dirent *, &zzip_ent, -1, le_zip_entry_name, le_zip_entry);

	zend_list_delete(Z_LVAL_P(zzip_ent));
}
/* }}} */

#endif	/* HAVE_ZZIPLIB */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
