/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
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
static void php_ob_destroy();
static void php_ob_append(const char *text, uint text_length);
static void php_ob_prepend(const char *text, uint text_length);
static inline void php_ob_send();

void php_start_ob_buffering();
void php_end_ob_buffering(int send_buffer);
int php_ob_get_buffer(pval *p);

/* HEAD support */
void set_header_request(int value);

typedef struct {
	int (*php_body_write)(const char *str, uint str_length);		/* string output */
	int (*php_header_write)(const char *str, uint str_length);	/* unbuffer string output */
	char *ob_buffer;
	uint ob_size;
	uint ob_block_size;
	uint ob_text_length;
} php_output_globals;

#ifdef ZTS
#define OUTPUT(v) (output_globals->v)
#define OUTPUTLS_FETCH() php_output_globals *output_globals = ts_resource(output_globals_id)
int output_globals_id;
#else
#define OUTPUT(v) (output_globals.v)
#define OUTPUTLS_FETCH()
php_output_globals output_globals;
#endif


PHP_FUNCTION(ob_start);
PHP_FUNCTION(ob_end_flush);
PHP_FUNCTION(ob_end_clean);
PHP_FUNCTION(ob_get_contents);


#ifdef ZTS
static void php_output_init_globals(php_output_globals *output_globals)
{
 	OUTPUT(php_body_write) = NULL;
	OUTPUT(php_header_write) = NULL;
	OUTPUT(ob_buffer) = NULL;
	OUTPUT(ob_size) = 0;
	OUTPUT(ob_block_size) = 0;
	OUTPUT(ob_text_length) = 0;
}
#endif


PHP_GINIT_FUNCTION(output)
{
#ifdef ZTS
	output_globals_id = ts_allocate_id(sizeof(php_output_globals), NULL, NULL);
#else
 	OUTPUT(php_body_write) = NULL;
	OUTPUT(php_header_write) = NULL;
	OUTPUT(ob_buffer) = NULL;
	OUTPUT(ob_size) = 0;
	OUTPUT(ob_block_size) = 0;
	OUTPUT(ob_text_length) = 0;
#endif

	return SUCCESS;
}

static zend_function_entry php_output_functions[] = {
	PHP_FE(ob_start,					NULL)
	PHP_FE(ob_end_flush,				NULL)
	PHP_FE(ob_end_clean,				NULL)
	PHP_FE(ob_get_contents,				NULL)
	{NULL, NULL, NULL}
};

PHP_RINIT_FUNCTION(output);
PHP_RSHUTDOWN_FUNCTION(output);

php3_module_entry output_module_entry = {
	"PHP_output", 
	php_output_functions, 
	NULL,                   /* extension-wide startup function */
	NULL,                   /* extension-wide shutdown function */
	PHP_RINIT(output),      /* per-request startup function */
	PHP_RSHUTDOWN(output),  /* per-request shutdown function */
	NULL,                   /* information function */
	PHP_GINIT(output),      /* global startup function */
	NULL,                   /* global shutdown function */
	STANDARD_MODULE_PROPERTIES_EX
};

