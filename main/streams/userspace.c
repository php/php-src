/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   |          Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/file.h"
#include "ext/standard/flock_compat.h"
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#include <stddef.h>

#if HAVE_UTIME
# ifdef PHP_WIN32
#  include <sys/utime.h>
# else
#  include <utime.h>
# endif
#endif

static int le_protocols;

struct php_user_stream_wrapper {
	char * protoname;
	char * classname;
	zend_class_entry *ce;
	php_stream_wrapper wrapper;
};

static php_stream *user_wrapper_opener(php_stream_wrapper *wrapper, const char *filename, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
static int user_wrapper_stat_url(php_stream_wrapper *wrapper, const char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context);
static int user_wrapper_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context);
static int user_wrapper_rename(php_stream_wrapper *wrapper, const char *url_from, const char *url_to, int options, php_stream_context *context);
static int user_wrapper_mkdir(php_stream_wrapper *wrapper, const char *url, int mode, int options, php_stream_context *context);
static int user_wrapper_rmdir(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context);
static int user_wrapper_metadata(php_stream_wrapper *wrapper, const char *url, int option, void *value, php_stream_context *context);
static php_stream *user_wrapper_opendir(php_stream_wrapper *wrapper, const char *filename, const char *mode,
		int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);

static php_stream_wrapper_ops user_stream_wops = {
	user_wrapper_opener,
	NULL, /* close - the streams themselves know how */
	NULL, /* stat - the streams themselves know how */
	user_wrapper_stat_url,
	user_wrapper_opendir,
	"user-space",
	user_wrapper_unlink,
	user_wrapper_rename,
	user_wrapper_mkdir,
	user_wrapper_rmdir,
	user_wrapper_metadata
};


static void stream_wrapper_dtor(zend_resource *rsrc)
{
	struct php_user_stream_wrapper * uwrap = (struct php_user_stream_wrapper*)rsrc->ptr;

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
	REGISTER_LONG_CONSTANT("STREAM_REPORT_ERRORS", 		REPORT_ERRORS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_MUST_SEEK", 			STREAM_MUST_SEEK, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_URL_STAT_LINK", 		PHP_STREAM_URL_STAT_LINK,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_URL_STAT_QUIET", 	PHP_STREAM_URL_STAT_QUIET,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_MKDIR_RECURSIVE",	PHP_STREAM_MKDIR_RECURSIVE,		CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_IS_URL",	PHP_STREAM_IS_URL,		CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_OPTION_BLOCKING",	PHP_STREAM_OPTION_BLOCKING,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_OPTION_READ_TIMEOUT",	PHP_STREAM_OPTION_READ_TIMEOUT,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_OPTION_READ_BUFFER",	PHP_STREAM_OPTION_READ_BUFFER,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_OPTION_WRITE_BUFFER",	PHP_STREAM_OPTION_WRITE_BUFFER,		CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_BUFFER_NONE",		PHP_STREAM_BUFFER_NONE,			CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_BUFFER_LINE",		PHP_STREAM_BUFFER_LINE,			CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_BUFFER_FULL",		PHP_STREAM_BUFFER_FULL,			CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_CAST_AS_STREAM",		PHP_STREAM_AS_STDIO,			CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CAST_FOR_SELECT",	PHP_STREAM_AS_FD_FOR_SELECT,		CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_META_TOUCH",			PHP_STREAM_META_TOUCH,			CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_META_OWNER",			PHP_STREAM_META_OWNER,			CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_META_OWNER_NAME",	PHP_STREAM_META_OWNER_NAME,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_META_GROUP",			PHP_STREAM_META_GROUP,			CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_META_GROUP_NAME",	PHP_STREAM_META_GROUP_NAME,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_META_ACCESS",		PHP_STREAM_META_ACCESS,			CONST_CS|CONST_PERSISTENT);
	return SUCCESS;
}

