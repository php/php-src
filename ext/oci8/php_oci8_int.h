/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
   |                Andi Gutmans <andi@php.net>                           |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

#if HAVE_OCI8
# ifndef PHP_OCI8_INT_H
#  define PHP_OCI8_INT_H

/* {{{ misc defines */
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

#ifdef HAVE_OCI8_DTRACE
#include "oci8_dtrace_gen.h"
#endif

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

/* {{{ constants */
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

#ifdef OCI_ERROR_MAXMSG_SIZE2
/* Bigger size is defined from 11.2.0.3 onwards */
#define PHP_OCI_ERRBUF_LEN OCI_ERROR_MAXMSG_SIZE2
#else
#define PHP_OCI_ERRBUF_LEN OCI_ERROR_MAXMSG_SIZE
#endif

/* The mode parameter for oci_connect() is overloaded and accepts both
 * privilege and external authentication flags OR'd together.
 * PHP_OCI_CRED_EXT must be distinct from the OCI_xxx privilege
 * values.
 */
#define PHP_OCI_CRED_EXT                    (1<<31)
#if ((PHP_OCI_CRED_EXT == OCI_DEFAULT) || (PHP_OCI_CRED_EXT & (OCI_SYSOPER | OCI_SYSDBA)))
#error Invalid value for PHP_OCI_CRED_EXT
#endif

#define PHP_OCI_IMPRES_UNKNOWN			0
#define PHP_OCI_IMPRES_NO_CHILDREN		1
#define PHP_OCI_IMPRES_HAS_CHILDREN		2
#define PHP_OCI_IMPRES_IS_CHILD			3

/*
 * Name passed to Oracle for tracing.  Note some DB views only show
 * the first nine characters of the driver name.
 */
#define PHP_OCI8_DRIVER_NAME     "PHP OCI8 : " PHP_OCI8_VERSION

/* }}} */

/* {{{ php_oci_spool */
typedef struct {
	zend_resource *id;					/* resource id */
	OCIEnv		 *env;					/* env of this session pool */
	OCIError	 *err;					/* pool's error handle	*/
	OCISPool	 *poolh;				/* pool handle */
	void		 *poolname;				/* session pool name */
	unsigned int  poolname_len;			/* length of session pool name */
	zend_string	 *spool_hash_key;		/* Hash key for session pool in plist */
} php_oci_spool;
/* }}} */

/* {{{ php_oci_connection */
typedef struct {
	zend_resource  *id;							/* resource ID */
	OCIEnv		   *env;						/* private env handle */
	ub2				charset;					/* charset ID */
	OCIServer	   *server;						/* private server handle */
	OCISvcCtx	   *svc;						/* private service context handle */
	OCISession	   *session;					/* private session handle */
	OCIAuthInfo	   *authinfo;					/* Cached authinfo handle for OCISessionGet */
	OCIError	   *err;						/* private error handle */
	php_oci_spool  *private_spool;				/* private session pool (for persistent) */
	sb4				errcode;					/* last ORA- error number */

	HashTable	   *descriptors;				/* descriptors hash, used to flush all the LOBs using this connection on commit */
	zend_ulong		descriptor_count;			/* used to index the descriptors hash table.  Not an accurate count */
	unsigned		is_open:1;					/* hels to determine if the connection is dead or not */
	unsigned		is_attached:1;				/* hels to determine if we should detach from the server when closing/freeing the connection */
	unsigned		is_persistent:1;			/* self-descriptive */
	unsigned		used_this_request:1;		/* helps to determine if we should reset connection's next ping time and check its timeout */
	unsigned		rb_on_disconnect:1;			/* helps to determine if we should rollback this connection on close/shutdown */
	unsigned		passwd_changed:1;			/* helps determine if a persistent connection hash should be invalidated after a password change */
	unsigned		is_stub:1;					/* flag to keep track whether the connection structure has a real OCI connection associated */
	unsigned		using_spool:1;				/* Is this connection from session pool? */
	time_t			idle_expiry;				/* time when the connection will be considered as expired */
	time_t		   *next_pingp;					/* (pointer to) time of the next ping */
	zend_string	   *hash_key;					/* hashed details of the connection */
#ifdef HAVE_OCI8_DTRACE
	char		   *client_id;					/* The oci_set_client_identifier() value */
#endif

	zval		    taf_callback;				/* The Oracle TAF callback function in the userspace */
} php_oci_connection;
/* }}} */

