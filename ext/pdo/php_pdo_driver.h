/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_DRIVER_H
#define PHP_PDO_DRIVER_H

#include "php_pdo.h"

/* forward declarations */
typedef struct _pdo_dbh_t 	pdo_dbh_t;
typedef struct _pdo_stmt_t	pdo_stmt_t;
struct pdo_bound_param_data;

#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif

#define PDO_DRIVER_API	20040513

enum pdo_param_type {
	PDO_PARAM_NULL,
	PDO_PARAM_INT,
	PDO_PARAM_STR,
	PDO_PARAM_LOB,
	PDO_PARAM_STMT, /* hierarchical result set */
};

enum pdo_fetch_type {
	PDO_FETCH_LAZY,
	PDO_FETCH_ASSOC,
	PDO_FETCH_NUM,
	PDO_FETCH_BOTH,
	PDO_FETCH_OBJ,
	PDO_FETCH_BOUND, /* return true/false only; rely on bound columns */
};

enum pdo_attribute_type {
	PDO_ATTR_AUTOCOMMIT,	/* use to turn on or off auto-commit mode */
	PDO_ATTR_SCROLL,		/* ask for a scrollable cursor (when you prepare()) */
	PDO_ATTR_PREFETCH,		/* configure the prefetch size for drivers that support it */
	PDO_ATTR_TIMEOUT,		/* connection timeout in seconds */
	PDO_ATTR_ERRMODE,		/* control how errors are handled */
	PDO_ATTR_SERVER_VERSION,	/* database server version */
	PDO_ATTR_CLIENT_VERSION,	/* client library version */
	PDO_ATTR_SERVER_INFO,		/* server information */
	PDO_ATTR_CONNECTION_STATUS,	/* connection status */
	PDO_ATTR_CASE,				/* control case folding for portability */
	PDO_ATTR_CURSOR_NAME,		/* name a cursor for use in "WHERE CURRENT OF <name>" */
	PDO_ATTR_CURSOR,			/* cursor type */

	/* this defines the start of the range for driver specific options.
	 * Drivers should define their own attribute constants beginning with this
	 * value. */
	PDO_ATTR_DRIVER_SPECIFIC = 1000
};

enum pdo_cursor_type {
	PDO_CURSOR_FWDONLY,		/* forward only cursor (default) */
	PDO_CURSOR_SCROLL,		/* scrollable cursor */
};

/* generic error code values.
 * Don't want to go overboard with these.
 * */
enum pdo_error_type {
	PDO_ERR_NONE,			/* no error condition */
	PDO_ERR_CANT_MAP,		/* no way to map native error to the generic codes; consult the native error for more info */
	PDO_ERR_SYNTAX,
	PDO_ERR_CONSTRAINT,
	PDO_ERR_NOT_FOUND,
	PDO_ERR_ALREADY_EXISTS,
	PDO_ERR_NOT_IMPLEMENTED,
	PDO_ERR_MISMATCH,
	PDO_ERR_TRUNCATED,
	PDO_ERR_DISCONNECTED,
};

enum pdo_error_mode {
	PDO_ERRMODE_SILENT,		/* just set error codes */
	PDO_ERRMODE_WARNING,	/* raise E_WARNING */
	PDO_ERRMODE_EXCEPTION,	/* throw exceptions */
};

enum pdo_case_conversion {
	PDO_CASE_NATURAL,
	PDO_CASE_UPPER,
	PDO_CASE_LOWER
};

/* {{{ utils for reading attributes set as driver_options */
static inline long pdo_attr_lval(zval *options, enum pdo_fetch_type option_name, long defval TSRMLS_DC)
{
	zval **v;

	if (options && SUCCESS == zend_hash_index_find(Z_ARRVAL_P(options), option_name, (void**)&v)) {
		convert_to_long_ex(v);
		return Z_LVAL_PP(v);
	}
	return defval;
}
/* }}} */

/* This structure is registered with PDO when a PDO driver extension is
 * initialized */
typedef struct {
	const char		*driver_name;
	unsigned long	driver_name_len;
	unsigned long	api_version; /* needs to be compatible with PDO */

#define PDO_DRIVER_HEADER(name)	\
	#name, sizeof(#name)-1, \
	PDO_DRIVER_API
	
	/* create driver specific portion of the database handle and stash it into
	 * the dbh.  dbh contains the data source string and flags for this
	 * instance */
	int (*db_handle_factory)(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC);

} pdo_driver_t;

