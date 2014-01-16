/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#if HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"

#ifndef OCI_STMT_CALL
#define OCI_STMT_CALL 10
#endif

/* {{{ proto bool oci_define_by_name(resource stmt, string name, mixed &var [, int type])
   Define a PHP variable to an Oracle column by name */
/* if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!! */
PHP_FUNCTION(oci_define_by_name)
{
	zval *stmt, *var;
	char *name;
	int name_len;
	long type = 0;
	php_oci_statement *statement;
	php_oci_define *define, *tmp_define;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsz/|l", &stmt, &name, &name_len, &var, &type) == FAILURE) {
		return;
	}

	if (!name_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column name cannot be empty");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(stmt, statement);

	if (statement->defines == NULL) {
		ALLOC_HASHTABLE(statement->defines);
		zend_hash_init(statement->defines, 13, NULL, php_oci_define_hash_dtor, 0);
	}

	define = ecalloc(1,sizeof(php_oci_define));

	if (zend_hash_add(statement->defines, name, name_len, define, sizeof(php_oci_define), (void **)&tmp_define) == SUCCESS) {
		efree(define);
		define = tmp_define;
	} else {
		efree(define);
		RETURN_FALSE;
	}

	define->name = (text*) estrndup(name, name_len);
	define->name_len = name_len;
	define->type = type;
	define->zval = var;
	zval_add_ref(&var);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_bind_by_name(resource stmt, string name, mixed &var, [, int maxlength [, int type]])
   Bind a PHP variable to an Oracle placeholder by name */
/* if you want to bind a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE binding!!! */
PHP_FUNCTION(oci_bind_by_name)
{
	ub2	bind_type = SQLT_CHR; /* unterminated string */
	int name_len;
	long maxlen = -1, type = 0;
	char *name;
	zval *z_statement;
	zval *bind_var = NULL;
	php_oci_statement *statement;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsz/|ll", &z_statement, &name, &name_len, &bind_var, &maxlen, &type) == FAILURE) {
		return;
	}

	if (type) {
		bind_type = (ub2) type;
	}
	
	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_bind_by_name(statement, name, name_len, bind_var, maxlen, bind_type TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_bind_array_by_name(resource stmt, string name, array &var, int max_table_length [, int max_item_length [, int type ]])
   Bind a PHP array to an Oracle PL/SQL type by name */
PHP_FUNCTION(oci_bind_array_by_name)
{
	int name_len;
	long max_item_len = -1;
	long max_array_len = 0;
	long type = SQLT_AFC;
	char *name;
	zval *z_statement;
	zval *bind_var = NULL;
	php_oci_statement *statement;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsz/l|ll", &z_statement, &name, &name_len, &bind_var, &max_array_len, &max_item_len, &type) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (ZEND_NUM_ARGS() == 5 && max_item_len <= 0) {
		max_item_len = -1;
	}
	
	if (max_array_len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Maximum array length must be greater than zero");
		RETURN_FALSE;
	}
	
	if (php_oci_bind_array_by_name(statement, name, name_len, bind_var, max_array_len, max_item_len, type TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_free_descriptor()
   Deletes large object description */
PHP_FUNCTION(oci_free_descriptor)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;

	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	zend_list_delete(descriptor->id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_save( string data [, int offset ])
   Saves a large object */
PHP_FUNCTION(oci_lob_save)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	char *data;
	int data_len;
	long offset = 0;
	ub4 bytes_written;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &offset) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|l", &z_descriptor, oci_lob_class_entry_ptr, &data, &data_len, &offset) == FAILURE) {
			return;
		}
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (offset < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset parameter must be greater than or equal to 0");
		RETURN_FALSE;
	}
	
	if (php_oci_lob_write(descriptor, offset, data, data_len, &bytes_written TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_import( string filename )
   Loads file into a LOB */
PHP_FUNCTION(oci_lob_import)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	char *filename;
	int filename_len;

	if (getThis()) {
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 3) || (PHP_MAJOR_VERSION > 5)
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
#else
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
#endif
			return;
		}
	}
	else {
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 3) || (PHP_MAJOR_VERSION > 5)
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Op", &z_descriptor, oci_lob_class_entry_ptr, &filename, &filename_len) == FAILURE) {
#else
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &z_descriptor, oci_lob_class_entry_ptr, &filename, &filename_len) == FAILURE) {
#endif
			return;
		}	
	}

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4) || (PHP_MAJOR_VERSION < 5)
	/* The "p" parsing parameter handles this case in PHP 5.4+ */
	if (strlen(filename) != filename_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename cannot contain null bytes");
		RETURN_FALSE;  
	}
#endif

	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (php_oci_lob_import(descriptor, filename TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string oci_lob_load()
   Loads a large object */
PHP_FUNCTION(oci_lob_load)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	char *buffer = NULL;
	ub4 buffer_len;

	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (php_oci_lob_read(descriptor, -1, 0, &buffer, &buffer_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	if (buffer_len > 0) {
		RETURN_STRINGL(buffer, buffer_len, 0);
	}
	else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string oci_lob_read( int length )
   Reads particular part of a large object */
PHP_FUNCTION(oci_lob_read)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	long length;
	char *buffer;
	ub4 buffer_len;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ol", &z_descriptor, oci_lob_class_entry_ptr, &length) == FAILURE) {
			return;
		}	
	}

	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (length <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}
	
	if (php_oci_lob_read(descriptor, length, descriptor->lob_current_position, &buffer, &buffer_len TSRMLS_CC)) {
		RETURN_FALSE;
	}	
	if (buffer_len > 0) {
		RETURN_STRINGL(buffer, buffer_len, 0);
	}
	else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto bool oci_lob_eof()
   Checks if EOF is reached */
PHP_FUNCTION(oci_lob_eof)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	ub4 lob_length;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (!php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC) && lob_length >= 0) {
		if (lob_length == descriptor->lob_current_position) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_lob_tell()
   Tells LOB pointer position */
PHP_FUNCTION(oci_lob_tell)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	RETURN_LONG(descriptor->lob_current_position);	
}
/* }}} */

/* {{{ proto bool oci_lob_rewind()
   Rewind pointer of a LOB */
PHP_FUNCTION(oci_lob_rewind)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	descriptor->lob_current_position = 0;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_seek( int offset [, int whence ])
   Moves the pointer of a LOB */
PHP_FUNCTION(oci_lob_seek)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	long offset, whence = PHP_OCI_SEEK_SET;
	ub4 lob_length;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &offset, &whence) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ol|l", &z_descriptor, oci_lob_class_entry_ptr, &offset, &whence) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC)) {
		RETURN_FALSE;
	}

	switch(whence) {
		case PHP_OCI_SEEK_CUR:
			descriptor->lob_current_position += offset;
			break;
		case PHP_OCI_SEEK_END:
			if ((descriptor->lob_size + offset) >= 0) {
				descriptor->lob_current_position = descriptor->lob_size + offset;
			}
			else {
				descriptor->lob_current_position = 0;
			}
			break;
		case PHP_OCI_SEEK_SET:
		default:
				descriptor->lob_current_position = (offset > 0) ? offset : 0;
			break;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int oci_lob_size()
   Returns size of a large object */
PHP_FUNCTION(oci_lob_size)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	ub4 lob_length;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_LONG(lob_length);
}
/* }}} */

