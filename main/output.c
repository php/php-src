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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "ext/standard/head.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/url_scanner_ex.h"
#include "SAPI.h"

#define OB_DEFAULT_HANDLER_NAME "default output handler"

/* output functions */
static int php_ub_body_write(const char *str, uint str_length TSRMLS_DC);
static int php_ub_body_write_no_header(const char *str, uint str_length TSRMLS_DC);
static int php_b_body_write(const char *str, uint str_length TSRMLS_DC);

static int php_ob_init(uint initial_size, uint block_size, zval *output_handler, uint chunk_size, zend_bool erase TSRMLS_DC);
static void php_ob_append(const char *text, uint text_length TSRMLS_DC);
#if 0
static void php_ob_prepend(const char *text, uint text_length);
#endif

#ifdef ZTS
int output_globals_id;
#else
php_output_globals output_globals;
#endif

/* {{{ php_default_output_func */
static inline int php_default_output_func(const char *str, uint str_len TSRMLS_DC)
{
	fwrite(str, 1, str_len, stderr);
	return str_len;
}
/* }}} */

/* {{{ php_output_init_globals */
static void php_output_init_globals(php_output_globals *output_globals_p TSRMLS_DC)
{
 	OG(php_body_write) = php_default_output_func;
	OG(php_header_write) = php_default_output_func;
	OG(implicit_flush) = 0;
	OG(output_start_filename) = NULL;
	OG(output_start_lineno) = 0;
}
/* }}} */


/* {{{ php_output_startup
   Start output layer */
PHPAPI void php_output_startup(void)
{
#ifdef ZTS
	ts_allocate_id(&output_globals_id, sizeof(php_output_globals), (ts_allocate_ctor) php_output_init_globals, NULL);
#else 
	php_output_init_globals(&output_globals TSRMLS_CC);
#endif
}
/* }}} */


/* {{{ php_output_activate
   Initilize output global for activation */
PHPAPI void php_output_activate(TSRMLS_D)
{
	OG(php_body_write) = php_ub_body_write;
	OG(php_header_write) = sapi_module.ub_write;
	OG(ob_nesting_level) = 0;
	OG(ob_lock) = 0;
	OG(disable_output) = 0;
	OG(output_start_filename) = NULL;
	OG(output_start_lineno) = 0;
}
/* }}} */


/* {{{ php_output_set_status
   Toggle output status.  Do NOT use in application code, only in SAPIs where appropriate. */
PHPAPI void php_output_set_status(zend_bool status TSRMLS_DC)
{
	OG(disable_output) = !status;
}
/* }}} */

/* {{{ php_output_register_constants */
void php_output_register_constants(TSRMLS_D)
{
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_START", PHP_OUTPUT_HANDLER_START, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_CONT", PHP_OUTPUT_HANDLER_CONT, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_END", PHP_OUTPUT_HANDLER_END, CONST_CS | CONST_PERSISTENT);
}
/* }}} */


/* {{{ php_body_wirte
 * Write body part */
PHPAPI int php_body_write(const char *str, uint str_length TSRMLS_DC)
{
	return OG(php_body_write)(str, str_length TSRMLS_CC);	
}
/* }}} */

/* {{{ php_header_wirte
 * Write HTTP header */
PHPAPI int php_header_write(const char *str, uint str_length TSRMLS_DC)
{
	if (OG(disable_output)) {
		return 0;
	} else {
		return OG(php_header_write)(str, str_length TSRMLS_CC);
	}
}
/* }}} */

/* {{{ php_start_ob_buffer
 * Start output buffering */
PHPAPI int php_start_ob_buffer(zval *output_handler, uint chunk_size, zend_bool erase TSRMLS_DC)
{
	uint initial_size, block_size;

	if (OG(ob_lock)) {
		if (SG(headers_sent) && !SG(request_info).headers_only) {
			OG(php_body_write) = php_ub_body_write_no_header;
		} else {
			OG(php_body_write) = php_ub_body_write;
		}
		OG(ob_nesting_level) = 0;
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_ERROR, "Cannot use output buffering in output buffering display handlers");
		return FAILURE;
	}
	if (chunk_size) {
		if (chunk_size==1)
			chunk_size = 4096;
		initial_size = (chunk_size*3/2);
		block_size = chunk_size/2;
	} else {
		initial_size = 40*1024;
		block_size = 10*1024;
	}
	return php_ob_init(initial_size, block_size, output_handler, chunk_size, erase TSRMLS_CC);
}
/* }}} */

