/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
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

#define DIVIDER  "************************************************************"
#define DIVIDER1 "------------------------------------------------------------"

/* {{{ void php_mysqli_profiler_header(char *query) */
void php_mysqli_profiler_header(char *query)
{
	printf("%s\n", DIVIDER);
	printf("File: %s\nLine: %d\n", zend_get_executed_filename(TSRMLS_C), zend_get_executed_lineno(TSRMLS_C));
	printf("Function: %s\n", get_active_function_name(TSRMLS_C));
	if (query) {
		printf("SQL: %s\n", query);
	}
}
/* }}} */

/* {{{ void php_mysqli_profiler_result(MYSQL_RES *) */
void php_mysqli_profiler_result_info(MYSQL_RES *res)
{
	printf("%s\nRows returned: %d\n", DIVIDER1, mysql_num_rows(res));
}
/* }}} */

/* {{{ void php_mysqli_profiler_explain(MYSQL *, char *) */
void php_mysqli_profiler_explain(MYSQL *mysql, char *query)
{
	MYSQL_RES 		*res;
	MYSQL_ROW 		row;
	MYSQL_FIELD		*fields;
	unsigned int	i;
	char *newquery = (char *)emalloc(strlen(query) + 10);
	sprintf (newquery, "EXPLAIN %s", query);

	mysql_real_query(mysql, newquery, strlen(newquery));
	efree (newquery);

	if (mysql_errno(mysql)) {
		printf ("%s\nError (%d): %s\n", DIVIDER1, mysql_errno(mysql), mysql_error(mysql));
		return;
	}

	res = mysql_use_result(mysql);

	printf ("%s\nEXPLAIN:\n", DIVIDER1);
	fields = mysql_fetch_fields(res);
	while ((row = mysql_fetch_row(res))) {
		for (i=0; i < mysql_num_fields(res); i++) {
			printf ("%20s: %s\n", fields[i].name, row[i]);
		} 
		printf("\n");
	}

	mysql_free_result(res);
	return;
}
/* }}} */

/* {{{ void php_mysqli_profiler_elapsed_time() */
void php_mysqli_profiler_elapsed_time()
{
	struct timeval	end, elapsed;

	gettimeofday(&end, NULL);

	elapsed.tv_sec = end.tv_sec - MyG(profiler.start.tv_sec);
	elapsed.tv_usec = end.tv_usec - MyG(profiler.start.tv_usec);
	if (elapsed.tv_usec < 0) {
		--(elapsed.tv_sec);
		elapsed.tv_usec = 1000000;
	}		
	printf("%s\nElapsed time: %3d.%06d seconds\n", DIVIDER1, elapsed.tv_sec, elapsed.tv_usec);
}
/* }}} */

/* {{{ proto void mysqli_set_profiler_opt (bool profiler)
*/
PHP_FUNCTION(mysqli_set_profiler_opt)
{ 
	int flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}
	MyG(profiler.active) = flags;
	
	return;
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
