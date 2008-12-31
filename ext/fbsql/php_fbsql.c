/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http:/*www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Frank M. Kromann <frank@kromann.info>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* TODO:
 *
 * ? Safe mode implementation
 */

/*	SB's list:
	- API for a more natural FB connect semantic
	- Connect & set session 
	- Autoreconnect when disconnected
	- Comments and cleanup

	BUGS
	- Select db with no arguments
	 - Query with everything defaulted
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#ifdef PHP_WIN32
#include <winsock2.h>
#else
#include <php_config.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#endif

#include "php_ini.h"

#define HAVE_FBSQL 1

#ifndef min
# define min(a,b) ((a)<(b)?(a):(b))
#endif

#if HAVE_FBSQL
#include "php_fbsql.h"
#include <signal.h>

static int le_result, le_link, le_plink;

struct PHPFBResult;
typedef struct PHPFBResult PHPFBResult;

struct PHPFBLink;
typedef struct PHPFBLink PHPFBLink;

/*	The PHPFBLink structure represents a fbsql link. The lion is used for
	a connection to a machine, it may be persistent and is reference counted.
	The reason for refcounting is mostly to avoid to think, it work independent of 
	any wierd and unforseen allocation deallocation order.

	The PHPFBDatabse structure implements to actual connection to a FrontBase server
	ot may be persistent is the link it is connected to is persistent, and refcounted
	for the same reasons as above.

	The PHPFBResult structure implements a result from the FrontBase server, and does all
	required buffereing from of results.

	In the PHP code the 3 above a data structures are referenced by means of integers in the
	range from 1 to som configurable maximum. You can put a limit to the number of links, databases
	and results. The integer identifications is implemented by insertion in the list, which is passed
	as an argument to all the functions, please note the list is polymorph.

	Database objects and link objects are all reused, base on the host name user name, host name database name 
	user name. So connecting twice to the same database as the same user will return the same database id.
	We use the same coding for that as fbsql does, explioiting the underlying implementation of the lists.

	Persistent objects are put in the persistent list as well, but only by name, if you connect to a persistent object
	and it is not in the list it is simply added and get a new index, and refcounted. Tricky, tricky ...
*/

/* Some functions which should be exported from FBCAccess */

void*			fbaObjectAtIndex();
void			fbaRelease();
unsigned int	fbaCount();

struct FBCAutoStartInfo {
	FBArray* infoLines;
};

struct PHPFBResult
{
	PHPFBLink*				link;				/* The link for the result, may be NULL if no link  */
	char*					fetchHandle;		/* The fetch handle, the id used by the server.   */
	FBCMetaData*			metaData;			/* The metadata describing the result */
	FBCMetaData*			ResultmetaData;		/* The metadata describing the result */
	FBCRowHandler*			rowHandler;			/* The row handler, the Frontbase structure used for accessing rows in the result */
	unsigned int			batchSize;			/* The number of row to fetch when expanding the number of rows in the row handler */
	unsigned int			rowCount;			/* The number of rows in the results set. The number of row is not in */
						/* general known when the select is done, one typically needs to fetch all the row
						   to figure out how many row you got. When the rowCount is unknown the value is
						   0x7ffffffff */
	int						columnCount;		/* Number of columns in the row set. */
	unsigned int			rowIndex;			/* The current row index. */
	int						columnIndex;		/* The current column index */
	void**					row;				/* The last row accessed */
	FBArray*				array;				/* The link may return a result set, the database list, we implement that by the  */
						/* FBArray, just a list of strings. */
	FBCPList*				list;				/* The same special kind result just for property list from extract, schema info. */
	unsigned int			selectResults;		/* number of results in select */
	unsigned int			currentResult;		/* current result number */
	int						lobMode;			/* 0=Fetch data (default); 1=Fetch handle */
};

struct PHPFBLink
{
	int						persistent;			/* persistent ? */
	char*					hostName;			/* Host name  */
	char*					userName;			/* User name */
	char*					userPassword;		/* User password */
	char*					databasePassword;	/* Database password */
	char*					databaseName;		/* The name of the database */
	FBCExecHandler*			execHandler;		/* The exechandler, can be used for database operations */
	FBCDatabaseConnection*	connection;			/* The connection to the database */
	unsigned int			affectedRows;		/* Number of rows affected by the last SQL statement */
	long					autoCommit;			/* Enable or disable autoCommit */
	unsigned int			errorNo;			/* The latest error on the connection, 0 is ok. */
	char*					errorText;			/* The error text */
	unsigned int			insert_id;			/* The row index of the latest row inserted into the database */
};

#define FBSQL_ASSOC		1<<0
#define FBSQL_NUM		1<<1
#define FBSQL_BOTH		(FBSQL_ASSOC|FBSQL_NUM)

#define FBSQL_LOCK_DEFERRED 0
#define FBSQL_LOCK_OPTIMISTIC 1
#define FBSQL_LOCK_PESSIMISTIC 2		/* default */

#define FBSQL_ISO_READ_UNCOMMITTED 0
#define FBSQL_ISO_READ_COMMITTED 1
#define FBSQL_ISO_REPEATABLE_READ 2
#define FBSQL_ISO_SERIALIZABLE 3		/* default */
#define FBSQL_ISO_VERSIONED 4

#define FBSQL_LOB_DIRECT 0				/* default */
#define FBSQL_LOB_HANDLE 1				/* default */

#define DIGEST_BUFFER_SIZE 17			/* fbcDigestPassword() expects a preallocated buffer for 16 bytes plus termination */

ZEND_DECLARE_MODULE_GLOBALS(fbsql)
static PHP_GINIT_FUNCTION(fbsql);

int mdOk(PHPFBLink* link, FBCMetaData* md, char* sql);
char *DigestPassword(char *user, char *password)
{
	char *digest = NULL;

	if (user && strlen(user) && password && strlen(password)) {
		char *user_upper = estrdup(user);
		digest = emalloc(DIGEST_BUFFER_SIZE);
		digest[0] = '\0';
		(void)fbcDigestPassword(php_strtoupper(user_upper, strlen(user_upper)), password, digest);
		efree(user_upper);
	}

	return digest;
}

/* {{{ arginfo */
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_connect, 0, 0, 0)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, username)
	ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_pconnect, 0, 0, 0)
	ZEND_ARG_INFO(0, username)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_close, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_set_transaction, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, locking)
	ZEND_ARG_INFO(0, isolation)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_autocommit, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, OnOff)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_commit, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_rollback, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_create_blob, 0, 0, 1)
	ZEND_ARG_INFO(0, blob_data)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_create_clob, 0, 0, 1)
	ZEND_ARG_INFO(0, clob_data)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_set_lob_mode, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, lob_mode)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_read_blob, 0, 0, 1)
	ZEND_ARG_INFO(0, blob_handle)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_read_clob, 0, 0, 1)
	ZEND_ARG_INFO(0, clob_handle)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_blob_size, 0, 0, 1)
	ZEND_ARG_INFO(0, blob_handle)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_clob_size, 0, 0, 1)
	ZEND_ARG_INFO(0, clob_handle)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_hostname, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, host_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_database, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_database_password, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, database_password)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_username, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, username)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_password, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_set_password, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, old_password)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_select_db, 0, 0, 0)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_set_characterset, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, charcterset)
	ZEND_ARG_INFO(0, in_out_both)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_change_user, 0, 0, 2)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, database)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_create_db, 0, 0, 1)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, database_options)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_drop_db, 0, 0, 1)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_start_db, 0, 0, 1)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, database_options)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_stop_db, 0, 0, 1)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_db_status, 0, 0, 1)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_query, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, batch_size)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_db_query, 0, 0, 2)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_list_dbs, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_list_tables, 0, 0, 1)
	ZEND_ARG_INFO(0, database)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_list_fields, 0, 0, 2)
	ZEND_ARG_INFO(0, database_name)
	ZEND_ARG_INFO(0, table_name)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_error, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_errno, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_warnings, 0, 0, 0)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_affected_rows, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_rows_fetched, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_insert_id, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_result, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_next_result, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_num_rows, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_num_fields, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_fetch_row, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_fetch_assoc, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_fetch_object, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_fetch_array, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_data_seek, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row_number)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_fetch_lengths, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_fetch_field, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_field_seek, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_field_name, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_field_table, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_field_len, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_field_type, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_field_flags, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_table_name, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fbsql_free_result, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fbsql_get_autostart_info, 0, 0, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ fbsql_functions[]
 */
zend_function_entry fbsql_functions[] = {
	PHP_FE(fbsql_connect,			arginfo_fbsql_connect)
	PHP_FE(fbsql_pconnect,			arginfo_fbsql_pconnect)
	PHP_FE(fbsql_close,				arginfo_fbsql_close)
	PHP_FE(fbsql_select_db,			arginfo_fbsql_select_db)
	PHP_FE(fbsql_set_characterset,	arginfo_fbsql_set_characterset)
	PHP_FE(fbsql_create_db,			arginfo_fbsql_create_db)
	PHP_FE(fbsql_drop_db,			arginfo_fbsql_drop_db)
	PHP_FE(fbsql_start_db,			arginfo_fbsql_start_db)
	PHP_FE(fbsql_stop_db,			arginfo_fbsql_stop_db)
	PHP_FE(fbsql_db_status,			arginfo_fbsql_db_status)
	PHP_FE(fbsql_query,				arginfo_fbsql_query)
	PHP_FE(fbsql_db_query,			arginfo_fbsql_db_query)
	PHP_FE(fbsql_list_dbs,			arginfo_fbsql_list_dbs)
	PHP_FE(fbsql_list_tables,		arginfo_fbsql_list_tables)
	PHP_FE(fbsql_list_fields,		arginfo_fbsql_list_fields)
	PHP_FE(fbsql_error,				arginfo_fbsql_error)
	PHP_FE(fbsql_errno,				arginfo_fbsql_errno)
	PHP_FE(fbsql_affected_rows,		arginfo_fbsql_affected_rows)
	PHP_FE(fbsql_rows_fetched,		arginfo_fbsql_rows_fetched)
	PHP_FE(fbsql_insert_id,			arginfo_fbsql_insert_id)
	PHP_FE(fbsql_result,			arginfo_fbsql_result)
	PHP_FE(fbsql_next_result,		arginfo_fbsql_next_result)
	PHP_FE(fbsql_num_rows,			arginfo_fbsql_num_rows)
	PHP_FE(fbsql_num_fields,		arginfo_fbsql_num_fields)
	PHP_FE(fbsql_fetch_row,			arginfo_fbsql_fetch_row)
	PHP_FE(fbsql_fetch_array,		arginfo_fbsql_fetch_array)
	PHP_FE(fbsql_fetch_assoc,		arginfo_fbsql_fetch_assoc)
	PHP_FE(fbsql_fetch_object,		arginfo_fbsql_fetch_object)
	PHP_FE(fbsql_data_seek,			arginfo_fbsql_data_seek)
	PHP_FE(fbsql_fetch_lengths,		arginfo_fbsql_fetch_lengths)
	PHP_FE(fbsql_fetch_field,		arginfo_fbsql_fetch_field)
	PHP_FE(fbsql_field_seek,		arginfo_fbsql_field_seek)
	PHP_FE(fbsql_free_result,		arginfo_fbsql_free_result)
	PHP_FE(fbsql_field_name,		arginfo_fbsql_field_name)
	PHP_FE(fbsql_field_table,		arginfo_fbsql_field_table)
	PHP_FE(fbsql_field_len,			arginfo_fbsql_field_len)
	PHP_FE(fbsql_field_type,		arginfo_fbsql_field_type)
	PHP_FE(fbsql_field_flags,		arginfo_fbsql_field_flags)
	PHP_FE(fbsql_table_name,		arginfo_fbsql_table_name)

/*	Fontbase additions:  */
	PHP_FE(fbsql_set_transaction,	arginfo_fbsql_set_transaction)
	PHP_FE(fbsql_autocommit,		arginfo_fbsql_autocommit)
	PHP_FE(fbsql_commit,			arginfo_fbsql_commit)
	PHP_FE(fbsql_rollback,			arginfo_fbsql_rollback)

	PHP_FE(fbsql_create_blob,		arginfo_fbsql_create_blob)
	PHP_FE(fbsql_create_clob,		arginfo_fbsql_create_clob)
	PHP_FE(fbsql_set_lob_mode,		arginfo_fbsql_set_lob_mode)
	PHP_FE(fbsql_read_blob,			arginfo_fbsql_read_blob)
	PHP_FE(fbsql_read_clob,			arginfo_fbsql_read_clob)
	PHP_FE(fbsql_blob_size,			arginfo_fbsql_blob_size)
	PHP_FE(fbsql_clob_size,			arginfo_fbsql_clob_size)

	PHP_FE(fbsql_hostname,			arginfo_fbsql_hostname)
	PHP_FE(fbsql_database,			arginfo_fbsql_database)
	PHP_FE(fbsql_database_password,	arginfo_fbsql_database_password)
	PHP_FE(fbsql_username,			arginfo_fbsql_username)
	PHP_FE(fbsql_password,			arginfo_fbsql_password)
	PHP_FE(fbsql_warnings,			arginfo_fbsql_warnings)
	PHP_FE(fbsql_set_password,		arginfo_fbsql_set_password)

	PHP_FE(fbsql_get_autostart_info,	arginfo_fbsql_get_autostart_info)
/*	PHP_FE(fbsql_set_autostart_info,	NULL) */

/*	Aliases:  */
	PHP_FALIAS(fbsql, 			fbsql_db_query,		arginfo_fbsql_db_query)
	PHP_FALIAS(fbsql_tablename,	fbsql_table_name,	arginfo_fbsql_table_name)

	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry fbsql_module_entry = {
	STANDARD_MODULE_HEADER,
	"fbsql",
	fbsql_functions,
	PHP_MINIT(fbsql),
	PHP_MSHUTDOWN(fbsql),
	PHP_RINIT(fbsql),
	PHP_RSHUTDOWN(fbsql),
	PHP_MINFO(fbsql),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(fbsql),
	PHP_GINIT(fbsql),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_FBSQL
ZEND_GET_MODULE(fbsql)
#endif

#define CHECK_LINK(link) { \
	if (link==-1) { \
		if (FB_SQL_G(generateWarnings)) \
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "A link to the server could not be established"); \
		RETURN_FALSE; \
	} \
}