/* {{{ php_start_ob_buffer_named
 * Start output buffering */
PHPAPI int php_start_ob_buffer_named(const char *output_handler_name, uint chunk_size, zend_bool erase TSRMLS_DC)
{
	zval *output_handler;
	int result;

	ALLOC_INIT_ZVAL(output_handler);
	Z_STRLEN_P(output_handler) = strlen(output_handler_name);	/* this can be optimized */
	Z_STRVAL_P(output_handler) = estrndup(output_handler_name, Z_STRLEN_P(output_handler));
	Z_TYPE_P(output_handler) = IS_STRING;
	result = php_start_ob_buffer(output_handler, chunk_size, erase TSRMLS_CC);
	zval_dtor(output_handler);
	FREE_ZVAL(output_handler);
	return result;
}
/* }}} */

/* {{{ php_end_ob_buffer
 * End output buffering (one level) */
PHPAPI void php_end_ob_buffer(zend_bool send_buffer, zend_bool just_flush TSRMLS_DC)
{
	char *final_buffer=NULL;
	unsigned int final_buffer_length=0;
	zval *alternate_buffer=NULL;
	char *to_be_destroyed_buffer, *to_be_destroyed_handler_name;
	char *to_be_destroyed_handled_output[2] = { 0, 0 };
	int status;
	php_ob_buffer *prev_ob_buffer_p=NULL;
	php_ob_buffer orig_ob_buffer;

	if (OG(ob_nesting_level)==0) {
		return;
	}
	status = 0;
	if (!OG(active_ob_buffer).status & PHP_OUTPUT_HANDLER_START) {
		/* our first call */
		status |= PHP_OUTPUT_HANDLER_START;
	}
	if (just_flush) {
		status |= PHP_OUTPUT_HANDLER_CONT;
	} else {
		status |= PHP_OUTPUT_HANDLER_END;
	}

#if 0
 {
	 FILE *fp;
	 fp = fopen("/tmp/ob_log", "a");
	 fprintf(fp, "NestLevel: %d  ObStatus: %d  HandlerName: %s\n", OG(ob_nesting_level), status, OG(active_ob_buffer).handler_name);
	 fclose(fp);
 }
#endif
	
	if (OG(active_ob_buffer).internal_output_handler) {
		final_buffer = OG(active_ob_buffer).internal_output_handler_buffer;
		final_buffer_length = OG(active_ob_buffer).internal_output_handler_buffer_size;
		OG(active_ob_buffer).internal_output_handler(OG(active_ob_buffer).buffer, OG(active_ob_buffer).text_length, &final_buffer, &final_buffer_length, status TSRMLS_CC);
	} else if (OG(active_ob_buffer).output_handler) {
		zval **params[2];
		zval *orig_buffer;
		zval *z_status;

		ALLOC_INIT_ZVAL(orig_buffer);
		ZVAL_STRINGL(orig_buffer, OG(active_ob_buffer).buffer, OG(active_ob_buffer).text_length, 1);
		orig_buffer->refcount=2;	/* don't let call_user_function() destroy our buffer */
		orig_buffer->is_ref=1;

		ALLOC_INIT_ZVAL(z_status);
		ZVAL_LONG(z_status, status);

		params[0] = &orig_buffer;
		params[1] = &z_status;
		OG(ob_lock) = 1;
		if (call_user_function_ex(CG(function_table), NULL, OG(active_ob_buffer).output_handler, &alternate_buffer, 2, params, 1, NULL TSRMLS_CC)==SUCCESS) {
			if (!(Z_TYPE_P(alternate_buffer)==IS_BOOL && Z_BVAL_P(alternate_buffer)==0)) {
				convert_to_string_ex(&alternate_buffer);
				final_buffer = Z_STRVAL_P(alternate_buffer);
				final_buffer_length = Z_STRLEN_P(alternate_buffer);
			}
		}
		OG(ob_lock) = 0;
		zval_ptr_dtor(&OG(active_ob_buffer).output_handler);
		orig_buffer->refcount -=2;
		if (orig_buffer->refcount <= 0) { /* free the zval */
			zval_dtor(orig_buffer);
			FREE_ZVAL(orig_buffer);
		}
		zval_ptr_dtor(&z_status);
	}

	if (!final_buffer) {
		final_buffer = OG(active_ob_buffer).buffer;
		final_buffer_length = OG(active_ob_buffer).text_length;
	}

	if (OG(ob_nesting_level)==1) { /* end buffering */
		if (SG(headers_sent) && !SG(request_info).headers_only) {
			OG(php_body_write) = php_ub_body_write_no_header;
		} else {
			OG(php_body_write) = php_ub_body_write;
		}
	}

	to_be_destroyed_buffer = OG(active_ob_buffer).buffer;
	to_be_destroyed_handler_name = OG(active_ob_buffer).handler_name;
	if (OG(active_ob_buffer).internal_output_handler
		&& (final_buffer != OG(active_ob_buffer).internal_output_handler_buffer)
		&& (final_buffer != OG(active_ob_buffer).buffer)) {
		to_be_destroyed_handled_output[0] = final_buffer;
	}

	if (!just_flush) {
		if (OG(active_ob_buffer).internal_output_handler) {
			to_be_destroyed_handled_output[1] = OG(active_ob_buffer).internal_output_handler_buffer;
		}
	}
	if (OG(ob_nesting_level)>1) { /* restore previous buffer */
		zend_stack_top(&OG(ob_buffers), (void **) &prev_ob_buffer_p);
		orig_ob_buffer = OG(active_ob_buffer);
		OG(active_ob_buffer) = *prev_ob_buffer_p;
		zend_stack_del_top(&OG(ob_buffers));
		if (!just_flush && OG(ob_nesting_level)==2) { /* destroy the stack */
			zend_stack_destroy(&OG(ob_buffers));
		}
	}
	OG(ob_nesting_level)--;

	if (send_buffer) {
		OG(php_body_write)(final_buffer, final_buffer_length TSRMLS_CC);
	}

	if (just_flush) { /* we restored the previous ob, return to the current */
		if (prev_ob_buffer_p) {
			zend_stack_push(&OG(ob_buffers), &OG(active_ob_buffer), sizeof(php_ob_buffer));
			OG(active_ob_buffer) = orig_ob_buffer;
		}
		OG(ob_nesting_level)++;
	}

	if (alternate_buffer) {
		zval_ptr_dtor(&alternate_buffer);
	}

	if (status & PHP_OUTPUT_HANDLER_END) {
		efree(to_be_destroyed_handler_name);
	}
	if (!just_flush) {
		efree(to_be_destroyed_buffer);
	} else {
		OG(active_ob_buffer).text_length = 0;
		OG(active_ob_buffer).status |= PHP_OUTPUT_HANDLER_START;
		OG(php_body_write) = php_b_body_write;
	}
	if (to_be_destroyed_handled_output[0]) {
		efree(to_be_destroyed_handled_output[0]);
	}
	if (to_be_destroyed_handled_output[1]) {
		efree(to_be_destroyed_handled_output[1]);
	}
}
/* }}} */

