/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http:/*www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Frank M. Kromann frank@frontbase.com>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* TODO:
 *
 * ? Safe mode implementation
 */

/*	SB's list:
	- BLOBs
	- API for a more natural FB connect semantic
	- Connect & set session 
	- Autoreconnect when disconnected
	- Comments and cleanup
	- Documentation

	- Format database error messages as HTML.

	BUGS
	- Select db with no arguments
	 - Query with everything defaulted
*/
#include "php.h"
#include "php_globals.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#if WIN32|WINNT
#include <winsock.h>
#else
#include <php_config.h>
#include <build-defs.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#endif

#include "php_ini.h"

#define HAVE_FBSQL 1

#if HAVE_FBSQL
#include "php_fbsql.h"
#include <signal.h>

static int le_result, le_link, le_plink, le_dba;

struct PHPFBDatabase;
typedef struct PHPFBDatabase PHPFBDatabase;

struct PHPFBResult;
typedef struct PHPFBResult PHPFBResult;

struct PHPFBLink;
typedef struct PHPFBLink PHPFBLink;

/*	The PHPFBLink structure represents a fbsql link. The lion is used for
	a connection to a machine, it may be persistant and is reference counted.
	The reason for refcounting is mostly to avoid to think, it work independent of 
	any wierd and unforseen allocation deallocation order.

	The PHPFBDatabse structure implements to actual connection to a FrontBase server
	ot may be persistant is the link it is connected to is persistant, and refcounted
	for the same reasons as above.

	The PHPFBResult structure implements a result from the FrontBase server, and does all
	required buffereing from of results.

	In the PHP code the 3 above a data structures are referenced by means of integers in the
	range from 1 to som configurable maximum.  You can put a limit to the number of links, databases
	and results.  The integer identifications is implemented by insertion in the list, which is passed
	as an argument to all the functions, please note the list is polymorph.

	Database objects and link objects are all reused, base on the host name user name, host name database name 
	user name.  So connecting twice to the same database as the same user will return the same database id.
	We use the same coding for that as fbsql does, explioiting the underlying implementation of the lists.

	Persistant objects are put in the persistent list as well, but only by name, if you connect to a persistant object
	and it is not in the list it is simply added and get a new index, and refcounted.  Tricky, tricky ...
*/

/* Some functions which should be exported from FBCAccess */

void*        fbaObjectAtIndex();
void         fbcInitialize();
void         fbaRelease();
unsigned int fbaCount();



struct PHPFBDatabase
{
	unsigned int			retainCount;		/* The refcount */
	unsigned int			index;				/* The index in the list */
	PHPFBLink*				link;				/* A pointer to the corresponding link object, may be NULL if no link were used */
	char*					databaseName;		/* The name of the database */
	FBCDatabaseConnection*	connection;			/* The connection to the database */
	unsigned int			errorNo;			/* The latest error on the connection, 0 is ok. */
	char*					errorText;			/* The error text */
	unsigned int			resultCount;		/* Number of active result for this database */
	unsigned int			rowIndex;			/* The row index of the latest row inserted into the database */
	PHPFBResult**			results;			/* Pointer to the active results */
};

struct PHPFBResult
{
	unsigned int			retainCount;		/* The refcount */
	unsigned int			index;				/* The index into the list */
	PHPFBLink*				link;				/* The link for the result, may be NULL if no link  */
	PHPFBDatabase*			database;			/* The database for the result, may be NULL of no database is related to the result */
	FBCDatabaseConnection*	connection;			/* The database connection, just a convinience */
	char*					fetchHandle;		/* The fetch handle, the id used by the server.   */
	FBCMetaData*			metaData;			/* The metadata describing the result */
	FBCMetaData*			ResultmetaData;		/* The metadata describing the result */
	FBCRowHandler*			rowHandler;			/* The row handler, the Frontbase structure used for accessing rows in the result */
	unsigned int			batchSize;			/* The number of row to fetch when expanding the number of rows in the row handler */
	int						rowCount;			/* The number of rows in the results set.  The number of row is not in */
						/* general known when the select is done, one typically needs to fetch all the row
						   to figure out how many row you got. When the rowCount is unknown the value is
						   0x7ffffffff */
	int						columnCount;		/* Number of columns in the row set. */
	int						rowIndex;			/* The current row index. */
	int						columnIndex;		/* The current column index */
	void**					row;				/* The last row accessed */
	FBArray*				array;				/* The link may return a result set, the database list, we implement that by the  */
						/* FBArray, just a list of strings. */
	FBCPList*				list;				/* The same special kind result just for property list from extract, schema info. */
	unsigned int			selectResults;		/* number of results in select */
	unsigned int			currentResult;		/* current result number */
};

struct PHPFBLink
{
	unsigned int			retainCount;		/* The refcount */
	unsigned int			index;				/* The index in the list */
	int						persistant;			/* persistant ? */
	char*					hostName;			/* Host name  */
	char*					userName;			/* User name */
	char*					userPassword;		/* User password */
	char*					databasePassword;	/* Database password */
	FBCExecHandler*			execHandler;		/* The exechandler, can be used for database operations */
	unsigned int			affectedRows;
	long					autoCommit;			/* Enable or disable autoCommit */

	PHPFBDatabase*			currentDatabase;	/* The latest referenced database on this link */

	unsigned int			databaseCount;		/* The number of database connect to the link */
	PHPFBDatabase**			databases;			/* and the pointers */

	unsigned int			resultCount;		/* The number of link results */
	PHPFBResult**			results;			/* and the pointers.  Only used for list_dbs */
};

#define FBSQL_ASSOC		1<<0
#define FBSQL_NUM		1<<1
#define FBSQL_BOTH		(FBSQL_ASSOC|FBSQL_NUM)


function_entry fbsql_functions[] = {
	PHP_FE(fbsql_connect,		NULL)
	PHP_FE(fbsql_pconnect,		NULL)
	PHP_FE(fbsql_close,			NULL)
	PHP_FE(fbsql_select_db,		NULL)
	PHP_FE(fbsql_create_db,		NULL)
	PHP_FE(fbsql_drop_db,		NULL)
	PHP_FE(fbsql_start_db,		NULL)
	PHP_FE(fbsql_stop_db,		NULL)
	PHP_FE(fbsql_query,			NULL)
	PHP_FE(fbsql_db_query,		NULL)
	PHP_FE(fbsql_list_dbs,		NULL)
	PHP_FE(fbsql_list_tables,	NULL)
	PHP_FE(fbsql_list_fields,	NULL)
	PHP_FE(fbsql_error,			NULL)
	PHP_FE(fbsql_errno,			NULL)
	PHP_FE(fbsql_affected_rows,	NULL)
	PHP_FE(fbsql_insert_id,		NULL)
	PHP_FE(fbsql_result,		NULL)
	PHP_FE(fbsql_next_result,	NULL)
	PHP_FE(fbsql_num_rows,		NULL)
	PHP_FE(fbsql_num_fields,	NULL)
	PHP_FE(fbsql_fetch_row,		NULL)
	PHP_FE(fbsql_fetch_array,	NULL)
	PHP_FE(fbsql_fetch_assoc,	NULL)
	PHP_FE(fbsql_fetch_object,	NULL)
	PHP_FE(fbsql_data_seek,		NULL)
	PHP_FE(fbsql_fetch_lengths,	NULL)
	PHP_FE(fbsql_fetch_field,	NULL)
	PHP_FE(fbsql_field_seek,	NULL)
	PHP_FE(fbsql_free_result,	NULL)
	PHP_FE(fbsql_field_name,	NULL)
	PHP_FE(fbsql_field_table,	NULL)
	PHP_FE(fbsql_field_len,		NULL)
	PHP_FE(fbsql_field_type,	NULL)
	PHP_FE(fbsql_field_flags,	NULL) 

/*	Fontbase additions:  */
	PHP_FE(fbsql_autocommit,	NULL)
	PHP_FE(fbsql_commit,		NULL)
	PHP_FE(fbsql_rollback,		NULL)

	PHP_FE(fbsql_hostname,		NULL)
	PHP_FE(fbsql_database,		NULL)
	PHP_FE(fbsql_database_password,	NULL)
	PHP_FE(fbsql_username,		NULL)
	PHP_FE(fbsql_password,		NULL)
	PHP_FE(fbsql_warnings,		NULL)

/*	Aliases:  */
	PHP_FALIAS(fbsql, fbsql_db_query, NULL)

	{NULL, NULL, NULL}
};

