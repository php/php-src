/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "ext/standard/head.h"
#include "ext/session/php_session.h"
#include "basic_functions.h"
#include "SAPI.h"

/* output functions */
static int php_ub_body_write(const char *str, uint str_length);
static int php_ub_body_write_no_header(const char *str, uint str_length);
static int php_b_body_write(const char *str, uint str_length);

static void php_ob_init(uint initial_size, uint block_size, zval *output_handler, uint chunk_size);
static void php_ob_append(const char *text, uint text_length);
#if 0
static void php_ob_prepend(const char *text, uint text_length);
#endif


#ifdef ZTS
int output_globals_id;
#else
php_output_globals output_globals;
#endif

static void php_output_init_globals(OLS_D)
{
 	OG(php_body_write) = NULL;
	OG(php_header_write) = NULL;
	OG(implicit_flush) = 0;
	OG(output_start_filename) = NULL;
	OG(output_start_lineno) = 0;
}


PHP_GINIT_FUNCTION(output)
{
#ifdef ZTS
	output_globals_id = ts_allocate_id(sizeof(php_output_globals), (ts_allocate_ctor) php_output_init_globals, NULL);
#else 
	php_output_init_globals(OLS_C);
#endif

	return SUCCESS;
}

/* Start output layer */
PHPAPI void php_output_startup()
{
	OLS_FETCH();

	OG(php_body_write) = php_ub_body_write;
	OG(php_header_write) = sapi_module.ub_write;
	OG(nesting_level) = 0;
	OG(lock) = 0;
}


void php_output_register_constants()
{
	ELS_FETCH();

	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_START", PHP_OUTPUT_HANDLER_START, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_CONT", PHP_OUTPUT_HANDLER_CONT, CONST_CS | CONST_PERSISTENT);
	REGISTER_MAIN_LONG_CONSTANT("PHP_OUTPUT_HANDLER_END", PHP_OUTPUT_HANDLER_END, CONST_CS | CONST_PERSISTENT);
}


PHPAPI int php_body_write(const char *str, uint str_length)
{
	OLS_FETCH();
	return OG(php_body_write)(str, str_length);	
}

PHPAPI int php_header_write(const char *str, uint str_length)
{
	OLS_FETCH();
	return OG(php_header_write)(str, str_length);
}

/* Start output buffering */
PHPAPI int php_start_ob_buffer(zval *output_handler, uint chunk_size)
{
	OLS_FETCH();

	if (OG(lock)) {
		return FAILURE;
	}
	if (chunk_size) {
		php_ob_init((chunk_size*3/2), chunk_size/2, output_handler, chunk_size);
	} else {
		php_ob_init(40*1024, 10*1024, output_handler, chunk_size);
	}
	OG(php_body_write) = php_b_body_write;
	return SUCCESS;
}


/* End output buffering (one level) */
PHPAPI void php_end_ob_buffer(zend_bool send_buffer, zend_bool just_flush)
{
	char *final_buffer=NULL;
	int final_buffer_length=0;
	zval *alternate_buffer=NULL;
	char *to_be_destroyed_buffer;
	SLS_FETCH();
	OLS_FETCH();

	if (OG(nesting_level)==0) {
		return;
	}

	if (OG(active_ob_buffer).output_handler) {
		zval **params[2];
		zval *orig_buffer;
		zval *z_status;
		CLS_FETCH();

		ALLOC_INIT_ZVAL(orig_buffer);
		orig_buffer->value.str.val = OG(active_ob_buffer).buffer;
		orig_buffer->value.str.len = OG(active_ob_buffer).text_length;
		orig_buffer->type = IS_STRING;
		orig_buffer->refcount=2;	/* don't let call_user_function() destroy our buffer */

		ALLOC_INIT_ZVAL(z_status);
		Z_TYPE_P(z_status) = IS_LONG;
		Z_LVAL_P(z_status) = 0;
		if (!OG(active_ob_buffer).status & PHP_OUTPUT_HANDLER_START) {
			/* our first call */
			Z_LVAL_P(z_status) |= PHP_OUTPUT_HANDLER_START;
		}
		if (just_flush) {
			Z_LVAL_P(z_status) |= PHP_OUTPUT_HANDLER_CONT;
		} else {
			Z_LVAL_P(z_status) |= PHP_OUTPUT_HANDLER_END;
		}

		params[0] = &orig_buffer;
		params[1] = &z_status;
		OG(lock) = 1;
		if (call_user_function_ex(CG(function_table), NULL, OG(active_ob_buffer).output_handler, &alternate_buffer, 2, params, 1, NULL)==SUCCESS) {
			convert_to_string_ex(&alternate_buffer);
			final_buffer = alternate_buffer->value.str.val;
			final_buffer_length = alternate_buffer->value.str.len;
		}
		OG(lock) = 0;
		zval_ptr_dtor(&OG(active_ob_buffer).output_handler);
		if (orig_buffer->refcount==2) { /* free the zval */
			FREE_ZVAL(orig_buffer);
		} else {
			orig_buffer->refcount-=2;
		}
		zval_ptr_dtor(&z_status);
	}

	if (!final_buffer) {
		final_buffer = OG(active_ob_buffer).buffer;
		final_buffer_length = OG(active_ob_buffer).text_length;
	}

	if (OG(nesting_level)==1) { /* end buffering */
		if (SG(headers_sent) && !SG(request_info).headers_only) {
			OG(php_body_write) = php_ub_body_write_no_header;
		} else {
			OG(php_body_write) = php_ub_body_write;
		}
	}

	to_be_destroyed_buffer = OG(active_ob_buffer).buffer;

	if (!just_flush) {
		if (OG(nesting_level)>1) { /* restore previous buffer */
			php_ob_buffer *ob_buffer_p;

			zend_stack_top(&OG(ob_buffers), (void **) &ob_buffer_p);
			OG(active_ob_buffer) = *ob_buffer_p;
			zend_stack_del_top(&OG(ob_buffers));
			if (OG(nesting_level)==2) { /* destroy the stack */
				zend_stack_destroy(&OG(ob_buffers));
			}
		}
	}

	if (send_buffer) {
		OG(php_body_write)(final_buffer, final_buffer_length);
	}

	if (alternate_buffer) {
		zval_ptr_dtor(&alternate_buffer);
	}

	if (!just_flush) {
		efree(to_be_destroyed_buffer);

		OG(nesting_level)--;
	} else {
		OG(active_ob_buffer).text_length = 0;
		OG(active_ob_buffer).status |= PHP_OUTPUT_HANDLER_START;
		OG(php_body_write) = php_b_body_write;
	}
}