static void phpfbReleaseResult(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpfbReleaseLink(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpfbReleasePLink(zend_rsrc_list_entry *rsrc TSRMLS_DC);

static void phpfbReleaseResult(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PHPFBResult* result = (PHPFBResult *)rsrc->ptr;

	if (result)
	{
		if (result->fetchHandle) {
			FBCMetaData *md = fbcdcCancelFetch(result->link->connection, result->fetchHandle);
			fbcmdRelease(md);
		}
		if (result->rowHandler)		fbcrhRelease(result->rowHandler);
		if (result->list)			fbcplRelease(result->list);
		if (result->array)			fbaRelease(result->array);
		if (result->ResultmetaData)	fbcmdRelease(result->ResultmetaData);
		efree(result);
	}
}


static void phpfbReleaseLink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PHPFBLink* link = (PHPFBLink *)rsrc->ptr;

	if (link)
	{
		if (link->hostName) free(link->hostName);
		if (link->userName) free(link->userName);
		if (link->userPassword) free(link->userPassword);
		if (link->databasePassword) free(link->databasePassword);
		if (link->databaseName) free(link->databaseName);
		if (link->errorText) free(link->errorText);
		if (link->connection) {
			fbcdcClose(link->connection);
			fbcdcRelease(link->connection);
		}
		if (link->execHandler) fbcehRelease(link->execHandler);
		efree(link);
		FB_SQL_G(linkCount)--;
	}
}

static void phpfbReleasePLink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PHPFBLink* link = (PHPFBLink *)rsrc->ptr;

	if (link)
	{
		if (link->hostName) free(link->hostName);
		if (link->userName) free(link->userName);
		if (link->userPassword) free(link->userPassword);
		if (link->databasePassword) free(link->databasePassword);
		if (link->databaseName) free(link->databaseName);
		if (link->errorText) free(link->errorText);
		if (link->connection) {
			fbcdcClose(link->connection);
			fbcdcRelease(link->connection);
		}
		if (link->execHandler) fbcehRelease(link->execHandler);
		free(link);
		FB_SQL_G(linkCount)--;
		FB_SQL_G(persistentCount)--;
	}
}

static void php_fbsql_set_default_link(int id TSRMLS_DC)
{
	if (FB_SQL_G(linkIndex)!=-1) {
		zend_list_delete(FB_SQL_G(linkIndex));
	}
	FB_SQL_G(linkIndex) = id;
	zend_list_addref(id);
}

static int php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (FB_SQL_G(linkIndex)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_fbsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	}
	return FB_SQL_G(linkIndex);
}


static void phpfbQuery(INTERNAL_FUNCTION_PARAMETERS, char* sql, PHPFBLink* link, long batch_size);

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN  ("fbsql.allow_persistent",				"1",		PHP_INI_SYSTEM, OnUpdateBool,	allowPersistent,  zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_BOOLEAN  ("fbsql.generate_warnings",			"0",		PHP_INI_SYSTEM, OnUpdateBool,	generateWarnings, zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_BOOLEAN  ("fbsql.autocommit",					"1",		PHP_INI_SYSTEM, OnUpdateBool,	autoCommit,	      zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_BOOLEAN  ("fbsql.show_timestamp_decimals",		"0",		PHP_INI_SYSTEM,	OnUpdateBool,	showTimestampDecimals,	zend_fbsql_globals,	fbsql_globals)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_persistent",				"-1",		PHP_INI_SYSTEM, OnUpdateLong,	maxPersistent,    zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_links",					"128",		PHP_INI_SYSTEM, OnUpdateLong,	maxLinks,         zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_connections",				"128",		PHP_INI_SYSTEM, OnUpdateLong,	maxConnections,   zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_results",					"128",		PHP_INI_SYSTEM, OnUpdateLong,	maxResults,       zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.batchsize",					"1000",		PHP_INI_ALL,    OnUpdateLong,	batchSize,		  zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY    ("fbsql.default_host",					NULL,		PHP_INI_SYSTEM, OnUpdateString, hostName,         zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_user",					"_SYSTEM",	PHP_INI_SYSTEM, OnUpdateString, userName,         zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_password",				"",         PHP_INI_SYSTEM, OnUpdateString, userPassword,     zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_database",				"",         PHP_INI_SYSTEM, OnUpdateString, databaseName,     zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_database_password",	"",         PHP_INI_SYSTEM, OnUpdateString, databasePassword, zend_fbsql_globals, fbsql_globals)
PHP_INI_END()
/* }}} */    

static PHP_GINIT_FUNCTION(fbsql)
{
	fbsql_globals->persistentCount = 0;

	if (fbsql_globals->hostName==NULL)
	{
		char name[256];
		gethostname(name, sizeof(name));
		name[sizeof(name)-1] = 0;
		fbsql_globals->hostName = strdup(name);
	}

	fbsql_globals->persistentCount	= 0;
	fbsql_globals->linkCount		= 0;
}

PHP_MINIT_FUNCTION(fbsql)
{
	REGISTER_INI_ENTRIES();

	fbcInitialize();
	fbcehSetMultiThreaded(True);
	le_result	= zend_register_list_destructors_ex(phpfbReleaseResult, NULL, "fbsql result", module_number);
	le_link		= zend_register_list_destructors_ex(phpfbReleaseLink, NULL, "fbsql link", module_number);
	le_plink	= zend_register_list_destructors_ex(NULL, phpfbReleasePLink, "fbsql plink", module_number);
	Z_TYPE(fbsql_module_entry) = type;

	REGISTER_LONG_CONSTANT("FBSQL_ASSOC", FBSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_NUM",   FBSQL_NUM,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_BOTH",  FBSQL_BOTH,  CONST_CS | CONST_PERSISTENT);

	/* Register Transaction constants */
	REGISTER_LONG_CONSTANT("FBSQL_LOCK_DEFERRED", FBSQL_LOCK_DEFERRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_LOCK_OPTIMISTIC", FBSQL_LOCK_OPTIMISTIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_LOCK_PESSIMISTIC", FBSQL_LOCK_PESSIMISTIC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FBSQL_ISO_READ_UNCOMMITTED", FBSQL_ISO_READ_UNCOMMITTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_ISO_READ_COMMITTED", FBSQL_ISO_READ_COMMITTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_ISO_REPEATABLE_READ", FBSQL_ISO_REPEATABLE_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_ISO_SERIALIZABLE", FBSQL_ISO_SERIALIZABLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_ISO_VERSIONED", FBSQL_ISO_VERSIONED, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FBSQL_UTF8", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_ISO8859_1", FBC_ISO8859_1, CONST_CS | CONST_PERSISTENT);

	/* Register Status constants */
	REGISTER_LONG_CONSTANT("FBSQL_UNKNOWN", FBUnknownStatus, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_STOPPED", FBStopped, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_STARTING", FBStarting, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_RUNNING", FBRunning, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_STOPPING", FBStopping, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_NOEXEC", FBNoExec, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FBSQL_LOB_DIRECT", FBSQL_LOB_DIRECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_LOB_HANDLE", FBSQL_LOB_HANDLE, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fbsql)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RINIT_FUNCTION(fbsql)
{
	FB_SQL_G(linkIndex) = -1;
	FB_SQL_G(linkCount) = FB_SQL_G(persistentCount);
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(fbsql)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(fbsql)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_header(2, "FrontBase support", "enabled");

	php_info_print_table_row(2, "Client API version", "2.24");

	if (FB_SQL_G(allowPersistent))
	{
		snprintf(buf, sizeof(buf), "%ld", FB_SQL_G(persistentCount));
		php_info_print_table_row(2, "Active Persistent Links", buf);
	}

	snprintf(buf, sizeof(buf), "%ld", FB_SQL_G(linkCount));
	php_info_print_table_row(2, "Active Links", buf);

/*
	snprintf(buf, sizeof(buf), "%ld", FB_SQL_G(resultCount));
	php_info_print_table_row(2, "Active Results", buf);
*/

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

static void php_fbsql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	PHPFBLink* phpLink;
	zend_rsrc_list_entry *lep;
	char name[1024];
	char *hostName = NULL, *userName = NULL, *userPassword = NULL;
	int argc = ZEND_NUM_ARGS(), create_new = 0;
	zval **argv[3];

	if ((argc < 0) || (argc > 3)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc, &argv[0], &argv[1], &argv[2])==FAILURE) RETURN_FALSE;
	if (argc >= 1)
	{
		convert_to_string_ex(argv[0]);
		hostName = Z_STRVAL_PP(argv[0]);
	}
	if (argc >= 2)
	{
		convert_to_string_ex(argv[1]);
		userName = Z_STRVAL_PP(argv[1]);
	}
	if (argc == 3)
	{
		convert_to_string_ex(argv[2]);
		userPassword = Z_STRVAL_PP(argv[2]);
	}

	if (hostName     == NULL) hostName     = FB_SQL_G(hostName);
	if (userName     == NULL) userName     = FB_SQL_G(userName);
	if (userPassword == NULL) userPassword = FB_SQL_G(userPassword);

	if (snprintf(name, sizeof(name), "fbsql_%s_%s_%s", hostName, userName, userPassword) < 0) {
		RETURN_FALSE;
	}

	if (!FB_SQL_G(allowPersistent)) {
		persistent=0;
	}
	if (persistent) {
		if (zend_hash_find(&EG(persistent_list), name, strlen(name) + 1, (void **)&lep) == SUCCESS)
		{
			FBCMetaData *md;
			phpLink = (PHPFBLink*)lep->ptr;
			// Check if connection still there.
			md = fbcdcRollback(phpLink->connection);
			if ( !mdOk(phpLink, md, "Rollback;") ) {
				if (FB_SQL_G(generateWarnings)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "FrontBase link is not connected, ty to reconnect.");
				}
				// Make sure select_db will reconnect.
				fbcmdRelease(md);
				fbcdcClose(phpLink->connection);
				fbcdcRelease(phpLink->connection);
				free(phpLink->connection);
				phpLink->connection = NULL;
				if (phpLink->databaseName) free(phpLink->databaseName);
				phpLink->databaseName = NULL;
			}
			else {
				fbcmdRelease(md);
			}
		}
		else {
			zend_rsrc_list_entry le;

			if ((FB_SQL_G(maxLinks) != -1 && FB_SQL_G(linkCount) == FB_SQL_G(maxLinks)))
			{
				if (FB_SQL_G(generateWarnings))
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "FrontBase link limit %d exceeded", FB_SQL_G(maxLinks));
				RETURN_FALSE;
			}

			if ((FB_SQL_G(maxPersistent) != -1 && FB_SQL_G(persistentCount) == FB_SQL_G(maxPersistent)))
			{
				if (FB_SQL_G(generateWarnings))
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "FrontBase persistent link limit %d exceeded", FB_SQL_G(maxPersistent));
				RETURN_FALSE;
			}

			phpLink = malloc(sizeof(PHPFBLink));
			phpLink->persistent       = persistent;
			phpLink->hostName         = strdup(hostName);
			phpLink->userName         = strdup(userName);
			phpLink->userPassword     = strdup(userPassword);
			phpLink->databasePassword = strdup(FB_SQL_G(databasePassword));
			phpLink->databaseName	  = NULL;
			phpLink->execHandler      = NULL;
			phpLink->affectedRows     = 0;
			phpLink->autoCommit	 	  = FB_SQL_G(autoCommit);
			phpLink->errorNo          = 0;
			phpLink->errorText        = NULL;
			phpLink->connection		  = NULL;


			le.ptr = phpLink;
			Z_TYPE(le) = le_plink;
			if (zend_hash_update(&EG(persistent_list), name, strlen(name) + 1, &le, sizeof(le), NULL)==FAILURE)
			{
				free(phpLink->hostName);
				free(phpLink->userName);
				free(phpLink->userPassword);
				free(phpLink->databasePassword);
				free(phpLink);
				RETURN_FALSE;
			}
			FB_SQL_G(linkCount)++;
			FB_SQL_G(persistentCount)++;
		}
		ZEND_REGISTER_RESOURCE(return_value, phpLink, le_plink);
	}
	else
	{
		zend_rsrc_list_entry le;

		if ((FB_SQL_G(maxLinks) != -1 && FB_SQL_G(linkCount) == FB_SQL_G(maxLinks)))
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "FrontBase link limit %d exceeded", FB_SQL_G(maxLinks));
			RETURN_FALSE;
		}

		if (zend_hash_find(&EG(regular_list), name, strlen(name) + 1, (void **)&lep) == SUCCESS)
		{
			int type, link;
			void *ptr;

			link = (int) lep->ptr;
			ptr = zend_list_find(link, &type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				zend_list_addref(link);
				Z_LVAL_P(return_value) = link;
				php_fbsql_set_default_link(link TSRMLS_CC);
				Z_TYPE_P(return_value) = IS_RESOURCE;
				return;
			} else {
				zend_hash_del(&EG(regular_list), name, strlen(name) + 1);
			}
			phpLink = (PHPFBLink*)lep->ptr;
		}

		phpLink = emalloc(sizeof(PHPFBLink));
		phpLink->persistent       = persistent;
		phpLink->hostName         = strdup(hostName);
		phpLink->userName         = strdup(userName);
		phpLink->userPassword     = strdup(userPassword);
		phpLink->databasePassword = strdup(FB_SQL_G(databasePassword));
		phpLink->databaseName	  = NULL;
		phpLink->execHandler      = NULL;
		phpLink->affectedRows     = 0;
		phpLink->autoCommit	 	  = FB_SQL_G(autoCommit);
		phpLink->errorNo          = 0;
		phpLink->errorText        = NULL;
		phpLink->connection		  = NULL;

		ZEND_REGISTER_RESOURCE(return_value, phpLink, le_link);

		le.ptr = (void *)Z_LVAL_P(return_value);
		Z_TYPE(le) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), name, strlen(name) + 1, &le, sizeof(le), NULL)==FAILURE)
		{
			free(phpLink->hostName);
			free(phpLink->userName);
			free(phpLink->userPassword);
			free(phpLink->databasePassword);
			efree(phpLink);
			RETURN_FALSE;
		}
		FB_SQL_G(linkCount)++;
	}
	php_fbsql_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
}

