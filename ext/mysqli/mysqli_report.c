/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli.h"

/* {{{ bool mysqli_report(int flags)
   sets report level */
PHP_FUNCTION(mysqli_report)
{
	int		flags;

	if (MyG(report_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Report level already set.");
		RETURN_FALSE;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}

	if (!flags) {
		RETURN_FALSE;
	}
	MyG(report_mode) = flags;

	RETURN_TRUE;
}
/* }}} */

/* {{{ void php_mysqli_report_error(char *sqlstate, int errorno, char *error) */ 
void php_mysqli_report_error(char *sqlstate, int errorno, char *error TSRMLS_DC) {
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error (%s/%d): %s", sqlstate, errorno, error);
}
/* }}} */

/* {{{ void php_mysqli_report_index() */ 
void php_mysqli_report_index(char *query, unsigned int status TSRMLS_DC) {
#if MYSQL_VERSION_ID > 40101
	char index[15];

	if (status & SERVER_QUERY_NO_GOOD_INDEX_USED) {
		strcpy(index, "Bad index");
	} else if (status & SERVER_QUERY_NO_INDEX_USED) {
		strcpy(index, "No index");
	} else {
		return;
	}
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s used in query %s", index, query);
#else
	return;
#endif
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