/* End output buffering (all buffers) */
PHPAPI void php_end_ob_buffers(zend_bool send_buffer)
{
	OLS_FETCH();

	while (OG(nesting_level)!=0) {
		php_end_ob_buffer(send_buffer, 0);
	}
}


PHPAPI void php_start_implicit_flush()
{
	OLS_FETCH();

	php_end_ob_buffer(1, 0);		/* Switch out of output buffering if we're in it */
	OG(implicit_flush)=1;
}


PHPAPI void php_end_implicit_flush()
{
	OLS_FETCH();

	OG(implicit_flush)=0;
}


/*
 * Output buffering - implementation
 */

static inline void php_ob_allocate(void)
{
	OLS_FETCH();

	if (OG(active_ob_buffer).size<OG(active_ob_buffer).text_length) {
		while (OG(active_ob_buffer).size <= OG(active_ob_buffer).text_length) {
			OG(active_ob_buffer).size+=OG(active_ob_buffer).block_size;
		}
			
		OG(active_ob_buffer).buffer = (char *) erealloc(OG(active_ob_buffer).buffer, OG(active_ob_buffer).size+1);
	}
}


static void php_ob_init(uint initial_size, uint block_size, zval *output_handler, uint chunk_size)
{
	OLS_FETCH();

	if (OG(nesting_level)>0) {
		if (OG(nesting_level)==1) { /* initialize stack */
			zend_stack_init(&OG(ob_buffers));
		}
		zend_stack_push(&OG(ob_buffers), &OG(active_ob_buffer), sizeof(php_ob_buffer));
	}
	OG(nesting_level)++;
	OG(active_ob_buffer).block_size = block_size;
	OG(active_ob_buffer).size = initial_size;
	OG(active_ob_buffer).buffer = (char *) emalloc(initial_size+1);
	OG(active_ob_buffer).text_length = 0;
	OG(active_ob_buffer).output_handler = output_handler;
	OG(active_ob_buffer).chunk_size = chunk_size;
	OG(active_ob_buffer).status = 0;
}


static void php_ob_append(const char *text, uint text_length)
{
	char *target;
	int original_ob_text_length;
	OLS_FETCH();

	original_ob_text_length=OG(active_ob_buffer).text_length;
	OG(active_ob_buffer).text_length = OG(active_ob_buffer).text_length + text_length;

	php_ob_allocate();
	target = OG(active_ob_buffer).buffer+original_ob_text_length;
	memcpy(target, text, text_length);
	target[text_length]=0;

	if (OG(active_ob_buffer).chunk_size
		&& OG(active_ob_buffer).text_length >= OG(active_ob_buffer).chunk_size) {
		zval *output_handler = OG(active_ob_buffer).output_handler;

		if (output_handler) {
			output_handler->refcount++;
		}
		php_end_ob_buffer(1, 1);
		return;
	}
}

