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
*/

#ifndef __HAVE_MYSQLI_PROFILER_H__
#define __HAVE_MYSQLI_PROFILER_H__

#ifdef PHP_WIN32
#include "win32/time.h"
#else
#include "sys/time.h"
#endif

#define MYSQLI_PROFILER_PROTOCOL_VERSION	1.0

/* common profiler header struct */
typedef struct {
	unsigned int	type;
	void		*child;
	void		*next;
	char		*filename;
	unsigned int	lineno;
	char		*functionname;
	struct timeval	starttime;
	struct timeval	elapsedtime;
	struct timeval	lifetime;
	char		*errormsg;
	ulong		error;
} PR_HEADER;

typedef struct {
	char		**value;
} PR_ROW;

/* explain output */
typedef struct {
	char		*query;
	unsigned int	exp_cnt;
	unsigned int	columns;
	char		**fields;
	PR_ROW		*row;
} PR_EXPLAIN;

/* main profiler */
typedef struct {
	PR_HEADER	header;
	unsigned int	mode;
	char		*name;
	int		port;
	char		*info;
	int		connection;
	FILE		*fp;
} PR_MAIN;

/* common */
typedef struct {
	PR_HEADER	header;
} PR_COMMON;

/* connection */
typedef struct {
	PR_HEADER	header;
	unsigned int	thread_id;
	char		*hostname;
	char		*username;
	unsigned int	closed;
} PR_MYSQL;

/* resultset */
typedef struct {
	PR_HEADER	header;
	unsigned int	columns;
	ulong		rows;
	ulong		fields;
	ulong		fetched_rows;
	unsigned int	closed;
} PR_RESULT;

/* command */
/* TODO: return values */
typedef struct {
	PR_HEADER	header;
	ulong		returntype;
	char		*returnvalue;
} PR_COMMAND;

/* query */
typedef struct {
	PR_HEADER	header;
	PR_EXPLAIN	explain;
	ulong		affectedrows;
	ulong		insertid;
} PR_QUERY;

/* statement */
typedef struct {
	PR_HEADER	header;
	PR_EXPLAIN	explain;
	unsigned int	param_cnt;
	unsigned int	field_cnt;
} PR_STMT;

/* Profiler protofypes */
extern void php_mysqli_profiler_report(PR_COMMON *, int);
extern PR_COMMON *php_mysqli_profiler_new_object(PR_COMMON *parent, unsigned int type, unsigned int settime);
extern void php_mysqli_profiler_result_info(MYSQL_RES *res);
extern int php_mysqli_profiler_explain(PR_EXPLAIN *explain, PR_HEADER *header, MYSQL *mysql, char *query);
extern void php_mysqli_profiler_timediff(struct timeval start, struct timeval *elapsed);
extern PR_MAIN *prmain;

/*** PROFILER MODES ***/
#define MYSQLI_PR_REPORT_STDERR		1
#define MYSQLI_PR_REPORT_PORT		2
#define MYSQLI_PR_REPORT_FILE		3

/*** PROFILER CONSTANTS ***/
#define MYSQLI_PR_MAIN			0
#define MYSQLI_PR_MYSQL			1
#define MYSQLI_PR_QUERY			2
#define MYSQLI_PR_QUERY_RESULT		3
#define MYSQLI_PR_STMT			4
#define MYSQLI_PR_STMT_RESULT		5
#define MYSQLI_PR_RESULT		6

/*** PROFILER MACROS ***/
#define MYSQLI_PROFILER_STARTTIME(ptr) gettimeofday(&ptr->header.starttime, NULL)
#define MYSQLI_PROFILER_ELAPSEDTIME(ptr) php_mysqli_profiler_timediff(ptr->header.starttime, &ptr->header.elapsedtime)
#define MYSQLI_PROFILER_LIFETIME(ptr) php_mysqli_profiler_timediff((ptr)->starttime, &(ptr)->lifetime)
#define MYSQLI_PROFILER_NEW(parent, type, time) php_mysqli_profiler_new_object((PR_COMMON *)parent, type, time)
#define MYSQLI_PROFILER_COMMAND_START(cmd,parent)\
if (MyG(profiler))\
{\
	cmd = (PR_COMMAND *)php_mysqli_profiler_new_object((PR_COMMON *)parent, MYSQLI_PR_COMMAND,1);\
} else {\
	cmd = NULL;\
}

