/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"

static int le_protocols;

struct php_user_stream_wrapper {
	char * protoname;
	char * classname;
	zend_class_entry *ce;
	php_stream_wrapper wrapper;
};

static php_stream *user_wrapper_opener(php_stream_wrapper *wrapper, char *filename, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);

static php_stream_wrapper_ops user_stream_wops = {
	user_wrapper_opener,
	NULL,
	NULL,
	NULL
};


static void stream_wrapper_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct php_user_stream_wrapper * uwrap = (struct php_user_stream_wrapper*)rsrc->ptr;

	php_unregister_url_stream_wrapper(uwrap->protoname TSRMLS_CC);
	efree(uwrap->protoname);
	efree(uwrap->classname);
	efree(uwrap);
}


PHP_MINIT_FUNCTION(user_streams)
{
	le_protocols = zend_register_list_destructors_ex(stream_wrapper_dtor, NULL, "stream factory", 0);
	if (le_protocols == FAILURE)
		return FAILURE;

	REGISTER_LONG_CONSTANT("STREAM_USE_PATH", 			USE_PATH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_IGNORE_URL", 		IGNORE_URL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_ENFORCE_SAFE_MODE",	ENFORCE_SAFE_MODE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_REPORT_ERRORS", 		REPORT_ERRORS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_MUST_SEEK", 			STREAM_MUST_SEEK, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

struct _php_userstream_data {
	struct php_user_stream_wrapper * wrapper;
	zval * object;
};
typedef struct _php_userstream_data php_userstream_data_t;

/* names of methods */
#define USERSTREAM_OPEN		"stream_open"
#define USERSTREAM_CLOSE	"stream_close"
#define USERSTREAM_READ		"stream_read"
#define USERSTREAM_WRITE	"stream_write"
#define USERSTREAM_FLUSH	"stream_flush"
#define USERSTREAM_SEEK		"stream_seek"
#define USERSTREAM_TELL		"stream_tell"
#define USERSTREAM_EOF		"stream_eof"

/* class should have methods like these:
 
	function stream_open($path, $mode, $options, &$opened_path)
	{
	  	return true/false;
	}
	
	function stream_read($count)
	{
	   	return false on error;
		else return string;
	}
	
	function stream_write($data)
	{
	   	return false on error;
		else return count written;
	}
	
	function stream_close()
	{
	}
	
	function stream_flush()
	{
		return true/false;
	}
	
	function stream_seek($offset, $whence)
	{
		return true/false;
	}

	function stream_tell()
	{
		return (int)$position;
	}

	function stream_eof()
	{
		return true/false;
	}
  
 **/

static php_stream *user_wrapper_opener(php_stream_wrapper *wrapper, char *filename, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	php_userstream_data_t *us;
	zval *zfilename, *zmode, *zopened, *zoptions, *zretval = NULL, *zfuncname;
	zval **args[4];	
	int call_result;
	php_stream *stream = NULL;

	us = emalloc(sizeof(*us));
	us->wrapper = uwrap;	

	/* create an instance of our class */
	ALLOC_ZVAL(us->object);
	object_init_ex(us->object, uwrap->ce);
	ZVAL_REFCOUNT(us->object) = 1;
	PZVAL_IS_REF(us->object) = 1;
	
	/* call it's stream_open method - set up params first */
	MAKE_STD_ZVAL(zfilename);
	ZVAL_STRING(zfilename, filename, 1);
	args[0] = &zfilename;

	MAKE_STD_ZVAL(zmode);
	ZVAL_STRING(zmode, mode, 1);
	args[1] = &zmode;

	MAKE_STD_ZVAL(zoptions);
	ZVAL_LONG(zoptions, options);
	args[2] = &zoptions;

	MAKE_STD_ZVAL(zopened);
	ZVAL_REFCOUNT(zopened) = 1;
	PZVAL_IS_REF(zopened) = 1;
	ZVAL_NULL(zopened);
	args[3] = &zopened;

	MAKE_STD_ZVAL(zfuncname);
	ZVAL_STRING(zfuncname, USERSTREAM_OPEN, 1);
	
	call_result = call_user_function_ex(NULL,
			&us->object,
			zfuncname,
			&zretval,
			4, args,
			0, NULL	TSRMLS_CC);
	
	if (call_result == SUCCESS && zretval != NULL && zval_is_true(zretval)) {
		/* the stream is now open! */
		stream = php_stream_alloc_rel(&php_stream_userspace_ops, us, 0, mode);

		/* if the opened path is set, copy it out */
		if (Z_TYPE_P(zopened) == IS_STRING && opened_path) {
			*opened_path = estrndup(Z_STRVAL_P(zopened), Z_STRLEN_P(zopened));
		}

		/* set wrapper data to be a reference to our object */
		stream->wrapperdata = us->object;
		zval_add_ref(&stream->wrapperdata);
	} else {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "\"%s::" USERSTREAM_OPEN "\" call failed",
			us->wrapper->classname);
	}
	
	/* destroy everything else */
	if (stream == NULL) {
		zval_ptr_dtor(&us->object);
		efree(us);
	}
	if (zretval)
		zval_ptr_dtor(&zretval);
	
	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&zopened);
	zval_ptr_dtor(&zoptions);
	zval_ptr_dtor(&zmode);
	zval_ptr_dtor(&zfilename);

	return stream;
}