#if 0
static void php_ob_prepend(const char *text, uint text_length)
{
	char *p, *start;
	OLS_FETCH();

	OG(active_ob_buffer).text_length += text_length;
	php_ob_allocate();

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


/* Return the current output buffer */
int php_ob_get_buffer(pval *p)
{
	OLS_FETCH();

	if (OG(nesting_level)==0) {
		return FAILURE;
	}
	p->type = IS_STRING;
	p->value.str.val = estrndup(OG(active_ob_buffer).buffer, OG(active_ob_buffer).text_length);
	p->value.str.len = OG(active_ob_buffer).text_length;
	return SUCCESS;
}


/* Return the size of the current output buffer */
int php_ob_get_length(pval *p)
{
	OLS_FETCH();

	if (OG(nesting_level) == 0) {
		return FAILURE;
	}
	p->type = IS_LONG;
	p->value.lval = OG(active_ob_buffer).text_length;
	return SUCCESS;
}

/*
 * Wrapper functions - implementation
 */


/* buffered output function */
static int php_b_body_write(const char *str, uint str_length)
{
	php_ob_append(str, str_length);
	return str_length;
}


static int php_ub_body_write_no_header(const char *str, uint str_length)
{
	char *newstr = NULL;
	size_t new_length=0;
	int result;
	OLS_FETCH();
	BLS_FETCH();

	if (BG(use_trans_sid)) {
		session_adapt_uris(str, str_length, &newstr, &new_length);
	}
		
	if (newstr) {
		str = newstr;
		str_length = new_length;
	}

	result = OG(php_header_write)(str, str_length);

	if (OG(implicit_flush)) {
		sapi_flush();
	}

	return result;
}


static int php_ub_body_write(const char *str, uint str_length)
{
	int result = 0;
	SLS_FETCH();
	OLS_FETCH();

	if (SG(request_info).headers_only) {
		zend_bailout();
	}
	if (php_header()) {
		if (zend_is_compiling()) {
			CLS_FETCH();

			OG(output_start_filename) = zend_get_compiled_filename(CLS_C);
			OG(output_start_lineno) = zend_get_compiled_lineno(CLS_C);
		} else if (zend_is_executing()) {
			ELS_FETCH();

			OG(output_start_filename) = zend_get_executed_filename(ELS_C);
			OG(output_start_lineno) = zend_get_executed_lineno(ELS_C);
		}

		OG(php_body_write) = php_ub_body_write_no_header;
		result = php_ub_body_write_no_header(str, str_length);
	}

	return result;
}


/*
 * HEAD support
 */


/* {{{ proto void ob_start([ string user_function])
   Turn on Output Buffering (specifying an optional output handler). */
PHP_FUNCTION(ob_start)
{
	zval *output_handler;
	uint chunk_size=0;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			output_handler = NULL;
			break;
		case 1: {
				zval **output_handler_p;

				if (zend_get_parameters_ex(1, &output_handler_p)==FAILURE) {
					RETURN_FALSE;
				}
				SEPARATE_ZVAL(output_handler_p);
				output_handler = *output_handler_p;
				output_handler->refcount++;
			}
			break;
		case 2: {
				zval **output_handler_p, **chunk_size_p;

				if (zend_get_parameters_ex(2, &output_handler_p, &chunk_size_p)==FAILURE) {
					RETURN_FALSE;
				}
				SEPARATE_ZVAL(output_handler_p);
				output_handler = *output_handler_p;
				output_handler->refcount++;
				convert_to_long_ex(chunk_size_p);
				chunk_size = (uint) Z_LVAL_PP(chunk_size_p);
			}
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}
	if (php_start_ob_buffer(output_handler, chunk_size)==FAILURE) {
		php_error(E_WARNING, "Cannot use output buffering in output buffering display handlers");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void ob_end_flush(void)
   Flush (send) the output buffer, and turn off output buffering */
PHP_FUNCTION(ob_end_flush)
{
	php_end_ob_buffer(1, 0);
}
/* }}} */


/* {{{ proto void ob_end_clean(void)
   Clean (erase) the output buffer, and turn off output buffering */
PHP_FUNCTION(ob_end_clean)
{
	php_end_ob_buffer(0, 0);
}
/* }}} */


/* {{{ proto string ob_get_contents(void)
   Return the contents of the output buffer */
PHP_FUNCTION(ob_get_contents)
{
	if (php_ob_get_buffer(return_value)==FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string ob_get_length(void)
   Return the length of the output buffer */
PHP_FUNCTION(ob_get_length)
{
	if (php_ob_get_length(return_value)==FAILURE) {
		RETURN_FALSE;
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
			flag = (*zv_flag)->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	if (flag) {
		php_start_implicit_flush();
	} else {
		php_end_implicit_flush();
	}
}
/* }}} */


PHPAPI char *php_get_output_start_filename()
{
	OLS_FETCH();

	return OG(output_start_filename);
}


PHPAPI int php_get_output_start_lineno()
{
	OLS_FETCH();

	return OG(output_start_lineno);
}



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
