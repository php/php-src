/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_dblib.h"
#include "php_pdo_dblib_int.h"
#include "zend_exceptions.h"

/* Cache of the server supported datatypes, initialized in handle_factory */
zval* pdo_dblib_datatypes;

static void dblib_fetch_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	pdo_dblib_err *einfo = &H->err;
	pdo_dblib_stmt *S = NULL;
	char *message;
	char *msg;

	if (stmt) {
		S = (pdo_dblib_stmt*)stmt->driver_data;
		einfo = &S->err;
	}

	if (einfo->lastmsg) {
		msg = einfo->lastmsg;
	} else if (DBLIB_G(err).lastmsg) {
		msg = DBLIB_G(err).lastmsg;
		DBLIB_G(err).lastmsg = NULL;
	} else {
		msg = einfo->dberrstr;
	}

	/* don't return anything if there's nothing to return */
	if (msg == NULL && einfo->dberr == 0 && einfo->oserr == 0 && einfo->severity == 0) {
		return;
	}

	spprintf(&message, 0, "%s [%d] (severity %d) [%s]",
		msg, einfo->dberr, einfo->severity, stmt ? ZSTR_VAL(stmt->active_query_string) : "");

	add_next_index_long(info, einfo->dberr);
	add_next_index_string(info, message);
	efree(message);
	add_next_index_long(info, einfo->oserr);
	add_next_index_long(info, einfo->severity);
	if (einfo->oserrstr) {
		add_next_index_string(info, einfo->oserrstr);
	}
}


static void dblib_handle_closer(pdo_dbh_t *dbh)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	if (H) {
		pdo_dblib_err_dtor(&H->err);
		if (H->link) {
			dbclose(H->link);
			H->link = NULL;
		}
		if (H->login) {
			dbfreelogin(H->login);
			H->login = NULL;
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
}

static bool dblib_handle_preparer(pdo_dbh_t *dbh, zend_string *sql, pdo_stmt_t *stmt, zval *driver_options)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	pdo_dblib_stmt *S = ecalloc(1, sizeof(*S));

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &dblib_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
	S->computed_column_name_count = 0;
	S->err.sqlstate = stmt->error_code;

	return true;
}

static zend_long dblib_handle_doer(pdo_dbh_t *dbh, const zend_string *sql)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	RETCODE ret, resret;

	dbsetuserdata(H->link, (BYTE*)&H->err);

	if (FAIL == dbcmd(H->link, ZSTR_VAL(sql))) {
		return -1;
	}

	if (FAIL == dbsqlexec(H->link)) {
		return -1;
	}

	resret = dbresults(H->link);

	if (resret == FAIL) {
		return -1;
	}

	ret = dbnextrow(H->link);
	if (ret == FAIL) {
		return -1;
	}

	if (dbnumcols(H->link) <= 0) {
		return DBCOUNT(H->link);
	}

	/* throw away any rows it might have returned */
	dbcanquery(H->link);

	return DBCOUNT(H->link);
}

static zend_string* dblib_handle_quoter(pdo_dbh_t *dbh, const zend_string *unquoted, enum pdo_param_type paramtype)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	bool use_national_character_set = 0;
	size_t i;
	char *q;
	size_t quotedlen = 0;
	zend_string *quoted_str;

	if (H->assume_national_character_set_strings) {
		use_national_character_set = 1;
	}
	if ((paramtype & PDO_PARAM_STR_NATL) == PDO_PARAM_STR_NATL) {
		use_national_character_set = 1;
	}
	if ((paramtype & PDO_PARAM_STR_CHAR) == PDO_PARAM_STR_CHAR) {
		use_national_character_set = 0;
	}

	/* Detect quoted length, adding extra char for doubled single quotes */
	for (i = 0; i < ZSTR_LEN(unquoted); i++) {
		if (ZSTR_VAL(unquoted)[i] == '\'') ++quotedlen;
		++quotedlen;
	}

	quotedlen += 2; /* +2 for opening, closing quotes */
	if (use_national_character_set) {
		++quotedlen; /* N prefix */
	}
	quoted_str = zend_string_alloc(quotedlen, 0);
	q = ZSTR_VAL(quoted_str);
	if (use_national_character_set) {
		*q++ = 'N';
	}
	*q++ = '\'';

	for (i = 0; i < ZSTR_LEN(unquoted); i++) {
		if (ZSTR_VAL(unquoted)[i] == '\'') {
			*q++ = '\'';
			*q++ = '\'';
		} else {
			*q++ = ZSTR_VAL(unquoted)[i];
		}
	}
	*q++ = '\'';
	*q = '\0';

	return quoted_str;
}