/* {{{ proto bool file_register_wrapper(string protocol, string classname)
   Registers a custom URL protocol handler class */
PHP_FUNCTION(file_register_wrapper)
{
	char *protocol, *classname;
	int protocol_len, classname_len;
	struct php_user_stream_wrapper * uwrap;
	int rsrc_id;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &protocol, &protocol_len, &classname, &classname_len) == FAILURE) {
		RETURN_FALSE;
	}
	
	uwrap = (struct php_user_stream_wrapper *)ecalloc(1, sizeof(*uwrap));
	uwrap->protoname = estrndup(protocol, protocol_len);
	uwrap->classname = estrndup(classname, classname_len);
	uwrap->wrapper.wops = &user_stream_wops;
	uwrap->wrapper.abstract = uwrap;

	zend_str_tolower(uwrap->classname, classname_len);
	rsrc_id = ZEND_REGISTER_RESOURCE(NULL, uwrap, le_protocols);
	
	if (zend_hash_find(EG(class_table), uwrap->classname, classname_len + 1, (void**)&uwrap->ce) == SUCCESS) {
#ifdef ZEND_ENGINE_2
		uwrap->ce = *(zend_class_entry**)uwrap->ce;
#endif
		if (php_register_url_stream_wrapper(protocol, &uwrap->wrapper TSRMLS_CC) == SUCCESS) {
			RETURN_TRUE;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "class '%s' is undefined",
				classname);
	}

	zend_list_delete(rsrc_id);
	RETURN_FALSE;
}
/* }}} */


static size_t php_userstreamop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	zval func_name;
	zval *retval = NULL;
	int call_result;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	zval **args[1];
	zval zbuff, *zbufptr;
	size_t didwrite = 0;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_WRITE, sizeof(USERSTREAM_WRITE)-1, 0);

	ZVAL_STRINGL(&zbuff, (char*)buf, count, 0);
	zbufptr = &zbuff;
	args[0] = &zbufptr;

	call_result = call_user_function_ex(NULL,
			&us->object,
			&func_name,
			&retval,
			1, args,
			0, NULL TSRMLS_CC);

	didwrite = 0;
	if (call_result == SUCCESS && retval != NULL) {
		convert_to_long_ex(&retval);
		didwrite = Z_LVAL_P(retval);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s::" USERSTREAM_WRITE " - is not implemented!",
				us->wrapper->classname);
	}

	/* don't allow strange buffer overruns due to bogus return */
	if (didwrite > count) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s::" USERSTREAM_WRITE " - wrote %d bytes more data than requested (%d written, %d max)",
				us->wrapper->classname,
				didwrite - count, didwrite, count);
		didwrite = count;
	}
	
	if (retval)
		zval_ptr_dtor(&retval);
	
	return didwrite;
}