zend_module_entry fbsql_module_entry = {
   "fbsql",
   fbsql_functions,
   PHP_MINIT(fbsql),
   PHP_MSHUTDOWN(fbsql),
   PHP_RINIT(fbsql),
   PHP_RSHUTDOWN(fbsql),
   PHP_MINFO(fbsql),
   STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(fbsql)

#ifdef COMPILE_DL_FBSQL
ZEND_GET_MODULE(fbsql)
#endif


void phpfbReleaseResult   (PHPFBResult*   result);
void phpfbReleaseDatabase (PHPFBDatabase* database);
void phpfbReleaseLink     (PHPFBLink*     link);
void phpfbReleasePLink    (PHPFBLink*     link);

PHPFBResult* phpfbRetainResult (PHPFBResult* result)
{
	if (result) result->retainCount++;
	return result;
}

void phpfbReleaseResult (PHPFBResult* result)
{
	unsigned int i;
	FBSQLLS_FETCH();
	if (result)
	{
		result->retainCount--;

		if (result->retainCount == 0)
		{
			if (result->fetchHandle) {
				FBCMetaData *md = fbcdcCancelFetch(result->connection,result->fetchHandle);
				fbcmdRelease(md);
			}
			if (result->rowHandler)  fbcrhRelease(result->rowHandler);
			if (result->ResultmetaData)    fbcmdRelease(result->ResultmetaData);
			if (result->list)        fbcplRelease(result->list);
			if (result->array)       fbaRelease(result->array);
			if (result->database)
			{
				for (i=0; i < result->database->resultCount; i++)
				{
					if (result->database->results[i] == result)
					{
						result->database->results[i] = NULL;
						break;
					}
				}
//				phpfbReleaseDatabase(result->database);
			}
			if (result->link)
			{
				for (i=0; i < result->link->resultCount; i++)
				{
					if (result->link->results[i] == result)
					{
						result->link->results[i] = NULL;
						break;
					}
				}
//				phpfbReleaseLink(result->link);
			}
			result->link        = 0;
			result->database    = 0;
			result->connection  = NULL;
			result->fetchHandle = NULL;
			result->metaData    = NULL;
			result->rowHandler  = NULL;
			result->batchSize   = 0;
			result->rowCount    = -1;
			result->rowIndex    = 0;
			result->columnIndex = 0;
			result->row         = NULL;
			result->array       = NULL;
			result->list        = NULL;
			efree(result);
		}
	}
}

PHPFBDatabase* phpfbRetainDatabase (PHPFBDatabase* database)
{
	if (database) database->retainCount++;
	return database;
}

void phpfbReleaseDatabase (PHPFBDatabase* database)
{
	if (database)
	{
		database->retainCount--;
		if (database->retainCount == 0)
		{
			unsigned int i;
			if (database->link)
			{
				for (i=0; i < database->link->databaseCount; i++)
				{
					if (database->link->databases[i] == database)
					{
						database->link->databases[i] = NULL;
						break;
					}
				}
//				phpfbReleaseLink(database->link);
			}
			fbcdcClose(database->connection);
			fbcdcRelease(database->connection);
			if (database->databaseName) efree(database->databaseName);
			if (database->results) efree(database->results);
			efree(database);
		}
	}
}

PHPFBLink* phpfbRetainLink (PHPFBLink* link)
{
	if (link) link->retainCount++;
	return link;
}

void phpfbReleaseLink (PHPFBLink* link)
{
	FBSQLLS_FETCH();
	if (link)
	{
		link->retainCount--;
		if (link->retainCount == 0)
		{
			if (link->index == FB_SQL_G(linkIndex)) FB_SQL_G(linkIndex)--;
			if (link->hostName) efree(link->hostName);
			if (link->userName) efree(link->userName);
			if (link->userPassword) efree(link->userPassword);
			if (link->databasePassword) efree(link->databasePassword);
			if (link->results) efree(link->results);
			if (link->databases) efree(link->databases);
			efree(link);
			FB_SQL_G(linkCount)--;
		}
	}
}

void phpfbReleasePLink (PHPFBLink* link)
{
	FBSQLLS_FETCH();
	if (link)
	{
		link->retainCount--;
		if (link->retainCount == 0)
		{
			if (link->index == FB_SQL_G(linkIndex)) FB_SQL_G(linkIndex)--;
			if (link->hostName) efree(link->hostName);
			if (link->userName) efree(link->userName);
			if (link->userPassword) efree(link->userPassword);
			if (link->databasePassword) efree(link->databasePassword);
			if (link->results) efree(link->results);
			if (link->databases) efree(link->databases);
			efree(link);
			FB_SQL_G(linkCount)--;
			FB_SQL_G(persistantCount)--;
		}
	}
}

PHPFBResult* phpfbQuery (INTERNAL_FUNCTION_PARAMETERS, char* sql, PHPFBDatabase* database);

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN  ("fbsql.allow_persistant",				"1",		PHP_INI_SYSTEM, OnUpdateInt,    allowPersistent,  zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_BOOLEAN  ("fbsql.generate_warnings",			"0",		PHP_INI_SYSTEM, OnUpdateInt,    generateWarnings, zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_BOOLEAN  ("fbsql.autocommit",					"1",		PHP_INI_SYSTEM, OnUpdateInt,    autoCommit,	      zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_persistent",				"-1",		PHP_INI_SYSTEM, OnUpdateInt,    maxPersistant,    zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_links",					"128",		PHP_INI_SYSTEM, OnUpdateInt,    maxLinks,         zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_connections",				"128",		PHP_INI_SYSTEM, OnUpdateInt,    maxConnections,   zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.max_results",					"128",		PHP_INI_SYSTEM, OnUpdateInt,    maxResults,       zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX ("fbsql.mbatchSize",					"1000",		PHP_INI_SYSTEM, OnUpdateInt,    batchSize,		  zend_fbsql_globals, fbsql_globals, display_link_numbers)
	STD_PHP_INI_ENTRY    ("fbsql.default_host",					NULL,		PHP_INI_SYSTEM, OnUpdateString, hostName,         zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_user",					"_SYSTEM",	PHP_INI_SYSTEM, OnUpdateString, userName,         zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_password",				"",         PHP_INI_SYSTEM, OnUpdateString, userPassword,     zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_database",				"",         PHP_INI_SYSTEM, OnUpdateString, databaseName,     zend_fbsql_globals, fbsql_globals)
	STD_PHP_INI_ENTRY    ("fbsql.default_database_password",	"",         PHP_INI_SYSTEM, OnUpdateString, databasePassword, zend_fbsql_globals, fbsql_globals)
PHP_INI_END()
      

static void php_fbsql_init_globals(zend_fbsql_globals *fbsql_globals)
{
	fbsql_globals->persistantCount = 0;

	if (fbsql_globals->hostName==NULL)
	{
		char name[256];
		gethostname(name,sizeof(name));
		name[sizeof(name)-1] = 0;
		fbsql_globals->hostName = estrdup(name);
	}

	fbsql_globals->persistantCount	= 0;
	fbsql_globals->linkCount		= 0;
	fbsql_globals->resultCount		= 0;
  
	fbsql_globals->linkIndex		= 0;
	fbsql_globals->databaseIndex	= 0;
	fbsql_globals->resultIndex		= 0;
}
                                        
PHP_MINIT_FUNCTION(fbsql)
{
	ZEND_INIT_MODULE_GLOBALS(fbsql, php_fbsql_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	fbcInitialize();

	le_result   = register_list_destructors(phpfbReleaseResult,NULL);
	le_link     = register_list_destructors(phpfbReleaseLink,NULL);
	le_plink    = register_list_destructors(NULL, phpfbReleasePLink);
	le_dba      = register_list_destructors(phpfbReleaseDatabase,NULL);

	REGISTER_LONG_CONSTANT("FBSQL_ASSOC", FBSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_NUM",   FBSQL_NUM,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FBSQL_BOTH",  FBSQL_BOTH,  CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fbsql)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RINIT_FUNCTION(fbsql)
{
   return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(fbsql)
{
   return SUCCESS;
}

PHP_MINFO_FUNCTION(fbsql)
{
	char buf[32];
	FBSQLLS_FETCH();
	php_info_print_table_start();
	php_info_print_table_header(2, "FrontBase support", "enabled");

	php_info_print_table_row(2, "Client API version", "2.20");

	if (FB_SQL_G(allowPersistent))
	{
		sprintf(buf, "%ld", FB_SQL_G(persistantCount));
		php_info_print_table_row(2, "Active Persistant Links", buf);
	}

	sprintf(buf, "%ld", FB_SQL_G(linkCount));
	php_info_print_table_row(2, "Active Links", buf);

	sprintf(buf, "%ld", FB_SQL_G(resultCount));
	php_info_print_table_row(2, "Active Result Sets", buf);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

PHPFBLink* phpfbConnect(INTERNAL_FUNCTION_PARAMETERS, char *hostName, char *userName, char *userPassword, int persistant)
{
/*	Ref count databases and links!!
	Search for the database/link in the hash plist and in the hashed list
	If a link was non persistant then an is opened peristant just insert it 
	in the persistant list

	Insert the persistant in the list and in the hash p list
	Insert the non persistant in the */
	PHPFBLink* result;
	list_entry *lep;
	int        type;
	char       name[1024];
	FBSQLLS_FETCH();

	if (hostName     == NULL) hostName     = FB_SQL_G(hostName);
	if (userName     == NULL) userName     = FB_SQL_G(userName);
	if (userPassword == NULL) userPassword = FB_SQL_G(userPassword);

	sprintf(name,"fbsql_%s_%s",hostName,userName);
	if(zend_hash_find(&EG(persistent_list), name, strlen(name), (void **) &lep) == SUCCESS)
	{
		PHPFBLink* lnk;
		result = lep->ptr;
		lnk = zend_list_find(result->index,&type);
		if (lnk != result) result->index = zend_list_insert(result, le_link);
	}
	else if(zend_hash_find(&EG(regular_list),name,strlen(name),(void **)&lep) == SUCCESS)
	{
		result = lep->ptr;
	}
	else if (FB_SQL_G(linkCount) == FB_SQL_G(maxLinks))
	{
		php_error(E_WARNING,"FrontBase link limit %d exceeded ", FB_SQL_G(maxLinks));
		return NULL;
	}
	else
	{
		FBCExecHandler* execHandler = fbcehHandlerForHost(hostName,128);
		list_entry le;
		result = emalloc(sizeof(PHPFBLink));
		result->retainCount      = 1;
		result->persistant       = persistant;
		result->hostName         = estrdup(hostName);
		result->userName         = estrdup(userName);
		result->userPassword     = estrdup(userPassword);
		result->databasePassword = estrdup(FB_SQL_G(databasePassword));
		result->execHandler      = execHandler;
		result->affectedRows     = 0;
		result->autoCommit	 	 = FB_SQL_G(autoCommit);
		result->currentDatabase  = NULL;
		result->databaseCount    = 0;
		result->databases        = NULL;
		result->resultCount      = 0;
		result->results          = NULL;

		le.ptr  = result;
		le.type = le_link; 
		if (zend_hash_update(persistant?&EG(persistent_list):&EG(regular_list), name, strlen(name), &le, sizeof(le), NULL)==FAILURE)
		{
			phpfbReleaseLink(result);
			return NULL;
        }
		result->index = zend_list_insert (phpfbRetainLink(result), le_link);
		FB_SQL_G(linkCount)++;
	}
	return result;
}


int phpfbFetchRow(PHPFBResult* result, int row)
{
	if (result->rowHandler == NULL)
	{
		void *rawData = fbcdcFetch(result->connection,result->batchSize,result->fetchHandle);
		if (rawData == NULL)
			result->rowCount = 0;
		else
			result->rowHandler = fbcrhInitWith(rawData, result->metaData);
	}
	for (;;)
	{
		void *rawData; 
		if (row >=  result->rowCount) return 0;
		if (fbcrhRowCount(result->rowHandler) > (unsigned int)row) return 1;
		rawData = fbcdcFetch(result->connection,result->batchSize,result->fetchHandle);
		if (!fbcrhAddBatch(result->rowHandler,rawData)) result->rowCount = fbcrhRowCount(result->rowHandler);
	}
	return 0;
}

void phpfbDoConnect(INTERNAL_FUNCTION_PARAMETERS,int persistant)
{
	PHPFBLink* result; 
	char	*hostName     = NULL;
	char	*userName     = NULL;
	char	*userPassword = NULL;
	int		argc         = ARG_COUNT(ht);
	zval	**argv[3];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 3)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1],&argv[2])==FAILURE) RETURN_FALSE;
	if (argc >= 1)
	{
		convert_to_string_ex(argv[0]);
		hostName = (*argv[0])->value.str.val;
	}
	if (argc >= 2)
	{
		convert_to_string_ex(argv[1]);
		userName =  (*argv[1])->value.str.val;
	}   
	if (argc == 3)
	{
		convert_to_string_ex(argv[2]);
		userPassword =  (*argv[2])->value.str.val;
	}
	result = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,hostName,userName,userPassword,persistant);
	if (result == NULL) RETURN_FALSE;
	FB_SQL_G(linkIndex)      = result->index;
	RETURN_LONG(result->index);
}

/* {{{ proto int fbsql_connect([string hostname [, string username [, string password]]]);
	*/
PHP_FUNCTION(fbsql_connect)
{
	phpfbDoConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */


/* {{{ proto int fbsql_pconnect([string hostname [, string username [, string password]]]);
	*/
PHP_FUNCTION(fbsql_pconnect)
{
	phpfbDoConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */


PHPFBLink* phpfbGetLink(int id)
{
	int         type;
	PHPFBLink* phpLink = NULL;
	FBSQLLS_FETCH();

	if (id  == 0)
	{
		php_error(E_WARNING,"FrontBase has no default connection");
	}
	else if ((!(phpLink = (PHPFBLink*) zend_list_find (id,&type))) || ((type != le_link)))
	{
		php_error(E_WARNING,"%d is not a FBSQL link index",id);
		phpLink = NULL;
	}
	return phpLink;
}

PHPFBResult* phpfbGetResult(int id)
{
	int           type;
	PHPFBResult* result = NULL;
	FBSQLLS_FETCH();

	if (id  == 0) {
		php_error(E_WARNING,"FBSQL no default result");
	}
	else if ((!(result = (PHPFBResult*) zend_list_find (id,&type))) || ((type != le_result))) {
		php_error(E_WARNING,"%d is not a FBSQL result index",id);
		result = NULL;
	}
	return result;
}


/* {{{ proto int fbsql_close([int link_identifier])
	*/
PHP_FUNCTION(fbsql_close)
{
	int   argc = ARG_COUNT(ht);
	unsigned   i,j;
	char  name[1024];
	PHPFBLink* phpLink = NULL;
	zval	**argv[1];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc, &argv[0])==FAILURE) RETURN_FALSE;
	if (argc == 1)
	{
		convert_to_long_ex(argv[0]);
		phpLink = phpfbGetLink((*argv[0])->value.lval);
	}
	else if (FB_SQL_G(linkIndex) != 0)
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
	for (i=0; i < phpLink->databaseCount; i++)
	{
		PHPFBDatabase* database = phpLink->databases[i];
		if (database)
		{
			for (j=0; j < database->resultCount; j++) if (database->results[j])
			{
				zend_list_delete (database->results[j]->index);
				FB_SQL_G(resultCount)--;
			}
			zend_list_delete (database->index);
			FB_SQL_G(databaseCount)--;
		}
	}
	sprintf(name,"fbsql_%s_%s",phpLink->hostName,phpLink->userName);
	FB_SQL_G(linkCount)--;
	if (phpLink->persistant)FB_SQL_G(persistantCount)--;
	zend_hash_del (&EG(regular_list),name,strlen(name));
	zend_list_delete(phpLink->index);
         
	RETURN_TRUE;
}
/* }}} */

PHPFBDatabase* phpfbSelectDB 
(	INTERNAL_FUNCTION_PARAMETERS,
	char*        databaseName,
	PHPFBLink*   link
)
{
	PHPFBDatabase* result = NULL;
	list_entry *lep;
	unsigned i, port;
	char name[1024];
	FBSQLLS_FETCH();

	sprintf(name,"fbsql_%s@%s:%s",databaseName,link->hostName,link->userName);
	if(zend_hash_find(&EG(persistent_list),name,strlen(name),(void **)&lep) == SUCCESS)
	{
		PHPFBDatabase* dba;
		int type;
		result = lep->ptr;
		dba = zend_list_find(result->index,&type);
		if (dba != result) result->index = zend_list_insert(result, le_dba);
	}
	else if(zend_hash_find(&EG(regular_list),name,strlen(name),(void **)&lep) == SUCCESS)
	{
		result = lep->ptr;
	}
	else if (FB_SQL_G(databaseCount) == FB_SQL_G(maxConnections))
	{
		php_error(E_WARNING,"FrontBase connection limit %d exceeded", FB_SQL_G(maxConnections));
		return NULL;
	}
	else
	{
		list_entry             le;
		FBCDatabaseConnection* c;
		FBCMetaData*           md;

		port = atoi(databaseName);
		if (port>0 && port<65535)
			c = fbcdcConnectToDatabaseUsingPort(link->hostName, port, link->databasePassword);
		else
			c = fbcdcConnectToDatabase(databaseName, link->hostName, link->databasePassword);
		if (c == NULL)
		{
			php_error(E_WARNING, fbcdcClassErrorMessage());
			return NULL;
		}
		md = fbcdcCreateSession(c,"PHP",link->userName, link->userPassword, link->userName);
		if (fbcmdErrorsFound(md))
		{
			FBCErrorMetaData* emd = fbcdcErrorMetaData(c,md);
			char*             emg = fbcemdAllErrorMessages(emd);
			if (emg)
				php_error(E_WARNING, emg);
			else
				php_error(E_WARNING,"No message");
			efree(emg);
			fbcemdRelease(emd);
			fbcmdRelease(md);
			fbcdcClose(c);
			fbcdcRelease(c);
			return NULL;
		}
		fbcmdRelease(md);

		if (c)
		{
			if (link->autoCommit)
				md = fbcdcExecuteDirectSQL(c,"SET COMMIT TRUE;");
			else
				md = fbcdcExecuteDirectSQL(c,"SET COMMIT FALSE;");
			fbcmdRelease(md);
		}
		fbcdcSetOutputCharacterSet(c,FBC_ISO8859_1);
		fbcdcSetInputCharacterSet(c,FBC_ISO8859_1);

		le.type = le_dba;
		le.ptr  = result = emalloc(sizeof(PHPFBDatabase));

		if (zend_hash_update(link->persistant?&EG(persistent_list):&EG(regular_list), name, strlen(name), &le, sizeof(le), NULL)==FAILURE)
		{
			fbcdcClose(c);
			fbcdcRelease(c);
			efree(result);
			return NULL;
		}
		result->retainCount      = 2;
		result->index            = zend_list_insert((PHPFBDatabase*)(le.ptr), le_dba);
		result->link             = phpfbRetainLink(link);
		result->databaseName     = estrdup(databaseName);
		result->connection       = c;
		result->errorNo          = 0;
		result->errorText        = NULL;
		result->resultCount      = 0;
		result->rowIndex         = 0;
		result->results          = NULL;
		for (i=0; i < link->databaseCount; i++) if ((link->databases[i]) == NULL) break;
		if (i==link->databaseCount)
		{
			unsigned j;
			link->databaseCount += 5;
			link->databases      = realloc(link->databases,sizeof(PHPFBDatabase*)*link->databaseCount);
			for (j=i; j < link->databaseCount; j++) link->databases[j] = NULL;
		}
		link->databases[i] = result;

		FB_SQL_G(databaseCount)++;
	}
	link->currentDatabase     = result;
	FB_SQL_G(databaseIndex)   = result->index;
	return_value->value.lval  = result->index;
	return_value->type        = IS_LONG;

	return result;
}
/* }}} */

void phpfbestrdup (const char * s, int* length, char** value)
{
	int   l = s?strlen(s):0;
	if (value)
	{
		char* r = emalloc(l+1);
		if (s)
			strcpy(r,s);
		else
			r[0] = 0;
		*value  = r;
	}
	*length = l;
}

/* {{{ proto bool fbsql_autocommit(int link_identifier [, bool OnOff])
	*/
PHP_FUNCTION(fbsql_autocommit)
{
	int		argc = ARG_COUNT(ht);
	zval	**argv[2];
	PHPFBLink*     phpLink = NULL;
	FBCMetaData* md;
	zend_bool OnOff;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	phpLink = phpfbGetLink((*argv[0])->value.lval);    

	if (argc == 2)
	{
		convert_to_boolean_ex(argv[1]);
		OnOff = Z_BVAL_PP(argv[1]);
		phpLink->autoCommit = OnOff;
		if (OnOff)
			md = fbcdcExecuteDirectSQL(phpLink->currentDatabase->connection, "SET COMMIT TRUE;");
		else
			md = fbcdcExecuteDirectSQL(phpLink->currentDatabase->connection, "SET COMMIT FALSE;");
		fbcmdRelease(md);
	}
	RETURN_BOOL(phpLink->autoCommit);
}
/* }}} */

/* {{{ proto int fbsql_commit([int link_identifier])
	*/
PHP_FUNCTION(fbsql_commit)
{
	int		argc    = ARG_COUNT(ht);
	zval	**argv[1];
	PHPFBLink*     phpLink = NULL;
	FBCMetaData* md;
	int link;
	FBSQLLS_FETCH();

	link = FB_SQL_G(linkIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc == 1)
	{
		convert_to_long_ex(argv[0]);
		link = (*argv[0])->value.lval;
	}

	phpLink = phpfbGetLink(link);
	if (phpLink == NULL) RETURN_FALSE;

	md = fbcdcCommit(phpLink->currentDatabase->connection);

	if (md) {
		fbcmdRelease(md);
		RETURN_TRUE;
	}
	else
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto int fbsql_rollback([int link_identifier])
	*/
PHP_FUNCTION(fbsql_rollback)
{
	int		argc    = ARG_COUNT(ht);
	zval	**argv[1];
	PHPFBLink*     phpLink = NULL;
	FBCMetaData* md;
	int link;
	FBSQLLS_FETCH();

	link = FB_SQL_G(linkIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc == 1)
	{
		convert_to_long_ex(argv[0]);
		link = Z_LVAL_PP(argv[0]);
	}

	phpLink = phpfbGetLink(link);
	if (phpLink == NULL) RETURN_FALSE;

	md = fbcdcRollback(phpLink->currentDatabase->connection);

	if (md) {
		fbcmdRelease(md);
		RETURN_TRUE;
	}
	else
		RETURN_FALSE;
}
/* }}} */


/* {{{ proto string fbsql_hostname(int link_identifier [, string host_name])
	*/
PHP_FUNCTION(fbsql_hostname)
{
	int		argc = ARG_COUNT(ht);
	zval	**argv[2];
	PHPFBLink*     phpLink = NULL;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	phpLink = phpfbGetLink(Z_LVAL_PP(argv[0]));
	if (phpLink == NULL) RETURN_FALSE;

	if (argc == 2)
	{
		convert_to_string_ex(argv[1]);
		if (phpLink->hostName) efree(phpLink->hostName);
		phpLink->hostName = estrndup(Z_STRVAL_PP(argv[1]), Z_STRLEN_PP(argv[1]));
	}
	RETURN_STRING(phpLink->hostName, 1);
}
/* }}} */


/* {{{ proto string fbsql_database(int link_identifier [, string database])
	*/
PHP_FUNCTION(fbsql_database)
{
	int  argc = ARG_COUNT(ht);
	zval	**argv[2];
	PHPFBLink*     phpLink = NULL;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	phpLink = phpfbGetLink(Z_LVAL_PP(argv[0]));
	if (phpLink == NULL) RETURN_FALSE;

	if (argc == 2)
	{
		convert_to_string_ex(argv[1]);
		if (phpLink->currentDatabase->databaseName) efree(phpLink->currentDatabase->databaseName);
		phpLink->currentDatabase->databaseName = estrndup(Z_STRVAL_PP(argv[1]), Z_STRLEN_PP(argv[1]));
	}
	RETURN_STRING(phpLink->currentDatabase->databaseName, 1);
}
/* }}} */


/* {{{ proto string fbsql_database_password(int link_identifier [, string database_password])
	*/
PHP_FUNCTION(fbsql_database_password)
{
	int  argc = ARG_COUNT(ht);
	zval	**argv[2];
	PHPFBLink*     phpLink = NULL;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	phpLink = phpfbGetLink(Z_LVAL_PP(argv[0]));
	if (phpLink == NULL) RETURN_FALSE;

	if (argc == 2)
	{
		convert_to_string_ex(argv[1]);
		if (phpLink->databasePassword) efree(phpLink->databasePassword);
		phpLink->databasePassword = estrndup(Z_STRVAL_PP(argv[1]), Z_STRLEN_PP(argv[1]));
	}
	RETURN_STRING(phpLink->databasePassword, 1);
}
/* }}} */


/* {{{ proto string fbsql_username(int link_identifier [, string username])
	*/
PHP_FUNCTION(fbsql_username)
{
	int  argc = ARG_COUNT(ht);
	zval	**argv[2];
	PHPFBLink*     phpLink = NULL;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	phpLink = phpfbGetLink(Z_LVAL_PP(argv[0]));
	if (phpLink == NULL) RETURN_FALSE;

	if (argc == 2)
	{
		convert_to_string_ex(argv[1]);
		if (phpLink->userName) efree(phpLink->userName);
		phpLink->userName = estrndup(Z_STRVAL_PP(argv[1]), Z_STRLEN_PP(argv[1]));
	}
	RETURN_STRING(phpLink->userName, 1);
}
/* }}} */


/* {{{ proto string fbsql_password(int link_identifier [, string password])
	*/
PHP_FUNCTION(fbsql_password)
{   
	int  argc = ARG_COUNT(ht);
	zval	**argv[1];
	PHPFBLink*     phpLink = NULL;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	phpLink = phpfbGetLink(Z_LVAL_PP(argv[0]));
	if (phpLink == NULL) RETURN_FALSE;

	if (argc == 2)
	{
		convert_to_string_ex(argv[1]);
		if (phpLink->userPassword) efree(phpLink->userPassword);
		phpLink->userPassword = estrndup(Z_STRVAL_PP(argv[1]), Z_STRLEN_PP(argv[1]));
	}
	RETURN_STRING(phpLink->userPassword, 1);
}
/* }}} */


/* {{{ proto int fbsql_select_db([string database_name [, int link_identifier]])   
	*/
PHP_FUNCTION(fbsql_select_db)
{
	int  argc = ARG_COUNT(ht);
	zval	**argv[2];
	char*          name = NULL;
	PHPFBLink*     phpLink = NULL;
	PHPFBDatabase* database = NULL;
	FBSQLLS_FETCH();

	name = FB_SQL_G(databaseName);

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc, &argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_string_ex(argv[0]);
		name = Z_STRVAL_PP(argv[0]);
	}

	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		phpLink = phpfbGetLink(Z_LVAL_PP(argv[1]));    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
	if (phpLink->execHandler == NULL)
	{
		int port = atoi(name);
		if (port == 0 || port > 64535) {
			php_error(E_WARNING,"Cannot connect to FBExec for database '%s'",name);
			php_error(E_WARNING,fbcehClassErrorMessage());
			RETURN_FALSE;
		}
	}

	database = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU,name, phpLink);
	if (database == NULL) RETURN_FALSE;

	phpLink->currentDatabase  = database;
	FB_SQL_G(linkIndex)       = phpLink->index;

	RETURN_LONG(database->index);
}
/* }}} */


/* {{{ proto int fbsql_change_user(string user, string password [, string database [, int link_identifier]]);
	*/
PHP_FUNCTION(fbsql_change_user)
{
	int             argc     = ARG_COUNT(ht);
	zval	**          argv[4];
	int             link;
	PHPFBLink*      phpLink;
	PHPFBDatabase * phpDatabase;
	char*           userName;
	char*           userPassword;
	char*           databaseName;
	FBSQLLS_FETCH();

	link = FB_SQL_G(linkIndex);
	databaseName = FB_SQL_G(databaseName);

	if ((argc < 2) || (argc > 4)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1],&argv[2],&argv[3])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	userName = (*argv[0])->value.str.val;

	convert_to_string_ex(argv[1]);
	userPassword = (*argv[1])->value.str.val;

	if (argc >= 3)
	{
		convert_to_string_ex(argv[2]);
		databaseName = (*argv[2])->value.str.val;
	}
	if (argc >= 4)
	{
		convert_to_long_ex(argv[3]);
		link = Z_LVAL_PP(argv[3]);
	}
	if ((phpLink = phpfbGetLink(link))==NULL) RETURN_FALSE;
	if ((phpDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, databaseName, phpLink)) == NULL) RETURN_FALSE;
	{
		char buffer[1024];
		sprintf(buffer,"SET AUTHORIZATION %s;",userName);

		phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU,buffer, phpDatabase);
		if (return_value->value.lval)
		{
			efree(phpLink->userName);
			phpLink->userName = estrdup(userName);
		}
	}
}
/* }}} */