struct _php_userstream_data {
	struct php_user_stream_wrapper * wrapper;
	zval object;
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
#define USERSTREAM_STAT		"stream_stat"
#define USERSTREAM_STATURL	"url_stat"
#define USERSTREAM_UNLINK	"unlink"
#define USERSTREAM_RENAME	"rename"
#define USERSTREAM_MKDIR	"mkdir"
#define USERSTREAM_RMDIR	"rmdir"
#define USERSTREAM_DIR_OPEN		"dir_opendir"
#define USERSTREAM_DIR_READ		"dir_readdir"
#define USERSTREAM_DIR_REWIND	"dir_rewinddir"
#define USERSTREAM_DIR_CLOSE	"dir_closedir"
#define USERSTREAM_LOCK     "stream_lock"
#define USERSTREAM_CAST		"stream_cast"
#define USERSTREAM_SET_OPTION	"stream_set_option"
#define USERSTREAM_TRUNCATE	"stream_truncate"
#define USERSTREAM_METADATA	"stream_metadata"

/* {{{ class should have methods like these:

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

	function stream_stat()
	{
		return array( just like that returned by fstat() );
	}

	function stream_cast($castas)
	{
		if ($castas == STREAM_CAST_FOR_SELECT) {
			return $this->underlying_stream;
		}
		return false;
	}

	function stream_set_option($option, $arg1, $arg2)
	{
		switch($option) {
		case STREAM_OPTION_BLOCKING:
			$blocking = $arg1;
			...
		case STREAM_OPTION_READ_TIMEOUT:
			$sec = $arg1;
			$usec = $arg2;
			...
		case STREAM_OPTION_WRITE_BUFFER:
			$mode = $arg1;
			$size = $arg2;
			...
		default:
			return false;
		}
	}

	function url_stat(string $url, int $flags)
	{
		return array( just like that returned by stat() );
	}

	function unlink(string $url)
	{
		return true / false;
	}

	function rename(string $from, string $to)
	{
		return true / false;
	}

	function mkdir($dir, $mode, $options)
	{
		return true / false;
	}

	function rmdir($dir, $options)
	{
		return true / false;
	}

	function dir_opendir(string $url, int $options)
	{
		return true / false;
	}

	function dir_readdir()
	{
		return string next filename in dir ;
	}

	function dir_closedir()
	{
		release dir related resources;
	}

	function dir_rewinddir()
	{
		reset to start of dir list;
	}

	function stream_lock($operation)
	{
		return true / false;
	}

 	function stream_truncate($new_size)
	{
		return true / false;
	}

	}}} **/

static void user_stream_create_object(struct php_user_stream_wrapper *uwrap, php_stream_context *context, zval *object)
{
	/* create an instance of our class */
	object_init_ex(object, uwrap->ce);

	if (context) {
		add_property_resource(object, "context", context->res);
		GC_REFCOUNT(context->res)++;
	} else {
		add_property_null(object, "context");
	}

	if (uwrap->ce->constructor) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval retval;

		fci.size = sizeof(fci);
		fci.function_table = &uwrap->ce->function_table;
		ZVAL_UNDEF(&fci.function_name);
		fci.symbol_table = NULL;
		fci.object = Z_OBJ_P(object);
		fci.retval = &retval;
		fci.param_count = 0;
		fci.params = NULL;
		fci.no_separation = 1;

		fcc.initialized = 1;
		fcc.function_handler = uwrap->ce->constructor;
		fcc.calling_scope = EG(scope);
		fcc.called_scope = Z_OBJCE_P(object);
		fcc.object = Z_OBJ_P(object);

		if (zend_call_function(&fci, &fcc) == FAILURE) {
			php_error_docref(NULL, E_WARNING, "Could not execute %s::%s()", ZSTR_VAL(uwrap->ce->name), ZSTR_VAL(uwrap->ce->constructor->common.function_name));
			zval_dtor(object);
			ZVAL_UNDEF(object);
		} else {
			zval_ptr_dtor(&retval);
		}
	}
}

static php_stream *user_wrapper_opener(php_stream_wrapper *wrapper, const char *filename, const char *mode,
									   int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	php_userstream_data_t *us;
	zval zretval, zfuncname;
	zval args[4];
	int call_result;
	php_stream *stream = NULL;
	zend_bool old_in_user_include;

	/* Try to catch bad usage without preventing flexibility */
	if (FG(user_stream_current_filename) != NULL && strcmp(filename, FG(user_stream_current_filename)) == 0) {
		php_stream_wrapper_log_error(wrapper, options, "infinite recursion prevented");
		return NULL;
	}
	FG(user_stream_current_filename) = filename;

	/* if the user stream was registered as local and we are in include context,
		we add allow_url_include restrictions to allow_url_fopen ones */
	/* we need only is_url == 0 here since if is_url == 1 and remote wrappers
		were restricted we wouldn't get here */
	old_in_user_include = PG(in_user_include);
	if(uwrap->wrapper.is_url == 0 &&
		(options & STREAM_OPEN_FOR_INCLUDE) &&
		!PG(allow_url_include)) {
		PG(in_user_include) = 1;
	}

	us = emalloc(sizeof(*us));
	us->wrapper = uwrap;

	user_stream_create_object(uwrap, context, &us->object);
	if (Z_TYPE(us->object) == IS_UNDEF) {
		FG(user_stream_current_filename) = NULL;
		PG(in_user_include) = old_in_user_include;
		efree(us);
		return NULL;
	}

	/* call it's stream_open method - set up params first */
	ZVAL_STRING(&args[0], filename);
	ZVAL_STRING(&args[1], mode);
	ZVAL_LONG(&args[2], options);
	ZVAL_NEW_REF(&args[3], &EG(uninitialized_zval));

	ZVAL_STRING(&zfuncname, USERSTREAM_OPEN);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&zfuncname,
			&zretval,
			4, args,
			0, NULL	);

	if (call_result == SUCCESS && Z_TYPE(zretval) != IS_UNDEF && zval_is_true(&zretval)) {
		/* the stream is now open! */
		stream = php_stream_alloc_rel(&php_stream_userspace_ops, us, 0, mode);

		/* if the opened path is set, copy it out */
		if (Z_ISREF(args[3]) && Z_TYPE_P(Z_REFVAL(args[3])) == IS_STRING && opened_path) {
			*opened_path = zend_string_copy(Z_STR_P(Z_REFVAL(args[3])));
		}

		/* set wrapper data to be a reference to our object */
		ZVAL_COPY(&stream->wrapperdata, &us->object);
	} else {
		php_stream_wrapper_log_error(wrapper, options, "\"%s::" USERSTREAM_OPEN "\" call failed",
			us->wrapper->classname);
	}

	/* destroy everything else */
	if (stream == NULL) {
		zval_ptr_dtor(&us->object);
		ZVAL_UNDEF(&us->object);
		efree(us);
	}
	zval_ptr_dtor(&zretval);
	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[3]);
	zval_ptr_dtor(&args[2]);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	FG(user_stream_current_filename) = NULL;

	PG(in_user_include) = old_in_user_include;
	return stream;
}