/* {{{ php_oci_descriptor */
typedef struct {
	zend_resource		*id;
	zend_ulong				 index;		            /* descriptors hash table index */
	php_oci_connection	*connection;			/* parent connection handle */
	dvoid				*descriptor;			/* OCI descriptor handle */
	ub4					 type;					/* descriptor type (FILE/LOB) */
	ub4					 lob_current_position;	/* LOB internal pointer */
	int					 lob_size;				/* cached LOB size. -1 = Lob wasn't initialized yet */
	int					 buffering;				/* cached buffering flag. 0 - off, 1 - on, 2 - on and buffer was used */
	ub4					 chunk_size;			/* chunk size of the LOB. 0 - unknown */
	ub1					 charset_form;			/* charset form, required for NCLOBs */
	ub2					 charset_id;			/* charset ID */
	unsigned			 is_open:1;				/* helps to determine if lob is open or not */
} php_oci_descriptor;
/* }}} */

/* {{{ php_oci_lob_ctx */
typedef struct {
	char			   **lob_data;				/* address of pointer to LOB data */
	ub4					*lob_len;				/* address of LOB length variable (bytes) */
	ub4					 alloc_len;
} php_oci_lob_ctx;
/* }}} */

/* {{{ php_oci_collection */
typedef struct {
	zend_resource		*id;
	php_oci_connection	*connection;			/* parent connection handle */
	OCIType				*tdo;					/* collection's type handle */
	OCITypeCode			 coll_typecode;			/* collection's typecode handle */
	OCIRef				*elem_ref;				/* element's reference handle */
	OCIType				*element_type;			/* element's type handle */
	OCITypeCode			 element_typecode;		/* element's typecode handle */
	OCIColl				*collection;			/* collection handle */
} php_oci_collection;
/* }}} */

/* {{{ php_oci_define */
typedef struct {
	zval		 val;			/* zval used in define */
	text		*name;			/* placeholder's name */
	ub4			 name_len;		/* placeholder's name length */
	ub4			 type;			/* define type */
} php_oci_define;
/* }}} */

/* {{{ php_oci_statement */
typedef struct {
	zend_resource		*id;
	zend_resource	 	*parent_stmtid;			/* parent statement id */
	struct php_oci_statement *impres_child_stmt;/* child of current Implicit Result Set statement handle */
	ub4                  impres_count;          /* count of remaining Implicit Result children on parent statement handle */
	php_oci_connection	*connection;			/* parent connection handle */
	sb4					 errcode;				/* last ORA- error number */
	OCIError			*err;					/* private error handle */
	OCIStmt				*stmt;					/* statement handle */
	char				*last_query;			/* last query issued. also used to determine if this is a statement or a refcursor received from Oracle */
	char                 impres_flag;           /* PHP_OCI_IMPRES_*_ */
	zend_long			 last_query_len;		/* last query length */
	HashTable			*columns;				/* hash containing all the result columns */
	HashTable			*binds;					/* binds hash */
	HashTable			*defines;				/* defines hash */
	int					 ncolumns;				/* number of columns in the result */
	unsigned			 executed:1;			/* statement executed flag */
	unsigned			 has_data:1;			/* statement has more data flag */
	unsigned			 has_descr:1;			/* statement has at least one descriptor or cursor column */
	ub2					 stmttype;				/* statement type */
	ub4                  prefetch_count;        /* current prefetch count */
} php_oci_statement;
/* }}} */

/* {{{ php_oci_bind */
typedef struct {
	OCIBind				*bind;					/* bind handle */
	zval				val;					/* value */
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
		zend_long	 type;
	} array;
	sb2					 indicator;				/* -1 means NULL */
	ub2					 retcode;
	ub4					 dummy_len;				/* a dummy var to store alenpp value in bind OUT callback */
} php_oci_bind;
/* }}} */