PHP_RINIT_FUNCTION(output)
{
	php_output_startup();
   
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(output)
{
	/* XXX needs filling in */
	return SUCCESS;
}

/* Start output layer */
PHPAPI void php_output_startup()
{
	OUTPUTLS_FETCH();

	OUTPUT(ob_buffer) = NULL;
	OUTPUT(php_body_write) = php_ub_body_write;
	OUTPUT(php_header_write) = sapi_module.ub_write;
}

PHPAPI int php_body_write(const char *str, uint str_length)
{
	OUTPUTLS_FETCH();
	return OUTPUT(php_body_write)(str, str_length);	
}

PHPAPI int php_header_write(const char *str, uint str_length)
{
	OUTPUTLS_FETCH();
	return OUTPUT(php_header_write)(str, str_length);
}

/* Start output buffering */
PHPAPI void php_start_ob_buffering()
{
	OUTPUTLS_FETCH();

	php_ob_init(4096, 1024);
	OUTPUT(php_body_write) = php_b_body_write;
}


/* End output buffering */
PHPAPI void php_end_ob_buffering(int send_buffer)
{
	SLS_FETCH();
	OUTPUTLS_FETCH();

	if (!OUTPUT(ob_buffer)) {
		return;
	}
	if (SG(headers_sent) && !SG(request_info).headers_only) {
		OUTPUT(php_body_write) = php_ub_body_write_no_header;
	} else {
		OUTPUT(php_body_write) = php_ub_body_write;
	}
	if (send_buffer) {
		php_ob_send();
	}
	php_ob_destroy();
}


/*
 * Output buffering - implementation
 */

static inline void php_ob_allocate()
{
	OUTPUTLS_FETCH();

	if (OUTPUT(ob_size)<OUTPUT(ob_text_length)) {
		while (OUTPUT(ob_size) <= OUTPUT(ob_text_length))
			OUTPUT(ob_size)+=OUTPUT(ob_block_size);
			
		OUTPUT(ob_buffer) = (char *) erealloc(OUTPUT(ob_buffer), OUTPUT(ob_size)+1);
	}
}


static void php_ob_init(uint initial_size, uint block_size)
{
	OUTPUTLS_FETCH();

	if (OUTPUT(ob_buffer)) {
		return;
	}
	OUTPUT(ob_block_size) = block_size;
	OUTPUT(ob_size) = initial_size;
	OUTPUT(ob_buffer) = (char *) emalloc(initial_size+1);
	OUTPUT(ob_text_length) = 0;
}


static void php_ob_destroy()
{
	OUTPUTLS_FETCH();

	if (OUTPUT(ob_buffer)) {
		efree(OUTPUT(ob_buffer));
		OUTPUT(ob_buffer) = NULL;
	}
}


static void php_ob_append(const char *text, uint text_length)
{
	char *target;
	int original_ob_text_length;
	OUTPUTLS_FETCH();

	original_ob_text_length=OUTPUT(ob_text_length);

	OUTPUT(ob_text_length) += text_length;
	php_ob_allocate();
	target = OUTPUT(ob_buffer)+original_ob_text_length;
	memcpy(target, text, text_length);
	target[text_length]=0;
}


static void php_ob_prepend(const char *text, uint text_length)
{
	char *p, *start;
	OUTPUTLS_FETCH();

	OUTPUT(ob_text_length) += text_length;
	php_ob_allocate();

	/* php_ob_allocate() may change OUTPUT(ob_buffer), so we can't initialize p&start earlier */
	p = OUTPUT(ob_buffer)+OUTPUT(ob_text_length);
	start = OUTPUT(ob_buffer);

	while (--p>=start) {
		p[text_length] = *p;
	}
	memcpy(OUTPUT(ob_buffer), text, text_length);
	OUTPUT(ob_buffer)[OUTPUT(ob_text_length)]=0;
}


static inline void php_ob_send()
{
	OUTPUTLS_FETCH();

	/* header_write is a simple, unbuffered output function */
	OUTPUT(php_body_write)(OUTPUT(ob_buffer), OUTPUT(ob_text_length));
}


/* Return the current output buffer */
int php_ob_get_buffer(pval *p)
{
	OUTPUTLS_FETCH();

	if (!OUTPUT(ob_buffer)) {
		return FAILURE;
	}
	p->type = IS_STRING;
	p->value.str.val = estrndup(OUTPUT(ob_buffer), OUTPUT(ob_text_length));
	p->value.str.len = OUTPUT(ob_text_length);
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
	OUTPUTLS_FETCH();

	session_adapt_uris(str, str_length, &newstr, &new_length);
		
	if (newstr) {
		str = newstr;
		str_length = new_length;
	}

	result = OUTPUT(php_header_write)(str, str_length);

	if (newstr) {
		free(newstr);
	}

	return result;
}


static int php_ub_body_write(const char *str, uint str_length)
{
	int result = 0;
	SLS_FETCH();
	OUTPUTLS_FETCH();

	if (SG(request_info).headers_only) {
		zend_bailout();
	}
	if (php3_header()) {
		OUTPUT(php_body_write) = php_ub_body_write_no_header;
		result = php_ub_body_write_no_header(str, str_length);
	}

	return result;
}


/*
 * HEAD support
 */

void set_header_request(int value)
{
	/* deprecated */
}

PHP_FUNCTION(ob_start)
{
	php_start_ob_buffering();
}


PHP_FUNCTION(ob_end_flush)
{
	php_end_ob_buffering(1);
}


PHP_FUNCTION(ob_end_clean)
{
	php_end_ob_buffering(0);
}


PHP_FUNCTION(ob_get_contents)
{
	if (php_ob_get_buffer(return_value)==FAILURE) {
		RETURN_FALSE;
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
