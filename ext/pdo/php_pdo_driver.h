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
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_DRIVER_H
#define PHP_PDO_DRIVER_H

#include "php_pdo.h"

/* forward declarations */
typedef struct _pdo_dbh_t 		 pdo_dbh_t;
typedef struct _pdo_dbh_object_t pdo_dbh_object_t;
typedef struct _pdo_stmt_t		 pdo_stmt_t;
typedef struct _pdo_row_t		 pdo_row_t;
struct pdo_bound_param_data;

#ifdef PHP_WIN32
typedef __int64 pdo_int64_t;
typedef unsigned __int64 pdo_uint64_t;
#else
typedef long long int pdo_int64_t;
typedef unsigned long long int pdo_uint64_t;
#endif
PDO_API char *php_pdo_int64_to_str(pdo_int64_t i64);

#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif

#define PDO_DRIVER_API	20150127

enum pdo_param_type {
	PDO_PARAM_NULL,

	/* int as in long (the php native int type).
	 * If you mark a column as an int, PDO expects get_col to return
	 * a pointer to a long */
	PDO_PARAM_INT,

	/* get_col ptr should point to start of the string buffer */
	PDO_PARAM_STR,

	/* get_col: when len is 0 ptr should point to a php_stream *,
	 * otherwise it should behave like a string. Indicate a NULL field
	 * value by setting the ptr to NULL */
	PDO_PARAM_LOB,

	/* get_col: will expect the ptr to point to a new PDOStatement object handle,
	 * but this isn't wired up yet */
	PDO_PARAM_STMT, /* hierarchical result set */

	/* get_col ptr should point to a zend_bool */
	PDO_PARAM_BOOL,

	/* get_col ptr should point to a zval*
	   and the driver is responsible for adding correct type information to get_column_meta()
	 */
	PDO_PARAM_ZVAL,

	/* magic flag to denote a parameter as being input/output */
	PDO_PARAM_INPUT_OUTPUT = 0x80000000
};

#define PDO_PARAM_FLAGS			0xFFFF0000

#define PDO_PARAM_TYPE(x)		((x) & ~PDO_PARAM_FLAGS)

enum pdo_fetch_type {
	PDO_FETCH_USE_DEFAULT,
	PDO_FETCH_LAZY,
	PDO_FETCH_ASSOC,
	PDO_FETCH_NUM,
	PDO_FETCH_BOTH,
	PDO_FETCH_OBJ,
	PDO_FETCH_BOUND, /* return true/false only; rely on bound columns */
	PDO_FETCH_COLUMN,	/* fetch a numbered column only */
	PDO_FETCH_CLASS,	/* create an instance of named class, call ctor and set properties */
	PDO_FETCH_INTO,		/* fetch row into an existing object */
	PDO_FETCH_FUNC,		/* fetch into function and return its result */
	PDO_FETCH_NAMED,    /* like PDO_FETCH_ASSOC, but can handle duplicate names */
	PDO_FETCH_KEY_PAIR,	/* fetch into an array where the 1st column is a key and all subsequent columns are values */
	PDO_FETCH__MAX /* must be last */
};

#define PDO_FETCH_FLAGS     0xFFFF0000  /* fetchAll() modes or'd to PDO_FETCH_XYZ */
#define PDO_FETCH_GROUP     0x00010000  /* fetch into groups */
#define PDO_FETCH_UNIQUE    0x00030000  /* fetch into groups assuming first col is unique */
#define PDO_FETCH_CLASSTYPE 0x00040000  /* fetch class gets its class name from 1st column */
#define PDO_FETCH_SERIALIZE 0x00080000  /* fetch class instances by calling serialize */
#define PDO_FETCH_PROPS_LATE 0x00100000  /* fetch props after calling ctor */