/* {{{ proto int oci_lob_write( string string [, int length ])
   Writes data to current position of a LOB */
PHP_FUNCTION(oci_lob_write)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	int data_len;
	long write_len = 0;
	ub4 bytes_written;
	char *data;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &write_len) == FAILURE) {
			return;
		}
		
		if (ZEND_NUM_ARGS() == 2) {
			data_len = MIN(data_len, write_len);
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|l", &z_descriptor, oci_lob_class_entry_ptr, &data, &data_len, &write_len) == FAILURE) {
			return;
		}

		if (ZEND_NUM_ARGS() == 3) {
			data_len = MIN(data_len, write_len);
		}
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (data_len <= 0) {
		RETURN_LONG(0);
	}
	
	if (php_oci_lob_write(descriptor, descriptor->lob_current_position, data, data_len, &bytes_written TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes_written);
}
/* }}} */

/* {{{ proto bool oci_lob_append( object lob )
   Appends data from a LOB to another LOB */
PHP_FUNCTION(oci_lob_append)
{
	zval **tmp_dest, **tmp_from, *z_descriptor_dest = getThis(), *z_descriptor_from;
	php_oci_descriptor *descriptor_dest, *descriptor_from;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor_from, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO", &z_descriptor_dest, oci_lob_class_entry_ptr, &z_descriptor_from, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor_dest), "descriptor", sizeof("descriptor"), (void **)&tmp_dest) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property. The first argument should be valid descriptor object");
		RETURN_FALSE;
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor_from), "descriptor", sizeof("descriptor"), (void **)&tmp_from) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property. The second argument should be valid descriptor object");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp_dest, descriptor_dest);
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp_from, descriptor_from);
	
	if (php_oci_lob_append(descriptor_dest, descriptor_from TSRMLS_CC)) {
		RETURN_FALSE;
	}
	/* XXX should we increase lob_size here ? */
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_truncate( [ int length ])
   Truncates a LOB */
PHP_FUNCTION(oci_lob_truncate)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	long trim_length = 0;
	ub4 ub_trim_length;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &trim_length) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &z_descriptor, oci_lob_class_entry_ptr, &trim_length) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	if (trim_length < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length must be greater than or equal to zero");
		RETURN_FALSE;
	}

	ub_trim_length = (ub4) trim_length;
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (php_oci_lob_truncate(descriptor, ub_trim_length TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int oci_lob_erase( [ int offset [, int length ] ] )
   Erases a specified portion of the internal LOB, starting at a specified offset */
PHP_FUNCTION(oci_lob_erase)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	ub4 bytes_erased;
	long offset = -1, length = -1;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &offset, &length) == FAILURE) {
			return;
		}

		if (ZEND_NUM_ARGS() > 0 && offset < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset must be greater than or equal to 0");
			RETURN_FALSE;
		}

		if (ZEND_NUM_ARGS() > 1 && length < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length must be greater than or equal to 0");
			RETURN_FALSE;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|ll", &z_descriptor, oci_lob_class_entry_ptr, &offset, &length) == FAILURE) {
			return;
		}

		if (ZEND_NUM_ARGS() > 1 && offset < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset must be greater than or equal to 0");
			RETURN_FALSE;
		}
		
		if (ZEND_NUM_ARGS() > 2 && length < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length must be greater than or equal to 0");
			RETURN_FALSE;
		}
	}

	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (php_oci_lob_erase(descriptor, offset, length, &bytes_erased TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes_erased);
}
/* }}} */

/* {{{ proto bool oci_lob_flush( [ int flag ] )
   Flushes the LOB buffer */
PHP_FUNCTION(oci_lob_flush)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	long flush_flag = 0;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flush_flag) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &z_descriptor, oci_lob_class_entry_ptr, &flush_flag) == FAILURE) {
			return;
		}
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (descriptor->buffering == PHP_OCI_LOB_BUFFER_DISABLED) {
		/* buffering wasn't enabled, there is nothing to flush */
		RETURN_FALSE;
	}

	if (php_oci_lob_flush(descriptor, flush_flag TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ocisetbufferinglob( boolean flag )
   Enables/disables buffering for a LOB */
PHP_FUNCTION(ocisetbufferinglob)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	zend_bool flag;
	
	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &flag) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ob", &z_descriptor, oci_lob_class_entry_ptr, &flag) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (php_oci_lob_set_buffering(descriptor, flag TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ocigetbufferinglob()
   Returns current state of buffering for a LOB */
PHP_FUNCTION(ocigetbufferinglob)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}

	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (descriptor->buffering != PHP_OCI_LOB_BUFFER_DISABLED) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_copy( object lob_to, object lob_from [, int length ] )
   Copies data from a LOB to another LOB */
