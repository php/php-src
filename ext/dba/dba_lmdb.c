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
  | Author: Anatol Belski <ab@php.net>                                   |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef DBA_LMDB
#include "php_lmdb.h"

#ifdef LMDB_INCLUDE_FILE
#include LMDB_INCLUDE_FILE
#endif

struct php_lmdb_info {
	MDB_env *env;
	MDB_txn *txn;
	MDB_dbi dbi;
	MDB_cursor *cur;
};

#define LMDB_IT(it) (((struct php_lmdb_info *)info->dbf)->it)

DBA_OPEN_FUNC(lmdb)
{
	MDB_env *env;
	MDB_txn *txn;
	int rc;
	int mode = info->file_permission;
	zend_long map_size = info->map_size;

	ZEND_ASSERT(map_size >= 0);

	/* By default use the MDB_NOSUBDIR flag */
	int flags = MDB_NOSUBDIR;
	/* Use flags passed by the user for driver flags */
	if (info->driver_flags != DBA_DEFAULT_DRIVER_FLAGS) {
		ZEND_ASSERT(info->driver_flags >= 0);
		switch (info->driver_flags) {
			case 0:
			case MDB_NOSUBDIR:
				flags = info->driver_flags;
				break;
			default:
				zend_argument_value_error(6, "must be either DBA_LMDB_USE_SUB_DIR or DBA_LMDB_NO_SUB_DIR for LMDB driver");
				return FAILURE;
		}
	}

	/* Add readonly flag if DB is opened in read only mode */
	if (info->mode == DBA_READER) {
		flags |= MDB_RDONLY;
	}

	rc = mdb_env_create(&env);
	if (rc) {
		*error = mdb_strerror(rc);
		return FAILURE;
	}

	if (map_size > 0) {
		rc = mdb_env_set_mapsize(env, (size_t) map_size);
		if (rc) {
			*error = mdb_strerror(rc);
			return FAILURE;
		}
	}

	rc = mdb_env_open(env, ZSTR_VAL(info->path), flags, mode);
	if (rc) {
		/* If this function [mdb_env_open()] fails, mdb_env_close() must be called to discard the MDB_env handle.
		 * http://www.lmdb.tech/doc/group__mdb.html#ga32a193c6bf4d7d5c5d579e71f22e9340 */
		mdb_env_close(env);
		*error = mdb_strerror(rc);
		return FAILURE;
	}

	rc = mdb_txn_begin(env, NULL, /* flags */ MDB_RDONLY, &txn);
	if (rc) {
		mdb_env_close(env);
		*error = mdb_strerror(rc);
		return FAILURE;
	}

	info->dbf = pemalloc(sizeof(struct php_lmdb_info), info->flags & DBA_PERSISTENT);
	if (!info->dbf) {
		*error = "Failed to allocate php_lmdb_info.";
		return FAILURE;
	}
	memset(info->dbf, 0, sizeof(struct php_lmdb_info));

	rc = mdb_dbi_open(txn, NULL, 0, &LMDB_IT(dbi));
	if (rc) {
		mdb_env_close(env);
		pefree(info->dbf, info->flags & DBA_PERSISTENT);
		*error = mdb_strerror(rc);
		return FAILURE;
	}

	LMDB_IT(env) = env;
	LMDB_IT(txn) = txn;

	mdb_txn_abort(LMDB_IT(txn));

	return SUCCESS;
}

DBA_CLOSE_FUNC(lmdb)
{
	mdb_dbi_close(LMDB_IT(env), LMDB_IT(dbi));
	mdb_env_close(LMDB_IT(env));

	pefree(info->dbf, info->flags & DBA_PERSISTENT);
}

DBA_FETCH_FUNC(lmdb)
{
	int rc;
	MDB_val k, v;
	zend_string *ret = NULL;

	if (LMDB_IT(cur)) {
		rc = mdb_txn_renew(LMDB_IT(txn));
	} else {
		rc = mdb_txn_begin(LMDB_IT(env), NULL, MDB_RDONLY, &LMDB_IT(txn));
	}
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return NULL;
	}

	k.mv_size = ZSTR_LEN(key);
	k.mv_data = ZSTR_VAL(key);

	rc = mdb_get(LMDB_IT(txn), LMDB_IT(dbi), &k, &v);
	if (rc) {
		if (MDB_NOTFOUND != rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		}
		mdb_txn_abort(LMDB_IT(txn));
		return NULL;
	}

	if (v.mv_data) {
		ret = zend_string_init(v.mv_data, v.mv_size, /* persistent */ false);
	}

	if (LMDB_IT(cur)) {
		mdb_txn_reset(LMDB_IT(txn));
	} else {
		mdb_txn_abort(LMDB_IT(txn));
	}

	return ret;
}