/* fetch orientation for scrollable cursors */
enum pdo_fetch_orientation {
	PDO_FETCH_ORI_NEXT,		/* default: fetch the next available row */
	PDO_FETCH_ORI_PRIOR,	/* scroll back to prior row and fetch that */
	PDO_FETCH_ORI_FIRST,	/* scroll to the first row and fetch that */
	PDO_FETCH_ORI_LAST,		/* scroll to the last row and fetch that */
	PDO_FETCH_ORI_ABS,		/* scroll to an absolute numbered row and fetch that */
	PDO_FETCH_ORI_REL		/* scroll relative to the current row, and fetch that */
};

enum pdo_attribute_type {
	PDO_ATTR_AUTOCOMMIT,	/* use to turn on or off auto-commit mode */
	PDO_ATTR_PREFETCH,		/* configure the prefetch size for drivers that support it. Size is in KB */
	PDO_ATTR_TIMEOUT,		/* connection timeout in seconds */
	PDO_ATTR_ERRMODE,		/* control how errors are handled */
	PDO_ATTR_SERVER_VERSION,	/* database server version */
	PDO_ATTR_CLIENT_VERSION,	/* client library version */
	PDO_ATTR_SERVER_INFO,		/* server information */
	PDO_ATTR_CONNECTION_STATUS,	/* connection status */
	PDO_ATTR_CASE,				/* control case folding for portability */
	PDO_ATTR_CURSOR_NAME,		/* name a cursor for use in "WHERE CURRENT OF <name>" */
	PDO_ATTR_CURSOR,			/* cursor type */
	PDO_ATTR_ORACLE_NULLS,		/* convert empty strings to NULL */
	PDO_ATTR_PERSISTENT,		/* pconnect style connection */
	PDO_ATTR_STATEMENT_CLASS,	/* array(classname, array(ctor_args)) to specify the class of the constructed statement */
	PDO_ATTR_FETCH_TABLE_NAMES, /* include table names in the column names, where available */
	PDO_ATTR_FETCH_CATALOG_NAMES, /* include the catalog/db name names in the column names, where available */
	PDO_ATTR_DRIVER_NAME,		  /* name of the driver (as used in the constructor) */
	PDO_ATTR_STRINGIFY_FETCHES,	/* converts integer/float types to strings during fetch */
	PDO_ATTR_MAX_COLUMN_LEN,	/* make database calculate maximum length of data found in a column */
	PDO_ATTR_DEFAULT_FETCH_MODE, /* Set the default fetch mode */
	PDO_ATTR_EMULATE_PREPARES,  /* use query emulation rather than native */

	/* this defines the start of the range for driver specific options.
	 * Drivers should define their own attribute constants beginning with this
	 * value. */
	PDO_ATTR_DRIVER_SPECIFIC = 1000
};

enum pdo_cursor_type {
	PDO_CURSOR_FWDONLY,		/* forward only cursor (default) */
	PDO_CURSOR_SCROLL		/* scrollable cursor */
};

/* SQL-92 SQLSTATE error codes.

The character string value returned for an SQLSTATE consists of a two-character
class value followed by a three-character subclass value. A class value of 01
indicates a warning and is accompanied by a return code of
SQL_SUCCESS_WITH_INFO.

Class values other than '01', except for the class 'IM',
indicate an error and are accompanied by a return code of SQL_ERROR. The class
'IM' is specific to warnings and errors that derive from the implementation of
ODBC itself.

The subclass value '000' in any class indicates that there is no
subclass for that SQLSTATE. The assignment of class and subclass values is
defined by SQL-92.
*/

typedef char pdo_error_type[6]; /* SQLSTATE */


#define PDO_ERR_NONE				"00000"

enum pdo_error_mode {
	PDO_ERRMODE_SILENT,		/* just set error codes */
	PDO_ERRMODE_WARNING,	/* raise E_WARNING */
	PDO_ERRMODE_EXCEPTION	/* throw exceptions */
};

enum pdo_case_conversion {
	PDO_CASE_NATURAL,
	PDO_CASE_UPPER,
	PDO_CASE_LOWER
};

/* oracle interop settings */
enum pdo_null_handling {
	PDO_NULL_NATURAL = 0,
	PDO_NULL_EMPTY_STRING = 1,
	PDO_NULL_TO_STRING = 2
};