PHP_FUNCTION(oci_lob_copy)
{
	zval **tmp_dest, **tmp_from, *z_descriptor_dest, *z_descriptor_from;
	php_oci_descriptor *descriptor_dest, *descriptor_from;
	long length = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO|l", &z_descriptor_dest, oci_lob_class_entry_ptr, &z_descriptor_from, oci_lob_class_entry_ptr, &length) == FAILURE) {
		return;
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor_dest), "descriptor", sizeof("descriptor"), (void **)&tmp_dest) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property. The first argument should be valid descriptor object");
		RETURN_FALSE;
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor_from), "descriptor", sizeof("descriptor"), (void **)&tmp_from) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property. The second argument should be valid descriptor object");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp_dest, descriptor_dest);
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp_from, descriptor_from);
	
	if (ZEND_NUM_ARGS() == 3 && length < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}
	
	if (ZEND_NUM_ARGS() == 2) {
		/* indicate that we want to copy from the current position to the end of the LOB */
		length = -1;
	}

	if (php_oci_lob_copy(descriptor_dest, descriptor_from, length TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_is_equal( object lob1, object lob2 )
   Tests to see if two LOB/FILE locators are equal */
PHP_FUNCTION(oci_lob_is_equal)
{
	zval **tmp_first, **tmp_second, *z_descriptor_first, *z_descriptor_second;
	php_oci_descriptor *descriptor_first, *descriptor_second;
	boolean is_equal;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO", &z_descriptor_first, oci_lob_class_entry_ptr, &z_descriptor_second, oci_lob_class_entry_ptr) == FAILURE) {
		return;
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor_first), "descriptor", sizeof("descriptor"), (void **)&tmp_first) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property. The first argument should be valid descriptor object");
		RETURN_FALSE;
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor_second), "descriptor", sizeof("descriptor"), (void **)&tmp_second) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property. The second argument should be valid descriptor object");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp_first, descriptor_first);
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp_second, descriptor_second);

	if (php_oci_lob_is_equal(descriptor_first, descriptor_second, &is_equal TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	if (is_equal == TRUE) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_export([string filename [, int start [, int length]]])
   Writes a large object into a file */
PHP_FUNCTION(oci_lob_export)
{	
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	char *filename;
	char *buffer;
	int filename_len;
	long start = -1, length = -1, block_length;
	php_stream *stream;
	ub4 lob_length;

	if (getThis()) {
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 3) || (PHP_MAJOR_VERSION > 5)
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|ll", &filename, &filename_len, &start, &length) == FAILURE) {
#else
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &filename, &filename_len, &start, &length) == FAILURE) {
#endif
			return;
		}
	
		if (ZEND_NUM_ARGS() > 1 && start < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Start parameter must be greater than or equal to 0");
			RETURN_FALSE;
		}
		if (ZEND_NUM_ARGS() > 2 && length < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than or equal to 0");
			RETURN_FALSE;
		}
	}
	else {
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 3) || (PHP_MAJOR_VERSION > 5)
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Op|ll", &z_descriptor, oci_lob_class_entry_ptr, &filename, &filename_len, &start, &length) == FAILURE) {
#else
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|ll", &z_descriptor, oci_lob_class_entry_ptr, &filename, &filename_len, &start, &length) == FAILURE) {
#endif
			return;
		}
			
		if (ZEND_NUM_ARGS() > 2 && start < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Start parameter must be greater than or equal to 0");
			RETURN_FALSE;
		}
		if (ZEND_NUM_ARGS() > 3 && length < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than or equal to 0");
			RETURN_FALSE;
		}
	}

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4) || (PHP_MAJOR_VERSION < 5)
	/* The "p" parsing parameter handles this case in PHP 5.4+ */
	if (strlen(filename) != filename_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename cannot contain null bytes");
		RETURN_FALSE;  
	}
#endif

	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);
	
	if (php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC)) {
		RETURN_FALSE;
	}		
	
	if (start == -1) {
		start = 0;
	}

	if (length == -1) {
		length = lob_length - descriptor->lob_current_position;
	}
	
	if (length == 0) {
		/* nothing to write, fail silently */
		RETURN_FALSE;
	}

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4) || (PHP_MAJOR_VERSION < 5)
	/* Safe mode has been removed in PHP 5.4 */
	if (PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}
#endif

	if (php_check_open_basedir(filename TSRMLS_CC)) {
		RETURN_FALSE;
	}

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 3) || (PHP_MAJOR_VERSION > 5)
	stream = php_stream_open_wrapper_ex(filename, "w", REPORT_ERRORS, NULL, NULL);
#else
	stream = php_stream_open_wrapper_ex(filename, "w", ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL);
#endif

	block_length = PHP_OCI_LOB_BUFFER_SIZE;
	if (block_length > length) {
		block_length = length;
	}

	while(length > 0) {
		ub4 tmp_bytes_read = 0;
		if (php_oci_lob_read(descriptor, block_length, start, &buffer, &tmp_bytes_read TSRMLS_CC)) {
			php_stream_close(stream);
			RETURN_FALSE;
		}
		if (tmp_bytes_read && !php_stream_write(stream, buffer, tmp_bytes_read)) {
			php_stream_close(stream);
			efree(buffer);
			RETURN_FALSE;
		}
		if (buffer) {
			efree(buffer);
		}
		
		length -= tmp_bytes_read;
		descriptor->lob_current_position += tmp_bytes_read;
		start += tmp_bytes_read;

		if (block_length > length) {
			block_length = length;
		}
	}

	php_stream_close(stream);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_write_temporary(string var [, int lob_type])
   Writes temporary blob */
PHP_FUNCTION(oci_lob_write_temporary)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	char *data;
	int data_len;
	long type = OCI_TEMP_CLOB;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &type) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|l", &z_descriptor, oci_lob_class_entry_ptr, &data, &data_len, &type) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (php_oci_lob_write_tmp(descriptor, type, data, data_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_close()
   Closes lob descriptor */
