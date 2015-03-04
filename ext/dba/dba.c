/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_DBA

#include "php_ini.h"
#include <stdio.h>
#include <fcntl.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include "php_dba.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "ext/standard/flock_compat.h"

#include "php_gdbm.h"
#include "php_ndbm.h"
#include "php_dbm.h"
#include "php_cdb.h"
#include "php_db1.h"
#include "php_db2.h"
#include "php_db3.h"
#include "php_db4.h"
#include "php_flatfile.h"
#include "php_inifile.h"
#include "php_qdbm.h"
#include "php_tcadb.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dba_popen, 0, 0, 2)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, handlername)
	ZEND_ARG_VARIADIC_INFO(0, handler_parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dba_open, 0, 0, 2)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, handlername)
	ZEND_ARG_VARIADIC_INFO(0, handler_parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_close, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_exists, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dba_fetch, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, skip)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_key_split, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_firstkey, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_nextkey, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_delete, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_insert, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_replace, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_optimize, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_sync, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dba_handlers, 0, 0, 0)
	ZEND_ARG_INFO(0, full_info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dba_list, 0)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ dba_functions[]
 */
const zend_function_entry dba_functions[] = {
	PHP_FE(dba_open, arginfo_dba_open)
	PHP_FE(dba_popen, arginfo_dba_popen)
	PHP_FE(dba_close, arginfo_dba_close)
	PHP_FE(dba_delete, arginfo_dba_delete)
	PHP_FE(dba_exists, arginfo_dba_exists)
	PHP_FE(dba_fetch, arginfo_dba_fetch)
	PHP_FE(dba_insert, arginfo_dba_insert)
	PHP_FE(dba_replace, arginfo_dba_replace)
	PHP_FE(dba_firstkey, arginfo_dba_firstkey)
	PHP_FE(dba_nextkey, arginfo_dba_nextkey)
	PHP_FE(dba_optimize, arginfo_dba_optimize)
	PHP_FE(dba_sync, arginfo_dba_sync)
	PHP_FE(dba_handlers, arginfo_dba_handlers)
	PHP_FE(dba_list, arginfo_dba_list)
	PHP_FE(dba_key_split, arginfo_dba_key_split)
	PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(dba);
PHP_MSHUTDOWN_FUNCTION(dba);
PHP_MINFO_FUNCTION(dba);

ZEND_BEGIN_MODULE_GLOBALS(dba)
	char *default_handler;
	dba_handler *default_hptr;
ZEND_END_MODULE_GLOBALS(dba)

ZEND_DECLARE_MODULE_GLOBALS(dba)

#ifdef ZTS
#define DBA_G(v) TSRMG(dba_globals_id, zend_dba_globals *, v)
#else
#define DBA_G(v) (dba_globals.v)
#endif

static PHP_GINIT_FUNCTION(dba);

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
	PHP_MODULE_GLOBALS(dba),
	PHP_GINIT(dba),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_DBA
ZEND_GET_MODULE(dba)
#endif

/* {{{ macromania */

#define DBA_ID_PARS 											\
	zval *id; 													\
	dba_info *info = NULL; 										\
	int ac = ZEND_NUM_ARGS()

/* these are used to get the standard arguments */

/* {{{ php_dba_myke_key */
static size_t php_dba_make_key(zval *key, char **key_str, char **key_free)
{
	if (Z_TYPE_P(key) == IS_ARRAY) {
		zval *group, *name;
		HashPosition pos;
		size_t len;

		if (zend_hash_num_elements(Z_ARRVAL_P(key)) != 2) {
			php_error_docref(NULL, E_RECOVERABLE_ERROR, "Key does not have exactly two elements: (key, name)");
			return -1;
		}
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(key), &pos);
		group = zend_hash_get_current_data_ex(Z_ARRVAL_P(key), &pos);
		zend_hash_move_forward_ex(Z_ARRVAL_P(key), &pos);
		name = zend_hash_get_current_data_ex(Z_ARRVAL_P(key), &pos);
		convert_to_string_ex(group);
		convert_to_string_ex(name);
		if (Z_STRLEN_P(group) == 0) {
			*key_str = Z_STRVAL_P(name);
			*key_free = NULL;
			return Z_STRLEN_P(name);
		}
		len = spprintf(key_str, 0, "[%s]%s", Z_STRVAL_P(group), Z_STRVAL_P(name));
		*key_free = *key_str;
		return len;
	} else {
		zval tmp;
		int len;

		ZVAL_COPY(&tmp, key);
		convert_to_string(&tmp);

		*key_free = *key_str = estrndup(Z_STRVAL(tmp), Z_STRLEN(tmp));
		len = Z_STRLEN(tmp);

		zval_ptr_dtor(&tmp);
		return len;
	}
}
/* }}} */