/* {{{ php_end_ob_buffers
 * End output buffering (all buffers) */
PHPAPI void php_end_ob_buffers(zend_bool send_buffer TSRMLS_DC)
{
	while (OG(ob_nesting_level)!=0) {
		php_end_ob_buffer(send_buffer, 0 TSRMLS_CC);
	}
}
/* }}} */

/* {{{ php_start_implicit_flush
 */
PHPAPI void php_start_implicit_flush(TSRMLS_D)
{
	OG(implicit_flush)=1;
}
/* }}} */

/* {{{ php_end_implicit_flush
 */
PHPAPI void php_end_implicit_flush(TSRMLS_D)
{
	OG(implicit_flush)=0;
}
/* }}} */

/* {{{ php_ob_set_internal_handler
 */
PHPAPI void php_ob_set_internal_handler(php_output_handler_func_t internal_output_handler, uint buffer_size, char *handler_name, zend_bool erase TSRMLS_DC)
{
	if (OG(ob_nesting_level)==0 || OG(active_ob_buffer).internal_output_handler || strcmp(OG(active_ob_buffer).handler_name, OB_DEFAULT_HANDLER_NAME)) {
		php_start_ob_buffer(NULL, buffer_size, erase TSRMLS_CC);
	}

	OG(active_ob_buffer).internal_output_handler = internal_output_handler;
	OG(active_ob_buffer).internal_output_handler_buffer = (char *) emalloc(buffer_size);
	OG(active_ob_buffer).internal_output_handler_buffer_size = buffer_size;
 	if (OG(active_ob_buffer).handler_name)
		efree(OG(active_ob_buffer).handler_name);
	OG(active_ob_buffer).handler_name = estrdup(handler_name);
	OG(active_ob_buffer).erase = erase;
}
/* }}} */