int phpfbFetchRow(PHPFBResult* result, unsigned int row)
{
	if (result->rowHandler == NULL)
	{
		void *rawData = fbcdcFetch(result->link->connection, result->batchSize, result->fetchHandle);
		if (rawData == NULL) {
			result->rowCount = 0;
		}
		else
			result->rowHandler = fbcrhInitWith(rawData, result->metaData);
	}
	for (;;)
	{
		void *rawData; 
		if (row >= result->rowCount && result->rowCount != 0x7fffffff) return 0;
		if (fbcrhRowCount(result->rowHandler) > (unsigned int)row) return 1;
		rawData = fbcdcFetch(result->link->connection, result->batchSize, result->fetchHandle);
		if (!fbcrhAddBatch(result->rowHandler, rawData)) result->rowCount = fbcrhRowCount(result->rowHandler);
	}
}


/* {{{ proto resource fbsql_connect([string hostname [, string username [, string password]]])
   Create a connection to a database server */
PHP_FUNCTION(fbsql_connect)
{
	php_fbsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource fbsql_pconnect([string hostname [, string username [, string password]]])
   Create a persistant connection to a database server */
PHP_FUNCTION(fbsql_pconnect)
{
	php_fbsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool fbsql_close([resource link_identifier])
   Close a connection to a database server */
PHP_FUNCTION(fbsql_close)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id, i, nument, type;
	void *ptr;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	nument = zend_hash_next_free_element(&EG(regular_list));
	for (i = 1; i < nument; i++) {
		ptr = zend_list_find(i, &type);
		if (ptr && (type == le_result)) {
			PHPFBResult *result;

			result = (PHPFBResult *)ptr;
			if (result->link == phpLink) {
				zend_list_delete(i);
			}
		}
	}

	if (id==-1) { /* explicit resource number */
		zend_list_delete(Z_RESVAL_PP(fbsql_link_index));
	}

	if (id!=-1 
		|| (fbsql_link_index && Z_RESVAL_PP(fbsql_link_index)==FB_SQL_G(linkIndex))) {
		zend_list_delete(FB_SQL_G(linkIndex));
		FB_SQL_G(linkIndex) = -1;
	}

	RETURN_TRUE;
}
/* }}} */

static int php_fbsql_select_db(char *databaseName, PHPFBLink *link TSRMLS_DC)
{
	unsigned port;
	FBCDatabaseConnection* c;
	FBCMetaData* md;

	if (!link->databaseName || strcmp(link->databaseName, databaseName)) 
	{
		port = atoi(databaseName);
		if (port>0 && port<65535)
			c = fbcdcConnectToDatabaseUsingPort(link->hostName, port, link->databasePassword);
		else
			c = fbcdcConnectToDatabase(databaseName, link->hostName, link->databasePassword);
		if (c == NULL)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", fbcdcClassErrorMessage());
			return 0;
		}
		md = fbcdcCreateSession(c, "PHP", link->userName, link->userPassword, link->userName);
		if (fbcmdErrorsFound(md))
		{
			FBCErrorMetaData* emd = fbcdcErrorMetaData(c, md);
			char*             emg = fbcemdAllErrorMessages(emd);
			if (FB_SQL_G(generateWarnings))
			{
				if (emg)
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", emg);
				else
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "No message");
			}
			link->errorText = strdup(emg);
			link->errorNo = fbcemdErrorCodeAtIndex(emd, 0);
			free(emg);
			fbcemdRelease(emd);
			fbcmdRelease(md);
			fbcdcClose(c);
			fbcdcRelease(c);
			return 0;
		}
		fbcmdRelease(md);

		if (c)
		{
			if (link->autoCommit)
				md = fbcdcExecuteDirectSQL(c, "SET COMMIT TRUE;");
			else
				md = fbcdcExecuteDirectSQL(c, "SET COMMIT FALSE;");
			fbcmdRelease(md);
		}
		fbcdcSetOutputCharacterSet(c, FBC_ISO8859_1);
		fbcdcSetInputCharacterSet(c, FBC_ISO8859_1);

		if (link->connection)
		{
			fbcdcClose(link->connection);
			fbcdcRelease(link->connection);
		}
		link->connection = c;
		if (link->databaseName) free(link->databaseName);
		link->databaseName = strdup(databaseName);
	}
	return 1;
}

void phpfbestrdup(const char * s, int* length, char** value)
{
	int l = s?strlen(s):0;
	if (value)
	{
		char* r = emalloc(l+1);
		if (s)
			strcpy(r, s);
		else
			r[0] = 0;
		*value = r;
	}
	*length = l;
}

/* {{{ proto void fbsql_set_transaction(resource link_identifier, int locking, int isolation)
   Sets the transaction locking and isolation */
PHP_FUNCTION(fbsql_set_transaction)
{
	PHPFBLink* phpLink = NULL;
	FBCMetaData* md;
	zval **fbsql_link_index = NULL, **Locking = NULL, **Isolation = NULL;
	char strSQL[1024];
	char *strLocking[] = {"DEFERRED", "OPTIMISTIC", "PESSIMISTIC"};
	char *strIsolation[] = {"READ UNCOMMITTED", "READ NCOMMITTED", "REPEATABLE READ", "SERIALIZABLE", "VERSIONED"};

	switch (ZEND_NUM_ARGS()) {
		case 3:
			if (zend_get_parameters_ex(3, &fbsql_link_index, &Locking, &Isolation)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	if (Z_LVAL_PP(Locking) < 0 || Z_LVAL_PP(Locking) > 2) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid locking type.");
		RETURN_FALSE;
	}
	if (Z_LVAL_PP(Isolation) < 0 || Z_LVAL_PP(Isolation) > 4) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid isolation type.");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (snprintf(strSQL, sizeof(strSQL) , "SET TRANSACTION LOCKING %s, ISOLATION %s;", strLocking[Z_LVAL_PP(Locking)], strIsolation[Z_LVAL_PP(Isolation)]) < 0) {
		RETURN_FALSE;
	}

	md = fbcdcExecuteDirectSQL(phpLink->connection, strSQL);
	fbcmdRelease(md);
}
/* }}} */