PHP_FUNCTION(oci_lob_close)
{
	zval **tmp, *z_descriptor = getThis();
	php_oci_descriptor *descriptor;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_DESCRIPTOR(*tmp, descriptor);

	if (php_oci_lob_close(descriptor TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object oci_new_descriptor(resource connection [, int type])
   Initialize a new empty descriptor LOB/FILE (LOB is default) */
PHP_FUNCTION(oci_new_descriptor)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_descriptor *descriptor;
	long type = OCI_DTYPE_LOB;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &z_connection, &type) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	/* php_oci_lob_create() checks type */
	descriptor = php_oci_lob_create(connection, type TSRMLS_CC);	
	
	if (!descriptor) {
		RETURN_NULL();
	}

	object_init_ex(return_value, oci_lob_class_entry_ptr);
	add_property_resource(return_value, "descriptor", descriptor->id);
}
/* }}} */

/* {{{ proto bool oci_rollback(resource connection)
   Rollback the current context */
PHP_FUNCTION(oci_rollback)
{
	zval *z_connection;
	php_oci_connection *connection;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_connection) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (connection->descriptors) {
		php_oci_connection_descriptors_free(connection TSRMLS_CC);
	}

	if (php_oci_connection_rollback(connection TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_commit(resource connection)
   Commit the current context */
PHP_FUNCTION(oci_commit)
{
	zval *z_connection;
	php_oci_connection *connection;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_connection) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (connection->descriptors) {
		php_oci_connection_descriptors_free(connection TSRMLS_CC);
	}
	
	if (php_oci_connection_commit(connection TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string oci_field_name(resource stmt, int col)
   Tell the name of a column */
PHP_FUNCTION(oci_field_name)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		RETURN_STRINGL(column->name, column->name_len, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_field_size(resource stmt, int col)
   Tell the maximum data size of a column */
PHP_FUNCTION(oci_field_size)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		/* Handle data type of LONG */
		if (column->data_type == SQLT_LNG){
			RETURN_LONG(column->storage_size4);
		}
		RETURN_LONG(column->data_size);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_field_scale(resource stmt, int col)
   Tell the scale of a column */
PHP_FUNCTION(oci_field_scale)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		RETURN_LONG(column->scale);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_field_precision(resource stmt, int col)
   Tell the precision of a column */
PHP_FUNCTION(oci_field_precision)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		RETURN_LONG(column->precision);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed oci_field_type(resource stmt, int col)
   Tell the data type of a column */
PHP_FUNCTION(oci_field_type)
{
	php_oci_out_column *column;

	column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);

	if (!column) {
		RETURN_FALSE;
	}
	
	switch (column->data_type) {
#ifdef SQLT_TIMESTAMP
		case SQLT_TIMESTAMP:
			RETVAL_STRING("TIMESTAMP",1);
			break;
#endif
#ifdef SQLT_TIMESTAMP_TZ
		case SQLT_TIMESTAMP_TZ:
			RETVAL_STRING("TIMESTAMP WITH TIMEZONE",1);
			break;
#endif
#ifdef SQLT_TIMESTAMP_LTZ
		case SQLT_TIMESTAMP_LTZ:
			RETVAL_STRING("TIMESTAMP WITH LOCAL TIMEZONE",1);
			break;
#endif
#ifdef SQLT_INTERVAL_YM
		case SQLT_INTERVAL_YM:
			RETVAL_STRING("INTERVAL YEAR TO MONTH",1);
			break;
#endif
#ifdef SQLT_INTERVAL_DS
		case SQLT_INTERVAL_DS:
			RETVAL_STRING("INTERVAL DAY TO SECOND",1);
			break;
#endif
		case SQLT_DAT:
			RETVAL_STRING("DATE",1);
			break;
		case SQLT_NUM:
			RETVAL_STRING("NUMBER",1);
			break;
		case SQLT_LNG:
			RETVAL_STRING("LONG",1);
			break;
		case SQLT_BIN:
			RETVAL_STRING("RAW",1);
			break;
		case SQLT_LBI:
			RETVAL_STRING("LONG RAW",1);
			break;
		case SQLT_CHR:
			RETVAL_STRING("VARCHAR2",1);
			break;
		case SQLT_RSET:
			RETVAL_STRING("REFCURSOR",1);
			break;
		case SQLT_AFC:
			RETVAL_STRING("CHAR",1);
			break;
		case SQLT_BLOB:
			RETVAL_STRING("BLOB",1);
			break;
		case SQLT_CLOB:
			RETVAL_STRING("CLOB",1);
			break;
		case SQLT_BFILE:
			RETVAL_STRING("BFILE",1);
			break;
		case SQLT_RDD:
			RETVAL_STRING("ROWID",1);
			break;
		default:
			RETVAL_LONG(column->data_type);
	}
}
/* }}} */

/* {{{ proto int oci_field_type_raw(resource stmt, int col)
   Tell the raw oracle data type of a column */
PHP_FUNCTION(oci_field_type_raw)
{
	php_oci_out_column *column;

	column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if (column) {
		RETURN_LONG(column->data_type);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_field_is_null(resource stmt, int col)
   Tell whether a column is NULL */
PHP_FUNCTION(oci_field_is_null)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		if (column->indicator == -1) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void oci_internal_debug(int onoff)
   Toggle internal debugging output for the OCI extension */
PHP_FUNCTION(oci_internal_debug)
{
	/* NOP in OCI8 2.0. Obsoleted by DTrace probes */
}
/* }}} */

/* {{{ proto bool oci_execute(resource stmt [, int mode])
   Execute a parsed statement */
PHP_FUNCTION(oci_execute)
{
	zval *z_statement;
	php_oci_statement *statement;
	long mode = OCI_COMMIT_ON_SUCCESS;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &z_statement, &mode) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_execute(statement, mode TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_cancel(resource stmt)
   Cancel reading from a cursor */
PHP_FUNCTION(oci_cancel)
{
	zval *z_statement;
	php_oci_statement *statement;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_cancel(statement TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_fetch(resource stmt)
   Prepare a new row of data for reading */
PHP_FUNCTION(oci_fetch)
{
	zval *z_statement;
	php_oci_statement *statement;
	ub4 nrows = 1; /* only one row at a time is supported for now */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_fetch(statement, nrows TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ocifetchinto(resource stmt, array &output [, int mode])
   Fetch a row of result data into an array */
PHP_FUNCTION(ocifetchinto)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_NUM, 3);
}
/* }}} */

/* {{{ proto int oci_fetch_all(resource stmt, array &output[, int skip[, int maxrows[, int flags]]])
   Fetch all rows of result data into an array */
PHP_FUNCTION(oci_fetch_all)
{
	zval *z_statement, *array, *element, *tmp;
	php_oci_statement *statement;
	php_oci_out_column **columns;
	zval ***outarrs;
	ub4 nrows = 1;
	int i;
	long rows = 0, flags = 0, skip = 0, maxrows = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz/|lll", &z_statement, &array, &skip, &maxrows, &flags) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	zval_dtor(array);
	array_init(array);

	while (skip--) {
		if (php_oci_statement_fetch(statement, nrows TSRMLS_CC)) {
			RETURN_LONG(0);
		}
	}

	if (flags & PHP_OCI_FETCHSTATEMENT_BY_ROW) {
		columns = safe_emalloc(statement->ncolumns, sizeof(php_oci_out_column *), 0);

		for (i = 0; i < statement->ncolumns; i++) {
			columns[ i ] = php_oci_statement_get_column(statement, i + 1, NULL, 0 TSRMLS_CC);
		}

		while (!php_oci_statement_fetch(statement, nrows TSRMLS_CC)) {
			zval *row;
			
			MAKE_STD_ZVAL(row);
			array_init(row);

			for (i = 0; i < statement->ncolumns; i++) {
				MAKE_STD_ZVAL(element);
				php_oci_column_to_zval(columns[ i ], element, PHP_OCI_RETURN_LOBS TSRMLS_CC);

				if (flags & PHP_OCI_NUM) {
					zend_hash_next_index_insert(Z_ARRVAL_P(row), &element, sizeof(zval*), NULL);
				} else { /* default to ASSOC */
					zend_symtable_update(Z_ARRVAL_P(row), columns[ i ]->name, columns[ i ]->name_len+1, &element, sizeof(zval*), NULL);
				}
			}

			zend_hash_next_index_insert(Z_ARRVAL_P(array), &row, sizeof(zval*), NULL);
			rows++;

			if (maxrows != -1 && rows == maxrows) {
				php_oci_statement_cancel(statement TSRMLS_CC);
				break;
			}
		}
		efree(columns);

	} else { /* default to BY_COLUMN */
		columns = safe_emalloc(statement->ncolumns, sizeof(php_oci_out_column *), 0);
		outarrs = safe_emalloc(statement->ncolumns, sizeof(zval*), 0);
		
		if (flags & PHP_OCI_NUM) {
			for (i = 0; i < statement->ncolumns; i++) {
				columns[ i ] = php_oci_statement_get_column(statement, i + 1, NULL, 0 TSRMLS_CC);
				
				MAKE_STD_ZVAL(tmp);
				array_init(tmp);
				zend_hash_next_index_insert(Z_ARRVAL_P(array), &tmp, sizeof(zval*), (void **) &(outarrs[ i ]));
			}
		} else { /* default to ASSOC */
			for (i = 0; i < statement->ncolumns; i++) {
				columns[ i ] = php_oci_statement_get_column(statement, i + 1, NULL, 0 TSRMLS_CC);
				
				MAKE_STD_ZVAL(tmp);
				array_init(tmp);
				zend_symtable_update(Z_ARRVAL_P(array), columns[ i ]->name, columns[ i ]->name_len+1, (void *) &tmp, sizeof(zval*), (void **) &(outarrs[ i ]));
			}
		}

		while (!php_oci_statement_fetch(statement, nrows TSRMLS_CC)) {
			for (i = 0; i < statement->ncolumns; i++) {
				MAKE_STD_ZVAL(element);
				php_oci_column_to_zval(columns[ i ], element, PHP_OCI_RETURN_LOBS TSRMLS_CC);
				zend_hash_index_update((*(outarrs[ i ]))->value.ht, rows, (void *)&element, sizeof(zval*), NULL);
			}

			rows++;

			if (maxrows != -1 && rows == maxrows) {
				php_oci_statement_cancel(statement TSRMLS_CC);
				break;
			}
		}
		
		efree(columns);
		efree(outarrs);
	}

	RETURN_LONG(rows);
}
/* }}} */

/* {{{ proto object oci_fetch_object( resource stmt )
   Fetch a result row as an object */
PHP_FUNCTION(oci_fetch_object)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_ASSOC | PHP_OCI_RETURN_NULLS, 2);

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto array oci_fetch_row( resource stmt )
   Fetch a result row as an enumerated array */
PHP_FUNCTION(oci_fetch_row)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_NUM | PHP_OCI_RETURN_NULLS, 1);
}
/* }}} */

/* {{{ proto array oci_fetch_assoc( resource stmt )
   Fetch a result row as an associative array */
PHP_FUNCTION(oci_fetch_assoc)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_ASSOC | PHP_OCI_RETURN_NULLS, 1);
}
/* }}} */

/* {{{ proto array oci_fetch_array( resource stmt [, int mode ])
   Fetch a result row as an array */
PHP_FUNCTION(oci_fetch_array)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_BOTH | PHP_OCI_RETURN_NULLS, 2);
}
/* }}} */

/* {{{ proto bool oci_free_statement(resource stmt)
   Free all resources associated with a statement */
PHP_FUNCTION(oci_free_statement)
{
	zval *z_statement;
	php_oci_statement *statement;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	zend_list_delete(statement->id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_close(resource connection)
   Disconnect from database */
PHP_FUNCTION(oci_close)
{
	/* oci_close for pconnect (if old_oci_close_semantics not set) would
	 * release the connection back to the client-side session pool (and to the
	 * server-side pool if Database Resident Connection Pool is being used).
	 * Subsequent pconnects in the same script are not guaranteed to get the
	 * same database session.
	 */

	zval *z_connection;
	php_oci_connection *connection;

	if (OCI_G(old_oci_close_semantics)) {
		/* do nothing to keep BC */
		return;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_connection) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);
	zend_list_delete(connection->id);

	ZVAL_NULL(z_connection);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource oci_new_connect(string user, string pass [, string db, string charset [, int session_mode ]])
   Connect to an Oracle database and log on. Returns a new session. */
PHP_FUNCTION(oci_new_connect)
{
	php_oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}
/* }}} */

/* {{{ proto resource oci_connect(string user, string pass [, string db [, string charset [, int session_mode ]])
   Connect to an Oracle database and log on. Returns a new session. */
PHP_FUNCTION(oci_connect)
{
	php_oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto resource oci_pconnect(string user, string pass [, string db [, string charset [, int session_mode ]])
   Connect to an Oracle database using a persistent connection and log on. Returns a new session. */
PHP_FUNCTION(oci_pconnect)
{
	php_oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

/* {{{ proto array oci_error([resource stmt|connection|global])
   Return the last error of stmt|connection|global. If no error happened returns false. */
PHP_FUNCTION(oci_error)
{
	zval *arg = NULL;
	php_oci_statement *statement;
	php_oci_connection *connection;
	text *errbuf;
	sb4 errcode = 0;
	dvoid *errh = NULL;
	ub2 error_offset = 0;
	text *sqltext = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &arg) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 0) {
		statement = (php_oci_statement *) zend_fetch_resource(&arg TSRMLS_CC, -1, NULL, NULL, 1, le_statement);
		if (statement) {
			errh = statement->err;
			errcode = statement->errcode;

			if (php_oci_fetch_sqltext_offset(statement, &sqltext, &error_offset TSRMLS_CC)) {
				RETURN_FALSE;
			}
			goto go_out;
		}

		connection = (php_oci_connection *) zend_fetch_resource(&arg TSRMLS_CC, -1, NULL, NULL, 1, le_connection);
		if (connection) {
			errh = connection->err;
			errcode = connection->errcode;
			goto go_out;
		}

		connection = (php_oci_connection *) zend_fetch_resource(&arg TSRMLS_CC, -1, NULL, NULL, 1, le_pconnection);
		if (connection) {
			errh = connection->err;
			errcode = connection->errcode;
			goto go_out;
		}
	} else {
		errh = OCI_G(err);
		errcode = OCI_G(errcode);
	}

go_out:
	if (errcode == 0) { /* no error set in the handle */
		RETURN_FALSE;
	}

	if (!errh) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Oci_error: unable to find error handle");
		RETURN_FALSE;
	}

	errcode = php_oci_fetch_errmsg(errh, &errbuf TSRMLS_CC);

	if (errcode) {
		array_init(return_value);
		add_assoc_long(return_value, "code", errcode);
		add_assoc_string(return_value, "message", (char*) errbuf, 0);
		add_assoc_long(return_value, "offset", error_offset);
		add_assoc_string(return_value, "sqltext", sqltext ? (char *) sqltext : "", 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int oci_num_fields(resource stmt)
   Return the number of result columns in a statement */
PHP_FUNCTION(oci_num_fields)
{
	zval *z_statement;
	php_oci_statement *statement;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	RETURN_LONG(statement->ncolumns);
}
/* }}} */

/* {{{ proto resource oci_parse(resource connection, string statement)
   Parse a SQL or PL/SQL statement and return a statement resource */
PHP_FUNCTION(oci_parse)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_statement *statement;
	char *query;
	int query_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_connection, &query, &query_len) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	statement = php_oci_statement_create(connection, query, query_len TSRMLS_CC);

	if (statement) {
		RETURN_RESOURCE(statement->id);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_set_prefetch(resource stmt, int prefetch_rows)
  Sets the number of rows to be prefetched on execute to prefetch_rows for stmt */
PHP_FUNCTION(oci_set_prefetch)
{
	zval *z_statement;
	php_oci_statement *statement;
	long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &z_statement, &size) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (size < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of rows to be prefetched has to be greater than or equal to 0");
		return;
	}

	if (php_oci_statement_set_prefetch(statement, (ub4)size TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_set_client_identifier(resource connection, string value)
  Sets the client identifier attribute on the connection */
PHP_FUNCTION(oci_set_client_identifier)
{
	zval *z_connection;
	php_oci_connection *connection;
	char *client_id;
	int client_id_len;
	sword errstatus;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_connection, &client_id, &client_id_len) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) client_id, (ub4) client_id_len, (ub4) OCI_ATTR_CLIENT_IDENTIFIER, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus TSRMLS_CC);
		RETURN_FALSE;
	}

#ifdef HAVE_OCI8_DTRACE
	/* The alternatives to storing client_id like done below are
	   i) display it in a probe here in oci_set_client_identifier and
	   let the user D script correlate the connection address probe
	   argument and the client_id. This would likely require user D
	   script variables, which would use kernel memory.
	   ii) call OCIAttrGet for each probe definition that uses
	   client_id. This would be slower than storing it.
	*/

	if (connection->client_id) {
		pefree(connection->client_id, connection->is_persistent);
	}

	if (client_id) {
		/* this long winded copy allows compatibility with older PHP versions */
		connection->client_id = (char *)pemalloc(client_id_len+1, connection->is_persistent);
		memcpy(connection->client_id, client_id, client_id_len);
		connection->client_id[client_id_len] = '\0';
	} else {
		connection->client_id = NULL;
	}
#endif /* HAVE_OCI8_DTRACE */

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_set_edition(string value)
  Sets the edition attribute for all subsequent connections created */
PHP_FUNCTION(oci_set_edition)
{
#if ((OCI_MAJOR_VERSION > 11) || ((OCI_MAJOR_VERSION == 11) && (OCI_MINOR_VERSION >= 2)))
	char *edition;
	int edition_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &edition, &edition_len) == FAILURE) {
		return;
	}

	if (OCI_G(edition)) {
		efree(OCI_G(edition));
	}

	if (edition) {
		OCI_G(edition) = (char *)safe_emalloc(edition_len+1, sizeof(char), 0);
		memcpy(OCI_G(edition), edition, edition_len);
		OCI_G(edition)[edition_len] = '\0';
	} else {
		OCI_G(edition) = NULL;
	}

	RETURN_TRUE;
#else
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unsupported attribute type");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool oci_set_module_name(resource connection, string value)
  Sets the module attribute on the connection for end-to-end tracing */
PHP_FUNCTION(oci_set_module_name)
{
#if (OCI_MAJOR_VERSION >= 10)
	zval *z_connection;
	php_oci_connection *connection;
	char *module;
	int module_len;
	sword errstatus;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_connection, &module, &module_len) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) module, (ub4) module_len, (ub4) OCI_ATTR_MODULE, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus TSRMLS_CC);
		RETURN_FALSE;
	}

	RETURN_TRUE;
#else
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unsupported attribute type");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool oci_set_action(resource connection, string value)
  Sets the action attribute on the connection for end-to-end tracing */
PHP_FUNCTION(oci_set_action)
{
#if (OCI_MAJOR_VERSION >= 10)
	zval *z_connection;
	php_oci_connection *connection;
	char *action;
	int action_len;
	sword errstatus;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_connection, &action, &action_len) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) action, (ub4) action_len, (ub4) OCI_ATTR_ACTION, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus TSRMLS_CC);
		RETURN_FALSE;
	}

	RETURN_TRUE;