static bool pdo_dblib_transaction_cmd(const char *cmd, pdo_dbh_t *dbh)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	if (FAIL == dbcmd(H->link, cmd)) {
		return false;
	}

	if (FAIL == dbsqlexec(H->link)) {
		return false;
	}

	return true;
}

static bool dblib_handle_begin(pdo_dbh_t *dbh)
{
	return pdo_dblib_transaction_cmd("BEGIN TRANSACTION", dbh);
}

static bool dblib_handle_commit(pdo_dbh_t *dbh)
{
	return pdo_dblib_transaction_cmd("COMMIT TRANSACTION", dbh);
}

static bool dblib_handle_rollback(pdo_dbh_t *dbh)
{
	return pdo_dblib_transaction_cmd("ROLLBACK TRANSACTION", dbh);
}

zend_string *dblib_handle_last_id(pdo_dbh_t *dbh, const zend_string *name)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	RETCODE ret;
	char *id = NULL;
	size_t len;
	zend_string *ret_id;

	/*
	 * Would use scope_identity() but it's not implemented on Sybase
	 */

	if (FAIL == dbcmd(H->link, "SELECT @@IDENTITY")) {
		return NULL;
	}

	if (FAIL == dbsqlexec(H->link)) {
		return NULL;
	}

	ret = dbresults(H->link);
	if (ret == FAIL || ret == NO_MORE_RESULTS) {
		dbcancel(H->link);
		return NULL;
	}

	ret = dbnextrow(H->link);

	if (ret == FAIL || ret == NO_MORE_ROWS) {
		dbcancel(H->link);
		return NULL;
	}

	if (dbdatlen(H->link, 1) == 0) {
		dbcancel(H->link);
		return NULL;
	}

	id = emalloc(32);
	len = dbconvert(NULL, (dbcoltype(H->link, 1)) , (dbdata(H->link, 1)) , (dbdatlen(H->link, 1)), SQLCHAR, (BYTE *)id, (DBINT)-1);
	dbcancel(H->link);

	ret_id = zend_string_init(id, len, 0);
	efree(id);
	return ret_id;
}

static bool dblib_set_attr(pdo_dbh_t *dbh, zend_long attr, zval *val)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	switch(attr) {
		case PDO_ATTR_DEFAULT_STR_PARAM:
			H->assume_national_character_set_strings = zval_get_long(val) == PDO_PARAM_STR_NATL ? 1 : 0;
			return true;
		case PDO_ATTR_TIMEOUT:
		case PDO_DBLIB_ATTR_QUERY_TIMEOUT:
			return SUCCEED == dbsettime(zval_get_long(val));
		case PDO_DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER:
			H->stringify_uniqueidentifier = zval_get_long(val);
			return true;
		case PDO_DBLIB_ATTR_SKIP_EMPTY_ROWSETS:
			H->skip_empty_rowsets = zval_is_true(val);
			return true;
		case PDO_DBLIB_ATTR_DATETIME_CONVERT:
			H->datetime_convert = zval_get_long(val);
			return true;
		default:
			return false;
	}
}

static void dblib_get_tds_version(zval *return_value, int tds)
{
	switch (tds) {
		case DBTDS_2_0:
			ZVAL_STRING(return_value, "2.0");
			break;

		case DBTDS_3_4:
			ZVAL_STRING(return_value, "3.4");
			break;

		case DBTDS_4_0:
			ZVAL_STRING(return_value, "4.0");
			break;

		case DBTDS_4_2:
			ZVAL_STRING(return_value, "4.2");
			break;

		case DBTDS_4_6:
			ZVAL_STRING(return_value, "4.6");
			break;

		case DBTDS_4_9_5:
			ZVAL_STRING(return_value, "4.9.5");
			break;

		case DBTDS_5_0:
			ZVAL_STRING(return_value, "5.0");
			break;

#ifdef DBTDS_7_0
		case DBTDS_7_0:
			ZVAL_STRING(return_value, "7.0");
			break;
#endif

#ifdef DBTDS_7_1
		case DBTDS_7_1:
			ZVAL_STRING(return_value, "7.1");
			break;
#endif

#ifdef DBTDS_7_2
		case DBTDS_7_2:
			ZVAL_STRING(return_value, "7.2");
			break;
#endif

#ifdef DBTDS_7_3
		case DBTDS_7_3:
			ZVAL_STRING(return_value, "7.3");
			break;
#endif

#ifdef DBTDS_7_4
		case DBTDS_7_4:
			ZVAL_STRING(return_value, "7.4");
			break;
#endif

		default:
			ZVAL_FALSE(return_value);
			break;
	}
}