/* {{{ proto bool fbsql_autocommit(resource link_identifier [, bool OnOff])
   Turns on auto-commit */
PHP_FUNCTION(fbsql_autocommit)
{
	PHPFBLink* phpLink = NULL;
	FBCMetaData* md;
	zval **fbsql_link_index = NULL, **onoff = NULL;
	zend_bool OnOff;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &onoff)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (onoff)
	{
		convert_to_boolean_ex(onoff);
		OnOff = Z_BVAL_PP(onoff);
		phpLink->autoCommit = OnOff;
		if (OnOff)
			md = fbcdcExecuteDirectSQL(phpLink->connection, "SET COMMIT TRUE;");
		else
			md = fbcdcExecuteDirectSQL(phpLink->connection, "SET COMMIT FALSE;");
		fbcmdRelease(md);
	}
	RETURN_BOOL(phpLink->autoCommit);
}
/* }}} */

/* {{{ proto bool fbsql_commit([resource link_identifier])
   Commit the transaction */
PHP_FUNCTION(fbsql_commit)
{
	PHPFBLink* phpLink = NULL;
	FBCMetaData* md;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	md = fbcdcCommit(phpLink->connection);

	if (md) {
		fbcmdRelease(md);
		RETURN_TRUE;
	}
	else
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool fbsql_rollback([resource link_identifier])
   Rollback all statments since last commit */
PHP_FUNCTION(fbsql_rollback)
{
	PHPFBLink* phpLink = NULL;
	FBCMetaData* md;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	md = fbcdcRollback(phpLink->connection);

	if (md) {
		fbcmdRelease(md);
		RETURN_TRUE;
	}
	else
		RETURN_FALSE;
}
/* }}} */


static void php_fbsql_create_lob(INTERNAL_FUNCTION_PARAMETERS, int lob_type)
{
	PHPFBLink* phpLink = NULL;
	FBCBlobHandle *lobHandle;
	zval	**lob_data, **fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &lob_data)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &lob_data, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(lob_data);
	switch (lob_type) {
		case 0 : /* BLOB */
			lobHandle = fbcdcWriteBLOB(phpLink->connection, Z_STRVAL_PP(lob_data), Z_STRLEN_PP(lob_data));
			break;
		case 1 : /* CLOB */
			lobHandle = fbcdcWriteCLOB(phpLink->connection, Z_STRVAL_PP(lob_data));
			break;
	}
	if (lobHandle) {
		RETURN_STRING(fbcbhDescription(lobHandle), 1);
		fbcbhRelease(lobHandle);
	}
	else
		RETURN_FALSE;
}

/* {{{ proto string fbsql_create_blob(string blob_data [, resource link_identifier])
   Create a BLOB in the database for use with an insert or update statement */
PHP_FUNCTION(fbsql_create_blob)
{
	php_fbsql_create_lob(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string fbsql_create_clob(string clob_data [, resource link_identifier])
   Create a CLOB in the database for use with an insert or update statement */
PHP_FUNCTION(fbsql_create_clob)
{
	php_fbsql_create_lob(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool fbsql_set_lob_mode(resource result, int lob_mode)
   Sets the mode for how LOB data re retreived (actual data or a handle) */
PHP_FUNCTION(fbsql_set_lob_mode)
{

	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **lob_mode = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &lob_mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(lob_mode);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	result->lobMode = Z_LVAL_PP(lob_mode);

	RETURN_TRUE;
}
/* }}} */

static void php_fbsql_read_lob(INTERNAL_FUNCTION_PARAMETERS, int lob_type)
{
	PHPFBLink* phpLink = NULL;
	zval	**lob_handle, **fbsql_link_index = NULL;
	int id;
	long length = 0;
	char* value = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &lob_handle)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &lob_handle, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(lob_handle);

	if (Z_STRLEN_PP(lob_handle) != 27 || Z_STRVAL_PP(lob_handle)[0] != '@') {
		if (FB_SQL_G(generateWarnings)) 
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The handle is invalid");
		RETURN_FALSE;
	}

	length = fbcbhBlobSize((FBCBlobHandle *)Z_STRVAL_PP(lob_handle));
	if (lob_type == 0) 
		value = estrndup((char *)fbcdcReadBLOB(phpLink->connection, (FBCBlobHandle *)Z_STRVAL_PP(lob_handle)), length);
	else
		value = estrndup((char *)fbcdcReadCLOB(phpLink->connection, (FBCBlobHandle *)Z_STRVAL_PP(lob_handle)), length);
	if (value) {
		RETURN_STRINGL(value, length, 0);
	}
	else {
		RETURN_FALSE;
	}
}

/* {{{ proto string fbsql_read_blob(string blob_handle [, resource link_identifier])
   Read the BLOB data identified by blob_handle */
PHP_FUNCTION(fbsql_read_blob)
{
	php_fbsql_read_lob(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string fbsql_read_clob(string clob_handle [, resource link_identifier])
   Read the CLOB data identified by clob_handle */
PHP_FUNCTION(fbsql_read_clob)
{
	php_fbsql_read_lob(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

static void php_fbsql_lob_size(INTERNAL_FUNCTION_PARAMETERS, int lob_type)
{
	PHPFBLink* phpLink = NULL;
	zval	**lob_handle, **fbsql_link_index = NULL;
	int id;
	char* value = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &lob_handle)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &lob_handle, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(lob_handle);

	if (Z_STRLEN_PP(lob_handle) != 27 || Z_STRVAL_PP(lob_handle)[0] != '@') {
		if (FB_SQL_G(generateWarnings)) 
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The handle is invalid");
		RETURN_FALSE;
	}

	RETURN_LONG(fbcbhBlobSize((FBCBlobHandle *)Z_STRVAL_PP(lob_handle)));
}

/* {{{ proto int fbsql_blob_size(string blob_handle [, resource link_identifier])
   Get the size of a BLOB identified by blob_handle */
PHP_FUNCTION(fbsql_blob_size)
{
	php_fbsql_lob_size(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int fbsql_clob_size(string clob_handle [, resource link_identifier])
   Get the size of a CLOB identified by clob_handle */
PHP_FUNCTION(fbsql_clob_size)
{
	php_fbsql_lob_size(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string fbsql_hostname(resource link_identifier [, string host_name])
   Get or set the host name used with a connection */
PHP_FUNCTION(fbsql_hostname)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **host_name = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &host_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (host_name)
	{
		convert_to_string_ex(host_name);
		if (phpLink->hostName) free(phpLink->hostName);
		phpLink->hostName = strdup(Z_STRVAL_PP(host_name));
	}
	RETURN_STRING(phpLink->hostName, 1);
}
/* }}} */

/* {{{ proto string fbsql_database(resource link_identifier [, string database]) 
   Get or set the database name used with a connection */
PHP_FUNCTION(fbsql_database)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **dbname = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &dbname)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (dbname)
	{
		convert_to_string_ex(dbname);
		if (phpLink->databaseName) free(phpLink->databaseName);
		phpLink->databaseName = strdup(Z_STRVAL_PP(dbname));
	}
	if (phpLink->databaseName) {
		RETURN_STRING(phpLink->databaseName, 1);
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string fbsql_database_password(resource link_identifier [, string database_password])
   Get or set the databsae password used with a connection */
PHP_FUNCTION(fbsql_database_password)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **db_password = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &db_password)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (db_password)
	{
		convert_to_string_ex(db_password);
		if (phpLink->databasePassword) free(phpLink->databasePassword);
		phpLink->databasePassword = strdup(Z_STRVAL_PP(db_password));
	}
	RETURN_STRING(phpLink->databasePassword, 1);
}
/* }}} */

/* {{{ proto string fbsql_username(resource link_identifier [, string username])
   Get or set the host user used with a connection */
PHP_FUNCTION(fbsql_username)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **username = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &username)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (username)
	{
		convert_to_string_ex(username);
		if (phpLink->userName) free(phpLink->userName);
		phpLink->userName = strdup(Z_STRVAL_PP(username));
	}
	RETURN_STRING(phpLink->userName, 1);
}
/* }}} */

/* {{{ proto string fbsql_password(resource link_identifier [, string password])
   Get or set the user password used with a connection */
PHP_FUNCTION(fbsql_password)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **password = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &password)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	if (password)
	{
		convert_to_string_ex(password);
		if (phpLink->userPassword) free(phpLink->userPassword);
		phpLink->userPassword = strdup(Z_STRVAL_PP(password));
	}
	RETURN_STRING(phpLink->userPassword, 1);
}
/* }}} */

/* {{{ proto bool fbsql_set_password(resource link_identifier, string user, string password, string old_password)
   Change the password for a given user */
PHP_FUNCTION(fbsql_set_password)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **user, **password, **old_password;
	char *digest_password, *digest_old_password;
	FBCMetaData *md;
	
	switch (ZEND_NUM_ARGS()) {
		case 4:
			if (zend_get_parameters_ex(4, &fbsql_link_index, &user, &password, &old_password)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, -1, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(user);
	convert_to_string_ex(password);
	convert_to_string_ex(old_password);

	digest_password = DigestPassword(Z_STRVAL_PP(user), Z_STRVAL_PP(password));
	digest_old_password = DigestPassword(Z_STRVAL_PP(user), Z_STRVAL_PP(old_password));

	md = fbcdcSetPasswordForUser(phpLink->connection, Z_STRVAL_PP(user), digest_password, digest_old_password);
	if (mdOk(phpLink, md, "Change password")) {
		ZVAL_BOOL(return_value, 1);
	}
	else {
		ZVAL_BOOL(return_value, 0);
	}	
	fbcmdRelease(md);
	if (digest_old_password) efree(digest_old_password);
	if (digest_password) efree(digest_password);
}
/* }}} */


/* {{{ proto bool fbsql_select_db([string database_name [, resource link_identifier]])
   Select the database to open */
PHP_FUNCTION(fbsql_select_db)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **dbname;
	int id;
	char* name = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			name = FB_SQL_G(databaseName);
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &dbname)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(dbname);
			name = Z_STRVAL_PP(dbname);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &dbname, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(dbname);
			name = Z_STRVAL_PP(dbname);
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	if (!php_fbsql_select_db(name, phpLink TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void fbsql_set_characterset(resource link_identifier, long charcterset [, long in_out_both]])
   Change input/output character set */
PHP_FUNCTION(fbsql_set_characterset)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index, **zcharset, **zin_out;
	int id = -1;
	int charset = -1, in_out_both = 3;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_link_index, &zcharset)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &fbsql_link_index, &zcharset, &zin_out)==FAILURE) {
				RETURN_FALSE;
			}
			in_out_both = Z_LVAL_PP(zin_out);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);
	
	charset = Z_LVAL_PP(zcharset);
	
	if (in_out_both & 1) {
		fbcdcSetInputCharacterSet(phpLink->connection, charset);
	}
	if (in_out_both & 2) {
		fbcdcSetOutputCharacterSet(phpLink->connection, charset);
	}
}
/* }}} */

/* {{{ proto int fbsql_change_user(string user, string password [, string database [, resource link_identifier]])
   Change the user for a session */
