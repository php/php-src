/*
** 2006 June 7
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used to dynamically load extensions into
** the SQLite library.
*/
#ifndef SQLITE_OMIT_LOAD_EXTENSION

#define SQLITE_CORE 1  /* Disable the API redefinition in sqlite3ext.h */
#include "sqlite3ext.h"
#include "sqliteInt.h"
#include "os.h"
#include <string.h>
#include <ctype.h>

/*
** Some API routines are omitted when various features are
** excluded from a build of SQLite.  Substitute a NULL pointer
** for any missing APIs.
*/
#ifndef SQLITE_ENABLE_COLUMN_METADATA
# define sqlite3_column_database_name   0
# define sqlite3_column_database_name16 0
# define sqlite3_column_table_name      0
# define sqlite3_column_table_name16    0
# define sqlite3_column_origin_name     0
# define sqlite3_column_origin_name16   0
# define sqlite3_table_column_metadata  0
#endif

#ifdef SQLITE_OMIT_AUTHORIZATION
# define sqlite3_set_authorizer     0
#endif

#ifdef SQLITE_OMIT_UTF16
# define sqlite3_bind_text16        0
# define sqlite3_collation_needed16 0
# define sqlite3_column_decltype16  0
# define sqlite3_column_name16      0
# define sqlite3_column_text16      0
# define sqlite3_complete16         0
# define sqlite3_create_collation16 0
# define sqlite3_create_function16  0
# define sqlite3_errmsg16           0
# define sqlite3_open16             0
# define sqlite3_prepare16          0
# define sqlite3_result_error16     0
# define sqlite3_result_text16      0
# define sqlite3_result_text16be    0
# define sqlite3_result_text16le    0
# define sqlite3_value_text16       0
# define sqlite3_value_text16be     0
# define sqlite3_value_text16le     0
#endif

#ifdef SQLITE_OMIT_COMPLETE
# define sqlite3_complete 0
# define sqlite3_complete16 0
#endif

#ifdef SQLITE_OMIT_PROGRESS_CALLBACK
# define sqlite3_progress_handler 0
#endif

#ifdef SQLITE_OMIT_VIRTUALTABLE
# define sqlite3_create_module 0
# define sqlite3_declare_vtab 0
#endif

#ifdef SQLITE_OMIT_SHARED_CACHE
# define sqlite3_enable_shared_cache 0
#endif

#ifdef SQLITE_OMIT_TRACE
# define sqlite3_profile       0
# define sqlite3_trace         0
#endif

#ifdef SQLITE_OMIT_GET_TABLE
# define sqlite3_free_table    0
# define sqlite3_get_table     0
#endif

