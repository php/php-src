/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999 PHP Development Team (See Credits file)           |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sas@schell.de>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#if HAVE_DBA

#include "php3_dba.h"

#include "php3_gdbm.h"
#include "php3_ndbm.h"
#include "php3_dbm.h"
#include "php3_cdb.h"
#include "php3_db2.h"

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
	{NULL,NULL,NULL}
};

static PHP_MINIT_FUNCTION(dba);
static PHP_MSHUTDOWN_FUNCTION(dba);
static PHP_MINFO_FUNCTION(dba);

php3_module_entry dba_module_entry = {
	    "DataBase API", dba_functions, 
		PHP_MINIT(dba), 
		PHP_MSHUTDOWN(dba), 
		NULL, NULL,
		PHP_MINFO(dba),
		STANDARD_MODULE_PROPERTIES
};

typedef struct dba_handler {
	char *name;
	int (*open)(dba_info *);
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
	int type, ac = ARG_COUNT(ht)

/* these are used to get the standard arguments */

#define DBA_GET1 												\
	if(ac != 1 || getParametersEx(ac, &id) != SUCCESS) { 		\
		WRONG_PARAM_COUNT; 										\
	}

#define DBA_GET2 												\
	pval **key; 												\
	if(ac != 2 || getParametersEx(ac, &key, &id) != SUCCESS) { 	\
		WRONG_PARAM_COUNT; 										\
	} 															\
	convert_to_string_ex(key)

#define DBA_IF_NOT_CORRECT_TYPE(link_id) 						\
	info = php3_list_find(link_id, &type); 						\
	if(!info || (type != GLOBAL(le_db) && type != GLOBAL(le_pdb)))
	
#define DBA_ID_GET 												\
	convert_to_long_ex(id); 									\
	DBA_IF_NOT_CORRECT_TYPE((*id)->value.lval) { 				\
		php_error(E_WARNING, "Unable to find DBA identifier %d", \
				(*id)->value.lval); 							\
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
		php_error(E_WARNING, "you cannot perform a modification to a database without proper access"); \
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
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static int le_db;
static int le_pdb;
static HashTable ht_keys;
/* }}} */

/* {{{ helper routines */
	/* {{{ dba_close */

static void dba_close(dba_info *info)
{
	if(info->hnd) info->hnd->close(info);
	if(info->path) free(info->path);
	free(info);
}
/* }}} */
	/* {{{ php3_minit_dba */

static PHP_MINIT_FUNCTION(dba)
{
	zend_hash_init(&ht_keys, 0, NULL, NULL, 1);
	GLOBAL(le_db) = register_list_destructors(dba_close, NULL);
	GLOBAL(le_pdb) = register_list_destructors(NULL, dba_close);
	return SUCCESS;
}
/* }}} */
	/* {{{ php3_mshutdown_dba */

static PHP_MSHUTDOWN_FUNCTION(dba)
{
	zend_hash_destroy(&ht_keys);
	return SUCCESS;
}
/* }}} */
	/* {{{ php3_info_dba */

static PHP_MINFO_FUNCTION(dba)
{
	dba_handler *hptr;
	
	PUTS("V1 ($Id$)");
	for(hptr = handler; hptr->name; hptr++) {
		PUTS(" ");
		PUTS(hptr->name);
	}
}
/* }}} */	
	/* {{{ _php3_dba_update */

static void _php3_dba_update(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	DBA_ID_PARS;
	pval **val, **key;

	if(ac != 3 || getParametersEx(ac, &key, &val, &id) != SUCCESS) {
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
	/* {{{ _php3_dba_open */

#define FREENOW if(args) efree(args); if(key) efree(key)

static void _php3_dba_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval ***args = (pval ***) NULL;
	int ac = ARG_COUNT(ht);
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
	if(getParametersArrayEx(ac, args) != SUCCESS) {
		FREENOW;
		WRONG_PARAM_COUNT;
	}
		
	/* we only take string arguments */
	for(i = 0; i < ac; i++) {
		convert_to_string_ex(args[i]);
		keylen += (*args[i])->value.str.len;
	}

	if(persistent) {
		/* calculate hash */
		key = emalloc(keylen);
		keylen = 0;
		
		for(i = 0; i < ac; i++) {
			memcpy(key+keylen,(*args[i])->value.str.val,(*args[i])->value.str.len);
			keylen += (*args[i])->value.str.len;
		}
		
		if(zend_hash_find(&ht_keys, key, keylen, (void **) &info) == SUCCESS) {
			FREENOW;
			RETURN_LONG(php3_list_insert(info, GLOBAL(le_pdb)));
		}
	}
	
	for(hptr = handler; hptr->name &&
			strcasecmp(hptr->name, (*args[2])->value.str.val); hptr++);

	if(!hptr->name) {
		php_error(E_WARNING, "no such handler: %s", (*args[2])->value.str.val);
		FREENOW;
		RETURN_FALSE;
	}

	switch((*args[1])->value.str.val[0]) {
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
			php_error(E_WARNING,"illegal DBA mode: %s",(*args[1])->value.str.val);
			FREENOW;
			RETURN_FALSE;
	}
			
	info = malloc(sizeof(*info));
	memset(info, 0, sizeof(info));
	info->path = strdup((*args[0])->value.str.val);
	info->mode = modenr;
	info->argc = ac - 3;
	info->argv = args + 3;
	info->hnd = NULL;

	if(hptr->open(info) != SUCCESS) {
		dba_close(info);
		php_error(E_WARNING, "driver initialization failed");
		FREENOW;
		RETURN_FALSE;
	}
	info->hnd = hptr;
	info->argc = 0;
	info->argv = NULL;

	listid = php3_list_insert(info, persistent?GLOBAL(le_pdb):GLOBAL(le_db));
	if(persistent) {
		zend_hash_update(&ht_keys, key, keylen, info, sizeof(*info), NULL);
	}
	
	FREENOW;
	RETURN_LONG(listid);
}
#undef FREENOW
/* }}} */
/* }}} */

/* {{{ proto int dba_popen(string path, string mode, string handlername[, ...])
   opens path using the specified handler in mode persistently */
PHP_FUNCTION(dba_popen)
{
	_php3_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int dba_open(string path, string mode, string handlername[, ...])
   opens path using the specified handler in mode*/
PHP_FUNCTION(dba_open)
{
	_php3_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void dba_close(int handle)
   closes database */
PHP_FUNCTION(dba_close)
{
	DBA_ID_GET1;	
	
	php3_list_delete((*id)->value.lval);
}
/* }}} */

/* {{{ proto bool dba_exists(string key, int handle)
   checks, if the specified key exists */
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
   fetches the data associated with key */
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
   resets the internal key pointer and returns the first key */
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
   returns the next key */
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
   deletes the entry associated with key */
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
   inserts value as key, returns false, if key exists already */
PHP_FUNCTION(dba_insert)
{
	_php3_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool dba_replace(string key, string value, int handle)
   inserts value as key, replaces key, if key exists already */
PHP_FUNCTION(dba_replace)
{
	_php3_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool dba_optimize(int handle)
   optimizes (e.g. clean up, vacuum) database */
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
   synchronizes database */
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