#else
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unsupported attribute type");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool oci_set_client_info(resource connection, string value)
  Sets the client info attribute on the connection  for end-to-end tracing */
PHP_FUNCTION(oci_set_client_info)
{
#if (OCI_MAJOR_VERSION >= 10)
	zval *z_connection;
	php_oci_connection *connection;
	char *client_info;
	int client_info_len;
	sword errstatus;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_connection, &client_info, &client_info_len) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) client_info, (ub4) client_info_len, (ub4) OCI_ATTR_CLIENT_INFO, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus TSRMLS_CC);
		RETURN_FALSE;
	}

	RETURN_TRUE;
#else
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unsupported attribute type");
	RETURN_FALSE;
#endif
}
/* }}} */

#ifdef WAITIING_ORACLE_BUG_16695981_FIX
/* {{{ proto bool oci_set_db_operation(resource connection, string value)
   Sets the "DB operation" on the connection for Oracle end-to-end tracing */
PHP_FUNCTION(oci_set_db_operation)
{
#if (OCI_MAJOR_VERSION > 11)
	zval *z_connection;
	php_oci_connection *connection;
	char *dbop_name;
	int dbop_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_connection, &dbop_name, &dbop_name_len) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) dbop_name, (ub4) dbop_name_len, (ub4) OCI_ATTR_DBOP, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		RETURN_FALSE;
	}
	RETURN_TRUE;
