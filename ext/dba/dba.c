/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef HAVE_DBA

#include "php_ini.h"
#include <stdio.h>
#include <fcntl.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include "php_dba.h"
#include "ext/standard/info.h"
#include "ext/standard/flock_compat.h" /* Compatibility for Windows */

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
#include "php_lmdb.h"
#include "dba_arginfo.h"

PHP_MINIT_FUNCTION(dba);
PHP_MSHUTDOWN_FUNCTION(dba);
PHP_MINFO_FUNCTION(dba);

ZEND_BEGIN_MODULE_GLOBALS(dba)
	const char *default_handler;
	const dba_handler *default_hptr;
	HashTable connections;
ZEND_END_MODULE_GLOBALS(dba)

ZEND_DECLARE_MODULE_GLOBALS(dba)

#define DBA_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(dba, v)

static PHP_GINIT_FUNCTION(dba);
static PHP_GSHUTDOWN_FUNCTION(dba);

zend_module_entry dba_module_entry = {
	STANDARD_MODULE_HEADER,
	"dba",
	ext_functions,
	PHP_MINIT(dba),
	PHP_MSHUTDOWN(dba),
	NULL,
	NULL,
	PHP_MINFO(dba),
	PHP_DBA_VERSION,
	PHP_MODULE_GLOBALS(dba),
	PHP_GINIT(dba),
	PHP_GSHUTDOWN(dba),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_DBA
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(dba)
#endif

/* {{{ php_dba_make_key */
static zend_string* php_dba_make_key(HashTable *key)
{
	zval *group, *name;
	zend_string *group_str, *name_str;
	HashPosition pos;

	if (zend_hash_num_elements(key) != 2) {
		zend_argument_error(NULL, 1, "must have exactly two elements: \"key\" and \"name\"");
		return NULL;
	}

	// TODO: Use ZEND_HASH_FOREACH_VAL() API?
	zend_hash_internal_pointer_reset_ex(key, &pos);
	group = zend_hash_get_current_data_ex(key, &pos);
	group_str = zval_try_get_string(group);
	if (!group_str) {
		return NULL;
	}

	zend_hash_move_forward_ex(key, &pos);
	name = zend_hash_get_current_data_ex(key, &pos);
	name_str = zval_try_get_string(name);
	if (!name_str) {
		zend_string_release_ex(group_str, false);
		return NULL;
	}

	// TODO: Check ZSTR_LEN(name) != 0
	if (ZSTR_LEN(group_str) == 0) {
		zend_string_release_ex(group_str, false);
		return name_str;
	}

	zend_string *key_str = zend_strpprintf(0, "[%s]%s", ZSTR_VAL(group_str), ZSTR_VAL(name_str));
	zend_string_release_ex(group_str, false);
	zend_string_release_ex(name_str, false);
	return key_str;
}
/* }}} */

#define DBA_RELEASE_HT_KEY_CREATION() if (key_ht) {zend_string_release_ex(key_str, false);}

#define CHECK_DBA_CONNECTION(info)	\
	if (info == NULL) { \
		zend_throw_error(NULL, "DBA connection has already been closed"); \
		RETURN_THROWS(); \
	}

/* check whether the user has write access */
#define DBA_WRITE_CHECK(info) \
	if ((info)->mode != DBA_WRITER && (info)->mode != DBA_TRUNC && (info)->mode != DBA_CREAT) { \
		php_error_docref(NULL, E_WARNING, "Cannot perform a modification on a readonly database"); \
		RETURN_FALSE; \
	}

/* a DBA handler must have specific routines */

#define DBA_NAMED_HND(alias, name, flags) \
{\
	#alias, flags, dba_open_##name, dba_close_##name, dba_fetch_##name, dba_update_##name, \
	dba_exists_##name, dba_delete_##name, dba_firstkey_##name, dba_nextkey_##name, \
	dba_optimize_##name, dba_sync_##name, dba_info_##name \
},

#define DBA_HND(name, flags) DBA_NAMED_HND(name, name, flags)

/* }}} */

/* {{{ globals */

static const dba_handler handler[] = {
#ifdef DBA_GDBM
	DBA_HND(gdbm, DBA_LOCK_EXT) /* Locking done in library if set */
#endif
#ifdef DBA_DBM
	DBA_HND(dbm, DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_NDBM
	DBA_HND(ndbm, DBA_LOCK_ALL) /* Could be done in library: filemode = 0644 + S_ENFMT */
#endif
#ifdef DBA_CDB
	DBA_HND(cdb, DBA_STREAM_OPEN|DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_CDB_BUILTIN
	DBA_NAMED_HND(cdb_make, cdb, DBA_STREAM_OPEN|DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_DB1
	DBA_HND(db1, DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_DB2
	DBA_HND(db2, DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_DB3
	DBA_HND(db3, DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_DB4
	DBA_HND(db4, DBA_LOCK_ALL) /* No lock in lib */
#endif
#ifdef DBA_INIFILE
	DBA_HND(inifile, DBA_STREAM_OPEN|DBA_LOCK_ALL|DBA_CAST_AS_FD) /* No lock in lib */
#endif
#ifdef DBA_FLATFILE
	DBA_HND(flatfile, DBA_STREAM_OPEN|DBA_LOCK_ALL|DBA_NO_APPEND) /* No lock in lib */
#endif
#ifdef DBA_QDBM
	DBA_HND(qdbm, DBA_LOCK_EXT)
#endif
#ifdef DBA_TCADB
	DBA_HND(tcadb, DBA_LOCK_ALL)
#endif
#ifdef DBA_LMDB
	DBA_HND(lmdb, DBA_LOCK_EXT)
#endif
	{ NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

#ifdef DBA_FLATFILE
#define DBA_DEFAULT "flatfile"
#elif defined(DBA_DB4)
#define DBA_DEFAULT "db4"
#elif defined(DBA_DB3)
#define DBA_DEFAULT "db3"
#elif defined(DBA_DB2)
#define DBA_DEFAULT "db2"
#elif defined(DBA_DB1)
#define DBA_DEFAULT "db1"
#elif defined(DBA_GDBM)
#define DBA_DEFAULT "gdbm"
#elif defined(DBA_NBBM)
#define DBA_DEFAULT "ndbm"
#elif defined(DBA_DBM)
#define DBA_DEFAULT "dbm"
#elif defined(DBA_QDBM)
#define DBA_DEFAULT "qdbm"
#elif defined(DBA_TCADB)
#define DBA_DEFAULT "tcadb"
#elif defined(DBA_LMDB)
#define DBA_DEFAULT "lmdb"
#else
#define DBA_DEFAULT ""
#endif
/* cdb/cdb_make and ini are no option here */

static int le_pdb;

static zend_class_entry *dba_connection_ce;
static zend_object_handlers dba_connection_object_handlers;

static void dba_close_info(dba_info *info)
{
	ZEND_ASSERT(info != NULL && "connection has already been closed");

	if (info->hnd) {
		info->hnd->close(info);
		info->hnd = NULL;
	}
	ZEND_ASSERT(info->path);
	zend_string_release_ex(info->path, info->flags&DBA_PERSISTENT);
	info->path = NULL;

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

	pefree(info, info->flags & DBA_PERSISTENT);
	info = NULL;
}

static int remove_pconnection_from_list(zval *zv, void *p)
{
	zend_resource *le = Z_RES_P(zv);

	if (le->ptr == p) {
		return ZEND_HASH_APPLY_REMOVE;
	}

	return ZEND_HASH_APPLY_KEEP;
}

static void close_pconnection(zend_resource *rsrc)
{
	dba_info *info = (dba_info *) rsrc->ptr;

	dba_close_info(info);

	rsrc->ptr = NULL;
}

static void dba_close_connection(dba_connection *connection)
{
	bool persistent = connection->info->flags & DBA_PERSISTENT;

	if (!persistent) {
		dba_close_info(connection->info);
	}

	connection->info = NULL;

	if (connection->hash) {
		zend_hash_del(&DBA_G(connections), connection->hash);
		zend_string_release_ex(connection->hash, persistent);
		connection->hash = NULL;
	}
}

static zend_object *dba_connection_create_object(zend_class_entry *class_type)
{
	dba_connection *intern = zend_object_alloc(sizeof(dba_connection), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *dba_connection_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "Cannot directly construct Dba\\Connection, use dba_open() or dba_popen() instead");
	return NULL;
}

static zend_result dba_connection_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_LONG) {
		/* For better backward compatibility, make (int) $dba return the object ID,
		 * similar to how it previously returned the resource ID. */
		ZVAL_LONG(result, obj->handle);

		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, result, type);
}

static inline dba_connection *dba_connection_from_obj(zend_object *obj)
{
	return (dba_connection *)((char *)(obj) - XtOffsetOf(dba_connection, std));
}

#define Z_DBA_CONNECTION_P(zv) dba_connection_from_obj(Z_OBJ_P(zv))
#define Z_DBA_INFO_P(zv) Z_DBA_CONNECTION_P(zv)->info

static void dba_connection_free_obj(zend_object *obj)
{
	dba_connection *connection = dba_connection_from_obj(obj);

	if (connection->info) {
		dba_close_connection(connection);
	}

	zend_object_std_dtor(&connection->std);
}

/* {{{ PHP_INI */
static ZEND_INI_MH(OnUpdateDefaultHandler)
{
	const dba_handler *hptr;

	if (!ZSTR_LEN(new_value)) {
		DBA_G(default_hptr) = NULL;
		return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}

	for (hptr = handler; hptr->name && strcasecmp(hptr->name, ZSTR_VAL(new_value)); hptr++);

	if (!hptr->name) {
		php_error_docref(NULL, E_WARNING, "No such handler: %s", ZSTR_VAL(new_value));
		return FAILURE;
	}
	DBA_G(default_hptr) = hptr;
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("dba.default_handler", DBA_DEFAULT, PHP_INI_ALL, OnUpdateDefaultHandler, default_handler,    zend_dba_globals, dba_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(dba)
{
#if defined(COMPILE_DL_DBA) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	dba_globals->default_handler = "";
	dba_globals->default_hptr    = NULL;
	zend_hash_init(&dba_globals->connections, 0, NULL, NULL, true);
	GC_MAKE_PERSISTENT_LOCAL(&dba_globals->connections);
}
/* }}} */

static PHP_GSHUTDOWN_FUNCTION(dba)
{
	zend_hash_destroy(&dba_globals->connections);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(dba)
{
	REGISTER_INI_ENTRIES();
	le_pdb = zend_register_list_destructors_ex(NULL, close_pconnection, "dba persistent", module_number);
	register_dba_symbols(module_number);

	dba_connection_ce = register_class_Dba_Connection();
	dba_connection_ce->create_object = dba_connection_create_object;
	dba_connection_ce->default_object_handlers = &dba_connection_object_handlers;

	memcpy(&dba_connection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	dba_connection_object_handlers.offset = XtOffsetOf(dba_connection, std);
	dba_connection_object_handlers.free_obj = dba_connection_free_obj;
	dba_connection_object_handlers.get_constructor = dba_connection_get_constructor;
	dba_connection_object_handlers.clone_obj = NULL;
	dba_connection_object_handlers.cast_object = dba_connection_cast_object;
	dba_connection_object_handlers.compare = zend_objects_not_comparable;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(dba)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

#include "zend_smart_str.h"

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(dba)
{
	const dba_handler *hptr;
	smart_str handlers = {0};

	for(hptr = handler; hptr->name; hptr++) {
		smart_str_appends(&handlers, hptr->name);
		smart_str_appendc(&handlers, ' ');
	}

	php_info_print_table_start();
	php_info_print_table_row(2, "DBA support", "enabled");
	if (handlers.s) {
		smart_str_0(&handlers);
		php_info_print_table_row(2, "Supported handlers", ZSTR_VAL(handlers.s));
		smart_str_free(&handlers);
	} else {
		php_info_print_table_row(2, "Supported handlers", "none");
	}
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_dba_update */
static void php_dba_update(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *id;
	dba_info *info = NULL;
	HashTable *key_ht = NULL;
	zend_string *key_str = NULL;
	zend_string *value;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ARRAY_HT_OR_STR(key_ht, key_str)
		Z_PARAM_STR(value)
		Z_PARAM_OBJECT_OF_CLASS(id, dba_connection_ce);
	ZEND_PARSE_PARAMETERS_END();

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);
	DBA_WRITE_CHECK(info);

	if (key_ht) {
		key_str = php_dba_make_key(key_ht);
		if (!key_str) {
			// TODO ValueError?
			RETURN_FALSE;
		}
	}

	RETVAL_BOOL(info->hnd->update(info, key_str, value, mode) == SUCCESS);
	DBA_RELEASE_HT_KEY_CREATION();
}
/* }}} */

/* {{{ php_find_dbm */
static dba_info *php_dba_find(const zend_string *path)
{
	zval *zv;

	ZEND_HASH_MAP_FOREACH_VAL(&DBA_G(connections), zv) {
		dba_info *info = Z_DBA_INFO_P(zv);
		if (info && zend_string_equals(path, info->path)) {
			return info;
		}
	} ZEND_HASH_FOREACH_END();

	return NULL;
}
/* }}} */

static zend_always_inline zend_string *php_dba_zend_string_dup_safe(zend_string *s, bool persistent)
{
	if (ZSTR_IS_INTERNED(s) && !persistent) {
		return s;
	} else {
		zend_string *duplicated_str = zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), persistent);
		if (persistent) {
			GC_MAKE_PERSISTENT_LOCAL(duplicated_str);
		}
		return duplicated_str;
	}
}

/* {{{ php_dba_open */
static void php_dba_open(INTERNAL_FUNCTION_PARAMETERS, bool persistent)
{
	dba_mode_t modenr;
	const dba_handler *hptr;
	const char *error = NULL;
	int lock_mode, lock_flag = 0;
	const char *file_mode;
	const char *lock_file_mode = NULL;
	int persistent_flag = persistent ? STREAM_OPEN_PERSISTENT : 0;
	char *lock_name;
#ifdef PHP_WIN32
	bool restarted = 0;
	bool need_creation = 0;
#endif

	zend_string *path;
	zend_string *mode;
	zend_string *handler_str = NULL;
	zend_long permission = 0644;
	zend_long map_size = 0;
	zend_long driver_flags = DBA_DEFAULT_DRIVER_FLAGS;
	bool is_flags_null = true;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "PS|S!lll!", &path, &mode, &handler_str,
			&permission, &map_size, &driver_flags, &is_flags_null)) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(path) == 0) {
		zend_argument_must_not_be_empty_error(1);
		RETURN_THROWS();
	}
	if (ZSTR_LEN(mode) == 0) {
		zend_argument_must_not_be_empty_error(2);
		RETURN_THROWS();
	}
	if (handler_str && ZSTR_LEN(handler_str) == 0) {
		zend_argument_must_not_be_empty_error(3);
		RETURN_THROWS();
	}
	// TODO Check Value for permission
	if (map_size < 0) {
		zend_argument_value_error(5, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (!is_flags_null && driver_flags < 0) {
		zend_argument_value_error(6, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	char *resource_key;
	size_t resource_key_len = spprintf(&resource_key, 0,
		"dba_%d_%s_%s_%s", persistent, ZSTR_VAL(path), ZSTR_VAL(mode), handler_str ? ZSTR_VAL(handler_str) : ""
	);

	if (persistent) {
		zend_resource *le;

		/* try to find if we already have this link in our persistent list */
		if ((le = zend_hash_str_find_ptr(&EG(persistent_list), resource_key, resource_key_len)) != NULL) {
			if (le->type != le_pdb) {
				// TODO This should never happen
				efree(resource_key);
				RETURN_FALSE;
			}

			object_init_ex(return_value, dba_connection_ce);
			dba_connection *connection = Z_DBA_CONNECTION_P(return_value);
			connection->info = (dba_info *)le->ptr;
			connection->hash = zend_string_init(resource_key, resource_key_len, persistent);
			if (persistent) {
				GC_MAKE_PERSISTENT_LOCAL(connection->hash);
			}

			if (zend_hash_exists(&DBA_G(connections), connection->hash)) {
				zend_hash_del(&DBA_G(connections), connection->hash);
			}

			zend_hash_add_new(&DBA_G(connections), connection->hash, return_value);
			efree(resource_key);
			return;
		}
	}

	if (!handler_str) {
		hptr = DBA_G(default_hptr);
		if (!hptr) {
			php_error_docref(NULL, E_WARNING, "No default handler selected");
			efree(resource_key);
			RETURN_FALSE;
		}
		ZEND_ASSERT(hptr->name);
	} else {
		/* Loop through global static var handlers to see if such a handler exists */
		for (hptr = handler; hptr->name && strcasecmp(hptr->name, ZSTR_VAL(handler_str)); hptr++);

		if (!hptr->name) {
			php_error_docref(NULL, E_WARNING, "Handler \"%s\" is not available", ZSTR_VAL(handler_str));
			efree(resource_key);
			RETURN_FALSE;
		}
	}

	/* Check mode: [rwnc][dl-]?t?
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
	bool is_test_lock = false;
	bool is_db_lock = false;
	bool is_lock_ignored = false;
	// bool is_file_lock = false;

	if (ZSTR_LEN(mode) > 3) {
		zend_argument_value_error(2, "must be at most 3 characters");
		efree(resource_key);
		RETURN_THROWS();
	}
	if (ZSTR_LEN(mode) == 3) {
		if (ZSTR_VAL(mode)[2] != 't') {
			zend_argument_value_error(2, "third character must be \"t\"");
			efree(resource_key);
			RETURN_THROWS();
		}
		is_test_lock = true;
	}
	if (ZSTR_LEN(mode) >= 2) {
		switch (ZSTR_VAL(mode)[1]) {
			case 't':
				is_test_lock = true;
				break;
			case '-':
				if ((hptr->flags & DBA_LOCK_ALL) == 0) {
					php_error_docref(NULL, E_WARNING, "Locking cannot be disabled for handler %s", hptr->name);
					efree(resource_key);
					RETURN_FALSE;
				}
				is_lock_ignored = true;
				lock_flag = 0;
			break;
			case 'd':
				is_db_lock = true;
				if ((hptr->flags & DBA_LOCK_ALL) == 0) {
					lock_flag = (hptr->flags & DBA_LOCK_ALL);
					break;
				}
				ZEND_FALLTHROUGH;
			case 'l':
				// is_file_lock = true;
				lock_flag = DBA_LOCK_ALL;
				if ((hptr->flags & DBA_LOCK_ALL) == 0) {
					php_error_docref(NULL, E_NOTICE, "Handler %s does locking internally", hptr->name);
				}
				break;
			default:
				zend_argument_value_error(2, "second character must be one of \"d\", \"l\", \"-\", or \"t\"");
				efree(resource_key);
				RETURN_THROWS();
		}
	} else {
		lock_flag = (hptr->flags&DBA_LOCK_ALL);
		is_db_lock = true;
	}

	switch (ZSTR_VAL(mode)[0]) {
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
		case 'c': {
#ifdef PHP_WIN32
			if (hptr->flags & (DBA_NO_APPEND|DBA_CAST_AS_FD)) {
				php_stream_statbuf ssb;
				need_creation = (SUCCESS != php_stream_stat_path(ZSTR_VAL(path), &ssb));
			}
#endif
			modenr = DBA_CREAT;
			lock_mode = (lock_flag & DBA_LOCK_CREAT) ? LOCK_EX : 0;
			if (lock_mode) {
				if (is_db_lock) {
					/* the create/append check will be done on the lock
					 * when the lib opens the file it is already created
					 */
					file_mode = "r+b";       /* read & write, seek 0 */
#ifdef PHP_WIN32
					if (!need_creation) {
						lock_file_mode = "r+b";
					} else
#endif
					lock_file_mode = "a+b";  /* append */
				} else {
#ifdef PHP_WIN32
					if (!need_creation) {
						file_mode = "r+b";
					} else
#endif
					file_mode = "a+b";       /* append */
					lock_file_mode = "w+b";  /* create/truncate */
				}
			} else {
#ifdef PHP_WIN32
				if (!need_creation) {
					file_mode = "r+b";
				} else
#endif
				file_mode = "a+b";
			}
			/* In case of the 'a+b' append mode, the handler is responsible
			 * to handle any rewind problems (see flatfile handler).
			 */
			break;
		}
		case 'n':
			modenr = DBA_TRUNC;
			lock_mode = (lock_flag & DBA_LOCK_TRUNC) ? LOCK_EX : 0;
			file_mode = "w+b";
			break;
		default:
			zend_argument_value_error(2, "first character must be one of \"r\", \"w\", \"c\", or \"n\"");
			efree(resource_key);
			RETURN_THROWS();
	}
	if (!lock_file_mode) {
		lock_file_mode = file_mode;
	}
	if (is_test_lock) {
		if (is_lock_ignored) {
			zend_argument_value_error(2, "cannot combine mode \"-\" (no lock) and \"t\" (test lock)");
			efree(resource_key);
			RETURN_THROWS();
		}
		if (!lock_mode) {
			if ((hptr->flags & DBA_LOCK_ALL) == 0) {
				php_error_docref(NULL, E_WARNING, "Handler %s uses its own locking which doesn't support mode modifier t (test lock)", hptr->name);
				efree(resource_key);
				RETURN_FALSE;
			} else {
				php_error_docref(NULL, E_WARNING, "Handler %s doesn't uses locking for this mode which makes modifier t (test lock) obsolete", hptr->name);
				efree(resource_key);
				RETURN_FALSE;
			}
		} else {
			lock_mode |= LOCK_NB; /* test =: non blocking */
		}
	}

	zval *connection_zval;
	dba_connection *connection;
	if ((connection_zval = zend_hash_str_find(&DBA_G(connections), resource_key, resource_key_len)) == NULL) {
		object_init_ex(return_value, dba_connection_ce);
		connection = Z_DBA_CONNECTION_P(return_value);

		connection->info = pecalloc(1, sizeof(dba_info), persistent);
		connection->info->path = php_dba_zend_string_dup_safe(path, persistent);
		connection->info->mode = modenr;
		connection->info->file_permission = permission;
		connection->info->map_size = map_size;
		connection->info->driver_flags = driver_flags;
		connection->info->flags = (hptr->flags & ~DBA_LOCK_ALL) | (lock_flag & DBA_LOCK_ALL) | (persistent ? DBA_PERSISTENT : 0);
		connection->info->lock.mode = lock_mode;
		connection->hash = zend_string_init(resource_key, resource_key_len, persistent);
		if (persistent) {
			GC_MAKE_PERSISTENT_LOCAL(connection->hash);
		}
	} else {
		ZVAL_COPY(return_value, connection_zval);
		connection = Z_DBA_CONNECTION_P(return_value);
	}

	/* if any open call is a locking call:
	 * check if we already have a locking call open that should block this call
	 * the problem is some systems would allow read during write
	 */
	if (hptr->flags & DBA_LOCK_ALL) {
		dba_info *other;
		if ((other = php_dba_find(connection->info->path)) != NULL) {
			if (   ( (lock_mode&LOCK_EX)        && (other->lock.mode&(LOCK_EX|LOCK_SH)) )
			    || ( (other->lock.mode&LOCK_EX) && (lock_mode&(LOCK_EX|LOCK_SH))        )
			   ) {
				error = "Unable to establish lock (database file already open)"; /* force failure exit */
			}
		}
	}

#ifdef PHP_WIN32
restart:
#endif
	if (!error && lock_mode) {
		if (is_db_lock) {
			lock_name = ZSTR_VAL(path);
		} else {
			spprintf(&lock_name, 0, "%s.lck", ZSTR_VAL(connection->info->path));
			if (!strcmp(file_mode, "r")) {
				zend_string *opened_path = NULL;
				/* when in read only mode try to use existing .lck file first */
				/* do not log errors for .lck file while in read only mode on .lck file */
				lock_file_mode = "rb";
				connection->info->lock.fp = php_stream_open_wrapper(lock_name, lock_file_mode, STREAM_MUST_SEEK|IGNORE_PATH|persistent_flag, &opened_path);
				if (opened_path) {
					zend_string_release_ex(opened_path, 0);
				}
			}
			if (!connection->info->lock.fp) {
				/* when not in read mode or failed to open .lck file read only. now try again in create(write) mode and log errors */
				lock_file_mode = "a+b";
			}
		}
		if (!connection->info->lock.fp) {
			zend_string *opened_path = NULL;
			connection->info->lock.fp = php_stream_open_wrapper(lock_name, lock_file_mode, STREAM_MUST_SEEK|REPORT_ERRORS|IGNORE_PATH|persistent_flag, &opened_path);
			if (connection->info->lock.fp) {
				if (is_db_lock) {
					if (opened_path) {
						/* replace the path info with the real path of the opened file */
						zend_string_release_ex(connection->info->path, persistent);
						connection->info->path = php_dba_zend_string_dup_safe(opened_path, persistent);
					} else {
						error = "Unable to determine path for locking";
					}
				}
			}
			if (opened_path) {
				zend_string_release_ex(opened_path, 0);
				opened_path = NULL;
			}
		}
		if (!is_db_lock) {
			efree(lock_name);
		}
		if (!connection->info->lock.fp) {
			/* stream operation already wrote an error message */
			efree(resource_key);
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}
		if (!error && !php_stream_supports_lock(connection->info->lock.fp)) {
			error = "Stream does not support locking";
		}
		if (!error && php_stream_lock(connection->info->lock.fp, lock_mode)) {
			error = "Unable to establish lock"; /* force failure exit */
		}
	}

	/* centralised open stream for builtin */
	if (!error && (hptr->flags&DBA_STREAM_OPEN)==DBA_STREAM_OPEN) {
		if (connection->info->lock.fp && is_db_lock) {
			connection->info->fp = connection->info->lock.fp; /* use the same stream for locking and database access */
		} else {
			connection->info->fp = php_stream_open_wrapper(ZSTR_VAL(connection->info->path), file_mode, STREAM_MUST_SEEK|REPORT_ERRORS|IGNORE_PATH|persistent_flag, NULL);
		}
		if (!connection->info->fp) {
			/* stream operation already wrote an error message */
			efree(resource_key);
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}
		if (hptr->flags & (DBA_NO_APPEND|DBA_CAST_AS_FD)) {
			/* Needed because some systems do not allow to write to the original
			 * file contents with O_APPEND being set.
			 */
			if (SUCCESS != php_stream_cast(connection->info->fp, PHP_STREAM_AS_FD, (void*)&connection->info->fd, 1)) {
				php_error_docref(NULL, E_WARNING, "Could not cast stream");
				efree(resource_key);
				zval_ptr_dtor(return_value);
				RETURN_FALSE;
#ifdef F_SETFL
			} else if (modenr == DBA_CREAT) {
				int flags = fcntl(connection->info->fd, F_GETFL);
				fcntl(connection->info->fd, F_SETFL, flags & ~O_APPEND);
#elif defined(PHP_WIN32)
			} else if (modenr == DBA_CREAT && need_creation && !restarted) {
				if (connection->info->lock.fp != NULL) {
					php_stream_free(connection->info->lock.fp, persistent ? PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE);
				}
				if (connection->info->fp != connection->info->lock.fp) {
					php_stream_free(connection->info->fp, persistent ? PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE);
				}
				connection->info->fp = NULL;
				connection->info->lock.fp = NULL;
				connection->info->fd = -1;

				lock_file_mode = "r+b";

				restarted = 1;
				goto restart;
#endif
			}
		}
	}

	if (error || hptr->open(connection->info, &error) == FAILURE) {
		if (EXPECTED(!EG(exception))) {
			if (error) {
				php_error_docref(NULL, E_WARNING, "Driver initialization failed for handler: %s: %s", hptr->name, error);
			} else {
				php_error_docref(NULL, E_WARNING, "Driver initialization failed for handler: %s", hptr->name);
			}
		}
		efree(resource_key);
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	connection->info->hnd = hptr;

	if (persistent) {
		if (zend_register_persistent_resource(resource_key, resource_key_len, connection->info, le_pdb) == NULL) {
			php_error_docref(NULL, E_WARNING, "Could not register persistent resource");
			efree(resource_key);
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}
	}

	zend_hash_add_new(&DBA_G(connections), connection->hash, return_value);
	efree(resource_key);
}
/* }}} */

/* {{{ Opens path using the specified handler in mode persistently */
PHP_FUNCTION(dba_popen)
{
	php_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Opens path using the specified handler in mode*/
PHP_FUNCTION(dba_open)
{
	php_dba_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Closes database */
PHP_FUNCTION(dba_close)
{
	zval *id;
	dba_connection *connection = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &id, dba_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	connection = Z_DBA_CONNECTION_P(id);
	CHECK_DBA_CONNECTION(connection->info);

	bool persistent = connection->info->flags & DBA_PERSISTENT;

	dba_close_connection(connection);

	if (persistent) {
		zend_hash_apply_with_argument(&EG(persistent_list), remove_pconnection_from_list, (void *) connection->info);
	}
}
/* }}} */

/* {{{ Checks, if the specified key exists */
PHP_FUNCTION(dba_exists)
{
	zval *id;
	dba_info *info = NULL;
	HashTable *key_ht = NULL;
	zend_string *key_str = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_HT_OR_STR(key_ht, key_str)
		Z_PARAM_OBJECT_OF_CLASS(id, dba_connection_ce);
	ZEND_PARSE_PARAMETERS_END();

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);

	if (key_ht) {
		key_str = php_dba_make_key(key_ht);
		if (!key_str) {
			// TODO ValueError?
			RETURN_FALSE;
		}
	}

	RETVAL_BOOL(info->hnd->exists(info, key_str) == SUCCESS);
	DBA_RELEASE_HT_KEY_CREATION();
}
/* }}} */

/* {{{ Fetches the data associated with key */
PHP_FUNCTION(dba_fetch)
{
	zval *id;
	dba_info *info = NULL;
	HashTable *key_ht = NULL;
	zend_string *key_str = NULL;
	zend_long skip = 0;

	/* Check for legacy signature */
	if (ZEND_NUM_ARGS() == 3) {
		ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_QUIET, 3, 3)
			Z_PARAM_ARRAY_HT_OR_STR(key_ht, key_str)
			Z_PARAM_LONG(skip)
			Z_PARAM_OBJECT_OF_CLASS(id, dba_connection_ce);
		ZEND_PARSE_PARAMETERS_END_EX(goto standard;);

		zend_error(E_DEPRECATED, "Calling dba_fetch() with $dba at the 3rd parameter is deprecated");
		if (UNEXPECTED(EG(exception))) {
			RETURN_THROWS();
		}
	} else {
		standard:
		ZEND_PARSE_PARAMETERS_START(2, 3)
			Z_PARAM_ARRAY_HT_OR_STR(key_ht, key_str)
			Z_PARAM_OBJECT_OF_CLASS(id, dba_connection_ce);
			Z_PARAM_OPTIONAL
			Z_PARAM_LONG(skip)
		ZEND_PARSE_PARAMETERS_END();
	}

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);

	if (key_ht) {
		key_str = php_dba_make_key(key_ht);
		if (!key_str) {
			// TODO ValueError?
			RETURN_FALSE;
		}
	}

	if (skip != 0) {
		if (!strcmp(info->hnd->name, "cdb")) {
			// TODO ValueError?
			if (skip < 0) {
				php_error_docref(NULL, E_NOTICE, "Handler %s accepts only skip values greater than or equal to zero, using skip=0", info->hnd->name);
				skip = 0;
			}
		} else if (!strcmp(info->hnd->name, "inifile")) {
			/* "-1" is comparable to 0 but allows a non restrictive
			 * access which is faster. For example 'inifile' uses this
			 * to allow faster access when the key was already found
			 * using firstkey/nextkey. However explicitly setting the
			 * value to 0 ensures the first value.
			 */
			if (skip < -1) {
				// TODO ValueError?
				php_error_docref(NULL, E_NOTICE, "Handler %s accepts only skip value -1 and greater, using skip=0", info->hnd->name);
				skip = 0;
			}
		} else {
			php_error_docref(NULL, E_NOTICE, "Handler %s does not support optional skip parameter, the value will be ignored", info->hnd->name);
			skip = 0;
		}
	}

	zend_string *val;
	if ((val = info->hnd->fetch(info, key_str, skip)) == NULL) {
		DBA_RELEASE_HT_KEY_CREATION();
		RETURN_FALSE;
	}
	DBA_RELEASE_HT_KEY_CREATION();
	RETURN_STR(val);
}
/* }}} */

/* {{{ Splits an inifile key into an array of the form array(0=>group,1=>value_name) but returns false if input is false or null */
PHP_FUNCTION(dba_key_split)
{
	zval *zkey;
	char *key, *name;
	size_t key_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &zkey) == SUCCESS) {
		if (Z_TYPE_P(zkey) == IS_NULL || (Z_TYPE_P(zkey) == IS_FALSE)) {
			php_error_docref(NULL, E_DEPRECATED, "Passing false or null is deprecated since 8.4");
			RETURN_FALSE;
		}
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &key_len) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Resets the internal key pointer and returns the first key */
PHP_FUNCTION(dba_firstkey)
{
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &id, dba_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);

	zend_string *fkey = info->hnd->firstkey(info);

	if (fkey) {
		RETURN_STR(fkey);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the next key */
PHP_FUNCTION(dba_nextkey)
{
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &id, dba_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);

	zend_string *nkey = info->hnd->nextkey(info);

	if (nkey) {
		RETURN_STR(nkey);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Deletes the entry associated with key
   If inifile: remove all other key lines */
PHP_FUNCTION(dba_delete)
{
	zval *id;
	dba_info *info = NULL;
	HashTable *key_ht = NULL;
	zend_string *key_str = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_HT_OR_STR(key_ht, key_str)
		Z_PARAM_OBJECT_OF_CLASS(id, dba_connection_ce);
	ZEND_PARSE_PARAMETERS_END();

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);
	DBA_WRITE_CHECK(info);

	if (key_ht) {
		key_str = php_dba_make_key(key_ht);
		if (!key_str) {
			// TODO ValueError?
			RETURN_FALSE;
		}
	}

	RETVAL_BOOL(info->hnd->delete(info, key_str) == SUCCESS);
	DBA_RELEASE_HT_KEY_CREATION();
}
/* }}} */

/* {{{ If not inifile: Insert value as key, return false, if key exists already
   If inifile: Add vakue as key (next instance of key) */
PHP_FUNCTION(dba_insert)
{
	php_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Inserts value as key, replaces key, if key exists already
   If inifile: remove all other key lines */
PHP_FUNCTION(dba_replace)
{
	php_dba_update(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Optimizes (e.g. clean up, vacuum) database */
PHP_FUNCTION(dba_optimize)
{
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &id, dba_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);
	DBA_WRITE_CHECK(info);

	if (info->hnd->optimize(info) == SUCCESS) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Synchronizes database */
PHP_FUNCTION(dba_sync)
{
	zval *id;
	dba_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &id, dba_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	info = Z_DBA_INFO_P(id);
	CHECK_DBA_CONNECTION(info);

	if (info->hnd->sync(info) == SUCCESS) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ List configured database handlers */
PHP_FUNCTION(dba_handlers)
{
	const dba_handler *hptr;
	bool full_info = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &full_info) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ List opened databases */
PHP_FUNCTION(dba_list)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	zval *zv;
	ZEND_HASH_MAP_FOREACH_VAL(&DBA_G(connections), zv) {
		dba_connection *connection = Z_DBA_CONNECTION_P(zv);
		if (connection->info) {
			add_index_str(return_value, connection->std.handle, zend_string_copy(connection->info->path));
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

#endif /* HAVE_DBA */