/*
** The following structure contains pointers to all SQLite API routines.
** A pointer to this structure is passed into extensions when they are
** loaded so that the extension can make calls back into the SQLite
** library.
**
** When adding new APIs, add them to the bottom of this structure
** in order to preserve backwards compatibility.
**
** Extensions that use newer APIs should first call the
** sqlite3_libversion_number() to make sure that the API they
** intend to use is supported by the library.  Extensions should
** also check to make sure that the pointer to the function is
** not NULL before calling it.
*/
const sqlite3_api_routines sqlite3_apis = {
  sqlite3_aggregate_context,
  sqlite3_aggregate_count,
  sqlite3_bind_blob,
  sqlite3_bind_double,
  sqlite3_bind_int,
  sqlite3_bind_int64,
  sqlite3_bind_null,
  sqlite3_bind_parameter_count,
  sqlite3_bind_parameter_index,
  sqlite3_bind_parameter_name,
  sqlite3_bind_text,
  sqlite3_bind_text16,
  sqlite3_bind_value,
  sqlite3_busy_handler,
  sqlite3_busy_timeout,
  sqlite3_changes,
  sqlite3_close,
  sqlite3_collation_needed,
  sqlite3_collation_needed16,
  sqlite3_column_blob,
  sqlite3_column_bytes,
  sqlite3_column_bytes16,
  sqlite3_column_count,
  sqlite3_column_database_name,
  sqlite3_column_database_name16,
  sqlite3_column_decltype,
  sqlite3_column_decltype16,
  sqlite3_column_double,
  sqlite3_column_int,
  sqlite3_column_int64,
  sqlite3_column_name,
  sqlite3_column_name16,
  sqlite3_column_origin_name,
  sqlite3_column_origin_name16,
  sqlite3_column_table_name,
  sqlite3_column_table_name16,
  sqlite3_column_text,
  sqlite3_column_text16,
  sqlite3_column_type,
  sqlite3_column_value,
  sqlite3_commit_hook,
  sqlite3_complete,
  sqlite3_complete16,
  sqlite3_create_collation,
  sqlite3_create_collation16,
  sqlite3_create_function,
  sqlite3_create_function16,
  sqlite3_create_module,
  sqlite3_data_count,
  sqlite3_db_handle,
  sqlite3_declare_vtab,
  sqlite3_enable_shared_cache,
  sqlite3_errcode,
  sqlite3_errmsg,
  sqlite3_errmsg16,
  sqlite3_exec,
  sqlite3_expired,
  sqlite3_finalize,
  sqlite3_free,
  sqlite3_free_table,
  sqlite3_get_autocommit,
  sqlite3_get_auxdata,
  sqlite3_get_table,
  0,     /* Was sqlite3_global_recover(), but that function is deprecated */
  sqlite3_interrupt,
  sqlite3_last_insert_rowid,
  sqlite3_libversion,
  sqlite3_libversion_number,
  sqlite3_malloc,
  sqlite3_mprintf,
  sqlite3_open,
  sqlite3_open16,
  sqlite3_prepare,
  sqlite3_prepare16,
  sqlite3_profile,
  sqlite3_progress_handler,
  sqlite3_realloc,
  sqlite3_reset,
  sqlite3_result_blob,
  sqlite3_result_double,
  sqlite3_result_error,
  sqlite3_result_error16,
  sqlite3_result_int,
  sqlite3_result_int64,
  sqlite3_result_null,
  sqlite3_result_text,
  sqlite3_result_text16,
  sqlite3_result_text16be,
  sqlite3_result_text16le,
  sqlite3_result_value,
  sqlite3_rollback_hook,
  sqlite3_set_authorizer,
  sqlite3_set_auxdata,
  sqlite3_snprintf,
  sqlite3_step,
  sqlite3_table_column_metadata,
  sqlite3_thread_cleanup,
  sqlite3_total_changes,
  sqlite3_trace,
  sqlite3_transfer_bindings,
  sqlite3_update_hook,
  sqlite3_user_data,
  sqlite3_value_blob,
  sqlite3_value_bytes,
  sqlite3_value_bytes16,
  sqlite3_value_double,
  sqlite3_value_int,
  sqlite3_value_int64,
  sqlite3_value_numeric_type,
  sqlite3_value_text,
  sqlite3_value_text16,
  sqlite3_value_text16be,
  sqlite3_value_text16le,
  sqlite3_value_type,
  sqlite3_vmprintf,
  /*
  ** The original API set ends here.  All extensions can call any
  ** of the APIs above provided that the pointer is not NULL.  But
  ** before calling APIs that follow, extension should check the
  ** sqlite3_libversion_number() to make sure they are dealing with
  ** a library that is new enough to support that API.
  *************************************************************************
  */
  sqlite3_overload_function,
};

/*
** Attempt to load an SQLite extension library contained in the file
** zFile.  The entry point is zProc.  zProc may be 0 in which case a
** default entry point name (sqlite3_extension_init) is used.  Use
** of the default name is recommended.
**
** Return SQLITE_OK on success and SQLITE_ERROR if something goes wrong.
**
** If an error occurs and pzErrMsg is not 0, then fill *pzErrMsg with 
** error message text.  The calling function should free this memory
** by calling sqlite3_free().
*/
int sqlite3_load_extension(
  sqlite3 *db,          /* Load the extension into this database connection */
  const char *zFile,    /* Name of the shared library containing extension */
  const char *zProc,    /* Entry point.  Use "sqlite3_extension_init" if 0 */
  char **pzErrMsg       /* Put error message here if not 0 */
){
  void *handle;
  int (*xInit)(sqlite3*,char**,const sqlite3_api_routines*);
  char *zErrmsg = 0;
  void **aHandle;

  /* Ticket #1863.  To avoid a creating security problems for older
  ** applications that relink against newer versions of SQLite, the
  ** ability to run load_extension is turned off by default.  One
  ** must call sqlite3_enable_load_extension() to turn on extension
  ** loading.  Otherwise you get the following error.
  */
  if( (db->flags & SQLITE_LoadExtension)==0 ){
    if( pzErrMsg ){
      *pzErrMsg = sqlite3_mprintf("not authorized");
    }
    return SQLITE_ERROR;
  }

  if( zProc==0 ){
    zProc = "sqlite3_extension_init";
  }

  handle = sqlite3OsDlopen(zFile);
  if( handle==0 ){
    if( pzErrMsg ){
      *pzErrMsg = sqlite3_mprintf("unable to open shared library [%s]", zFile);
    }
    return SQLITE_ERROR;
  }
  xInit = (int(*)(sqlite3*,char**,const sqlite3_api_routines*))
                   sqlite3OsDlsym(handle, zProc);
  if( xInit==0 ){
    if( pzErrMsg ){
       *pzErrMsg = sqlite3_mprintf("no entry point [%s] in shared library [%s]",
                                   zProc, zFile);
    }
    sqlite3OsDlclose(handle);
    return SQLITE_ERROR;
  }else if( xInit(db, &zErrmsg, &sqlite3_apis) ){
    if( pzErrMsg ){
      *pzErrMsg = sqlite3_mprintf("error during initialization: %s", zErrmsg);
    }
    sqlite3_free(zErrmsg);
    sqlite3OsDlclose(handle);
    return SQLITE_ERROR;
  }

  /* Append the new shared library handle to the db->aExtension array. */
  db->nExtension++;
  aHandle = sqliteMalloc(sizeof(handle)*db->nExtension);
  if( aHandle==0 ){
    return SQLITE_NOMEM;
  }
  if( db->nExtension>0 ){
    memcpy(aHandle, db->aExtension, sizeof(handle)*(db->nExtension-1));
  }
  sqliteFree(db->aExtension);
  db->aExtension = aHandle;

  db->aExtension[db->nExtension-1] = handle;
  return SQLITE_OK;
}