PHP_FUNCTION(fbsql_change_user)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **user, **password, **database;
	int id;
	char *name = NULL, *userName, *userPassword;
	char buffer[1024];

	switch (ZEND_NUM_ARGS()) {
		case 2:
			name = FB_SQL_G(databaseName);
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(2, &user, &password)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(3, &user, &password, &database)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(database);
			name = Z_STRVAL_PP(database);
			break;
		case 4:
			if (zend_get_parameters_ex(4, &user, &password, &database, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(database);
			name = Z_STRVAL_PP(database);
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(user);
	userName = Z_STRVAL_PP(user);

	convert_to_string_ex(password);
	userPassword = Z_STRVAL_PP(password);

	if (snprintf(buffer, sizeof(buffer), "SET AUTHORIZATION %s;", userName) < 0) {
		RETURN_FALSE;
	}

	phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU, buffer, phpLink, 0);
	if (Z_LVAL_P(return_value))
	{
		free(phpLink->userName);
		phpLink->userName = strdup(userName);
	}
}
/* }}} */

/* {{{ proto bool fbsql_create_db(string database_name [, resource link_identifier [, string database_options]])
   Create a new database on the server */
PHP_FUNCTION(fbsql_create_db)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **database_name, **database_options = NULL;
	int id;
	int i, status;
	char *databaseName, *databaseOptions = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &database_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &database_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &database_name, &fbsql_link_index, &database_options)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			convert_to_string_ex(database_options);
			databaseOptions = Z_STRVAL_PP(database_options);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);

	if (phpLink->execHandler == NULL) phpLink->execHandler = fbcehHandlerForHost(phpLink->hostName, 128);
	status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
	if (status != FBUnknownStatus)
	{
		char* txt = "Unknown status";
		if      (status == FBStopped ) txt = "stopped";
		else if (status == FBStarting) txt = "starting";
		else if (status == FBRunning ) txt = "running";
		else if (status == FBStopping) txt = "stopping";
		else if (status == FBNoExec  ) txt = "no exec";
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create %s@%s, database is %s", databaseName, phpLink->hostName, txt);
		RETURN_FALSE;
	}
	if (!fbcehCreateDatabaseNamedWithOptions(phpLink->execHandler, databaseName, databaseOptions))
	{
		char* error = fbechErrorMessage(phpLink->execHandler);
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create %s@%s. %s", databaseName, phpLink->hostName, error);
		RETURN_FALSE;
	}
	for (i=0; i < 20; i++)
	{
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
		if (status == FBRunning) break;
	}
	if (status != FBRunning)
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Database %s@%s created -- status unknown", databaseName, phpLink->hostName);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int fbsql_drop_db(string database_name [, resource link_identifier])
   Drop a database on the server */
PHP_FUNCTION(fbsql_drop_db)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **database_name;
	int id;
	int i, status;
	char *databaseName;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &database_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &database_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);

	if (phpLink->execHandler == NULL) phpLink->execHandler = fbcehHandlerForHost(phpLink->hostName, 128);
	status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
	if (status != FBStopped)
	{
		char* txt = "Unknown status";
		if      (status == FBStopped      ) txt = "stopped";
		else if (status == FBUnknownStatus) txt = "nonexisting";
		else if (status == FBStarting     ) txt = "starting";
		else if (status == FBRunning      ) txt = "running";
		else if (status == FBStopping     ) txt = "stopping";
		else if (status == FBNoExec       ) txt = "no exec";
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not drop %s@%s, database is %s", databaseName, phpLink->hostName, txt);
		RETURN_FALSE;
	}

	if (!fbcehDeleteDatabaseNamed(phpLink->execHandler, databaseName))
	{
		char* error = fbechErrorMessage(phpLink->execHandler);
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not drop %s@%s. %s", databaseName, phpLink->hostName, error);
		RETURN_FALSE;
	}
	for (i=0; i < 20; i++)
	{
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
		if (status == FBUnknownStatus) break;
	}
	if (status != FBUnknownStatus)
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Database %s@%s dropped -- status unknown", databaseName, phpLink->hostName);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool fbsql_start_db(string database_name [, resource link_identifier [, string database_options]])
   Start a database on the server */
PHP_FUNCTION(fbsql_start_db)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **database_name, **database_options;
	int id;
	int i, status;
	char *databaseName, *databaseOptions = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &database_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &database_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &database_name, &fbsql_link_index, &database_options)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			convert_to_string_ex(database_options);
			databaseOptions = Z_STRVAL_PP(database_options);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);

	if (phpLink->execHandler == NULL) phpLink->execHandler = fbcehHandlerForHost(phpLink->hostName, 128);
	status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
	if ((status != FBStopped) && (status != FBRunning) && (status != FBStarting))
	{
		char* txt = "Unknown status";
		if      (status == FBStopped ) txt = "stopped";
		else if (status == FBStarting) txt = "starting";
		else if (status == FBRunning ) txt = "running";
		else if (status == FBStopping) txt = "stopping";
		else if (status == FBNoExec  ) txt = "no exec";
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not start %s@%s, as database is %s", databaseName, phpLink->hostName, txt);
		RETURN_FALSE;
	}

	if (status == FBStopped)
	{
		int dbstarted;
		if (databaseOptions != NULL) 
		{
			dbstarted = fbcehStartDatabaseNamedWithOptions(phpLink->execHandler, databaseName, databaseOptions);
		}
		else 
		{
			dbstarted = fbcehStartDatabaseNamed(phpLink->execHandler, databaseName);
		}
		if (!dbstarted)
		{
			char* error = fbechErrorMessage(phpLink->execHandler);
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not start %s@%s. %s", databaseName, phpLink->hostName, error);
			RETURN_FALSE;
		}
	}

	for (i=0; i < 20; i++)
	{
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
		if (status == FBRunning) break;
	}
	if (status != FBRunning)
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Database %s@%s started -- status unknown", databaseName, phpLink->hostName);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fbsql_stop_db(string database_name [, resource link_identifier])
   Stop a database on the server */
PHP_FUNCTION(fbsql_stop_db)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **database_name;
	int id;
	int i, status;
	char *databaseName;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &database_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &database_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);

	if (!php_fbsql_select_db(databaseName, phpLink TSRMLS_CC)) {
		RETURN_FALSE;
	}

/*	printf("Stop db %x\n", phpDatabase->connection); */
	if (!fbcdcStopDatabase(phpLink->connection))
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot stop database %s@%s", databaseName, phpLink->hostName);
		RETURN_FALSE;
	}

	for (i=0; i < 20; i++)
	{
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName);
		if (status == FBStopped) break;
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int fbsql_db_status(string database_name [, resource link_identifier])
   Gets the status (Stopped, Starting, Running, Stopping) for a given database */
PHP_FUNCTION(fbsql_db_status)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **database_name;
	int id;
	char *databaseName;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &database_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &database_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);

	if (phpLink->execHandler == NULL) phpLink->execHandler = fbcehHandlerForHost(phpLink->hostName, 128);
	if (phpLink->execHandler) {
		RETURN_LONG(fbcehStatusForDatabaseNamed(phpLink->execHandler, databaseName));
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ mdOk
 */
int mdOk(PHPFBLink* link, FBCMetaData* md, char* sql)
{
	FBCDatabaseConnection* c = link->connection;
	int result = 1;
	TSRMLS_FETCH();

	link->errorNo = 0;
	if (link->errorText)
	{
		free(link->errorText);
		link->errorText = NULL;
	}
	if (md == NULL)
	{
		link->errorNo = 1;
		link->errorText = strdup("Connection to database server was lost");
		if (FB_SQL_G(generateWarnings)) 
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", link->errorText);
		result = 0;
	}
	else if (fbcmdErrorsFound(md))
	{
		FBCErrorMetaData* emd = fbcdcErrorMetaData(c, md);
		char*             emg = fbcemdAllErrorMessages(emd);
		if (FB_SQL_G(generateWarnings))
		{
			if (emg)
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error in statement: '%s' %s", sql, emg);
			else
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "No message");
		}
		link->errorText = strdup(emg);
		link->errorNo = fbcemdErrorCodeAtIndex(emd, 0);
		free(emg);
		fbcemdRelease(emd);
		result = 0;
	}
	else if (fbcmdWarningsFound(md))
	{
		FBCErrorMetaData* emd = fbcdcErrorMetaData(c, md);
		char*             emg = fbcemdAllErrorMessages(emd);
		if (FB_SQL_G(generateWarnings))
		{
			if (emg)
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Warning in statement: '%s' %s", sql, emg);
			else
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "No message");
		}
		link->errorText = strdup(emg);
		link->errorNo = fbcemdErrorCodeAtIndex(emd, 0);
		free(emg);
		fbcemdRelease(emd);
		result = 1;
	}
	return result;
}
/* }}} */

static void phpfbQuery(INTERNAL_FUNCTION_PARAMETERS, char* sql, PHPFBLink* link, long batch_size)
{
	PHPFBResult* result = NULL;
	FBCMetaData*   md, *meta;
	char*          tp;
	char*          fh = NULL;
	unsigned int   sR = 1, cR = 0;

	meta = fbcdcExecuteDirectSQL(link->connection, sql);
	if (!mdOk(link, meta, sql))
	{
		fbcmdRelease(meta);
		ZVAL_BOOL(return_value, 0)
	}
	else {
		if (fbcmdHasMetaDataArray(meta)) {
			sR = fbcmdMetaDataArrayCount(meta);
			md = (FBCMetaData*)fbcmdMetaDataAtIndex(meta, cR);
		}
		else
			md = meta;

		tp = fbcmdStatementType(md);
		if (tp == NULL) {
			fbcmdRelease(meta);
			ZVAL_BOOL(return_value, 1)
		}
		else if ((tp[0] == 'C') || (tp[0] == 'R'))
		{
			if (sR == 1 && md) fbcmdRelease(md);
			ZVAL_BOOL(return_value, 1)
		}
		else if ((fh = fbcmdFetchHandle(md)) || tp[0] == 'E' || (tp[0] == 'U' && fh))
		{
			result = (PHPFBResult *)emalloc(sizeof(PHPFBResult));
			result->link        = link;
			result->rowHandler  = NULL;
			result->fetchHandle = NULL;
			result->ResultmetaData    = meta;
			result->metaData    = md;
			result->batchSize   = batch_size > 0 ? batch_size : FB_SQL_G(batchSize);
			result->rowCount    = 0x7fffffff;
			result->columnCount = 0;
			result->rowIndex    = 0;
			result->columnIndex = 0;
			result->row         = NULL;
			result->array       = NULL;
			result->list        = NULL;
			result->selectResults = sR;
			result->currentResult = cR;
			result->lobMode		= FBSQL_LOB_DIRECT;

			if (tp[0] != 'E')
			{
				result->columnCount = fbcmdColumnCount(md);
				result->fetchHandle = fh;
			}
			else 
			{
				char* r = fbcmdMessage(result->metaData);
				fbcrhConvertToOutputCharSet(fbcdcOutputCharacterSet(link->connection), (unsigned char *)r);
				if ((result->list = fbcplParse(r)))
				{
					result->rowCount = fbcplCount(result->list);
					result->columnCount = 7;
				}
			}
			ZEND_REGISTER_RESOURCE(return_value, result, le_result);
		}
		else if (tp[0] == 'I' || tp[0] == 'U')
		{
			if (tp[0] == 'I') link->insert_id = fbcmdRowIndex(md);
			if (sR == 1 && md) fbcmdRelease(md);
			ZVAL_BOOL(return_value, 1)
		}
		else if(tp[0] == 'A' || tp[0] == 'D') 
		{
			if (sR == 1 && md) fbcmdRelease(md);
			ZVAL_BOOL(return_value, 1)
		}
		if (link) link->affectedRows = fbcmdRowCount(md);
	}
}

