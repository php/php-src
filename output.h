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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _OUTPUT_BUFFER
#define _OUTPUT_BUFFER

#include "php.h"

PHPAPI void zend_output_startup();

/* exported output functions */
PHPAPI int (*zend_body_write)(const char *str, uint str_length);		/* string output */
PHPAPI int (*zend_header_write)(const char *str, uint str_length);	/* unbuffer string output */
void zend_start_ob_buffering();
void zend_end_ob_buffering(int send_buffer);
int zend_ob_get_buffer(pval *p);

/* HEAD support */
void set_header_request(int value);

#endif /* _OUTPUT_BUFFER */