/* {{{ php_oci_out_column */
typedef struct {
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
	zend_resource		*stmtid;				/* statement id for cursors */
	zend_resource		*descid;				/* descriptor id for descriptors */
	void				*data;
	php_oci_define		*define;				/* define handle */
	int					 piecewise;				/* column is fetched piece-by-piece */
	ub4					 cb_retlen;
	sb1					 scale;					/* column scale */
	sb2					 precision;				/* column precision */
	ub1					 charset_form;			/* charset form, required for NCLOBs */
	ub2					 charset_id;			/* charset ID */
	ub4					 chunk_size;			/* LOB chunk size */
} php_oci_out_column;
/* }}} */

/* {{{ macros */

#define PHP_OCI_CALL(func, params)								\
	do {																\
		OCI_G(in_call) = 1;												\
		func params;													\
		OCI_G(in_call) = 0;												\
	} while (0)

#define PHP_OCI_CALL_RETURN(__retval, func, params)			\
	do {																\
		OCI_G(in_call) = 1;												\
		__retval = func params;											\
		OCI_G(in_call) = 0;												\
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
		ub4 serverStatus = OCI_SERVER_NORMAL;	  \
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
			case  3136:							  \
			case 12153:							  \
			case 12161:							  \
			case 27146:							  \
			case 28511:							  \
				(connection)->is_open = 0;		  \
				break;							  \
			default:										\
			{												\
				PHP_OCI_CALL(OCIAttrGet, ((dvoid *)(connection)->server, OCI_HTYPE_SERVER, (dvoid *)&serverStatus, \
										  (ub4 *)0, OCI_ATTR_SERVER_STATUS, (connection)->err)); \
				if (serverStatus != OCI_SERVER_NORMAL) {	\
					(connection)->is_open = 0;				\
				}											\
			}												\
			break;											\
		}													\
		php_oci_dtrace_check_connection(connection, errcode, serverStatus); \
	} while (0)

#define PHP_OCI_REGISTER_RESOURCE(resource, le_resource) \
	do { \
		resource->id = zend_register_resource(resource, le_resource); \
	} while (0)

#define PHP_OCI_ZVAL_TO_CONNECTION(zval, connection) \
	if ((connection = (php_oci_connection *)zend_fetch_resource2(Z_RES_P(zval), "oci8 connection", le_connection, le_pconnection)) == NULL) { \
		RETURN_FALSE; \
	}

#define PHP_OCI_ZVAL_TO_STATEMENT(zval, statement) \
	if ((statement = (php_oci_statement *)zend_fetch_resource(Z_RES_P(zval), "oci8 statement", le_statement)) == NULL) { \
		RETURN_FALSE; \
	}

#define PHP_OCI_ZVAL_TO_DESCRIPTOR(zval, descriptor) \
	if ((descriptor = (php_oci_descriptor *)zend_fetch_resource(Z_RES_P(zval), "oci8 descriptor", le_descriptor)) == NULL) { \
		RETURN_FALSE; \
	}

#define PHP_OCI_ZVAL_TO_COLLECTION(zval, collection) \
	if ((collection = (php_oci_collection *)zend_fetch_resource(Z_RES_P(zval), "oci8 collection", le_collection)) == NULL) { \
		RETURN_FALSE; \
	}

#define PHP_OCI_FETCH_RESOURCE_EX(zval, var, type, name, resource_type)						 \
	do { \
		var = (type) zend_fetch_resource(Z_RES_P(zval), name, resource_type);                \
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

/* {{{ main prototypes */

void php_oci_column_hash_dtor(zval *data);
void php_oci_define_hash_dtor(zval *data);
void php_oci_bind_hash_dtor(zval *data);
void php_oci_descriptor_flush_hash_dtor(zval *data);
void php_oci_connection_descriptors_free(php_oci_connection *connection);
sb4 php_oci_error(OCIError *err_p, sword status);
sb4 php_oci_fetch_errmsg(OCIError *error_handle, text *error_buf, size_t error_buf_size);
int php_oci_fetch_sqltext_offset(php_oci_statement *statement, text **sqltext, ub2 *error_offset);
void php_oci_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent, int exclusive);
php_oci_connection *php_oci_do_connect_ex(char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, char *dbname, int dbname_len, char *charset, zend_long session_mode, int persistent, int exclusive);
int php_oci_connection_rollback(php_oci_connection *connection);
int php_oci_connection_commit(php_oci_connection *connection);
int php_oci_connection_release(php_oci_connection *connection);
int php_oci_password_change(php_oci_connection *connection, char *user, int user_len, char *pass_old, int pass_old_len, char *pass_new, int pass_new_len);
void php_oci_client_get_version(char *version, size_t version_size);
int php_oci_server_get_version(php_oci_connection *connection, char *version, size_t version_size);
void php_oci_fetch_row(INTERNAL_FUNCTION_PARAMETERS, int mode, int expected_args);
int php_oci_column_to_zval(php_oci_out_column *column, zval *value, int mode);
void php_oci_dtrace_check_connection(php_oci_connection *connection, sb4 errcode, ub4 serverStatus);