/*
** Call this routine when the database connection is closing in order
** to clean up loaded extensions
*/
void sqlite3CloseExtensions(sqlite3 *db){
  int i;
  for(i=0; i<db->nExtension; i++){
    sqlite3OsDlclose(db->aExtension[i]);
  }
  sqliteFree(db->aExtension);
}

/*
** Enable or disable extension loading.  Extension loading is disabled by
** default so as not to open security holes in older applications.
*/
int sqlite3_enable_load_extension(sqlite3 *db, int onoff){
  if( onoff ){
    db->flags |= SQLITE_LoadExtension;
  }else{
    db->flags &= ~SQLITE_LoadExtension;
  }
  return SQLITE_OK;
}

/*
** A list of automatically loaded extensions.
**
** This list is shared across threads, so be sure to hold the
** mutex while accessing or changing it.
*/
static int nAutoExtension = 0;
static void **aAutoExtension = 0;


/*
** Register a statically linked extension that is automatically
** loaded by every new database connection.
*/
int sqlite3_auto_extension(void *xInit){
  int i;
  int rc = SQLITE_OK;
  sqlite3OsEnterMutex();
  for(i=0; i<nAutoExtension; i++){
    if( aAutoExtension[i]==xInit ) break;
  }
  if( i==nAutoExtension ){
    nAutoExtension++;
    aAutoExtension = sqlite3Realloc( aAutoExtension,
                                     nAutoExtension*sizeof(aAutoExtension[0]) );
    if( aAutoExtension==0 ){
      nAutoExtension = 0;
      rc = SQLITE_NOMEM;
    }else{
      aAutoExtension[nAutoExtension-1] = xInit;
    }
  }
  sqlite3OsLeaveMutex();
  assert( (rc&0xff)==rc );
  return rc;
}

/*
** Reset the automatic extension loading mechanism.
*/
void sqlite3_reset_auto_extension(void){
  sqlite3OsEnterMutex();
  sqliteFree(aAutoExtension);
  aAutoExtension = 0;
  nAutoExtension = 0;
  sqlite3OsLeaveMutex();
}

/*
** Load all automatic extensions.
*/
int sqlite3AutoLoadExtensions(sqlite3 *db){
  int i;
  int go = 1;
  int rc = SQLITE_OK;
  int (*xInit)(sqlite3*,char**,const sqlite3_api_routines*);

  if( nAutoExtension==0 ){
    /* Common case: early out without every having to acquire a mutex */
    return SQLITE_OK;
  }
  for(i=0; go; i++){
    char *zErrmsg = 0;
    sqlite3OsEnterMutex();
    if( i>=nAutoExtension ){
      xInit = 0;
      go = 0;
    }else{
      xInit = (int(*)(sqlite3*,char**,const sqlite3_api_routines*))
              aAutoExtension[i];
    }
    sqlite3OsLeaveMutex();
    if( xInit && xInit(db, &zErrmsg, &sqlite3_apis) ){
      sqlite3Error(db, SQLITE_ERROR,
            "automatic extension loading failed: %s", zErrmsg);
      go = 0;
      rc = SQLITE_ERROR;
    }
  }
  return rc;
}

#endif /* SQLITE_OMIT_LOAD_EXTENSION */