/* {{{ proto int fbsql_create_db(string database_name [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_create_db)
{
	PHPFBLink* phpLink = NULL;
	int   i;
	int   argc = ARG_COUNT(ht);
	zval	**argv[2];
	int   link;
	char* databaseName;
	char* hostName;
	FBDatabaseStatus status;
	FBSQLLS_FETCH();

	link = FB_SQL_G(linkIndex);
	hostName = FB_SQL_G(hostName);

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	databaseName = (*argv[0])->value.str.val;

	if (argc == 2)
	{
		convert_to_long_ex(argv[1]);
		link = Z_LVAL_PP(argv[1]);
		phpLink = phpfbGetLink(link);
		if (phpLink == NULL) RETURN_FALSE;
	}

	if (phpLink == NULL)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
		if (phpLink == NULL) RETURN_FALSE;
	}
	status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
	if (status != FBUnknownStatus)
	{
		char* txt = "Unknown status";
		if      (status == FBStopped ) txt = "stopped";
		else if (status == FBStarting) txt = "starting";
		else if (status == FBRunning ) txt = "running";
		else if (status == FBStopping) txt = "stopping";
		else if (status == FBNoExec  ) txt = "no exec";
		php_error(E_WARNING, "Could not create %s@%s, database is %s",databaseName,hostName,txt);
		RETURN_FALSE;
	}
	if (!fbcehCreateDatabaseNamedWithOptions(phpLink->execHandler, databaseName, ""))
	{
		char* error = fbechErrorMessage(phpLink->execHandler);
		php_error(E_WARNING, "Could not create %s@%s. %s.",databaseName,hostName,error);
		RETURN_FALSE;
	}
	for (i=0; i < 20; i++)
	{
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
		if (status == FBRunning) break;
	}
	FB_SQL_G(linkIndex) = phpLink->index;
	if (status != FBRunning)
	{
		php_error(E_WARNING, "Database %s@%s created -- status unknown",databaseName,hostName);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int fbsql_drop_db(string database_name [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_drop_db)
{
	char* databaseName;
	int   argc     = ARG_COUNT(ht);
	zval	**argv[2];
	PHPFBLink* phpLink;
	FBDatabaseStatus status;
	int   i;
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1],&argv[2])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	databaseName = (*argv[0])->value.str.val;

	if (argc == 2)
	{
		convert_to_long_ex(argv[1]);
		phpLink = phpfbGetLink(Z_LVAL_PP(argv[1]));
		if (phpLink == NULL) RETURN_FALSE;
	}
	else
	{
      phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
    }

	if (phpLink == NULL)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
		if (phpLink) RETURN_FALSE;
	}
	status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
	if (status != FBStopped)
	{
		char* txt = "Unknown status";
		if      (status == FBStopped      ) txt = "stopped";
		else if (status == FBUnknownStatus) txt = "nonexisting";
		else if (status == FBStarting     ) txt = "starting";
		else if (status == FBRunning      ) txt = "running";
		else if (status == FBStopping     ) txt = "stopping";
		else if (status == FBNoExec       ) txt = "no exec";
		php_error(E_WARNING, "Could not drop %s@%s, database is %s.",databaseName,phpLink->hostName,txt);
		RETURN_FALSE;
	}

	if (! fbcehDeleteDatabaseNamed (phpLink->execHandler, databaseName))
	{
		char* error = fbechErrorMessage(phpLink->execHandler);
		php_error(E_WARNING, "Could not drop %s@%s. %s.",databaseName,phpLink->hostName,error);
		RETURN_FALSE;
	}
	for (i=0; i < 20; i++)
	{
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
		if (status == FBUnknownStatus) break;
	}
	if (status != FBUnknownStatus)
	{
		php_error(E_WARNING, "Database %s@%s dropped -- status unknown",databaseName,phpLink->hostName);
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int fbsql_stop_db(string database_name [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_stop_db)
{
	PHPFBDatabase*  phpDatabase;
	PHPFBLink*      phpLink      = NULL;
	FBDatabaseStatus status;
	char*            databaseName;

	int   argc     = ARG_COUNT(ht);
	zval	**argv[2];
	int   link;
	unsigned   i;
	char  name[1024];
	FBSQLLS_FETCH();

	link = FB_SQL_G(linkIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1],&argv[2])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	databaseName = (*argv[0])->value.str.val;

	if (argc == 2)
	{
		convert_to_long_ex(argv[1]);
		link = Z_LVAL_PP(argv[1]);
		phpLink = phpfbGetLink(link);
		if (phpLink == NULL) RETURN_FALSE;
	}
	if (phpLink == NULL)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
		if (phpLink == NULL) RETURN_FALSE;
	}

	phpDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, databaseName, phpLink);
	if (phpDatabase == NULL) RETURN_FALSE;

/*	printf("Stop db %x\n",phpDatabase->connection); */
	if (!fbcdcStopDatabase(phpDatabase->connection))
	{
		php_error(E_WARNING, "Cannot stop database %s@%s",databaseName,phpLink->hostName);
		RETURN_FALSE;
	}

	for (i=0; i < 20; i++)
	{
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
		if (status == FBStopped) break;
#ifdef PHP_WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	for (i=0; i < phpLink->databaseCount; i++) if (phpLink->databases[i] == phpDatabase) phpLink->databases[i] = NULL;

	for (i=0; i < phpDatabase->resultCount; i++) if (phpDatabase->results[i])
	{
		FB_SQL_G(resultCount)--;
		zend_list_delete(phpDatabase->results[i]->index);
	}
/*	printf("Database %X %d %d\n",phpDatabase,phpDatabase->index,phpDatabase->retainCount); */
	sprintf(name,"fbsql_%s@%s:%s",databaseName,phpLink->hostName, phpLink->userName);
	zend_list_delete(phpDatabase->index);
	FB_SQL_G(databaseCount)--;

	zend_hash_del(&EG(regular_list),name,strlen(name));

/*	printf("After list delete\n"); */
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int fbsql_start_db(string database_name [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_start_db)
{
	PHPFBLink* phpLink = NULL;
	int   i;
	int   argc = ARG_COUNT(ht);
	zval	**argv[2];
	char* databaseName;
	char* hostName;
	FBDatabaseStatus status;
	FBSQLLS_FETCH();

	hostName = FB_SQL_G(hostName);

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	databaseName = (*argv[0])->value.str.val;

	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		phpLink = phpfbGetLink(Z_LVAL_PP(argv[1]));    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
    }
	if (phpLink == NULL) RETURN_FALSE;
/*	printf("Start db at link %s@%s\n",phpLink->hostName,phpLink->userName); */
	status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
	if ((status != FBStopped) && (status != FBRunning) && (status != FBStarting))
	{
		char* txt = "Unknown status";
		if      (status == FBStopped ) txt = "stopped";
		else if (status == FBStarting) txt = "starting";
		else if (status == FBRunning ) txt = "running";
		else if (status == FBStopping) txt = "stopping";
		else if (status == FBNoExec  ) txt = "no exec";
		php_error(E_WARNING, "Could not start %s@%s, as database is %s.",databaseName,hostName,txt);
		RETURN_FALSE;
	}

	if (status == FBStopped)
	{
		if (!fbcehStartDatabaseNamed (phpLink->execHandler, databaseName))
		{
			char* error = fbechErrorMessage(phpLink->execHandler);
			php_error(E_WARNING, "Could not start %s@%s. %s.",databaseName,hostName,error);
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
		status = fbcehStatusForDatabaseNamed(phpLink->execHandler,databaseName);
		if (status == FBRunning) break;
	}
	if (status != FBRunning)
	{
		php_error(E_WARNING, "Database %s@%s started -- status unknown",databaseName,hostName);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


int mdOk(PHPFBDatabase* database, FBCMetaData* md)
{
	FBCDatabaseConnection* c = database->connection;
	int result = 1;
	FBSQLLS_FETCH();

	database->errorNo        = 0;
	if (database->errorText)
	{
		efree(database->errorText);
		database->errorText = NULL;
	}
	if (md == NULL)
	{
		database->errorNo  = 1;
		database->errorText  = estrdup("Connection was database server was lost");
		if (FB_SQL_G(generateWarnings)) php_error(E_WARNING, database->errorText);
		result = 0;
	}
	else if (fbcmdErrorsFound(md))
	{
		FBCErrorMetaData* emd = fbcdcErrorMetaData(c,md);
		char*             emg = fbcemdAllErrorMessages(emd);
		if (FB_SQL_G(generateWarnings))
		{
			if (emg)
				php_error(E_WARNING, emg);
			else
				php_error(E_WARNING,"No message");
		}
		database->errorText = emg;
		database->errorNo  = 1;
		fbcemdRelease(emd);
		result = 0;
	}
	return result;
}

PHPFBResult* phpfbQuery(INTERNAL_FUNCTION_PARAMETERS, char* sql, PHPFBDatabase* database)
{
	PHPFBLink*    link   = database->link;
	PHPFBResult*  result = NULL;
	FBCMetaData*   md, *meta;
	int            ok;
	char*          tp;
	char*          fh; 
	unsigned int   sR = 1, cR = 0;
	FBSQLLS_FETCH();

	meta     = fbcdcExecuteDirectSQL(database->connection,sql);

	if (fbcmdHasMetaDataArray(meta)) {
		sR = fbcmdMetaDataArrayCount(meta);
		md = (FBCMetaData*)fbcmdMetaDataAtIndex(meta, cR);
	}
	else
		md = meta;

	ok     = mdOk(database, md);
	tp     = fbcmdStatementType(md);

	return_value->value.lval = ok?-1:0;
	return_value->type       = IS_LONG;

/*	printf("Result kind %s\n",tp); */
	if (!ok || (tp[0] == 'C') || (tp[0] == 'R'))
	{
		unsigned i;
		for (i=0; i < database->resultCount; i++)
		{
			PHPFBResult* result = database->results[i];
			if (result)
			{
/*				printf("Delete from list %d %x",i,database->results[i],database->results[i]->index); */
				zend_list_delete(database->results[i]->index);
				FB_SQL_G(databaseCount)--;
				database->results[i] = NULL;
			}
		}
	}
	else if (tp[0] == 'I')
	{
		database->rowIndex = fbcmdRowIndex(md);
	}
	else if ((fh = fbcmdFetchHandle(md)) && (FB_SQL_G(resultCount) == FB_SQL_G(maxResults)))
	{
		FBCMetaData *md = fbcdcCancelFetch(database->connection,fh);
		fbcmdRelease(md);
		php_error(E_WARNING,"FrontBase result set limit %d exceeded",FB_SQL_G(maxResults));
	}
	else if (fh || (tp[0] == 'E'))
	{
		unsigned i;
		result = emalloc(sizeof(PHPFBResult));
		result->retainCount = 1;
		result->index       = zend_list_insert(result,le_result);
		result->link        = NULL;
		result->database    = phpfbRetainDatabase(database);
		result->connection  = database->connection;
		result->fetchHandle = fh;
		result->ResultmetaData    = meta;
		result->metaData    = md;
		result->rowHandler  = NULL;
		result->batchSize   = FB_SQL_G(batchSize);
		result->rowCount    = 0x7fffffff;
		result->columnCount = 0;
		result->rowIndex    = 0;
		result->columnIndex = 0;
		result->row         = NULL;
		result->array       = NULL;
		result->list        = NULL;
		result->selectResults = sR;
		result->currentResult = cR;
		FB_SQL_G(resultCount)++;
		for (i=0; i < database->resultCount; i++) if (database->results[i] == NULL) break;
		if (i == database->resultCount)
		{
			unsigned j;
			database->resultCount += 5;
			database->results     = realloc(database->results,sizeof(PHPFBResult*)*database->resultCount);
			for (j=i; j < database->resultCount; j++) database->results[j] = NULL;
		}
		database->results[i] = result;
		return_value->value.lval = result->index;

		if (tp[0] != 'E')
		{
			result->rowCount    = 0x7fffffff;
			result->columnCount = fbcmdColumnCount(md);
			result->fetchHandle = fh;
			result->batchSize   = FB_SQL_G(batchSize);
		}
		else 
		{
			char* r = fbcmdMessage(result->metaData);
			if ((result->list = fbcplParse(r)))
			{
				result->rowCount    = fbcplCount(result->list);
				result->columnCount = 7;
			}
		}
	}
	if (link) link->affectedRows = fbcmdRowCount(md);
	if (result == NULL) fbcmdRelease(md);
	return result;
}

/* {{{ proto int fbsql_query(string query [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_query)
{
	PHPFBDatabase*  phpDatabase;
	PHPFBLink*      phpLink = NULL;
	PHPFBResult*    phpResult; 
	char* sql;
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	FBSQLLS_FETCH();

	if ((argc < 1) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	sql = (*argv[0])->value.str.val;

	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		phpLink = phpfbGetLink(Z_LVAL_PP(argv[1]));    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
/*	printf("Query db at link %s@%s\n",phpLink->hostName,phpLink->userName); */

	phpDatabase = phpLink->currentDatabase;
	if (phpDatabase == NULL) RETURN_FALSE;

	phpResult = phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU,sql,phpDatabase);
	if (return_value->value.lval)
	{
		FB_SQL_G(linkIndex)                    = phpLink->index;
		if (phpResult) FB_SQL_G(resultIndex)  = phpResult->index;
	}
}
/* }}} */


/* {{{ proto int fbsql_db_query(string database_name, string query [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_db_query)
{
	PHPFBDatabase*  phpDatabase;
	PHPFBLink*      phpLink;
	PHPFBResult*    phpResult;
	char* sql;
	char* databaseName;
	int   argc     = ARG_COUNT(ht);
	zval	**argv[3];
	FBSQLLS_FETCH();

	if ((argc < 2) || (argc > 3)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1],&argv[2])==FAILURE) RETURN_FALSE;

	convert_to_string_ex(argv[0]);
	databaseName = (*argv[0])->value.str.val;

	convert_to_string_ex(argv[1]);
	sql = (*argv[1])->value.str.val;

	if (argc >= 3)
	{
		convert_to_long_ex(argv[2]);
		phpLink = phpfbGetLink((*argv[2])->value.lval);    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
/*	printf("Query db at link %s@%s\n",phpLink->hostName,phpLink->userName); */
	phpDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, databaseName, phpLink);
	if (phpDatabase == NULL) RETURN_FALSE;
 
	phpResult = phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU,sql,phpDatabase);
	if (return_value->value.lval)
	{
		FB_SQL_G(linkIndex)                   = phpLink->index;
		if (phpResult) FB_SQL_G(resultIndex)  = phpResult->index;
	}
}
/* }}} */