/*
 * Output buffering - implementation
 */

/* {{{ php_ob_allocate
 */
static inline void php_ob_allocate(TSRMLS_D)
{
	if (OG(active_ob_buffer).size<OG(active_ob_buffer).text_length) {
		while (OG(active_ob_buffer).size <= OG(active_ob_buffer).text_length) {
			OG(active_ob_buffer).size+=OG(active_ob_buffer).block_size;
		}
			
		OG(active_ob_buffer).buffer = (char *) erealloc(OG(active_ob_buffer).buffer, OG(active_ob_buffer).size+1);
	}
}
/* }}} */

/* {{{ php_ob_init_conflict
 * Returns 1 if handler_set is already used and generates error message
 */
PHPAPI int php_ob_init_conflict(char *handler_new, char *handler_set TSRMLS_DC)
{
	if (php_ob_handler_used(handler_set TSRMLS_CC))
	{
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "output handler '%s' conflicts with '%s'", handler_new, handler_set);
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ php_ob_init_named
 */
static int php_ob_init_named(uint initial_size, uint block_size, char *handler_name, zval *output_handler, uint chunk_size, zend_bool erase TSRMLS_DC)
{
	if (OG(ob_nesting_level)>0) {
		if (OG(ob_nesting_level)==1) { /* initialize stack */
			zend_stack_init(&OG(ob_buffers));
		}
		zend_stack_push(&OG(ob_buffers), &OG(active_ob_buffer), sizeof(php_ob_buffer));
	}
	OG(ob_nesting_level)++;
	OG(active_ob_buffer).block_size = block_size;
	OG(active_ob_buffer).size = initial_size;
	OG(active_ob_buffer).buffer = (char *) emalloc(initial_size+1);
	OG(active_ob_buffer).text_length = 0;
	OG(active_ob_buffer).output_handler = output_handler;
	OG(active_ob_buffer).chunk_size = chunk_size;
	OG(active_ob_buffer).status = 0;
	OG(active_ob_buffer).internal_output_handler = NULL;
	OG(active_ob_buffer).handler_name = estrdup(handler_name&&handler_name[0]?handler_name:OB_DEFAULT_HANDLER_NAME);
	OG(active_ob_buffer).erase = erase;	
	OG(php_body_write) = php_b_body_write;
	return SUCCESS;
}
/* }}} */

/* {{{ php_ob_handler_from_string
 * Create zval output handler from string 
 */
static zval* php_ob_handler_from_string(const char *handler_name TSRMLS_DC)
{
	zval *output_handler;

	ALLOC_INIT_ZVAL(output_handler);
	Z_STRLEN_P(output_handler) = strlen(handler_name);	/* this can be optimized */
	Z_STRVAL_P(output_handler) = estrndup(handler_name, Z_STRLEN_P(output_handler));
	Z_TYPE_P(output_handler) = IS_STRING;
	return output_handler;
}
/* }}} */

/* {{{ php_ob_init
 */
static int php_ob_init(uint initial_size, uint block_size, zval *output_handler, uint chunk_size, zend_bool erase TSRMLS_DC)
{
	int res, result, len;
	char *handler_name, *next_handler_name;
	HashPosition pos;
	zval **tmp;
	zval *handler_zval;

	if (output_handler && output_handler->type == IS_STRING) {
		result = 0;
		handler_name = Z_STRVAL_P(output_handler);
		while ((next_handler_name=strchr(handler_name, ',')) != NULL) {
			len = next_handler_name-handler_name;
			next_handler_name = estrndup(handler_name, len);
			handler_zval = php_ob_handler_from_string(next_handler_name TSRMLS_CC);
			res = php_ob_init_named(initial_size, block_size, next_handler_name, handler_zval, chunk_size, erase TSRMLS_CC);
			result &= res;
			if (!res==SUCCESS) {
				zval_dtor(handler_zval);
				FREE_ZVAL(handler_zval);
			}
			handler_name += len+1;
			efree(next_handler_name);
		}
		handler_zval = php_ob_handler_from_string(handler_name TSRMLS_CC);
		res = php_ob_init_named(initial_size, block_size, handler_name, handler_zval, chunk_size, erase TSRMLS_CC);
		result &= res;
		if (!res==SUCCESS) {
			zval_dtor(handler_zval);
			FREE_ZVAL(handler_zval);
		}
		result = result ? SUCCESS : FAILURE;
	}
	else if (output_handler && output_handler->type == IS_ARRAY) {
		result = 0;
		/* do we have array(object,method) */
		if (zend_is_callable(output_handler, 1, &handler_name)) {
			SEPARATE_ZVAL(&output_handler);
			output_handler->refcount++;
			result = php_ob_init_named(initial_size, block_size, handler_name, output_handler, chunk_size, erase TSRMLS_CC);
			efree(handler_name);
		} else {
			/* init all array elements recursively */
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(output_handler), &pos);
			while (zend_hash_get_current_data_ex(Z_ARRVAL_P(output_handler), (void **)&tmp, &pos) == SUCCESS) {
				result &= php_ob_init(initial_size, block_size, *tmp, chunk_size, erase TSRMLS_CC);
				zend_hash_move_forward_ex(Z_ARRVAL_P(output_handler), &pos);
			}
		}
		result = result ? SUCCESS : FAILURE;
	}
	else if (output_handler && output_handler->type == IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "No method name given: use ob_start(array($object,'method')) to specify instance $object and the name of a method of class %s to use as output handler", Z_OBJCE_P(output_handler)->name);
		result = FAILURE;
	}
	else {
		if (output_handler) {
			SEPARATE_ZVAL(&output_handler);
			output_handler->refcount++;
		}
		result = php_ob_init_named(initial_size, block_size, OB_DEFAULT_HANDLER_NAME, output_handler, chunk_size, erase TSRMLS_CC);
	}
	return result;
}
/* }}} */