static php_stream *user_wrapper_opendir(php_stream_wrapper *wrapper, const char *filename, const char *mode,
		int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	php_userstream_data_t *us;
	zval zretval, zfuncname;
	zval args[2];
	int call_result;
	php_stream *stream = NULL;

	/* Try to catch bad usage without preventing flexibility */
	if (FG(user_stream_current_filename) != NULL && strcmp(filename, FG(user_stream_current_filename)) == 0) {
		php_stream_wrapper_log_error(wrapper, options, "infinite recursion prevented");
		return NULL;
	}
	FG(user_stream_current_filename) = filename;

	us = emalloc(sizeof(*us));
	us->wrapper = uwrap;

	user_stream_create_object(uwrap, context, &us->object);
	if (Z_TYPE(us->object) == IS_UNDEF) {
		FG(user_stream_current_filename) = NULL;
		efree(us);
		return NULL;
	}

	/* call it's dir_open method - set up params first */
	ZVAL_STRING(&args[0], filename);
	ZVAL_LONG(&args[1], options);

	ZVAL_STRING(&zfuncname, USERSTREAM_DIR_OPEN);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&zfuncname,
			&zretval,
			2, args,
			0, NULL	);

	if (call_result == SUCCESS && Z_TYPE(zretval) != IS_UNDEF && zval_is_true(&zretval)) {
		/* the stream is now open! */
		stream = php_stream_alloc_rel(&php_stream_userspace_dir_ops, us, 0, mode);

		/* set wrapper data to be a reference to our object */
		ZVAL_COPY(&stream->wrapperdata, &us->object);
	} else {
		php_stream_wrapper_log_error(wrapper, options, "\"%s::" USERSTREAM_DIR_OPEN "\" call failed",
			us->wrapper->classname);
	}

	/* destroy everything else */
	if (stream == NULL) {
		zval_ptr_dtor(&us->object);
		ZVAL_UNDEF(&us->object);
		efree(us);
	}
	zval_ptr_dtor(&zretval);

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	FG(user_stream_current_filename) = NULL;

	return stream;
}


/* {{{ proto bool stream_wrapper_register(string protocol, string classname[, integer flags])
   Registers a custom URL protocol handler class */
PHP_FUNCTION(stream_wrapper_register)
{
	zend_string *protocol, *classname;
	struct php_user_stream_wrapper * uwrap;
	zend_resource *rsrc;
	zend_long flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|l", &protocol, &classname, &flags) == FAILURE) {
		RETURN_FALSE;
	}

	uwrap = (struct php_user_stream_wrapper *)ecalloc(1, sizeof(*uwrap));
	uwrap->protoname = estrndup(ZSTR_VAL(protocol), ZSTR_LEN(protocol));
	uwrap->classname = estrndup(ZSTR_VAL(classname), ZSTR_LEN(classname));
	uwrap->wrapper.wops = &user_stream_wops;
	uwrap->wrapper.abstract = uwrap;
	uwrap->wrapper.is_url = ((flags & PHP_STREAM_IS_URL) != 0);

	rsrc = zend_register_resource(uwrap, le_protocols);

	if ((uwrap->ce = zend_lookup_class(classname)) != NULL) {
		if (php_register_url_stream_wrapper_volatile(ZSTR_VAL(protocol), &uwrap->wrapper) == SUCCESS) {
			RETURN_TRUE;
		} else {
			/* We failed.  But why? */
			if (zend_hash_exists(php_stream_get_url_stream_wrappers_hash(), protocol)) {
				php_error_docref(NULL, E_WARNING, "Protocol %s:// is already defined.", ZSTR_VAL(protocol));
			} else {
				/* Hash doesn't exist so it must have been an invalid protocol scheme */
				php_error_docref(NULL, E_WARNING, "Invalid protocol scheme specified. Unable to register wrapper class %s to %s://", ZSTR_VAL(classname), ZSTR_VAL(protocol));
			}
		}
	} else {
		php_error_docref(NULL, E_WARNING, "class '%s' is undefined", ZSTR_VAL(classname));
	}

	zend_list_delete(rsrc);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool stream_wrapper_unregister(string protocol)
	Unregister a wrapper for the life of the current request. */
