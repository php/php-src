/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#if HAVE_OCI8
# ifndef PHP_OCI8_INT_H
#  define PHP_OCI8_INT_H

/* misc defines {{{ */
# if (defined(__osf__) && defined(__alpha))
#  ifndef A_OSF
#	define A_OSF
#  endif
#  ifndef OSF1
#	define OSF1
#  endif
#  ifndef _INTRINSICS
#	define _INTRINSICS
#  endif
# endif /* osf alpha */

#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif
/* }}} */

#include "ext/standard/php_string.h"
#include <oci.h>

extern int le_connection;
extern int le_pconnection;
extern int le_statement;
extern int le_descriptor;
extern int le_collection;
extern int le_server;
extern int le_session;

extern zend_class_entry *oci_lob_class_entry_ptr;
extern zend_class_entry *oci_coll_class_entry_ptr;

/* constants {{{ */
#define PHP_OCI_SEEK_SET 0
#define PHP_OCI_SEEK_CUR 1
#define PHP_OCI_SEEK_END 2

#define PHP_OCI_MAX_NAME_LEN  64
#define PHP_OCI_MAX_DATA_SIZE INT_MAX
#define PHP_OCI_PIECE_SIZE	  ((64*1024)-1)
#define PHP_OCI_LOB_BUFFER_SIZE 1048576l  /* 1Mb seems to be the most reasonable buffer size for LOB reading */

#define PHP_OCI_ASSOC				(1<<0)
#define PHP_OCI_NUM					(1<<1)
#define PHP_OCI_BOTH				(PHP_OCI_ASSOC|PHP_OCI_NUM)

#define PHP_OCI_RETURN_NULLS		(1<<2)
#define PHP_OCI_RETURN_LOBS			(1<<3)

#define PHP_OCI_FETCHSTATEMENT_BY_COLUMN	(1<<4)
#define PHP_OCI_FETCHSTATEMENT_BY_ROW		(1<<5)
#define PHP_OCI_FETCHSTATEMENT_BY			(PHP_OCI_FETCHSTATEMENT_BY_COLUMN | PHP_OCI_FETCHSTATEMENT_BY_ROW)

#define PHP_OCI_LOB_BUFFER_DISABLED 0
#define PHP_OCI_LOB_BUFFER_ENABLED  1
#define PHP_OCI_LOB_BUFFER_USED     2

/* The mode parameter for oci_connect() is overloaded and accepts both
 * privilege and external authentication flags OR'd together.
 * PHP_OCI_CRED_EXT must be distinct from the OCI_xxx privilege
 * values.
 */
#define PHP_OCI_CRED_EXT                    (1<<31)
#if ((PHP_OCI_CRED_EXT == OCI_DEFAULT) || (PHP_OCI_CRED_EXT & (OCI_SYSOPER | OCI_SYSDBA)))
#error Invalid value for PHP_OCI_CRED_EXT
#endif

/*
 * Name passed to Oracle for tracing.  Note some DB views only show
 * the first nine characters of the driver name.
 */
#define PHP_OCI8_DRIVER_NAME "PHP OCI8 " PHP_OCI8_VERSION

/* }}} */

typedef struct { /* php_oci_spool {{{ */
	OCIEnv		 *env;					/*env of this session pool */
	OCIError	 *err;					/* pool's error handle	*/
	OCISPool	 *poolh;				/* pool handle */
	void		 *poolname;				/* session pool name */
	unsigned int  poolname_len;			/* length of session pool name */
	char		 *spool_hash_key;		/* Hash key for session pool in plist */
} php_oci_spool; /* }}} */