/* {{{ proto int fbsql_list_dbs([int link_identifier])
	*/
PHP_FUNCTION(fbsql_list_dbs)
{
	PHPFBLink*      phpLink = NULL;
	PHPFBResult*    phpResult;
	unsigned   i;
	int   argc = ARG_COUNT(ht);
	zval	**argv[1];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc, &argv[0])==FAILURE) RETURN_FALSE;
	if (argc == 1)
	{
		convert_to_long_ex(argv[0]);
		phpLink = phpfbGetLink((*argv[0])->value.lval);    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
	if (FB_SQL_G(resultCount) == FB_SQL_G(maxResults))
	{
		php_error(E_WARNING,"FrontBase result set limit %d exceeded",FB_SQL_G(maxResults));
		RETURN_FALSE
	}
	phpResult = emalloc(sizeof(PHPFBResult));
	phpResult->retainCount = 1;
	phpResult->index       = zend_list_insert(phpResult, le_result);
	phpResult->link        = phpfbRetainLink(phpLink);
	phpResult->database    = NULL;
	phpResult->connection  = NULL;
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
	FB_SQL_G(resultCount)++;
	for (i=0; i < phpLink->resultCount; i++) if (phpLink->results[i] == NULL) break;
	if (i == phpLink->resultCount)
	{
		unsigned j;
		phpLink->resultCount += 5;
		phpLink->results     = realloc(phpLink->results,sizeof(PHPFBResult*)* phpLink->resultCount);
		for (j=i; j < phpLink->resultCount ; j++)phpLink->results[j] = NULL;
	}
	phpLink->results[i] = phpResult;
	RETURN_LONG(phpResult->index);
	FB_SQL_G(resultIndex) = phpResult->index;
	FB_SQL_G(linkIndex) = phpLink->index;
}
/* }}} */