/* {{{ proto resource fbsql_query(string query [, resource link_identifier [, long batch_size]])
   Send one or more SQL statements to the server and execute them */
PHP_FUNCTION(fbsql_query)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **query, **batch_size;
	int id, bs = 0;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &query, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &query, &fbsql_link_index, &batch_size)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			convert_to_long_ex(batch_size);
			bs = Z_LVAL_PP(batch_size);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(query);
	phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU, Z_STRVAL_PP(query), phpLink, bs);
}
/* }}} */

/* {{{ proto resource fbsql_db_query(string database_name, string query [, resource link_identifier])
   Send one or more SQL statements to a specified database on the server */
PHP_FUNCTION(fbsql_db_query)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **dbname, **query;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(2, &dbname, &query)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &dbname, &query, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(query);
	convert_to_string_ex(dbname);

	if (php_fbsql_select_db(Z_STRVAL_PP(dbname), phpLink TSRMLS_CC)) {
		phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU, Z_STRVAL_PP(query), phpLink, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource fbsql_list_dbs([resource link_identifier])
   Retreive a list of all databases on the server */
PHP_FUNCTION(fbsql_list_dbs)
{
	PHPFBResult* phpResult;
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	if (phpLink->execHandler == NULL) phpLink->execHandler = fbcehHandlerForHost(phpLink->hostName, 128);
	phpResult = emalloc(sizeof(PHPFBResult));
	phpResult->link        = phpLink;
	phpResult->fetchHandle = NULL;
	phpResult->rowHandler  = NULL;
	phpResult->ResultmetaData    = NULL;
	phpResult->metaData    = NULL;
	phpResult->batchSize   = FB_SQL_G(batchSize);
	phpResult->columnCount = 1;
	phpResult->rowIndex    = 0;
	phpResult->columnIndex = 0;
	phpResult->row         = NULL;
	phpResult->array       = fbcehAvailableDatabases(phpLink->execHandler);
	phpResult->rowCount    = fbaCount(phpResult->array);
	phpResult->list        = NULL;

	ZEND_REGISTER_RESOURCE(return_value, phpResult, le_result);
}
/* }}} */

/* {{{ proto resource fbsql_list_tables(string database [, int link_identifier])
   Retreive a list of all tables from the specifoied database */
PHP_FUNCTION(fbsql_list_tables)
{
	char* sql = "select t0.\"table_name\"from information_schema.tables t0, information_schema.SCHEMATA t1 where t0.schema_pk = t1.schema_pk and t1.\"schema_name\" = current_schema order by \"table_name\";";
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL, **database_name;
	int id;
	char *databaseName;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(1, &database_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &database_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);

	if (databaseName == NULL) {
		php_fbsql_select_db(FB_SQL_G(databaseName), phpLink TSRMLS_CC);
	} else {
		php_fbsql_select_db(databaseName, phpLink TSRMLS_CC);
	}

	phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU, sql, phpLink, 0);
}
/* }}} */

/* {{{ proto resource fbsql_list_fields(string database_name, string table_name [, resource link_identifier])
   Retrieve a list of all fields for the specified database.table */
PHP_FUNCTION(fbsql_list_fields)
{
	PHPFBLink* phpLink = NULL;
	zval **fbsql_link_index = NULL, **database_name, **table_name;
	int id;
	char *databaseName, *tableName;
	char sql[1024];

	switch (ZEND_NUM_ARGS()) {
		case 2:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			if (zend_get_parameters_ex(2, &database_name, &table_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &database_name, &table_name, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	convert_to_string_ex(database_name);
	databaseName = Z_STRVAL_PP(database_name);
	convert_to_string_ex(table_name);
	tableName = Z_STRVAL_PP(table_name);

	if (!php_fbsql_select_db(databaseName, phpLink TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (snprintf(sql, sizeof(sql), "SELECT * FROM %s WHERE 1=0;", tableName) < 0) {
		RETURN_FALSE;
	}

	phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU, sql, phpLink, 0);
}
/* }}} */

/* {{{ proto string fbsql_error([resource link_identifier])
   Returns the last error string */
PHP_FUNCTION(fbsql_error)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	if (phpLink->errorText == NULL) {
		RETURN_FALSE;
	}
	else {
		RETURN_STRING(phpLink->errorText, 1);
	}
}
/* }}} */

/* {{{ proto int fbsql_errno([resource link_identifier])
   Returns the last error code */
PHP_FUNCTION(fbsql_errno)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	RETURN_LONG(phpLink->errorNo);
}
/* }}} */

/* {{{ proto bool fbsql_warnings([int flag])
   Enable or disable FrontBase warnings */
PHP_FUNCTION(fbsql_warnings)
{
	int   argc     = ZEND_NUM_ARGS();
	zval	**argv[1];

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc, &argv[0])==FAILURE) RETURN_FALSE;
	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		FB_SQL_G(generateWarnings) = Z_LVAL_PP(argv[0]) != 0;
	}
	RETURN_BOOL(FB_SQL_G(generateWarnings));
}
/* }}} */

/* {{{ proto int fbsql_affected_rows([resource link_identifier])
   Get the number of rows affected by the last statement */
PHP_FUNCTION(fbsql_affected_rows)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	RETURN_LONG(phpLink->affectedRows);
}
/* }}} */

/* {{{ proto int fbsql_rows_fetched(resource result)
   Get the number of rows affected by the last statement */
PHP_FUNCTION(fbsql_rows_fetched)
{
	PHPFBResult* phpResult = NULL;
	zval	**result = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(phpResult, PHPFBResult *, result, id, "FrontBase-Result", le_result);

	if (!phpResult->rowHandler) {
		RETURN_FALSE;
	}
	else {
		RETURN_LONG(fbcrhRowCount(phpResult->rowHandler));
	}
}
/* }}} */

/* {{{ proto int fbsql_insert_id([resource link_identifier])
   Get the internal index for the last insert statement */
PHP_FUNCTION(fbsql_insert_id)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	RETURN_LONG(phpLink->insert_id);
}
/* }}} */

/* {{{ phpSizeOfInt
 */
int phpSizeOfInt (int i)
{
	int s = 1;
	if (i < 0)
	{
		s++;
		i = -i;
	}
	while ((i = i / 10)) s++;
	return s;
}
/* }}} */

void phpfbColumnAsString(PHPFBResult* result, int column, void* data , int* length, char** value TSRMLS_DC)
{
	FBCMetaData*               md          = result->metaData;
	const FBCDatatypeMetaData* dtmd        = fbcmdDatatypeMetaDataAtIndex(md, column);
	unsigned                   dtc         = fbcdmdDatatypeCode(dtmd);
	switch (dtc)
	{
		case FB_Boolean:
		{
			unsigned char v = *((unsigned char*)(data));
			if (v == 255)
				phpfbestrdup("Unknown", length, value);
			else if (v == 0)
				phpfbestrdup("False", length, value);
			else
				phpfbestrdup("True", length, value);
		}
		break;

		case FB_PrimaryKey:
		case FB_Integer:
		{ 
			int v = *((int*)data);
			char b[128];
			snprintf(b, sizeof(b), "%d", v);
			phpfbestrdup(b, length, value);
		}
		break;
#ifdef FB_TinyInteger
		case FB_TinyInteger:
		{ 
			short int v = *((FBTinyInteger*)data);
			char b[128];
			snprintf(b, sizeof(b), "%d", v);
			phpfbestrdup(b, length, value);
		}
		break;
#endif
#ifdef FB_LongInteger
		case FB_LongInteger:
		{ 
			FBLongInteger v = *((FBLongInteger*)data);
			char b[128];
#ifdef PHP_WIN32
			snprintf(b, sizeof(b), "%I64i", v);
#else
			snprintf(b, sizeof(b), "%ll", v);
#endif
			phpfbestrdup(b, length, value);
		}
		break;
#endif
		case FB_SmallInteger:
		{
			short v = *((short*)data);
			char b[128];
			snprintf(b, sizeof(b), "%d", v);
			phpfbestrdup(b, length, value);
		}
		break; 

		case FB_Float:
		case FB_Real:
		case FB_Double:
		{
			double v = *((double*)data);
			char b[128];
			snprintf(b, sizeof(b), "%F", v);
			phpfbestrdup(b, length, value);
		}
		break;

		case FB_Numeric:
		case FB_Decimal:
		{
 			unsigned precision = fbcdmdPrecision(dtmd);
 			unsigned scale = fbcdmdScale(dtmd);
  			double v = *((double*)data);
  			char b[128];
 			snprintf(b, sizeof(b), "%.*F", scale, v);
  			phpfbestrdup(b, length, value);
		}
		break;
		
		case FB_Character:
		case FB_VCharacter:
		{
			char* v = (char*)data;
			phpfbestrdup(v, length, value);
		}
		break;

		case FB_Bit:
		case FB_VBit:
		{
			const FBCColumnMetaData* clmd = fbcmdColumnMetaDataAtIndex(md, column);
			struct bitValue
			{
				unsigned int nBytes;
				unsigned char* bytes;
			};
			struct bitValue* ptr = data;
			unsigned nBits = ptr->nBytes * 8;

			if (dtc == FB_Bit) nBits = fbcdmdLength(fbccmdDatatype(clmd));
			if (nBits %8 == 0)
			{
				unsigned i;
				unsigned int l = nBits / 8;
				*length = l*2+3+1;
				if (value)
				{
					char* r = safe_emalloc(l, 2, 4);
					r[0] = 'X';
					r[1] = '\'';
					for (i = 0; i < nBits / 8; i++)
					{
						char c[4];
						snprintf(c, sizeof(c), "%02x", ptr->bytes[i]);
						r[i*2+2] = c[0];
						r[i*2+3] = c[1];
					}
					r[i*2+2] = '\'';
					r[i*2+3] = 0;
					*value = r;
				}
			}
			else
			{
				unsigned i;
				unsigned int l = nBits;
				*length = l*2+3+1;
				if (value)
				{
					char* r = safe_emalloc(l, 2, 4);
					r[0] = 'B';
					r[1] = '\'';
					for (i = 0; i < nBits; i++)
					{
						int bit = 0;
						if (i/8 < ptr->nBytes) bit = ptr->bytes[i/8] & (1<<(7-(i%8)));
						r[i*2+2] = bit?'1':'0';
					}
					r[i*2+2] = '\'';
					r[i*2+3] = 0;
					*value = r;
				}
			}
		}
		break;

		case FB_Date:
		case FB_Time:
		case FB_TimeTZ:
		case FB_TimestampTZ:
		{
			char* v = (char*)data;
			phpfbestrdup(v, length, value);
		}
		break;

		case FB_Timestamp:
		{
			char* v = (char*)data;
			if (FB_SQL_G(showTimestampDecimals)) {
				phpfbestrdup(v, length, value);
			}
			// Copy only YYYY-MM-DD HH:MM:SS
			else {
				int stringLength = strlen(v);
				stringLength = min(stringLength, 19);
 				if (value) {
					char* r = emalloc(stringLength+1);
					memcpy(r, v, stringLength);
					r[stringLength] = 0;
					*value = r;
				}
				*length = stringLength;
			}
		}
		break;
		
		case FB_YearMonth:
		{
			char b[128];
			int v = *((unsigned int*)data);
			snprintf(b, sizeof(b), "%d", v);
			phpfbestrdup(b, length, value);
		}
		break;

		case FB_DayTime:
		{
			char b[128];
			double seconds = *((double*)data);
			snprintf(b, sizeof(b), "%F", seconds);
			phpfbestrdup(b, length, value);
		}
		break;

		case FB_CLOB:
		case FB_BLOB:
		{
			if (*((unsigned char*) data) == '\1')
			{  /* Direct */
				*length = ((FBCBlobDirect *)data)->blobSize;
				*value = estrndup((char *)((FBCBlobDirect *)data)->blobData, *length);
			}
			else
			{
				FBCBlobHandle *lobHandle;
				unsigned char *bytes = (unsigned char *)data;
				char *handle = (char *)(&bytes[1]);
				lobHandle = fbcbhInitWithHandle(handle);
				*length = fbcbhBlobSize(lobHandle);

				if (result->lobMode == FBSQL_LOB_HANDLE) {
					phpfbestrdup(fbcbhDescription(lobHandle), length, value);
				}
				else {
					if (dtc == FB_BLOB) 
						*value = estrndup((char *)fbcdcReadBLOB(result->link->connection, lobHandle), *length);
					else
						*value = estrndup((char *)fbcdcReadCLOB(result->link->connection, lobHandle), *length);
				}
				fbcbhRelease(lobHandle); 
			}
		}
		break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unimplemented type (%d)", dtc);
		break;
	}
}