typedef struct { /* php_oci_connection {{{ */
	OCIEnv		   *env;						/* private env handle */
	ub2				charset;					/* charset ID */
	OCIServer	   *server;						/* private server handle */
	OCISvcCtx	   *svc;						/* private service context handle */
	OCISession	   *session;					/* private session handle */
	OCIAuthInfo	   *authinfo;					/* Cached authinfo handle for OCISessionGet */
	OCIError	   *err;						/* private error handle */
	php_oci_spool  *private_spool;				/* private session pool (for persistent) */
	sword			errcode;					/* last errcode */

	HashTable	   *descriptors;				/* descriptors hash, used to flush all the LOBs using this connection on commit */
	ulong			descriptor_count;			/* used to index the descriptors hash table.  Not an accurate count */
	unsigned		is_open:1;					/* hels to determine if the connection is dead or not */
	unsigned		is_attached:1;				/* hels to determine if we should detach from the server when closing/freeing the connection */
	unsigned		is_persistent:1;			/* self-descriptive */
	unsigned		used_this_request:1;		/* helps to determine if we should reset connection's next ping time and check its timeout */
	unsigned		needs_commit:1;				/* helps to determine if we should rollback this connection on close/shutdown */
	unsigned		passwd_changed:1;			/* helps determine if a persistent connection hash should be invalidated after a password change */
	unsigned		is_stub:1;					/* flag to keep track whether the connection structure has a real OCI connection associated */
	unsigned		using_spool:1;				/* Is this connection from session pool? */
	int				rsrc_id;					/* resource ID */
	time_t			idle_expiry;				/* time when the connection will be considered as expired */
	time_t		   *next_pingp;					/* (pointer to) time of the next ping */
	char		   *hash_key;					/* hashed details of the connection */
} php_oci_connection; /* }}} */

typedef struct { /* php_oci_descriptor {{{ */
	int					 id;
	ulong				 index;		            /* descriptors hash table index */
	php_oci_connection	*connection;			/* parent connection handle */
	dvoid				*descriptor;			/* OCI descriptor handle */
	ub4					 type;					/* descriptor type (FILE/LOB) */
	int					 lob_current_position;	/* LOB internal pointer */
	int					 lob_size;				/* cached LOB size. -1 = Lob wasn't initialized yet */
	int					 buffering;				/* cached buffering flag. 0 - off, 1 - on, 2 - on and buffer was used */
	ub4					 chunk_size;			/* chunk size of the LOB. 0 - unknown */
	ub1					 charset_form;			/* charset form, required for NCLOBs */
	ub2					 charset_id;			/* charset ID */
	unsigned			 is_open:1;				/* helps to determine if lob is open or not */
} php_oci_descriptor; /* }}} */

typedef struct { /* php_oci_lob_ctx {{{ */
	char			   **lob_data;				/* address of pointer to LOB data */
	ub4					*lob_len;				/* address of LOB length variable (bytes) */
	ub4					 alloc_len;
} php_oci_lob_ctx; /* }}} */

typedef struct { /* php_oci_collection {{{ */
	int					 id;
	php_oci_connection	*connection;			/* parent connection handle */
	OCIType				*tdo;					/* collection's type handle */
	OCITypeCode			 coll_typecode;			/* collection's typecode handle */
	OCIRef				*elem_ref;				/* element's reference handle */
	OCIType				*element_type;			/* element's type handle */
	OCITypeCode			 element_typecode;		/* element's typecode handle */
	OCIColl				*collection;			/* collection handle */
} php_oci_collection; /* }}} */

typedef struct { /* php_oci_define {{{ */
	zval		*zval;			/* zval used in define */
	text		*name;			/* placeholder's name */
	ub4			 name_len;		/* placeholder's name length */
	ub4			 type;			/* define type */
} php_oci_define; /* }}} */

typedef struct { /* php_oci_statement {{{ */
	int					 id;
	int					 parent_stmtid;			/* parent statement id */
	php_oci_connection	*connection;			/* parent connection handle */
	sword				 errcode;				/* last errcode*/
	OCIError			*err;					/* private error handle */
	OCIStmt				*stmt;					/* statement handle */
	char				*last_query;			/* last query issued. also used to determine if this is a statement or a refcursor recieved from Oracle */
	long				 last_query_len;		/* last query length */
	HashTable			*columns;				/* hash containing all the result columns */
	HashTable			*binds;					/* binds hash */
	HashTable			*defines;				/* defines hash */
	int					 ncolumns;				/* number of columns in the result */
	unsigned			 executed:1;			/* statement executed flag */
	unsigned			 has_data:1;			/* statement has more data flag */
	unsigned			 has_descr:1;			/* statement has at least one descriptor or cursor column */
	ub2					 stmttype;				/* statement type */
} php_oci_statement; /* }}} */