/* {{{ proto int fbsql_list_tables(string database, int [link_identifier]);
	*/
PHP_FUNCTION(fbsql_list_tables)
{
	PHPFBDatabase*  phpDatabase;
	PHPFBLink*      phpLink;
	PHPFBResult*    phpResult;
	char* sql = "select t0.\"table_name\"from information_schema.tables t0, information_schema.SCHEMATA t1 where t0.schema_pk = t1.schema_pk and t1.\"schema_name\" = current_schema;";
	char* databaseName  = NULL;
	int   argc     = ARG_COUNT(ht);
	zval	**argv[2];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_string_ex(argv[0]);
		databaseName = (*argv[0])->value.str.val;
	} 

	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		phpLink = phpfbGetLink(Z_LVAL_PP(argv[1]));    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
  
	if (databaseName == NULL)
	{
		phpLink->currentDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, FB_SQL_G(databaseName),phpLink);
	}
	else 
	{
		phpLink->currentDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, databaseName, phpLink);
	}
	phpDatabase = phpLink->currentDatabase;
	if (phpDatabase == NULL) RETURN_FALSE;

	phpResult = phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU,sql,phpDatabase);

	if (return_value->value.lval)
	{
		FB_SQL_G(linkIndex)                   = phpLink->index;
		if (phpResult) FB_SQL_G(resultIndex)  = phpResult->index;
	}
}
/* }}} */


