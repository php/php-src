/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#ifndef PHP_DB_H
#define PHP_DB_H


#ifndef DLEXPORT
#define DLEXPORT
#endif


extern zend_module_entry dbm_module_entry;
#define phpext_db_ptr &dbm_module_entry



typedef struct dbm_info {
        char *filename;
        char *lockfn;
        int lockfd;
        void *dbf;
} dbm_info;

/*
  we're not going to bother with flatfile on win32
  because the dbm module will be external, and we
  do not want flatfile compiled staticly
*/
#if defined(PHP_WIN32) && !defined(COMPILE_DL_DB)
#undef phpext_db_ptr
#define phpext_db_ptr NULL
#endif

dbm_info *php_find_dbm(pval *id TSRMLS_DC);
void php_dbm_close(zend_rsrc_list_entry *rsrc TSRMLS_DC);
dbm_info *php_dbm_open(char *filename, char *mode TSRMLS_DC);
char *php_dbm_fetch(dbm_info *info, char *key TSRMLS_DC);
int php_dbm_insert_replace(dbm_info *info, char *key, char *value, int replace_mode TSRMLS_DC);
int php_dbm_exists(dbm_info *info, char *key TSRMLS_DC);
int php_dbm_delete(dbm_info *info, char *key TSRMLS_DC);
char *php_dbm_first_key(dbm_info *info TSRMLS_DC);
char *php_dbm_nextkey(dbm_info *info, char *key TSRMLS_DC);

/* db file functions */
PHP_MINIT_FUNCTION(db);
PHP_RINIT_FUNCTION(db);
PHP_MINFO_FUNCTION(db);

PHP_FUNCTION(dblist);
PHP_FUNCTION(dbmopen);
PHP_FUNCTION(dbmclose);
PHP_FUNCTION(dbminsert);
PHP_FUNCTION(dbmfetch);
PHP_FUNCTION(dbmreplace);
PHP_FUNCTION(dbmexists);
PHP_FUNCTION(dbmdelete);
PHP_FUNCTION(dbmfirstkey);
PHP_FUNCTION(dbmnextkey);

#endif /* PHP_DB_H */