/* {{{ phpfbSqlResult 
 */
void phpfbSqlResult(INTERNAL_FUNCTION_PARAMETERS, PHPFBResult* result, int rowIndex, int columnIndex)
{
	void** row;
	if (result->list)
	{
		FBCPList* columns = (FBCPList*)fbcplValueForKey(result->list, "COLUMNS");
		FBCPList* column  = (FBCPList*)fbcplValueAtIndex(columns, result->rowIndex);
		if (columnIndex == 0)
		{ /* Name */
			FBCPList* name = (FBCPList*)fbcplValueForKey(column, "NAME");
			RETURN_STRING((char *)fbcplString((FBCPList*)name), 1);
		}
		else if (columnIndex == 2)
		{ /* Length */
			FBCPList* name = (FBCPList*)fbcplValueForKey(column, "WIDTH");
			RETURN_STRING((char *)fbcplString((FBCPList*)name), 1);
		}
		else if (columnIndex == 1)
		{ /* Type */
			FBCPList* name = (FBCPList*)fbcplValueForKey(column, "DATATYPE");
			RETURN_STRING((char *)fbcplString((FBCPList*)name), 1);
		}
		else if (columnIndex == 3)
		{ /* Flags */
			RETURN_STRING("", 1);
		}
		else
		{
			RETURN_STRING("", 1);
		}
	}
	else if (result->array)
	{ /* Special case for get dbs */
		RETURN_STRING(fbaObjectAtIndex(result->array, rowIndex), 1);
	}
	else if (!phpfbFetchRow(result, rowIndex))
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such row %d in result set %d", rowIndex, rowIndex);
		RETURN_FALSE;
	}
	else if (columnIndex >= result->columnCount)
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such column %d in result set %d", columnIndex, rowIndex);
		RETURN_FALSE;
	}
	else
	{
		row = fbcrhRowAtIndex(result->rowHandler, rowIndex);
		if (row == NULL)
		{
			RETURN_FALSE;
		}
		else if (row[columnIndex])
		{
			phpfbColumnAsString(result, columnIndex, row[columnIndex], &Z_STRLEN_P(return_value), &Z_STRVAL_P(return_value) TSRMLS_CC);
			Z_TYPE_P(return_value) = IS_STRING;
		}
		else
		{
			RETURN_NULL();
		}
	}
}
/* }}} */

/* {{{ proto mixed fbsql_result(int result [, int row [, mixed field]])
   ??? */
PHP_FUNCTION(fbsql_result)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **row = NULL, **field = NULL;
	int rowIndex;
	int columnIndex;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &row)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &fbsql_result_index, &row, &field)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	rowIndex = result->rowIndex;
	if (row)
	{
		convert_to_long_ex(row);
		rowIndex = Z_LVAL_PP(row);
	}

	columnIndex = result->columnIndex;
	if (field)
	{
		if ((Z_TYPE_PP(field) == IS_STRING) && (result->metaData))
		{
			for (columnIndex =0; columnIndex < result->columnCount; columnIndex ++)
			{
				const FBCColumnMetaData* cmd = fbcmdColumnMetaDataAtIndex(result->metaData, columnIndex);
				const char*              lbl = fbccmdLabelName(cmd);
				if (strcmp((char*)lbl, Z_STRVAL_PP(field)) == 0) break;
			}
			if (columnIndex == result->columnCount) RETURN_FALSE;
		}
		else
		{
			convert_to_long_ex(field);
			columnIndex = Z_LVAL_PP(field);
			if (columnIndex < 0)
			{
				if (FB_SQL_G(generateWarnings))
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal column index - %d", columnIndex);
				RETURN_FALSE;
			}
		}
	}

	phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU, result, rowIndex, columnIndex);

	result->columnIndex++;
	if (result->columnIndex == result->columnCount)
	{
		result->rowIndex++;
		result->columnIndex = 0;
	}
}
/* }}} */

/* {{{ proto bool fbsql_next_result(int result)
   Switch to the next result if multiple results are available */
PHP_FUNCTION(fbsql_next_result)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	result->currentResult++;
	if (result->currentResult < result->selectResults) {
		if (result->fetchHandle) {
			FBCMetaData *md = fbcdcCancelFetch(result->link->connection, result->fetchHandle);
			fbcmdRelease(md);
		}
		if (result->rowHandler) fbcrhRelease(result->rowHandler);
		result->metaData    = (FBCMetaData*)fbcmdMetaDataAtIndex(result->ResultmetaData, result->currentResult);
		result->fetchHandle = fbcmdFetchHandle(result->metaData);
		result->rowHandler  = NULL;
		result->batchSize   = FB_SQL_G(batchSize);
		result->rowCount    = 0x7fffffff;
		result->columnCount = fbcmdColumnCount(result->metaData);;
		result->rowIndex    = 0;
		result->columnIndex = 0;
		result->row         = NULL;
		result->array       = NULL;
		result->list        = NULL;
		if (result->link) 
			result->link->affectedRows = fbcmdRowCount(result->metaData);

		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int fbsql_num_rows(int result)
   Get number of rows */
PHP_FUNCTION(fbsql_num_rows)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL;
	int rowCount;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	if (result->array)
		rowCount = result->rowCount;
	else {
		rowCount = fbcmdRowCount(result->metaData);
		if (rowCount == -1)
		{
			phpfbFetchRow(result, 0x7fffffff);
			rowCount = result->rowCount;
		}
	}
	RETURN_LONG(rowCount);
}
/* }}} */

/* {{{ proto int fbsql_num_fields(int result)
   Get number of fields in the result set */
PHP_FUNCTION(fbsql_num_fields)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	RETURN_LONG(result->columnCount);
}
/* }}} */

/* {{{ proto array fbsql_fetch_row(resource result)
   Fetch a row of data. Returns an indexed array */
PHP_FUNCTION(fbsql_fetch_row)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_NUM, 1);
}
/* }}} */

/* {{{ proto object fbsql_fetch_assoc(resource result)
   Detch a row of data. Returns an assoc array */
PHP_FUNCTION(fbsql_fetch_assoc)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_ASSOC, 1);
}
/* }}} */

/* {{{ proto object fbsql_fetch_object(resource result [, int result_type])
   Fetch a row of data. Returns an object */
PHP_FUNCTION(fbsql_fetch_object)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_ASSOC, 2);
	if (Z_TYPE_P(return_value)==IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto array fbsql_fetch_array(resource result [, int result_type])
   Fetches a result row as an array (associative, numeric or both)*/
PHP_FUNCTION(fbsql_fetch_array)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_BOTH, 2);
}
/* }}} */

static void _parse_list(zval** return_value, FBCPList* list) 
{
	int count = fbcplCount(list);
	int i,j;

	for (i=0; i<count; i++)
	{
		char* key = (char*)fbcplKeyAtIndex(list, i);
		FBCPList* value = (FBCPList*)fbcplValueForKey(list, key);
		if (fbcplIsDictionary(value))
		{
			zval *value_array; 

			MAKE_STD_ZVAL(value_array);
			array_init(value_array);

			_parse_list(&value_array, value);
			add_assoc_zval(*return_value, key, value_array);
		}
		else if (fbcplIsArray(value))
		{
			zval *value_array; 
			int valcount = fbcplCount(value);

			MAKE_STD_ZVAL(value_array);
			array_init(value_array);

			for (j=0; j<valcount; j++)
			{
				FBCPList* arrayValue = (FBCPList*)fbcplValueAtIndex(value, j);
				if (fbcplIsString(arrayValue))
					add_index_string(value_array, j, (char*)fbcplString(arrayValue), 1);
				else {
					_parse_list(&value_array, arrayValue);
				}
			}
			add_assoc_zval(*return_value, key, value_array);
		}
		else if (fbcplIsString(value)) {
			add_assoc_string(*return_value, key, (char*)fbcplString(value), 1);
		}
	}
}

static void php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type, int expected_args)
{

	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **zresult_type = NULL;
	int rowIndex;
	int i;
	void **row;

	if (ZEND_NUM_ARGS() > expected_args) {
		WRONG_PARAM_COUNT;
	}

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &zresult_type)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(zresult_type);
			result_type = Z_LVAL_PP(zresult_type);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	rowIndex = result->rowIndex;
	if (((result_type & FBSQL_NUM) != FBSQL_NUM) && ((result_type & FBSQL_ASSOC) != FBSQL_ASSOC))
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal result type use FBSQL_NUM, FBSQL_ASSOC, or FBSQL_BOTH");
		RETURN_FALSE;
	}
	array_init(return_value);
	if (result->fetchHandle == NULL)
	{
		if (result->array == NULL && result->list == NULL)
		{
			RETURN_FALSE;
		}
		if (result->rowIndex >= result->rowCount)
		{
			RETURN_FALSE;
		}
		if (result->list) {
			char* key;
			FBCPList* value;

			value = (FBCPList*)fbcplValueAtIndex(result->list, result->rowIndex);
			key = (char*)fbcplKeyAtIndex(result->list, result->rowIndex);

			if (key && key[0] == 2)
				key = NULL;

			if (fbcplIsDictionary(value)) {
				zval *value_array; 

				MAKE_STD_ZVAL(value_array);
				array_init(value_array);

				_parse_list(&value_array, value);
				if (result_type & FBSQL_NUM || key == NULL)
				{
					add_index_zval(return_value, 0, value_array);
				}
				if (result_type & FBSQL_ASSOC && key != NULL)
				{
					add_assoc_zval(return_value, key, value_array);
				}
			}
			else if (fbcplIsArray(value)) {
				zval *value_array; 
				int valcount = fbcplCount(value);
				int j;

				MAKE_STD_ZVAL(value_array);
				array_init(value_array);

				for (j=0; j<valcount; j++)
				{
					FBCPList* arrayValue = (FBCPList*)fbcplValueAtIndex(value, j);
					if (fbcplIsString(arrayValue))
						add_index_string(value_array, j, (char*)fbcplString(arrayValue), 1);
					else {
						_parse_list(&value_array, arrayValue);
					}
				}
				if (result_type & FBSQL_NUM || key == NULL)
				{
					add_index_zval(return_value, 0, value_array);
				}
				if (result_type & FBSQL_ASSOC && key != NULL)
				{
					add_assoc_zval(return_value, key, value_array);
				}
			}
			else if (fbcplIsString(value)) {
				if (result_type & FBSQL_NUM)
				{
					add_index_string(return_value, 0, (char*)fbcplString(value), 1);
				}
				if (result_type & FBSQL_ASSOC)
				{
					add_assoc_string(return_value, key, (char*)fbcplString(value), 1);
				}
			}
		}
		else {
			if (result_type & FBSQL_NUM)
			{
				add_index_string(return_value, 0, estrdup(fbaObjectAtIndex(result->array, result->rowIndex)), 0);
			}
			if (result_type & FBSQL_ASSOC)
			{
				add_assoc_string(return_value, "Database", estrdup(fbaObjectAtIndex(result->array, result->rowIndex)), 0);
			}
		}
	}
	else {
		if (result->rowCount == 0) {
			RETURN_FALSE;
		}
		if (result->rowCount == 0x7fffffff)
		{
			if (!phpfbFetchRow(result, result->rowIndex)) {
				RETURN_FALSE;
			}
		}
		row = fbcrhRowAtIndex(result->rowHandler, rowIndex);
		if (row == NULL)
		{
			RETURN_FALSE;
		}
		for (i=0; i < result->columnCount; i++)
		{
			if (row[i])
			{
				char* value;
				unsigned int length;
				unsigned int c = 0;
				phpfbColumnAsString(result, i, row[i], &length, &value TSRMLS_CC);
				if (result_type & FBSQL_NUM)
				{
					add_index_stringl(return_value, i, value, length, c);
					c = 1;
				}
				if (result_type & FBSQL_ASSOC)
				{
					char* key = (char*)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, i));
					add_assoc_stringl(return_value, key, value, length, c);
				}
			}
			else
			{
				if (result_type & FBSQL_NUM)
				{
					add_index_unset(return_value, i);
				}
				if (result_type & FBSQL_ASSOC)
				{
					char* key = (char*)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, i));
					add_assoc_unset(return_value, key);
				}
			}
		}
	}
	result->rowIndex++;
	result->columnIndex = 0;
}