/* {{{ utils for reading attributes set as driver_options */
static inline zend_long pdo_attr_lval(zval *options, enum pdo_attribute_type option_name, zend_long defval)
{
	zval *v;

	if (options && (v = zend_hash_index_find(Z_ARRVAL_P(options), option_name))) {
		return zval_get_long(v);
	}
	return defval;
}
static inline zend_string *pdo_attr_strval(zval *options, enum pdo_attribute_type option_name, zend_string *defval)
{
	zval *v;

	if (options && (v = zend_hash_index_find(Z_ARRVAL_P(options), option_name))) {
		return zval_get_string(v);
	}
	return defval ? zend_string_copy(defval) : NULL;
}
/* }}} */

/* This structure is registered with PDO when a PDO driver extension is
 * initialized */
typedef struct {
	const char	*driver_name;
	size_t		driver_name_len;
	zend_ulong	api_version; /* needs to be compatible with PDO */

#define PDO_DRIVER_HEADER(name)	\
	#name, sizeof(#name)-1, \
	PDO_DRIVER_API

	/* create driver specific portion of the database handle and stash it into
	 * the dbh.  dbh contains the data source string and flags for this
	 * instance.  You MUST respect dbh->is_persistent and pass that flag to
	 * pemalloc() for all allocations that are stored in the dbh or your instance
	 * data in the db, otherwise you will crash PHP when persistent connections
	 * are used.
	 */
	int (*db_handle_factory)(pdo_dbh_t *dbh, zval *driver_options);

} pdo_driver_t;

/* {{{ methods for a database handle */

/* close or otherwise disconnect the database */
typedef int (*pdo_dbh_close_func)(pdo_dbh_t *dbh);

/* prepare a statement and stash driver specific portion into stmt */
typedef int (*pdo_dbh_prepare_func)(pdo_dbh_t *dbh, const char *sql, size_t sql_len, pdo_stmt_t *stmt, zval *driver_options);

/* execute a statement (that does not return a result set) */
typedef zend_long (*pdo_dbh_do_func)(pdo_dbh_t *dbh, const char *sql, size_t sql_len);

/* quote a string */
typedef int (*pdo_dbh_quote_func)(pdo_dbh_t *dbh, const char *unquoted, size_t unquotedlen, char **quoted, size_t *quotedlen, enum pdo_param_type paramtype);

/* transaction related */
typedef int (*pdo_dbh_txn_func)(pdo_dbh_t *dbh);

/* setting of attributes */
typedef int (*pdo_dbh_set_attr_func)(pdo_dbh_t *dbh, zend_long attr, zval *val);

/* return last insert id.  NULL indicates error condition, otherwise, the return value
 * MUST be an emalloc'd NULL terminated string. */
typedef char *(*pdo_dbh_last_id_func)(pdo_dbh_t *dbh, const char *name, size_t *len);

/* fetch error information.  if stmt is not null, fetch information pertaining
 * to the statement, otherwise fetch global error information.  The driver
 * should add the following information to the array "info" in this order:
 * - native error code
 * - string representation of the error code ... any other optional driver
 *   specific data ...  */
typedef	int (*pdo_dbh_fetch_error_func)(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info);

/* fetching of attributes */
typedef int (*pdo_dbh_get_attr_func)(pdo_dbh_t *dbh, zend_long attr, zval *val);

/* checking/pinging persistent connections; return SUCCESS if the connection
 * is still alive and ready to be used, FAILURE otherwise.
 * You may set this handler to NULL, which is equivalent to returning SUCCESS. */
typedef int (*pdo_dbh_check_liveness_func)(pdo_dbh_t *dbh);

/* called at request end for each persistent dbh; this gives the driver
 * the opportunity to safely release resources that only have per-request
 * scope */
typedef void (*pdo_dbh_request_shutdown)(pdo_dbh_t *dbh);

