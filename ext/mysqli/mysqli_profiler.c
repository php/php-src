/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
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
#include "mysqli_profiler_com.h"

/* {{{ PR_COMMON *php_mysqli_profiler_new_object(PR_COMMON *parent, unsigned int type, unsigned int settime) */
PR_COMMON *php_mysqli_profiler_new_object(PR_COMMON *parent, unsigned int type, unsigned int settime)
{
	PR_COMMON	*prnew, *child;
	TSRMLS_FETCH();

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
		default:
			return NULL; 
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
	unsigned int	i,j;
	MYSQL_FIELD		*fields;
	MYSQL_ROW		row;
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

	explain->columns = mysql_num_fields(res);

	explain->row = (PR_ROW *)safe_emalloc(sizeof(PR_ROW), explain->exp_cnt, 0);
	explain->fields = (char **)safe_emalloc(sizeof(char *), explain->columns, 0);

	fields = mysql_fetch_fields(res);

	for (j=0; j < explain->columns; j++) {
		explain->fields[j] = estrdup(fields[j].name);
	}

	for (i=0; i < explain->exp_cnt; i++) {
		explain->row[i].value = (char **)safe_emalloc(sizeof(char *), explain->columns, 0);
		row = mysql_fetch_row(res);
		for (j=0; j < explain->columns; j++) {
			explain->row[i].value[j] = my_estrdup(row[j]); 
		}
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

/* {{{ void php_mysqli_profiler_report_header(PR_HEADER, char *, char *) */
void php_mysqli_profiler_report_header(PR_HEADER header, char *ident)
{
	char	buffer[8192];
	switch (header.type) {
		case MYSQLI_PR_MYSQL:
			php_mysqli_profiler_timediff(header.starttime, &header.lifetime);
			break;
		case MYSQLI_PR_STMT:
			php_mysqli_profiler_timediff(header.starttime, &header.lifetime);
			break;
		case MYSQLI_PR_RESULT:
			php_mysqli_profiler_timediff(header.starttime, &header.lifetime);
			break;
	}
	MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "functionname", header.functionname);
	
	MYSQLI_PROFILER_ADD_STARTTAG(buffer, "fileinfo",0);
	MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "filename", header.filename);
	MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "line", header.lineno);
	MYSQLI_PROFILER_ADD_ENDTAG(buffer, "fileinfo");
	MYSQLI_PROFILER_ADD_STARTTAG(buffer, "timeinfo",0);
	MYSQLI_PROFILER_ADD_ATTR_TIME(buffer, "execution_time", header.elapsedtime.tv_sec, header.elapsedtime.tv_usec);
	if (header.lifetime.tv_sec + header.lifetime.tv_usec) {
		MYSQLI_PROFILER_ADD_ATTR_TIME(buffer, "life_time", header.lifetime.tv_sec, header.lifetime.tv_usec);
	}
	MYSQLI_PROFILER_ADD_ENDTAG(buffer, "timeinfo");
	if (header.error) {
		MYSQLI_PROFILER_ADD_STARTTAG(buffer,  "errors",0);
		MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "errno", header.error);
		MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "errmsg", header.errormsg);
		MYSQLI_PROFILER_ADD_ENDTAG(buffer, "errors");
	}

	/* free header */
	my_efree(header.functionname);
	my_efree(header.filename);
	my_efree(header.errormsg);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_explain(PR_EXPLAIN, char *, char *) */
void php_mysqli_profiler_report_explain(PR_EXPLAIN explain, char *ident)
{
	int		i, j;
	char	buffer[8192];

	if (explain.query) {
		MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "query_string", explain.query);
	}

	if (explain.exp_cnt) {
		MYSQLI_PROFILER_ADD_STARTTAG(buffer, "explain", 1);
		for (i=0; i < explain.exp_cnt; i++) {
			for (j=0; j < explain.columns; j++) {
				MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, explain.fields[j], explain.row[i].value[j]);
				my_efree(explain.row[i].value[j]);
			}
			efree(explain.row[i].value);
		}
		MYSQLI_PROFILER_ADD_ENDTAG(buffer, "explain");
	}
	efree(explain.row);
	for (j=0; j < explain.columns; j++) {
		my_efree(explain.fields[j]);
	}
	efree(explain.fields);

	/* free explain */
	my_efree(explain.query);
}
/* }}} */