static int dblib_get_attribute(pdo_dbh_t *dbh, zend_long attr, zval *return_value)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_DEFAULT_STR_PARAM:
			ZVAL_LONG(return_value, H->assume_national_character_set_strings ? PDO_PARAM_STR_NATL : PDO_PARAM_STR_CHAR);
			break;

		case PDO_ATTR_EMULATE_PREPARES:
			/* this is the only option available, but expose it so common tests and whatever else can introspect */
			ZVAL_TRUE(return_value);
			break;

		case PDO_DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER:
			ZVAL_BOOL(return_value, H->stringify_uniqueidentifier);
			break;

		case PDO_DBLIB_ATTR_VERSION:
			ZVAL_STRING(return_value, dbversion());
			break;

		case PDO_DBLIB_ATTR_TDS_VERSION:
			dblib_get_tds_version(return_value, dbtds(H->link));
			break;

		case PDO_DBLIB_ATTR_SKIP_EMPTY_ROWSETS:
			ZVAL_BOOL(return_value, H->skip_empty_rowsets);
			break;

		case PDO_DBLIB_ATTR_DATETIME_CONVERT:
			ZVAL_BOOL(return_value, H->datetime_convert);
			break;

		default:
			return 0;
	}

	return 1;
}

static const struct pdo_dbh_methods dblib_methods = {
	dblib_handle_closer,
	dblib_handle_preparer,
	dblib_handle_doer,
	dblib_handle_quoter,
	dblib_handle_begin, /* begin */
	dblib_handle_commit, /* commit */
	dblib_handle_rollback, /* rollback */
	dblib_set_attr, /*set attr */
	dblib_handle_last_id, /* last insert id */
	dblib_fetch_error, /* fetch error */
	dblib_get_attribute, /* get attr */
	NULL, /* check liveness */
	NULL, /* get driver methods */
	NULL, /* request shutdown */
	NULL, /* in transaction, use PDO's internal tracking mechanism */
	NULL /* get gc */
};

static int pdo_dblib_handle_factory(pdo_dbh_t *dbh, zval *driver_options)
{
	pdo_dblib_db_handle *H;
	int i, nvars, nvers, ret = 0;

	const pdo_dblib_keyval tdsver[] = {
		 {"4.2",DBVERSION_42}
		,{"4.6",DBVERSION_46}
		,{"5.0",DBVERSION_70} /* FIXME: This does not work with Sybase, but environ will */
		,{"6.0",DBVERSION_70}
		,{"7.0",DBVERSION_70}
#ifdef DBVERSION_71
		,{"7.1",DBVERSION_71}
#endif
#ifdef DBVERSION_72
		,{"7.2",DBVERSION_72}
		,{"8.0",DBVERSION_72}
#endif
#ifdef DBVERSION_73
		,{"7.3",DBVERSION_73}
#endif
#ifdef DBVERSION_74
		,{"7.4",DBVERSION_74}
#endif
		,{"10.0",DBVERSION_100}
		,{"auto",0} /* Only works with FreeTDS. Other drivers will bork */

	};

	struct pdo_data_src_parser vars[] = {
		{ "charset",	NULL,	0 }
		,{ "appname",	"PHP " PDO_DBLIB_FLAVOUR,	0 }
		,{ "host",		"127.0.0.1", 0 }
		,{ "dbname",	NULL,	0 }
		,{ "secure",	NULL,	0 } /* DBSETLSECURE */
		,{ "version",	NULL,	0 } /* DBSETLVERSION */
		,{ "user",      NULL,   0 }
		,{ "password",  NULL,   0 }
	};

	nvars = sizeof(vars)/sizeof(vars[0]);
	nvers = sizeof(tdsver)/sizeof(tdsver[0]);

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, nvars);

	H = pecalloc(1, sizeof(*H), dbh->is_persistent);
	H->login = dblogin();
	H->err.sqlstate = dbh->error_code;
	H->assume_national_character_set_strings = 0;
	H->stringify_uniqueidentifier = 0;
	H->skip_empty_rowsets = 0;
	H->datetime_convert = 0;

	if (!H->login) {
		goto cleanup;
	}

	if (driver_options) {
		int connect_timeout = pdo_attr_lval(driver_options, PDO_DBLIB_ATTR_CONNECTION_TIMEOUT, -1);
		int query_timeout = pdo_attr_lval(driver_options, PDO_DBLIB_ATTR_QUERY_TIMEOUT, -1);
		int timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, 30);

		if (connect_timeout == -1) {
			connect_timeout = timeout;
		}
		if (query_timeout == -1) {
			query_timeout = timeout;
		}

		dbsetlogintime(connect_timeout); /* Connection/Login Timeout */
		dbsettime(query_timeout); /* Statement Timeout */

		H->assume_national_character_set_strings = pdo_attr_lval(driver_options, PDO_ATTR_DEFAULT_STR_PARAM, 0) == PDO_PARAM_STR_NATL ? 1 : 0;
		H->stringify_uniqueidentifier = pdo_attr_lval(driver_options, PDO_DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER, 0);
		H->skip_empty_rowsets = pdo_attr_lval(driver_options, PDO_DBLIB_ATTR_SKIP_EMPTY_ROWSETS, 0);
		H->datetime_convert = pdo_attr_lval(driver_options, PDO_DBLIB_ATTR_DATETIME_CONVERT, 0);
	}

	DBERRHANDLE(H->login, (EHANDLEFUNC) pdo_dblib_error_handler);
	DBMSGHANDLE(H->login, (MHANDLEFUNC) pdo_dblib_msg_handler);

	if(vars[5].optval) {
		for(i=0;i<nvers;i++) {
			if(strcmp(vars[5].optval,tdsver[i].key) == 0) {
				if(FAIL==dbsetlversion(H->login, tdsver[i].value)) {
					pdo_raise_impl_error(dbh, NULL, "HY000", "PDO_DBLIB: Failed to set version specified in connection string.");
					goto cleanup;
				}
				break;
			}
		}

		if (i==nvers) {
			printf("Invalid version '%s'\n", vars[5].optval);
			pdo_raise_impl_error(dbh, NULL, "HY000", "PDO_DBLIB: Invalid version specified in connection string.");
			goto cleanup; /* unknown version specified */
		}
	}

	if (!dbh->username && vars[6].optval) {
		dbh->username = pestrdup(vars[6].optval, dbh->is_persistent);
	}

	if (dbh->username) {
		if(FAIL == DBSETLUSER(H->login, dbh->username)) {
			goto cleanup;
		}
	}

	if (!dbh->password && vars[7].optval) {
		dbh->password = pestrdup(vars[7].optval, dbh->is_persistent);
	}

	if (dbh->password) {
		if(FAIL == DBSETLPWD(H->login, dbh->password)) {
			goto cleanup;
		}
	}

