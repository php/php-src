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
	PHP_FE(dba_handlers, NULL)
	PHP_FE(dba_list, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

PHP_MINIT_FUNCTION(dba);
PHP_MINFO_FUNCTION(dba);

zend_module_entry dba_module_entry = {
	STANDARD_MODULE_HEADER,
	"dba",
	dba_functions, 
	PHP_MINIT(dba), 
	NULL,
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
	int (*open)(dba_info *, char **error TSRMLS_DC);
	void (*close)(dba_info * TSRMLS_DC);
	char* (*fetch)(dba_info *, char *, int, int, int * TSRMLS_DC);
	int (*update)(dba_info *, char *, int, char *, int, int TSRMLS_DC);
	int (*exists)(dba_info *, char *, int TSRMLS_DC);
	int (*delete)(dba_info *, char *, int TSRMLS_DC);
	char* (*firstkey)(dba_info *, int * TSRMLS_DC);
	char* (*nextkey)(dba_info *, int * TSRMLS_DC);
	int (*optimize)(dba_info * TSRMLS_DC);
	int (*sync)(dba_info * TSRMLS_DC);
} dba_handler;

/* {{{ macromania */

#define DBA_ID_PARS 											\
	zval **id; 													\
	dba_info *info = NULL; 										\
	int ac = ZEND_NUM_ARGS()

/* these are used to get the standard arguments */

#define DBA_GET1 												\
	if(ac != 1 || zend_get_parameters_ex(ac, &id) != SUCCESS) { 		\
		WRONG_PARAM_COUNT; 										\
	}

#define DBA_GET2 												\
	zval **key; 												\
	if(ac != 2 || zend_get_parameters_ex(ac, &key, &id) != SUCCESS) { 	\
		WRONG_PARAM_COUNT; 										\
	} 															\
	convert_to_string_ex(key)

#define DBA_GET2_3												\
	zval **key; 												\
	zval **tmp; 												\
	int skip = 0;  												\
	switch(ac) {												\
	case 2: 													\
		if (zend_get_parameters_ex(ac, &key, &id) != SUCCESS) { \
			WRONG_PARAM_COUNT; 									\
		} 														\
		break;  												\
	case 3: 													\
		if (zend_get_parameters_ex(ac, &key, &tmp, &id) != SUCCESS) { \
			WRONG_PARAM_COUNT; 									\
		} 														\
		convert_to_long_ex(tmp);								\
		skip = Z_LVAL_PP(tmp);  								\
		break;  												\
	default:													\
		WRONG_PARAM_COUNT; 										\
	} 															\
	convert_to_string_ex(key)

#define DBA_ID_GET 												\
	ZEND_FETCH_RESOURCE2(info, dba_info *, id, -1, "DBA identifier", le_db, le_pdb);
	
#define DBA_ID_GET1   DBA_ID_PARS; DBA_GET1;   DBA_ID_GET
#define DBA_ID_GET2   DBA_ID_PARS; DBA_GET2;   DBA_ID_GET
#define DBA_ID_GET2_3 DBA_ID_PARS; DBA_GET2_3; DBA_ID_GET

/* a DBA handler must have specific routines */

#define DBA_NAMED_HND(name, x) \
{\
	#name, dba_open_##x, dba_close_##x, dba_fetch_##x, dba_update_##x, \
	dba_exists_##x, dba_delete_##x, dba_firstkey_##x, dba_nextkey_##x, \
	dba_optimize_##x, dba_sync_##x \
},

#define DBA_HND(x) DBA_NAMED_HND(x, x)

/* check whether the user has write access */
#define DBA_WRITE_CHECK \
	if(info->mode != DBA_WRITER && info->mode != DBA_TRUNC && info->mode != DBA_CREAT) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You cannot perform a modification to a database without proper access"); \
		RETURN_FALSE; \
	}

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
#if DBA_CDB_BUILTIN
    DBA_NAMED_HND(cdb_make, cdb)
#endif
#if DBA_DB2
	DBA_HND(db2)
#endif
#if DBA_DB3
	DBA_HND(db3)
#endif
#if DBA_FLATFILE
	DBA_HND(flatfile)
#endif
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static int le_db;
static int le_pdb;
/* }}} */

/* {{{ dba_close 
 */ 
static void dba_close(dba_info *info TSRMLS_DC)
{
	if(info->hnd) info->hnd->close(info TSRMLS_CC);
	if(info->path) efree(info->path);
	efree(info);
}
/* }}} */

/* {{{ dba_close_rsrc
 */
static void dba_close_rsrc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	dba_info *info = (dba_info *)rsrc->ptr;

	dba_close(info TSRMLS_CC);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(dba)
{
	le_db = zend_register_list_destructors_ex(dba_close_rsrc, NULL, "dba", module_number);
	le_pdb = zend_register_list_destructors_ex(NULL, dba_close_rsrc, "dba persistent", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(dba)
{
	return SUCCESS;
}
/* }}} */

#include "ext/standard/php_smart_str.h"

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(dba)
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
	zval **val, **key;

	if(ac != 3 || zend_get_parameters_ex(ac, &key, &val, &id) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(key);
	convert_to_string_ex(val);
	DBA_ID_GET;

	DBA_WRITE_CHECK;
	
	if(info->hnd->update(info, VALLEN(key), VALLEN(val), mode TSRMLS_CC) == SUCCESS)
		RETURN_TRUE;
	RETURN_FALSE;
}
/* }}} */

#define FREENOW if(args) efree(args); if(key) efree(key)

/* {{{ php_dba_open
 */
static void php_dba_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval ***args = (zval ***) NULL;
	int ac = ZEND_NUM_ARGS();
	dba_mode_t modenr;
	dba_info *info;
	dba_handler *hptr;
	char *key = NULL, *error = NULL;
	int keylen = 0;
	int i;
	
	if(ac < 3) {
		WRONG_PARAM_COUNT;
	}
	
	/* we pass additional args to the respective handler */
	args = emalloc(ac * sizeof(zval *));
	if (zend_get_parameters_array_ex(ac, args) != SUCCESS) {
		FREENOW;
		WRONG_PARAM_COUNT;
	}
		
	/* we only take string arguments */
	for (i = 0; i < ac; i++) {
		convert_to_string_ex(args[i]);
		keylen += Z_STRLEN_PP(args[i]);
	}

	if (persistent) {
		list_entry *le;
		
		/* calculate hash */
		key = emalloc(keylen);
		keylen = 0;
		
		for(i = 0; i < ac; i++) {
			memcpy(key+keylen, Z_STRVAL_PP(args[i]), Z_STRLEN_PP(args[i]));
			keylen += Z_STRLEN_PP(args[i]);
		}

		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), key, keylen+1, (void **) &le) == SUCCESS) {
			FREENOW;
			
			if (Z_TYPE_P(le) != le_pdb) {
				RETURN_FALSE;
			}
		
			info = (dba_info *)le->ptr;

			ZEND_REGISTER_RESOURCE(return_value, info, le_pdb);
			return;
		}
	}
	
	for (hptr = handler; hptr->name && strcasecmp(hptr->name, Z_STRVAL_PP(args[2])); hptr++);

	if (!hptr->name) {
		php_error_docref2(NULL TSRMLS_CC, Z_STRVAL_PP(args[0]), Z_STRVAL_PP(args[1]), E_WARNING, "No such handler: %s", Z_STRVAL_PP(args[2]));
		FREENOW;
		RETURN_FALSE;
	}

	switch (Z_STRVAL_PP(args[1])[0]) {
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
			php_error_docref2(NULL TSRMLS_CC, Z_STRVAL_PP(args[0]), Z_STRVAL_PP(args[1]), E_WARNING, "Illegal DBA mode");
			FREENOW;
			RETURN_FALSE;
	}
			
	info = ecalloc(sizeof(dba_info), 1);
	info->path = estrdup(Z_STRVAL_PP(args[0]));
	info->mode = modenr;
	info->argc = ac - 3;
	info->argv = args + 3;
	/* info->hnd is NULL here */

	if (hptr->open(info, &error TSRMLS_CC) != SUCCESS) {
		dba_close(info TSRMLS_CC);
		php_error_docref2(NULL TSRMLS_CC, Z_STRVAL_PP(args[0]), Z_STRVAL_PP(args[1]), E_WARNING, "Driver initialization failed for handler: %s%s%s", Z_STRVAL_PP(args[2]), error?": ":"", error?error:"");
		FREENOW;
		RETURN_FALSE;
	}

	info->hnd = hptr;
	info->argc = 0;
	info->argv = NULL;

	if (persistent) {
		list_entry new_le;

		Z_TYPE(new_le) = le_pdb;
		new_le.ptr = info;
		if (zend_hash_update(&EG(persistent_list), key, keylen+1, &new_le, sizeof(list_entry), NULL) == FAILURE) {
			FREENOW;
			RETURN_FALSE;
		}
	}

	ZEND_REGISTER_RESOURCE(return_value, info, (persistent ? le_pdb : le_db));
	FREENOW;
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