PHP_FUNCTION(stream_wrapper_unregister)
{
	char *protocol;
	size_t protocol_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &protocol, &protocol_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (php_unregister_url_stream_wrapper_volatile(protocol) == FAILURE) {
		/* We failed */
		php_error_docref(NULL, E_WARNING, "Unable to unregister protocol %s://", protocol);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool stream_wrapper_restore(string protocol)
	Restore the original protocol handler, overriding if necessary */
PHP_FUNCTION(stream_wrapper_restore)
{
	zend_string *protocol;
	php_stream_wrapper *wrapper;
	HashTable *global_wrapper_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &protocol) == FAILURE) {
		RETURN_FALSE;
	}

	global_wrapper_hash = php_stream_get_url_stream_wrappers_hash_global();
	if (php_stream_get_url_stream_wrappers_hash() == global_wrapper_hash) {
		php_error_docref(NULL, E_NOTICE, "%s:// was never changed, nothing to restore", ZSTR_VAL(protocol));
		RETURN_TRUE;
	}

	if ((wrapper = zend_hash_find_ptr(global_wrapper_hash, protocol)) == NULL) {
		php_error_docref(NULL, E_WARNING, "%s:// never existed, nothing to restore", ZSTR_VAL(protocol));
		RETURN_FALSE;
	}

	/* A failure here could be okay given that the protocol might have been merely unregistered */
	php_unregister_url_stream_wrapper_volatile(ZSTR_VAL(protocol));

	if (php_register_url_stream_wrapper_volatile(ZSTR_VAL(protocol), wrapper) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Unable to restore original %s:// wrapper", ZSTR_VAL(protocol));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

static size_t php_userstreamop_write(php_stream *stream, const char *buf, size_t count)
{
	zval func_name;
	zval retval;
	int call_result;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	zval args[1];
	size_t didwrite = 0;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_WRITE, sizeof(USERSTREAM_WRITE)-1);

	ZVAL_STRINGL(&args[0], (char*)buf, count);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			1, args,
			0, NULL);
	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&func_name);

	didwrite = 0;

	if (EG(exception)) {
		return 0;
	}

	if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		convert_to_long(&retval);
		didwrite = Z_LVAL(retval);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_WRITE " is not implemented!",
				us->wrapper->classname);
	}

	/* don't allow strange buffer overruns due to bogus return */
	if (didwrite > count) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_WRITE " wrote " ZEND_LONG_FMT " bytes more data than requested (" ZEND_LONG_FMT " written, " ZEND_LONG_FMT " max)",
				us->wrapper->classname,
				(zend_long)(didwrite - count), (zend_long)didwrite, (zend_long)count);
		didwrite = count;
	}

	zval_ptr_dtor(&retval);

	return didwrite;
}

static size_t php_userstreamop_read(php_stream *stream, char *buf, size_t count)
{
	zval func_name;
	zval retval;
	zval args[1];
	int call_result;
	size_t didread = 0;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_READ, sizeof(USERSTREAM_READ)-1);

	ZVAL_LONG(&args[0], count);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			1, args,
			0, NULL);

	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&func_name);

	if (EG(exception)) {
		return -1;
	}

	if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		convert_to_string(&retval);
		didread = Z_STRLEN(retval);
		if (didread > count) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_READ " - read " ZEND_LONG_FMT " bytes more data than requested (" ZEND_LONG_FMT " read, " ZEND_LONG_FMT " max) - excess data will be lost",
					us->wrapper->classname, (zend_long)(didread - count), (zend_long)didread, (zend_long)count);
			didread = count;
		}
		if (didread > 0)
			memcpy(buf, Z_STRVAL(retval), didread);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_READ " is not implemented!",
				us->wrapper->classname);
	}

	zval_ptr_dtor(&retval);
	ZVAL_UNDEF(&retval);

	/* since the user stream has no way of setting the eof flag directly, we need to ask it if we hit eof */

	ZVAL_STRINGL(&func_name, USERSTREAM_EOF, sizeof(USERSTREAM_EOF)-1);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL);

	if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF && zval_is_true(&retval)) {
		stream->eof = 1;
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING,
				"%s::" USERSTREAM_EOF " is not implemented! Assuming EOF",
				us->wrapper->classname);

		stream->eof = 1;
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	return didread;
}

static int php_userstreamop_close(php_stream *stream, int close_handle)
{
	zval func_name;
	zval retval;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_CLOSE, sizeof(USERSTREAM_CLOSE)-1);

	call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL);

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	zval_ptr_dtor(&us->object);
	ZVAL_UNDEF(&us->object);

	efree(us);

	return 0;
}

static int php_userstreamop_flush(php_stream *stream)
{
	zval func_name;
	zval retval;
	int call_result;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_FLUSH, sizeof(USERSTREAM_FLUSH)-1);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL);

	if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF && zval_is_true(&retval))
		call_result = 0;
	else
		call_result = -1;

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	return call_result;
}

static int php_userstreamop_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs)
{
	zval func_name;
	zval retval;
	int call_result, ret;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	zval args[2];

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_SEEK, sizeof(USERSTREAM_SEEK)-1);

	ZVAL_LONG(&args[0], offset);
	ZVAL_LONG(&args[1], whence);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			2, args,
			0, NULL);

	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&func_name);

	if (call_result == FAILURE) {
		/* stream_seek is not implemented, so disable seeks for this stream */
		stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
		/* there should be no retval to clean up */

		zval_ptr_dtor(&retval);

		return -1;
	} else if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF && zval_is_true(&retval)) {
		ret = 0;
	} else {
		ret = -1;
	}

	zval_ptr_dtor(&retval);
	ZVAL_UNDEF(&retval);

	if (ret) {
		return ret;
	}

	/* now determine where we are */
	ZVAL_STRINGL(&func_name, USERSTREAM_TELL, sizeof(USERSTREAM_TELL)-1);

	call_result = call_user_function_ex(NULL,
		Z_ISUNDEF(us->object)? NULL : &us->object,
		&func_name,
		&retval,
		0, NULL, 0, NULL);

	if (call_result == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		*newoffs = Z_LVAL(retval);
		ret = 0;
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_TELL " is not implemented!", us->wrapper->classname);
		ret = -1;
	} else {
		ret = -1;
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);
	return ret;
}

/* parse the return value from one of the stat functions and store the
 * relevant fields into the statbuf provided */