/* for adding methods to the dbh or stmt objects
pointer to a list of driver specific functions. The convention is
to prefix the function names using the PDO driver name; this will
reduce the chance of collisions with future functionality in the
PDO class or in user code (they can extend the PDO object).
*/
enum {
	PDO_DBH_DRIVER_METHOD_KIND_DBH = 0,
	PDO_DBH_DRIVER_METHOD_KIND_STMT,
	PDO_DBH_DRIVER_METHOD_KIND__MAX
};

typedef const zend_function_entry *(*pdo_dbh_get_driver_methods_func)(pdo_dbh_t *dbh, int kind);

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
	pdo_dbh_check_liveness_func	check_liveness;
	pdo_dbh_get_driver_methods_func get_driver_methods;
	pdo_dbh_request_shutdown	persistent_shutdown;
	pdo_dbh_txn_func		in_transaction;
};

/* }}} */

/* {{{ methods for a statement handle */

/* free the statement handle */
typedef int (*pdo_stmt_dtor_func)(pdo_stmt_t *stmt);

/* start the query */
typedef int (*pdo_stmt_execute_func)(pdo_stmt_t *stmt);

/* causes the next row in the set to be fetched; indicates if there are no
 * more rows.  The ori and offset params modify which row should be returned,
 * if the stmt represents a scrollable cursor */
typedef int (*pdo_stmt_fetch_func)(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, zend_long offset);

/* queries information about the type of a column, by index (0 based).
 * Driver should populate stmt->columns[colno] with appropriate info */
typedef int (*pdo_stmt_describe_col_func)(pdo_stmt_t *stmt, int colno);

/* retrieves pointer and size of the value for a column.
 * Note that PDO expects the driver to manage the lifetime of this data;
 * it will copy the value into a zval on behalf of the script.
 * If the driver sets caller_frees, ptr should point to emalloc'd memory
 * and PDO will free it as soon as it is done using it.
 */
typedef int (*pdo_stmt_get_col_data_func)(pdo_stmt_t *stmt, int colno, char **ptr, size_t *len, int *caller_frees);

/* hook for bound params */
enum pdo_param_event {
	PDO_PARAM_EVT_ALLOC,
	PDO_PARAM_EVT_FREE,
	PDO_PARAM_EVT_EXEC_PRE,
	PDO_PARAM_EVT_EXEC_POST,
	PDO_PARAM_EVT_FETCH_PRE,
	PDO_PARAM_EVT_FETCH_POST,
	PDO_PARAM_EVT_NORMALIZE
};

typedef int (*pdo_stmt_param_hook_func)(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event event_type);

/* setting of attributes */
typedef int (*pdo_stmt_set_attr_func)(pdo_stmt_t *stmt, zend_long attr, zval *val);

/* fetching of attributes */
typedef int (*pdo_stmt_get_attr_func)(pdo_stmt_t *stmt, zend_long attr, zval *val);

/* retrieves meta data for a numbered column.
 * Returns SUCCESS/FAILURE.
 * On SUCCESS, fill in return_value with an array with the following fields.
 * If a particular field is not supported, then the driver simply does not add it to
 * the array, so that scripts can use isset() to check for it.
 *
 * ### this is just a rough first cut, and subject to change ###
 *
 * these are added by PDO itself, based on data from the describe handler:
 *   name => the column name
 *   len => the length/size of the column
 *   precision => precision of the column
 *   pdo_type => an integer, one of the PDO_PARAM_XXX values
 *
 *   scale => the floating point scale
 *   table => the table for that column
 *   type => a string representation of the type, mapped to the PHP equivalent type name
 *   native_type => a string representation of the type, native style, if different from
 *                  the mapped name.
 *   flags => an array of flags including zero or more of the following:
 *            primary_key, not_null, unique_key, multiple_key, unsigned, auto_increment, blob
 *
 * Any driver specific data should be returned using a prefixed key or value.
 * Eg: custom data for the mysql driver would use either
 *   'mysql:foobar' => 'some data' // to add a new key to the array
 * or
 *   'flags' => array('not_null', 'mysql:some_flag'); // to add data to an existing key
 */