/* {{{ methods for a database handle */

/* close or otherwise disconnect the database */
typedef int (*pdo_dbh_close_func)(pdo_dbh_t *dbh TSRMLS_DC);

/* prepare a statement and stash driver specific portion into stmt */
typedef int (*pdo_dbh_prepare_func)(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC);

/* execute a statement (that does not return a result set) */
typedef long (*pdo_dbh_do_func)(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC);

/* quote a string */
typedef int (*pdo_dbh_quote_func)(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen TSRMLS_DC);

/* transaction related */
typedef int (*pdo_dbh_txn_func)(pdo_dbh_t *dbh TSRMLS_DC);

/* setting of attributes */
typedef int (*pdo_dbh_set_attr_func)(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC);

/* return last insert id */
typedef long (*pdo_dbh_last_id_func)(pdo_dbh_t *dbh TSRMLS_DC);

/* fetch error information.  if stmt is not null, fetch information pertaining
 * to the statement, otherwise fetch global error information.  The driver
 * should add the following information to the array "info" in this order:
 * - native error code
 * - string representation of the error code ... any other optional driver
 *   specific data ...  */
typedef	int (*pdo_dbh_fetch_error_func)(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC);

/* fetching of attributes */
typedef int (*pdo_dbh_get_attr_func)(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC);

struct pdo_dbh_methods {
	pdo_dbh_close_func		closer;
	pdo_dbh_prepare_func	preparer;
	pdo_dbh_do_func			doer;
	pdo_dbh_quote_func		quoter;
	pdo_dbh_txn_func		begin;
	pdo_dbh_txn_func		commit;
	pdo_dbh_txn_func		rollback;
	pdo_dbh_set_attr_func	set_attribute;
	pdo_dbh_last_id_func		last_id;
	pdo_dbh_fetch_error_func	fetch_err;
	pdo_dbh_get_attr_func   	get_attribute;
};

/* }}} */

/* {{{ methods for a statement handle */

/* free the statement handle */
typedef int (*pdo_stmt_dtor_func)(pdo_stmt_t *stmt TSRMLS_DC);

/* start the query */
typedef int (*pdo_stmt_execute_func)(pdo_stmt_t *stmt TSRMLS_DC);

/* causes the next row in the set to be fetched; indicates if there are no
 * more rows */
typedef int (*pdo_stmt_fetch_func)(pdo_stmt_t *stmt TSRMLS_DC);

/* queries information about the type of a column, by index (0 based).
 * Driver should populate stmt->columns[colno] with appropriate info */
typedef int (*pdo_stmt_describe_col_func)(pdo_stmt_t *stmt, int colno TSRMLS_DC);

/* retrieves pointer and size of the value for a column */
typedef int (*pdo_stmt_get_col_data_func)(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len TSRMLS_DC);

/* hook for bound params */
enum pdo_param_event {
	PDO_PARAM_EVT_ALLOC,
	PDO_PARAM_EVT_FREE,
	PDO_PARAM_EVT_EXEC_PRE,
	PDO_PARAM_EVT_EXEC_POST,
	PDO_PARAM_EVT_FETCH_PRE,
	PDO_PARAM_EVT_FETCH_POST,
};

typedef int (*pdo_stmt_param_hook_func)(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event event_type TSRMLS_DC);

struct pdo_stmt_methods {
	pdo_stmt_dtor_func			dtor;
	pdo_stmt_execute_func		executer;
	pdo_stmt_fetch_func			fetcher;
	pdo_stmt_describe_col_func	describer;
	pdo_stmt_get_col_data_func	get_col;
	pdo_stmt_param_hook_func	param_hook;
};

/* }}} */

enum pdo_placeholder_support {
	PDO_PLACEHOLDER_NONE=0,
	PDO_PLACEHOLDER_NAMED=1,
	PDO_PLACEHOLDER_POSITIONAL=2
};

/* represents a connection to a database */
struct _pdo_dbh_t {
	/* driver specific methods */
	struct pdo_dbh_methods *methods;
	/* driver specific data */
	void *driver_data;

	/* credentials */
	char *username, *password;
	
	/* if true, then data stored and pointed at by this handle must all be
	 * persistently allocated */
	unsigned is_persistent:1;