static int statbuf_from_array(zval *array, php_stream_statbuf *ssb)
{
	zval *elem;

#define STAT_PROP_ENTRY_EX(name, name2)                        \
	if (NULL != (elem = zend_hash_str_find(Z_ARRVAL_P(array), #name, sizeof(#name)-1))) {     \
		ssb->sb.st_##name2 = zval_get_long(elem);                                                      \
	}

#define STAT_PROP_ENTRY(name) STAT_PROP_ENTRY_EX(name,name)

	memset(ssb, 0, sizeof(php_stream_statbuf));
	STAT_PROP_ENTRY(dev);
	STAT_PROP_ENTRY(ino);
	STAT_PROP_ENTRY(mode);
	STAT_PROP_ENTRY(nlink);
	STAT_PROP_ENTRY(uid);
	STAT_PROP_ENTRY(gid);
#if HAVE_ST_RDEV
	STAT_PROP_ENTRY(rdev);
#endif
	STAT_PROP_ENTRY(size);
#ifdef NETWARE
	STAT_PROP_ENTRY_EX(atime, atime.tv_sec);
	STAT_PROP_ENTRY_EX(mtime, mtime.tv_sec);
	STAT_PROP_ENTRY_EX(ctime, ctime.tv_sec);
#else
	STAT_PROP_ENTRY(atime);
	STAT_PROP_ENTRY(mtime);
	STAT_PROP_ENTRY(ctime);
#endif
#ifdef HAVE_ST_BLKSIZE
	STAT_PROP_ENTRY(blksize);
#endif
#ifdef HAVE_ST_BLOCKS
	STAT_PROP_ENTRY(blocks);
#endif

#undef STAT_PROP_ENTRY
#undef STAT_PROP_ENTRY_EX
	return SUCCESS;
}

static int php_userstreamop_stat(php_stream *stream, php_stream_statbuf *ssb)
{
	zval func_name;
	zval retval;
	int call_result;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	int ret = -1;

	ZVAL_STRINGL(&func_name, USERSTREAM_STAT, sizeof(USERSTREAM_STAT)-1);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL);

	if (call_result == SUCCESS && Z_TYPE(retval) == IS_ARRAY) {
		if (SUCCESS == statbuf_from_array(&retval, ssb))
			ret = 0;
	} else {
		if (call_result == FAILURE) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_STAT " is not implemented!",
					us->wrapper->classname);
		}
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	return ret;
}