#define DBA_GET2 												\
	zval *key;													\
	char *key_str, *key_free;									\
	size_t key_len; 											\
	if (zend_parse_parameters(ac, "zr", &key, &id) == FAILURE) { 	\
		return; 												\
	} 															\
	if ((key_len = php_dba_make_key(key, &key_str, &key_free)) == 0) {\
		RETURN_FALSE;											\
	}

#define DBA_GET2_3												\
	zval *key;													\
	char *key_str, *key_free;									\
	size_t key_len; 											\
	zend_long skip = 0;  											\
	switch(ac) {												\
	case 2: 													\
		if (zend_parse_parameters(ac, "zr", &key, &id) == FAILURE) { \
			return;												\
		} 														\
		break;  												\
	case 3: 													\
		if (zend_parse_parameters(ac, "zlr", &key, &skip, &id) == FAILURE) { \
			return;												\
		} 														\
		break;  												\
	default:													\
		WRONG_PARAM_COUNT; 										\
	} 															\
	if ((key_len = php_dba_make_key(key, &key_str, &key_free)) == 0) {\
		RETURN_FALSE;											\
	}


#define DBA_FETCH_RESOURCE(info, id)	\
	if ((info = (dba_info *)zend_fetch_resource2(Z_RES_P(id), "DBA identifier", le_db, le_pdb)) == NULL) { \
		RETURN_FALSE; \
	}

#define DBA_ID_GET2   DBA_ID_PARS; DBA_GET2;   DBA_FETCH_RESOURCE(info, id)
#define DBA_ID_GET2_3 DBA_ID_PARS; DBA_GET2_3; DBA_FETCH_RESOURCE(info, id)

#define DBA_ID_DONE												\
	if (key_free) efree(key_free)
/* a DBA handler must have specific routines */

#define DBA_NAMED_HND(alias, name, flags) \
{\
	#alias, flags, dba_open_##name, dba_close_##name, dba_fetch_##name, dba_update_##name, \
	dba_exists_##name, dba_delete_##name, dba_firstkey_##name, dba_nextkey_##name, \
	dba_optimize_##name, dba_sync_##name, dba_info_##name \
},

#define DBA_HND(name, flags) DBA_NAMED_HND(name, name, flags)

/* check whether the user has write access */
#define DBA_WRITE_CHECK \
	if(info->mode != DBA_WRITER && info->mode != DBA_TRUNC && info->mode != DBA_CREAT) { \
		php_error_docref(NULL, E_WARNING, "You cannot perform a modification to a database without proper access"); \
		RETURN_FALSE; \
	}

/* the same check, but with a call to DBA_ID_DONE before returning */
#define DBA_WRITE_CHECK_WITH_ID \
	if(info->mode != DBA_WRITER && info->mode != DBA_TRUNC && info->mode != DBA_CREAT) { \
		php_error_docref(NULL, E_WARNING, "You cannot perform a modification to a database without proper access"); \
		DBA_ID_DONE; \
		RETURN_FALSE; \
	}

/* }}} */

/* {{{ globals */

