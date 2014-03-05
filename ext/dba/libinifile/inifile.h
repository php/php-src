/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_LIB_INIFILE_H
#define PHP_LIB_INIFILE_H

typedef struct {
	char *group;
	char *name;
} key_type;

typedef struct {
	char *value;
} val_type;

typedef struct {
	key_type key;
	val_type val;
	size_t pos;
} line_type;

typedef struct {
	char *lockfn;
	int lockfd;
	php_stream *fp;
	int readonly;
	line_type curr;
	line_type next;
} inifile;

val_type inifile_fetch(inifile *dba, const key_type *key, int skip TSRMLS_DC);
int inifile_firstkey(inifile *dba TSRMLS_DC);
int inifile_nextkey(inifile *dba TSRMLS_DC);
int inifile_delete(inifile *dba, const key_type *key TSRMLS_DC);
int inifile_replace(inifile *dba, const key_type *key, const val_type *val TSRMLS_DC);
int inifile_append(inifile *dba, const key_type *key, const val_type *val TSRMLS_DC);
char *inifile_version();

key_type inifile_key_split(const char *group_name);
char * inifile_key_string(const key_type *key);

void inifile_key_free(key_type *key);
void inifile_val_free(val_type *val);
void inifile_line_free(line_type *ln);

inifile * inifile_alloc(php_stream *fp, int readonly, int persistent TSRMLS_DC);
void inifile_free(inifile *dba, int persistent);

#endif
