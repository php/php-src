/*
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
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@php.net>                           |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#ifdef HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"

#ifndef OCI_STMT_CALL
#define OCI_STMT_CALL 10
#endif

#define ERROR_ARG_POS(arg_num) (getThis() ? (arg_num-1) : (arg_num))

/* {{{ Register a callback function for Oracle Transparent Application Failover (TAF) */
PHP_FUNCTION(oci_register_taf_callback)
{
	zval *z_connection;
	php_oci_connection *connection;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *callback = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|f!", &z_connection, &fci, &fcc) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	/* If callable passed, assign callback zval so that it can be passed to php_oci_register_taf_callback() */
	if (ZEND_FCI_INITIALIZED(fci)) {
		callback = &fci.function_name;
	}

	if (php_oci_register_taf_callback(connection, callback) == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{    Unregister a callback function for Oracle Transparent Application Failover (TAF) */
PHP_FUNCTION(oci_unregister_taf_callback)
{
	zval *z_connection;
	php_oci_connection *connection;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_connection) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (php_oci_unregister_taf_callback(connection) == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Define a PHP variable to an Oracle column by name */
/* if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!! */
PHP_FUNCTION(oci_define_by_name)
{
	zval *stmt, *var;
	char *name;
	size_t name_len;
	zend_long type = 0;
	php_oci_statement *statement;
	php_oci_define *define;
	zend_string *zvtmp;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_RESOURCE(stmt)
		Z_PARAM_STRING(name, name_len)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(type)
	ZEND_PARSE_PARAMETERS_END();

	if (!name_len) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	PHP_OCI_ZVAL_TO_STATEMENT(stmt, statement);

	if (statement->defines == NULL) {
		ALLOC_HASHTABLE(statement->defines);
		zend_hash_init(statement->defines, 13, NULL, php_oci_define_hash_dtor, 0);
	}
	else if (zend_hash_str_exists(statement->defines, (const char *)name, name_len)) {
		RETURN_FALSE;
	}

	define = ecalloc(1,sizeof(php_oci_define));

	/* if (zend_hash_add(statement->defines, name, name_len, define, sizeof(php_oci_define), (void **)&tmp_define) == SUCCESS) { */
	zvtmp = zend_string_init(name, name_len, 0);
	if ((define = zend_hash_add_new_ptr(statement->defines, zvtmp, define)) != NULL) {
		zend_string_release_ex(zvtmp, 0);
	} else {
		efree(define);
		zend_string_release_ex(zvtmp, 0);
		RETURN_FALSE;
	}

	define->name = (text*) ecalloc(1, name_len+1);
	memcpy(define->name, name, name_len);
	define->name[name_len] = '\0';
	define->name_len = (ub4) name_len;
	define->type = (ub4) type;
	ZEND_ASSERT(Z_ISREF_P(var));
	ZVAL_COPY(&define->val, var);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Bind a PHP variable to an Oracle placeholder by name */
/* if you want to bind a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE binding!!! */
PHP_FUNCTION(oci_bind_by_name)
{
	ub2	bind_type = SQLT_CHR; /* unterminated string */
	size_t name_len;
	zend_long maxlen = -1, type = 0;
	char *name;
	zval *z_statement;
	zval *bind_var = NULL;
	php_oci_statement *statement;

	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_RESOURCE(z_statement)
		Z_PARAM_STRING(name, name_len)
		Z_PARAM_ZVAL(bind_var)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(maxlen)
		Z_PARAM_LONG(type)
	ZEND_PARSE_PARAMETERS_END();

	if (type) {
		bind_type = (ub2) type;
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_bind_by_name(statement, name, name_len, bind_var, maxlen, bind_type)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Bind a PHP array to an Oracle PL/SQL type by name */
PHP_FUNCTION(oci_bind_array_by_name)
{
	size_t name_len;
	zend_long max_item_len = -1;
	zend_long max_array_len = 0;
	zend_long type = SQLT_AFC;
	char *name;
	zval *z_statement;
	zval *bind_var = NULL;
	php_oci_statement *statement;

	ZEND_PARSE_PARAMETERS_START(4, 6)
		Z_PARAM_RESOURCE(z_statement)
		Z_PARAM_STRING(name, name_len)
		Z_PARAM_ZVAL(bind_var)
		Z_PARAM_LONG(max_array_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(max_item_len)
		Z_PARAM_LONG(type)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (ZEND_NUM_ARGS() == 5 && max_item_len <= 0) {
		max_item_len = -1;
	}

	if (max_array_len <= 0) {
		zend_argument_value_error(4, "must be greater than 0");
		RETURN_THROWS();
	}

	if (php_oci_bind_array_by_name(statement, name, (sb4) name_len, bind_var, max_array_len, max_item_len, type)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Deletes large object description */
PHP_FUNCTION(oci_free_descriptor)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	zend_list_close(descriptor->id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Saves a large object */
PHP_FUNCTION(oci_lob_save)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	char *data;
	size_t data_len;
	zend_long offset = 0;
	ub4 bytes_written;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l", &z_descriptor, oci_lob_class_entry_ptr, &data, &data_len, &offset) == FAILURE) {
		RETURN_THROWS();
	}

	if (offset < 0) {
		zend_argument_value_error(ERROR_ARG_POS(3), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_write(descriptor, (ub4) offset, data, (ub4) data_len, &bytes_written)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Loads file into a LOB */
PHP_FUNCTION(oci_lob_import)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	char *filename;
	size_t filename_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Op", &z_descriptor, oci_lob_class_entry_ptr, &filename, &filename_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_import(descriptor, filename)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Loads a large object */
PHP_FUNCTION(oci_lob_load)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	char *buffer = NULL;
	ub4 buffer_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_read(descriptor, -1, 0, &buffer, &buffer_len)) {
		RETURN_FALSE;
	}
	if (buffer_len > 0) {
		zend_string *ret = zend_string_init(buffer, buffer_len, 0);
		if (buffer)
			efree(buffer);
		RETURN_STR(ret);
	}
	else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Reads particular part of a large object */
PHP_FUNCTION(oci_lob_read)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	zend_long length;
	char *buffer;
	ub4 buffer_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &z_descriptor, oci_lob_class_entry_ptr, &length) == FAILURE) {
		RETURN_THROWS();
	}

	if (length <= 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than 0");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_read(descriptor, length, descriptor->lob_current_position, &buffer, &buffer_len)) {
		RETURN_FALSE;
	}
	if (buffer_len > 0) {
		zend_string *ret = zend_string_init(buffer, buffer_len, 0);
		efree(buffer);
		RETURN_STR(ret);
	}
	else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Checks if EOF is reached */
PHP_FUNCTION(oci_lob_eof)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	ub4 lob_length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (!php_oci_lob_get_length(descriptor, &lob_length)) {
		if (lob_length == descriptor->lob_current_position) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Tells LOB pointer position */
PHP_FUNCTION(oci_lob_tell)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	RETURN_LONG(descriptor->lob_current_position);
}
/* }}} */

/* {{{ Rewind pointer of a LOB */
PHP_FUNCTION(oci_lob_rewind)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	descriptor->lob_current_position = 0;

	RETURN_TRUE;
}
/* }}} */

/* {{{ Moves the pointer of a LOB */
PHP_FUNCTION(oci_lob_seek)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	zend_long offset, whence = PHP_OCI_SEEK_SET;
	ub4 lob_length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol|l", &z_descriptor, oci_lob_class_entry_ptr, &offset, &whence) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_get_length(descriptor, &lob_length)) {
		RETURN_FALSE;
	}

	switch(whence) {
		case PHP_OCI_SEEK_CUR:
			descriptor->lob_current_position += (ub4) offset;
			break;
		case PHP_OCI_SEEK_END:
			if ((descriptor->lob_size + offset) >= 0) {
				descriptor->lob_current_position = descriptor->lob_size + (ub4) offset;
			}
			else {
				descriptor->lob_current_position = 0;
			}
			break;
		case PHP_OCI_SEEK_SET:
		default:
				descriptor->lob_current_position = (offset > 0) ? (ub4) offset : 0;
			break;
	}
	if (descriptor->lob_current_position > UB4MAXVAL) {
		php_error_docref(NULL, E_WARNING, "Invalid offset or LOB position");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns size of a large object */
PHP_FUNCTION(oci_lob_size)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	ub4 lob_length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_get_length(descriptor, &lob_length)) {
		RETURN_FALSE;
	}
	RETURN_LONG(lob_length);
}
/* }}} */

/* {{{ Writes data to current position of a LOB */
PHP_FUNCTION(oci_lob_write)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	size_t data_len;
	zend_long write_len;
	bool write_len_is_null = 1;
	ub4 bytes_written;
	char *data;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l!", &z_descriptor, oci_lob_class_entry_ptr, &data, &data_len, &write_len, &write_len_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (!write_len_is_null) {
		data_len = MIN((zend_long) data_len, write_len);
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (data_len <= 0) {
		RETURN_LONG(0);
	}

	if (php_oci_lob_write(descriptor, descriptor->lob_current_position, data, (ub4) data_len, &bytes_written)) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes_written);
}
/* }}} */

/* {{{ Appends data from a LOB to another LOB */
PHP_FUNCTION(oci_lob_append)
{
	zval *tmp_dest, *tmp_from, *z_descriptor_dest, *z_descriptor_from;
	php_oci_descriptor *descriptor_dest, *descriptor_from;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO", &z_descriptor_dest, oci_lob_class_entry_ptr, &z_descriptor_from, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp_dest = zend_hash_str_find(Z_OBJPROP_P(z_descriptor_dest), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property. The first argument should be valid descriptor object");
		RETURN_FALSE;
	}

	if ((tmp_from = zend_hash_str_find(Z_OBJPROP_P(z_descriptor_from), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property. The second argument should be valid descriptor object");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp_dest, descriptor_dest);
	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp_from, descriptor_from);

	if (php_oci_lob_append(descriptor_dest, descriptor_from)) {
		RETURN_FALSE;
	}
	/* XXX should we increase lob_size here ? */
	RETURN_TRUE;
}
/* }}} */

/* {{{ Truncates a LOB */
PHP_FUNCTION(oci_lob_truncate)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	zend_long trim_length = 0;
	ub4 ub_trim_length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &z_descriptor, oci_lob_class_entry_ptr, &trim_length) == FAILURE) {
		RETURN_THROWS();
	}

	if (trim_length < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	ub_trim_length = (ub4) trim_length;
	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_truncate(descriptor, ub_trim_length)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Erases a specified portion of the internal LOB, starting at a specified offset */
PHP_FUNCTION(oci_lob_erase)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	ub4 bytes_erased;
	zend_long offset, length;
	bool offset_is_null = 1, length_is_null = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l!l!", &z_descriptor, oci_lob_class_entry_ptr, &offset, &offset_is_null, &length, &length_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (offset_is_null) {
		offset = -1;
	} else if (offset < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (length_is_null) {
		length = -1;
	} else if (length < 0) {
		zend_argument_value_error(ERROR_ARG_POS(3), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_erase(descriptor, offset, (ub4) length, &bytes_erased)) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes_erased);
}
/* }}} */

/* {{{ Flushes the LOB buffer */
PHP_FUNCTION(oci_lob_flush)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	zend_long flush_flag = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &z_descriptor, oci_lob_class_entry_ptr, &flush_flag) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (descriptor->buffering == PHP_OCI_LOB_BUFFER_DISABLED) {
		/* buffering wasn't enabled, there is nothing to flush */
		RETURN_FALSE;
	}

	if (php_oci_lob_flush(descriptor, flush_flag)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Enables/disables buffering for a LOB */
PHP_FUNCTION(ocisetbufferinglob)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	bool flag;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ob", &z_descriptor, oci_lob_class_entry_ptr, &flag) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_set_buffering(descriptor, flag)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns current state of buffering for a LOB */
PHP_FUNCTION(ocigetbufferinglob)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (descriptor->buffering != PHP_OCI_LOB_BUFFER_DISABLED) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Copies data from a LOB to another LOB */
PHP_FUNCTION(oci_lob_copy)
{
	zval *tmp_dest, *tmp_from, *z_descriptor_dest, *z_descriptor_from;
	php_oci_descriptor *descriptor_dest, *descriptor_from;
	zend_long length;
	bool length_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO|l!", &z_descriptor_dest, oci_lob_class_entry_ptr, &z_descriptor_from, oci_lob_class_entry_ptr, &length, &length_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (length_is_null) {
		length = -1;
	} else if (length < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if ((tmp_dest = zend_hash_str_find(Z_OBJPROP_P(z_descriptor_dest), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property. The first argument should be valid descriptor object");
		RETURN_FALSE;
	}

	if ((tmp_from = zend_hash_str_find(Z_OBJPROP_P(z_descriptor_from), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property. The second argument should be valid descriptor object");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp_dest, descriptor_dest);
	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp_from, descriptor_from);

	if (php_oci_lob_copy(descriptor_dest, descriptor_from, length)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Tests to see if two LOB/FILE locators are equal */
PHP_FUNCTION(oci_lob_is_equal)
{
	zval *tmp_first, *tmp_second, *z_descriptor_first, *z_descriptor_second;
	php_oci_descriptor *descriptor_first, *descriptor_second;
	boolean is_equal;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &z_descriptor_first, oci_lob_class_entry_ptr, &z_descriptor_second, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp_first = zend_hash_str_find(Z_OBJPROP_P(z_descriptor_first), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property. The first argument should be valid descriptor object");
		RETURN_FALSE;
	}

	if ((tmp_second = zend_hash_str_find(Z_OBJPROP_P(z_descriptor_second), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property. The second argument should be valid descriptor object");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp_first, descriptor_first);
	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp_second, descriptor_second);

	if (php_oci_lob_is_equal(descriptor_first, descriptor_second, &is_equal)) {
		RETURN_FALSE;
	}

	if (is_equal == TRUE) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Writes a large object into a file */
PHP_FUNCTION(oci_lob_export)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	char *filename;
	char *buffer;
	size_t filename_len;
	zend_long start, length, block_length;
	bool start_is_null = 1, length_is_null = 1;
	php_stream *stream;
	ub4 lob_length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Op|l!l!", &z_descriptor, oci_lob_class_entry_ptr, &filename, &filename_len, &start, &start_is_null, &length, &length_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (start_is_null) {
		start = -1;
	} else if (start < 0) {
		zend_argument_value_error(ERROR_ARG_POS(3), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (length_is_null) {
		length = -1;
	} else if (length < 0) {
		zend_argument_value_error(ERROR_ARG_POS(4), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_get_length(descriptor, &lob_length)) {
		RETURN_FALSE;
	}

	if (start == -1) {
		start = 0;
	}

	if (length == -1) {
		length = lob_length - descriptor->lob_current_position;
	}

	if (lob_length == 0) {
		length = 0;
	}

	if (length == 0) {
		/* nothing to write, fail silently */
		RETURN_FALSE;
	}

	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	stream = php_stream_open_wrapper_ex(filename, "w", REPORT_ERRORS, NULL, NULL);

	block_length = PHP_OCI_LOB_BUFFER_SIZE;
	if (block_length > length) {
		block_length = length;
	}

	while(length > 0) {
		ub4 tmp_bytes_read = 0;
		if (php_oci_lob_read(descriptor, block_length, start, &buffer, &tmp_bytes_read)) {
			php_stream_close(stream);
			RETURN_FALSE;
		}
		if (tmp_bytes_read && !php_stream_write(stream, buffer, tmp_bytes_read)) {
			php_stream_close(stream);
			if (buffer)
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

/* {{{ Writes temporary blob */
PHP_METHOD(OCILob, writeTemporary)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;
	char *data;
	size_t data_len;
	zend_long type = OCI_TEMP_CLOB;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l", &z_descriptor, oci_lob_class_entry_ptr, &data, &data_len, &type) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_write_tmp(descriptor, type, data, (int) data_len)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Closes lob descriptor */
PHP_METHOD(OCILob, close)
{
	zval *tmp, *z_descriptor;
	php_oci_descriptor *descriptor;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_descriptor, oci_lob_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_descriptor), "descriptor", sizeof("descriptor")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_DESCRIPTOR(tmp, descriptor);

	if (php_oci_lob_close(descriptor)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Initialize a new empty descriptor LOB/FILE (LOB is default) */
PHP_FUNCTION(oci_new_descriptor)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_descriptor *descriptor;
	zend_long type = OCI_DTYPE_LOB;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &z_connection, &type) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	/* php_oci_lob_create() checks type */
	descriptor = php_oci_lob_create(connection, type);

	if (!descriptor) {
		RETURN_NULL();
	}

	object_init_ex(return_value, oci_lob_class_entry_ptr);
	add_property_resource(return_value, "descriptor", descriptor->id);
}
/* }}} */

/* {{{ Rollback the current context */
PHP_FUNCTION(oci_rollback)
{
	zval *z_connection;
	php_oci_connection *connection;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_connection)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (connection->descriptors) {
		php_oci_connection_descriptors_free(connection);
	}

	if (php_oci_connection_rollback(connection)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Commit the current context */
PHP_FUNCTION(oci_commit)
{
	zval *z_connection;
	php_oci_connection *connection;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_connection)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (connection->descriptors) {
		php_oci_connection_descriptors_free(connection);
	}

	if (php_oci_connection_commit(connection)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Tell the name of a column */
PHP_FUNCTION(oci_field_name)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		RETURN_STRINGL(column->name, column->name_len);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Tell the maximum data size of a column */
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

/* {{{ Tell the scale of a column */
PHP_FUNCTION(oci_field_scale)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		RETURN_LONG(column->scale);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Tell the precision of a column */
PHP_FUNCTION(oci_field_precision)
{
	php_oci_out_column *column;

	if ( ( column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) ) ) {
		RETURN_LONG(column->precision);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Tell the data type of a column */
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
			RETVAL_STRING("TIMESTAMP");
			break;
#endif
#ifdef SQLT_TIMESTAMP_TZ
		case SQLT_TIMESTAMP_TZ:
			RETVAL_STRING("TIMESTAMP WITH TIMEZONE");
			break;
#endif
#ifdef SQLT_TIMESTAMP_LTZ
		case SQLT_TIMESTAMP_LTZ:
			RETVAL_STRING("TIMESTAMP WITH LOCAL TIMEZONE");
			break;
#endif
#ifdef SQLT_INTERVAL_YM
		case SQLT_INTERVAL_YM:
			RETVAL_STRING("INTERVAL YEAR TO MONTH");
			break;
#endif
#ifdef SQLT_INTERVAL_DS
		case SQLT_INTERVAL_DS:
			RETVAL_STRING("INTERVAL DAY TO SECOND");
			break;
#endif
		case SQLT_DAT:
			RETVAL_STRING("DATE");
			break;
		case SQLT_NUM:
			RETVAL_STRING("NUMBER");
			break;
		case SQLT_LNG:
			RETVAL_STRING("LONG");
			break;
		case SQLT_BIN:
			RETVAL_STRING("RAW");
			break;
		case SQLT_LBI:
			RETVAL_STRING("LONG RAW");
			break;
		case SQLT_CHR:
			RETVAL_STRING("VARCHAR2");
			break;
		case SQLT_RSET:
			RETVAL_STRING("REFCURSOR");
			break;
		case SQLT_AFC:
			RETVAL_STRING("CHAR");
			break;
		case SQLT_BLOB:
			RETVAL_STRING("BLOB");
			break;
		case SQLT_CLOB:
			RETVAL_STRING("CLOB");
			break;
		case SQLT_BFILE:
			RETVAL_STRING("BFILE");
			break;
		case SQLT_RDD:
			RETVAL_STRING("ROWID");
			break;
		default:
			RETVAL_LONG(column->data_type);
	}
}
/* }}} */

/* {{{ Tell the raw oracle data type of a column */
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

/* {{{ Tell whether a field in the current row is NULL */
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

/* {{{ Execute a parsed statement */
PHP_FUNCTION(oci_execute)
{
	zval *z_statement;
	php_oci_statement *statement;
	zend_long mode = OCI_COMMIT_ON_SUCCESS;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(z_statement)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_execute(statement, (ub4) mode)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Cancel reading from a cursor */
PHP_FUNCTION(oci_cancel)
{
	zval *z_statement;
	php_oci_statement *statement;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_statement) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_cancel(statement)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Prepare a new row of data for reading */
PHP_FUNCTION(oci_fetch)
{
	zval *z_statement;
	php_oci_statement *statement;
	ub4 nrows = 1; /* only one row at a time is supported for now */

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_statement)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_fetch(statement, nrows)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Fetch a row of result data into an array */
PHP_FUNCTION(ocifetchinto)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_NUM, 3);
}
/* }}} */

/* {{{ Fetch all rows of result data into an array */
PHP_FUNCTION(oci_fetch_all)
{
	zval *z_statement, *array;
	zval element, tmp;
	php_oci_statement *statement;
	php_oci_out_column **columns;
	zval **outarrs;
	ub4 nrows = 1;
	int i;
	zend_long rows = 0, flags = PHP_OCI_FETCHSTATEMENT_BY_COLUMN | PHP_OCI_ASSOC, skip = 0, maxrows = -1;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_RESOURCE(z_statement)
		Z_PARAM_ZVAL(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(skip)
		Z_PARAM_LONG(maxrows)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	while (skip--) {
		if (php_oci_statement_fetch(statement, nrows)) {
			zend_try_array_init(array);
			RETURN_LONG(0);
		}
	}

	if (flags & PHP_OCI_FETCHSTATEMENT_BY_ROW) {
		/* Fetch by Row: array will contain one sub-array per query row */
		array = zend_try_array_init(array);
		if (!array) {
			RETURN_THROWS();
		}

		columns = safe_emalloc(statement->ncolumns, sizeof(php_oci_out_column *), 0);
		for (i = 0; i < statement->ncolumns; i++) {
			columns[ i ] = php_oci_statement_get_column(statement, i + 1, NULL, 0);
		}

		while (!php_oci_statement_fetch(statement, nrows)) {
			zval row;

			array_init_size(&row, statement->ncolumns);

			for (i = 0; i < statement->ncolumns; i++) {
				php_oci_column_to_zval(columns[ i ], &element, PHP_OCI_RETURN_LOBS);

				if (flags & PHP_OCI_NUM) {
					zend_hash_next_index_insert(Z_ARRVAL(row), &element);
				} else { /* default to ASSOC */
					zend_string *zvtmp;
					zvtmp = zend_string_init(columns[ i ]->name, columns[ i ]->name_len, 0);
					zend_symtable_update(Z_ARRVAL(row), zvtmp, &element);
					zend_string_release_ex(zvtmp, 0);
				}
			}

			zend_hash_next_index_insert(Z_ARRVAL_P(array), &row);
			rows++;

			if (maxrows != -1 && rows == maxrows) {
				php_oci_statement_cancel(statement);
				break;
			}
		}
		efree(columns);

	} else { /* default to BY_COLUMN */
		/* Fetch by columns: array will contain one sub-array per query column */
		array = zend_try_array_init_size(array, statement->ncolumns);
		if (!array) {
			RETURN_THROWS();
		}

		columns = safe_emalloc(statement->ncolumns, sizeof(php_oci_out_column *), 0);
		outarrs = safe_emalloc(statement->ncolumns, sizeof(zval*), 0);

		if (flags & PHP_OCI_NUM) {
			for (i = 0; i < statement->ncolumns; i++) {
				columns[ i ] = php_oci_statement_get_column(statement, i + 1, NULL, 0);

				array_init(&tmp);
				outarrs[ i ] = zend_hash_next_index_insert(Z_ARRVAL_P(array), &tmp);
			}
		} else { /* default to ASSOC */
			for (i = 0; i < statement->ncolumns; i++) {
				zend_string *zvtmp;
				columns[ i ] = php_oci_statement_get_column(statement, i + 1, NULL, 0);

				array_init(&tmp);
				zvtmp = zend_string_init(columns[ i ]->name, columns[ i ]->name_len, 0);
				outarrs[ i ] = zend_symtable_update(Z_ARRVAL_P(array), zvtmp, &tmp);
				zend_string_release_ex(zvtmp, 0);
			}
		}

		while (!php_oci_statement_fetch(statement, nrows)) {
			for (i = 0; i < statement->ncolumns; i++) {
				php_oci_column_to_zval(columns[ i ], &element, PHP_OCI_RETURN_LOBS);
				zend_hash_index_update(Z_ARRVAL_P(outarrs[ i ]), rows, &element);
			}

			rows++;

			if (maxrows != -1 && rows == maxrows) {
				php_oci_statement_cancel(statement);
				break;
			}
		}

		efree(columns);
		efree(outarrs);
	}

	RETURN_LONG(rows);
}
/* }}} */

/* {{{ Fetch a result row as an object */
PHP_FUNCTION(oci_fetch_object)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_ASSOC | PHP_OCI_RETURN_NULLS, 2);

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ Fetch a result row as an enumerated array */
PHP_FUNCTION(oci_fetch_row)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_NUM | PHP_OCI_RETURN_NULLS, 1);
}
/* }}} */

/* {{{ Fetch a result row as an associative array */
PHP_FUNCTION(oci_fetch_assoc)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_ASSOC | PHP_OCI_RETURN_NULLS, 1);
}
/* }}} */

/* {{{ Fetch a result row as an array */
PHP_FUNCTION(oci_fetch_array)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_OCI_BOTH | PHP_OCI_RETURN_NULLS, 2);
}
/* }}} */

/* {{{ Free all resources associated with a statement */
PHP_FUNCTION(oci_free_statement)
{
	zval *z_statement;
	php_oci_statement *statement;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_statement)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	zend_list_close(statement->id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Disconnect from database */
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_connection)
	ZEND_PARSE_PARAMETERS_END();

	if (OCI_G(old_oci_close_semantics)) {
		/* do nothing to keep BC */
		RETURN_NULL();
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);
	if (GC_REFCOUNT(connection->id) == 2) { /* CHANGED VERSION::PHP7
											   Changed the refCount to 2 since
											   internally Zend engine increments
											   RefCount value by 1 */
		/* Unregister Oracle TAF */
		php_oci_unregister_taf_callback(connection);

		zend_list_close(connection->id);
	}

	/* ZVAL_NULL(z_connection); */

	RETURN_TRUE;
}
/* }}} */

/* {{{ Connect to an Oracle database and log on. Returns a new session. */
PHP_FUNCTION(oci_new_connect)
{
	php_oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}
/* }}} */

/* {{{ Connect to an Oracle database and log on. Returns a new session. */
PHP_FUNCTION(oci_connect)
{
	php_oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ Connect to an Oracle database using a persistent connection and log on. Returns a new session. */
PHP_FUNCTION(oci_pconnect)
{
	php_oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

/* {{{ Return the last error of stmt|connection|global. If no error happened returns false. */
PHP_FUNCTION(oci_error)
{
	zval *arg = NULL;
	php_oci_statement *statement;
	php_oci_connection *connection;
	text errbuf[PHP_OCI_ERRBUF_LEN];
	sb4 errcode = 0;
	dvoid *errh = NULL;
	ub2 error_offset = 0;
	text *sqltext = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(arg)
	ZEND_PARSE_PARAMETERS_END();

	if (arg) {
		statement = (php_oci_statement *) zend_fetch_resource_ex(arg, NULL, le_statement);
		if (statement) {
			errh = statement->err;
			errcode = statement->errcode;

			if (php_oci_fetch_sqltext_offset(statement, &sqltext, &error_offset)) {
				RETURN_FALSE;
			}
			goto go_out;
		}

		connection = (php_oci_connection *) zend_fetch_resource_ex(arg, NULL, le_connection);
		if (connection) {
			errh = connection->err;
			errcode = connection->errcode;
			goto go_out;
		}

		connection = (php_oci_connection *) zend_fetch_resource_ex(arg, NULL, le_pconnection);
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
		php_error_docref(NULL, E_WARNING, "oci_error: unable to find error handle");
		RETURN_FALSE;
	}

	errcode = php_oci_fetch_errmsg(errh, errbuf, sizeof(errbuf));

	if (errcode) {
		array_init(return_value);
		add_assoc_long(return_value, "code", errcode);
		/* TODO: avoid reallocation ??? */
		add_assoc_string(return_value, "message", (char*) errbuf);
		add_assoc_long(return_value, "offset", error_offset);
		add_assoc_string(return_value, "sqltext", sqltext ? (char *) sqltext : "");
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Return the number of result columns in a statement */
PHP_FUNCTION(oci_num_fields)
{
	zval *z_statement;
	php_oci_statement *statement;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_statement)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	RETURN_LONG(statement->ncolumns);
}
/* }}} */

/* {{{ Parse a SQL or PL/SQL statement and return a statement resource */
PHP_FUNCTION(oci_parse)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_statement *statement;
	char *query;
	size_t query_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_STRING(query, query_len)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	statement = php_oci_statement_create(connection, query, (int) query_len);

	if (statement) {
		RETURN_RES(statement->id);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Sets the number of rows to be prefetched on execute to prefetch_rows for stmt */
PHP_FUNCTION(oci_set_prefetch)
{
	zval *z_statement;
	php_oci_statement *statement;
	zend_long size;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_statement)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (size < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (php_oci_statement_set_prefetch(statement, (ub4)size)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets the amount of LOB data to be prefetched when OCI LOB locators are fetched */
PHP_FUNCTION(oci_set_prefetch_lob)
{
	zval *z_statement;
	zend_long prefetch_lob_size;
#if (OCI_MAJOR_VERSION > 12 || (OCI_MAJOR_VERSION == 12 && OCI_MINOR_VERSION >= 2))
	php_oci_statement *statement;
#endif	

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_statement)
		Z_PARAM_LONG(prefetch_lob_size)
		ZEND_PARSE_PARAMETERS_END();

#if (OCI_MAJOR_VERSION > 12 || (OCI_MAJOR_VERSION == 12 && OCI_MINOR_VERSION >= 2))
	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (prefetch_lob_size < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	statement->prefetch_lob_size = (ub4) prefetch_lob_size;
	RETURN_TRUE;
#else
	/* Although the LOB prefetch feature was available in some earlier Oracle
	 * version, I don't consider it stable until 12.2 */
	php_error_docref(NULL, E_NOTICE, "Unsupported with this version of Oracle Client");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ Sets the client identifier attribute on the connection */
PHP_FUNCTION(oci_set_client_identifier)
{
	zval *z_connection;
	php_oci_connection *connection;
	char *client_id;
	size_t client_id_len;
	sword errstatus;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_STRING(client_id, client_id_len)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) client_id, (ub4) client_id_len, (ub4) OCI_ATTR_CLIENT_IDENTIFIER, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
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

/* {{{ Sets the edition attribute for all subsequent connections created */
PHP_FUNCTION(oci_set_edition)
{
	char *edition;
	size_t edition_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(edition, edition_len)
	ZEND_PARSE_PARAMETERS_END();

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
}
/* }}} */

/* {{{ Sets the module attribute on the connection for end-to-end tracing */
PHP_FUNCTION(oci_set_module_name)
{
	zval *z_connection;
	char *module;
	size_t module_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_STRING(module, module_len)
	ZEND_PARSE_PARAMETERS_END();

	php_oci_connection *connection;
	sword errstatus;

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) module, (ub4) module_len, (ub4) OCI_ATTR_MODULE, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets the action attribute on the connection for end-to-end tracing */
PHP_FUNCTION(oci_set_action)
{
	zval *z_connection;
	char *action;
	size_t action_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_STRING(action, action_len)
	ZEND_PARSE_PARAMETERS_END();

	php_oci_connection *connection;
	sword errstatus;

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) action, (ub4) action_len, (ub4) OCI_ATTR_ACTION, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets the client info attribute on the connection  for end-to-end tracing */
PHP_FUNCTION(oci_set_client_info)
{
	zval *z_connection;
	char *client_info;
	size_t client_info_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_STRING(client_info, client_info_len)
	ZEND_PARSE_PARAMETERS_END();

	php_oci_connection *connection;
	sword errstatus;

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) client_info, (ub4) client_info_len, (ub4) OCI_ATTR_CLIENT_INFO, connection->err));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets the "DB operation" on the connection for Oracle end-to-end tracing.
   For history, see Oracle bug 16695981 */
PHP_FUNCTION(oci_set_db_operation)
{
	zval *z_connection;
	char *dbop_name;
	size_t dbop_name_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_STRING(dbop_name, dbop_name_len)
	ZEND_PARSE_PARAMETERS_END();

#if (OCI_MAJOR_VERSION > 11)
	php_oci_connection *connection;

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) dbop_name, (ub4) dbop_name_len, (ub4) OCI_ATTR_DBOP, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#else
	php_error_docref(NULL, E_NOTICE, "Unsupported attribute type");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ */
PHP_FUNCTION(oci_set_call_timeout)
{
	zval *z_connection;
	zend_long call_timeout;  // milliseconds

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(z_connection)
		Z_PARAM_LONG(call_timeout)
	ZEND_PARSE_PARAMETERS_END();

#if (OCI_MAJOR_VERSION >= 18)
	php_oci_connection *connection;

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->svc, (ub4) OCI_HTYPE_SVCCTX, (ub4 *) &call_timeout, 0, OCI_ATTR_CALL_TIMEOUT, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#else
	php_error_docref(NULL, E_NOTICE, "Unsupported with this version of Oracle Client");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ Changes the password of an account */
PHP_FUNCTION(oci_password_change)
{
	zval *z_connection;
	char *user, *pass_old, *pass_new, *dbname;
	size_t user_len, pass_old_len, pass_new_len, dbname_len;
	php_oci_connection *connection;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "rsss", &z_connection, &user, &user_len, &pass_old, &pass_old_len, &pass_new, &pass_new_len) == SUCCESS) {
		PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

		if (!user_len) {
			zend_argument_value_error(2, "cannot be empty");
			RETURN_THROWS();
		}
		if (!pass_old_len) {
			zend_argument_value_error(3, "cannot be empty");
			RETURN_THROWS();
		}
		if (!pass_new_len) {
			zend_argument_value_error(4, "cannot be empty");
			RETURN_THROWS();
		}

		if (php_oci_password_change(connection, user, (int) user_len, pass_old, (int) pass_old_len, pass_new, (int) pass_new_len)) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "ssss", &dbname, &dbname_len, &user, &user_len, &pass_old, &pass_old_len, &pass_new, &pass_new_len) == SUCCESS) {

		if (!user_len) {
			zend_argument_value_error(2, "cannot be empty");
			RETURN_THROWS();
		}
		if (!pass_old_len) {
			zend_argument_value_error(3, "cannot be empty");
			RETURN_THROWS();
		}
		if (!pass_new_len) {
			zend_argument_value_error(4, "cannot be empty");
			RETURN_THROWS();
		}

		connection = php_oci_do_connect_ex(user, (int) user_len, pass_old, (int) pass_old_len, pass_new, (int) pass_new_len, dbname, (int) dbname_len, NULL, OCI_DEFAULT, 0, 0);
		if (!connection) {
			RETURN_FALSE;
		}
		RETURN_RES(connection->id);
	}
	WRONG_PARAM_COUNT;
}
/* }}} */

/* {{{ Return a new cursor (Statement-Handle) - use this to bind ref-cursors! */
PHP_FUNCTION(oci_new_cursor)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_statement *statement;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_connection)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	statement = php_oci_statement_create(connection, NULL, 0);

	if (statement) {
		RETURN_RES(statement->id);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Return a single column of result data */
PHP_FUNCTION(oci_result)
{
	php_oci_out_column *column;

	column = php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
	if(column) {
		php_oci_column_to_zval(column, return_value, 0);
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Return a string containing runtime client library version information */
PHP_FUNCTION(oci_client_version)
{
	char version[256];
	ZEND_PARSE_PARAMETERS_NONE();

	php_oci_client_get_version(version, sizeof(version));
	RETURN_STRING(version);
}
/* }}} */

/* {{{ Return a string containing server version information */
PHP_FUNCTION(oci_server_version)
{
	zval *z_connection;
	php_oci_connection *connection;
	char version[256];
	zend_string *ret;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_connection)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if (php_oci_server_get_version(connection, version, sizeof(version))) {
		RETURN_FALSE;
	}

	ret = zend_string_init(version, strlen(version), 0);
	RETURN_STR(ret);
}
/* }}} */

/* {{{ Return the query type of an OCI statement */
PHP_FUNCTION(oci_statement_type)
{
	zval *z_statement;
	php_oci_statement *statement;
	ub2 type;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_statement)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_get_type(statement, &type)) {
		RETURN_FALSE;
	}

	switch (type) {
		case OCI_STMT_SELECT:
			RETVAL_STRING("SELECT");
			break;
		case OCI_STMT_UPDATE:
			RETVAL_STRING("UPDATE");
			break;
		case OCI_STMT_DELETE:
			RETVAL_STRING("DELETE");
			break;
		case OCI_STMT_INSERT:
			RETVAL_STRING("INSERT");
			break;
		case OCI_STMT_CREATE:
			RETVAL_STRING("CREATE");
			break;
		case OCI_STMT_DROP:
			RETVAL_STRING("DROP");
			break;
		case OCI_STMT_ALTER:
			RETVAL_STRING("ALTER");
			break;
		case OCI_STMT_BEGIN:
			RETVAL_STRING("BEGIN");
			break;
		case OCI_STMT_DECLARE:
			RETVAL_STRING("DECLARE");
			break;
		case OCI_STMT_CALL:
			RETVAL_STRING("CALL");
			break;
		default:
			RETVAL_STRING("UNKNOWN");
	}
}
/* }}} */

/* {{{ Return the row count of an OCI statement */
PHP_FUNCTION(oci_num_rows)
{
	zval *z_statement;
	php_oci_statement *statement;
	ub4 rowcount;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_statement)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_get_numrows(statement, &rowcount)) {
		RETURN_FALSE;
	}
	RETURN_LONG(rowcount);
}
/* }}} */

/* {{{ Deletes collection object*/
PHP_FUNCTION(oci_free_collection)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_collection, oci_coll_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	zend_list_close(collection->id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Append an object to the collection */
PHP_FUNCTION(oci_collection_append)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;
	char *value;
	size_t value_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &z_collection, oci_coll_class_entry_ptr, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	if (php_oci_collection_append(collection, value, (int) value_len)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Retrieve the value at collection index ndx */
PHP_FUNCTION(oci_collection_element_get)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;
	zend_long element_index;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &z_collection, oci_coll_class_entry_ptr, &element_index) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	if (php_oci_collection_element_get(collection, element_index, return_value)) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Assign a collection from another existing collection */
PHP_FUNCTION(oci_collection_assign)
{
	zval *tmp_dest, *tmp_from, *z_collection_dest, *z_collection_from;
	php_oci_collection *collection_dest, *collection_from;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OO", &z_collection_dest, oci_coll_class_entry_ptr, &z_collection_from, oci_coll_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp_dest = zend_hash_str_find(Z_OBJPROP_P(z_collection_dest), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property. The first argument should be valid collection object");
		RETURN_FALSE;
	}

	if ((tmp_from = zend_hash_str_find(Z_OBJPROP_P(z_collection_from), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property. The second argument should be valid collection object");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp_dest, collection_dest);
	PHP_OCI_ZVAL_TO_COLLECTION(tmp_from, collection_from);

	if (php_oci_collection_assign(collection_dest, collection_from)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Assign element val to collection at index ndx */
PHP_FUNCTION(oci_collection_element_assign)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;
	size_t value_len;
	zend_long element_index;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ols", &z_collection, oci_coll_class_entry_ptr, &element_index, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	if (php_oci_collection_element_set(collection, element_index, value, (int) value_len)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Return the size of a collection */
PHP_FUNCTION(oci_collection_size)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;
	sb4 size = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_collection, oci_coll_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	if (php_oci_collection_size(collection, &size)) {
		RETURN_FALSE;
	}
	RETURN_LONG(size);
}
/* }}} */

/* {{{ Return the max value of a collection. For a varray this is the maximum length of the array */
PHP_FUNCTION(oci_collection_max)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;
	zend_long max;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_collection, oci_coll_class_entry_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	if (php_oci_collection_max(collection, &max)) {
		RETURN_FALSE;
	}
	RETURN_LONG(max);
}
/* }}} */

/* {{{ Trim num elements from the end of a collection */
PHP_FUNCTION(oci_collection_trim)
{
	zval *tmp, *z_collection;
	php_oci_collection *collection;
	zend_long trim_size;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &z_collection, oci_coll_class_entry_ptr, &trim_size) == FAILURE) {
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(z_collection), "collection", sizeof("collection")-1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to find collection property");
		RETURN_FALSE;
	}

	PHP_OCI_ZVAL_TO_COLLECTION(tmp, collection);

	if (php_oci_collection_trim(collection, trim_size)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Initialize a new collection */
PHP_FUNCTION(oci_new_collection)
{
	zval *z_connection;
	php_oci_connection *connection;
	php_oci_collection *collection;
	char *tdo, *schema = NULL;
	size_t tdo_len, schema_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|s!", &z_connection, &tdo, &tdo_len, &schema, &schema_len) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OCI_ZVAL_TO_CONNECTION(z_connection, connection);

	if ( (collection = php_oci_collection_create(connection, tdo, (int) tdo_len, schema, (int) schema_len)) ) {
		object_init_ex(return_value, oci_coll_class_entry_ptr);
		add_property_resource(return_value, "collection", collection->id);
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Get the next statement resource from an Oracle 12c PL/SQL Implicit Result Set */
PHP_FUNCTION(oci_get_implicit_resultset)
{
	zval *z_statement;
	php_oci_statement *statement;
	php_oci_statement *imp_statement;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(z_statement)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	imp_statement = php_oci_get_implicit_resultset(statement);

	if (imp_statement) {
		if (php_oci_statement_execute(imp_statement, (ub4)OCI_DEFAULT))
			RETURN_FALSE;
		RETURN_RES(imp_statement->id);
	}
	RETURN_FALSE;
}

/* }}} */

#endif /* HAVE_OCI8 */