#define MYSQLI_PROFILER_COMMAND_RETURNLONG(cmd, value)\
if (MyG(profiler))\
{\
	char tmp[30];\
	sprintf ((char *)&tmp, "%ld", value);\
	MYSQLI_PROFILER_ELAPSEDTIME(cmd);\
	cmd->returnvalue = my_estrdup(tmp);\
}

#define MYSQLI_PROFILER_COMMAND_RETURNSTRING(cmd, value)\
if (MyG(profiler))\
{\
	MYSQLI_PROFILER_ELAPSEDTIME(cmd);\
	cmd->returnvalue = my_estrdup(value);\
}
#define MYSQLI_PROFILER_EXPLAIN(explain,header,mysql,query) php_mysqli_profiler_explain(explain,header, mysql, query)

#define MYSQLI_PROFILER_OUT(buffer)\
	switch (prmain->mode) {\
		case MYSQLI_PR_REPORT_STDERR:\
			fprintf(stderr, "%s", buffer);\
			break;\
		case MYSQLI_PR_REPORT_FILE:\
			fprintf(prmain->fp, "%s", buffer);\
			break;\
		case MYSQLI_PR_REPORT_PORT:\
			PR_SSEND(prmain->port,buffer);\
			break;\
	}\
	buffer[0] = '\0';

#define	MYSQLI_PROFILER_ADD_STARTTAG(buffer, str, out)\
	if (prmain->mode == MYSQLI_PR_REPORT_STDERR) {\
		if (out) { \
			sprintf((char *)&buffer, "%s[%s]\n", ident, str);\
		} \
	}\
	else {\
		sprintf((char *)&buffer, "<%s>\n", str);\
	}\
	MYSQLI_PROFILER_OUT(buffer);

#define	MYSQLI_PROFILER_ADD_ATTR_STRING(buffer, tag, value)\
	if (prmain->mode == MYSQLI_PR_REPORT_STDERR) {\
		sprintf((char *)&buffer, "%s%s: %s\n", ident, tag, value);\
	}\
	else {\
		sprintf((char *)&buffer, "<%s>%s</%s>\n", tag, value, tag);\
	}\
	MYSQLI_PROFILER_OUT(buffer);

#define	MYSQLI_PROFILER_ADD_ATTR_LONG(buffer, tag, value)\
	if (prmain->mode == MYSQLI_PR_REPORT_STDERR) {\
		sprintf((char *)&buffer, "%s%s: %ld\n", ident, tag, (long)value);\
	}\
	else {\
		sprintf((char *)&buffer, "<%s>%ld</%s>\n", tag, (long)value, tag);\
	}\
	MYSQLI_PROFILER_OUT(buffer);

#define	MYSQLI_PROFILER_ADD_ATTR_TIME(buffer, tag, sec, usec)\
	if (prmain->mode == MYSQLI_PR_REPORT_STDERR) {\
		sprintf((char *)&buffer, "%s%s: %ld.%06ld\n", ident, tag, sec, usec);\
	}\
	else {\
		sprintf((char *)&buffer, "<%s>%ld.%06ld</%s>\n", tag, sec, usec, tag);\
	}\
	MYSQLI_PROFILER_OUT(buffer);

#define	MYSQLI_PROFILER_ADD_ENDTAG(buffer, tag)\
	if (prmain->mode == MYSQLI_PR_REPORT_STDERR) {\
		fprintf(stderr, "\n"); \
	} else { \
		sprintf((char *)&buffer, "</%s>\n", tag);\
	}\
	MYSQLI_PROFILER_OUT(buffer);
#endif