/* {{{ proto bool fbsql_data_seek(int result, int row_number)
   Move the internal row counter to the specified row_number */
PHP_FUNCTION(fbsql_data_seek)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **row_number = NULL;
	unsigned int rowIndex;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &row_number)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	convert_to_long_ex(row_number);
	rowIndex = Z_LVAL_PP(row_number);

	if (rowIndex < 0)
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal row_index (%d)", rowIndex);
		RETURN_FALSE;
	}

	if (result->rowCount == 0x7fffffff) phpfbFetchRow(result, rowIndex);
	if (rowIndex > result->rowCount) RETURN_FALSE;
	result->rowIndex = rowIndex;
	result->columnIndex = 0;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array fbsql_fetch_lengths(int result)
   Returns an array of the lengths of each column in the result set */
PHP_FUNCTION(fbsql_fetch_lengths)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL;
	int i;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	if (result->row == NULL) RETURN_FALSE;
	array_init(return_value);
	for (i=0; i < result->columnCount; i++)
	{
		unsigned length = 0;
		if (result->row[i]) phpfbColumnAsString(result, i, result->row[i], &length, NULL TSRMLS_CC);
		add_index_long(return_value, i, length);
	}
}
/* }}} */

/* {{{ proto object fbsql_fetch_field(int result [, int field_index])
   Get the field properties for a specified field_index */
PHP_FUNCTION(fbsql_fetch_field)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}
	object_init(return_value);

	add_property_string(return_value, "name",       (char*)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, column)), 1);
	add_property_string(return_value, "table",      (char*)fbccmdTableName(fbcmdColumnMetaDataAtIndex(result->metaData, column)), 1);
	add_property_long(return_value,   "max_length", fbcdmdLength(fbccmdDatatype(fbcmdColumnMetaDataAtIndex(result->metaData, column))));
	add_property_string(return_value, "type",       (char*)fbcdmdDatatypeString(fbcmdDatatypeMetaDataAtIndex(result->metaData, column)), 1);
	add_property_long(return_value,   "not_null",   !fbccmdIsNullable(fbcmdColumnMetaDataAtIndex(result->metaData, column)));
/*	Remember to add the rest */
/*	add_property_long(return_value, "primary_key", IS_PRI_KEY(fbsql_field->flags)?1:0); */
/*	add_property_long(return_value, "multiple_key", (fbsql_field->flags&MULTIPLE_KEY_FLAG?1:0)); */
/*	add_property_long(return_value, "unique_key", (fbsql_field->flags&UNIQUE_KEY_FLAG?1:0)); */
/*	add_property_long(return_value, "numeric", IS_NUM(Z_TYPE_P(fbsql_field))?1:0); */
/*	add_property_long(return_value, "blob", IS_BLOB(fbsql_field->flags)?1:0); */
/*	add_property_long(return_value, "unsigned", (fbsql_field->flags&UNSIGNED_FLAG?1:0)); */
/*	add_property_long(return_value, "zerofill", (fbsql_field->flags&ZEROFILL_FLAG?1:0)); */
}
/* }}} */

/* {{{ proto bool fbsql_field_seek(int result [, int field_index])
   ??? */
PHP_FUNCTION(fbsql_field_seek)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}

	result->columnIndex = column;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string fbsql_field_name(int result [, int field_index])
   Get the column name for a specified field_index */
PHP_FUNCTION(fbsql_field_name)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}
	if (result->list)
	{
		phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU, result, result->rowIndex, 0);
	}
	else if (result->metaData)
	{
		RETURN_STRING((char *)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, column)), 1);
		result->columnIndex = column;
	}
}
/* }}} */

/* {{{ proto string fbsql_field_table(int result [, int field_index])
   Get the table name for a specified field_index */
PHP_FUNCTION(fbsql_field_table)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}
	RETURN_STRING((char *)fbccmdTableName(fbcmdColumnMetaDataAtIndex(result->metaData, column)), 1);
}
/* }}} */

/* {{{ proto mixed fbsql_field_len(int result [, int field_index])
   Get the column length for a specified field_index */
PHP_FUNCTION(fbsql_field_len)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}
	if (result->list)
	{
		phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU, result, result->rowIndex, 2);
	}
	else if (result->metaData)
	{
		RETURN_LONG(fbcdmdLength(fbccmdDatatype(fbcmdColumnMetaDataAtIndex(result->metaData, column))));
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string fbsql_field_type(int result [, int field_index])
   Get the field type for a specified field_index */
PHP_FUNCTION(fbsql_field_type)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}
	if (result->list)
	{
		phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU, result, result->rowIndex, 1);
	}
	else if (result->metaData)
	{
		RETURN_STRING((char *)fbcdmdDatatypeString (fbcmdDatatypeMetaDataAtIndex(result->metaData, column)), 1);
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string fbsql_field_flags(int result [, int field_index])
   ??? */
PHP_FUNCTION(fbsql_field_flags)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **field_index = NULL;
	int column = -1;
	char buf[512];
	int len;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &field_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	column = result->columnIndex;
	if (field_index)
	{
		convert_to_long_ex(field_index);
		column = Z_LVAL_PP(field_index);
		if (column < 0 || column >= result->columnCount)
		{
			if (FB_SQL_G(generateWarnings))
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d no such column in result", column);
			RETURN_FALSE;
		}
	}
	strcpy(buf, "");
	if (!fbccmdIsNullable(fbcmdColumnMetaDataAtIndex(result->metaData, column))) {
		strcat(buf, "not_null ");
	}
#if 0
	if (IS_PRI_KEY(fbsql_field->flags)) {
		strcat(buf, "primary_key ");
	}
	if (fbsql_field->flags&UNIQUE_KEY_FLAG) {
		strcat(buf, "unique_key ");
	}
	if (fbsql_field->flags&MULTIPLE_KEY_FLAG) {
		strcat(buf, "multiple_key ");
	}
	if (IS_BLOB(fbsql_field->flags)) {
		strcat(buf, "blob ");
	}
	if (fbsql_field->flags&UNSIGNED_FLAG) {
		strcat(buf, "unsigned ");
	}
	if (fbsql_field->flags&ZEROFILL_FLAG) {
		strcat(buf, "zerofill ");
	}
	if (fbsql_field->flags&BINARY_FLAG) {
		strcat(buf, "binary ");
	}
	if (fbsql_field->flags&ENUM_FLAG) {
		strcat(buf, "enum ");
	}
	if (fbsql_field->flags&AUTO_INCREMENT_FLAG) {
		strcat(buf, "auto_increment ");
	}
	if (fbsql_field->flags&TIMESTAMP_FLAG) {
		strcat(buf, "timestamp ");
	}
#endif
	len = strlen(buf);
	/* remove trailing space, if present */
	if (len && buf[len-1] == ' ') {
		buf[len-1] = 0;
		len--;
	}
	RETURN_STRING(buf, 1);
}
/* }}} */

/* {{{ proto string fbsql_table_name(resource result, int index)
   Retreive the table name for index after a call to fbsql_list_tables() */
PHP_FUNCTION(fbsql_table_name)
{
	PHPFBResult* result = NULL;
	zval **fbsql_result_index = NULL, **table_index;
	unsigned index;
	char* value;	
	unsigned int length;
	void** row;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &fbsql_result_index, &table_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(table_index);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	index = Z_LVAL_PP(table_index);
	if (index < 0)
	{
		if (FB_SQL_G(generateWarnings))
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal index (%i)", index);
		RETURN_FALSE;
	}

	if (result->rowCount == 0x7fffffff) phpfbFetchRow(result, index);
	if (index > result->rowCount) RETURN_FALSE;
	result->rowIndex = index;
	result->columnIndex = 0;

	row = fbcrhRowAtIndex(result->rowHandler, index);
	phpfbColumnAsString(result, 0, row[0], &length, &value TSRMLS_CC);
	RETURN_STRINGL(value, length, 1);
}
/* }}} */

/* {{{ proto bool fbsql_free_result(resource result)
   free the memory used to store a result */
PHP_FUNCTION(fbsql_free_result)
{
	PHPFBResult* result = NULL;
	zval	**fbsql_result_index = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(result, PHPFBResult *, fbsql_result_index, -1, "FrontBase-Result", le_result);

	zend_list_delete(Z_LVAL_PP(fbsql_result_index));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array fbsql_get_autostart_info([resource link_identifier])
   ??? */
PHP_FUNCTION(fbsql_get_autostart_info)
{
	PHPFBLink* phpLink = NULL;
	zval	**fbsql_link_index = NULL;
	int id;
	FBCAutoStartInfo* asInfo;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_fbsql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &fbsql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(phpLink, PHPFBLink *, fbsql_link_index, id, "FrontBase-Link", le_link, le_plink);

	if (phpLink->execHandler == NULL) phpLink->execHandler = fbcehHandlerForHost(phpLink->hostName, 128);
	if (phpLink->execHandler == NULL) {
		if (FB_SQL_G(generateWarnings)) 
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No valid Exec handler available for this connection");
		RETURN_FALSE;
	}
	else {
		array_init(return_value);
		asInfo = fbcehGetAutoStartInfo(phpLink->execHandler);
		if (asInfo != NULL) {
			unsigned i;

			for (i=0; i<fbaCount(asInfo->infoLines); i++) {
				FBArray* infoLine = fbaObjectAtIndex(asInfo->infoLines, i);
/*
				if (fbaCount(infoLine) == 2) {
					fbaObjectAtIndex(infoLine, 0);
					fbaObjectAtIndex(infoLine, 1);
				}
				else {
*/
					add_index_string(return_value, i, fbaObjectAtIndex(infoLine, 0), 1);
/*				} 
 */

			}
		}
	}
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
