/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Hartmut Holzgraefe <hartmut@six.de>                         |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "php_dbplus.h"

/* If you declare any globals in php_dbplus.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(dbplus)
*/

#ifdef COMPILE_DL_DBPLUS
ZEND_GET_MODULE(dbplus)
#endif


/* {{{ Resource handling */

/* True global resources - no need for thread safety here */
int le_dbplus_relation;
int le_dbplus_tupel;

void dbplus_destruct_relation(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	relf *conn = (relf *)(rsrc->ptr);

	cdb_close(conn);
}


void dbplus_destruct_tupel(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	tuple *t = (tuple *)(rsrc->ptr);

	efree(t);
}

/* }}} */

/* {{{ function table */

/* Every user visible function must have an entry in dbplus_functions[].
*/
function_entry dbplus_functions[] = {
	PHP_FE(dbplus_add,	NULL)
	PHP_FE(dbplus_aql,	NULL)
	PHP_FE(dbplus_change,	NULL)
	PHP_FE(dbplus_chdir,	NULL)
	PHP_FE(dbplus_close,	NULL)
	PHP_FE(dbplus_curr,	NULL)
	PHP_FE(dbplus_errno,	NULL)
	PHP_FE(dbplus_errcode,	NULL)
	PHP_FE(dbplus_find, 	NULL)
	PHP_FE(dbplus_first,	NULL)
	PHP_FE(dbplus_flush,	NULL)
	PHP_FE(dbplus_freealllocks,	NULL)
	PHP_FE(dbplus_freelock,	NULL)
	PHP_FE(dbplus_freerlocks,	NULL)
	PHP_FE(dbplus_getlock,	NULL)
	PHP_FE(dbplus_getunique,	NULL)
	PHP_FE(dbplus_info,	NULL)
	PHP_FE(dbplus_last,	NULL)
	PHP_FE(dbplus_next,	NULL)
	PHP_FE(dbplus_open,	NULL)
	PHP_FE(dbplus_prev,	NULL)
	PHP_FE(dbplus_rchperm,	NULL)
	PHP_FE(dbplus_rcreate,	NULL)
	PHP_FE(dbplus_rcrtexact,	NULL)
	PHP_FE(dbplus_rcrtlike,	NULL)
	PHP_FE(dbplus_resolve,	NULL)
	PHP_FE(dbplus_restorepos,	NULL)
	PHP_FE(dbplus_rkeys,	NULL)
	PHP_FE(dbplus_ropen,	NULL)
	PHP_FE(dbplus_rquery,	NULL)
	PHP_FE(dbplus_rrename,	NULL)
	PHP_FE(dbplus_rsecindex,	NULL)
	PHP_FE(dbplus_runlink,	NULL)
	PHP_FE(dbplus_rzap,	NULL)
	PHP_FE(dbplus_savepos,	NULL)
	PHP_FE(dbplus_setindex,	NULL)
	PHP_FE(dbplus_setindexbynumber,	NULL)
	PHP_FE(dbplus_sql,	NULL)
	PHP_FE(dbplus_tcl,	NULL)
	PHP_FE(dbplus_tremove,	NULL)
	PHP_FE(dbplus_undo,	NULL)
	PHP_FE(dbplus_undoprepare,	NULL)
	PHP_FE(dbplus_unlockrel,	NULL)
	PHP_FE(dbplus_unselect,	NULL)
	PHP_FE(dbplus_update,	NULL)
	PHP_FE(dbplus_xlockrel,	NULL)
	PHP_FE(dbplus_xunlockrel,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in dbplus_functions[] */
};

/* }}} */

/* {{{ module entry */

zend_module_entry dbplus_module_entry = {
	"dbplus",
	dbplus_functions,
	PHP_MINIT(dbplus),
	PHP_MSHUTDOWN(dbplus),
	PHP_RINIT(dbplus),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(dbplus),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(dbplus),
	STANDARD_MODULE_PROPERTIES
};

/* }}} */

/* {{{ ini parameters */

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
PHP_INI_END()
*/

/* }}} */

/* {{{ module init */

PHP_MINIT_FUNCTION(dbplus)
{
	/* resource id for cdb connections */
	le_dbplus_relation = zend_register_list_destructors_ex(dbplus_destruct_relation, NULL, "dbplus_relation", module_number);
	le_dbplus_tupel    = zend_register_list_destructors_ex(dbplus_destruct_tupel   , NULL, "dbplus_tuple"   , module_number);

	return SUCCESS;
}

/* }}} */

/* {{{ module shutdown */

PHP_MSHUTDOWN_FUNCTION(dbplus)
{
	return SUCCESS;
}

/* }}} */

/* {{{ request init */

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(dbplus)
{
	return SUCCESS;
}

/* }}} */

/* {{{ request shutdown */

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(dbplus)
{
	return SUCCESS;
}

/* }}} */

/* {{{ module information */

PHP_MINFO_FUNCTION(dbplus)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "dbplus support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* }}} */

#if 0
/* {{{ proto int dbplus_open(string name, int writing, int searchpath)

    */
PHP_FUNCTION(dbplus_open)
{
	relf *conn;

	zval **name, **writing, **searchpath;
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &name, &writing, &searchpath) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);
	convert_to_long_ex(writing);
	convert_to_long_ex(searchpath);

	conn = cdb_open((*name)->value.str.val,(*writing)->value.lval,(*searchpath)->value.lval);
	if(conn == NULL) {
		/* TODO error handling */
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value,conn,le_dbplus);
}

/* }}} */

/* {{{ proto void dbplus_close(int conn)
    */
PHP_FUNCTION(dbplus_close)
{
	zval **conn;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &conn) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(conn);

	/* TODO resource type check */

	zend_list_delete((*conn)->value.lval);	
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
