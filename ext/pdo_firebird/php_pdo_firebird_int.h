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
  | Author: Ard Biesheuvel <abies@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_FIREBIRD_INT_H
#define PHP_PDO_FIREBIRD_INT_H

#include <ibase.h>

#if FB_API_VER >= 40

#ifndef CLOOP_EXTERN_C
#ifdef __cplusplus
#define CLOOP_EXTERN_C extern "C"
#else
#define CLOOP_EXTERN_C
#endif
#endif

struct fb_Status;
struct fb_Master;
struct fb_Util;

CLOOP_EXTERN_C struct fb_Master* ISC_EXPORT fb_get_master_interface(void);

#define fb_Status_VERSION 3

#define fb_Status_STATE_WARNINGS ((unsigned) (0x1))
#define fb_Status_STATE_ERRORS ((unsigned) (0x2))
#define fb_Status_RESULT_ERROR ((int) (-1))
#define fb_Status_RESULT_OK ((int) (0))
#define fb_Status_RESULT_NO_DATA ((int) (1))
#define fb_Status_RESULT_SEGMENT ((int) (2))

struct fb_StatusVTable
{
	void* cloopDummy[1];
	uintptr_t version;
	void (*dispose)(struct fb_Status* self);
	void (*init)(struct fb_Status* self);
	unsigned (*getState)(const struct fb_Status* self);
	void (*setErrors2)(struct fb_Status* self, unsigned length, const intptr_t* value);
	void (*setWarnings2)(struct fb_Status* self, unsigned length, const intptr_t* value);
	void (*setErrors)(struct fb_Status* self, const intptr_t* value);
	void (*setWarnings)(struct fb_Status* self, const intptr_t* value);
	const intptr_t* (*getErrors)(const struct fb_Status* self);
	const intptr_t* (*getWarnings)(const struct fb_Status* self);
	struct fb_Status* (*clone)(const struct fb_Status* self);
};

struct fb_Status
{
	void* cloopDummy[1];
	struct fb_StatusVTable* vtable;
};

#define fb_Master_VERSION 2

struct fb_MasterVTable
{
	void* cloopDummy[1];
	uintptr_t version;
	struct fb_Status* (*getStatus)(struct fb_Master* self);
	/* stubs */
	void* getDispatcher;
	void* getPluginManager;
	void* getTimerControl;
	void* getDtc;
	void* registerAttachment;
	void* registerTransaction;
	void* getMetadataBuilder;
	/*
	struct fb_Status* (*getStatus)(struct fb_Master* self);
	struct fb_Provider* (*getDispatcher)(struct fb_Master* self);
	struct fb_PluginManager* (*getPluginManager)(struct fb_Master* self);
	struct fb_TimerControl* (*getTimerControl)(struct fb_Master* self);
	struct fb_Dtc* (*getDtc)(struct fb_Master* self);
	struct fb_Attachment* (*registerAttachment)(struct fb_Master* self, struct fb_Provider* provider, struct fb_Attachment* attachment);
	struct fb_Transaction* (*registerTransaction)(struct fb_Master* self, struct fb_Attachment* attachment, struct fb_Transaction* transaction);
	struct fb_MetadataBuilder* (*getMetadataBuilder)(struct fb_Master* self, struct fb_Status* status, unsigned fieldCount);	
	*/
	int (*serverMode)(struct fb_Master* self, int mode);
	struct fb_Util* (*getUtilInterface)(struct fb_Master* self);
	void* getConfigManager; 
	/* struct fb_ConfigManager* (*getConfigManager)(struct fb_Master* self); */
	FB_BOOLEAN (*getProcessExiting)(struct fb_Master* self);
};

struct fb_Master
{
	void* cloopDummy[1];
	struct fb_MasterVTable* vtable;
};

#define fb_Util_VERSION 4