static int php_userstreamop_set_option(php_stream *stream, int option, int value, void *ptrparam) {
	zval func_name;
	zval retval;
	int call_result;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	int ret = PHP_STREAM_OPTION_RETURN_NOTIMPL;
	zval args[3];

	switch (option) {
	case PHP_STREAM_OPTION_CHECK_LIVENESS:
		ZVAL_STRINGL(&func_name, USERSTREAM_EOF, sizeof(USERSTREAM_EOF)-1);
		call_result = call_user_function_ex(NULL, Z_ISUNDEF(us->object)? NULL : &us->object, &func_name, &retval, 0, NULL, 0, NULL);
		if (call_result == SUCCESS && (Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE)) {
			ret = zval_is_true(&retval) ? PHP_STREAM_OPTION_RETURN_ERR : PHP_STREAM_OPTION_RETURN_OK;
		} else {
			ret = PHP_STREAM_OPTION_RETURN_ERR;
			php_error_docref(NULL, E_WARNING,
					"%s::" USERSTREAM_EOF " is not implemented! Assuming EOF",
					us->wrapper->classname);
		}
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&func_name);
		break;

	case PHP_STREAM_OPTION_LOCKING:
		ZVAL_LONG(&args[0], 0);

		if (value & LOCK_NB) {
			Z_LVAL_P(&args[0]) |= PHP_LOCK_NB;
		}
		switch(value & ~LOCK_NB) {
		case LOCK_SH:
			Z_LVAL_P(&args[0]) |= PHP_LOCK_SH;
			break;
		case LOCK_EX:
			Z_LVAL_P(&args[0]) |= PHP_LOCK_EX;
			break;
		case LOCK_UN:
			Z_LVAL_P(&args[0]) |= PHP_LOCK_UN;
			break;
		}

		/* TODO wouldblock */
		ZVAL_STRINGL(&func_name, USERSTREAM_LOCK, sizeof(USERSTREAM_LOCK)-1);

		call_result = call_user_function_ex(NULL,
						Z_ISUNDEF(us->object)? NULL : &us->object,
						&func_name,
						&retval,
						1, args, 0, NULL);

		if (call_result == SUCCESS && (Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE)) {
			ret = (Z_TYPE(retval) == IS_FALSE);
		} else if (call_result == FAILURE) {
			if (value == 0) {
			   	/* lock support test (TODO: more check) */
				ret = PHP_STREAM_OPTION_RETURN_OK;
			} else {
				php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_LOCK " is not implemented!",
								 us->wrapper->classname);
				ret = PHP_STREAM_OPTION_RETURN_ERR;
			}
		}

		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&func_name);
		zval_ptr_dtor(&args[0]);
		break;

	case PHP_STREAM_OPTION_TRUNCATE_API:
		ZVAL_STRINGL(&func_name, USERSTREAM_TRUNCATE, sizeof(USERSTREAM_TRUNCATE)-1);

		switch (value) {
		case PHP_STREAM_TRUNCATE_SUPPORTED:
			if (zend_is_callable_ex(&func_name,
					Z_ISUNDEF(us->object)? NULL : Z_OBJ(us->object),
					IS_CALLABLE_CHECK_SILENT, NULL, NULL, NULL))
				ret = PHP_STREAM_OPTION_RETURN_OK;
			else
				ret = PHP_STREAM_OPTION_RETURN_ERR;
			break;

		case PHP_STREAM_TRUNCATE_SET_SIZE: {
			ptrdiff_t new_size = *(ptrdiff_t*) ptrparam;
			if (new_size >= 0 && new_size <= (ptrdiff_t)LONG_MAX) {
				ZVAL_LONG(&args[0], (zend_long)new_size);
				call_result = call_user_function_ex(NULL,
								Z_ISUNDEF(us->object)? NULL : &us->object,
								&func_name,
								&retval,
								1, args, 0, NULL);
				if (call_result == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
					if (Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE) {
						ret = (Z_TYPE(retval) == IS_TRUE) ? PHP_STREAM_OPTION_RETURN_OK :
											   PHP_STREAM_OPTION_RETURN_ERR;
					} else {
						php_error_docref(NULL, E_WARNING,
								"%s::" USERSTREAM_TRUNCATE " did not return a boolean!",
								us->wrapper->classname);
					}
				} else {
					php_error_docref(NULL, E_WARNING,
							"%s::" USERSTREAM_TRUNCATE " is not implemented!",
							us->wrapper->classname);
				}
				zval_ptr_dtor(&retval);
				zval_ptr_dtor(&args[0]);
			} else { /* bad new size */
				ret = PHP_STREAM_OPTION_RETURN_ERR;
			}
			break;
		}
		}
		zval_ptr_dtor(&func_name);
		break;

	case PHP_STREAM_OPTION_READ_BUFFER:
	case PHP_STREAM_OPTION_WRITE_BUFFER:
	case PHP_STREAM_OPTION_READ_TIMEOUT:
	case PHP_STREAM_OPTION_BLOCKING: {

		ZVAL_STRINGL(&func_name, USERSTREAM_SET_OPTION, sizeof(USERSTREAM_SET_OPTION)-1);

		ZVAL_LONG(&args[0], option);
		ZVAL_NULL(&args[1]);
		ZVAL_NULL(&args[2]);

		switch(option) {
		case PHP_STREAM_OPTION_READ_BUFFER:
		case PHP_STREAM_OPTION_WRITE_BUFFER:
			ZVAL_LONG(&args[1], value);
			if (ptrparam) {
				ZVAL_LONG(&args[2], *(long *)ptrparam);
			} else {
				ZVAL_LONG(&args[2], BUFSIZ);
			}
			break;
		case PHP_STREAM_OPTION_READ_TIMEOUT: {
			struct timeval tv = *(struct timeval*)ptrparam;
			ZVAL_LONG(&args[1], tv.tv_sec);
			ZVAL_LONG(&args[2], tv.tv_usec);
			break;
			}
		case PHP_STREAM_OPTION_BLOCKING:
			ZVAL_LONG(&args[1], value);
			break;
		default:
			break;
		}

		call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			3, args, 0, NULL);

		if (call_result == FAILURE) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_SET_OPTION " is not implemented!",
					us->wrapper->classname);
			ret = PHP_STREAM_OPTION_RETURN_ERR;
		} else if (Z_TYPE(retval) != IS_UNDEF && zend_is_true(&retval)) {
			ret = PHP_STREAM_OPTION_RETURN_OK;
		} else {
			ret = PHP_STREAM_OPTION_RETURN_ERR;
		}

		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&args[2]);
		zval_ptr_dtor(&args[1]);
		zval_ptr_dtor(&args[0]);
		zval_ptr_dtor(&func_name);

		break;
		}
	}

	return ret;
}


static int user_wrapper_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	zval zfuncname, zretval;
	zval args[1];
	int call_result;
	zval object;
	int ret = 0;

	/* create an instance of our class */
	user_stream_create_object(uwrap, context, &object);
	if (Z_TYPE(object) == IS_UNDEF) {
		return ret;
	}

	/* call the unlink method */
	ZVAL_STRING(&args[0], url);

	ZVAL_STRING(&zfuncname, USERSTREAM_UNLINK);

	call_result = call_user_function_ex(NULL,
			&object,
			&zfuncname,
			&zretval,
			1, args,
			0, NULL	);

	if (call_result == SUCCESS && (Z_TYPE(zretval) == IS_FALSE || Z_TYPE(zretval) == IS_TRUE)) {
		ret = (Z_TYPE(zretval) == IS_TRUE);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_UNLINK " is not implemented!", uwrap->classname);
 	}

	/* clean up */
	zval_ptr_dtor(&object);
	zval_ptr_dtor(&zretval);
	zval_ptr_dtor(&zfuncname);

	zval_ptr_dtor(&args[0]);

	return ret;
}