/* {{{ proto int fbsql_list_fields(string database_name, string table_name [, int link_identifier])
	*/
PHP_FUNCTION(fbsql_list_fields)
{
	PHPFBDatabase*  phpDatabase;
	PHPFBLink*      phpLink;
	PHPFBResult*    phpResult;
	char             sql[1024];
	char* databaseName = NULL;
	char* tableName    = NULL;
	int   argc         = ARG_COUNT(ht);
	zval	**argv[2];
	FBSQLLS_FETCH();

	if ((argc < 2) || (argc > 3)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_string_ex(argv[0]);
		databaseName = (*argv[0])->value.str.val;
	} 

	if (argc >= 2)
	{
		convert_to_string_ex(argv[1]);
		tableName = (*argv[1])->value.str.val;
	} 

	if (argc >= 3)
	{
		convert_to_long_ex(argv[2]);
		phpLink = phpfbGetLink((*argv[2])->value.lval);    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;

	phpDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, databaseName,phpLink);
	phpLink->currentDatabase = phpDatabase;
	if (phpDatabase == NULL) RETURN_FALSE;

	sprintf(sql,"EXTRACT TABLE %s;",tableName);

	phpResult = phpfbQuery(INTERNAL_FUNCTION_PARAM_PASSTHRU,sql,phpDatabase);
	if (return_value->value.lval)
	{
		FB_SQL_G(linkIndex)                   = phpLink->index;
		if (phpResult) FB_SQL_G(resultIndex)  = phpResult->index;
	}
}
/* }}} */


/* {{{ proto string fbsql_error([int link_identifier])
	*/
PHP_FUNCTION(fbsql_error)
{
	PHPFBLink*     phpLink = NULL;
	PHPFBDatabase* phpDatabase;
	int   argc     = ARG_COUNT(ht);
	zval	**argv[1];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;
	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		phpLink = phpfbGetLink((*argv[0])->value.lval);    
	}
	else if (FB_SQL_G(linkIndex) != 0)
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
/*	printf("Query db at link %s@%s\n",phpLink->hostName,phpLink->userName); */
	phpDatabase = phpLink->currentDatabase;
	if ((phpDatabase == NULL) || (phpDatabase->errorText == NULL))  RETURN_FALSE;
	RETURN_STRING(phpDatabase->errorText, 1);
}
/* }}} */


/* {{{ proto int fbsql_errno([int link_identifier])
	*/
PHP_FUNCTION(fbsql_errno)
{
	PHPFBLink*     phpLink = NULL;
	PHPFBDatabase* phpDatabase;
	int   argc     = ARG_COUNT(ht);
	zval	**argv[1];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;
	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		phpLink = phpfbGetLink((*argv[0])->value.lval);    
	}
	else if (FB_SQL_G(linkIndex) != 0)
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;
/*	printf("Query db at link %s@%s\n",phpLink->hostName,phpLink->userName); */
	phpDatabase = phpLink->currentDatabase;
	if (phpDatabase == NULL) RETURN_FALSE;
	RETURN_LONG(phpDatabase->errorNo);
}
/* }}} */


/* {{{ proto bool fbsql_generate_warnings([int flag]);
	*/
PHP_FUNCTION(fbsql_warnings)
{
	int   argc     = ARG_COUNT(ht);
	zval	**argv[1];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;
	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		FB_SQL_G(generateWarnings) = (*argv[0])->value.lval != 0;
	}
	RETURN_BOOL(FB_SQL_G(generateWarnings));
}
/* }}} */


/* {{{ proto int fbsql_affected_rows([int link_identifier])
	*/
PHP_FUNCTION(fbsql_affected_rows)
{
	PHPFBLink*      phpLink;
	int   argc       = ARG_COUNT(ht);
	zval	**argv[1];
	int   link;
	FBSQLLS_FETCH();

	link = FB_SQL_G(linkIndex);

	if (argc > 1) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;

	if (argc == 1)
	{
		convert_to_string_ex(argv[0]);
		link = (*argv[0])->value.lval;
	}
	phpLink = phpfbGetLink(link);
	if (phpLink == NULL) RETURN_FALSE;
	RETURN_LONG(phpLink->affectedRows);
}
/* }}} */


/* {{{ proto int fbsql_insert_id([int link_identifier])
	*/
PHP_FUNCTION(fbsql_insert_id)
{
	PHPFBLink* phpLink;
	PHPFBDatabase* phpDatabase;
	int   argc     = ARG_COUNT(ht);
	zval	**argv[1];
	FBSQLLS_FETCH();

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc, &argv[0])==FAILURE) RETURN_FALSE;
	if (argc == 1)
	{
		convert_to_long_ex(argv[0]);
		phpLink = phpfbGetLink((*argv[0])->value.lval);    
	}
	else if (FB_SQL_G(linkIndex) == 0)
	{
		phpLink = phpfbConnect(INTERNAL_FUNCTION_PARAM_PASSTHRU,NULL,NULL,NULL,0);
	}
	else
	{
		phpLink = phpfbGetLink(FB_SQL_G(linkIndex));
	}
	if (phpLink == NULL) RETURN_FALSE;

	if (phpLink->currentDatabase == NULL)
	{
		phpLink->currentDatabase = phpfbSelectDB(INTERNAL_FUNCTION_PARAM_PASSTHRU, FB_SQL_G(databaseName),phpLink);
	}
	phpDatabase = phpLink->currentDatabase;
	if (phpDatabase == NULL) RETURN_FALSE;

	RETURN_LONG(phpDatabase->rowIndex);
	FB_SQL_G(linkIndex) = phpLink->index;
}
/* }}} */


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