/* {{{ proto void dba_close(resource handle)
   Closes database */
PHP_FUNCTION(dba_close)
{
	DBA_ID_GET1;
	
	zend_list_delete(Z_RESVAL_PP(id));
}
/* }}} */

/* {{{ proto bool dba_exists(string key, int handle)
   Checks, if the specified key exists */
PHP_FUNCTION(dba_exists)
{
	DBA_ID_GET2;

	if(info->hnd->exists(info, VALLEN(key) TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string dba_fetch(string key, [int skip ,] int handle)
   Fetches the data associated with key */
PHP_FUNCTION(dba_fetch)
{
	char *val;
	int len = 0;
	DBA_ID_GET2_3;

	if (ac==3 && strcmp(info->hnd->name, "cdb")) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Handler %s does not support optional skip parameter", info->hnd->name);
	}
	if((val = info->hnd->fetch(info, VALLEN(key), skip, &len TSRMLS_CC)) != NULL) {
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

	fkey = info->hnd->firstkey(info, &len TSRMLS_CC);
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

	nkey = info->hnd->nextkey(info, &len TSRMLS_CC);
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
	
	if(info->hnd->delete(info, VALLEN(key) TSRMLS_CC) == SUCCESS)
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
	if(info->hnd->optimize(info TSRMLS_CC) == SUCCESS) {
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
	
	if(info->hnd->sync(info TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array dba_handlers()
   List configured databases */
PHP_FUNCTION(dba_handlers)
{
	dba_handler *hptr;

	if (ZEND_NUM_ARGS()!=0) {
		ZEND_WRONG_PARAM_COUNT();
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize array");
		RETURN_FALSE;
	}
	for(hptr = handler; hptr->name; hptr++) {
		add_next_index_string(return_value, hptr->name, 1);
 	}
}
/* }}} */

/* {{{ proto array dba_list()
   List configured databases */
PHP_FUNCTION(dba_list)
{
	ulong numitems, i;
	zend_rsrc_list_entry *le;
	dba_info *info;

	if (ZEND_NUM_ARGS()!=0) {
		ZEND_WRONG_PARAM_COUNT();
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize array");
		RETURN_FALSE;
	}
	numitems = zend_hash_next_free_element(&EG(regular_list));
	for (i=1; i<numitems; i++) {
		if (zend_hash_index_find(&EG(regular_list), i, (void **) &le)==FAILURE) {
			continue;
		}
		if (Z_TYPE_P(le) == le_db || Z_TYPE_P(le) == le_pdb) {
			info = (dba_info *)(le->ptr);
			add_index_string(return_value, i, info->path, 1);
		}
	}
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