#else
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unsupported attribute type");
	RETURN_FALSE;
#endif
}
/* }}} */
#endif /* WAITIING_ORACLE_BUG_16695981_FIX */

/* {{{ proto bool oci_password_change(resource connection, string username, string old_password, string new_password)
  Changes the password of an account */
PHP_FUNCTION(oci_password_change)
{
	zval *z_connection;
	char *user, *pass_old, *pass_new, *dbname;
	int user_len, pass_old_len, pass_new_len, dbname_len;
	php_oci_connection *connection;

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4) || (PHP_MAJOR_VERSION < 5)
	/* Safe mode has been removed in PHP 5.4 */
	if (PG(safe_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "is disabled in Safe Mode");
		RETURN_FALSE;
	}
#endif

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &z_connection, &user, &user_len, &pass_old, &pass_old_len, &pass_new, &pass_new_len) == SUCCESS) {
		PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

		if (!user_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "username cannot be empty");
			RETURN_FALSE;
		}
		if (!pass_old_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "old password cannot be empty");
			RETURN_FALSE;
		}
		if (!pass_new_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "new password cannot be empty");
			RETURN_FALSE;
		}

		if (php_oci_password_change(connection, user, user_len, pass_old, pass_old_len, pass_new, pass_new_len TSRMLS_CC)) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "ssss", &dbname, &dbname_len, &user, &user_len, &pass_old, &pass_old_len, &pass_new, &pass_new_len) == SUCCESS) {

		if (!user_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "username cannot be empty");
			RETURN_FALSE;
		}
		if (!pass_old_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "old password cannot be empty");
			RETURN_FALSE;
		}
		if (!pass_new_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "new password cannot be empty");
			RETURN_FALSE;
		}

		connection = php_oci_do_connect_ex(user, user_len, pass_old, pass_old_len, pass_new, pass_new_len, dbname, dbname_len, NULL, OCI_DEFAULT, 0, 0 TSRMLS_CC);
		if (!connection) {
			RETURN_FALSE;
		}
		RETURN_RESOURCE(connection->id);
	}
	WRONG_PARAM_COUNT;
}
/* }}} */