void phpfbColumnAsString (PHPFBResult* result, int column, void* data ,int* length, char** value)
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
				phpfbestrdup("Unknown",length,value);
			else if (v == 0)
				phpfbestrdup("False",length,value);
			else
				phpfbestrdup("True",length,value);
		}
		break;
        
		case FB_PrimaryKey:
		case FB_Integer:
		{ 
			int   v = *((int*)data);
			char  b[128];
			sprintf(b,"%d",v);
			phpfbestrdup(b,length,value);
		}
		break;

		case FB_SmallInteger:
		{
			short v = *((short*)data);
			char  b[128];
			sprintf(b,"%d",v);
			phpfbestrdup(b,length,value);
		}
		break; 

		case FB_Float:
		case FB_Real:
		case FB_Double:
		case FB_Numeric:
		case FB_Decimal:
		{
			double v = *((double*)data);
			char  b[128];
			sprintf(b,"%f",v);
			phpfbestrdup(b,length,value);
		}
		break;

		case FB_Character:
		case FB_VCharacter:
		{
			char* v = (char*)data;
			phpfbestrdup(v,length,value);
		}
		break;

		case FB_Bit:
		case FB_VBit:
		{
			const FBCColumnMetaData* clmd  =  fbcmdColumnMetaDataAtIndex(md,column);
			struct bitValue
			{
				unsigned int   nBytes;
				unsigned char* bytes;
			};
			struct bitValue*  ptr = data;
			unsigned nBits = ptr->nBytes * 8;

			if (dtc == FB_Bit) nBits = fbcdmdLength(fbccmdDatatype(clmd));
			if (nBits %8 == 0)
			{
				unsigned i;
				unsigned int l = nBits / 8;
				*length = l + 5;
				if (value)
				{
					char*        r = emalloc(l*2+3+1);
					r[0] = 'X';
					r[1] = '\'';
					for (i = 0; i < nBits / 8; i++)
					{
						char c[4];
						sprintf(c,"%02x",ptr->bytes[i]);
						r[i*2+2] = c[0];
						r[i*2+3] = c[1];
					}
					r[i*2+2] = '\'';
					r[i*2+3] = 0;
					*value  = r;
				}
			}
			else
			{
				unsigned i;
				unsigned int l = nBits;
				*length = l + 5;
				if (value)
				{
					char*        r = emalloc(l*2+3+1);
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
					*value  = r;
				}
			}
		}
		break;

		case FB_Date:
		case FB_Time:
		case FB_TimeTZ:
		case FB_Timestamp:
		case FB_TimestampTZ:
		{
			char* v = (char*)data;
			phpfbestrdup(v,length,value);
		}
		break;

		case FB_YearMonth:
		{
			char b[128];
			int  v = *((unsigned int*)data);
			sprintf(b,"%d",v);
			phpfbestrdup(b,length,value);
		}
		break;

		case FB_DayTime:
		{
			char b[128];
			double seconds = *((double*)data);
			sprintf(b,"%f",seconds);
			phpfbestrdup(b,length,value);
		}
		break;

		case FB_CLOB:
		case FB_BLOB:
/*      {
			unsigned char* bytes = (unsigned char*)data;
			if (*bytes == '\1')
			{  /* Direct
				unsigned int   l   = *((unsigned int*)(bytes+1));
				unsigned char* ptr = *((unsigned char**)(bytes+5));
				unsigned int   i;
				mf(file,"%4d:",l);
				for (i=0; i < l; i++)
				{
					if (i)
					{
						if ((i % 32) == 0) 
							mf(file,"\n     %*d:",lw+4,i);
						else if ((i % 4) == 0) 
							mf(file,"  ");
					}
					mf(file,"%02x",*ptr++);
				}
			}
			else
			{
				mf(file,"%s",bytes+1);
			}
		}
		break;
*/
		default:
			php_error(E_WARNING,"Unimplemented type");
		break;
	}
}

void phpfbSqlResult (INTERNAL_FUNCTION_PARAMETERS, PHPFBResult* result, int rowIndex, int  columnIndex)
{
	void** row;
	if (result->list)
	{
		FBCPList* columns = (FBCPList*)fbcplValueForKey(result->list,"COLUMNS");
		FBCPList* column  = (FBCPList*)fbcplValueAtIndex(columns,result->rowIndex);
		if (columnIndex == 0)  
		{ /* Name */
			FBCPList* name = (FBCPList*)fbcplValueForKey(column,"NAME");
			RETURN_STRING((char *)fbcplString((FBCPList*)name), 1);
		}
		else if (columnIndex == 2)
		{ /* Length */
			FBCPList* name = (FBCPList*)fbcplValueForKey(column,"WIDTH");
			RETURN_STRING((char *)fbcplString((FBCPList*)name), 1);
		}
		else if (columnIndex == 1)
		{ /* Type */
			FBCPList* name = (FBCPList*)fbcplValueForKey(column,"DATATYPE");
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
		RETURN_STRING(fbaObjectAtIndex(result->array,rowIndex), 1);
	}
	else if (!phpfbFetchRow(result,rowIndex))
	{
		php_error(E_WARNING,"No such row %d in result set %d",rowIndex,rowIndex);
		RETURN_FALSE;
	}
	else if (columnIndex >= result->columnCount)
	{
		php_error(E_WARNING,"No such column %d in result set %d",columnIndex,rowIndex);
		RETURN_FALSE;
	}
	else
	{
		row = fbcrhRowAtIndex(result->rowHandler,rowIndex);
		if (row == NULL)
		{
			RETURN_FALSE;
		}
		else if (row[columnIndex])
		{
			phpfbColumnAsString(result,columnIndex,row[columnIndex],&return_value->value.str.len,&return_value->value.str.val);
			return_value->type = IS_STRING;
		}
		else
		{
			RETURN_NULL();
		}
	}
}
                       
/* {{{ proto int fbsql_result(int result, int row [, mixed field])
	*/
PHP_FUNCTION(fbsql_result)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[3];
	PHPFBResult*       result;
	int                 resultIndex;
	int                 rowIndex;
	int                 columnIndex;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 3)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1],&argv[2])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	rowIndex = result->rowIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		rowIndex = Z_LVAL_PP(argv[1]);
	}

	columnIndex  = result->columnIndex;
	if (argc == 3)
	{
		if (((*argv[2])->type == IS_STRING) && (result->metaData))
		{
			for (columnIndex =0; columnIndex < result->columnCount; columnIndex ++)
			{
				const FBCColumnMetaData* cmd = fbcmdColumnMetaDataAtIndex(result->metaData, columnIndex);
				const char*              lbl = fbccmdLabelName(cmd);
				if (strcmp((char*)lbl, (*argv[2])->value.str.val) == 0) break;
			}
			if (columnIndex == result->columnCount) RETURN_FALSE;
		}
		else
		{
			convert_to_long_ex(argv[2]);
			columnIndex = (*argv[2])->value.lval;
			if (columnIndex < 0)
			{
				php_error(E_WARNING,"Illegal column index - %d",columnIndex);
				RETURN_FALSE;
			}
		}
    }
   
	phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU,result,rowIndex,columnIndex);

	FB_SQL_G(resultIndex) = resultIndex;
	result->columnIndex++;
	if (result->columnIndex == result->columnCount)
	{
		result->rowIndex++;
		result->columnIndex = 0;
	}
}
/* }}} */


/* {{{ proto int fbsql_next_result(int result)
	*/
PHP_FUNCTION(fbsql_next_result)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[1];
	PHPFBResult*       result;
	int                 resultIndex;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;

    convert_to_long_ex(argv[0]);
	result = phpfbGetResult((*argv[0])->value.lval);
	if (result == NULL) RETURN_FALSE;

    result->currentResult++;
	if (result->currentResult < result->selectResults) {
        if (result->fetchHandle) {
			FBCMetaData *md = fbcdcCancelFetch(result->connection, result->fetchHandle);
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
		if (result->database->link) 
			result->database->link->affectedRows = fbcmdRowCount(result->metaData);

		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int fbsql_num_rows(int result)
	*/
PHP_FUNCTION(fbsql_num_rows)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[1];
	int                 resultIndex;
	PHPFBResult* result;
	int                 rowCount;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	if (result->array)
		rowCount = result->rowCount;
	else {
		rowCount = fbcmdRowCount(result->metaData);
		if (rowCount == -1)
		{
			phpfbFetchRow(result,0x7fffffff);
			rowCount = result->rowCount;
		}
	}
	RETURN_LONG(rowCount);
}
/* }}} */


/* {{{ proto int fbsql_num_fields(int result)
	*/
PHP_FUNCTION(fbsql_num_fields)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[1];
	int   resultIndex;
	PHPFBResult* result;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	RETURN_LONG(result->columnCount);
	FB_SQL_G(resultIndex) = resultIndex;
}
/* }}} */


/* {{{ proto array fbsql_fetch_row(int result)
	*/
PHP_FUNCTION(fbsql_fetch_row)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_NUM);
}
/* }}} */


/* {{{ proto object fbsql_fetch_assoc(int result)
	*/
PHP_FUNCTION(fbsql_fetch_assoc)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_ASSOC);
}
/* }}} */


/* {{{ proto object fbsql_fetch_object(int result [, int result_type])
	*/
PHP_FUNCTION(fbsql_fetch_object)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_ASSOC);
	if (return_value->type==IS_ARRAY)
	{
		return_value->type=IS_OBJECT;
		return_value->value.obj.properties = return_value->value.ht;
		return_value->value.obj.ce = &zend_standard_class_def;
	}
}
/* }}} */


/* {{{ proto array fbsql_fetch_array(int result [, int result_type])
   Fetch a result row as an array (associative, numeric or both)*/
PHP_FUNCTION(fbsql_fetch_array)
{
	php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FBSQL_NUM);
}
/* }}} */

