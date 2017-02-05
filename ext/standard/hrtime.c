/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Niklas Keller <kelunik@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#include "hrtime.h"
#include "timer.h"

/* {{{ */
PHP_MINIT_FUNCTION(hrtime)
{
	if (timer_lib_initialize()) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to initialize internal timer");
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ */
PHP_MSHUTDOWN_FUNCTION(hrtime)
{
	timer_lib_shutdown();

	return SUCCESS;
}
/* }}} */

/* {{{ proto mixed hrtime()
   Returns an integer containing the current high-resolution time in nanoseconds
   counted from an arbitrary point in time */
PHP_FUNCTION(hrtime)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	// Use double to avoid integer overflows as we don't know the order of magnitude
	RETURN_LONG((uint64_t) ((double) 1000000000 * timer_current() / timer_ticks_per_second()));
}
/* }}} */