#ifndef PHP_DBLIB_IS_MSSQL
	if (vars[0].optval) {
		DBSETLCHARSET(H->login, vars[0].optval);
	}
#endif

	DBSETLAPP(H->login, vars[1].optval);

/* DBSETLDBNAME is only available in FreeTDS 0.92 or above */
#ifdef DBSETLDBNAME
	if (vars[3].optval) {
		if(FAIL == DBSETLDBNAME(H->login, vars[3].optval)) goto cleanup;
	}
#endif

	H->link = dbopen(H->login, vars[2].optval);

	if (!H->link) {
		goto cleanup;
	}

/*
 * FreeTDS < 0.92 does not support the DBSETLDBNAME option
 * Send use database here after login (Will not work with SQL Azure)
 */
#ifndef DBSETLDBNAME
	if (vars[3].optval) {
		if(FAIL == dbuse(H->link, vars[3].optval)) goto cleanup;
	}
#endif

#ifdef PHP_DBLIB_IS_MSSQL
	/* dblib do not return more than this length from text/image */
	DBSETOPT(H->link, DBTEXTLIMIT, "2147483647");
#endif

	/* limit text/image from network */
	DBSETOPT(H->link, DBTEXTSIZE, "2147483647");

	/* allow double quoted indentifiers */
	DBSETOPT(H->link, DBQUOTEDIDENT, "1");

	ret = 1;
	dbh->max_escaped_char_length = 2;
	dbh->alloc_own_columns = 1;

cleanup:
	for (i = 0; i < nvars; i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	dbh->methods = &dblib_methods;
	dbh->driver_data = H;

	if (!ret) {
		zend_throw_exception_ex(php_pdo_get_exception(), DBLIB_G(err).dberr,
			"SQLSTATE[%s] %s (severity %d)",
			DBLIB_G(err).sqlstate,
			DBLIB_G(err).dberrstr,
			DBLIB_G(err).severity);
	}

	return ret;
}

const pdo_driver_t pdo_dblib_driver = {
#ifdef PDO_DBLIB_IS_MSSQL
	PDO_DRIVER_HEADER(mssql),
#elif defined(PHP_WIN32)
#define PDO_DBLIB_IS_SYBASE
	PDO_DRIVER_HEADER(sybase),
#else
	PDO_DRIVER_HEADER(dblib),
#endif
	pdo_dblib_handle_factory
};