	/* if true, driver should act as though a COMMIT were executed between
	 * each executed statement; otherwise, COMMIT must be carried out manually
	 * */
	unsigned auto_commit:1;

	/* if true, the handle has been closed and will not function anymore */
	unsigned is_closed:1;

	/* if true, the driver requires that memory be allocated explicitly for
	 * the columns that are returned */
	unsigned alloc_own_columns:1;

	/* if true, the driver supports placeholders and can implement
	 * bindParam() for its prepared statements, if false, PDO should
	 * emulate prepare and bind on its behalf */
	unsigned supports_placeholders:2;

	/* if true, commit or rollBack is allowed to be called */
	unsigned in_txn:1;

	/* max length a single character can become after correct quoting */
	unsigned max_escaped_char_length:3;

	/* the sum of the number of bits here and the bit fields preceeding should
	 * equal 32 */
	unsigned _reserved_flags:22;

	/* data source string used to open this handle */
	const char *data_source;
	unsigned long data_source_len;

	/* the global error code. */
	enum pdo_error_type error_code;

	enum pdo_error_mode error_mode;

	enum pdo_case_conversion native_case, desired_case;
#if 0
	/* persistent hash key associated with this handle */
	const char *persistent_id;
	/* and the list id associated with it */
	int persistent_rsrc_id;
#endif
};

/* describes a column */
struct pdo_column_data {
	char *name;
	long namelen;
	unsigned long maxlen;
	enum pdo_param_type param_type;
	unsigned long precision;
};

/* describes a bound parameter */
struct pdo_bound_param_data {
	long paramno; /* if -1, then it has a name, and we don't know the index *yet* */
	char *name;
	long namelen;

	long max_value_len;	/* as a hint for pre-allocation */
	
	zval *parameter;				/* the variable itself */
	enum pdo_param_type param_type; /* desired or suggested type */

	zval *driver_params;			/* optional parameter(s) for the driver */
	void *driver_data;

	pdo_stmt_t *stmt;	/* for convenience in dtor */
	int is_param;		/* parameter or column ? */
};

/* represents a prepared statement */
struct _pdo_stmt_t {
	/* driver specifics */
	struct pdo_stmt_methods *methods;
	void *driver_data;

	/* if true, we've already successfully executed this statement at least
	 * once */
	unsigned executed:1;
	unsigned _reserved:31;

	/* the number of columns in the result set; not valid until after
	 * the statement has been executed at least once.  In some cases, might
	 * not be valid until fetch (at the driver level) has been called at least once.
	 * */
	int column_count;
	struct pdo_column_data *columns;
	
	/* we want to keep the dbh alive while we live, so we own a reference */
	zval database_object_handle;
	pdo_dbh_t *dbh;

	/* keep track of bound input parameters.  Some drivers support
	 * input/output parameters, but you can't rely on that working */
	HashTable *bound_params;
	/* keep track of PHP variables bound to named (or positional) columns
	 * in the result set */
	HashTable *bound_columns;

	/* not always meaningful */
	long row_count;

	/* used to hold the statement's current query */
	char *query_string;
	int query_stringlen;

	/* the copy of the query with expanded binds ONLY for emulated-prepare drivers */
	char *active_query_string;
	int active_query_stringlen;

	/* the cursor specific error code. */
	enum pdo_error_type error_code;

	/* for lazy fetches, we always return the same lazy object handle.
	 * Let's keep it here. */
	zval lazy_object_ref;
	unsigned long refcount;
};

/* call this in MINIT to register your PDO driver */
PDO_API int php_pdo_register_driver(pdo_driver_t *driver);
/* call this in MSHUTDOWN to unregister your PDO driver */
PDO_API void php_pdo_unregister_driver(pdo_driver_t *driver);

/* For the convenience of drivers, this function will parse a data source
 * string, of the form "name=value; name2=value2" and populate variables
 * according to the data you pass in and array of pdo_data_src_parser structures */
struct pdo_data_src_parser {
	const char *optname;
	char *optval;
	int freeme;
};

PDO_API int php_pdo_parse_data_source(const char *data_source,
		unsigned long data_source_len, struct pdo_data_src_parser *parsed,
		int nparams);

PDO_API zend_class_entry *php_pdo_get_exception(void);

#endif /* PHP_PDO_DRIVER_H */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
