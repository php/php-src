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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_DBA

#include "php_dba.h"
#include "ext/standard/info.h"

#include "php_gdbm.h"
#include "php_ndbm.h"
#include "php_dbm.h"
#include "php_cdb.h"
#include "php_db2.h"
#include "php_db3.h"

/* {{{ dba_functions[]
 */
function_entry dba_functions[] = {
	PHP_FE(dba_open, NULL)
	PHP_FE(dba_popen, NULL)
	PHP_FE(dba_close, NULL)
	PHP_FE(dba_delete, NULL)
	PHP_FE(dba_exists, NULL)
	PHP_FE(dba_fetch, NULL)
	PHP_FE(dba_insert, NULL)
	PHP_FE(dba_replace, NULL)
	PHP_FE(dba_firstkey, NULL)
	PHP_FE(dba_nextkey, NULL)
	PHP_FE(dba_optimize, NULL)
	PHP_FE(dba_sync, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

static PHP_MINIT_FUNCTION(dba);
static PHP_MSHUTDOWN_FUNCTION(dba);
static PHP_MINFO_FUNCTION(dba);

zend_module_entry dba_module_entry = {
    STANDARD_MODULE_HEADER,
	"dba",
    dba_functions, 
	PHP_MINIT(dba), 
	PHP_MSHUTDOWN(dba), 
	NULL,
    NULL,
	PHP_MINFO(dba),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_DBA
ZEND_GET_MODULE(dba)
#endif

typedef struct dba_handler {
	char *name;
	int (*open)(dba_info * TSRMLS_DC);
	void (*close)(dba_info *);
	char* (*fetch)(dba_info *, char *, int, int *);
	int (*update)(dba_info *, char *, int, char *, int, int);
	int (*exists)(dba_info *, char *, int);
	int (*delete)(dba_info *, char *, int);
	char* (*firstkey)(dba_info *, int *);
	char* (*nextkey)(dba_info *, int *);
	int (*optimize)(dba_info *);
	int (*sync)(dba_info *);
} dba_handler;

/* {{{ macromania */

#define DBA_ID_PARS 											\
	pval **id; 													\
	dba_info *info = NULL; 										\
	int type, ac = ZEND_NUM_ARGS()

/* these are used to get the standard arguments */

#define DBA_GET1 												\
	if(ac != 1 || zend_get_parameters_ex(ac, &id) != SUCCESS) { 		\
		WRONG_PARAM_COUNT; 										\
	}

#define DBA_GET2 												\
	pval **key; 												\
	if(ac != 2 || zend_get_parameters_ex(ac, &key, &id) != SUCCESS) { 	\
		WRONG_PARAM_COUNT; 										\
	} 															\
	convert_to_string_ex(key)

#define DBA_IF_NOT_CORRECT_TYPE(link_id) 						\
	info = zend_list_find(link_id, &type); 						\
	if(!info || (type != GLOBAL(le_db) && type != GLOBAL(le_pdb)))
	
#define DBA_ID_GET 												\
	convert_to_long_ex(id); 									\
	DBA_IF_NOT_CORRECT_TYPE(Z_LVAL_PP(id)) { 					\
		php_error(E_WARNING, "%s(): Unable to find DBA identifier %d", \
			get_active_function_name(TSRMLS_C),					\
			Z_LVAL_PP(id));										\
		RETURN_FALSE; 											\
	}
	
#define DBA_ID_GET1 DBA_ID_PARS; DBA_GET1; DBA_ID_GET
#define DBA_ID_GET2 DBA_ID_PARS; DBA_GET2; DBA_ID_GET

/* a DBA handler must have specific routines */

#define DBA_HND(x) \
{\
	#x, dba_open_##x, dba_close_##x, dba_fetch_##x, dba_update_##x, \
	dba_exists_##x, dba_delete_##x, dba_firstkey_##x, dba_nextkey_##x, \
	dba_optimize_##x, dba_sync_##x \
},

/* check whether the user has write access */
#define DBA_WRITE_CHECK \
	if(info->mode != DBA_WRITER && info->mode != DBA_TRUNC && info->mode != DBA_CREAT) { \
		php_error(E_WARNING, "%s(): you cannot perform a modification to a database without proper access", get_active_function_name(TSRMLS_C)); \
		RETURN_FALSE; \
	}

#define GLOBAL(a) a

/* }}} */

/* {{{ globals */

static dba_handler handler[] = {
#if DBA_GDBM
	DBA_HND(gdbm)
#endif
#if DBA_DBM
	DBA_HND(dbm)
#endif
#if DBA_NDBM
	DBA_HND(ndbm)
#endif
#if DBA_CDB
	DBA_HND(cdb)
#endif
#if DBA_DB2
	DBA_HND(db2)
#endif
#if DBA_DB3
	DBA_HND(db3)
#endif
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static int le_db;
static int le_pdb;
static HashTable ht_keys;
/* }}} */

/* {{{ dba_close 
 */ 
static void dba_close(dba_info *info)
{
	if(info->hnd) info->hnd->close(info);
	if(info->path) free(info->path);
	free(info);
}
/* }}} */

/* {{{ dba_close_rsrc
 */
static void dba_close_rsrc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	dba_info *info = (dba_info *)rsrc->ptr;

	dba_close(info);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(dba)
{
	zend_hash_init(&ht_keys, 0, NULL, NULL, 1);
	GLOBAL(le_db) = zend_register_list_destructors_ex(dba_close_rsrc, NULL, "dba", module_number);
	GLOBAL(le_pdb) = zend_register_list_destructors_ex(NULL, dba_close_rsrc, "dba persistent", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(dba)
{
	zend_hash_destroy(&ht_keys);
	return SUCCESS;
}
/* }}} */

#include "ext/standard/php_smart_str.h"

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(dba)
{
	dba_handler *hptr;
	smart_str handlers = {0};

	for(hptr = handler; hptr->name; hptr++) {
		smart_str_appends(&handlers, hptr->name);
		smart_str_appendc(&handlers, ' ');
 	}

	php_info_print_table_start();
 	php_info_print_table_row(2, "DBA support", "enabled");
	if (handlers.c) {
		smart_str_0(&handlers);
		php_info_print_table_row(2, "Supported handlers", handlers.c);
		smart_str_free(&handlers);
	} else {
		php_info_print_table_row(2, "Supported handlers", "none");
	}
	php_info_print_table_end();
}
/* }}} */
                                
/* {{{ php_dba_update
 */
static void php_dba_update(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	DBA_ID_PARS;
	pval **val, **key;

	if(ac != 3 || zend_get_parameters_ex(ac, &key, &val, &id) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(key);
	convert_to_string_ex(val);
	DBA_ID_GET;

	DBA_WRITE_CHECK;
	
	if(info->hnd->update(info, VALLEN(key), VALLEN(val), mode) == SUCCESS)
		RETURN_TRUE;
	RETURN_FALSE;
}
/* }}} */

#define FREENOW if(args) efree(args); if(key) efree(key)

/* {{{ php_dba_open
 */
static void php_dba_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval ***args = (pval ***) NULL;
	int ac = ZEND_NUM_ARGS();
	dba_mode_t modenr;
	dba_info *info;
	dba_handler *hptr;
	char *key = NULL;
	int keylen = 0;
	int listid;
	int i;
	
	if(ac < 3) {
		WRONG_PARAM_COUNT;
	}
	
	/* we pass additional args to the respective handler */
	args = emalloc(ac * sizeof(pval *));
	if(zend_get_parameters_array_ex(ac, args) != SUCCESS) {
		FREENOW;
		WRONG_PARAM_COUNT;
	}
		
	/* we only take string arguments */
	for(i = 0; i < ac; i++) {
		convert_to_string_ex(args[i]);
		keylen += Z_STRLEN_PP(args[i]);
	}

	if(persistent) {
		/* calculate hash */
		key = emalloc(keylen);
		keylen = 0;
		
		for(i = 0; i < ac; i++) {
			memcpy(key+keylen, Z_STRVAL_PP(args[i]), Z_STRLEN_PP(args[i]));
			keylen += Z_STRLEN_PP(args[i]);
		}
		
		if(zend_hash_find(&ht_keys, key, keylen, (void **) &info) == SUCCESS) {
			FREENOW;
			RETURN_LONG(zend_list_insert(info, GLOBAL(le_pdb)));
		}
	}
	
	for(hptr = handler; hptr->name &&
			strcasecmp(hptr->name, Z_STRVAL_PP(args[2])); hptr++);

	if(!hptr->name) {
		php_error(E_WARNING, "%s(): no such handler: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(args[2]));
		FREENOW;
		RETURN_FALSE;
	}

	switch(Z_STRVAL_PP(args[1])[0]) {
		case 'c': 
			modenr = DBA_CREAT; 
			break;
		case 'w': 
			modenr = DBA_WRITER; 
			break;
		case 'r': 
			modenr = DBA_READER; 
			break;
		case 'n':
			modenr = DBA_TRUNC;
			break;
		default:
			php_error(E_WARNING, "%s(): illegal DBA mode: %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(args[1]));
			FREENOW;
			RETURN_FALSE;
	}
			
	info = malloc(sizeof(*info));
	memset(info, 0, sizeof(info));
	info->path = strdup(Z_STRVAL_PP(args[0]));
	info->mode = modenr;
	info->argc = ac - 3;
	info->argv = args + 3;
	info->hnd = NULL;

	if(hptr->open(info TSRMLS_CC) != SUCCESS) {
		dba_close(info);
		php_error(E_WARNING, "%s(): driver initialization failed", get_active_function_name(TSRMLS_C));
		FREENOW;
		RETURN_FALSE;
	}
	info->hnd = hptr;
	info->argc = 0;
	info->argv = NULL;

	listid = zend_list_insert(info, persistent?GLOBAL(le_pdb):GLOBAL(le_db));
	if(persistent) {
		zend_hash_update(&ht_keys, key, keylen, info, sizeof(*info), NULL);
	}
	
	FREENOW;
	RETURN_LONG(listid);
}
/* }}} */
#undef FREENOW

/* {{{ proto int dba_popen(string path, string mode, string handlername [, string ...])
   Opens path using the specified handler in mode persistently */
PHP_FUNCTION(dba_popen)
{
	php_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int dba_open(string path, string mode, string handlername [, string ...])
   Opens path using the specified handler in mode*/
PHP_FUNCTION(dba_open)
{
	php_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void dba_close(int handle)
   Closes database */
PHP_FUNCTION(dba_close)
{
	DBA_ID_GET1;	
	
	zend_list_delete(Z_LVAL_PP(id));
}
/* }}} */

/* {{{ proto bool dba_exists(string key, int handle)
   Checks, if the specified key exists */
PHP_FUNCTION(dba_exists)
{
	DBA_ID_GET2;

	if(info->hnd->exists(info, VALLEN(key)) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string dba_fetch(string key, int handle)
   Fetches the data associated with key */
PHP_FUNCTION(dba_fetch)
{
	char *val;
	int len = 0;
	DBA_ID_GET2;

	if((val = info->hnd->fetch(info, VALLEN(key), &len)) != NULL) {
		RETURN_STRINGL(val, len, 0);
	} 
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string dba_firstkey(int handle)
   Resets the internal key pointer and returns the first key */
PHP_FUNCTION(dba_firstkey)
{
	char *fkey;
	int len;
	DBA_ID_GET1;

	fkey = info->hnd->firstkey(info, &len);
	if(fkey)
		RETURN_STRINGL(fkey, len, 0);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string dba_nextkey(int handle)
   Returns the next key */
PHP_FUNCTION(dba_nextkey)
{
	char *nkey;
	int len;
	DBA_ID_GET1;

	nkey = info->hnd->nextkey(info, &len);
	if(nkey)
		RETURN_STRINGL(nkey, len, 0);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool dba_delete(string key, int handle)
   Deletes the entry associated with key */
PHP_FUNCTION(dba_delete)
{
	DBA_ID_GET2;
	
	DBA_WRITE_CHECK;
	
	if(info->hnd->delete(info, VALLEN(key)) == SUCCESS)
		RETURN_TRUE;
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool dba_insert(string key, string value, int handle)
   Inserts value as key, returns false, if key exists already */
PHP_FUNCTION(dba_insert)
{
	php_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool dba_replace(string key, string value, int handle)
   Inserts value as key, replaces key, if key exists already */
PHP_FUNCTION(dba_replace)
{
	php_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool dba_optimize(int handle)
   Optimizes (e.g. clean up, vacuum) database */
PHP_FUNCTION(dba_optimize)
{
	DBA_ID_GET1;
	
	DBA_WRITE_CHECK;
	if(info->hnd->optimize(info) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool dba_sync(int handle)
   Synchronizes database */
PHP_FUNCTION(dba_sync)
{
	DBA_ID_GET1;
	
	if(info->hnd->sync(info) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