typedef struct { /* php_oci_bind {{{ */
	OCIBind				*bind;					/* bind handle */
	zval				*zval;					/* value */
	dvoid				*descriptor;			/* used for binding of LOBS etc */
	OCIStmt				*statement;				/* used for binding REFCURSORs */
	php_oci_statement	*parent_statement;		/* pointer to the parent statement */
	ub2 type;						/* bind type */
	struct {
		void	*elements;
		sb2		*indicators;
		ub2		*element_lengths;
		ub4		 current_length;
		ub4		 old_length;
		ub4		 max_length;
		long	 type;
	} array;
	sb2					 indicator;				/* -1 means NULL */
	ub2					 retcode;
	ub4					 dummy_len;				/* a dummy var to store alenpp value in bind OUT callback */
} php_oci_bind; /* }}} */

typedef struct { /* php_oci_out_column {{{ */
	php_oci_statement	*statement;				/* statement handle. used when fetching REFCURSORS */
	php_oci_statement	*nested_statement;		/* statement handle. used when fetching REFCURSORS */
	OCIDefine			*oci_define;			/* define handle */
	char				*name;					/* column name */
	ub4					 name_len;				/* column name length */
	ub2					 data_type;				/* column data type */
	ub2					 data_size;				/* data size */
	ub4					 storage_size4;			/* size used when allocating buffers */
	sb2					 indicator;
	ub2					 retcode;				/* code returned when fetching this particular column */
	ub2					 retlen;
	ub4					 retlen4;
	ub2					 is_descr;				/* column contains a descriptor */
	ub2					 is_cursor;				/* column contains a cursor */
	int					 stmtid;				/* statement id for cursors */
	int					 descid;				/* descriptor id for descriptors */
	void				*data;
	php_oci_define		*define;				/* define handle */
	int					 piecewise;				/* column is fetched piece-by-piece */
	ub4					 cb_retlen;
	sb1					 scale;					/* column scale */
	sb2					 precision;				/* column precision */
	ub1					 charset_form;			/* charset form, required for NCLOBs */
	ub2					 charset_id;			/* charset ID */
} php_oci_out_column; /* }}} */

/* {{{ macros */