struct fb_UtilVTable
{
	void* cloopDummy[1];
	uintptr_t version;
	/* stubs */
	void* getFbVersion;
	void* loadBlob;
	void* dumpBlob;
	void* getPerfCounters;
	void* executeCreateDatabase;
	/*
	void (*getFbVersion)(struct fb_Util* self, struct fb_Status* status, struct fb_Attachment* att, struct fb_VersionCallback* callback);
	void (*loadBlob)(struct fb_Util* self, struct fb_Status* status, ISC_QUAD* blobId, struct fb_Attachment* att, struct fb_Transaction* tra, const char* file, FB_BOOLEAN txt);
	void (*dumpBlob)(struct fb_Util* self, struct fb_Status* status, ISC_QUAD* blobId, struct fb_Attachment* att, struct fb_Transaction* tra, const char* file, FB_BOOLEAN txt);
	void (*getPerfCounters)(struct fb_Util* self, struct fb_Status* status, struct fb_Attachment* att, const char* countersSet, ISC_INT64* counters);
	struct fb_Attachment* (*executeCreateDatabase)(struct fb_Util* self, struct fb_Status* status, unsigned stmtLength, const char* creatDBstatement, unsigned dialect, FB_BOOLEAN* stmtIsCreateDb);
	*/
	void (*decodeDate)(struct fb_Util* self, ISC_DATE date, unsigned* year, unsigned* month, unsigned* day);
	void (*decodeTime)(struct fb_Util* self, ISC_TIME time, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions);
	ISC_DATE (*encodeDate)(struct fb_Util* self, unsigned year, unsigned month, unsigned day);
	ISC_TIME (*encodeTime)(struct fb_Util* self, unsigned hours, unsigned minutes, unsigned seconds, unsigned fractions);
	unsigned (*formatStatus)(struct fb_Util* self, char* buffer, unsigned bufferSize, struct fb_Status* status);
	unsigned (*getClientVersion)(struct fb_Util* self);
	/*
	struct fb_XpbBuilder* (*getXpbBuilder)(struct fb_Util* self, struct fb_Status* status, unsigned kind, const unsigned char* buf, unsigned len);
	unsigned (*setOffsets)(struct fb_Util* self, struct fb_Status* status, struct fb_MessageMetadata* metadata, struct fb_OffsetsCallback* callback);
	struct fb_DecFloat16* (*getDecFloat16)(struct fb_Util* self, struct fb_Status* status);
	struct fb_DecFloat34* (*getDecFloat34)(struct fb_Util* self, struct fb_Status* status);
	*/
	void* getXpbBuilder;
	void* setOffsets;
	void* getDecFloat16;
	void* getDecFloat34;
	void (*decodeTimeTz)(struct fb_Util* self, struct fb_Status* status, const ISC_TIME_TZ* timeTz, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions, unsigned timeZoneBufferLength, char* timeZoneBuffer);
	void (*decodeTimeStampTz)(struct fb_Util* self, struct fb_Status* status, const ISC_TIMESTAMP_TZ* timeStampTz, unsigned* year, unsigned* month, unsigned* day, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions, unsigned timeZoneBufferLength, char* timeZoneBuffer);
	void (*encodeTimeTz)(struct fb_Util* self, struct fb_Status* status, ISC_TIME_TZ* timeTz, unsigned hours, unsigned minutes, unsigned seconds, unsigned fractions, const char* timeZone);
	void (*encodeTimeStampTz)(struct fb_Util* self, struct fb_Status* status, ISC_TIMESTAMP_TZ* timeStampTz, unsigned year, unsigned month, unsigned day, unsigned hours, unsigned minutes, unsigned seconds, unsigned fractions, const char* timeZone);
	/* struct fb_Int128* (*getInt128)(struct fb_Util* self, struct fb_Status* status); */
	void* getInt128;
	void (*decodeTimeTzEx)(struct fb_Util* self, struct fb_Status* status, const ISC_TIME_TZ_EX* timeTz, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions, unsigned timeZoneBufferLength, char* timeZoneBuffer);
	void (*decodeTimeStampTzEx)(struct fb_Util* self, struct fb_Status* status, const ISC_TIMESTAMP_TZ_EX* timeStampTz, unsigned* year, unsigned* month, unsigned* day, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions, unsigned timeZoneBufferLength, char* timeZoneBuffer);
};

struct fb_Util
{
	void* cloopDummy[1];
	struct fb_UtilVTable* vtable;
};

#endif

#ifdef SQLDA_VERSION
#define PDO_FB_SQLDA_VERSION SQLDA_VERSION
#else
#define PDO_FB_SQLDA_VERSION 1
#endif

