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
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "ext/standard/head.h"
#include "SAPI.h"

/* output functions */
PHPAPI int (*zend_body_write)(const char *str, uint str_length);		/* string output */
PHPAPI int (*zend_header_write)(const char *str, uint str_length);	/* unbuffer string output */
static int zend_ub_body_write(const char *str, uint str_length);
static int zend_ub_body_write_no_header(const char *str, uint str_length);
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


/*
 * Main
 */

/* Start output layer */
PHPAPI void zend_output_startup()
{
	ob_buffer = NULL;
	zend_body_write = zend_ub_body_write;
	zend_header_write = sapi_module.ub_write;
}


/* Start output buffering */
void zend_start_ob_buffering()
{
	zend_ob_init(4096, 1024);
	zend_body_write = zend_b_body_write;
}


/* End output buffering */
void zend_end_ob_buffering(int send_buffer)
{
	SLS_FETCH();

	if (!ob_buffer) {
		return;
	}
	if (SG(headers_sent)) {
		zend_body_write = zend_ub_body_write_no_header;
	} else {
		zend_body_write = zend_ub_body_write;
	}
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


static void zend_ob_init(uint initial_size, uint block_size)
{
	if (ob_buffer) {
		return;
	}
	ob_block_size = block_size;
	ob_buffer_size = initial_size;
	ob_buffer = (char *) emalloc(initial_size+1);
	ob_text_length = 0;
}


static void zend_ob_destroy()
{
	if (ob_buffer) {
		efree(ob_buffer);
		ob_buffer = NULL;
	}
}


static void zend_ob_append(const char *text, uint text_length)
{
	char *target;
	int original_ob_text_length=ob_text_length;

	ob_text_length += text_length;
	zend_ob_allocate();
	target = ob_buffer+original_ob_text_length;
	memcpy(target, text, text_length);
	target[text_length]=0;
}


static void zend_ob_prepend(const char *text, uint text_length)
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


/* Return the current output buffer */
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


static int zend_ub_body_write_no_header(const char *str, uint str_length)
{
	return zend_header_write(str, str_length);
}


static int zend_ub_body_write(const char *str, uint str_length)
{
	SLS_FETCH();

	if (SG(request_info).headers_only) {
		zend_bailout();
	}
	if (php3_header()) {
		zend_body_write = zend_ub_body_write_no_header;
		return zend_header_write(str, str_length);
	} else {
		return 0;
	}
}


/*
 * HEAD support
 */

void set_header_request(int value)
{
	/* deprecated */
}
