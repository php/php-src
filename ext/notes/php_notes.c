/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Brad Atkins <brad@youreshop.com>                             |
   +----------------------------------------------------------------------+
*/
 
/* $Id$ */

#ifdef COMPILE_DL_NOTES
#define HAVE_NOTES 1
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "php_notes.h"
#include "ext/standard/info.h"

#ifdef PHP_WIN32
#include <global.h>
#else
#include <notes_global.h>
#endif

#include <nsfdb.h>
#include <nsfdata.h>
#include <osmisc.h>
#include <miscerr.h>
#include <nif.h>
#include <nsfnote.h>
#include <editods.h>
#include <osmem.h>
#include <stdnames.h>
#include <nsfsearc.h>
#include <colorid.h>
#include <names.h>
#include <stdnames.h>
#include <nsferr.h>

/* notes_unread */

#include <idtable.h>
#include <kfm.h>

/* notes_header_info, notes_body */

#include <malloc.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <globerr.h>

#ifdef PHP_WIN32
#include <mail.h>
#else
#include <notes_mail.h>
#endif

#include <mailserv.h>
#include <nsf.h>
#include <osfile.h>                     /* OSPathNetConstruct */
#include <osenv.h>                      /* OSGetEnvironmentString */

/* notes_nav_create */
#include <vmods.h>

/* notes_search */

#include <ft.h>
#include <textlist.h>

/* notes_list */

#define  STRING_LENGTH  256

function_entry notes_functions[] = {
	PHP_FE(notes_create_db,										NULL)
	PHP_FE(notes_drop_db,										NULL)
	PHP_FE(notes_version,										NULL)
	PHP_FE(notes_create_note,									NULL)
	PHP_FE(notes_unread,										NULL)
	PHP_FE(notes_mark_read,										NULL)
	PHP_FE(notes_mark_unread,									NULL)
	PHP_FE(notes_header_info,									NULL)
	PHP_FE(notes_body,											NULL)
	PHP_FE(notes_find_note,										NULL)
	PHP_FE(notes_nav_create,									NULL)
	PHP_FE(notes_search,										NULL)
	PHP_FE(notes_copy_db,										NULL)
	PHP_FE(notes_list_msgs,										NULL)
	{NULL, NULL, NULL}
};

zend_module_entry notes_module_entry = {
	STANDARD_MODULE_HEADER,
	"notes", 
	notes_functions, 
	PHP_MINIT(notes), 
	PHP_MSHUTDOWN(notes), 
	PHP_RINIT(notes), 
	PHP_RSHUTDOWN(notes), 
	PHP_MINFO(notes), 
        NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_NOTES
ZEND_GET_MODULE(notes)
#endif


PHP_MINIT_FUNCTION(notes)
{
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(notes)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(notes)
{
	return SUCCESS;
}


PHP_RINIT_FUNCTION(notes)
{

	return SUCCESS;
}


PHP_MINFO_FUNCTION(notes)
{

	php_info_print_table_start();
	php_info_print_table_row(2, "Lotus Notes Database Support", "enabled");
	php_info_print_table_end();
}

/* {{{ proto bool notes_create_db(string database_name)
   Creates a Lotus Notes database */
PHP_FUNCTION(notes_create_db)
{

	int argc;
	pval *argv[1];
	STATUS error;
	pval **db;
	char error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(1, &db)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);

	error = NotesInitExtended( argc, (char **) argv );

	if (error) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	if (error = NSFDbCreate (Z_STRVAL_PP(db), (USHORT) DBCLASS_NOTEFILE, FALSE)) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to create database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}


	NotesTerm();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool notes_drop_db(string database_name)
   Drops a Lotus Notes database */
PHP_FUNCTION(notes_drop_db)
{

	int argc;
	pval *argv[1];
	STATUS error;
	pval **db;
	char error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(1, &db)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);

	error = NotesInitExtended( argc, (char **) argv );

	if (error) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	if (error = NSFDbDelete (Z_STRVAL_PP(db))) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to delete database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}


	NotesTerm();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string notes_version(string database_name)
   Gets the Lotus Notes version */
PHP_FUNCTION(notes_version)
{

	int argc;
	pval *argv[1];
	STATUS error;
	DBHANDLE db_handle;
	WORD wbuild;
	char error_string[200];
	pval **db;

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(1, &db)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);

	error = NotesInitExtended(argc, (char **) argv);

	if (error) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	if (error = NSFDbOpen(Z_STRVAL_PP(db), &db_handle)) {
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	if (error = NSFDbGetBuildVersion(db_handle, &wbuild)) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to get Lotus Notes version: %s", error_string);
		NSFDbClose(db_handle);
		NotesTerm();
		RETURN_FALSE;
	}

	NSFDbClose(db_handle);
	NotesTerm();

	if(wbuild > 0 && wbuild < 82) {
		RETURN_DOUBLE(1.0);
	} else if (wbuild > 81 && wbuild < 94) {
		RETURN_DOUBLE(2.0);
	} else if (wbuild > 93 && wbuild < 119) {
		RETURN_DOUBLE(3.0);
	} else if (wbuild > 118 && wbuild < 137) {
		RETURN_DOUBLE(4.0);
	} else if (wbuild == 138) {
		RETURN_DOUBLE(4.1);
	} else if (wbuild > 139 && wbuild < 146) {
		RETURN_DOUBLE(4.5);
	} else if (wbuild == 147) {
		RETURN_DOUBLE(4.6);
	} else {
		RETURN_DOUBLE(5.0);
	}
}
/* }}} */

/* {{{ proto string notes_create_note(string database_name, string form_name)
   Creates a note using form form_name */
PHP_FUNCTION(notes_create_note)
{

	int argc;
	pval *argv[2];
	STATUS error;
	DBHANDLE db_handle;
	NOTEHANDLE note_handle;
	char		error_string[200];
	pval **db;
	pval **form_name;

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(2, &db, &form_name)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(form_name);

	error = NotesInitExtended( argc, (char **) argv );

	if (error) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	if (error = NSFDbOpen(Z_STRVAL_PP(db), &db_handle)) {
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

    if (error = NSFNoteCreate (db_handle, &note_handle)) {

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to create note: error_string");
		NSFDbClose( db_handle );
		RETURN_FALSE;
    }

    /* Append the form name item to the new note */
    if (error = NSFItemSetText(note_handle, FIELD_FORM, Z_STRVAL_PP(form_name), MAXWORD)) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to use the form: %s", error_string);
        NSFNoteClose (note_handle);
        NSFDbClose( db_handle );
		RETURN_FALSE;
    }

    if (error = NSFNoteUpdate (note_handle, 0)) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to save the note: %s", error_string);
        NSFNoteClose (note_handle);
		NSFDbClose(db_handle);
        RETURN_FALSE;
    }

	error = NSFNoteClose (note_handle);

	NSFDbClose(db_handle);
	NotesTerm();

	RETURN_TRUE;
}
/* }}} */

/* Declarations for notes_mark_read and notes_mark_unread */

#define ACTION_COUNT_MAX   (16)


/* Entries in the action table */

typedef struct {
   BOOL     AddFlag;      /* TRUE: Add, FALSE: Remove */
   NOTEID   NoteID;
} ACTION_ENTRY;

   /* Table of actions to be performed */
ACTION_ENTRY ActionTable [ACTION_COUNT_MAX] = {FALSE, 0L};

/*
 *      Forward references
 */

   /* Function to display unread note table */
STATUS DisplayUnread (
   DBHANDLE     db_handle,
   char         NOTESPTR pName,
   WORD         nameLen
);

   /* Function to apply actions to unread note table */
STATUS UpdateUnread (
   DBHANDLE       db_handle,
   char           NOTESPTR pName,
   WORD           nameLen,
   ACTION_ENTRY   *pActions,
   int            actionCount,
   NOTEID         *pUndoID
);

   /* Function that updates a note without changing its unread status */
STATUS UndoUnreadStatus (
   DBHANDLE       db_handle, 
   char           NOTESPTR pName, 
   WORD           nameLen, 
   ACTION_ENTRY   *pActions,
   int            actionCount,
   NOTEID         UndoID);