#define PHP_OCI_CALL(func, params) \
	do { \
		if (OCI_G(debug_mode)) { \
			php_printf ("OCI8 DEBUG: " #func " at (%s:%d) \n", __FILE__, __LINE__); \
		} \
		OCI_G(in_call) = 1; \
		func params; \
		OCI_G(in_call) = 0; \
	} while (0)

#define PHP_OCI_CALL_RETURN(__retval, func, params) \
	do { \
		if (OCI_G(debug_mode)) { \
			php_printf ("OCI8 DEBUG: " #func " at (%s:%d) \n", __FILE__, __LINE__); \
		} \
		OCI_G(in_call) = 1; \
		__retval = func params; \
		OCI_G(in_call) = 0; \
	} while (0)

/* Check for errors that indicate the connection to the DB is no
 * longer valid.  If it isn't, then the PHP connection is marked to be
 * reopened by the next PHP OCI8 connect command.  This is most useful
 * for persistent connections.	The error number list is not
 * exclusive.  The error number comparisons and the
 * OCI_ATTR_SERVER_STATUS check are done for maximum cross-version
 * compatibility. In the far future, only the attribute check will be
 * needed.
 */
#define PHP_OCI_HANDLE_ERROR(connection, errcode) \
	do {										  \
		switch (errcode) {						  \
			case  1013:							  \
				zend_bailout();					  \
				break;							  \
			case	22:							  \
			case	28:							  \
			case   378:							  \
			case   602:							  \
			case   603:							  \
			case   604:							  \
			case   609:							  \
			case  1012:							  \
			case  1033:							  \
			case  1041:							  \
			case  1043:							  \
			case  1089:							  \
			case  1090:							  \
			case  1092:							  \
			case  3113:							  \
			case  3114:							  \
			case  3122:							  \
			case  3135:							  \
			case 12153:							  \
			case 27146:							  \
			case 28511:							  \
				(connection)->is_open = 0;		  \
				break;							  \
			default:										\
			{												\
				ub4 serverStatus = OCI_SERVER_NORMAL;		\
				PHP_OCI_CALL(OCIAttrGet, ((dvoid *)(connection)->server, OCI_HTYPE_SERVER, (dvoid *)&serverStatus, \
										  (ub4 *)0, OCI_ATTR_SERVER_STATUS, (connection)->err)); \
				if (serverStatus != OCI_SERVER_NORMAL) {	\
					(connection)->is_open = 0;				\
				}											\
			}												\
			break;											\
		}													\
	} while (0)

#define PHP_OCI_REGISTER_RESOURCE(resource, le_resource) \
	do { \
		resource->id = ZEND_REGISTER_RESOURCE(NULL, resource, le_resource); \
	} while (0)

#define PHP_OCI_ZVAL_TO_CONNECTION(zval, connection) \
	ZEND_FETCH_RESOURCE2(connection, php_oci_connection *, &zval, -1, "oci8 connection", le_connection, le_pconnection)

#define PHP_OCI_ZVAL_TO_STATEMENT(zval, statement) \
	ZEND_FETCH_RESOURCE(statement, php_oci_statement *, &zval, -1, "oci8 statement", le_statement)

#define PHP_OCI_ZVAL_TO_DESCRIPTOR(zval, descriptor) \
	ZEND_FETCH_RESOURCE(descriptor, php_oci_descriptor *, &zval, -1, "oci8 descriptor", le_descriptor)

#define PHP_OCI_ZVAL_TO_COLLECTION(zval, collection) \
	ZEND_FETCH_RESOURCE(collection, php_oci_collection *, &zval, -1, "oci8 collection", le_collection)

#define PHP_OCI_FETCH_RESOURCE_EX(zval, var, type, name, resource_type)						 \
	do { \
		var = (type) zend_fetch_resource(&zval TSRMLS_CC, -1, name, NULL, 1, resource_type); \
		if (!var) {																			 \
			return 1;																		 \
		} \
	} while (0)

#define PHP_OCI_ZVAL_TO_CONNECTION_EX(zval, connection) \
	PHP_OCI_FETCH_RESOURCE_EX(zval, connection, php_oci_connection *, "oci8 connection", le_connection)

#define PHP_OCI_ZVAL_TO_STATEMENT_EX(zval, statement) \
	PHP_OCI_FETCH_RESOURCE_EX(zval, statement, php_oci_statement *, "oci8 statement", le_statement)

#define PHP_OCI_ZVAL_TO_DESCRIPTOR_EX(zval, descriptor) \
	PHP_OCI_FETCH_RESOURCE_EX(zval, descriptor, php_oci_descriptor *, "oci8 descriptor", le_descriptor)

#define PHP_OCI_ZVAL_TO_COLLECTION_EX(zval, collection) \
	PHP_OCI_FETCH_RESOURCE_EX(zval, collection, php_oci_collection *, "oci8 collection", le_collection)

/* }}} */

/* PROTOS */

/* main prototypes {{{ */

void php_oci_column_hash_dtor (void *data);
void php_oci_define_hash_dtor (void *data);
void php_oci_bind_hash_dtor (void *data);
void php_oci_descriptor_flush_hash_dtor (void *data);

void php_oci_connection_descriptors_free(php_oci_connection *connection TSRMLS_DC);

sb4 php_oci_error (OCIError *, sword TSRMLS_DC);
sb4 php_oci_fetch_errmsg(OCIError *, text ** TSRMLS_DC);
int php_oci_fetch_sqltext_offset(php_oci_statement *, text **, ub2 * TSRMLS_DC);

void php_oci_do_connect (INTERNAL_FUNCTION_PARAMETERS, int , int);
php_oci_connection *php_oci_do_connect_ex(char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, char *dbname, int dbname_len, char *charset, long session_mode, int persistent, int exclusive TSRMLS_DC);

int php_oci_connection_rollback(php_oci_connection * TSRMLS_DC);
int php_oci_connection_commit(php_oci_connection * TSRMLS_DC);
int php_oci_connection_release(php_oci_connection *connection TSRMLS_DC);

int php_oci_password_change(php_oci_connection *, char *, int, char *, int, char *, int TSRMLS_DC);
void php_oci_client_get_version(char ** TSRMLS_DC);
int php_oci_server_get_version(php_oci_connection *, char ** TSRMLS_DC);

void php_oci_fetch_row(INTERNAL_FUNCTION_PARAMETERS, int, int);
int php_oci_column_to_zval(php_oci_out_column *, zval *, int TSRMLS_DC);

/* }}} */

/* lob related prototypes {{{ */

php_oci_descriptor * php_oci_lob_create (php_oci_connection *, long TSRMLS_DC);
int php_oci_lob_get_length (php_oci_descriptor *, ub4 * TSRMLS_DC);
int php_oci_lob_read (php_oci_descriptor *, long, long, char **, ub4 * TSRMLS_DC);
int php_oci_lob_write (php_oci_descriptor *, ub4, char *, int, ub4 * TSRMLS_DC);
int php_oci_lob_flush (php_oci_descriptor *, long TSRMLS_DC);
int php_oci_lob_set_buffering (php_oci_descriptor *, int TSRMLS_DC);
int php_oci_lob_get_buffering (php_oci_descriptor *);
int php_oci_lob_copy (php_oci_descriptor *, php_oci_descriptor *, long TSRMLS_DC);
int php_oci_lob_close (php_oci_descriptor * TSRMLS_DC);
int php_oci_temp_lob_close (php_oci_descriptor * TSRMLS_DC);
int php_oci_lob_write_tmp (php_oci_descriptor *, long, char *, int TSRMLS_DC);
void php_oci_lob_free(php_oci_descriptor * TSRMLS_DC);
int php_oci_lob_import(php_oci_descriptor *descriptor, char * TSRMLS_DC);
int php_oci_lob_append (php_oci_descriptor *, php_oci_descriptor * TSRMLS_DC);
int php_oci_lob_truncate (php_oci_descriptor *, long TSRMLS_DC);
int php_oci_lob_erase (php_oci_descriptor *, long, ub4, ub4 * TSRMLS_DC);
int php_oci_lob_is_equal (php_oci_descriptor *, php_oci_descriptor *, boolean * TSRMLS_DC);
#if defined(HAVE_OCI_LOB_READ2)
sb4 php_oci_lob_callback (dvoid *ctxp, CONST dvoid *bufxp, oraub8 len, ub1 piece, dvoid **changed_bufpp, oraub8 *changed_lenp);
#else
sb4 php_oci_lob_callback (dvoid *ctxp, CONST dvoid *bufxp, ub4 len, ub1 piece);
#endif
/* }}} */

/* collection related prototypes {{{ */

php_oci_collection * php_oci_collection_create(php_oci_connection *, char *, int, char *, int TSRMLS_DC);
int php_oci_collection_size(php_oci_collection *, sb4 * TSRMLS_DC);
int php_oci_collection_max(php_oci_collection *, long * TSRMLS_DC);
int php_oci_collection_trim(php_oci_collection *, long TSRMLS_DC);
int php_oci_collection_append(php_oci_collection *, char *, int TSRMLS_DC);
int php_oci_collection_element_get(php_oci_collection *, long, zval** TSRMLS_DC);
int php_oci_collection_element_set(php_oci_collection *, long, char *, int TSRMLS_DC);
int php_oci_collection_element_set_null(php_oci_collection *, long TSRMLS_DC);
int php_oci_collection_element_set_date(php_oci_collection *, long, char *, int TSRMLS_DC);
int php_oci_collection_element_set_number(php_oci_collection *, long, char *, int TSRMLS_DC);
int php_oci_collection_element_set_string(php_oci_collection *, long, char *, int TSRMLS_DC);
int php_oci_collection_assign(php_oci_collection *, php_oci_collection * TSRMLS_DC);
void php_oci_collection_close(php_oci_collection * TSRMLS_DC);
int php_oci_collection_append_null(php_oci_collection * TSRMLS_DC);
int php_oci_collection_append_date(php_oci_collection *, char *, int TSRMLS_DC);
int php_oci_collection_append_number(php_oci_collection *, char *, int TSRMLS_DC);
int php_oci_collection_append_string(php_oci_collection *, char *, int TSRMLS_DC);


/* }}} */

/* statement related prototypes {{{ */

php_oci_statement * php_oci_statement_create (php_oci_connection *, char *, int TSRMLS_DC);
int php_oci_statement_set_prefetch (php_oci_statement *, long TSRMLS_DC);
int php_oci_statement_fetch (php_oci_statement *, ub4 TSRMLS_DC);
php_oci_out_column * php_oci_statement_get_column (php_oci_statement *, long, char *, int TSRMLS_DC);
int php_oci_statement_execute (php_oci_statement *, ub4 TSRMLS_DC);
int php_oci_statement_cancel (php_oci_statement * TSRMLS_DC);
void php_oci_statement_free (php_oci_statement * TSRMLS_DC);
int php_oci_bind_pre_exec(void *data, void *result TSRMLS_DC);
int php_oci_bind_post_exec(void *data TSRMLS_DC);
int php_oci_bind_by_name(php_oci_statement *, char *, int, zval*, long, ub2 TSRMLS_DC);
sb4 php_oci_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
sb4 php_oci_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);
php_oci_out_column *php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAMETERS, int need_data);
int php_oci_cleanup_pre_fetch(void *data TSRMLS_DC);