/* }}} */

/* {{{ lob related prototypes */

php_oci_descriptor *php_oci_lob_create(php_oci_connection *connection, zend_long type);
int php_oci_lob_get_length(php_oci_descriptor *descriptor, ub4 *length);
int php_oci_lob_read(php_oci_descriptor *descriptor, zend_long read_length, zend_long inital_offset, char **data, ub4 *data_len);
int php_oci_lob_write(php_oci_descriptor *descriptor, ub4 offset, char *data, int data_len, ub4 *bytes_written);
int php_oci_lob_flush(php_oci_descriptor *descriptor, zend_long flush_flag);
int php_oci_lob_set_buffering(php_oci_descriptor *descriptor, int on_off);
int php_oci_lob_get_buffering(php_oci_descriptor *descriptor);
int php_oci_lob_copy(php_oci_descriptor *descriptor, php_oci_descriptor *descriptor_from, zend_long length);
int php_oci_lob_close(php_oci_descriptor *descriptor);
int php_oci_temp_lob_close(php_oci_descriptor *descriptor);
int php_oci_lob_write_tmp(php_oci_descriptor *descriptor, zend_long type, char *data, int data_len);
void php_oci_lob_free(php_oci_descriptor *descriptor);
int php_oci_lob_import(php_oci_descriptor *descriptor, char *filename);
int php_oci_lob_append(php_oci_descriptor *descriptor_dest, php_oci_descriptor *descriptor_from);
int php_oci_lob_truncate(php_oci_descriptor *descriptor, zend_long new_lob_length);
int php_oci_lob_erase(php_oci_descriptor *descriptor, zend_long offset, ub4 length, ub4 *bytes_erased);
int php_oci_lob_is_equal(php_oci_descriptor *descriptor_first, php_oci_descriptor *descriptor_second, boolean *result);
sb4 php_oci_lob_callback(dvoid *ctxp, CONST dvoid *bufxp, oraub8 len, ub1 piece, dvoid **changed_bufpp, oraub8 *changed_lenp);
/* }}} */

/* {{{ collection related prototypes */

php_oci_collection *php_oci_collection_create(php_oci_connection *connection, char *tdo, int tdo_len, char *schema, int schema_len);
int php_oci_collection_size(php_oci_collection *collection, sb4 *size);
int php_oci_collection_max(php_oci_collection *collection, zend_long *max);
int php_oci_collection_trim(php_oci_collection *collection, zend_long trim_size);
int php_oci_collection_append(php_oci_collection *collection, char *element, int element_len);
int php_oci_collection_element_get(php_oci_collection *collection, zend_long index, zval *result_element);
int php_oci_collection_element_set(php_oci_collection *collection, zend_long index, char *value, int value_len);
int php_oci_collection_element_set_null(php_oci_collection *collection, zend_long index);
int php_oci_collection_element_set_date(php_oci_collection *collection, zend_long index, char *date, int date_len);
int php_oci_collection_element_set_number(php_oci_collection *collection, zend_long index, char *number, int number_len);
int php_oci_collection_element_set_string(php_oci_collection *collection, zend_long index, char *element, int element_len);
int php_oci_collection_assign(php_oci_collection *collection_dest, php_oci_collection *collection_from);
void php_oci_collection_close(php_oci_collection *collection);
int php_oci_collection_append_null(php_oci_collection *collection);
int php_oci_collection_append_date(php_oci_collection *collection, char *date, int date_len);
int php_oci_collection_append_number(php_oci_collection *collection, char *number, int number_len);
int php_oci_collection_append_string(php_oci_collection *collection, char *element, int element_len);


/* }}} */

/* {{{ statement related prototypes */