typedef int (*pdo_stmt_get_column_meta_func)(pdo_stmt_t *stmt, zend_long colno, zval *return_value);

/* advances the statement to the next rowset of the batch.
 * If it returns 1, PDO will tear down its idea of columns
 * and meta data.  If it returns 0, PDO will indicate an error
 * to the caller. */
typedef int (*pdo_stmt_next_rowset_func)(pdo_stmt_t *stmt);

/* closes the active cursor on a statement, leaving the prepared
 * statement ready for re-execution.  Useful to explicitly state
 * that you are done with a given rowset, without having to explicitly
 * fetch all the rows. */
typedef int (*pdo_stmt_cursor_closer_func)(pdo_stmt_t *stmt);

struct pdo_stmt_methods {
	pdo_stmt_dtor_func			dtor;
	pdo_stmt_execute_func		executer;
	pdo_stmt_fetch_func			fetcher;
	pdo_stmt_describe_col_func	describer;
	pdo_stmt_get_col_data_func	get_col;
	pdo_stmt_param_hook_func	param_hook;
	pdo_stmt_set_attr_func		set_attribute;
	pdo_stmt_get_attr_func		get_attribute;
	pdo_stmt_get_column_meta_func get_column_meta;
	pdo_stmt_next_rowset_func		next_rowset;
	pdo_stmt_cursor_closer_func 	cursor_closer;
};

/* }}} */

enum pdo_placeholder_support {
	PDO_PLACEHOLDER_NONE=0,
	PDO_PLACEHOLDER_NAMED=1,
	PDO_PLACEHOLDER_POSITIONAL=2
};

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

	/* if true, commit or rollBack is allowed to be called */
	unsigned in_txn:1;

	/* max length a single character can become after correct quoting */
	unsigned max_escaped_char_length:3;

	/* oracle compat; see enum pdo_null_handling */
	unsigned oracle_nulls:2;

	/* when set, convert int/floats to strings */
	unsigned stringify:1;

	/* the sum of the number of bits here and the bit fields preceding should
	 * equal 32 */
	unsigned _reserved_flags:21;

	/* data source string used to open this handle */
	const char *data_source;
	size_t data_source_len;

	/* the global error code. */
	pdo_error_type error_code;

	enum pdo_error_mode error_mode;

	enum pdo_case_conversion native_case, desired_case;

	/* persistent hash key associated with this handle */
	const char *persistent_id;
	size_t persistent_id_len;
	unsigned int refcount;

	/* driver specific "class" methods for the dbh and stmt */
	HashTable *cls_methods[PDO_DBH_DRIVER_METHOD_KIND__MAX];

	pdo_driver_t *driver;

	zend_class_entry *def_stmt_ce;

	zval def_stmt_ctor_args;

	/* when calling PDO::query(), we need to keep the error
	 * context from the statement around until we next clear it.
	 * This will allow us to report the correct error message
	 * when PDO::query() fails */
	pdo_stmt_t *query_stmt;
	zval query_stmt_zval;

	/* defaults for fetches */
	enum pdo_fetch_type default_fetch_type;
};

/* represents a connection to a database */
struct _pdo_dbh_object_t {
	pdo_dbh_t *inner;
	/* these items must appear in this order at the beginning of the
       struct so that this can be cast as a zend_object.  we need this
       to allow the extending class to escape all the custom handlers
	   that PDO declares.
    */
	zend_object std;
};

static inline pdo_dbh_t *php_pdo_dbh_fetch_inner(zend_object *obj) {
	return (pdo_dbh_t *)(((pdo_dbh_object_t *)((char*)(obj) - XtOffsetOf(pdo_dbh_object_t, std)))->inner);
}

static inline pdo_dbh_object_t *php_pdo_dbh_fetch_object(zend_object *obj) {
	return (pdo_dbh_object_t *)((char*)(obj) - XtOffsetOf(pdo_dbh_object_t, std));
}