/* {{{ proto resource oci_new_cursor(resource connection)
   Return a new cursor (Statement-Handle) - use this to bind ref-cursors! */
PHP_FUNCTION(oci_new_cursor)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_statement *statement;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_connection) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	statement = php_oci_statement_create(connection, NULL, 0 TSRMLS_CC);
	
	if (statement) {
		RETURN_RESOURCE(statement->id);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string oci_result(resource stmt, mixed column)
   Return a single column of result data */
PHP_FUNCTION(oci_result)
{
	php_oci_out_column *column;
	
	column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
	if(column) {
		php_oci_column_to_zval(column, return_value, 0 TSRMLS_CC);
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string oci_client_version()
   Return a string containing runtime client library version information */
PHP_FUNCTION(oci_client_version)
{
	char *version = NULL;

	php_oci_client_get_version(&version TSRMLS_CC);
	RETURN_STRING(version, 0);
}
/* }}} */

/* {{{ proto string oci_server_version(resource connection)
   Return a string containing server version information */
PHP_FUNCTION(oci_server_version)
{
	zval *z_connection;
	php_oci_connection *connection;
	char *version = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_connection) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (php_oci_server_get_version(connection, &version TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	RETURN_STRING(version, 0);
}
/* }}} */

/* {{{ proto string oci_statement_type(resource stmt)
   Return the query type of an OCI statement */
PHP_FUNCTION(oci_statement_type)
{
	zval *z_statement;
	php_oci_statement *statement;
	ub2 type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_get_type(statement, &type TSRMLS_CC)) {
		RETURN_FALSE;
	}

	switch (type) {
		case OCI_STMT_SELECT:
			RETVAL_STRING("SELECT",1);
			break;
		case OCI_STMT_UPDATE:
			RETVAL_STRING("UPDATE",1);
			break;
		case OCI_STMT_DELETE:
			RETVAL_STRING("DELETE",1);
			break;
		case OCI_STMT_INSERT:
			RETVAL_STRING("INSERT",1);
			break;
		case OCI_STMT_CREATE:
			RETVAL_STRING("CREATE",1);
			break;
		case OCI_STMT_DROP:
			RETVAL_STRING("DROP",1);
			break;
		case OCI_STMT_ALTER:
			RETVAL_STRING("ALTER",1);
			break;
		case OCI_STMT_BEGIN:
			RETVAL_STRING("BEGIN",1);
			break;
		case OCI_STMT_DECLARE:
			RETVAL_STRING("DECLARE",1);
			break;
		case OCI_STMT_CALL:
			RETVAL_STRING("CALL",1);
			break;
		default:
			RETVAL_STRING("UNKNOWN",1);
	}
}
/* }}} */

/* {{{ proto int oci_num_rows(resource stmt)
   Return the row count of an OCI statement */
PHP_FUNCTION(oci_num_rows)
{
	zval *z_statement;
	php_oci_statement *statement;
	ub4 rowcount;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_get_numrows(statement, &rowcount TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_LONG(rowcount);
}
/* }}} */

/* {{{ proto bool oci_free_collection()
   Deletes collection object*/
PHP_FUNCTION(oci_free_collection)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;

	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_collection, oci_coll_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	zend_list_delete(collection->id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_collection_append(string value)
   Append an object to the collection */
PHP_FUNCTION(oci_collection_append)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;
	char *value;
	int value_len;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &z_collection, oci_coll_class_entry_ptr, &value, &value_len) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	if (php_oci_collection_append(collection, value, value_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string oci_collection_element_get(int ndx)
   Retrieve the value at collection index ndx */
PHP_FUNCTION(oci_collection_element_get)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;
	long element_index;
	zval *value;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &element_index) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ol", &z_collection, oci_coll_class_entry_ptr, &element_index) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	if (php_oci_collection_element_get(collection, element_index, &value TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	*return_value = *value;
	zval_copy_ctor(return_value);
	zval_ptr_dtor(&value);
}
/* }}} */

/* {{{ proto bool oci_collection_assign(object from)
   Assign a collection from another existing collection */
PHP_FUNCTION(oci_collection_assign)
{
	zval **tmp_dest, **tmp_from, *z_collection_dest = getThis(), *z_collection_from;
	php_oci_collection *collection_dest, *collection_from;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_collection_from, oci_coll_class_entry_ptr) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO", &z_collection_dest, oci_coll_class_entry_ptr, &z_collection_from, oci_coll_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection_dest), "collection", sizeof("collection"), (void **)&tmp_dest) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property. The first argument should be valid collection object");
		RETURN_FALSE;
	}

	if (zend_hash_find(Z_OBJPROP_P(z_collection_from), "collection", sizeof("collection"), (void **)&tmp_from) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property. The second argument should be valid collection object");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(*tmp_dest, collection_dest);
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp_from, collection_from);

	if (php_oci_collection_assign(collection_dest, collection_from TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_collection_element_assign(int index, string val)
   Assign element val to collection at index ndx */
PHP_FUNCTION(oci_collection_element_assign)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;
	int value_len;
	long element_index;
	char *value;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &element_index, &value, &value_len) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ols", &z_collection, oci_coll_class_entry_ptr, &element_index, &value, &value_len) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	if (php_oci_collection_element_set(collection, element_index, value, value_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int oci_collection_size()
   Return the size of a collection */
PHP_FUNCTION(oci_collection_size)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;
	sb4 size = 0;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_collection, oci_coll_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	if (php_oci_collection_size(collection, &size TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto int oci_collection_max()
   Return the max value of a collection. For a varray this is the maximum length of the array */
PHP_FUNCTION(oci_collection_max)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;
	long max;
	
	if (!getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_collection, oci_coll_class_entry_ptr) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	if (php_oci_collection_max(collection, &max TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_LONG(max);
}
/* }}} */

/* {{{ proto bool oci_collection_trim(int num)
   Trim num elements from the end of a collection */
PHP_FUNCTION(oci_collection_trim)
{
	zval **tmp, *z_collection = getThis();
	php_oci_collection *collection;
	long trim_size;

	if (getThis()) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &trim_size) == FAILURE) {
			return;
		}
	}
	else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ol", &z_collection, oci_coll_class_entry_ptr, &trim_size) == FAILURE) {
			return;
		}	
	}
	
	if (zend_hash_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection"), (void **)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}
	
	PHP_OCI_ZVAL_TO_COLLECTION(*tmp, collection);

	if (php_oci_collection_trim(collection, trim_size TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;	
}
/* }}} */

/* {{{ proto object oci_new_collection(resource connection, string tdo [, string schema])
   Initialize a new collection */
PHP_FUNCTION(oci_new_collection)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_collection *collection;
	char *tdo, *schema = NULL;
	int tdo_len, schema_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s", &z_connection, &tdo, &tdo_len, &schema, &schema_len) == FAILURE) {
		return;
	}
	
	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);
	
	if ( (collection = php_oci_collection_create(connection, tdo, tdo_len, schema, schema_len TSRMLS_CC)) ) {
		object_init_ex(return_value, oci_coll_class_entry_ptr);
		add_property_resource(return_value, "collection", collection->id);
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool oci_get_implicit(resource stmt)
   Get the next statement resource from an Oracle 12c PL/SQL Implicit Result Set */
PHP_FUNCTION(oci_get_implicit_resultset)
{
	zval *z_statement;
	php_oci_statement *statement;
	php_oci_statement *imp_statement;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
		return;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	imp_statement = php_oci_get_implicit_resultset(statement TSRMLS_CC);

	if (imp_statement) {
		if (php_oci_statement_execute(imp_statement, (ub4)OCI_DEFAULT TSRMLS_CC))
			RETURN_FALSE;
		RETURN_RESOURCE(imp_statement->id);
	}
	RETURN_FALSE;
}

/* }}} */

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
