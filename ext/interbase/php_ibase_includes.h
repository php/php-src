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
   | Authors: Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Andrew Avdeev <andy@simgts.mv.ru>                           |
   |          Ard Biesheuvel <a.k.biesheuvel@ewi.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_IBASE_INCLUDES_H
#define PHP_IBASE_INCLUDES_H

#include <ibase.h>

#ifndef SQLDA_CURRENT_VERSION
#define SQLDA_CURRENT_VERSION SQLDA_VERSION1
#endif

#ifndef METADATALENGTH
#define METADATALENGTH 68
#endif

#define RESET_ERRMSG do { IBG(errmsg)[0] = '\0'; IBG(sql_code) = 0; } while (0)

#define IB_STATUS (IBG(status))

#ifdef IBASE_DEBUG
#define IBDEBUG(a) php_printf("::: %s (%d)\n", a, __LINE__);
#endif

#ifndef IBDEBUG
#define IBDEBUG(a)
#endif

extern int le_link, le_plink, le_trans;

#define LE_LINK "Firebird/InterBase link"
#define LE_PLINK "Firebird/InterBase persistent link"
#define LE_TRANS "Firebird/InterBase transaction"

#define IBASE_MSGSIZE 512
#define MAX_ERRMSG (IBASE_MSGSIZE*2)

#define IB_DEF_DATE_FMT "%Y-%m-%d"
#define IB_DEF_TIME_FMT "%H:%M:%S"

/* this value should never be > USHRT_MAX */
#define IBASE_BLOB_SEG 4096

ZEND_BEGIN_MODULE_GLOBALS(ibase)
	ISC_STATUS status[20];
	long default_link;
	long num_links, num_persistent;
	char errmsg[MAX_ERRMSG];
	long sql_code;
ZEND_END_MODULE_GLOBALS(ibase)

ZEND_EXTERN_MODULE_GLOBALS(ibase)

typedef struct {
	isc_db_handle handle;
	struct tr_list *tr_list;
	unsigned short dialect;
	struct event *event_head;
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
	isc_blob_handle bl_handle;
	unsigned short type;
	ISC_QUAD bl_qd;
} ibase_blob;

typedef struct event {
	ibase_db_link *link;
	long link_res_id;
	ISC_LONG event_id;
	unsigned short event_count;
	char **events;
	char *event_buffer, *result_buffer;
	zval callback;
	void *thread_ctx;
	struct event *event_next;
	enum event_state { NEW, ACTIVE, DEAD } state;
} ibase_event;

enum php_interbase_option {
	PHP_IBASE_DEFAULT 			= 0,
	PHP_IBASE_CREATE            = 0,
	/* fetch flags */
	PHP_IBASE_FETCH_BLOBS		= 1,
	PHP_IBASE_FETCH_ARRAYS      = 2,
	PHP_IBASE_UNIXTIME 			= 4,
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
#else
#endif

#ifdef ZTS
# define IBG(v) ZEND_TSRMG(ibase_globals_id, zend_ibase_globals *, v)
# ifdef COMPILE_DL_INTERBASE
ZEND_TSRMLS_CACHE_EXTERN();
# endif
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
typedef void (__stdcall *info_func_t)(char*);
#else
#define LL_MASK "ll"
#define LL_LIT(lit) lit ## ll
typedef void (*info_func_t)(char*);
#endif

void _php_ibase_error(void);
void _php_ibase_module_error(char *, ...)
	PHP_ATTRIBUTE_FORMAT(printf,1,PHP_ATTR_FMT_OFFSET +2);

/* determine if a resource is a link or transaction handle */
#define PHP_IBASE_LINK_TRANS(zv, lh, th)													\
	do { if (!zv) {																			\
			lh = (ibase_db_link *)zend_fetch_resource2(IBG(default_link),				\
				"InterBase link", le_link, le_plink); }										\
		else																				\
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, zv, &lh, &th);		\
		if (SUCCESS != _php_ibase_def_trans(lh, &th)) { RETURN_FALSE; }			\
	} while (0)

int _php_ibase_def_trans(ibase_db_link *ib_link, ibase_trans **trans);
void _php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAMETERS, zval *link_id,
	ibase_db_link **ib_link, ibase_trans **trans);

/* provided by ibase_query.c */
void php_ibase_query_minit(INIT_FUNC_ARGS);

/* provided by ibase_blobs.c */
void php_ibase_blobs_minit(INIT_FUNC_ARGS);
int _php_ibase_string_to_quad(char const *id, ISC_QUAD *qd);
char *_php_ibase_quad_to_string(ISC_QUAD const qd);
int _php_ibase_blob_get(zval *return_value, ibase_blob *ib_blob, unsigned long max_len);
int _php_ibase_blob_add(zval *string_arg, ibase_blob *ib_blob);

/* provided by ibase_events.c */
void php_ibase_events_minit(INIT_FUNC_ARGS);
void _php_ibase_free_event(ibase_event *event);

/* provided by ibase_service.c */
void php_ibase_service_minit(INIT_FUNC_ARGS);

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif /* PHP_IBASE_INCLUDES_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