static dba_handler handler[] = {
#if DBA_GDBM
	DBA_HND(gdbm, DBA_LOCK_EXT) /* Locking done in library if set */
#endif
#if DBA_DBM
	DBA_HND(dbm, DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_NDBM
	DBA_HND(ndbm, DBA_LOCK_ALL) /* Could be done in library: filemode = 0644 + S_ENFMT */
#endif
#if DBA_CDB
	DBA_HND(cdb, DBA_STREAM_OPEN|DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_CDB_BUILTIN
    DBA_NAMED_HND(cdb_make, cdb, DBA_STREAM_OPEN|DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_DB1
	DBA_HND(db1, DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_DB2
	DBA_HND(db2, DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_DB3
	DBA_HND(db3, DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_DB4
	DBA_HND(db4, DBA_LOCK_ALL) /* No lock in lib */
#endif
#if DBA_INIFILE
	DBA_HND(inifile, DBA_STREAM_OPEN|DBA_LOCK_ALL|DBA_CAST_AS_FD) /* No lock in lib */
#endif
#if DBA_FLATFILE
	DBA_HND(flatfile, DBA_STREAM_OPEN|DBA_LOCK_ALL|DBA_NO_APPEND) /* No lock in lib */
#endif
#if DBA_QDBM
	DBA_HND(qdbm, DBA_LOCK_EXT)
#endif
#if DBA_TCADB
	DBA_HND(tcadb, DBA_LOCK_ALL)
#endif
	{ NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

#if DBA_FLATFILE
#define DBA_DEFAULT "flatfile"
#elif DBA_DB4
#define DBA_DEFAULT "db4"
#elif DBA_DB3
#define DBA_DEFAULT "db3"
#elif DBA_DB2
#define DBA_DEFAULT "db2"
#elif DBA_DB1
#define DBA_DEFAULT "db1"
#elif DBA_GDBM
#define DBA_DEFAULT "gdbm"
#elif DBA_NBBM
#define DBA_DEFAULT "ndbm"
#elif DBA_DBM
#define DBA_DEFAULT "dbm"
#elif DBA_QDBM
#define DBA_DEFAULT "qdbm"
#elif DBA_TCADB
#define DBA_DEFAULT "tcadb"
#else
#define DBA_DEFAULT ""
#endif
/* cdb/cdb_make and ini are no option here */

static int le_db;
static int le_pdb;
/* }}} */

/* {{{ dba_fetch_resource
PHPAPI void dba_fetch_resource(dba_info **pinfo, zval **id)
{
	dba_info *info;
	DBA_ID_FETCH
	*pinfo = info;
}
*/
/* }}} */

/* {{{ dba_get_handler
PHPAPI dba_handler *dba_get_handler(const char* handler_name)
{
	dba_handler *hptr;
	for (hptr = handler; hptr->name && strcasecmp(hptr->name, handler_name); hptr++);
	return hptr;
}
*/
/* }}} */

/* {{{ dba_close
 */
static void dba_close(dba_info *info)
{
	if (info->hnd) {
		info->hnd->close(info);
	}
	if (info->path) {
		pefree(info->path, info->flags&DBA_PERSISTENT);
	}
	if (info->fp && info->fp != info->lock.fp) {
		if (info->flags & DBA_PERSISTENT) {
			php_stream_pclose(info->fp);
		} else {
			php_stream_close(info->fp);
		}
	}
	if (info->lock.fp) {
		if (info->flags & DBA_PERSISTENT) {
			php_stream_pclose(info->lock.fp);
		} else {
			php_stream_close(info->lock.fp);
		}
	}
	if (info->lock.name) {
		pefree(info->lock.name, info->flags&DBA_PERSISTENT);
	}
	pefree(info, info->flags&DBA_PERSISTENT);
}
/* }}} */

/* {{{ dba_close_rsrc
 */
static void dba_close_rsrc(zend_resource *rsrc)
{
	dba_info *info = (dba_info *)rsrc->ptr;

	dba_close(info);
}
/* }}} */

/* {{{ dba_close_pe_rsrc_deleter */
int dba_close_pe_rsrc_deleter(zval *el, void *pDba)
{
	return ((zend_resource *)Z_PTR_P(el))->ptr == pDba ? ZEND_HASH_APPLY_REMOVE: ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ dba_close_pe_rsrc */
static void dba_close_pe_rsrc(zend_resource *rsrc)
{
	dba_info *info = (dba_info *)rsrc->ptr;

	/* closes the resource by calling dba_close_rsrc() */
	zend_hash_apply_with_argument(&EG(persistent_list), dba_close_pe_rsrc_deleter, info);
}
/* }}} */

/* {{{ PHP_INI
 */
ZEND_INI_MH(OnUpdateDefaultHandler)
{
	dba_handler *hptr;

	if (!new_value->len) {
		DBA_G(default_hptr) = NULL;
		return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}

	for (hptr = handler; hptr->name && strcasecmp(hptr->name, new_value->val); hptr++);

	if (!hptr->name) {
		php_error_docref(NULL, E_WARNING, "No such handler: %s", new_value->val);
		return FAILURE;
	}
	DBA_G(default_hptr) = hptr;
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("dba.default_handler", DBA_DEFAULT, PHP_INI_ALL, OnUpdateDefaultHandler, default_handler,    zend_dba_globals, dba_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(dba)
{
	dba_globals->default_handler = "";
	dba_globals->default_hptr    = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(dba)
{
	REGISTER_INI_ENTRIES();
	le_db = zend_register_list_destructors_ex(dba_close_rsrc, NULL, "dba", module_number);
	le_pdb = zend_register_list_destructors_ex(dba_close_pe_rsrc, dba_close_rsrc, "dba persistent", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(dba)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

#include "zend_smart_str.h"

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
	if (handlers.s) {
		smart_str_0(&handlers);
		php_info_print_table_row(2, "Supported handlers", handlers.s->val);
		smart_str_free(&handlers);
	} else {
		php_info_print_table_row(2, "Supported handlers", "none");
	}
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_dba_update
 */
static void php_dba_update(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	size_t val_len;
	zval *id;
	dba_info *info = NULL;
	int ac = ZEND_NUM_ARGS();
	zval *key;
	char *val;
	char *key_str, *key_free;
	size_t key_len;

	if (zend_parse_parameters(ac, "zsr", &key, &val, &val_len, &id) == FAILURE) {
		return;
	}

	if ((key_len = php_dba_make_key(key, &key_str, &key_free)) == 0) {
		RETURN_FALSE;
	}

	DBA_FETCH_RESOURCE(info, id);

	DBA_WRITE_CHECK_WITH_ID;

	if (info->hnd->update(info, key_str, key_len, val, val_len, mode) == SUCCESS) {
		DBA_ID_DONE;
		RETURN_TRUE;
	}

	DBA_ID_DONE;
	RETURN_FALSE;
}
/* }}} */

#define FREENOW if(args) {int i; for (i=0; i<ac; i++) { zval_ptr_dtor(&args[i]); } efree(args);} if(key) efree(key)

/* {{{ php_find_dbm
 */
dba_info *php_dba_find(const char* path)
{
	zend_resource *le;
	dba_info *info;
	int numitems, i;

	numitems = zend_hash_next_free_element(&EG(regular_list));
	for (i=1; i<numitems; i++) {
		if ((le = zend_hash_index_find_ptr(&EG(regular_list), i)) == NULL) {
			continue;
		}
		if (le->type == le_db || le->type == le_pdb) {
			info = (dba_info *)(le->ptr);
			if (!strcmp(info->path, path)) {
				return (dba_info *)(le->ptr);
			}
		}
	}

	return NULL;
}
/* }}} */

/* {{{ php_dba_open
 */
static void php_dba_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval *args = NULL;
	int ac = ZEND_NUM_ARGS();
	dba_mode_t modenr;
	dba_info *info, *other;
	dba_handler *hptr;
	char *key = NULL, *error = NULL;
	int keylen = 0;
	int i;
	int lock_mode, lock_flag, lock_dbf = 0;
	char *file_mode;
	char mode[4], *pmode, *lock_file_mode = NULL;
	int persistent_flag = persistent ? STREAM_OPEN_PERSISTENT : 0;
	zend_string *opened_path = NULL;
	char *lock_name;

	if (ac < 2) {
		WRONG_PARAM_COUNT;
	}

	/* we pass additional args to the respective handler */
	args = safe_emalloc(ac, sizeof(zval), 0);
	if (zend_get_parameters_array_ex(ac, args) != SUCCESS) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	/* we only take string arguments */
	for (i = 0; i < ac; i++) {
		if (Z_TYPE(args[i]) != IS_STRING) {
			convert_to_string_ex(&args[i]);
		} else if (Z_REFCOUNTED(args[i])) {
			Z_ADDREF(args[i]);
		}
		keylen += Z_STRLEN(args[i]);
	}

	if (persistent) {
		zend_resource *le;

		/* calculate hash */
		key = safe_emalloc(keylen, 1, 1);
		key[keylen] = '\0';
		keylen = 0;

		for(i = 0; i < ac; i++) {
			memcpy(key+keylen, Z_STRVAL(args[i]), Z_STRLEN(args[i]));
			keylen += Z_STRLEN(args[i]);
		}

		/* try to find if we already have this link in our persistent list */
		if ((le = zend_hash_str_find_ptr(&EG(persistent_list), key, keylen)) != NULL) {
			FREENOW;

			if (le->type != le_pdb) {
				RETURN_FALSE;
			}

			info = (dba_info *)le->ptr;

			GC_REFCOUNT(le)++;
			RETURN_RES(le);
			return;
		}
	}

	if (ac==2) {
		hptr = DBA_G(default_hptr);
		if (!hptr) {
			php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "No default handler selected");
			FREENOW;
			RETURN_FALSE;
		}
	} else {
		for (hptr = handler; hptr->name && strcasecmp(hptr->name, Z_STRVAL(args[2])); hptr++);
	}

	if (!hptr->name) {
		php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "No such handler: %s", Z_STRVAL(args[2]));
		FREENOW;
		RETURN_FALSE;
	}

	/* Check mode: [rwnc][fl]?t?
	 * r: Read
	 * w: Write
	 * n: Create/Truncate
	 * c: Create
	 *
	 * d: force lock on database file
	 * l: force lock on lck file
	 * -: ignore locking
	 *
	 * t: test open database, warning if locked
	 */
	strlcpy(mode, Z_STRVAL(args[1]), sizeof(mode));
	pmode = &mode[0];
	if (pmode[0] && (pmode[1]=='d' || pmode[1]=='l' || pmode[1]=='-')) { /* force lock on db file or lck file or disable locking */
		switch (pmode[1]) {
		case 'd':
			lock_dbf = 1;
			if ((hptr->flags & DBA_LOCK_ALL) == 0) {
				lock_flag = (hptr->flags & DBA_LOCK_ALL);
				break;
			}
			/* no break */
		case 'l':
			lock_flag = DBA_LOCK_ALL;
			if ((hptr->flags & DBA_LOCK_ALL) == 0) {
				php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_NOTICE, "Handler %s does locking internally", hptr->name);
			}
			break;
		default:
		case '-':
			if ((hptr->flags & DBA_LOCK_ALL) == 0) {
				php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Locking cannot be disabled for handler %s", hptr->name);
				FREENOW;
				RETURN_FALSE;
			}
			lock_flag = 0;
			break;
		}
	} else {
		lock_flag = (hptr->flags&DBA_LOCK_ALL);
		lock_dbf = 1;
	}
	switch (*pmode++) {
		case 'r':
			modenr = DBA_READER;
			lock_mode = (lock_flag & DBA_LOCK_READER) ? LOCK_SH : 0;
			file_mode = "r";
			break;
		case 'w':
			modenr = DBA_WRITER;
			lock_mode = (lock_flag & DBA_LOCK_WRITER) ? LOCK_EX : 0;
			file_mode = "r+b";
			break;
		case 'c':
			modenr = DBA_CREAT;
			lock_mode = (lock_flag & DBA_LOCK_CREAT) ? LOCK_EX : 0;
			if (lock_mode) {
				if (lock_dbf) {
					/* the create/append check will be done on the lock
					 * when the lib opens the file it is already created
					 */
					file_mode = "r+b";       /* read & write, seek 0 */
					lock_file_mode = "a+b";  /* append */
				} else {
					file_mode = "a+b";       /* append */
					lock_file_mode = "w+b";  /* create/truncate */
				}
			} else {
				file_mode = "a+b";
			}
			/* In case of the 'a+b' append mode, the handler is responsible
			 * to handle any rewind problems (see flatfile handler).
			 */
			break;
		case 'n':
			modenr = DBA_TRUNC;
			lock_mode = (lock_flag & DBA_LOCK_TRUNC) ? LOCK_EX : 0;
			file_mode = "w+b";
			break;
		default:
			php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Illegal DBA mode");
			FREENOW;
			RETURN_FALSE;
	}
	if (!lock_file_mode) {
		lock_file_mode = file_mode;
	}
	if (*pmode=='d' || *pmode=='l' || *pmode=='-') {
		pmode++; /* done already - skip here */
	}
	if (*pmode=='t') {
		pmode++;
		if (!lock_flag) {
			php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "You cannot combine modifiers - (no lock) and t (test lock)");
			FREENOW;
			RETURN_FALSE;
		}
		if (!lock_mode) {
			if ((hptr->flags & DBA_LOCK_ALL) == 0) {
				php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Handler %s uses its own locking which doesn't support mode modifier t (test lock)", hptr->name);
				FREENOW;
				RETURN_FALSE;
			} else {
				php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Handler %s doesn't uses locking for this mode which makes modifier t (test lock) obsolete", hptr->name);
				FREENOW;
				RETURN_FALSE;
			}
		} else {
			lock_mode |= LOCK_NB; /* test =: non blocking */
		}
	}
	if (*pmode) {
		php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Illegal DBA mode");
		FREENOW;
		RETURN_FALSE;
	}

	info = pemalloc(sizeof(dba_info), persistent);
	memset(info, 0, sizeof(dba_info));
	info->path = pestrdup(Z_STRVAL(args[0]), persistent);
	info->mode = modenr;
	info->argc = ac - 3;
	info->argv = args + 3;
	info->flags = (hptr->flags & ~DBA_LOCK_ALL) | (lock_flag & DBA_LOCK_ALL) | (persistent ? DBA_PERSISTENT : 0);
	info->lock.mode = lock_mode;

	/* if any open call is a locking call:
	 * check if we already habe a locking call open that should block this call
	 * the problem is some systems would allow read during write
	 */
	if (hptr->flags & DBA_LOCK_ALL) {
		if ((other = php_dba_find(info->path)) != NULL) {
			if (   ( (lock_mode&LOCK_EX)        && (other->lock.mode&(LOCK_EX|LOCK_SH)) )
			    || ( (other->lock.mode&LOCK_EX) && (lock_mode&(LOCK_EX|LOCK_SH))        )
			   ) {
				error = "Unable to establish lock (database file already open)"; /* force failure exit */
			}
		}
	}

	if (!error && lock_mode) {
		if (lock_dbf) {
			lock_name = Z_STRVAL(args[0]);
		} else {
			spprintf(&lock_name, 0, "%s.lck", info->path);
			if (!strcmp(file_mode, "r")) {
				/* when in read only mode try to use existing .lck file first */
				/* do not log errors for .lck file while in read ony mode on .lck file */
				lock_file_mode = "rb";
				info->lock.fp = php_stream_open_wrapper(lock_name, lock_file_mode, STREAM_MUST_SEEK|IGNORE_PATH|persistent_flag, &opened_path);
			}
			if (!info->lock.fp) {
				/* when not in read mode or failed to open .lck file read only. now try again in create(write) mode and log errors */
				lock_file_mode = "a+b";
			} else {
				if (opened_path) {
					info->lock.name = pestrndup(opened_path->val, opened_path->len, persistent);
					zend_string_release(opened_path);
				}
			}
		}
		if (!info->lock.fp) {
			info->lock.fp = php_stream_open_wrapper(lock_name, lock_file_mode, STREAM_MUST_SEEK|REPORT_ERRORS|IGNORE_PATH|persistent_flag, &opened_path);
			if (info->lock.fp) {
				if (lock_dbf) {
					/* replace the path info with the real path of the opened file */
					pefree(info->path, persistent);
					info->path = pestrndup(opened_path->val, opened_path->len, persistent);
				}
				/* now store the name of the lock */
				info->lock.name = pestrndup(opened_path->val, opened_path->len, persistent);
				zend_string_release(opened_path);
			}
		}
		if (!lock_dbf) {
			efree(lock_name);
		}
		if (!info->lock.fp) {
			dba_close(info);
			/* stream operation already wrote an error message */
			FREENOW;
			RETURN_FALSE;
		}
		if (!php_stream_supports_lock(info->lock.fp)) {
			error = "Stream does not support locking";
		}
		if (php_stream_lock(info->lock.fp, lock_mode)) {
			error = "Unable to establish lock"; /* force failure exit */
		}
	}

	/* centralised open stream for builtin */
	if (!error && (hptr->flags&DBA_STREAM_OPEN)==DBA_STREAM_OPEN) {
		if (info->lock.fp && lock_dbf) {
			info->fp = info->lock.fp; /* use the same stream for locking and database access */
		} else {
			info->fp = php_stream_open_wrapper(info->path, file_mode, STREAM_MUST_SEEK|REPORT_ERRORS|IGNORE_PATH|persistent_flag, NULL);
		}
		if (!info->fp) {
			dba_close(info);
			/* stream operation already wrote an error message */
			FREENOW;
			RETURN_FALSE;
		}
		if (hptr->flags & (DBA_NO_APPEND|DBA_CAST_AS_FD)) {
			/* Needed because some systems do not allow to write to the original
			 * file contents with O_APPEND being set.
			 */
			if (SUCCESS != php_stream_cast(info->fp, PHP_STREAM_AS_FD, (void*)&info->fd, 1)) {
				php_error_docref(NULL, E_WARNING, "Could not cast stream");
				dba_close(info);
				FREENOW;
				RETURN_FALSE;
#ifdef F_SETFL
			} else if (modenr == DBA_CREAT) {
				int flags = fcntl(info->fd, F_SETFL);
				fcntl(info->fd, F_SETFL, flags & ~O_APPEND);
#endif
			}

		}
	}

	if (error || hptr->open(info, &error) != SUCCESS) {
		dba_close(info);
		php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Driver initialization failed for handler: %s%s%s", hptr->name, error?": ":"", error?error:"");
		FREENOW;
		RETURN_FALSE;
	}

	info->hnd = hptr;
	info->argc = 0;
	info->argv = NULL;

	if (persistent) {
		zend_resource new_le;

		new_le.type = le_pdb;
		new_le.ptr = info;
		if (zend_hash_str_update_mem(&EG(persistent_list), key, keylen, &new_le, sizeof(zend_resource)) == NULL) {
			dba_close(info);
			php_error_docref2(NULL, Z_STRVAL(args[0]), Z_STRVAL(args[1]), E_WARNING, "Could not register persistent resource");
			FREENOW;
			RETURN_FALSE;
		}
	}

	RETVAL_RES(zend_register_resource(info, (persistent ? le_pdb : le_db)));
	FREENOW;
}
/* }}} */
#undef FREENOW

/* {{{ proto resource dba_popen(string path, string mode [, string handlername, string ...])
   Opens path using the specified handler in mode persistently */
PHP_FUNCTION(dba_popen)
{
	php_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto resource dba_open(string path, string mode [, string handlername, string ...])
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
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &id) == FAILURE) {
		return;
	}

	DBA_FETCH_RESOURCE(info, id);

	zend_list_close(Z_RES_P(id));
}
/* }}} */

/* {{{ proto bool dba_exists(string key, resource handle)
   Checks, if the specified key exists */
PHP_FUNCTION(dba_exists)
{
	DBA_ID_GET2;

	if(info->hnd->exists(info, key_str, key_len) == SUCCESS) {
		DBA_ID_DONE;
		RETURN_TRUE;
	}
	DBA_ID_DONE;
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string dba_fetch(string key, [int skip ,] resource handle)
   Fetches the data associated with key */
PHP_FUNCTION(dba_fetch)
{
	char *val;
	int len = 0;
	DBA_ID_GET2_3;

	if (ac==3) {
		if (!strcmp(info->hnd->name, "cdb")) {
			if (skip < 0) {
				php_error_docref(NULL, E_NOTICE, "Handler %s accepts only skip values greater than or equal to zero, using skip=0", info->hnd->name);
				skip = 0;
			}
		} else if (!strcmp(info->hnd->name, "inifile")) {
			/* "-1" is compareable to 0 but allows a non restrictive
			 * access which is fater. For example 'inifile' uses this
			 * to allow faster access when the key was already found
			 * using firstkey/nextkey. However explicitly setting the
			 * value to 0 ensures the first value.
			 */
			if (skip < -1) {
				php_error_docref(NULL, E_NOTICE, "Handler %s accepts only skip value -1 and greater, using skip=0", info->hnd->name);
				skip = 0;
			}
		} else {
			php_error_docref(NULL, E_NOTICE, "Handler %s does not support optional skip parameter, the value will be ignored", info->hnd->name);
			skip = 0;
		}
	} else {
		skip = 0;
	}
	if((val = info->hnd->fetch(info, key_str, key_len, skip, &len)) != NULL) {
		DBA_ID_DONE;
		RETVAL_STRINGL(val, len);
		efree(val);
		return;
	}
	DBA_ID_DONE;
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array|false dba_key_split(string key)
   Splits an inifile key into an array of the form array(0=>group,1=>value_name) but returns false if input is false or null */
PHP_FUNCTION(dba_key_split)
{
	zval *zkey;
	char *key, *name;
	size_t key_len;

	if (ZEND_NUM_ARGS() != 1) {
		WRONG_PARAM_COUNT;
	}
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &zkey) == SUCCESS) {
		if (Z_TYPE_P(zkey) == IS_NULL || (Z_TYPE_P(zkey) == IS_FALSE)) {
			RETURN_BOOL(0);
		}
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &key_len) == FAILURE) {
		RETURN_BOOL(0);
	}
	array_init(return_value);
	if (key[0] == '[' && (name = strchr(key, ']')) != NULL) {
		add_next_index_stringl(return_value, key+1, name - (key + 1));
		add_next_index_stringl(return_value, name+1, key_len - (name - key + 1));
	} else {
		add_next_index_stringl(return_value, "", 0);
		add_next_index_stringl(return_value, key, key_len);
	}
}
/* }}} */

/* {{{ proto string dba_firstkey(resource handle)
   Resets the internal key pointer and returns the first key */
PHP_FUNCTION(dba_firstkey)
{
	char *fkey;
	int len;
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &id) == FAILURE) {
		return;
	}

	DBA_FETCH_RESOURCE(info, id);

	fkey = info->hnd->firstkey(info, &len);

	if (fkey) {
		RETVAL_STRINGL(fkey, len);
		efree(fkey);
		return;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string dba_nextkey(resource handle)
   Returns the next key */
PHP_FUNCTION(dba_nextkey)
{
	char *nkey;
	int len;
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &id) == FAILURE) {
		return;
	}

	DBA_FETCH_RESOURCE(info, id);

	nkey = info->hnd->nextkey(info, &len);

	if (nkey) {
		RETVAL_STRINGL(nkey, len);
		efree(nkey);
		return;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool dba_delete(string key, resource handle)
   Deletes the entry associated with key
   If inifile: remove all other key lines */
PHP_FUNCTION(dba_delete)
{
	DBA_ID_GET2;

	DBA_WRITE_CHECK_WITH_ID;

	if(info->hnd->delete(info, key_str, key_len) == SUCCESS)
	{
		DBA_ID_DONE;
		RETURN_TRUE;
	}
	DBA_ID_DONE;
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool dba_insert(string key, string value, resource handle)
   If not inifile: Insert value as key, return false, if key exists already
   If inifile: Add vakue as key (next instance of key) */
PHP_FUNCTION(dba_insert)
{
	php_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool dba_replace(string key, string value, resource handle)
   Inserts value as key, replaces key, if key exists already
   If inifile: remove all other key lines */
PHP_FUNCTION(dba_replace)
{
	php_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool dba_optimize(resource handle)
   Optimizes (e.g. clean up, vacuum) database */
PHP_FUNCTION(dba_optimize)
{
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &id) == FAILURE) {
		return;
	}

	DBA_FETCH_RESOURCE(info, id);

	DBA_WRITE_CHECK;

	if (info->hnd->optimize(info) == SUCCESS) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool dba_sync(resource handle)
   Synchronizes database */
PHP_FUNCTION(dba_sync)
{
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &id) == FAILURE) {
		return;
	}

	DBA_FETCH_RESOURCE(info, id);

	if (info->hnd->sync(info) == SUCCESS) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array dba_handlers([bool full_info])
   List configured database handlers */
PHP_FUNCTION(dba_handlers)
{
	dba_handler *hptr;
	zend_bool full_info = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &full_info) == FAILURE) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for(hptr = handler; hptr->name; hptr++) {
		if (full_info) {
			// TODO: avoid reallocation ???
			char *str = hptr->info(hptr, NULL);
			add_assoc_string(return_value, hptr->name, str);
			efree(str);
		} else {
			add_next_index_string(return_value, hptr->name);
		}
 	}
}
/* }}} */

/* {{{ proto array dba_list()
   List opened databases */
PHP_FUNCTION(dba_list)
{
	zend_ulong numitems, i;
	zend_resource *le;
	dba_info *info;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	array_init(return_value);

	numitems = zend_hash_next_free_element(&EG(regular_list));
	for (i=1; i<numitems; i++) {
		if ((le = zend_hash_index_find_ptr(&EG(regular_list), i)) == NULL) {
			continue;
		}
		if (le->type == le_db || le->type == le_pdb) {
			info = (dba_info *)(le->ptr);
			add_index_string(return_value, i, info->path);
		}
	}
}
/* }}} */

#endif /* HAVE_DBA */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
