/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_DBPLUS_H
#define PHP_DBPLUS_H


extern zend_module_entry dbplus_module_entry;
#define phpext_dbplus_ptr &dbplus_module_entry

#ifdef PHP_WIN32
#define PHP_DBPLUS_API __declspec(dllexport)
#else
#define PHP_DBPLUS_API
#endif

PHP_MINIT_FUNCTION(dbplus);
PHP_MSHUTDOWN_FUNCTION(dbplus);
PHP_RINIT_FUNCTION(dbplus);
PHP_RSHUTDOWN_FUNCTION(dbplus);
PHP_MINFO_FUNCTION(dbplus);

PHP_FUNCTION(dbplus_add);
PHP_FUNCTION(dbplus_aql);
PHP_FUNCTION(dbplus_chdir);
PHP_FUNCTION(dbplus_close);
PHP_FUNCTION(dbplus_curr);
PHP_FUNCTION(dbplus_errno);
PHP_FUNCTION(dbplus_errcode);
PHP_FUNCTION(dbplus_find);
PHP_FUNCTION(dbplus_first);
PHP_FUNCTION(dbplus_flush);
PHP_FUNCTION(dbplus_freealllocks);
PHP_FUNCTION(dbplus_freelock);
PHP_FUNCTION(dbplus_freerlocks);
PHP_FUNCTION(dbplus_getlock);
PHP_FUNCTION(dbplus_getunique);
PHP_FUNCTION(dbplus_info);
PHP_FUNCTION(dbplus_last);
PHP_FUNCTION(dbplus_next);
PHP_FUNCTION(dbplus_open);
PHP_FUNCTION(dbplus_prev);
PHP_FUNCTION(dbplus_rchperm);
PHP_FUNCTION(dbplus_rcreate);
PHP_FUNCTION(dbplus_rcrtexact);
PHP_FUNCTION(dbplus_rcrtlike);
PHP_FUNCTION(dbplus_resolve);
PHP_FUNCTION(dbplus_restorepos);
PHP_FUNCTION(dbplus_rkeys);
PHP_FUNCTION(dbplus_ropen);
PHP_FUNCTION(dbplus_rquery);
PHP_FUNCTION(dbplus_rrename);
PHP_FUNCTION(dbplus_rsecindex);
PHP_FUNCTION(dbplus_runlink);
PHP_FUNCTION(dbplus_rzap);
PHP_FUNCTION(dbplus_savepos);
PHP_FUNCTION(dbplus_setindex);
PHP_FUNCTION(dbplus_setindexbynumber);
PHP_FUNCTION(dbplus_sql);
PHP_FUNCTION(dbplus_tcl);
PHP_FUNCTION(dbplus_tremove);
PHP_FUNCTION(dbplus_undo);
PHP_FUNCTION(dbplus_undoprepare);
PHP_FUNCTION(dbplus_unlockrel);
PHP_FUNCTION(dbplus_unselect);
PHP_FUNCTION(dbplus_update);
PHP_FUNCTION(dbplus_xlockrel);
PHP_FUNCTION(dbplus_xunlockrel);


/* 
    Declare any global variables you may need between the BEGIN
    and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(dbplus)
    int global_variable;
ZEND_END_MODULE_GLOBALS(dbplus)
*/

/* In every function that needs to use variables in php_dbplus_globals,
   do call DBPLUSLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as DBPLUSG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define AG(v) TSRMG(dbplus_globals_id, php_dbplus_globals *, v)
#else
#define DBPLUSG(v) (dbplus_globals.v)
#endif


int le_dbplus_relation;
int le_dbplus_tuple;

void dbplus_destruct_relation(zend_rsrc_list_entry *rsrc TSRMLS_DC);
void dbplus_destruct_tupel(zend_rsrc_list_entry *rsrc TSRMLS_DC);


#endif  /* PHP_DBPLUS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
