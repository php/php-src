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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#include "php_apache.h"

PHP_FUNCTION(apache_sub_req)
{
	zval **p1;
	request_req *rr;
	php_struct *ctx;
	SLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &p1) == FAILURE)
		WRONG_NUM_ARGS;

	convert_to_string_ex(p1);

	ctx = SG(server_context);
	rr = ap_sub_req_lookup_uri(Z_STRVAL_PP(p1), ctx->f->r);
	if (rr->status == HTTP_OK) {
		ap_run_sub_req(rr);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