php_oci_statement *php_oci_statement_create(php_oci_connection *connection, char *query, int query_len);
php_oci_statement *php_oci_get_implicit_resultset(php_oci_statement *statement);
int php_oci_statement_set_prefetch(php_oci_statement *statement, ub4 prefetch);
int php_oci_statement_fetch(php_oci_statement *statement, ub4 nrows);
php_oci_out_column *php_oci_statement_get_column(php_oci_statement *statement, zend_long column_index, char *column_name, int column_name_len);
int php_oci_statement_execute(php_oci_statement *statement, ub4 mode);
int php_oci_statement_cancel(php_oci_statement *statement);
void php_oci_statement_free(php_oci_statement *statement);
int php_oci_bind_pre_exec(zval *data, void *result);
int php_oci_bind_post_exec(zval *data);
int php_oci_bind_by_name(php_oci_statement *statement, char *name, size_t name_len, zval *var, zend_long maxlength, ub2 type);
sb4 php_oci_bind_in_callback(dvoid *ictxp, OCIBind *bindp, ub4 iter, ub4 index, dvoid **bufpp, ub4 *alenp, ub1 *piecep, dvoid **indpp);
sb4 php_oci_bind_out_callback(dvoid *octxp, OCIBind *bindp, ub4 iter, ub4 index, dvoid **bufpp, ub4 **alenpp, ub1 *piecep, dvoid **indpp, ub2 **rcodepp);
php_oci_out_column *php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAMETERS, int need_data);
int php_oci_cleanup_pre_fetch(zval *data);
int php_oci_statement_get_type(php_oci_statement *statement, ub2 *type);
int php_oci_statement_get_numrows(php_oci_statement *statement, ub4 *numrows);
int php_oci_bind_array_by_name(php_oci_statement *statement, char *name, size_t name_len, zval *var, zend_long max_table_length, zend_long maxlength, zend_long type);
php_oci_bind *php_oci_bind_array_helper_number(zval *var, zend_long max_table_length);
php_oci_bind *php_oci_bind_array_helper_double(zval *var, zend_long max_table_length);
php_oci_bind *php_oci_bind_array_helper_string(zval *var, zend_long max_table_length, zend_long maxlength);
php_oci_bind *php_oci_bind_array_helper_date(zval *var, zend_long max_table_length, php_oci_connection *connection);

/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(oci) /* {{{ Module globals */
	sb4			 errcode;						/* global last ORA- error number. Used when connect fails, for example */
	OCIError	*err;							/* global error handle */

	zend_long		 max_persistent;				/* maximum number of persistent connections per process */
	zend_long		 num_persistent;				/* number of existing persistent connections */
	zend_long		 num_links;						/* non-persistent + persistent connections */
	zend_long		 num_statements;				/* number of statements open */
	zend_long		 ping_interval;					/* time interval between pings */
	zend_long		 persistent_timeout;			/* time period after which idle persistent connection is considered expired */
	zend_long		 statement_cache_size;			/* statement cache size. used with 9i+ clients only*/
	zend_long		 default_prefetch;				/* default prefetch setting */
	zend_bool	 privileged_connect;			/* privileged connect flag (On/Off) */
	zend_bool	 old_oci_close_semantics;		/* old_oci_close_semantics flag (to determine the way oci_close() should behave) */

	int			 shutdown;						/* in shutdown flag */

	OCIEnv		*env;							/* global environment handle */

	zend_bool	 in_call;
	char		*connection_class;
	zend_bool	 events;
	char		*edition;
ZEND_END_MODULE_GLOBALS(oci) /* }}} */

/* {{{ transparent failover related prototypes */

int php_oci_register_taf_callback(php_oci_connection *connection, zval *callback);
int php_oci_unregister_taf_callback(php_oci_connection *connection);

/* }}} */

#ifdef ZTS
#define OCI_G(v) TSRMG(oci_globals_id, zend_oci_globals *, v)
#else
#define OCI_G(v) (oci_globals.v)
#endif

/* Allow install from PECL on PHP < 7.3 */
#ifndef GC_ADDREF
# define GC_ADDREF(p) (++GC_REFCOUNT(p))
#endif
#ifndef GC_DELREF
# define GC_DELREF(p) (GC_REFCOUNT(p)--)
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
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