#define PDO_FB_DIALECT 3

#define PDO_FB_DEF_DATE_FMT "%Y-%m-%d"
#define PDO_FB_DEF_TIME_FMT "%H:%M:%S"
#define PDO_FB_DEF_TIMESTAMP_FMT PDO_FB_DEF_DATE_FMT " " PDO_FB_DEF_TIME_FMT

#define SHORT_MAX (1 << (8*sizeof(short)-1))

#if SIZEOF_ZEND_LONG == 8 && !defined(PHP_WIN32)
# define LL_LIT(lit) lit ## L
#else
# define LL_LIT(lit) lit ## LL
#endif
#define LL_MASK "ll"

/* Firebird API has a couple of missing const decls in its API */
#define const_cast(s) ((char*)(s))

#ifdef PHP_WIN32
typedef void (__stdcall *info_func_t)(char*);
#else
typedef void (*info_func_t)(char*);
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
# define PDO_FIREBIRD_HANDLE_INITIALIZER 0U
#else
# define PDO_FIREBIRD_HANDLE_INITIALIZER NULL
#endif

typedef struct {
	int sqlcode;
	char *errmsg;
	size_t errmsg_length;
} pdo_firebird_error_info;

typedef struct {
	/* the result of the last API call */
	ISC_STATUS isc_status[20];

	/* the connection handle */
	isc_db_handle db;

	/* the transaction handle */
	isc_tr_handle tr;
	bool in_manually_txn;
	bool is_writable_txn;
	zend_ulong txn_isolation_level;

	/* date and time format strings, can be set by the set_attribute method */
	char *date_format;
	char *time_format;
	char *timestamp_format;

	unsigned sql_dialect:2;

	/* prepend table names on column names in fetch */
	unsigned fetch_table_names:1;

	unsigned _reserved:29;

	pdo_firebird_error_info einfo;
} pdo_firebird_db_handle;


typedef struct {
	/* the link that owns this statement */
	pdo_firebird_db_handle *H;

	/* the statement handle */
	isc_stmt_handle stmt;

	/* the name of the cursor (if it has one) */
	char name[32];

	/* the type of statement that was issued */
	char statement_type:8;

	/* whether EOF was reached for this statement */
	unsigned exhausted:1;

	/* successful isc_dsql_execute opens a cursor */
	unsigned cursor_open:1;

	unsigned _reserved:22;

	/* the named params that were converted to ?'s by the driver */
	HashTable *named_params;

	/* the input SQLDA */
	XSQLDA *in_sqlda;

	/* the output SQLDA */
	XSQLDA out_sqlda; /* last member */
} pdo_firebird_stmt;

extern const pdo_driver_t pdo_firebird_driver;

extern const struct pdo_stmt_methods firebird_stmt_methods;

extern void php_firebird_set_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *state, const size_t state_len,
	const char *msg, const size_t msg_len);
#define php_firebird_error(d) php_firebird_set_error(d, NULL, NULL, 0, NULL, 0)
#define php_firebird_error_stmt(s) php_firebird_set_error(s->dbh, s, NULL, 0, NULL, 0)
#define php_firebird_error_with_info(d,e,el,m,ml) php_firebird_set_error(d, NULL, e, el, m, ml)
#define php_firebird_error_stmt_with_info(s,e,el,m,ml) php_firebird_set_error(s->dbh, s, e, el, m, ml)

extern bool php_firebird_commit_transaction(pdo_dbh_t *dbh, bool retain);

enum {
	PDO_FB_ATTR_DATE_FORMAT = PDO_ATTR_DRIVER_SPECIFIC,
	PDO_FB_ATTR_TIME_FORMAT,
	PDO_FB_ATTR_TIMESTAMP_FORMAT,

	/*
	 * transaction isolation level
	 * firebird does not have a level equivalent to read uncommited.
	 */
	PDO_FB_TRANSACTION_ISOLATION_LEVEL,
	PDO_FB_READ_COMMITTED,
	PDO_FB_REPEATABLE_READ,
	PDO_FB_SERIALIZABLE,

	/* transaction access mode */
	PDO_FB_WRITABLE_TRANSACTION,
};

#endif	/* PHP_PDO_FIREBIRD_INT_H */
