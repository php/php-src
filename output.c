/* 
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | The contents of this source file is the sole property of Andi        |
   | Gutmans and Zeev Suraski, and may not be distributed without prior   |
   | written permission from both of them.                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "ext/standard/head.h"

/* output functions */
int (*zend_body_write)(const char *str, uint str_length);		/* string output */
int (*zend_header_write)(const char *str, uint str_length);	/* unbuffer string output */
static int zend_ub_body_write(const char *str, uint str_length);
static int zend_b_body_write(const char *str, uint str_length);

/* output buffering */
static char *ob_buffer;
static uint ob_buffer_size;
static uint ob_block_size;
static uint ob_text_length;
void zend_ob_init(uint initial_size, uint block_size);
void zend_ob_destroy();
void zend_ob_append(const char *text, uint text_length);
void zend_ob_prepend(const char *text, uint text_length);
static inline void zend_ob_send();

/* HEAD support  */
static int header_request;

/* wrappers */
#if APACHE
static int zend_apache_ub_write(const char *str, uint str_length);
#elif CGI_BINARY
static int zend_cgibin_ub_write(const char *str, uint str_length);
#endif


/*
 * Main
 */

void zend_output_startup()
{
	ob_buffer = NULL;
	zend_body_write = zend_ub_body_write;
	header_request=0;
#if APACHE
	zend_header_write = zend_apache_ub_write;
#elif CGI_BINARY
	zend_header_write = zend_cgibin_ub_write;
#endif
}


void zend_start_ob_buffering()
{
	zend_ob_init(4096, 1024);
	zend_body_write = zend_b_body_write;
}


void zend_end_ob_buffering(int send_buffer)
{
	if (!ob_buffer) {
		return;
	}
	zend_body_write = zend_ub_body_write;
	if (send_buffer) {
		zend_ob_send();
	}
	zend_ob_destroy();
}


/*
 * Output buffering - implementation
 */

static inline void zend_ob_allocate()
{
	if (ob_buffer_size<ob_text_length) {
		while ((ob_buffer_size+=ob_block_size) < ob_text_length);
		ob_buffer = (char *) erealloc(ob_buffer, ob_buffer_size+1);
	}
}


void zend_ob_init(uint initial_size, uint block_size)
{
	if (ob_buffer) {
		return;
	}
	ob_block_size = block_size;
	ob_buffer_size = initial_size;
	ob_buffer = (char *) emalloc(initial_size+1);
	ob_text_length = 0;
}


void zend_ob_destroy()
{
	if (ob_buffer) {
		efree(ob_buffer);
		ob_buffer = NULL;
	}
}


void zend_ob_append(const char *text, uint text_length)
{
	char *target;
	int original_ob_text_length=ob_text_length;

	ob_text_length += text_length;
	zend_ob_allocate();
	target = ob_buffer+original_ob_text_length;
	memcpy(target, text, text_length);
	target[text_length]=0;
}


void zend_ob_prepend(const char *text, uint text_length)
{
	char *p, *start;

	ob_text_length += text_length;
	zend_ob_allocate();

	/* zend_ob_allocate() may change ob_buffer, so we can't initialize p&start earlier */
	p = ob_buffer+ob_text_length;
	start = ob_buffer;

	while (--p>=start) {
		p[text_length] = *p;
	}
	memcpy(ob_buffer, text, text_length);
	ob_buffer[ob_text_length]=0;
}


static inline void zend_ob_send()
{
	/* header_write is a simple, unbuffered output function */
	zend_body_write(ob_buffer, ob_text_length);
}


int zend_ob_get_buffer(pval *p)
{
	if (!ob_buffer) {
		return FAILURE;
	}
	p->type = IS_STRING;
	p->value.str.val = estrndup(ob_buffer, ob_text_length);
	p->value.str.len = ob_text_length;
	return SUCCESS;
}


/*
 * Wrapper functions - implementation
 */


/* buffered output function */
static int zend_b_body_write(const char *str, uint str_length)
{
	zend_ob_append(str, str_length);
	return str_length;
}


static int zend_ub_body_write(const char *str, uint str_length)
{
	if (header_request) {
		zend_bailout();
	}
	if (php3_header()) {
		return zend_header_write(str, str_length);
	} else {
		return 0;
	}
}


#if APACHE

static int zend_apache_ub_write(const char *str, uint str_length)
{
	if (php3_rqst) {
		return rwrite(str, str_length, php3_rqst);
	} else {
		return fwrite(str, 1, str_length, stdout);
	}
}

#elif CGI_BINARY

static int zend_cgibin_ub_write(const char *str, uint str_length)
{
	return fwrite(str, 1, str_length, stdout);
}



#endif


/*
 * HEAD support
 */

void set_header_request(int value)
{
	header_request = value;
}