/* {{{ php_mysqli_profiler_report_mysql(PR_MYSQL *, int) */
void php_mysqli_profiler_report_mysql(PR_MYSQL *prmysql, int depth)
{
	char	*ident = php_mysqli_profiler_indent(depth);
	char	buffer[8192];

	php_mysqli_profiler_report_header(prmysql->header, ident);

	MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "hostname", prmysql->hostname);
	MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "user", prmysql->username);
	MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "thread_id", prmysql->thread_id);

	if (!prmysql->closed) {
		MYSQLI_PROFILER_ADD_STARTTAG(buffer, "warnings",0);
		MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "warning", "connection wasn't closed by mysqli_close()");
		MYSQLI_PROFILER_ADD_ENDTAG(buffer, "warnings");
	}

	my_efree(prmysql->hostname);
	my_efree(prmysql->username);

	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_query(PR_QUERY *, int) */
void php_mysqli_profiler_report_query(PR_QUERY *prquery, int depth)
{
	char	buffer[8192]; 
	char	*ident = php_mysqli_profiler_indent(depth);

	php_mysqli_profiler_report_header(prquery->header, ident);
	php_mysqli_profiler_report_explain(prquery->explain, ident);

	if (prquery->affectedrows > 0) {
		MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "affected_rows", prquery->affectedrows);
	}
	if (prquery->insertid) {
		MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "insert_id", prquery->insertid);
	}
	
	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_stmt(PR_STMT *, int) */
void php_mysqli_profiler_report_stmt(PR_STMT *prstmt, int depth)
{
	char	*ident = php_mysqli_profiler_indent(depth);
	char	buffer[8192];

	buffer[0] = '\0';
	php_mysqli_profiler_report_header(prstmt->header, ident);
	php_mysqli_profiler_report_explain(prstmt->explain, ident);

	printf("\n");
	MYSQLI_PROFILER_OUT(buffer);
	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_result(PR_RESULT *, int) */
void php_mysqli_profiler_report_result(PR_RESULT *prresult, int depth)
{
	char 	*ident = php_mysqli_profiler_indent(depth);
	char	buffer[8192];

	buffer[0] = '\0';
	php_mysqli_profiler_report_header(prresult->header, ident);

	MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "columns", prresult->columns);
	MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "rows", prresult->rows);
	MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, "fetched_rows", prresult->fetched_rows);
	if (!prresult->closed || prresult->fetched_rows != prresult->rows) {
		MYSQLI_PROFILER_ADD_STARTTAG(buffer, "warnings",0);
		if (!prresult->closed) {
			MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "warning", "resultset wasn't closed by mysqli_free_result()");
		}
		if (prresult->fetched_rows != prresult->rows) {
			MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "warning", "Not all rows from resultset were fetched.");
		}
		MYSQLI_PROFILER_ADD_ENDTAG(buffer, "warnings");
	}
	efree(ident);
}
/* }}} */

/* {{{ void php_mysqli_profiler_report_command(PR_COMMAND *, int) */
void php_mysqli_profiler_report_command(PR_COMMAND *prcommand, int depth)
{
	char	*ident = php_mysqli_profiler_indent(depth);
	char	buffer[8192];

	buffer[0] = '\0';
	php_mysqli_profiler_report_header(prcommand->header, ident);

	if (prcommand->returnvalue) {
		MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, "return_value", prcommand->returnvalue);
		efree(prcommand->returnvalue);
	}
	efree(ident);
}
/* }}} */