static size_t php_userstreamop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	zval func_name;
	zval *retval = NULL;
	zval **args[1];
	int call_result;
	size_t didread = 0;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);

	if (buf == NULL && count == 0) {
		ZVAL_STRINGL(&func_name, USERSTREAM_EOF, sizeof(USERSTREAM_EOF)-1, 0);

		call_result = call_user_function_ex(NULL,
			&us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL TSRMLS_CC);

		if (call_result == SUCCESS && retval != NULL && zval_is_true(retval))
			didread = 0;
		else {
			if (call_result == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s::" USERSTREAM_EOF " - is not implemented! Assuming EOF",
						us->wrapper->classname);
			}

			didread = EOF;
		}

	} else {
		zval *zcount;

		ZVAL_STRINGL(&func_name, USERSTREAM_READ, sizeof(USERSTREAM_READ)-1, 0);

		MAKE_STD_ZVAL(zcount);
		ZVAL_LONG(zcount, count);
		args[0] = &zcount;

		call_result = call_user_function_ex(NULL,
				&us->object,
				&func_name,
				&retval,
				1, args,
				0, NULL TSRMLS_CC);

		if (call_result == SUCCESS && retval != NULL) {
			convert_to_string_ex(&retval);
			didread = Z_STRLEN_P(retval);
			if (didread > count) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s::" USERSTREAM_READ " - read %d bytes more data than requested (%d read, %d max) - excess data will be lost",
						us->wrapper->classname, didread - count, didread, count);
				didread = count;
			}
			if (didread > 0)
				memcpy(buf, Z_STRVAL_P(retval), didread);
		} else if (call_result == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s::" USERSTREAM_READ " - is not implemented!",
					us->wrapper->classname);
		}
		zval_ptr_dtor(&zcount);
	}
	
	if (retval)
		zval_ptr_dtor(&retval);
	
	return didread;
}

static int php_userstreamop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	zval func_name;
	zval *retval = NULL;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);
	
	ZVAL_STRINGL(&func_name, USERSTREAM_CLOSE, sizeof(USERSTREAM_CLOSE)-1, 0);
	
	call_user_function_ex(NULL,
			&us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL TSRMLS_CC);

	if (retval)
		zval_ptr_dtor(&retval);
	
	zval_ptr_dtor(&us->object);

	efree(us);
	
	return 0;
}

static int php_userstreamop_flush(php_stream *stream TSRMLS_DC)
{
	zval func_name;
	zval *retval = NULL;
	int call_result;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_FLUSH, sizeof(USERSTREAM_FLUSH)-1, 0);
	
	call_result = call_user_function_ex(NULL,
			&us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL TSRMLS_CC);

	if (call_result == SUCCESS && retval != NULL && zval_is_true(retval))
		call_result = 0;
	else
		call_result = -1;
	
	if (retval)
		zval_ptr_dtor(&retval);
	
	return call_result;
}

static int php_userstreamop_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	zval func_name;
	zval *retval = NULL;
	int call_result, ret;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	zval **args[2];
	zval *zoffs, *zwhence;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_SEEK, sizeof(USERSTREAM_SEEK)-1, 0);

	MAKE_STD_ZVAL(zoffs);
	ZVAL_LONG(zoffs, offset);
	args[0] = &zoffs;

	MAKE_STD_ZVAL(zwhence);
	ZVAL_LONG(zwhence, whence);
	args[1] = &zwhence;

	call_result = call_user_function_ex(NULL,
			&us->object,
			&func_name,
			&retval,
			2, args,
			0, NULL TSRMLS_CC);

	zval_ptr_dtor(&zoffs);
	zval_ptr_dtor(&zwhence);

	if (call_result == FAILURE) {
		/* stream_seek is not implemented, so disable seeks for this stream */
		stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
		/* there should be no retval to clean up */
		return -1;
	} else if (call_result == SUCCESS && retval != NULL && zval_is_true(retval)) {
		ret = 0;
	} else {
		ret = -1;
	}

	if (retval)
		zval_ptr_dtor(&retval);

	/* now determine where we are */
	ZVAL_STRINGL(&func_name, USERSTREAM_TELL, sizeof(USERSTREAM_TELL)-1, 0);

	call_result = call_user_function_ex(NULL,
		&us->object,
		&func_name,
		&retval,
		0, NULL, 0, NULL TSRMLS_CC);

	if (call_result == SUCCESS && retval != NULL && Z_TYPE_P(retval) == IS_LONG)
		*newoffs = Z_LVAL_P(retval);
	else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s::" USERSTREAM_TELL " - is not implemented!",
				us->wrapper->classname);
	
	if (retval)
		zval_ptr_dtor(&retval);

	return 0;
}

php_stream_ops php_stream_userspace_ops = {
	php_userstreamop_write, php_userstreamop_read,
	php_userstreamop_close, php_userstreamop_flush,
	"user-space",
	php_userstreamop_seek,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};