static int user_wrapper_rename(php_stream_wrapper *wrapper, const char *url_from, const char *url_to,
							   int options, php_stream_context *context)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	zval zfuncname, zretval;
	zval args[2];
	int call_result;
	zval object;
	int ret = 0;

	/* create an instance of our class */
	user_stream_create_object(uwrap, context, &object);
	if (Z_TYPE(object) == IS_UNDEF) {
		return ret;
	}

	/* call the rename method */
	ZVAL_STRING(&args[0], url_from);
	ZVAL_STRING(&args[1], url_to);

	ZVAL_STRING(&zfuncname, USERSTREAM_RENAME);

	call_result = call_user_function_ex(NULL,
			&object,
			&zfuncname,
			&zretval,
			2, args,
			0, NULL	);

	if (call_result == SUCCESS && (Z_TYPE(zretval) == IS_FALSE || Z_TYPE(zretval) == IS_TRUE)) {
		ret = (Z_TYPE(zretval) == IS_TRUE);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_RENAME " is not implemented!", uwrap->classname);
 	}

	/* clean up */
	zval_ptr_dtor(&object);
	zval_ptr_dtor(&zretval);

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	return ret;
}

static int user_wrapper_mkdir(php_stream_wrapper *wrapper, const char *url, int mode,
							  int options, php_stream_context *context)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	zval zfuncname, zretval;
	zval args[3];
	int call_result;
	zval object;
	int ret = 0;

	/* create an instance of our class */
	user_stream_create_object(uwrap, context, &object);
	if (Z_TYPE(object) == IS_UNDEF) {
		return ret;
	}

	/* call the mkdir method */
	ZVAL_STRING(&args[0], url);
	ZVAL_LONG(&args[1], mode);
	ZVAL_LONG(&args[2], options);

	ZVAL_STRING(&zfuncname, USERSTREAM_MKDIR);

	call_result = call_user_function_ex(NULL,
			&object,
			&zfuncname,
			&zretval,
			3, args,
			0, NULL	);

	if (call_result == SUCCESS && (Z_TYPE(zretval) == IS_FALSE || Z_TYPE(zretval) == IS_TRUE)) {
		ret = (Z_TYPE(zretval) == IS_TRUE);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_MKDIR " is not implemented!", uwrap->classname);
 	}

	/* clean up */
	zval_ptr_dtor(&object);
	zval_ptr_dtor(&zretval);

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[2]);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	return ret;
}

static int user_wrapper_rmdir(php_stream_wrapper *wrapper, const char *url,
							  int options, php_stream_context *context)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	zval zfuncname, zretval;
	zval args[2];
	int call_result;
	zval object;
	int ret = 0;

	/* create an instance of our class */
	user_stream_create_object(uwrap, context, &object);
	if (Z_TYPE(object) == IS_UNDEF) {
		return ret;
	}

	/* call the rmdir method */
	ZVAL_STRING(&args[0], url);
	ZVAL_LONG(&args[1], options);

	ZVAL_STRING(&zfuncname, USERSTREAM_RMDIR);

	call_result = call_user_function_ex(NULL,
			&object,
			&zfuncname,
			&zretval,
			2, args,
			0, NULL	);

	if (call_result == SUCCESS && (Z_TYPE(zretval) == IS_FALSE || Z_TYPE(zretval) == IS_TRUE)) {
		ret = (Z_TYPE(zretval) == IS_TRUE);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_RMDIR " is not implemented!", uwrap->classname);
 	}

	/* clean up */
	zval_ptr_dtor(&object);
	zval_ptr_dtor(&zretval);

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	return ret;
}

static int user_wrapper_metadata(php_stream_wrapper *wrapper, const char *url, int option,
								 void *value, php_stream_context *context)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	zval zfuncname, zretval;
	zval args[3];
	int call_result;
	zval object;
	int ret = 0;

	switch(option) {
		case PHP_STREAM_META_TOUCH:
			array_init(&args[2]);
			if(value) {
				struct utimbuf *newtime = (struct utimbuf *)value;
				add_index_long(&args[2], 0, newtime->modtime);
				add_index_long(&args[2], 1, newtime->actime);
			}
			break;
		case PHP_STREAM_META_GROUP:
		case PHP_STREAM_META_OWNER:
		case PHP_STREAM_META_ACCESS:
			ZVAL_LONG(&args[2], *(long *)value);
			break;
		case PHP_STREAM_META_GROUP_NAME:
		case PHP_STREAM_META_OWNER_NAME:
			ZVAL_STRING(&args[2], value);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown option %d for " USERSTREAM_METADATA, option);
			zval_ptr_dtor(&args[2]);
			return ret;
	}

	/* create an instance of our class */
	user_stream_create_object(uwrap, context, &object);
	if (Z_TYPE(object) == IS_UNDEF) {
		zval_ptr_dtor(&args[2]);
		return ret;
	}

	/* call the mkdir method */
	ZVAL_STRING(&args[0], url);
	ZVAL_LONG(&args[1], option);

	ZVAL_STRING(&zfuncname, USERSTREAM_METADATA);

	call_result = call_user_function_ex(NULL,
			&object,
			&zfuncname,
			&zretval,
			3, args,
			0, NULL	);

	if (call_result == SUCCESS && (Z_TYPE(zretval) == IS_FALSE || Z_TYPE(zretval) == IS_TRUE)) {
		ret = Z_TYPE(zretval) == IS_TRUE;
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_METADATA " is not implemented!", uwrap->classname);
 	}

	/* clean up */
	zval_ptr_dtor(&object);
	zval_ptr_dtor(&zretval);

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[2]);

	return ret;
}


