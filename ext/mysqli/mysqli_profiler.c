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

/* {{{ PR_COMMON *php_mysqli_profiler_new_object(PR_COMMON *parent, unsigned int type, unsigned int settime) */
PR_COMMON *php_mysqli_profiler_new_object(PR_COMMON *parent, unsigned int type, unsigned int settime)
{
	PR_COMMON	*prnew, *child;

	switch (type) {
		case MYSQLI_PR_MYSQL:
			prnew = (PR_COMMON *)ecalloc(1, sizeof(PR_MYSQL));
		break;
		case MYSQLI_PR_QUERY:
			prnew = (PR_COMMON *)ecalloc(1, sizeof(PR_QUERY));
		break;
		case MYSQLI_PR_STMT:
			prnew = (PR_COMMON *)ecalloc(1, sizeof(PR_STMT));
		break;
		case MYSQLI_PR_COMMAND:
			prnew = (PR_COMMON *)ecalloc(1, sizeof(PR_COMMAND));
		break; 
		case MYSQLI_PR_RESULT:
			prnew = (PR_COMMON *)ecalloc(1, sizeof(PR_RESULT));
		break; 
	}
	prnew->header.type = type;
	prnew->header.filename = estrdup(zend_get_executed_filename(TSRMLS_C));
	prnew->header.lineno = zend_get_executed_lineno(TSRMLS_C);
	prnew->header.functionname = estrdup(get_active_function_name(TSRMLS_C));
	if (settime) {
		gettimeofday(&prnew->header.starttime, NULL);
	}

	if (!parent) {
		return prnew;
	}

	if (!parent->header.child) {
		parent->header.child = (void *)prnew;
		return (prnew);
	}
	child = parent->header.child;
	while (child->header.next != NULL) {
		child = child->header.next;
	}
	child->header.next = (void *)prnew;
	return (prnew);
}
/* }}} */

/* {{{ int *php_mysqli_profiler_explain(PR_EXPLAIN *, PR_HEADER *, MYSQL *, char *) */
int php_mysqli_profiler_explain(PR_EXPLAIN *explain, PR_HEADER *header, MYSQL *mysql, char *query)
{
	MYSQL_RES 		*res;
	MYSQL_ROW 		row;
	unsigned int	i;
	char *newquery = (char *)emalloc(strlen(query) + 10);

	sprintf (newquery, "EXPLAIN %s", query);

	mysql_real_query(mysql, newquery, strlen(newquery));
	efree (newquery);

	if (mysql_errno(mysql)) {
		header->error = mysql_errno(mysql);
		header->errormsg = my_estrdup(mysql_error(mysql));
		return 0;
	}

	res = mysql_store_result(mysql);

	if (!(explain->exp_cnt = mysql_num_rows(res))) {
		return 0;
	}

	explain->exp_table = (char **)emalloc(sizeof(char *) * explain->exp_cnt);	
	explain->exp_type = (char **)emalloc(sizeof(char *) * explain->exp_cnt);	
	explain->exp_key = (char **)emalloc(sizeof(char *) * explain->exp_cnt);	
	explain->exp_rows = (ulong *)emalloc(sizeof(ulong) * explain->exp_cnt);	

	for (i=0; i < explain->exp_cnt; i++) {
		row = mysql_fetch_row(res);
		explain->exp_table[i] = my_estrdup(row[2]);
		explain->exp_type[i]  = my_estrdup(row[3]);
		explain->exp_key[i]   = my_estrdup(row[4]);
		explain->exp_rows[i]  = atol(row[8]);
	}
	
	mysql_free_result(res);
	return 1;
}
/* }}} */

/* {{{ void php_mysqli_profiler_timediff(struct timeval, struct timeval *) */
void php_mysqli_profiler_timediff(struct timeval starttime, struct timeval *elapsed)
{
	struct timeval	end;

	gettimeofday(&end, NULL);

	elapsed->tv_sec = end.tv_sec - starttime.tv_sec;
	elapsed->tv_usec = end.tv_usec - starttime.tv_usec;
	if (elapsed->tv_usec < 0) {
		--(elapsed->tv_sec);
		elapsed->tv_usec = 1000000;
	}
	return;
}
/* }}} */

