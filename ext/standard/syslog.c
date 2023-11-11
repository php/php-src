/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#ifdef HAVE_SYSLOG_H
#include "php_ini.h"
#include "zend_globals.h"

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>
#include "basic_functions.h"
#include "php_ext_syslog.h"

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(syslog)
{
	BG(syslog_device)=NULL;

	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(syslog)
{
	php_closelog();
	if (BG(syslog_device)) {
		free(BG(syslog_device));
		BG(syslog_device) = NULL;
	}
	return SUCCESS;
}


/* {{{ Open connection to system logger */
/*
   ** OpenLog("nettopp", $LOG_PID, $LOG_LOCAL1);
   ** Syslog($LOG_EMERG, "help me!")
   ** CloseLog();
 */
PHP_FUNCTION(openlog)
{
	char *ident;
	zend_long option, facility;
	size_t ident_len;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STRING(ident, ident_len)
		Z_PARAM_LONG(option)
		Z_PARAM_LONG(facility)
	ZEND_PARSE_PARAMETERS_END();

	if (BG(syslog_device)) {
		free(BG(syslog_device));
	}
	BG(syslog_device) = zend_strndup(ident, ident_len);
	php_openlog(BG(syslog_device), option, facility);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Close connection to system logger */
PHP_FUNCTION(closelog)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_closelog();
	if (BG(syslog_device)) {
		free(BG(syslog_device));
		BG(syslog_device)=NULL;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Generate a system log message */
PHP_FUNCTION(syslog)
{
	zend_long priority;
	zend_string *message;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(priority)
		Z_PARAM_STR(message)
	ZEND_PARSE_PARAMETERS_END();

	php_syslog_str(priority, message);
	RETURN_TRUE;
}
/* }}} */

#endif