/* {{{ php_ob_list_each
 */
static int php_ob_list_each(php_ob_buffer *ob_buffer, zval *ob_handler_array) 
{
	add_next_index_string(ob_handler_array, ob_buffer->handler_name, 1);
	return 0;
}
/* }}} */

/* {{{ proto false|array ob_list_handlers()
 *  List all output_buffers in an array 
 */
PHP_FUNCTION(ob_list_handlers)
{
	if (ZEND_NUM_ARGS()!=0) {
		WRONG_PARAM_COUNT;
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_ERROR, "Unable to initialize array");
		RETURN_FALSE;
	}
	if (OG(ob_nesting_level)) {
		if (OG(ob_nesting_level)>1) {
			zend_stack_apply_with_argument(&OG(ob_buffers), ZEND_STACK_APPLY_BOTTOMUP, (int (*)(void *element, void *)) php_ob_list_each, return_value);
		}
		php_ob_list_each(&OG(active_ob_buffer), return_value);
	}
}
/* }}} */

/* {{{ php_ob_used_each
 *  Sets handler_name to NULL is found
 */
static int php_ob_handler_used_each(php_ob_buffer *ob_buffer, char **handler_name) 
{
	if (!strcmp(ob_buffer->handler_name, *handler_name))
	{
		*handler_name = NULL;
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ php_ob_used
 * returns 1 if given handler_name is used as output_handler
 */
PHPAPI int php_ob_handler_used(char *handler_name TSRMLS_DC)
{
	char *tmp = handler_name;

	if (OG(ob_nesting_level)) {
		if (!strcmp(OG(active_ob_buffer).handler_name, handler_name)) {
			return 1;
		}
		if (OG(ob_nesting_level)>1) {
			zend_stack_apply_with_argument(&OG(ob_buffers), ZEND_STACK_APPLY_BOTTOMUP, (int (*)(void *element, void *)) php_ob_handler_used_each, &tmp);
		}
	}
	return tmp ? 0 : 1;
}
/* }}} */

/* {{{ php_ob_append
 */
static inline void php_ob_append(const char *text, uint text_length TSRMLS_DC)
{
	char *target;
	int original_ob_text_length;

	original_ob_text_length=OG(active_ob_buffer).text_length;
	OG(active_ob_buffer).text_length = OG(active_ob_buffer).text_length + text_length;

	php_ob_allocate(TSRMLS_C);
	target = OG(active_ob_buffer).buffer+original_ob_text_length;
	memcpy(target, text, text_length);
	target[text_length]=0;

 	/* If implicit_flush is On or chunked buffering, send contents to next buffer and return. */
	if (OG(active_ob_buffer).chunk_size
		&& OG(active_ob_buffer).text_length >= OG(active_ob_buffer).chunk_size) {
		zval *output_handler = OG(active_ob_buffer).output_handler;
		
		if (output_handler) {
			output_handler->refcount++;
		}
		php_end_ob_buffer(1, 1 TSRMLS_CC);
		return;
	}
}
/* }}} */

#if 0
static inline void php_ob_prepend(const char *text, uint text_length)
{
	char *p, *start;
	TSRMLS_FETCH();

	OG(active_ob_buffer).text_length += text_length;
	php_ob_allocate(TSRMLS_C);

	/* php_ob_allocate() may change OG(ob_buffer), so we can't initialize p&start earlier */
	p = OG(ob_buffer)+OG(ob_text_length);
	start = OG(ob_buffer);

	while (--p>=start) {
		p[text_length] = *p;
	}
	memcpy(OG(ob_buffer), text, text_length);
	OG(ob_buffer)[OG(active_ob_buffer).text_length]=0;
}
#endif


/* {{{ php_ob_get_buffer
 * Return the current output buffer */
PHPAPI int php_ob_get_buffer(zval *p TSRMLS_DC)
{
	if (OG(ob_nesting_level)==0) {
		return FAILURE;
	}
	ZVAL_STRINGL(p, OG(active_ob_buffer).buffer, OG(active_ob_buffer).text_length, 1);
	return SUCCESS;
}
/* }}} */

/* {{{ php_ob_get_length
 * Return the size of the current output buffer */
PHPAPI int php_ob_get_length(zval *p TSRMLS_DC)
{
	if (OG(ob_nesting_level) == 0) {
		return FAILURE;
	}
	ZVAL_LONG(p, OG(active_ob_buffer).text_length);
	return SUCCESS;
}
/* }}} */

/*
 * Wrapper functions - implementation
 */


/* buffered output function */
static int php_b_body_write(const char *str, uint str_length TSRMLS_DC)
{
	php_ob_append(str, str_length TSRMLS_CC);
	return str_length;
}

/* {{{ php_ub_body_write_no_header
 */
static int php_ub_body_write_no_header(const char *str, uint str_length TSRMLS_DC)
{
	int result;

	if (OG(disable_output)) {
		return 0;
	}		

	result = OG(php_header_write)(str, str_length TSRMLS_CC);

	if (OG(implicit_flush)) {
		sapi_flush(TSRMLS_C);
	}

	return result;
}
/* }}} */

/* {{{ php_ub_body_write
 */
static int php_ub_body_write(const char *str, uint str_length TSRMLS_DC)
{
	int result = 0;

	if (SG(request_info).headers_only) {
		php_header();
		zend_bailout();
	}
	if (php_header()) {
		if (zend_is_compiling(TSRMLS_C)) {
			OG(output_start_filename) = zend_get_compiled_filename(TSRMLS_C);
			OG(output_start_lineno) = zend_get_compiled_lineno(TSRMLS_C);
		} else if (zend_is_executing(TSRMLS_C)) {
			OG(output_start_filename) = zend_get_executed_filename(TSRMLS_C);
			OG(output_start_lineno) = zend_get_executed_lineno(TSRMLS_C);
		}

		OG(php_body_write) = php_ub_body_write_no_header;
		result = php_ub_body_write_no_header(str, str_length TSRMLS_CC);
	}

	return result;
}
/* }}} */

/*
 * HEAD support
 */

/* {{{ proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
   Turn on Output Buffering (specifying an optional output handler). */
PHP_FUNCTION(ob_start)
{
	zval *output_handler=NULL;
	uint chunk_size=0;
	zend_bool erase=1;
	int argc = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|zlb", &output_handler, 
							  &chunk_size, &erase) == FAILURE)
		RETURN_FALSE;

	if (php_start_ob_buffer(output_handler, chunk_size, erase TSRMLS_CC)==FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_flush(void)
   Flush (send) contents of the output buffer. The last buffer content is sent to next buffer */
PHP_FUNCTION(ob_flush)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;

	if (!OG(ob_nesting_level)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to flush buffer. No buffer to flush.");
		RETURN_FALSE;
	}
	
	php_end_ob_buffer(1, 1 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool ob_clean(void)
   Clean (delete) the current output buffer */
PHP_FUNCTION(ob_clean)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;

	
	if (!OG(ob_nesting_level)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer. No buffer to delete.");
		RETURN_FALSE;
	}
	if (!OG(active_ob_buffer).status && !OG(active_ob_buffer).erase) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer %s.", OG(active_ob_buffer).handler_name);
		RETURN_FALSE;
	}
	
	php_end_ob_buffer(0, 1 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_end_flush(void)
   Flush (send) the output buffer, and delete current output buffer */
PHP_FUNCTION(ob_end_flush)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;
	
	if (!OG(ob_nesting_level)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete and flush buffer. No buffer to delete or flush.");
		RETURN_FALSE;
	}
	if (OG(ob_nesting_level) && !OG(active_ob_buffer).status && !OG(active_ob_buffer).erase) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer %s.", OG(active_ob_buffer).handler_name);
		RETURN_FALSE;
	}
	
	php_end_ob_buffer(1, 0 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_end_clean(void)
   Clean the output buffer, and delete current output buffer */
PHP_FUNCTION(ob_end_clean)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;
		
	if (!OG(ob_nesting_level)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer. No buffer to delete.");
		RETURN_FALSE;
	}
	if (OG(ob_nesting_level) && !OG(active_ob_buffer).status && !OG(active_ob_buffer).erase) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer %s.", OG(active_ob_buffer).handler_name);
		RETURN_FALSE;
	}
	
	php_end_ob_buffer(0, 0 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ob_get_flush(void)
   Get current buffer contents, flush (send) the output buffer, and delete current output buffer */
PHP_FUNCTION(ob_get_flush)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;

	/* get contents */
	if (php_ob_get_buffer(return_value TSRMLS_CC)==FAILURE) {
		RETURN_FALSE;
	}
	/* error checks */
	if (!OG(ob_nesting_level)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete and flush buffer. No buffer to delete or flush.");
		RETURN_FALSE;
	}
	if (OG(ob_nesting_level) && !OG(active_ob_buffer).status && !OG(active_ob_buffer).erase) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer %s.", OG(active_ob_buffer).handler_name);
		RETURN_FALSE;
	}
	/* flush */
	php_end_ob_buffer(1, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool ob_get_clean(void)
   Get current buffer contents and delete current output buffer */
PHP_FUNCTION(ob_get_clean)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;
		
	/* get contents */
	if (php_ob_get_buffer(return_value TSRMLS_CC)==FAILURE) {
		RETURN_FALSE;
	}
	/* error checks */
	if (!OG(ob_nesting_level)) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer. No buffer to delete.");
		RETURN_FALSE;
	}
	if (OG(ob_nesting_level) && !OG(active_ob_buffer).status && !OG(active_ob_buffer).erase) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_NOTICE, "failed to delete buffer %s.", OG(active_ob_buffer).handler_name);
		RETURN_FALSE;
	}
	/* delete buffer */
	php_end_ob_buffer(0, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto string ob_get_contents(void)
   Return the contents of the output buffer */
PHP_FUNCTION(ob_get_contents)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;
		
	if (php_ob_get_buffer(return_value TSRMLS_CC)==FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int ob_get_level(void)
   Return the nesting level of the output buffer */
PHP_FUNCTION(ob_get_level)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;
		
	RETURN_LONG (OG(ob_nesting_level));
}
/* }}} */

/* {{{ proto string ob_get_length(void)
   Return the length of the output buffer */
PHP_FUNCTION(ob_get_length)
{
	if (ZEND_NUM_ARGS() != 0)
			WRONG_PARAM_COUNT;
		
	if (php_ob_get_length(return_value TSRMLS_CC)==FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ int php_ob_buffer_status(php_ob_buffer *ob_buffer, zval *result) */
static int php_ob_buffer_status(php_ob_buffer *ob_buffer, zval *result) 
{
	zval *elem;

	MAKE_STD_ZVAL(elem);
	if (array_init(elem) == FAILURE) {
		return FAILURE;
	}

	add_assoc_long(elem, "chunk_size", ob_buffer->chunk_size);
	if (!ob_buffer->chunk_size) {
		add_assoc_long(elem, "size", ob_buffer->size);
		add_assoc_long(elem, "block_size", ob_buffer->block_size);
	}
	if (ob_buffer->internal_output_handler) {
		add_assoc_long(elem, "type", PHP_OUTPUT_HANDLER_INTERNAL);
		add_assoc_long(elem, "buffer_size", ob_buffer->internal_output_handler_buffer_size);
	}
	else {
		add_assoc_long(elem, "type", PHP_OUTPUT_HANDLER_USER);
	}
	add_assoc_long(elem, "status", ob_buffer->status);
	add_assoc_string(elem, "name", ob_buffer->handler_name, 1);
	add_assoc_bool(elem, "del", ob_buffer->erase);
	add_next_index_zval(result, elem);

	return SUCCESS;
}
/* }}} */


/* {{{ proto false|array ob_get_status([bool full_status])
   Return the status of the active or all output buffers */
PHP_FUNCTION(ob_get_status)
{
	int argc = ZEND_NUM_ARGS();
	zend_bool full_status = 0;
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|b", &full_status) == FAILURE )
		RETURN_FALSE;
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	if (full_status) {
		if (OG(ob_nesting_level)>1) {
			zend_stack_apply_with_argument(&OG(ob_buffers), ZEND_STACK_APPLY_BOTTOMUP, (int (*)(void *elem, void *))php_ob_buffer_status, return_value);
		}
		if (OG(ob_nesting_level)>0 && php_ob_buffer_status(&OG(active_ob_buffer), return_value)==FAILURE) {
			RETURN_FALSE;
		}
	}
	else if (OG(ob_nesting_level)>0) {
		add_assoc_long(return_value, "level", OG(ob_nesting_level));
		if (OG(active_ob_buffer).internal_output_handler) {
			add_assoc_long(return_value, "type", PHP_OUTPUT_HANDLER_INTERNAL);
		}
		else {
			add_assoc_long(return_value, "type", PHP_OUTPUT_HANDLER_USER);
		}
		add_assoc_long(return_value, "status", OG(active_ob_buffer).status);
		add_assoc_string(return_value, "name", OG(active_ob_buffer).handler_name, 1);
		add_assoc_bool(return_value, "del", OG(active_ob_buffer).erase);
	}
}
/* }}} */


/* {{{ proto void ob_implicit_flush([int flag])
   Turn implicit flush on/off and is equivalent to calling flush() after every output call */
PHP_FUNCTION(ob_implicit_flush)
{
	zval **zv_flag;
	int flag;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			flag = 1;
			break;
		case 1:
			if (zend_get_parameters_ex(1, &zv_flag)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(zv_flag);
			flag = Z_LVAL_PP(zv_flag);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	if (flag) {
		php_start_implicit_flush(TSRMLS_C);
	} else {
		php_end_implicit_flush(TSRMLS_C);
	}
}
/* }}} */


/* {{{ char *php_get_output_start_filename(TSRMLS_D)
   Return filename start output something */
PHPAPI char *php_get_output_start_filename(TSRMLS_D)
{
	return OG(output_start_filename);
}
/* }}} */


/* {{{ char *php_get_output_start_lineno(TSRMLS_D)
   Return line number start output something */
PHPAPI int php_get_output_start_lineno(TSRMLS_D)
{
	return OG(output_start_lineno);
}
/* }}} */


/* {{{ proto bool output_reset_rewrite_vars(void)
   Reset(clear) URL rewriter values */
PHP_FUNCTION(output_reset_rewrite_vars)
{
	if (php_url_scanner_reset_vars(TSRMLS_C) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool output_add_rewrite_var(string name, string value)
   Add URL rewriter values */
PHP_FUNCTION(output_add_rewrite_var)
{
	char *name, *value;
	int name_len, value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (php_url_scanner_add_var(name, name_len, value, value_len, 1 TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
