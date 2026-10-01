/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_mysqli_structs.h"

extern void php_mysqli_throw_sql_exception(char *sqlstate, int errorno, char *format, ...);

/* {{{ sets report level */
PHP_FUNCTION(mysqli_report)
{
	zend_long flags;


	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	MyG(report_mode) = flags;

	RETURN_TRUE;
}
/* }}} */

/* {{{ void php_mysqli_report_error(char *sqlstate, int errorno, char *error) */
void php_mysqli_report_error(const char *sqlstate, int errorno, const char *error)
{
	php_mysqli_throw_sql_exception((char *)sqlstate, errorno, "%s", error);
}
/* }}} */

/* {{{ void php_mysqli_report_index() */
void php_mysqli_report_index(const char *query, unsigned int status) {
	const char *index;

	if (status & SERVER_QUERY_NO_GOOD_INDEX_USED) {
		index = "Bad index";
	} else if (status & SERVER_QUERY_NO_INDEX_USED) {
		index = "No index";
	} else {
		return;
	}
	php_mysqli_throw_sql_exception("00000", 0, "%s used in query/prepared statement %s", index, query);
}
/* }}} */