int php_oci_statement_get_type(php_oci_statement *, ub2 * TSRMLS_DC);
int php_oci_statement_get_numrows(php_oci_statement *, ub4 * TSRMLS_DC);
int php_oci_bind_array_by_name(php_oci_statement *statement, char *name, int name_len, zval* var, long max_table_length, long maxlength, long type TSRMLS_DC);
php_oci_bind *php_oci_bind_array_helper_number(zval* var, long max_table_length TSRMLS_DC);
php_oci_bind *php_oci_bind_array_helper_double(zval* var, long max_table_length TSRMLS_DC);
php_oci_bind *php_oci_bind_array_helper_string(zval* var, long max_table_length, long maxlength TSRMLS_DC);
php_oci_bind *php_oci_bind_array_helper_date(zval* var, long max_table_length, php_oci_connection *connection TSRMLS_DC);

/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(oci) /* {{{ */
	sword		 errcode;						/* global last error code (used when connect fails, for example) */
	OCIError	*err;							/* global error handle */

	zend_bool	 debug_mode;					/* debug mode flag */

	long		 max_persistent;				/* maximum number of persistent connections per process */
	long		 num_persistent;				/* number of existing persistent connections */
	long		 num_links;						/* non-persistent + persistent connections */
	long		 num_statements;				/* number of statements open */
	long		 ping_interval;					/* time interval between pings */
	long		 persistent_timeout;			/* time period after which idle persistent connection is considered expired */
	long		 statement_cache_size;			/* statement cache size. used with 9i+ clients only*/
	long		 default_prefetch;				/* default prefetch setting */
	zend_bool	 privileged_connect;			/* privileged connect flag (On/Off) */
	zend_bool	 old_oci_close_semantics;		/* old_oci_close_semantics flag (to determine the way oci_close() should behave) */

	int			 shutdown;						/* in shutdown flag */

	OCIEnv		*env;							/* global environment handle */

	zend_bool	 in_call;
	char		*connection_class;
	zend_bool	 events;
	char		*edition;
ZEND_END_MODULE_GLOBALS(oci) /* }}} */

#ifdef ZTS
#define OCI_G(v) TSRMG(oci_globals_id, zend_oci_globals *, v)
#else
#define OCI_G(v) (oci_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(oci)

# endif /* !PHP_OCI8_INT_H */
#else /* !HAVE_OCI8 */

# define oci8_module_ptr NULL

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