static void php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 i;
	int                 resultIndex;
	PHPFBResult* result;
	int                 rowIndex;
	void**              row;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	rowIndex = result->rowIndex;
	if (argc == 2)
	{
		convert_to_long_ex(argv[1]);
		result_type = Z_LVAL_PP(argv[1]);
		if (((result_type & FBSQL_NUM) != FBSQL_NUM) && ((result_type & FBSQL_ASSOC) != FBSQL_ASSOC))
		{
			php_error(E_WARNING,"Illegal result type use FBSQL_NUM, FBSQL_ASSOC, or FBSQL_BOTH.");
			RETURN_FALSE;
		}
	}
	if (array_init(return_value)==FAILURE)
	{
		RETURN_FALSE;
	}
	if (result->fetchHandle == NULL)
	{
		unsigned int c = 0;
		if (result->array == NULL)
		{
			RETURN_FALSE;
		}
		if (result->rowIndex >= result->rowCount)
		{
			RETURN_FALSE;
		}
		if (result_type & FBSQL_NUM)
		{
			add_index_string(return_value,0,estrdup(fbaObjectAtIndex(result->array,result->rowIndex)),c);
			c = 1;
		}
		if (result_type & FBSQL_ASSOC)
		{
			add_assoc_string(return_value, "Database", estrdup(fbaObjectAtIndex(result->array,result->rowIndex)), c);
		}
	}
	else {
		if (result->rowCount == 0) {
			RETURN_FALSE;
		}
		if (result->rowCount == 0x7fffffff)
		{
			if (!phpfbFetchRow(result,result->rowIndex)) {
				RETURN_FALSE;
			}
		}
		row = fbcrhRowAtIndex(result->rowHandler,rowIndex);
		if (row == NULL)
		{
			RETURN_FALSE;
		}
		for (i=0; i < result->columnCount; i++)
		{
			if (row[i])
			{
				char*        value;
				unsigned int length;
				unsigned int c = 0;
				phpfbColumnAsString(result,i,row[i],&length,&value);
				if (result_type & FBSQL_NUM)
				{
					add_index_stringl(return_value,i,value,length,c);
					c = 1;
				}
				if (result_type & FBSQL_ASSOC)
				{
					char* key = (char*)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, i));
					add_assoc_stringl(return_value,key, value, length, c);
				}
			}
			else
			{
				if (result_type & FBSQL_NUM)
				{
					add_index_unset(return_value,i);
				}
				if (result_type & FBSQL_ASSOC)
				{
					char* key = (char*)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, i));
					add_assoc_unset(return_value,key);
				}
			}
		}
	}
	result->rowIndex    = result->rowIndex+1;
	result->columnIndex = 0;
}


/* {{{ proto int fbsql_data_seek(int result, int row_number)
	*/
PHP_FUNCTION(fbsql_data_seek)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult*       result;
	int                 rowIndex;
	FBSQLLS_FETCH();

	if (argc != 2) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	convert_to_long_ex(argv[0]);
	resultIndex = (*argv[0])->value.lval;
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	convert_to_long_ex(argv[1]);
	rowIndex = Z_LVAL_PP(argv[1]);
	if (rowIndex < 0)
	{
		php_error(E_WARNING,"Illegal index",resultIndex+1);
		RETURN_FALSE;
	}

	if (result->rowCount == 0x7fffffff) phpfbFetchRow(result,rowIndex);
	if (rowIndex > result->rowCount) RETURN_FALSE;
	result->rowIndex = rowIndex;

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto array fbsql_fetch_lengths([int result])
	*/
PHP_FUNCTION(fbsql_fetch_lengths)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 i;
	int                 resultIndex;
	PHPFBResult* result;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

   
	if (result->row == NULL) RETURN_FALSE;
	if (array_init(return_value)==FAILURE) RETURN_FALSE;
	for (i=0; i < result->columnCount; i++)
	{
		unsigned  length = 0;
		if (result->row[i]) phpfbColumnAsString(result,i, result->row[i],&length,NULL);
		add_index_long(return_value, i, length);
	}
	FB_SQL_G(resultIndex) = resultIndex;
}
/* }}} */


/* {{{ proto object fbsql_fetch_field(int result [, int field_offset])
	*/
PHP_FUNCTION(fbsql_fetch_field)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
		if (column < 0)
		{
			php_error(E_WARNING,"Illegal index",resultIndex+1);
			RETURN_FALSE;
		}
		if (column >= result->columnCount)
		{
			php_error(E_WARNING,"%d no such column in result",column);
			RETURN_FALSE;
		}
	}
	if (object_init(return_value)==FAILURE)
	{
		RETURN_FALSE;
	}
	add_property_string(return_value, "name",       (char*)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, column)),1);
	add_property_string(return_value, "table",      (char*)fbccmdTableName(fbcmdColumnMetaDataAtIndex(result->metaData,column)),1);
	add_property_long(return_value,   "max_length", fbcdmdLength(fbccmdDatatype(fbcmdColumnMetaDataAtIndex(result->metaData,column))));
	add_property_string(return_value, "type",       (char*)fbcdmdDatatypeString(fbcmdDatatypeMetaDataAtIndex(result->metaData, column)),1);
	add_property_long(return_value,   "not_null",   !fbccmdIsNullable(fbcmdColumnMetaDataAtIndex(result->metaData, column)));
/*	Remember to add the rest */
/*	add_property_long(return_value, "primary_key",IS_PRI_KEY(fbsql_field->flags)?1:0); */
/*	add_property_long(return_value, "multiple_key",(fbsql_field->flags&MULTIPLE_KEY_FLAG?1:0)); */
/*	add_property_long(return_value, "unique_key",(fbsql_field->flags&UNIQUE_KEY_FLAG?1:0)); */
/*	add_property_long(return_value, "numeric",IS_NUM(fbsql_field->type)?1:0); */
/*	add_property_long(return_value, "blob",IS_BLOB(fbsql_field->flags)?1:0); */
/*	add_property_long(return_value, "unsigned",(fbsql_field->flags&UNSIGNED_FLAG?1:0)); */
/*	add_property_long(return_value, "zerofill",(fbsql_field->flags&ZEROFILL_FLAG?1:0)); */
}
/* }}} */


/* {{{ proto int fbsql_field_seek(int result, int field_offset)
	*/
PHP_FUNCTION(fbsql_field_seek)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc>=2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
	}
	if (column < 0)
	{
		php_error(E_WARNING,"Illegal column index %d",column);
		RETURN_FALSE;
	}
	if (column >= result->columnCount)
	{
		php_error(E_WARNING,"No such column %d in result %d",column,resultIndex);
		RETURN_FALSE;
	}

	FB_SQL_G(resultIndex) = resultIndex;
	result->columnIndex = column;
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string fbsql_field_name(int result, int field_index)
	*/
PHP_FUNCTION(fbsql_field_name)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
		if (column < 0)
		{
			php_error(E_WARNING,"Illegal index",resultIndex+1);
			RETURN_FALSE;
		}
		if (column >= result->columnCount)
		{
			php_error(E_WARNING,"%d no such row in result",column);
			RETURN_FALSE;
		}
	}
	if (result->list)
	{
		phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU,result,result->rowIndex,0);
	}
	else if (result->metaData)
	{
		RETURN_STRING((char *)fbccmdLabelName(fbcmdColumnMetaDataAtIndex(result->metaData, column)), 1);
		FB_SQL_G(resultIndex) = resultIndex;
		result->columnIndex = column;
	}
}
/* }}} */


/* {{{ proto string fbsql_field_table(int result, int field_index)
	*/
PHP_FUNCTION(fbsql_field_table)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
		if (column < 0)
		{
			php_error(E_WARNING,"Illegal index",resultIndex+1);
			RETURN_FALSE;
		}
		if (column >= result->columnCount)
		{
			php_error(E_WARNING,"%d no such row in result",column);
			RETURN_FALSE;
		}
	}
	RETURN_STRING((char *)fbccmdTableName(fbcmdColumnMetaDataAtIndex(result->metaData,column)), 1);
}
/* }}} */


/* {{{ proto string fbsql_field_lene(int result, int field_index)
	*/
PHP_FUNCTION(fbsql_field_len)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
		if (column < 0)
		{
			php_error(E_WARNING,"Illegal index",resultIndex+1);
			RETURN_FALSE;
		}
		if (column >= result->columnCount)
		{
			php_error(E_WARNING,"%d no such row in result",column);
			RETURN_FALSE;
		}
	}
	if (result->list)
	{
		phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU,result,result->rowIndex,2);
	}
	else if (result->metaData)
	{
		unsigned int length = fbcdmdLength(fbccmdDatatype(fbcmdColumnMetaDataAtIndex(result->metaData,column)));
		char         buffer[50];
		sprintf(buffer,"%d",length);
		RETURN_STRING(buffer, 1);
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string fbsql_field_type(int result, int field_index)
	*/
PHP_FUNCTION(fbsql_field_type)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
		if (column < 0)
		{
			php_error(E_WARNING,"Illegal index",resultIndex+1);
			RETURN_FALSE;
		}
		if (column >= result->columnCount)
		{
			php_error(E_WARNING,"%d no such column in result",column);
			RETURN_FALSE;
		}
	}
	if (result->list)
	{
		phpfbSqlResult(INTERNAL_FUNCTION_PARAM_PASSTHRU,result,result->rowIndex,1);
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


/* {{{ proto string string fbsql_field_flags(int result[, int field_index])
	*/
PHP_FUNCTION(fbsql_field_flags)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[2];
	int                 resultIndex;
	PHPFBResult* result;
	int                 column;
	char buf[512];
	int  len;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 2)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0],&argv[1])==FAILURE) RETURN_FALSE;

	if (argc >= 1)
	{
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;

	column = result->columnIndex;
	if (argc >= 2)
	{
		convert_to_long_ex(argv[1]);
		column = Z_LVAL_PP(argv[1]);
		if (column < 0)
		{
			php_error(E_WARNING,"Illegal index",resultIndex+1);
			RETURN_FALSE;
		}
		if (column >= result->columnCount)
		{
			php_error(E_WARNING,"%d no such column in result",column);
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


/* {{{ proto int fbsql_free_result(int result)
	*/
PHP_FUNCTION(fbsql_free_result)
{
	int   argc    = ARG_COUNT(ht);
	zval	**argv[1];
	int           resultIndex;
	PHPFBResult* result;
	FBSQLLS_FETCH();

	resultIndex = FB_SQL_G(resultIndex);

	if ((argc < 0) || (argc > 1)) WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(argc,&argv[0])==FAILURE) RETURN_FALSE;

	if (argc >= 1) {
		convert_to_long_ex(argv[0]);
		resultIndex = (*argv[0])->value.lval;
	}
	result = phpfbGetResult(resultIndex);
	if (result == NULL) RETURN_FALSE;
	zend_list_delete(result->index);
	FB_SQL_G(resultCount)--;
	FB_SQL_G(resultIndex) = 0;
	RETURN_TRUE;
}
/* }}} */

#endif