/* {{{ php_mysqli_profiler_report(PR_COMMON *, int) */
void php_mysqli_profiler_report(PR_COMMON *current, int depth)
{
	PR_COMMON	*child;
	PR_COMMON	*next;
	char		buffer[8192];
	char		tag[50];
	char		*ident = php_mysqli_profiler_indent(depth);

	buffer[0] = '\0';

	switch (current->header.type) {
		case MYSQLI_PR_MAIN:
		{
			MYSQLI_PROFILER_ADD_STARTTAG(buffer,"?xml version=\"1.0\" ?", 0); 
			sprintf((char *)&tag, "protocol origin='PHP5' version='%2.1f'", MYSQLI_PROFILER_PROTOCOL_VERSION);
			MYSQLI_PROFILER_ADD_STARTTAG(buffer, tag, 0);
			strcpy (tag, "protocol");
		}
		break;
		case MYSQLI_PR_MYSQL:
		{
			PR_MYSQL *prmysql = (PR_MYSQL *)current;
			strcpy(tag, "connection");
			MYSQLI_PROFILER_ADD_STARTTAG(buffer, tag, 1);
			php_mysqli_profiler_report_mysql(prmysql, depth);
		}
		break;
		case MYSQLI_PR_COMMAND:
		{
			PR_COMMAND *prcommand = (PR_COMMAND *)current;
			strcpy(tag, "command");
			MYSQLI_PROFILER_ADD_STARTTAG(buffer, tag, 1);
			php_mysqli_profiler_report_command(prcommand, depth);
		}
		break;
		case MYSQLI_PR_RESULT:
		{
			PR_RESULT *prresult = (PR_RESULT *)current;
			strcpy(tag, "resultset");
			MYSQLI_PROFILER_ADD_STARTTAG(buffer, tag, 1);
			php_mysqli_profiler_report_result(prresult, depth);
		}
		break;
		case MYSQLI_PR_STMT:
		case MYSQLI_PR_STMT_RESULT:
		{
			PR_STMT *prstmt = (PR_STMT *)current;
			strcpy(tag, "statement");
			MYSQLI_PROFILER_ADD_STARTTAG(buffer, tag, 1);
			php_mysqli_profiler_report_stmt(prstmt, depth);
		}
		break;
		case MYSQLI_PR_QUERY:
		case MYSQLI_PR_QUERY_RESULT:
		{
			PR_QUERY *prquery = (PR_QUERY *)current;
			strcpy(tag, "query");
			MYSQLI_PROFILER_ADD_STARTTAG(buffer, tag, 1);
			php_mysqli_profiler_report_query(prquery, depth);
		}
		break;
	}
	child = current->header.child;
	if (child) {
		php_mysqli_profiler_report(child, depth+1);
	}

	MYSQLI_PROFILER_ADD_ENDTAG(buffer, tag);
	next = (current->header.next) ? current->header.next : NULL;
	
	if (next) {
		php_mysqli_profiler_report(next, depth);
	}

	if (current->header.type == MYSQLI_PR_MAIN) {
		PR_MAIN *prmain = (PR_MAIN *)current;
		my_efree(prmain->name);
	}
	efree(current);
	efree(ident);
	return;
}
/* }}} */

/* {{{ proto bool mysqli_profiler (int flags, string info, int port)
*/
PHP_FUNCTION(mysqli_profiler)
{ 
	int 	flag;
	char	*name;
	int		name_len = 0, port = 0;
	int		connection;

	if (MyG(profiler)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Profiler was already started.");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|sl", &flag, &name, &name_len, &port) == FAILURE) {
		return;
	}

	switch (flag) {
		case MYSQLI_PR_REPORT_STDERR:
			prmain = ecalloc(1, sizeof(PR_MAIN));
			prmain->mode = flag;
			break;
		case MYSQLI_PR_REPORT_PORT:
			if (!name_len) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Hostname not specified.");
				RETURN_FALSE;
			}
			if (!port) {	
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Portnumber not specified.");
				RETURN_FALSE;
			}
			if (!(connection = php_mysqli_create_socket(name, port))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to connect to host %s on port %d", name, port);
				RETURN_FALSE;
			}	
			prmain = ecalloc(1, sizeof(PR_MAIN));
			prmain->mode = flag;
			prmain->port = port;
			prmain->name = my_estrdup(name);
			prmain->connection = connection;
			break;
		case MYSQLI_PR_REPORT_FILE:
			if (!name_len) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename not specified.");
				RETURN_FALSE;
			}
			prmain = ecalloc(1, sizeof(PR_MAIN));
			if (!(prmain->fp = fopen(name, "w"))){
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't write to file %s.", name);
				efree(prmain);
				RETURN_FALSE;
			}
			prmain->mode = flag;
			prmain->name = my_estrdup(name);
			
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported flag: %d", flag);
			RETURN_FALSE;
	}


/*    PR_SSEND(port,"<application>");
	php_mysqli_close_socket(prmain.connection);
*/
	MyG(profiler) = flag;
	RETURN_TRUE;
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