DBA_UPDATE_FUNC(lmdb)
{
	int rc;
	MDB_val k, v;

	rc = mdb_txn_begin(LMDB_IT(env), NULL, 0, &LMDB_IT(txn));
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return FAILURE;
	}

	k.mv_size = ZSTR_LEN(key);
	k.mv_data = ZSTR_VAL(key);
	v.mv_size = ZSTR_LEN(val);
	v.mv_data = ZSTR_VAL(val);

	rc = mdb_put(LMDB_IT(txn), LMDB_IT(dbi), &k, &v, mode == 1 ? MDB_NOOVERWRITE : 0);
	if (rc) {
		if (MDB_KEYEXIST != rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		}
		mdb_txn_abort(LMDB_IT(txn));
		return FAILURE;
	}

	rc = mdb_txn_commit(LMDB_IT(txn));
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		mdb_txn_abort(LMDB_IT(txn));
		return FAILURE;
	}

	return SUCCESS;
}

DBA_EXISTS_FUNC(lmdb)
{
	int rc;
	MDB_val k, v;

	if (LMDB_IT(cur)) {
		rc = mdb_txn_renew(LMDB_IT(txn));
	} else {
		rc = mdb_txn_begin(LMDB_IT(env), NULL, MDB_RDONLY, &LMDB_IT(txn));
	}
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return FAILURE;
	}

	k.mv_size = ZSTR_LEN(key);
	k.mv_data = ZSTR_VAL(key);

	rc = mdb_get(LMDB_IT(txn), LMDB_IT(dbi), &k, &v);
	if (rc) {
		if (MDB_NOTFOUND != rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		}
		mdb_txn_abort(LMDB_IT(txn));
		return FAILURE;
	}

	if (LMDB_IT(cur)) {
		mdb_txn_reset(LMDB_IT(txn));
	} else {
		mdb_txn_abort(LMDB_IT(txn));
	}

	return SUCCESS;
}

DBA_DELETE_FUNC(lmdb)
{
	int rc;
	MDB_val k;

	rc = mdb_txn_begin(LMDB_IT(env), NULL, 0, &LMDB_IT(txn));
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return FAILURE;
	}

	k.mv_size = ZSTR_LEN(key);
	k.mv_data = ZSTR_VAL(key);

	rc = mdb_del(LMDB_IT(txn), LMDB_IT(dbi), &k, NULL);
	if (!rc) {
		rc = mdb_txn_commit(LMDB_IT(txn));
		if (rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
			mdb_txn_abort(LMDB_IT(txn));
			return FAILURE;
		}
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));

	mdb_txn_abort(LMDB_IT(txn));
	return FAILURE;
}

DBA_FIRSTKEY_FUNC(lmdb)
{
	int rc;
	MDB_val k, v;
	zend_string *ret = NULL;

	rc = mdb_txn_begin(LMDB_IT(env), NULL, MDB_RDONLY, &LMDB_IT(txn));
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return NULL;
	}

	rc = mdb_cursor_open(LMDB_IT(txn), LMDB_IT(dbi), &LMDB_IT(cur));
	if (rc) {
		mdb_txn_abort(LMDB_IT(txn));
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return NULL;
	}

	rc = mdb_cursor_get(LMDB_IT(cur), &k, &v, MDB_FIRST);
	if (rc) {
		mdb_txn_abort(LMDB_IT(txn));
		mdb_cursor_close(LMDB_IT(cur));
		LMDB_IT(cur) = NULL;
		if (MDB_NOTFOUND != rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		}
		return NULL;
	}

	if (k.mv_data) {
		ret = zend_string_init(k.mv_data, k.mv_size, /* persistent */ false);
	}

	mdb_txn_reset(LMDB_IT(txn));

	return ret;
}

DBA_NEXTKEY_FUNC(lmdb)
{
	int rc;
	MDB_val k, v;
	zend_string *ret = NULL;

	rc = mdb_txn_renew(LMDB_IT(txn));
	if (rc) {
		php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		return NULL;
	}

	rc = mdb_cursor_get(LMDB_IT(cur), &k, &v, MDB_NEXT);
	if (rc) {
		mdb_txn_abort(LMDB_IT(txn));
		mdb_cursor_close(LMDB_IT(cur));
		LMDB_IT(cur) = NULL;
		if (MDB_NOTFOUND != rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
		}
		return NULL;
	}

	if (k.mv_data) {
		ret = zend_string_init(k.mv_data, k.mv_size, /* persistent */ false);
	}

	mdb_txn_reset(LMDB_IT(txn));

	return ret;
}

DBA_OPTIMIZE_FUNC(lmdb)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(lmdb)
{
	int rc;

	rc = mdb_env_sync(LMDB_IT(env), 1);
	if (rc) {
			php_error_docref(NULL, E_WARNING, "%s", mdb_strerror(rc));
			return FAILURE;
	}

	return SUCCESS;
}

DBA_INFO_FUNC(lmdb)
{
	return estrdup(MDB_VERSION_STRING);
}

#endif
