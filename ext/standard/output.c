/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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
   |          Thies C. Arntzen <thies@digicol.de>                         |
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "ext/standard/head.h"
#include "ext/session/php_session.h"
#include "SAPI.h"

/* output functions */
static int php_ub_body_write(const char *str, uint str_length);
static int php_ub_body_write_no_header(const char *str, uint str_length);
static int php_b_body_write(const char *str, uint str_length);

static void php_ob_init(uint initial_size, uint block_size);
static void php_ob_destroy(void);
static void php_ob_append(const char *text, uint text_length);
#if 0
static void php_ob_prepend(const char *text, uint text_length);
#endif
static inline void php_ob_send(void);


#ifdef ZTS
int output_globals_id;
#else
php_output_globals output_globals;
#endif

static void php_output_init_globals(OLS_D)
{
 	OG(php_body_write) = NULL;
	OG(php_header_write) = NULL;
	OG(ob_buffer) = NULL;
	OG(ob_size) = 0;
	OG(ob_block_size) = 0;
	OG(ob_text_length) = 0;
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

	OG(ob_buffer) = NULL;
	OG(php_body_write) = php_ub_body_write;
	OG(php_header_write) = sapi_module.ub_write;
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
PHPAPI void php_start_ob_buffering()
{
	OLS_FETCH();

	php_ob_init(4096, 1024);
	OG(php_body_write) = php_b_body_write;
}


/* End output buffering */
PHPAPI void php_end_ob_buffering(int send_buffer)
{
	SLS_FETCH();
	OLS_FETCH();

	if (!OG(ob_buffer)) {
		return;
	}
	if (SG(headers_sent) && !SG(request_info).headers_only) {
		OG(php_body_write) = php_ub_body_write_no_header;
	} else {
		OG(php_body_write) = php_ub_body_write;
	}
	if (send_buffer) {
		php_ob_send();
	}
	php_ob_destroy();
}


PHPAPI void php_start_implicit_flush()
{
	OLS_FETCH();

	php_end_ob_buffering(1);		/* Switch out of output buffering if we're in it */
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

	if (OG(ob_size)<OG(ob_text_length)) {
		while (OG(ob_size) <= OG(ob_text_length))
			OG(ob_size)+=OG(ob_block_size);
			
		OG(ob_buffer) = (char *) erealloc(OG(ob_buffer), OG(ob_size)+1);
	}
}


static void php_ob_init(uint initial_size, uint block_size)
{
	OLS_FETCH();

	if (OG(ob_buffer)) {
		return;
	}
	OG(ob_block_size) = block_size;
	OG(ob_size) = initial_size;
	OG(ob_buffer) = (char *) emalloc(initial_size+1);
	OG(ob_text_length) = 0;
}


static void php_ob_destroy()
{
	OLS_FETCH();

	if (OG(ob_buffer)) {
		efree(OG(ob_buffer));
		OG(ob_buffer) = NULL;
	}
}


static void php_ob_append(const char *text, uint text_length)
{
	char *target;
	int original_ob_text_length;
	OLS_FETCH();

	original_ob_text_length=OG(ob_text_length);

	OG(ob_text_length) += text_length;
	php_ob_allocate();
	target = OG(ob_buffer)+original_ob_text_length;
	memcpy(target, text, text_length);
	target[text_length]=0;
}

#if 0
static void php_ob_prepend(const char *text, uint text_length)
{
	char *p, *start;
	OLS_FETCH();

	OG(ob_text_length) += text_length;
	php_ob_allocate();

	/* php_ob_allocate() may change OG(ob_buffer), so we can't initialize p&start earlier */
	p = OG(ob_buffer)+OG(ob_text_length);
	start = OG(ob_buffer);

	while (--p>=start) {
		p[text_length] = *p;
	}
	memcpy(OG(ob_buffer), text, text_length);
	OG(ob_buffer)[OG(ob_text_length)]=0;
}
#endif

static inline void php_ob_send()
{
	OLS_FETCH();

	/* header_write is a simple, unbuffered output function */
	OG(php_body_write)(OG(ob_buffer), OG(ob_text_length));
}


/* Return the current output buffer */
int php_ob_get_buffer(pval *p)
{
	OLS_FETCH();

	if (!OG(ob_buffer)) {
		return FAILURE;
	}
	p->type = IS_STRING;
	p->value.str.val = estrndup(OG(ob_buffer), OG(ob_text_length));
	p->value.str.len = OG(ob_text_length);
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
	uint new_length=0;
	int result;
	OLS_FETCH();

	session_adapt_uris(str, str_length, &newstr, &new_length);
		
	if (newstr) {
		str = newstr;
		str_length = new_length;
	}

	result = OG(php_header_write)(str, str_length);

	if (newstr) {
		free(newstr);
	}

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


/* {{{ proto void ob_start(void)
   Turn on Output Buffering */
PHP_FUNCTION(ob_start)
{
	php_start_ob_buffering();
}
/* }}} */


/* {{{ proto void ob_end_flush(void)
   Flush (send) the output buffer, and turn off output buffering */
PHP_FUNCTION(ob_end_flush)
{
	php_end_ob_buffering(1);
}
/* }}} */


/* {{{ proto void ob_end_clean(void)
   Clean (erase) the output buffer, and turn off output buffering */
PHP_FUNCTION(ob_end_clean)
{
	php_end_ob_buffering(0);
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