#define Z_PDO_DBH_P(zv) php_pdo_dbh_fetch_inner(Z_OBJ_P((zv)))
#define Z_PDO_OBJECT_P(zv) php_pdo_dbh_fetch_object(Z_OBJ_P((zv)))

/* describes a column */
struct pdo_column_data {
	zend_string *name;
	size_t maxlen;
	zend_ulong precision;
	enum pdo_param_type param_type;

	/* don't touch this unless your name is dbdo */
	void *dbdo_data;
};

/* describes a bound parameter */
struct pdo_bound_param_data {
	zval parameter;				/* the variable itself */

	zval driver_params;			/* optional parameter(s) for the driver */

	zend_long paramno; /* if -1, then it has a name, and we don't know the index *yet* */
	zend_string *name;

	zend_long max_value_len;	/* as a hint for pre-allocation */

	void *driver_data;

	pdo_stmt_t *stmt;	/* for convenience in dtor */

	enum pdo_param_type param_type; /* desired or suggested variable type */

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
	/* if true, the statement supports placeholders and can implement
	 * bindParam() for its prepared statements, if false, PDO should
	 * emulate prepare and bind on its behalf */
	unsigned supports_placeholders:2;

	unsigned _reserved:29;

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
	/* When rewriting from named to positional, this maps positions to names */
	HashTable *bound_param_map;
	/* keep track of PHP variables bound to named (or positional) columns
	 * in the result set */
	HashTable *bound_columns;

	/* not always meaningful */
	zend_long row_count;

	/* used to hold the statement's current query */
	char *query_string;
	size_t query_stringlen;

	/* the copy of the query with expanded binds ONLY for emulated-prepare drivers */
	char *active_query_string;
	size_t active_query_stringlen;

	/* the cursor specific error code. */
	pdo_error_type error_code;

	/* for lazy fetches, we always return the same lazy object handle.
	 * Let's keep it here. */
	zval lazy_object_ref;
	zend_ulong refcount;

	/* defaults for fetches */
	enum pdo_fetch_type default_fetch_type;
	union {
		int column;
		struct {
			zval ctor_args;            /* freed */
			zend_fcall_info fci;
			zend_fcall_info_cache fcc;
			zval retval;
			zend_class_entry *ce;
		} cls;
		struct {
			zval fetch_args;           /* freed */
			zend_fcall_info fci;
			zend_fcall_info_cache fcc;
			zval object;
			zval function;
			zval *values;              /* freed */
		} func;
		zval into;
	} fetch;

	/* used by the query parser for driver specific
	 * parameter naming (see pgsql driver for example) */
	const char *named_rewrite_template;

	/* these items must appear in this order at the beginning of the
       struct so that this can be cast as a zend_object.  we need this
       to allow the extending class to escape all the custom handlers
	   that PDO declares.
    */
	zend_object std;
};

static inline pdo_stmt_t *php_pdo_stmt_fetch_object(zend_object *obj) {
	return (pdo_stmt_t *)((char*)(obj) - XtOffsetOf(pdo_stmt_t, std));
}

#define Z_PDO_STMT_P(zv) php_pdo_stmt_fetch_object(Z_OBJ_P((zv)))

struct _pdo_row_t {
	zend_object std;
	pdo_stmt_t *stmt;
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
		zend_ulong data_source_len, struct pdo_data_src_parser *parsed,
		int nparams);

PDO_API zend_class_entry *php_pdo_get_dbh_ce(void);
PDO_API zend_class_entry *php_pdo_get_exception(void);

PDO_API int pdo_parse_params(pdo_stmt_t *stmt, char *inquery, size_t inquery_len,
	char **outquery, size_t *outquery_len);

PDO_API void pdo_raise_impl_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt,
	const char *sqlstate, const char *supp);

PDO_API void php_pdo_dbh_addref(pdo_dbh_t *dbh);
PDO_API void php_pdo_dbh_delref(pdo_dbh_t *dbh);

PDO_API void php_pdo_free_statement(pdo_stmt_t *stmt);


#endif /* PHP_PDO_DRIVER_H */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
