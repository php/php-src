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
   | Authors: Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Andrew Avdeev <andy@simgts.mv.ru>                           |
   |          Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef INTERBASE_H
#define INTERBASE_H

#include <ibase.h>

#ifndef SQLDA_CURRENT_VERSION
#define SQLDA_CURRENT_VERSION SQLDA_VERSION1
#endif

/* IB < 6 doesn't define these */
#ifndef SQL_DIALECT_CURRENT
#define SQL_DIALECT_CURRENT 1 /* == SQL_DIALECT_V5 */

#ifdef PHP_WIN32
typedef __int64 ISC_INT64;
typedef unsigned __int64 ISC_UINT64;
#else
typedef long long ISC_INT64;
typedef unsigned long long ISC_UINT64;
#endif /* PHP_WIN32 */
#endif /* SQL_DIALECT_CURRENT */

#define RESET_ERRMSG { IBG(errmsg)[0] = '\0'; IBG(sql_code) = 0; }

#define IB_STATUS (IBG(status))

extern int le_blob, le_link, le_plink, le_result, le_query, le_trans, 
	le_event, le_service;

#define IBASE_MSGSIZE 256
#define MAX_ERRMSG (IBASE_MSGSIZE*2)

/* this value should never be > USHRT_MAX */
#define IBASE_BLOB_SEG 4096

ZEND_BEGIN_MODULE_GLOBALS(ibase)
	ISC_STATUS status[20];
	long default_link;
	long num_links, num_persistent;
	long max_links, max_persistent;
	long allow_persistent;
	char *default_user, *default_password;
	char *timestampformat;
	char *cfg_timestampformat;
	char *dateformat;
	char *cfg_dateformat;
	char *timeformat;
	char *cfg_timeformat;
	char errmsg[MAX_ERRMSG];
	long sql_code;
ZEND_END_MODULE_GLOBALS(ibase)

ZEND_EXTERN_MODULE_GLOBALS(ibase)

typedef struct {
	isc_db_handle handle;
	struct tr_list *tr_list;
	unsigned short dialect;
} ibase_db_link;

typedef struct {
	isc_tr_handle handle;
	unsigned short link_cnt;
	unsigned long affected_rows;
	ibase_db_link *db_link[1]; /* last member */
} ibase_trans;

typedef struct tr_list {
	ibase_trans *trans;
	struct tr_list *next;
} ibase_tr_list;

typedef struct {
	ISC_ARRAY_DESC ar_desc;
	ISC_LONG ar_size; /* size of entire array in bytes */
	unsigned short el_type, el_size;
} ibase_array;

typedef struct {
	isc_blob_handle bl_handle;
	ISC_QUAD bl_qd;
	unsigned short type;
} ibase_blob;

typedef struct {
	ibase_db_link *link;
	ibase_trans *trans;
	isc_stmt_handle stmt;
	unsigned short type;
	unsigned char has_more_rows, statement_type;
	XSQLDA *out_sqlda;
	ibase_array out_array[1]; /* last member */
} ibase_result;

typedef struct {
	ibase_db_link *link;
	ibase_trans *trans;
	int result_res_id;
	isc_stmt_handle stmt;
	XSQLDA *in_sqlda, *out_sqlda;
	ibase_array *in_array, *out_array;
	unsigned short dialect;
	char statement_type;
	char *query;
	long trans_res_id;
} ibase_query;

typedef struct {
	ibase_db_link *link;
	long link_res_id;
	ISC_LONG event_id;
	unsigned short event_count;
	char **events;
	char *event_buffer, *result_buffer;
	zval *callback;
	void **thread_ctx;
} ibase_event;

typedef struct {
	isc_svc_handle handle;
	char *hostname;
	char *username;
} ibase_service;

enum php_interbase_option {
	PHP_IBASE_DEFAULT 			= 0,
	/* fetch flags */
	PHP_IBASE_FETCH_BLOBS		= 1,
	PHP_IBASE_FETCH_ARRAYS      = 2,
	PHP_IBASE_UNIXTIME 			= 4,
	/* timefmt flags */
	PHP_IBASE_TIMESTAMP 		= 1,
	PHP_IBASE_DATE 				= 2,
	PHP_IBASE_TIME 				= 4,
	/* transaction access mode */	
	PHP_IBASE_WRITE 			= 1,
	PHP_IBASE_READ 				= 2,
	/* transaction isolation level */
	PHP_IBASE_CONCURRENCY 		= 4,
	PHP_IBASE_COMMITTED 		= 8,
	  PHP_IBASE_REC_NO_VERSION 	= 32,
	  PHP_IBASE_REC_VERSION 	= 64,
	PHP_IBASE_CONSISTENCY 		= 16,
	/* transaction lock resolution */
	PHP_IBASE_WAIT 				= 128,
	PHP_IBASE_NOWAIT 			= 256
};

#ifdef ZTS
#define IBG(v) TSRMG(ibase_globals_id, zend_ibase_globals *, v)
#else
#define IBG(v) (ibase_globals.v)
#endif

#define BLOB_ID_LEN		18
#define BLOB_ID_MASK	"0x%" LL_MASK "x"

#define BLOB_INPUT		1
#define BLOB_OUTPUT		2

#ifdef PHP_WIN32
#define LL_MASK "I64"
#define LL_LIT(lit) lit ## I64
#else
#define LL_MASK "ll"
#define LL_LIT(lit) lit ## ll
#endif

int _php_ibase_string_to_quad(char const *id, ISC_QUAD *qd);
char *_php_ibase_quad_to_string(ISC_QUAD const qd);
int _php_ibase_blob_get(zval *return_value, ibase_blob *ib_blob, unsigned long max_len TSRMLS_DC);
int _php_ibase_blob_add(zval **string_arg, ibase_blob *ib_blob TSRMLS_DC);

void _php_ibase_error(TSRMLS_D);
void _php_ibase_module_error(char * TSRMLS_DC, ...) 
	PHP_ATTRIBUTE_FORMAT(printf,1,PHP_ATTR_FMT_OFFSET +2);

int _php_ibase_def_trans(ibase_db_link *ib_link, ibase_trans **trans TSRMLS_DC);
void _php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAMETERS, zval **link_id, 
	ibase_db_link **ib_link, ibase_trans **trans);

void _php_ibase_event_free(char *event_buf, char *result_buf);

#endif /* INTERBASE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