static int user_wrapper_stat_url(php_stream_wrapper *wrapper, const char *url, int flags,
								 php_stream_statbuf *ssb, php_stream_context *context)
{
	struct php_user_stream_wrapper *uwrap = (struct php_user_stream_wrapper*)wrapper->abstract;
	zval zfuncname, zretval;
	zval args[2];
	int call_result;
	zval object;
	int ret = -1;

	/* create an instance of our class */
	user_stream_create_object(uwrap, context, &object);
	if (Z_TYPE(object) == IS_UNDEF) {
		return ret;
	}

	/* call it's stat_url method - set up params first */
	ZVAL_STRING(&args[0], url);
	ZVAL_LONG(&args[1], flags);

	ZVAL_STRING(&zfuncname, USERSTREAM_STATURL);

	call_result = call_user_function_ex(NULL,
			&object,
			&zfuncname,
			&zretval,
			2, args,
			0, NULL	);

	if (call_result == SUCCESS && Z_TYPE(zretval) == IS_ARRAY) {
		/* We got the info we needed */
		if (SUCCESS == statbuf_from_array(&zretval, ssb))
			ret = 0;
	} else {
		if (call_result == FAILURE) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_STATURL " is not implemented!",
					uwrap->classname);
		}
	}

	/* clean up */
	zval_ptr_dtor(&object);
	zval_ptr_dtor(&zretval);

	zval_ptr_dtor(&zfuncname);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);

	return ret;

}

static size_t php_userstreamop_readdir(php_stream *stream, char *buf, size_t count)
{
	zval func_name;
	zval retval;
	int call_result;
	size_t didread = 0;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	php_stream_dirent *ent = (php_stream_dirent*)buf;

	/* avoid problems if someone mis-uses the stream */
	if (count != sizeof(php_stream_dirent))
		return 0;

	ZVAL_STRINGL(&func_name, USERSTREAM_DIR_READ, sizeof(USERSTREAM_DIR_READ)-1);

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL,
			0, NULL);

	if (call_result == SUCCESS && Z_TYPE(retval) != IS_FALSE && Z_TYPE(retval) != IS_TRUE) {
		convert_to_string(&retval);
		PHP_STRLCPY(ent->d_name, Z_STRVAL(retval), sizeof(ent->d_name), Z_STRLEN(retval));

		didread = sizeof(php_stream_dirent);
	} else if (call_result == FAILURE) {
		php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_DIR_READ " is not implemented!",
				us->wrapper->classname);
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	return didread;
}

static int php_userstreamop_closedir(php_stream *stream, int close_handle)
{
	zval func_name;
	zval retval;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	assert(us != NULL);

	ZVAL_STRINGL(&func_name, USERSTREAM_DIR_CLOSE, sizeof(USERSTREAM_DIR_CLOSE)-1);

	call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL);

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);
	zval_ptr_dtor(&us->object);
	ZVAL_UNDEF(&us->object);

	efree(us);

	return 0;
}

static int php_userstreamop_rewinddir(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs)
{
	zval func_name;
	zval retval;
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;

	ZVAL_STRINGL(&func_name, USERSTREAM_DIR_REWIND, sizeof(USERSTREAM_DIR_REWIND)-1);

	call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			0, NULL, 0, NULL);

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);

	return 0;

}

static int php_userstreamop_cast(php_stream *stream, int castas, void **retptr)
{
	php_userstream_data_t *us = (php_userstream_data_t *)stream->abstract;
	zval func_name;
	zval retval;
	zval args[1];
	php_stream * intstream = NULL;
	int call_result;
	int ret = FAILURE;

	ZVAL_STRINGL(&func_name, USERSTREAM_CAST, sizeof(USERSTREAM_CAST)-1);

	switch(castas) {
	case PHP_STREAM_AS_FD_FOR_SELECT:
		ZVAL_LONG(&args[0], PHP_STREAM_AS_FD_FOR_SELECT);
		break;
	default:
		ZVAL_LONG(&args[0], PHP_STREAM_AS_STDIO);
		break;
	}

	call_result = call_user_function_ex(NULL,
			Z_ISUNDEF(us->object)? NULL : &us->object,
			&func_name,
			&retval,
			1, args, 0, NULL);

	do {
		if (call_result == FAILURE) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_CAST " is not implemented!",
					us->wrapper->classname);
			break;
		}
		if (Z_ISUNDEF(retval) || !zend_is_true(&retval)) {
			break;
		}
		php_stream_from_zval_no_verify(intstream, &retval);
		if (!intstream) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_CAST " must return a stream resource",
					us->wrapper->classname);
			break;
		}
		if (intstream == stream) {
			php_error_docref(NULL, E_WARNING, "%s::" USERSTREAM_CAST " must not return itself",
					us->wrapper->classname);
			intstream = NULL;
			break;
		}
		ret = php_stream_cast(intstream, castas, retptr, 1);
	} while (0);

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func_name);
	zval_ptr_dtor(&args[0]);

	return ret;
}

php_stream_ops php_stream_userspace_ops = {
	php_userstreamop_write, php_userstreamop_read,
	php_userstreamop_close, php_userstreamop_flush,
	"user-space",
	php_userstreamop_seek,
	php_userstreamop_cast,
	php_userstreamop_stat,
	php_userstreamop_set_option,
};

php_stream_ops php_stream_userspace_dir_ops = {
	NULL, /* write */
	php_userstreamop_readdir,
	php_userstreamop_closedir,
	NULL, /* flush */
	"user-space-dir",
	php_userstreamop_rewinddir,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};