/* {{{ proto string notes_mark_read(string database_name, string user_name, string note_id)
   Marks a note_id as read for the User user_name.  Note: user_name must be fully distinguished user name */
PHP_FUNCTION(notes_mark_read)
{

	int argc;
	int ActionCount;
	pval *argv[3];
	STATUS error;
	DBHANDLE db_handle;
	pval **db;
	pval **note_id;
	pval **user_name;


/* Local data declarations */

	WORD        UserNameLen = 0;
	char       *pEnd;
	int         curAction;
	int         curArg;
	NOTEID      UndoID = 0L;
	char		error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(3, &db, &user_name, &note_id)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(note_id);
	convert_to_string_ex(user_name);

	error = NotesInitExtended( argc, (char **) argv );

	if (error) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	curArg = 2;

	UserNameLen = Z_STRLEN_PP(user_name);

	curAction = 0;
	ActionCount = 1;

	ActionTable[curAction].AddFlag = FALSE;

	ActionTable[curAction].NoteID = strtoul (Z_STRVAL_PP(note_id), &pEnd, 16);

	if (error = NSFDbOpen(Z_STRVAL_PP(db), &db_handle)) {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	error = UpdateUnread (db_handle, Z_STRVAL_PP(user_name), UserNameLen,
				ActionTable, ActionCount, &UndoID);

	if ((error == NOERROR) && UndoID) {
	 error = UndoUnreadStatus (db_handle, Z_STRVAL_PP(user_name), UserNameLen, ActionTable,
								ActionCount, UndoID);
	}

    /* Close the database */
	error = NSFDbClose (db_handle);

	if (NOERROR == error) {
		RETURN_TRUE;
	} else {
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to close database: %s", error_string);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string notes_mark_unread(string database_name, string user_name, string note_id)
   Marks a note_id as unread for the User user_name.  Note: user_name must be fully distinguished user name */
PHP_FUNCTION(notes_mark_unread)
{

	int argc;
	int ActionCount;
	pval *argv[3];
	STATUS error;
	DBHANDLE db_handle;
	pval **db;
	pval **note_id;
	pval **user_name;


/* Local data declarations */

	WORD        UserNameLen = 0;
	char       *pEnd;
	int         curAction;
	int         curArg;
	NOTEID      UndoID = 0L;
	char		error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(3, &db, &user_name, &note_id)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(note_id);
	convert_to_string_ex(user_name);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	curArg = 2;

	UserNameLen = strlen (Z_STRVAL_PP(user_name));

	curAction = 0;
	ActionCount = 1;

	ActionTable[curAction].AddFlag = TRUE;

	ActionTable[curAction].NoteID =
	 strtoul (Z_STRVAL_PP(note_id), &pEnd, 16);

	if (error = NSFDbOpen (Z_STRVAL_PP(db), &db_handle)){
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	error = UpdateUnread (db_handle, Z_STRVAL_PP(user_name), UserNameLen,
				ActionTable, ActionCount, &UndoID);

	if ( (error == NOERROR) && UndoID ){

		error = UndoUnreadStatus (db_handle, Z_STRVAL_PP(user_name), UserNameLen, ActionTable,
								ActionCount, UndoID);
	}

    /* Close the database */
	error = NSFDbClose (db_handle);

	if (NOERROR == error){
	
		RETURN_TRUE;
	}
	else{

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to close the database: %s", error_string);
		RETURN_FALSE;
	}
}
/* }}} */

/*
 *      UpdateUnread - Update the Unread Note list
 */

STATUS UpdateUnread (
         DBHANDLE      db_handle,
         char          *pName,
         WORD          nameLen,
         ACTION_ENTRY  *pActions,
         int           actionCount,
         NOTEID        *pUndoID)
{
	STATUS         error;
	HANDLE         table_handle;
	HANDLE         hOriginalTable;
	NOTEHANDLE     note_handle;
	BOOL           gotUndoID = FALSE;

	/* Get the unread list */
	error = NSFDbGetUnreadNoteTable (
			  db_handle,
			  pName,
			  nameLen,
			  TRUE,         /* Create the list if it's not already there */
			  &table_handle);

	if (NOERROR != error){

	  return (error);
	}

	/* Notes requires the original unread table to merge changes */
	error = IDTableCopy (table_handle, &hOriginalTable);

	if (NOERROR != error){

	  IDDestroyTable (table_handle);
	  return (error);
	}

	/* Bring table up to date */
	error = NSFDbUpdateUnread (db_handle, table_handle);
	if (NOERROR == error){

		if (pActions[0].AddFlag){

			/* Adding a Note ID */
			/* (Marks note as Unread) */
			if (IDIsPresent (table_handle, pActions[0].NoteID)){

				php_error(E_WARNING,"Note %lX is already marked unread", pActions[0].NoteID);
			}
			else{
				/* make sure we check to see if this note really exists 
				  at all */
				error = NSFNoteOpen(db_handle, pActions[0].NoteID, OPEN_SUMMARY, &note_handle);

				/* if it does we'll add it to the unread list */
				if (error == NOERROR){

				   NSFNoteClose(note_handle);
				   error = IDInsert (table_handle, pActions[0].NoteID,
									  (NOTESBOOL NOTESPTR) NULL);

				}
			}   
		}
		else{

			/* Removing a Note ID */
			/* (Marks note as Read) */

			if (IDIsPresent (table_handle, pActions[0].NoteID)){

			   error = IDDelete (table_handle, pActions[0].NoteID,
				  (NOTESBOOL NOTESPTR) NULL);

			   if (NOERROR == error){

				  if (!gotUndoID){
					 *pUndoID = pActions[0].NoteID;
					 gotUndoID = TRUE;
				  }
			   }
			}
			else{

				php_error(E_WARNING,"Note %lX is already marked read", pActions[0].NoteID);
			}
		}

		if (NOERROR == error){

			error = NSFDbSetUnreadNoteTable (db_handle, pName, nameLen,
										   FALSE,      /* Don't force the 
														  write to disk */
										   hOriginalTable,
										   table_handle);
		}
	}

	error = IDDestroyTable (hOriginalTable);
	if (NOERROR == error){

	  error = error;
	}

	error = IDDestroyTable (table_handle);

	if (NOERROR == error){

		error = error;
	}

	return (error);
}


/*
 *      UndoUnreadStatus - Update a note in the database without
                           changing its read/unread mark
 */

STATUS UndoUnreadStatus (
         DBHANDLE       db_handle,
         char           *pName,
         WORD           nameLen,
         ACTION_ENTRY   *pActions,
         int            actionCount,
         NOTEID         UndoID)
{
	STATUS         error;
	HANDLE         table_handle;
	HANDLE         hOriginalTable;
	DWORD          noteID = 0L;
	NOTEHANDLE     note_handle;
	BOOL           bWasRead = TRUE;


	/* Get the unread list */
	error = NSFDbGetUnreadNoteTable (db_handle, pName, nameLen,
									 TRUE, /* Create the list if it's 
											  not already there */
									  &table_handle);
	if (NOERROR != error){

	  return (error);
	}

	/* Notes requires the original unread table to merge changes */
	
	error = IDTableCopy (table_handle, &hOriginalTable);

	if (NOERROR != error){

		IDDestroyTable (table_handle);
		return (error);
	}

	/* See if note to be modified is marked as read */
	bWasRead = !(IDIsPresent (table_handle, UndoID));

	/* Make a change to this note and update it. */
	error = NSFNoteOpen (db_handle, UndoID, 0, &note_handle);

	if (error){  

		IDDestroyTable (table_handle);
		IDDestroyTable (hOriginalTable);
		return (error);
	}

	error = NSFItemSetText (note_handle, "plain_text", "Unread undone", 
							(WORD) strlen("Unread undone"));
	if (error){

		NSFNoteClose (note_handle);
		IDDestroyTable (table_handle);
		IDDestroyTable (hOriginalTable);
		return (error);
	}  

	error = NSFNoteUpdate (note_handle, 0L);

	if (error){

		NSFNoteClose (note_handle);
		IDDestroyTable (table_handle);
		IDDestroyTable (hOriginalTable);
		return (error);
	}  

	error = NSFNoteClose (note_handle);

	if (error){

		IDDestroyTable (table_handle);
		IDDestroyTable (hOriginalTable);
		return (error);
	}

	/* The note just modified is now marked as unread.
	  Bring table up to date */

	error = NSFDbUpdateUnread (db_handle, table_handle);

	if ( (NOERROR == error) && bWasRead ){

		/* Remove the Note ID that we just modified to mark it as read*/

		error = IDDelete (table_handle, UndoID,
				  (NOTESBOOL NOTESPTR) NULL);

		if( error != NOERROR ){

			php_error(E_NOTICE,"Note %lX is already marked read.", UndoID);
		}
	}

	/* Save unread table to disk */
	if (NOERROR == error){

	  error = NSFDbSetUnreadNoteTable (db_handle, pName, nameLen,
										FALSE,  /* Don't force the write 
												   to disk */
										hOriginalTable, table_handle);
	}

	/* Clean up */
	IDDestroyTable (hOriginalTable);
	IDDestroyTable (table_handle);

	return (error);
}

/* {{{ proto string notes_unread(string database_name, string user_name)
   Returns the unread note id's for the current User user_name.  Note: user_name must be fully distinguished user name */
PHP_FUNCTION(notes_unread)
{

	int argc;
	pval *argv[2];
	
	STATUS		error;
	DBHANDLE	db_handle;
	WORD		UserNameLen;
	HANDLE		table_handle;
	DWORD	    noteID;
	BOOL        FirstNote;
	char		error_string[200];

	pval **db;
	pval **user_name;

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(2, &db, &user_name)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(user_name);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}

	if (error = NSFDbOpen (Z_STRVAL_PP(db), &db_handle)){
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	UserNameLen = strlen(Z_STRVAL_PP(user_name));

	/* Get the unread list */
	if( error = NSFDbGetUnreadNoteTable (
		   db_handle,
		   Z_STRVAL_PP(user_name),
		   UserNameLen,
		   TRUE,         /* Create the list if it's not already there */
		   &table_handle) ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to get unread list: %s", error_string );
		NotesTerm();
		RETURN_FALSE;
	}


   error = NSFDbUpdateUnread (db_handle, table_handle);
   if (NOERROR != error)
   {
		IDDestroyTable (table_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to get unread list: %s", error_string );
		NotesTerm();
		RETURN_FALSE;
   }
	

	FirstNote = TRUE;

	array_init(return_value);

	/* Print the entries in the unread list */
	while (IDScan (table_handle, FirstNote, &noteID)){

		FirstNote = FALSE;

		add_next_index_long(return_value, (long) noteID );
	}
   
	error = IDDestroyTable (table_handle);

    /* Close the database */
	error = NSFDbClose (db_handle);

	if( error != NOERROR ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to close database: %s", error_string);
		RETURN_FALSE;
	}
}
/* }}} */

#define     ERR_READMAIL_NOUNIQUE   (PKG_ADDIN + 0)
#define     READMAIL_BODY_LINELEN   40

STATUS near pascal GetUniqueFileName(char *Drive, char *Ext, char *FileName);

/* {{{ proto object notes_header_info(string server, string mailbox, int msg_number)
   Opens the message msg_number in the specified mailbox on the specified server (leave server blank for local) */
/*	
	Elements:

		originator (Contains orignal message sender. Relevant only when forwarded messages)
		to
		cc
		bcc
		from
		date
		status (blank if delivered, otherwise contains failure reason)
		priority H: High N: Normal L: Low
		receipt 1: Yes 0: No
		subject
*/
PHP_FUNCTION(notes_header_info)
{

	int argc;
	pval *argv[3];

	pval **server;
	pval **mail_box;
	pval **msg_number;

    STATUS      error = NOERROR;
    char        szMailFilePath[MAXPATH+1];
    HANDLE      hMessageFile;
    HANDLE      hMessageList = NULLHANDLE, hMessage;
    DARRAY     *MessageList;
    WORD        MessageCount;
    char        Originator[MAXRECIPIENTNAME+1];
    WORD        OriginatorLength;
    char        String[MAXSPRINTF+1];
    WORD        StringLength;
    TIMEDATE    Time;
    BOOL        NonDeliveryReport;
	char		error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(3, &server, &mail_box, &msg_number)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(server);
	convert_to_string_ex(mail_box);
	convert_to_long_ex(msg_number);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}

    OSPathNetConstruct( NULL,               /* port name  */
                        Z_STRVAL_PP(server),   
                        Z_STRVAL_PP(mail_box),
                        szMailFilePath);

    /* Open the message file. */

    if (error = MailOpenMessageFile(szMailFilePath, &hMessageFile)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Error: unable to open '%s': %s", szMailFilePath, error_string);
		RETURN_FALSE;
    }

    /* Create message list of messages in the file - just 64K */

    if (error = MailCreateMessageList(hMessageFile, 
                        &hMessageList, &MessageList, &MessageCount)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Unable to create message list: %s", error_string);

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		RETURN_FALSE;
    }


    /* Print out each of the outbound messages. */

	object_init(return_value);

    if (error = MailOpenMessage (MessageList, (WORD)Z_LVAL_PP(msg_number), &hMessage)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Unable to open message number %d: %s", (WORD)Z_LVAL_PP(msg_number), error_string);

		if (hMessageList != NULLHANDLE){

			OSUnlockObject(hMessageList);
			OSMemFree(hMessageList);
		}

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		RETURN_FALSE;
    }

    /* Get the originator's name/address. */

    if (error = MailGetMessageOriginator(MessageList, (WORD)Z_LVAL_PP(msg_number), 
            Originator, sizeof(Originator), &OriginatorLength)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Unable to get message originator: %s", error_string);

		if (hMessageList != NULLHANDLE){

			OSUnlockObject(hMessageList);
			OSMemFree(hMessageList);
		}

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		RETURN_FALSE;
    }

	add_property_string(return_value,"originator",Originator, 1);

    /* SendTo */
    MailGetMessageItem (hMessage, MAIL_SENDTO_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

	add_property_string(return_value,"to",String, 1);

    /* PostedDate */
    MailGetMessageItemTimeDate(hMessage, MAIL_POSTEDDATE_ITEM_NUM, &Time);
    ConvertTIMEDATEToText(NULL, NULL, &Time, String, 
                                sizeof(String), &StringLength);

	add_property_string(return_value,"date",String, 1);

    /* BCC */
    MailGetMessageItem (hMessage, MAIL_BLINDCOPYTO_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

   	add_property_string(return_value,"bcc", String, 1);

    /* CopyTo */
    MailGetMessageItem (hMessage, MAIL_COPYTO_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

   	add_property_string(return_value,"cc", String, 1);

    /* From */
    MailGetMessageItem (hMessage, MAIL_FROM_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

	add_property_string(return_value,"from",String, 1);

    /* Subject */

    MailGetMessageItem (hMessage, MAIL_SUBJECT_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

	add_property_string(return_value,"subject",String, 1);

    /* Priority H: High N: Normal L: Low*/

    MailGetMessageItem (hMessage, MAIL_DELIVERYPRIORITY_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

	add_property_string(return_value,"priority",String, 1);

    /* Return Receipt 1: Yes 0: No*/

    MailGetMessageItem (hMessage, MAIL_RETURNRECEIPT_ITEM_NUM, String, 
                                    MAXSPRINTF, &StringLength);

	add_property_string(return_value,"receipt",String, 1);

	NonDeliveryReport = MailIsNonDeliveryReport(hMessage);

    if (NonDeliveryReport)
    {
        MailGetMessageItem(hMessage, MAIL_FAILUREREASON_ITEM_NUM,
                        String, sizeof(String), &StringLength);

		add_property_string(return_value,"status",String, 1);
    }
	else{

		add_property_string(return_value,"status","", 1);
	}

    MailCloseMessage (hMessage);

    /* Free the message list and close the message file */

    if (hMessageList != NULLHANDLE){

        OSUnlockObject(hMessageList);
        OSMemFree(hMessageList);
    }

    if (hMessageFile != NULLHANDLE){

        MailCloseMessageFile(hMessageFile);
	}
}
/* }}} */

STATUS near pascal GetUniqueFileName(char *Drive, char *Ext,
                                        char *FileName)

{
    int     file;
    WORD    Num;
    char    Name[17];
    char    cwd[MAXPATH];
    char   *Dir;
        
    /* Increment through numbered file names until a non-existent one found. */
   getcwd(cwd, MAXPATH);
   Dir = (char *)&cwd;
                
    for (Num = 0; Num <= 32767; Num++)
    {
        _itoa(Num, Name, 10);
        _makepath(FileName, Drive, Dir, Name, Ext);
        if ((file = open(FileName, O_BINARY | O_RDONLY)) == -1)
            return(NOERROR);
        close(file);
    }
    FileName[0] = '\0';
    return(ERR_READMAIL_NOUNIQUE);
}

/* {{{ proto array notes_body(string server, string mailbox, int msg_number)
   Opens the message msg_number in the specified mailbox on the specified server (leave server blank for local) and returns an array of body text lines */
PHP_FUNCTION(notes_body)
{

	int argc;
	pval *argv[3];

	pval **server;
	pval **mail_box;
	pval **msg_number;

    STATUS      error = NOERROR;
    char        szMailFilePath[MAXPATH+1];
    HANDLE      hMessageFile;
    HANDLE      hMessageList = NULLHANDLE, hMessage;
    DARRAY     *MessageList;
    WORD        MessageCount;
    char        String[MAXSPRINTF+1];
    DWORD       BodyFileSize;
    char        BodyFileName[MAXPATH_OLE];
    FILE       *BodyFile;
	char		error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(3, &server, &mail_box, &msg_number)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(server);
	convert_to_string_ex(mail_box);
	convert_to_long_ex(msg_number);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}

    OSPathNetConstruct( NULL,               /* port name  */
                        Z_STRVAL_PP(server),   
                        Z_STRVAL_PP(mail_box),
                        szMailFilePath);

    /* Open the message file. */

    if (error = MailOpenMessageFile(szMailFilePath, &hMessageFile)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Error: unable to open '%s': %s.", szMailFilePath, error_string);
		RETURN_FALSE;
    }

    /* Create message list of messages in the file - just 64K */

    if (error = MailCreateMessageList(hMessageFile, 
                        &hMessageList, &MessageList, &MessageCount)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Unable to create message list: %s", error_string);

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		RETURN_FALSE;
    }

    if (error = MailOpenMessage (MessageList, (WORD)Z_LVAL_PP(msg_number), &hMessage)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
        php_error(E_WARNING,"Unable to open message number %d: %s", (WORD)Z_LVAL_PP(msg_number), error_string);

		if (hMessageList != NULLHANDLE){

			OSUnlockObject(hMessageList);
			OSMemFree(hMessageList);
		}

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		RETURN_FALSE;
    }

    /* Body */
    if (error = GetUniqueFileName("", "TMP", BodyFileName)){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Enable to create temporary file name: %s", error_string);

		if (hMessageList != NULLHANDLE){

			OSUnlockObject(hMessageList);
			OSMemFree(hMessageList);
		}

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		MailCloseMessage (hMessage);
		RETURN_FALSE;

    }

	/* Using MailGetBodyComposite instead of MailGetBody because it's not limited to 64k */

	   if (error = MailGetMessageBodyText(hMessage,
                                NULL,     /* Use standard Body item */
                                "\r\n",    /* Newline-terminate */
                                80,        /* 80 chars per line */
                                TRUE,     /* Convert TABs */
                                BodyFileName,
                                &BodyFileSize)){

        unlink(BodyFileName);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to get Message body into temporary file: %s", error_string);

		if (hMessageList != NULLHANDLE){

			OSUnlockObject(hMessageList);
			OSMemFree(hMessageList);
		}

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		MailCloseMessage (hMessage);
		RETURN_FALSE;
    }

    /* Print each line of body text to the screen. */

    if (!(BodyFile = fopen(BodyFileName, "r"))){

        unlink(BodyFileName);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open temporary file: %s", error_string);

		if (hMessageList != NULLHANDLE){

			OSUnlockObject(hMessageList);
			OSMemFree(hMessageList);
		}

		if (hMessageFile != NULLHANDLE){

			MailCloseMessageFile(hMessageFile);
		}
		MailCloseMessage (hMessage);
		RETURN_FALSE;
    }

	array_init(return_value);

	/* this should probably return the body in one string, but I don't know how */

    while (fgets(String, READMAIL_BODY_LINELEN, BodyFile)){

        add_next_index_string( return_value, String, 1);
    }
    fclose(BodyFile);
    unlink(BodyFileName);

    MailCloseMessage (hMessage);

    /* Free the message list and close the message file */

    if (hMessageList != NULLHANDLE){

        OSUnlockObject(hMessageList);
        OSMemFree(hMessageList);
    }

    if (hMessageFile != NULLHANDLE){

        MailCloseMessageFile(hMessageFile);
	}
}
/* }}} */

/* {{{ proto bool notes_find_note(string database_name, string name [, string type])
   Returns a note id found in database_name */
/*
Specify the name of the note. Leaving type blank
will default to all, otherwise specify:
	
		FORM
		VIEW
		FILTER
		FIELD
*/
PHP_FUNCTION(notes_find_note)
{

	int argc;
	pval *argv[3];

	pval **db;
	pval **name;
	pval **type;

    STATUS      error = NOERROR;
	DBHANDLE db_handle;
	NOTEID note_id;

	char error_string[200];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(3, &db, &name, &type)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(name);
	convert_to_string_ex(type);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}

	if (error = NSFDbOpen (Z_STRVAL_PP(db), &db_handle)){
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	if( strcmp( Z_STRVAL_PP(type), "FORM" ) == 0 ){
	
		if ((error = NIFFindDesignNote(db_handle, Z_STRVAL_PP(name), NOTE_CLASS_FORM, &note_id)) != ERR_NOT_FOUND) {

			RETVAL_LONG( (long) note_id );
		}
	} else if( strcmp( Z_STRVAL_PP(type), "VIEW" ) == 0 ){
	
		if ((error = NIFFindDesignNote(db_handle, Z_STRVAL_PP(name), NOTE_CLASS_VIEW, &note_id)) != ERR_NOT_FOUND) {

			RETVAL_LONG( (long) note_id );
		}
	} else if( strcmp( Z_STRVAL_PP(type), "FILTER" ) == 0 ){
	
		if ((error = NIFFindDesignNote(db_handle, Z_STRVAL_PP(name), NOTE_CLASS_FILTER, &note_id)) != ERR_NOT_FOUND) {

			RETVAL_LONG( (long) note_id );
		}

	} else if( strcmp( Z_STRVAL_PP(type), "FIELD" ) == 0 ){
	
		if ((error = NIFFindDesignNote(db_handle, Z_STRVAL_PP(name), NOTE_CLASS_FIELD, &note_id)) != ERR_NOT_FOUND) {

			RETVAL_LONG( (long) note_id );
		}
	}
	else{

		if ((error = NIFFindDesignNote(db_handle, Z_STRVAL_PP(name), NOTE_CLASS_ALL, &note_id)) != ERR_NOT_FOUND) {

			RETVAL_LONG( (long) note_id );
		}
	}

	NSFDbClose(db_handle);
	NotesTerm();
}
/* }}} */

/* {{{ proto bool notes_nav_create(string database_name, string name)
   Creates a navigator name, in database_name */
PHP_FUNCTION(notes_nav_create)
{

	int argc;
	pval *argv[2];

	pval **db;
	pval **name;
	char error_string[200];

    STATUS  error = NOERROR;
	DBHANDLE db_handle;

    WORD    ClassView = NOTE_CLASS_VIEW;

    NOTEHANDLE view_handle;      /* sample navigator view handle */

    char szMainView[] = "MainView";            /* title of view to be used */

    VIEWMAP_HEADER_RECORD  NavHeader;

	WORD	wLayoutCount;
	WORD    wNavLayoutBufLen;
	HANDLE  hNavLayoutBuffer;
	char   *pNavLayoutBuffer;
	char   *pNLBuf;

	char    szDFlags[3];

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(2, &db, &name)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(name);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}

	if (error = NSFDbOpen (Z_STRVAL_PP(db), &db_handle)){
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	error = NSFNoteCreate( db_handle, &view_handle );
	if ( error ) {

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to create note in database: %s", error_string);
		NSFDbClose( db_handle );
		NotesTerm();
		RETURN_FALSE;
	}


    NSFNoteSetInfo( view_handle, _NOTE_CLASS, &ClassView );

/*
 * Set the view name. 
 */
	error = NSFItemSetText( view_handle, VIEW_TITLE_ITEM,
						  Z_STRVAL_PP(name), MAXWORD );
	if ( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to set text item '%s' in view note: %s", VIEW_TITLE_ITEM, error_string);
		NSFNoteClose( view_handle );
		NSFDbClose( db_handle );
		NotesTerm();
		RETURN_FALSE;
	}

/*
 * Append Design flags for the Navigator view.
 */
	szDFlags[0] = DESIGN_FLAG_VIEWMAP;
	szDFlags[1] = DESIGN_FLAG_HIDE_FROM_V3;
	szDFlags[2] = '\0';
	error = NSFItemAppend(  view_handle,
					   ITEM_SUMMARY,
					   DESIGN_FLAGS,
					   (WORD)strlen(DESIGN_FLAGS),
					   TYPE_TEXT,
					   szDFlags,
					   (DWORD)strlen(szDFlags ));
	if ( error ) {

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to append item '%s' to view note: %s", DESIGN_FLAGS, error_string);

		NSFNoteClose( view_handle );
		NSFDbClose( db_handle );
		NotesTerm();
		RETURN_FALSE;
	}

	wLayoutCount = 0;
	wNavLayoutBufLen =  ODSLength( _VIEWMAP_HEADER_RECORD );   

/* 
 * Allocate and initialize the CD buffer for the entire $ViewMapLayout item.
 */
    if ( error = OSMemAlloc( 0, wNavLayoutBufLen, &hNavLayoutBuffer )){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to allocate %d bytes memory: %s", wNavLayoutBufLen, error_string);

		NSFNoteClose( view_handle );
		NSFDbClose( db_handle );
		NotesTerm();
		RETURN_FALSE;
    }

    pNavLayoutBuffer = (char*) OSLockObject( hNavLayoutBuffer );
    memset( pNavLayoutBuffer, 0, (size_t) wNavLayoutBufLen );

	/* 
	 * Initialize pNLBuf. pNavLayoutBuffer will remain pointing to the top 
	 * of the buffer. pNLBuf will move to point to the next available byte.
	 */
    pNLBuf = pNavLayoutBuffer;


	NavHeader.Header.Signature = SIG_CD_VMHEADER;
	NavHeader.Header.Length = (BYTE) ODSLength(_VIEWMAP_HEADER_RECORD );
	NavHeader.Version = VIEWMAP_VERSION;
	NavHeader.NameLen = 0;

    ODSWriteMemory( &pNLBuf, _VIEWMAP_HEADER_RECORD, &NavHeader, 1 );

    error = NSFItemAppend( view_handle,
                            ITEM_SUMMARY,
                            VIEWMAP_LAYOUT_ITEM,
                            (WORD)strlen(VIEWMAP_LAYOUT_ITEM),
                            TYPE_VIEWMAP_LAYOUT,
                            pNavLayoutBuffer,
                            (DWORD)wNavLayoutBufLen );

    OSUnlockObject( hNavLayoutBuffer );
    OSMemFree( hNavLayoutBuffer );

    if ( error ) {

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to append item '%s' to view note: %s", VIEWMAP_LAYOUT_ITEM, error_string);

		NSFNoteClose( view_handle );
		NSFDbClose( db_handle );
		NotesTerm();
		RETURN_FALSE;
    }

/*
 *  Done constructing the view note. Now store the view note
 *  in the database.
 */
    error = NSFNoteUpdate( view_handle, 0 );
                   
    if ( error) {

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to update note: %s", error_string);

		NSFNoteClose( view_handle );
		NSFDbClose( db_handle );
		NotesTerm();
		RETURN_FALSE;
    }

	NSFNoteClose( view_handle );
	NSFDbClose( db_handle );
	NotesTerm();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string notes_search(string database_name, string keywords)
   Finds notes that match keywords in database_name.  The note(s) that are returned must be converted to base 16. Example base_convert($note_id, "10", "16") */

STATUS LNPUBLIC file_action (void *, SEARCH_MATCH *, ITEM_TABLE *);
STATUS LNPUBLIC print_file_summary (ITEM_TABLE *);


PHP_FUNCTION(notes_search)
{

	int argc;
	pval *argv[2];
	STATUS error;
	DBHANDLE db_handle;
    HANDLE search_handle;         /* handle to a search */
	pval **db;
	pval **keywords;
	char		error_string[200];
    FT_INDEX_STATS Stats;   /* statistics from FTIndex */
    DWORD RetDocs;        /* number of documents returned by the search */
    HANDLE SearchResults_handle;  /* handle to the results of the search */
    FT_SEARCH_RESULTS *pSearchResults;    /* pointer to the results of the search */
    HANDLE IDTable_handle;        /* handle to id table built with found NOTEIDs */
    NOTEID *pNoteID;        /* pointer to the NOTEIDs found */
    BYTE *pScores;          /* pointer to the scores */
    DWORD i;
   
	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(2, &db, &keywords)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);
	convert_to_string_ex(keywords);
	
	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
	if (error = NSFDbOpen (Z_STRVAL_PP(db), &db_handle)){
	
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	error = FTIndex(db_handle, FT_INDEX_AUTOOPTIONS, NULL, &Stats);
	if (error){

		NSFDbClose (db_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to search database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

   if (error = FTOpenSearch(&search_handle)){

		NSFDbClose (db_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open search: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
   }

   /* do the search on the query */
   error = FTSearch (db_handle,                  /* database handle */
                     &search_handle,             /* pointer to previously 
                                              allocated search handle */
                     (HCOLLECTION) NULLHANDLE, /* no collection specified - 
                                              query all docs */
                     Z_STRVAL_PP(keywords),                /* query string */
                     FT_SEARCH_SCORES |    /* find relevancy scores */
                     FT_SEARCH_STEM_WORDS, /* find word variants */
                     0,                    /* maximum number of docs to
                                              return; 0 = unlimited */
                     NULLHANDLE,           /* no refining IDTABLE   */
                     &RetDocs,           /* returned number of docs */
                     NULL,                 /* reserved */
                     &SearchResults_handle);     /* returned info */
   if (error){

		FTCloseSearch (search_handle);
		NSFDbClose (db_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error during searchkkk: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
   }

   /* return if no document in the result */
   if (RetDocs == 0 )
   {
		php_printf("\n0 documents returned \n");
		FTCloseSearch (search_handle);
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   pSearchResults = OSLock (FT_SEARCH_RESULTS, SearchResults_handle);

   /* Create an IDTABLE to further refine our search */

   if (error = IDCreateTable(sizeof(NOTEID), &IDTable_handle))
   {
		FTCloseSearch (search_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error creating ID table: %s", error_string);
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   /* set up a pointer to the array of note id's that occur after
   the FT_SEARCH_RESULTS structure */

   pNoteID = (NOTEID *) (((char *) pSearchResults)
       + sizeof(FT_SEARCH_RESULTS));

   pScores = (BYTE *) (pNoteID + pSearchResults->NumHits);
   for (i = 0; i < pSearchResults->NumHits; i++, pNoteID++, pScores++)
   {
      /* save the note ids in the IDTABLE */

      if (error = IDInsert (IDTable_handle, *pNoteID, NULL))
      {

        OSUnlockObject(SearchResults_handle);
        OSMemFree (SearchResults_handle);
        FTCloseSearch (search_handle);
        IDDestroyTable (IDTable_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error saving note to ID table: %s", error_string);
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
      }
   }

   OSUnlockObject(SearchResults_handle);
   OSMemFree (SearchResults_handle);

   if (error = FTCloseSearch(search_handle))
   {
        IDDestroyTable (IDTable_handle);
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   if (error = FTOpenSearch(&search_handle))
   {
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   /* do the refined search on the query with no word variants */

   error = FTSearch (db_handle,                  /* database handle */
                     &search_handle,             /* pointer to previously 
                                              allocated search handle */
                     (HCOLLECTION) NULLHANDLE, /* no collection specified - 
                                              query all docs */
                     Z_STRVAL_PP(keywords),                /* query string */
                     FT_SEARCH_SCORES |    /* find relevancy scores */
                     FT_SEARCH_REFINE,     /* refine the search - use the
                                            * given id table */
                     0,                    /* maximum number of docs to
                                              return; 0 = unlimited */
                     IDTable_handle,             /* refining IDTABLE   */
                     &RetDocs,           /* returned number of docs */
                     NULL,                 /* reserved */
                     &SearchResults_handle);     /* returned info */
   if (error)
   {
		FTCloseSearch (search_handle);
		IDDestroyTable (IDTable_handle);
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   /* return if no document in the result */
   if (RetDocs == 0 )
   {
		php_printf("\n0 documents returned \n");
		FTCloseSearch (search_handle);
		IDDestroyTable (IDTable_handle);
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   /* obtain a pointer to the search results */
   pSearchResults = OSLock (FT_SEARCH_RESULTS, SearchResults_handle);

   /* set up a pointer to the array of note id's that occur after
      the FT_SEARCH_RESULTS structure */

   pNoteID = (NOTEID *) (((char *) pSearchResults)
           + sizeof(FT_SEARCH_RESULTS));
   
   //pSearchResults->NumHits
   array_init(return_value);

   pScores = (BYTE *) (pNoteID + pSearchResults->NumHits);
   for (i = 0; i < pSearchResults->NumHits; i++, pNoteID++, pScores++){

	    add_next_index_long(return_value, (long) *pNoteID );
		//RETVAL_LONG( (long) *pNoteID );
   }

   OSUnlockObject (SearchResults_handle);
   OSMemFree (SearchResults_handle);
   IDDestroyTable (IDTable_handle);

   if (error = FTCloseSearch(search_handle))
   {
		NSFDbClose (db_handle);
		NotesTerm();
		RETURN_FALSE;
   }

   NSFDbClose (db_handle);
   NotesTerm();
}
/* }}} */

/* {{{ proto string notes_copy_db(string from_database_name, string to_database_name [, string title])
   Creates a note using form form_name */
PHP_FUNCTION(notes_copy_db)
{

	int argc;
	pval *argv[3];
	STATUS error;
	DBHANDLE input_handle;
	DBHANDLE output_handle;
	char		error_string[200];
	pval **db_input;
	pval **db_output;
	pval **title;
    DBREPLICAINFO    replica_info;  /* replication info for the databases */
    char        output_db_info[NSF_INFO_SIZE];  /* database info buffer */
    TIMEDATE    start_time;         /* time and date used to control what
                                        notes we copy */
    TIMEDATE    last_time;          /* returned from 
                                        NSFDbGetModifiedNoteTable */
    DBID        input_dbid;         /* dbid of input database */
    DBID        output_dbid;        /* dbid of output database */
    HANDLE      idtable_p;          /* handle to id table */
    DWORD       num_scanned, num_entries;
    NOTEID      note_id;
    NOTEHANDLE  hIconNote;          /* handle to the icon note */
	
	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(3, &db_input, &db_output, &title)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db_input);
	convert_to_string_ex(db_output);
	convert_to_string_ex(title);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}
	
/* Open the input database. */

	if (error = NSFDbOpen (Z_STRVAL_PP(db_input), &input_handle)){
	
        NSFDbClose (input_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open input database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

/* Create and open the output database. */

    if (error = NSFDbCreate (Z_STRVAL_PP(db_output), DBCLASS_NOTEFILE, FALSE))
    {
        NSFDbClose (input_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to create database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

    if (error = NSFDbOpen (Z_STRVAL_PP(db_output), &output_handle))
    {
        NSFDbClose (input_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error opening output database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }


/* Copy the replication settings (not the replication history) from
the input database to the output database. The replication settings
include the database replica ID. This makes the destination database
a replica copy of the source database. */

    if (error = NSFDbReplicaInfoGet (input_handle, &replica_info))
    {
        NSFDbClose (input_handle);
        NSFDbClose (output_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error copying replication information: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

/* Copy the ACL from the input database to the output database. */

    if (error = NSFDbCopyACL (input_handle, output_handle))
    {
        NSFDbClose (input_handle);
        NSFDbClose (output_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error copying ACL: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

/* Set a time/date structure that will determine the date of the earliest
note copied in the next call.  Use TimeConstant with TIMEDATE_WILDCARD
specified to indicate that we do not want any cutoff date.  */

    TimeConstant (TIMEDATE_WILDCARD, &start_time);

/* Do not use NSFDbCopy to copy all notes in the input database to the 
   output database. Such copies are not guaranteed to be replicas of 
   the original notes.  Instead get an IDTABLE of all notes in the 
   database, use IDScan to obtain each NOTEID, and then call 
   NSFDbCopyNote to copy each note from one database to the other.
*/

    NSFDbIDGet (input_handle, &input_dbid);
    NSFDbIDGet (output_handle, &output_dbid);


/* Get the NoteID table for all notes in the input database */
    if (error = NSFDbGetModifiedNoteTable (input_handle,
                                           NOTE_CLASS_ALL,
                                           start_time, &last_time,
                                           &idtable_p) )
		if (error == ERR_NO_MODIFIED_NOTES){
            NSFDbClose (input_handle);
            NSFDbClose (output_handle);
			OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
			php_error(E_NOTICE,"There are no documents in the Database: %s", error_string);
			NotesTerm();
			RETURN_TRUE;
		}
        else
        {
            NSFDbClose (input_handle);
            NSFDbClose (output_handle);
			OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
			php_error(E_WARNING,"Error copying replication information: %s", error_string);
			NotesTerm();
			RETURN_FALSE;
        }
    num_scanned = 0L;
    num_entries = IDEntries (idtable_p);
    if (num_entries)
        while (IDScan (idtable_p, (FLAG)(num_scanned++ == 0), &note_id) )
            if (error = NSFDbCopyNote (input_handle, &input_dbid,
                                       &replica_info.ID, note_id,
                                       output_handle, &output_dbid,
                                       &replica_info.ID, NULL,
                                       NULL) )
            {
                IDDestroyTable (idtable_p);
                NSFDbClose (input_handle);
                NSFDbClose (output_handle);
				OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
				php_error(E_WARNING,"Error copying notes: %s", error_string);
				NotesTerm();
				RETURN_FALSE;
            }
    IDDestroyTable (idtable_p);
    
/* Now we can change the title of the output database
   by following these steps:

   - Get the info buffer of the database (NSFDbInfoGet);
   - Modify the title information in the buffer (NSFDbInfoModify);
   - Write the modified info buffer into the database (NSFDbInfoSet);
   - If necessary, update the ICON note with the updated database
     information buffer.  This is required for databases created from 
     a template.
*/

/* Clear out the database information buffer */
    output_db_info[0] = '\0';

/* Get the output database information buffer. */
   
    if (error = NSFDbInfoGet (output_handle, output_db_info))
    {
        NSFDbClose (input_handle);
        NSFDbClose (output_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error getting output database information buffer: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

/* Add the database title to the database information buffer */
    
    NSFDbInfoModify (output_db_info, INFOPARSE_TITLE, Z_STRVAL_PP(title));
    if (error = NSFDbInfoSet (output_handle, output_db_info))
    {
        NSFDbClose (input_handle);
        NSFDbClose (output_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error adding the database title to the buffer: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

/* If creating a new database from a template, in order to change
   the database title or any other component of the database information
   buffer, you also need to update this information in the ICON
   note after updating it in the database information buffer. */ 

    if (!NSFNoteOpen(output_handle, NOTE_ID_SPECIAL+NOTE_CLASS_ICON, 
                     0, &hIconNote))
    {
        
        /* Update the FIELD_TITLE ("$TITLE") field if present */
        if (NSFItemIsPresent (hIconNote, FIELD_TITLE, (WORD) strlen (FIELD_TITLE)) )
        {
            NSFItemSetText(hIconNote, FIELD_TITLE, output_db_info, MAXWORD);
            NSFNoteUpdate(hIconNote, 0);
        }
        NSFNoteClose(hIconNote);
    }


/* Close the databases. */

    if (error = NSFDbClose (input_handle))
        {
        NSFDbClose (output_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error closing the input database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
        }

    if (error = NSFDbClose (output_handle)){
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Error closing the output database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

	NotesTerm();
	RETURN_TRUE;
}
/* }}} */











STATUS LNPUBLIC ReadSummaryData (     /* called for every document */
            VOID far *,
            SEARCH_MATCH far *,
            ITEM_TABLE far *);
STATUS PrintSummary (char *);
STATUS ExtractTextList (char *, char *);

/* Constants */
/* Notes imposes a 32K max summary buffer total size. Therefore,
   34K will safely store the printable text rendering of any single 
   item.
*/
#define  MAX_ITEM_LEN       34816

/* The maximum number of items in the summary buffer of a single note 
   is limited to the number of ITEM structures that will fit in 32K.
*/
#define  MAX_ITEMS          32768/sizeof(ITEM)

#define  MAX_ITEM_NAME_LEN  DESIGN_NAME_MAX

/* Global variables */
ITEM    Items[MAX_ITEMS];           /* Stores the array of ITEMs */
char    ItemText[MAX_ITEM_LEN];     /* Text rendering of item value */
char    ItemName[MAX_ITEM_NAME_LEN];/* Zero terminated item name */


/* {{{ proto bool notes_list_msgs(string db)
   ??? */
PHP_FUNCTION(notes_list_msgs)
{

	int argc;
	pval *argv[1];
	
	STATUS		error;
	DBHANDLE	db_handle;
	char		error_string[200];

	pval **db;

	argc = ARG_COUNT(ht);

	if (getParametersArray(ht, argc, argv) == FAILURE){

		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(1, &db)==FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(db);

	error = NotesInitExtended( argc, (char **) argv );

	if( error ){

		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to initialize Lotus Notes: %s", error_string);
		RETURN_FALSE;
	}

	if (error = NSFDbOpen (Z_STRVAL_PP(db), &db_handle)){
	
        NSFDbClose (db_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to open database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
	}

    /*  Call NSFSearch to find all data notes in the database.
        Specify search flag SEARCH_SUMMARY so that the action
        routine gets passed the summary buffer as input.
    */

    if (error = NSFSearch (
        db_handle,          /* database handle */
        NULLHANDLE,         /* selection formula */
        NULL,               /* title of view in selection formula */
        SEARCH_SUMMARY,     /* search flags: get summary data! */
        NOTE_CLASS_DATA,    /* note class to find */
        NULL,               /* starting date (unused) */
        ReadSummaryData,    /* action routine for notes found */
        NULL,               /* argument to action routine */
        NULL))              /* returned ending date (unused) */

    {
        NSFDbClose (db_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to search database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

    if (error = NSFDbClose (db_handle))
    {
        NSFDbClose (db_handle);
		OSLoadString(NULLHANDLE, ERR(error), error_string, sizeof(error_string));
		php_error(E_WARNING,"Unable to close database: %s", error_string);
		NotesTerm();
		RETURN_FALSE;
    }

	RETURN_TRUE
}

STATUS LNPUBLIC ReadSummaryData
            (VOID far *optional_param,
            SEARCH_MATCH far *search_info,
            ITEM_TABLE far *summary_info)
{
    SEARCH_MATCH  SearchMatch;
    STATUS        error;

    memcpy ((char*)(&SearchMatch), (char *)search_info, sizeof(SEARCH_MATCH));
    
    if (!(SearchMatch.SERetFlags & SE_FMATCH))     
        return (NOERROR);

    /* Print the note ID. */

    php_printf ("\nNote ID is: %lX.\n<br />", SearchMatch.ID.NoteID);

    /* Print the summary data. */

    if (error = PrintSummary( (char*)summary_info ))
        return (error);

    return (NOERROR);
}


/************************************************************************

    FUNCTION:   PrintSummary

    PURPOSE:    Print all the items in a summary buffer.

    INPUTS:
        pSummary - a pointer to the summary buffer for one note.

    DESCRIPTION:
        The information in a summary buffer consists of an ITEM_TABLE
        structure, followed by an array of ITEM structures, followed by
        a packed sequence of field names and item values. 
        
        ITEM_TABLE
        ITEM 1
        ITEM 2
        ...
        ITEM N
        name of item 1
        value of item 1
        name of item 2
        value of item 2
        ...
        name of item N
        value if item N

        Item names are not zero terminated. Each item value starts with 
        a data type word. The lengths of the item names and item values 
        will vary. The NameLength member of the ITEM structure specifies 
        the length of the corresponding name, and the ValueLength member 
        of the ITEM structure specifies the length of the item value. 
        The ValueLength includes the length of the data type word.

        Note that this summary buffer differs in format from the buffer
        parsed by sample program VIEWSUMM. VIEWSUMM calls NIFReadEntries
        specifying READ_MASK_SUMMARYVALUES, which yields a buffer with
        format specified by ITEM_VALUE_TABLE rather than ITEM_TABLE.

        This function can parse the summary buffer of any data note
        because it does not assume the note contains items with any 
        particular names. If you know the item name in advance, use
        either NSFGetSummaryValue() or NSFLocateSummaryValue().

*************************************************************************/


STATUS PrintSummary (char *pSummary)
{
    char       *pSummaryPos;        /* current position in pSummary */
    ITEM_TABLE  ItemTable;          /* header at start of pSummary */
    USHORT      ItemCount;          /* number of items in pSummary */
    USHORT      NameLength;         /* length of item name w/out terminator*/
    USHORT      ValueLength;        /* length of item value, incl. type */
    WORD        DataType;           /* item data type word */
    char       *szDataType;         /* printable data type name */
    USHORT      TextLen;            /* length of printable item text */
    NUMBER      NumericItem;        /* an item of type TYPE_NUMBER */
    NUMBER_PAIR NumberPair;         /* part of item of TYPE_NUMBER_RANGE */
    RANGE       Range;              /* part of item of TYPE_NUMBER_RANGE */
    TIMEDATE    TimeItem;           /* a time/date item */
    TIMEDATE_PAIR   TimePairItem;   /* part of time/date list or range */
    WORD        TimeStringLen;      /* length of ASCII time/date */
    STATUS      error;              /* return code from API calls */
    USHORT      i;                  /* counter for loop over items */
    USHORT      j;                  /* " " " " multi-valued items */

   /* Initialize pSummaryPos to the position of the beginning of
      the summary buffer. Keep pSummary unmodified. Modify pSummaryPos.
    */

    pSummaryPos = pSummary;

   /* Copy the ITEM_TABLE header at the beginning of the summary buffer 
      to a local variable. Advance pSummaryPos to point to the next 
      byte in the summary buffer after the ITEM_TABLE.
    */
    memcpy ((char*)(&ItemTable), pSummaryPos, sizeof(ITEM_TABLE));
    pSummaryPos += sizeof(ItemTable);

   /* pSummaryPos now points to the first ITEM in an array of ITEM 
      structures. Copy this array of ITEM structures into the global 
      Items[] array.
    */

    ItemCount = ItemTable.Items;

    for (i=0; i < ItemCount; i++)
    {
        memcpy((char*)(&Items[i]), pSummaryPos, sizeof(ITEM));
        pSummaryPos += sizeof(ITEM);
    }

   /* pSummaryPos now points to the first item name. Loop over each
      item, copying the item name into the ItemName variable and 
      converting the item value to printable text in ItemText.
    */

    for (i=0; i < ItemCount; i++)
    {
       /* zero out two global arrays that we re-use for each item */
        memset (ItemText, 0, MAX_ITEM_LEN);
        memset (ItemName, 0, MAX_ITEM_NAME_LEN);

        NameLength = Items[i].NameLength; 
        memcpy (ItemName, pSummaryPos, NameLength);
        ItemName[NameLength] = '\0';
        pSummaryPos += NameLength;

        /*  Handle the case of a zero length item  */

        if (Items[i].ValueLength == 0)
        {
            strcpy(ItemName, "  *  ");
            ItemText[0] = '\0';
            goto PrintItem;
        }

        /* pSummaryPos now points to the item value. First get the
           data type. Then step over the data type word to the data 
           value and convert the value to printable text. Store the 
           text in ItemText.
        */

        memcpy ((char*)(&DataType), pSummaryPos, sizeof(WORD));
        pSummaryPos += sizeof(WORD);

        ValueLength = Items[i].ValueLength - sizeof(WORD);

        /* The way to extract an item depends on the data type. */       

        switch (DataType)
        {
            case TYPE_TEXT:
                szDataType = "TYPE_TEXT";
                memcpy (ItemText, pSummaryPos, ValueLength);
                ItemText[ValueLength] = '\0';
                pSummaryPos += ValueLength;
                break;

            case TYPE_TEXT_LIST:
                szDataType = "TYPE_TEXT_LIST";
                if (error = ExtractTextList (pSummaryPos, ItemText))
                {
                    printf ("Error: unable to extract text list.\n");
                    ItemText[0] = '\0';
                }
                pSummaryPos += ValueLength;
                break;

            case TYPE_NUMBER:
                szDataType = "TYPE_NUMBER";
                memcpy ((char *)(&NumericItem), pSummaryPos, sizeof(NUMBER));
                  sprintf (ItemText, "%f", NumericItem);
                pSummaryPos += sizeof(NUMBER);
                break;

            case TYPE_NUMBER_RANGE:
                szDataType = "TYPE_NUMBER_RANGE";
                TextLen = 0;
                memcpy ((char*)(&Range), pSummaryPos, sizeof(RANGE));
                pSummaryPos += sizeof(RANGE);
                for (j=0; j < Range.ListEntries; j++)
                {
                    memcpy ((char*)(&NumericItem), pSummaryPos, 
                            sizeof(NUMBER));
                    sprintf (ItemText+TextLen, "%f ", NumericItem);
                    pSummaryPos += sizeof(NUMBER);
                    TextLen = strlen(ItemText);
                }
                if ((Range.ListEntries != 0) && (Range.RangeEntries != 0))
                {
                    memcpy (ItemText+TextLen, "; ", 2);
                    TextLen += 2;
                }
                for (j=0; j < Range.RangeEntries; j++)
                {
                    memcpy ((char*)(&NumberPair), pSummaryPos, 
                            sizeof(NUMBER_PAIR));
                    sprintf (ItemText+TextLen, "%f - %f ", NumberPair.Lower,
                             NumberPair.Upper);
                    pSummaryPos += sizeof(NUMBER_PAIR);
                    TextLen = strlen(ItemText);
                }
                break;
            
#ifndef APITESTMODE

            case TYPE_TIME:
                szDataType = "TYPE_TIME";
                memcpy ((char*)(&TimeItem), pSummaryPos, sizeof(TIMEDATE));
                pSummaryPos += sizeof(TIMEDATE);
                if (error = ConvertTIMEDATEToText (
                            NULL, NULL, &TimeItem,
                            ItemText, MAXALPHATIMEDATE,
                            &TimeStringLen))
                {
                    printf ("Error: unable to convert TIMEDATE to text.\n");
                    TimeStringLen = 0;
                }
                ItemText[TimeStringLen] = '\0';
                break;

            case TYPE_TIME_RANGE:
                szDataType = "TYPE_TIME_RANGE";
                TextLen = 0;
                memcpy ((char*)(&Range), pSummaryPos, sizeof(RANGE));
                pSummaryPos += sizeof(RANGE);
                for (j=0; j < Range.ListEntries; j++)
                {
                    if (j != 0)
                    {
                        memcpy (ItemText+TextLen, ",", 1);
                        TextLen++;
                    }
                    memcpy ((char*)(&TimeItem), pSummaryPos, 
                            sizeof(TIMEDATE));
                    pSummaryPos += sizeof(TIMEDATE);
                    if (error = ConvertTIMEDATEToText (
                                NULL, NULL, 
                                &TimeItem, 
                                ItemText+TextLen, 
                                MAXALPHATIMEDATE, &TimeStringLen))
                    {
                        printf("Error: unable to convert TIMEDATE to text.\n");
                        TimeStringLen = 0;
                    }
                    TextLen += TimeStringLen;
                }
                
                if (Range.RangeEntries == 0)
                {
                    ItemText[TextLen] = '\0';
                    break;
                }
        
               /* Append a space, semicolon, space to separate time/dates 
                  from time/date ranges.
                */

                memcpy (ItemText+TextLen, " ; ", 3);
                TextLen += 3;

                for (j=0; j < Range.RangeEntries; j++)

                {
                    if (j != 0)
                    {
                        memcpy (ItemText+TextLen, ",", 1);
                        TextLen++;
                    }
                    memcpy ((char*)&TimePairItem, pSummaryPos, 
                            sizeof(TIMEDATE_PAIR));
                    pSummaryPos += sizeof(TIMEDATE_PAIR);
                    if (error = ConvertTIMEDATEToText (
                                NULL, NULL, 
                                &TimePairItem.Lower, ItemText+TextLen, 
                                MAXALPHATIMEDATE, &TimeStringLen))
                    {
                        printf("Error: unable to convert TIMEDATE to text.\n");
                        TimeStringLen = 0;
                    }
                    TextLen += TimeStringLen;
                    memcpy (ItemText+TextLen, "-", 1);
                    TextLen++;
                    if (error = ConvertTIMEDATEToText (
                                NULL, NULL, 
                                &TimePairItem.Upper, ItemText+TextLen, 
                                MAXALPHATIMEDATE, &TimeStringLen))
                    {
                        printf("Error: unable to convert TIMEDATE to text.\n");
                        TimeStringLen = 0;
                    }
                    TextLen += TimeStringLen;
                }
                ItemText[TextLen] = '\0'; /* zero terminate the text */
                break;

#endif
            /* Item is not one of the data types this program handles. */

            default:
                szDataType = "Unrecognized";
                ItemText[0] = '\0';
                pSummaryPos += ValueLength;
                break;
        } /* end of switch on data type */

PrintItem:
        /* Print the item name, data type, and value. */
        printf ("\tItem Name = %s\n", ItemName);
        printf ("\tData Type = %s\n", szDataType);

#ifndef APITESTMODE
        printf ("\tItem Value = \"%s\"\n\n", ItemText);
#else
        if (strcmp(ItemName,"$UpdatedBy")==0)
          printf ("\tItem Value =\n\n", ItemText);
        else
          printf ("\tItem Value = \"%s\"\n\n", ItemText);
#endif
    
    }/* End of loop over items in the summary. */

    /* Print final line feed to end display for this note. */

    printf ("\n");

    return (NOERROR);
}

/************************************************************************

    FUNCTION:   ExtractTextList

    PURPOSE:    This function extracts an item of TYPE_TEXT_LIST from a
                pSummary buffer.

    INPUTS:     char *pBuffer

    OUTPUTS:    char *TextList

*************************************************************************/

STATUS ExtractTextList (char *pBuffer, char *TextList)
{
    LIST    List;
    char   *ListEntry;  /* pointer to list entry */
    WORD    TextLen;    /* total length of string output to TextList */
    WORD    EntryLen;   /* length of one entry */
    STATUS  error;      /* return code from API calls */
    USHORT  i;          /* a counter */
    
    /* Initialize the total length of the list. */

    TextLen = 0;

    /* Clear the string that we'll fill up. */

    TextList[0] = '\0';

    /* Get the list header structure with the number of entries. */

    memcpy ((char*)(&List), pBuffer, sizeof(LIST));

    /* Loop over each entry of the list. */

    for (i=0; i<List.ListEntries; i++)
    {
        /* Get one entry from the list. */

        if (error = ListGetText (
                    pBuffer,
                    FALSE, /* DataType not prepended to list */
                    i,
                    &ListEntry,
                    &EntryLen))
        {
            return (ERR(error));
        }

        /*  Copy this entry to the string we are building and move the 
            pointer that keeps track of how much we have in the string. 
         */

        memcpy (TextList+TextLen, ListEntry, EntryLen);
        TextLen += EntryLen;

        /* Put a comma after the entry and advance the text length. */

        memcpy (TextList+TextLen, ",", 1);
        TextLen++;

    }   /* End of loop over list entries. */

    /* Put a null in place of the last comma */

    TextLen--;
    memcpy (TextList+TextLen, "\0", 1);

    return (NOERROR);
}

/* }}} */