/* {{{ char *php_mysqli_profiler_indent(int) */
char *php_mysqli_profiler_indent(int i)
{
	char	*ret = (char *)ecalloc(i*4+1, sizeof(char));
	memset(ret, 0x20, i*4);
	return ret;
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_header(PR_HEADER, char *) */
void php_mysqli_profiler_report_header(PR_HEADER header, char *ident)
{
	switch (header.type) {
		case MYSQLI_PR_MYSQL:
			printf("%s[Connection]\n", ident);
			php_mysqli_profiler_timediff(header.starttime, &header.lifetime);
			break;
		case MYSQLI_PR_QUERY:
			printf("%s[Query]\n", ident);
			break;
		case MYSQLI_PR_STMT:
			printf("%s[Statement]\n", ident);
			php_mysqli_profiler_timediff(header.starttime, &header.lifetime);
			break;
		case MYSQLI_PR_RESULT:
			printf("%s[Resultset]\n", ident);
			php_mysqli_profiler_timediff(header.starttime, &header.lifetime);
			break;
		case MYSQLI_PR_COMMAND:
			printf("%s[Command]\n", ident);
			break;
	}
	printf ("%sFunction: %s\n", ident, header.functionname);
	printf ("%sFile: %s\n", ident, header.filename);
	printf ("%sLine: %d\n", ident, header.lineno);
	printf ("%sExecution time: %ld.%06ld\n", ident, header.elapsedtime.tv_sec, header.elapsedtime.tv_usec);
	if (header.lifetime.tv_sec + header.lifetime.tv_usec) {
		printf ("%sLife time: %ld.%06ld\n", ident, header.lifetime.tv_sec, header.lifetime.tv_usec);
	}
	if (header.error) {
		printf("%sError: %s (%ld)\n", ident, header.errormsg, header.error);
	}

	/* free header */
	my_efree(header.functionname);
	my_efree(header.filename);
	my_efree(header.errormsg);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_explain(PR_EXPLAIN, char *) */
void php_mysqli_profiler_report_explain(PR_EXPLAIN explain, char *ident)
{
	int	i;

	if (explain.query) {
		printf("%sQuery: %s\n", ident, explain.query);
	}

	if (explain.exp_cnt) {
		printf("%sTable(s):", ident);
		for (i=0; i < explain.exp_cnt; i++) {
			printf(" %s%c", explain.exp_table[i], (i == explain.exp_cnt - 1) ? '\n' : ',');
			my_efree(explain.exp_table[i]);
		}
		printf("%sJoin-Types(s):", ident);
		for (i=0; i < explain.exp_cnt; i++) {
			printf(" %s%c", explain.exp_type[i], (i == explain.exp_cnt - 1) ? '\n' : ',');
			my_efree(explain.exp_type[i]);
		}
		printf("%sKey(s):", ident);
		for (i=0; i < explain.exp_cnt; i++) {
			printf(" %s%c", explain.exp_key[i], (i == explain.exp_cnt - 1) ? '\n' : ',');
			my_efree(explain.exp_key[i]);
		}
		printf("%sRow(s):", ident);
		for (i=0; i < explain.exp_cnt; i++) {
			printf(" %ld%c", explain.exp_rows[i], (i == explain.exp_cnt - 1) ? '\n' : ',');
		}
		my_efree(explain.exp_table);
		my_efree(explain.exp_type);
		my_efree(explain.exp_key);
		my_efree(explain.exp_rows);
	}
	/* free explain */
	my_efree(explain.query);
}
/* }}} */

/* {{{ php_mysqli_profiler_report_mysql(PR_MYSQL *, int) */
void php_mysqli_profiler_report_mysql(PR_MYSQL *prmysql, int depth)
{
	char 			*ident = php_mysqli_profiler_indent(depth);

	php_mysqli_profiler_report_header(prmysql->header, ident);

	printf ("%sHost: %s\n", ident, prmysql->hostname);
	printf ("%sUser: %s\n", ident, prmysql->username);
	printf ("%sThread-id: %d\n", ident, prmysql->thread_id);
	if (!prmysql->closed) {
		printf ("%sWarning: connection wasn't closed by mysqli_close()\n", ident);
	}
	printf("\n");

	my_efree(prmysql->hostname);
	my_efree(prmysql->username);

	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_query(PR_QUERY *, int) */
void php_mysqli_profiler_report_query(PR_QUERY *prquery, int depth)
{
	char 			*ident = php_mysqli_profiler_indent(depth);

	php_mysqli_profiler_report_header(prquery->header, ident);
	php_mysqli_profiler_report_explain(prquery->explain, ident);

	if (prquery->affectedrows > 0) {
		printf ("%saffected rows: %ld\n", ident, prquery->affectedrows);
	}
	if (prquery->insertid) {
		printf ("%sinsert id: %ld\n", ident, prquery->insertid);
	}
	

	printf("\n");
	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_stmt(PR_STMT *, int) */
void php_mysqli_profiler_report_stmt(PR_STMT *prstmt, int depth)
{
	char 			*ident = php_mysqli_profiler_indent(depth);

	php_mysqli_profiler_report_header(prstmt->header, ident);
	php_mysqli_profiler_report_explain(prstmt->explain, ident);

	printf("\n");
	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_result(PR_RESULT *, int) */
void php_mysqli_profiler_report_result(PR_RESULT *prresult, int depth)
{
	char 			*ident = php_mysqli_profiler_indent(depth);

	php_mysqli_profiler_report_header(prresult->header, ident);

	printf ("%sColumns: %d\n", ident, prresult->columns);
	printf ("%sRows: %ld\n", ident, prresult->rows);
	printf ("%sFetched rows: %ld\n", ident, prresult->fetched_rows);
	if (!prresult->closed) {
		printf ("%sWarning: resultset wasn't closed by mysqli_free_result()\n", ident);
	}
	printf("\n");
	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_command(PR_COMMAND *, int) */
void php_mysqli_profiler_report_command(PR_COMMAND *prcommand, int depth)
{
	char 			*ident = php_mysqli_profiler_indent(depth);

	php_mysqli_profiler_report_header(prcommand->header, ident);
	if (prcommand->returnvalue) {
		printf("%sReturnvalue: %s\n", ident, prcommand->returnvalue);
		efree(prcommand->returnvalue);
	}
	printf("\n");
	efree(ident);
}
/* }}} */

/* {{{ php_mysqli_profiler_report(PR_COMMON *, int) */
void php_mysqli_profiler_report(PR_COMMON *current, int depth)
{
	PR_COMMON	*child;
	PR_COMMON	*next;
	switch (current->header.type) {
		case MYSQLI_PR_MYSQL:
		{
			PR_MYSQL *prmysql = (PR_MYSQL *)current;
			php_mysqli_profiler_report_mysql(prmysql, depth);
		}
		break;
		case MYSQLI_PR_COMMAND:
		{
			PR_COMMAND *prcommand = (PR_COMMAND *)current;
			child = NULL;
			php_mysqli_profiler_report_command(prcommand, depth);
		}
		break;
		case MYSQLI_PR_RESULT:
		{
			PR_RESULT *prresult = (PR_RESULT *)current;
			php_mysqli_profiler_report_result(prresult, depth);
		}
		break;
		case MYSQLI_PR_STMT:
		case MYSQLI_PR_STMT_RESULT:
		{
			PR_STMT *prstmt = (PR_STMT *)current;
			php_mysqli_profiler_report_stmt(prstmt, depth);
		}
		break;
		case MYSQLI_PR_QUERY:
		case MYSQLI_PR_QUERY_RESULT:
		{
			PR_QUERY *prquery = (PR_QUERY *)current;
			php_mysqli_profiler_report_query(prquery, depth);
		}
		break;
	}
	child = current->header.child;
	if (child) {
		php_mysqli_profiler_report(child, depth+1);
	}

	next = (current->header.next) ? current->header.next : NULL;
	
	if (next) {
		php_mysqli_profiler_report(next, depth);
	}
	efree(current);
	return;
}
/* }}} */

/* {{{ proto void mysqli_profiler (bool profiler)
*/
PHP_FUNCTION(mysqli_profiler)
{ 
	int flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}
	MyG(profiler) = flags;
	
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
