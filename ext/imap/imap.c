/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rex Logan           <veebert@dimensional.com>               |
   |          Mark Musone         <musone@afterfive.com>                  |
   |          Brian Wang          <brian@vividnet.com>                    |
   |          Kaj-Michael Lang    <milang@tal.org>                        |
   |          Antoni Pamies Olive <toni@readysoft.net>                    |
   |          Rasmus Lerdorf      <rasmus@lerdorf.on.ca>                  |
   | PHP 4.0 updates:  Zeev Suraski <zeev@zend.com>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#define IMAP41

#ifdef ERROR
#undef ERROR
#endif

#include "php.h"

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#if HAVE_IMAP

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "imap.h"
#include "mail.h"
#include "rfc822.h"
#include "modules.h"
#if (WIN32|WINNT)
#include "winsock.h"
#endif

#ifdef IMAP41
#define LSIZE text.size
#define LTEXT text.data
#define DTYPE int
#define CONTENT_PART nested.part
#define CONTENT_MSG_BODY nested.msg->body
#define IMAPVER "Imap 4R1"
#else
#define LSIZE size
#define LTEXT text
#define DTYPE char
#define CONTENT_PART contents.part
#define CONTENT_MSG_BODY contents.msg.body
#define IMAPVER "Imap 4"
#endif

#define PHP_EXPUNGE 32768

/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName",dllFunctionName,1} 
 */

/* type casts left out, put here to remove warnings in
   msvc
*/
void rfc822_date(char *date);
extern char *cpystr(const char *string);
extern unsigned long find_rightmost_bit (unsigned long *valptr);
void fs_give (void **block);
void *fs_get (size_t size);
int add_assoc_object(pval *arg, char *key, pval *tmp);
int add_next_index_object(pval *arg, pval *tmp);
void imap_add_body( pval *arg, BODY *body );

typedef struct php3_imap_le_struct {
	MAILSTREAM *imap_stream;
	long flags;
} pils;

MAILSTREAM *mail_close_it (pils *imap_le_struct);

function_entry imap_functions[] = {
	{"imap_open", php3_imap_open, NULL},
	{"imap_reopen", php3_imap_reopen, NULL},
	{"imap_num_msg", php3_imap_num_msg, NULL},
	{"imap_num_recent", php3_imap_num_recent, NULL},
	{"imap_headers", php3_imap_headers, NULL},
	{"imap_header", php3_imap_headerinfo, NULL},
	{"imap_headerinfo", php3_imap_headerinfo, NULL},
	{"imap_body", php3_imap_body, NULL},
	{"imap_fetchstructure", php3_imap_fetchstructure, NULL},
	{"imap_fetchbody", php3_imap_fetchbody, NULL},
	{"imap_expunge", php3_imap_expunge, NULL},
	{"imap_delete", php3_imap_delete, NULL},
	{"imap_undelete", php3_imap_undelete, NULL},
	{"imap_check", php3_imap_check, NULL},
	{"imap_close", php3_imap_close, NULL},
	{"imap_mail_copy", php3_imap_mail_copy, NULL},
	{"imap_mail_move", php3_imap_mail_move, NULL},
	{"imap_createmailbox", php3_imap_createmailbox, NULL},
	{"imap_renamemailbox", php3_imap_renamemailbox, NULL},
	{"imap_deletemailbox", php3_imap_deletemailbox, NULL},
	{"imap_listmailbox", php3_imap_list, NULL},
	{"imap_scanmailbox", php3_imap_listscan, NULL},
	{"imap_listsubscribed", php3_imap_lsub, NULL},
	{"imap_subscribe", php3_imap_subscribe, NULL},
	{"imap_unsubscribe", php3_imap_unsubscribe, NULL},
	{"imap_append", php3_imap_append, NULL},
	{"imap_ping", php3_imap_ping, NULL},
	{"imap_base64", php3_imap_base64, NULL},
	{"imap_qprint", php3_imap_qprint, NULL},
	{"imap_8bit", php3_imap_8bit, NULL},
	{"imap_binary", php3_imap_binary, NULL},
	{"imap_mailboxmsginfo", php3_imap_mailboxmsginfo, NULL},
	{"imap_rfc822_write_address", php3_imap_rfc822_write_address, NULL},
	{"imap_rfc822_parse_adrlist", php3_imap_rfc822_parse_adrlist, NULL},
	{"imap_setflag_full", php3_imap_setflag_full, NULL},
	{"imap_clearflag_full", php3_imap_clearflag_full, NULL},
	{"imap_sort", php3_imap_sort, NULL},
	{"imap_fetchheader", php3_imap_fetchheader, NULL},
	{"imap_fetchtext", php3_imap_body, NULL},
	{"imap_uid", php3_imap_uid, NULL},
	{"imap_msgno",php3_imap_msgno, NULL},
	{"imap_list", php3_imap_list, NULL},
	{"imap_scan", php3_imap_listscan, NULL},
	{"imap_lsub", php3_imap_lsub, NULL},
	{"imap_create", php3_imap_createmailbox, NULL},
	{"imap_rename", php3_imap_renamemailbox, NULL},
	{"imap_status", php3_imap_status, NULL},
	{"imap_bodystruct", php3_imap_bodystruct, NULL},
	{"imap_fetch_overview", php3_imap_fetch_overview, NULL},
	{"imap_mail_compose", php3_imap_mail_compose, NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_imap_module_entry = {
	IMAPVER, imap_functions, PHP_MINIT(imap), NULL, NULL, NULL,PHP_MINFO(imap), 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &php3_imap_module_entry; }
#endif

/* 
   I beleive since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local storage
*/
int le_imap;
char imap_user[80]="";
char imap_password[80]="";
STRINGLIST *imap_folders=NIL;
STRINGLIST *imap_sfolders=NIL;
long status_flags;
unsigned long status_messages;
unsigned long status_recent;
unsigned long status_unseen;
unsigned long status_uidnext;
unsigned long status_uidvalidity;

MAILSTREAM *mail_close_it (pils *imap_le_struct)
{
	MAILSTREAM *ret;
	ret = mail_close_full (imap_le_struct->imap_stream,imap_le_struct->flags);
	efree(imap_le_struct);
	return ret;
}

inline int add_assoc_object(pval *arg, char *key, pval *tmp)
{
	HashTable *symtable;
	
	if (arg->type == IS_OBJECT) {
		symtable = arg->value.obj.properties;
	} else {
		symtable = arg->value.ht;
	}
	return zend_hash_update(symtable, key, strlen(key)+1, (void *) &tmp, sizeof(pval *), NULL);
}

inline int add_next_index_object(pval *arg, pval *tmp)
{
	HashTable *symtable;
	
	if (arg->type == IS_OBJECT) {
		symtable = arg->value.obj.properties;
	} else {
		symtable = arg->value.ht;
	}

	return zend_hash_next_index_insert(symtable, (void *) &tmp, sizeof(pval *), NULL); 
}


PHP_MINFO_FUNCTION(imap)
{
	php_printf("Imap Support enabled<br>");
	php_printf("<table>");
	php_printf("<tr><td>Imap c-client Version:</td>");
#ifdef IMAP41
	php_printf("<td>Imap 4.1</td>");
#else
	php_printf("<td>Imap 4.0</td>");
#endif
	php_printf("</tr></table>");
}

PHP_MINIT_FUNCTION(imap)
{
#if !(WIN32|WINNT)
	mail_link(&unixdriver);   /* link in the unix driver */
#endif
	mail_link (&imapdriver);      /* link in the imap driver */
	mail_link (&nntpdriver);      /* link in the nntp driver */
	mail_link (&pop3driver);      /* link in the pop3 driver */
#if !(WIN32|WINNT)
	mail_link (&mhdriver);        /* link in the mh driver */
	mail_link (&mxdriver);        /* link in the mx driver */
#endif
	mail_link (&mbxdriver);       /* link in the mbx driver */
	mail_link (&tenexdriver);     /* link in the tenex driver */
	mail_link (&mtxdriver);       /* link in the mtx driver */
#if !(WIN32|WINNT)
	mail_link (&mmdfdriver);      /* link in the mmdf driver */
	mail_link (&newsdriver);      /* link in the news driver */
	mail_link (&philedriver);     /* link in the phile driver */
#endif
	mail_link (&dummydriver);     /* link in the dummy driver */
	auth_link (&auth_log);        /* link in the log authenticator */
	/* lets allow NIL */

	REGISTER_MAIN_LONG_CONSTANT("NIL", NIL, CONST_PERSISTENT | CONST_CS);


	/* Open Options */

	REGISTER_MAIN_LONG_CONSTANT("OP_DEBUG", OP_DEBUG, CONST_PERSISTENT | CONST_CS);
	/* debug protocol negotiations */

	REGISTER_MAIN_LONG_CONSTANT("OP_READONLY", OP_READONLY, CONST_PERSISTENT | CONST_CS);
	/* read-only open */

	REGISTER_MAIN_LONG_CONSTANT("OP_ANONYMOUS", OP_ANONYMOUS, CONST_PERSISTENT | CONST_CS);
	/* anonymous open of newsgroup */

	REGISTER_MAIN_LONG_CONSTANT("OP_SHORTCACHE", OP_SHORTCACHE, CONST_PERSISTENT | CONST_CS);

	/* short (elt-only) caching */

	REGISTER_MAIN_LONG_CONSTANT("OP_SILENT", OP_SILENT, CONST_PERSISTENT | CONST_CS);
	/* don't pass up events (internal use) */
	REGISTER_MAIN_LONG_CONSTANT("OP_PROTOTYPE", OP_PROTOTYPE, CONST_PERSISTENT | CONST_CS);
	/* return driver prototype */
	REGISTER_MAIN_LONG_CONSTANT("OP_HALFOPEN", OP_HALFOPEN, CONST_PERSISTENT | CONST_CS);
	/* half-open (IMAP connect but no select) */

	REGISTER_MAIN_LONG_CONSTANT("OP_EXPUNGE", OP_EXPUNGE, CONST_PERSISTENT | CONST_CS);
	/* silently expunge recycle stream */

	REGISTER_MAIN_LONG_CONSTANT("OP_SECURE", OP_SECURE, CONST_PERSISTENT | CONST_CS);
	/* don't do non-secure authentication */

	/* 
	PHP re-assigns CL_EXPUNGE a custom value that can be used as part of the imap_open() bitfield
	because it seems like a good idea to be able to indicate that the mailbox should be 
	automatically expunged during imap_open in case the script get interrupted and it doesn't get
	to the imap_close() where this option is normally placed.  If the c-client library adds other
	options and the value for this one conflicts, simply make PHP_EXPUNGE higher at the top of 
	this file 
	*/
	REGISTER_MAIN_LONG_CONSTANT("CL_EXPUNGE", PHP_EXPUNGE, CONST_PERSISTENT | CONST_CS);
	/* expunge silently */


	/* Fetch options */

	REGISTER_MAIN_LONG_CONSTANT("FT_UID", FT_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID */
	REGISTER_MAIN_LONG_CONSTANT("FT_PEEK", FT_PEEK, CONST_PERSISTENT | CONST_CS);
	/* peek at data */
	REGISTER_MAIN_LONG_CONSTANT("FT_NOT", FT_NOT, CONST_PERSISTENT | CONST_CS);
	/* NOT flag for header lines fetch */
	REGISTER_MAIN_LONG_CONSTANT("FT_INTERNAL", FT_INTERNAL, CONST_PERSISTENT | CONST_CS);
	/* text can be internal strings */
	REGISTER_MAIN_LONG_CONSTANT("FT_PREFETCHTEXT", FT_PREFETCHTEXT, CONST_PERSISTENT | CONST_CS);
	/* IMAP prefetch text when fetching header */


	/* Flagging options */

	REGISTER_MAIN_LONG_CONSTANT("ST_UID", ST_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID sequence */
	REGISTER_MAIN_LONG_CONSTANT("ST_SILENT", ST_SILENT, CONST_PERSISTENT | CONST_CS);
	/* don't return results */
	REGISTER_MAIN_LONG_CONSTANT("ST_SET", ST_SET, CONST_PERSISTENT | CONST_CS);
	/* set vs. clear */

	/* Copy options */

	REGISTER_MAIN_LONG_CONSTANT("CP_UID", CP_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID sequence */
	REGISTER_MAIN_LONG_CONSTANT("CP_MOVE", CP_MOVE, CONST_PERSISTENT | CONST_CS);
	/* delete from source after copying */


	/* Search/sort options */

	REGISTER_MAIN_LONG_CONSTANT("SE_UID", SE_UID, CONST_PERSISTENT | CONST_CS);
	/* return UID */
	REGISTER_MAIN_LONG_CONSTANT("SE_FREE", SE_FREE, CONST_PERSISTENT | CONST_CS);
	/* free search program after finished */
	REGISTER_MAIN_LONG_CONSTANT("SE_NOPREFETCH", SE_NOPREFETCH, CONST_PERSISTENT | CONST_CS);
	/* no search prefetching */
	REGISTER_MAIN_LONG_CONSTANT("SO_FREE", SO_FREE, CONST_PERSISTENT | CONST_CS);
	/* free sort program after finished */
	REGISTER_MAIN_LONG_CONSTANT("SO_NOSERVER", SO_NOSERVER, CONST_PERSISTENT | CONST_CS);
	/* don't do server-based sort */

	/* Status options */

	REGISTER_MAIN_LONG_CONSTANT("SA_MESSAGES",SA_MESSAGES , CONST_PERSISTENT | CONST_CS);
	/* number of messages */
	REGISTER_MAIN_LONG_CONSTANT("SA_RECENT", SA_RECENT, CONST_PERSISTENT | CONST_CS);
	/* number of recent messages */
	REGISTER_MAIN_LONG_CONSTANT("SA_UNSEEN",SA_UNSEEN , CONST_PERSISTENT | CONST_CS);
	/* number of unseen messages */
	REGISTER_MAIN_LONG_CONSTANT("SA_UIDNEXT", SA_UIDNEXT, CONST_PERSISTENT | CONST_CS);
	/* next UID to be assigned */
	REGISTER_MAIN_LONG_CONSTANT("SA_UIDVALIDITY",SA_UIDVALIDITY , CONST_PERSISTENT | CONST_CS);
	/* UID validity value */


	/* Bits for mm_list() and mm_lsub() */

	REGISTER_MAIN_LONG_CONSTANT("LATT_NOINFERIORS",LATT_NOINFERIORS , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("LATT_NOSELECT", LATT_NOSELECT, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("LATT_MARKED", LATT_MARKED, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("LATT_UNMARKED",LATT_UNMARKED , CONST_PERSISTENT | CONST_CS);


	/* Sort functions */

	REGISTER_MAIN_LONG_CONSTANT("SORTDATE",SORTDATE , CONST_PERSISTENT | CONST_CS);
	/* date */
	REGISTER_MAIN_LONG_CONSTANT("SORTARRIVAL",SORTARRIVAL , CONST_PERSISTENT | CONST_CS);
	/* arrival date */
	REGISTER_MAIN_LONG_CONSTANT("SORTFROM",SORTFROM , CONST_PERSISTENT | CONST_CS);
	/* from */
	REGISTER_MAIN_LONG_CONSTANT("SORTSUBJECT",SORTSUBJECT , CONST_PERSISTENT | CONST_CS);
	/* subject */
	REGISTER_MAIN_LONG_CONSTANT("SORTTO",SORTTO , CONST_PERSISTENT | CONST_CS);
	/* to */
	REGISTER_MAIN_LONG_CONSTANT("SORTCC",SORTCC , CONST_PERSISTENT | CONST_CS);
	/* cc */
	REGISTER_MAIN_LONG_CONSTANT("SORTSIZE",SORTSIZE , CONST_PERSISTENT | CONST_CS);
	/* size */

	REGISTER_MAIN_LONG_CONSTANT("TYPETEXT",TYPETEXT , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEMULTIPART",TYPEMULTIPART , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEMESSAGE",TYPEMESSAGE , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEAPPLICATION",TYPEAPPLICATION , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEAUDIO",TYPEAUDIO , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEIMAGE",TYPEIMAGE , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEVIDEO",TYPEVIDEO , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TYPEOTHER",TYPEOTHER , CONST_PERSISTENT | CONST_CS);
	/*      TYPETEXT                unformatted text
	TYPEMULTIPART           multiple part
	TYPEMESSAGE             encapsulated message
	TYPEAPPLICATION         application data
	TYPEAUDIO               audio
	TYPEIMAGE               static image (GIF, JPEG, etc.)
	TYPEVIDEO               video
	TYPEOTHER               unknown
	*/
	REGISTER_MAIN_LONG_CONSTANT("ENC7BIT",ENC7BIT , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ENC8BIT",ENC8BIT , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ENCBINARY",ENCBINARY , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ENCBASE64",ENCBASE64, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ENCQUOTEDPRINTABLE",ENCQUOTEDPRINTABLE , CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("ENCOTHER",ENCOTHER , CONST_PERSISTENT | CONST_CS);
	/*
	ENC7BIT                 7 bit SMTP semantic data
	ENC8BIT                 8 bit SMTP semantic data
	ENCBINARY               8 bit binary data
	ENCBASE64               base-64 encoded data
	ENCQUOTEDPRINTABLE      human-readable 8-as-7 bit data
	ENCOTHER                unknown
	*/

    le_imap = register_list_destructors(mail_close_it,NULL);
	return SUCCESS;
}

/* {{{ proto int imap_open(string mailbox, string user, string password [, int options])
   Open an IMAP stream to a mailbox */
PHP_FUNCTION(imap_open)
{
	pval *mailbox;
	pval *user;
	pval *passwd;
	pval *options;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct;
	long flags=NIL;
	long cl_flags=NIL;
	
	int ind;
        int myargc=ARG_COUNT(ht);
	
	if (myargc <3 || myargc >4 || getParameters(ht, myargc, &mailbox,&user,&passwd,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_string(mailbox);
		convert_to_string(user);
		convert_to_string(passwd);
		if(myargc ==4) {
			convert_to_long(options);
			flags = options->value.lval;
			if(flags & PHP_EXPUNGE) {
				cl_flags = CL_EXPUNGE;
				flags ^= PHP_EXPUNGE;
			}
		}
		strcpy(imap_user,user->value.str.val);
		strcpy(imap_password,passwd->value.str.val);
		imap_stream = NIL;
		imap_stream = mail_open(imap_stream,mailbox->value.str.val,flags);

		if (imap_stream == NIL){
			php_error(E_WARNING,"Couldn't open stream %s\n",mailbox->value.str.val);
			RETURN_FALSE;
		}
		/* Note that if we ever come up with a persistent imap stream, the persistent version of this
		   struct needs to be malloc'ed and not emalloc'ed */
		imap_le_struct = emalloc(sizeof(pils));
		imap_le_struct->imap_stream = imap_stream;
		imap_le_struct->flags = cl_flags;	
		ind = php3_list_insert(imap_le_struct, le_imap);

		RETURN_LONG(ind);
	}
	return;
}
/* }}} */

/* {{{ proto int imap_reopen(int stream_id, string mailbox [, int options])
   Reopen IMAP stream to new mailbox */
PHP_FUNCTION(imap_reopen)
{
	pval *streamind;
	pval *mailbox;
	pval *options;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct; 
	int ind, ind_type;
	long flags=NIL;
	long cl_flags=NIL;
	int myargc=ARG_COUNT(ht);

	if (myargc<2 || myargc>3 || getParameters(ht,myargc,&streamind, &mailbox, &options) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

	convert_to_long(streamind);
	ind = streamind->value.lval;
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	convert_to_string(mailbox);
	if(myargc == 3) {
		convert_to_long(options);
		flags = options->value.lval;
		if(flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		imap_le_struct->flags = cl_flags;	
	}
	imap_stream = mail_open(imap_le_struct->imap_stream, mailbox->value.str.val, flags);
	if (imap_stream == NIL) {
		php_error(E_WARNING,"Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imap_append(int stream_id, string folder, string message [, string flags])
   Append a string message to a specified mailbox */
PHP_FUNCTION(imap_append)
{
	pval *streamind,*folder, *message,*flags;
	int ind, ind_type;
	pils *imap_le_struct; 
	STRING st;
	int myargc=ARG_COUNT(ht);
  
	if ( myargc < 3 || myargc > 4 || getParameters( ht, myargc, &streamind, &folder, &message,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
  
	convert_to_long(streamind);
	convert_to_string(folder);
	convert_to_string(message);
	if (myargc == 4) convert_to_string(flags);
	ind = streamind->value.lval;
  
	imap_le_struct = (pils *)php3_list_find( ind, &ind_type );

	if ( !imap_le_struct || ind_type != le_imap ) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	INIT (&st,mail_string,(void *) message->value.str.val,message->value.str.len);
	if(mail_append_full(imap_le_struct->imap_stream, folder->value.str.val,myargc==4?flags->value.str.val:NIL,NIL,&st)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto imap_num_msg(int stream_id)
   Gives the number of messages in the current mailbox */
PHP_FUNCTION(imap_num_msg)
{
	pval *streamind;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(imap_le_struct->imap_stream->nmsgs);
}
/* }}} */

/* {{{ proto int imap_ping(int stream_id)
   Check if the IMAP stream is still active */
PHP_FUNCTION(imap_ping)
{
	pval *streamind;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long( streamind );
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find( ind, &ind_type );
	if ( !imap_le_struct || ind_type != le_imap ) {
		php_error( E_WARNING, "Unable to find stream pointer" );
		RETURN_FALSE;
	}
	RETURN_LONG( mail_ping( imap_le_struct->imap_stream ) );
}
/* }}} */

/* {{{ proto int imap_num_recent(int stream_id)
   Gives the number of recent messages in current mailbox */
PHP_FUNCTION(imap_num_recent)
{
	pval *streamind;
	int ind, ind_type;
	pils *imap_le_struct; 
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	ind = streamind->value.lval;
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETURN_LONG(imap_le_struct->imap_stream->recent);
}
/* }}} */

/* {{{ proto int imap_expunge(int stream_id)
   Delete all messages marked for deletion */
PHP_FUNCTION(imap_expunge)
{
	pval *streamind;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_expunge (imap_le_struct->imap_stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imap_close(int stream_id [, int options])
   Close an IMAP stream */
PHP_FUNCTION(imap_close)
{
	pval *options, *streamind;
	int ind, ind_type;
	pils *imap_le_struct=NULL; 
	int myargcount=ARG_COUNT(ht);
	long flags = NIL;

	if (myargcount < 1 || myargcount > 2 || getParameters(ht, myargcount, &streamind, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	ind = streamind->value.lval;
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
    }
	if(myargcount==2) {
		convert_to_long(options);
		flags = options->value.lval;
		/* Do the translation from PHP's internal PHP_EXPUNGE define to c-client's CL_EXPUNGE */
		if(flags & PHP_EXPUNGE) {
			flags ^= PHP_EXPUNGE;
			flags |= CL_EXPUNGE;
		}	
		imap_le_struct->flags = flags;
	}
	php3_list_delete(ind);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imap_headers(int stream_id)
   Returns headers for all messages in a mailbox */
PHP_FUNCTION(imap_headers)
{
	pval *streamind;
	int ind, ind_type;
	unsigned long i;
	char *t;
	unsigned int msgno;
	pils *imap_le_struct; 
	char tmp[MAILTMPLEN];

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream,msgno);
		mail_fetchstructure (imap_le_struct->imap_stream,msgno,NIL);
		tmp[0] = cache->recent ? (cache->seen ? 'R': 'N') : ' ';
		tmp[1] = (cache->recent | cache->seen) ? ' ' : 'U';
		tmp[2] = cache->flagged ? 'F' : ' ';
		tmp[3] = cache->answered ? 'A' : ' ';
		tmp[4] = cache->deleted ? 'D' : ' ';
		sprintf (tmp+5,"%4ld) ",cache->msgno);
		mail_date (tmp+11,cache);
		tmp[17] = ' ';
		tmp[18] = '\0';
		mail_fetchfrom (tmp+18,imap_le_struct->imap_stream,msgno,(long) 20);
		strcat (tmp," ");
		if ((i = cache->user_flags)) {
			strcat (tmp,"{");
			while (i) {
				strcat (tmp,imap_le_struct->imap_stream->user_flags[find_rightmost_bit (&i)]);
				if (i) strcat (tmp," ");
			}
			strcat (tmp,"} ");
		}
		mail_fetchsubject(t=tmp+strlen(tmp),imap_le_struct->imap_stream,msgno,(long)25);
		sprintf (t+=strlen(t)," (%ld chars)",cache->rfc822_size);
		add_next_index_string(return_value,tmp,1);
	}
}
/* }}} */

/* {{{ proto imap_body(int stream_id, int msg_no [, int options])
   Read the message body */
PHP_FUNCTION(imap_body)
{
	pval *streamind, * msgno, *flags;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <2 || myargc > 3 || getParameters(ht,myargc,&streamind,&msgno,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);
	if(myargc == 3) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext_full (imap_le_struct->imap_stream,msgno->value.lval,NIL,myargc == 3 ? flags->value.lval : NIL),1);
}
/* }}} */

/*    v--- add proto here when this function is done */
/* {{{ string imap_fetchtext_full(int stream_id, int msg_no [, int options])
   Read the body of a message*/
PHP_FUNCTION(imap_fetchtext_full)
{
	pval *streamind, * msgno, *flags;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargcount = ARG_COUNT(ht);
	if (myargcount >3 || myargcount <2 || getParameters(ht,myargcount,&streamind,&msgno,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);
	if (myargcount == 3) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext_full (imap_le_struct->imap_stream,msgno->value.lval,NIL,myargcount==3?flags->value.lval:NIL),1);
}
/* }}} */

/* {{{ proto int imap_mail_copy(int stream_id, int msg_no, string mailbox [, int options])
   Copy specified message to a mailbox */
PHP_FUNCTION(imap_mail_copy)
{
	pval *streamind,*seq, *folder, *options;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargcount = ARG_COUNT(ht);
	if (myargcount > 4 || myargcount < 3 
		|| getParameters(ht,myargcount,&streamind,&seq,&folder,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(seq);
	convert_to_string(folder);
	if(myargcount == 4) convert_to_long(options);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_copy_full(imap_le_struct->imap_stream,seq->value.str.val,folder->value.str.val,myargcount == 4 ? options->value.lval : NIL)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto imap_mail_move(int stream_id, int msg_no, string mailbox)
   Move specified message to a mailbox */
PHP_FUNCTION(imap_mail_move)
{
	pval *streamind,*seq, *folder;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht)!=3 
		|| getParameters(ht,3,&streamind,&seq,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(seq);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_move(imap_le_struct->imap_stream,seq->value.str.val,folder->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imap_createmailbox(int stream_id, string mailbox)
   Create a new mailbox */
PHP_FUNCTION(imap_createmailbox)
{
	pval *streamind, *folder;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_create(imap_le_struct->imap_stream,folder->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imap_renamemailbox(int stream_id, string old_name, string new_name)
   Rename a mailbox */
PHP_FUNCTION(imap_renamemailbox)
{
	pval *streamind, *old, *new;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht)!=3 || getParameters(ht,3,&streamind,&old,&new)==FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(old);
	convert_to_string(new);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_rename(imap_le_struct->imap_stream,old->value.str.val,new->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto imap_deletemailbox(int stream_id, string mailbox)
   Delete a mailbox */
PHP_FUNCTION(imap_deletemailbox)
{
	pval *streamind, *folder;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_delete(imap_le_struct->imap_stream,folder->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imap_list(int stream_id, string ref, string pattern)
   Read the list of mailboxes */
PHP_FUNCTION(imap_list)
{
	pval *streamind, *ref, *pat;
	int ind, ind_type;
	pils *imap_le_struct; 
	STRINGLIST *cur=NIL;

	if (ARG_COUNT(ht)!=3 
		|| getParameters(ht,3,&streamind,&ref,&pat) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(ref);
	convert_to_string(pat);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
    imap_folders = NIL;
	mail_list(imap_le_struct->imap_stream,ref->value.str.val,pat->value.str.val);
	if (imap_folders == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
    cur=imap_folders;
    while (cur != NIL ) {
		add_next_index_string(return_value,cur->LTEXT,1);
        cur=cur->next;
    }
	mail_free_stringlist (&imap_folders);
}
/* }}} */

/* {{{ proto imap_scan(int stream_id, string ref, string pattern, string content)
   Read list of mailboxes containing a certain string */
PHP_FUNCTION(imap_listscan)
{
	pval *streamind, *ref, *pat, *content;
	int ind, ind_type;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (ARG_COUNT(ht)!=4 || getParameters(ht,4,&streamind,&ref,&pat,&content) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(ref);
	convert_to_string(pat);
	convert_to_string(content);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	imap_folders = NIL;
	mail_scan(imap_le_struct->imap_stream,ref->value.str.val,pat->value.str.val,content->value.str.val);
	if (imap_folders == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
	cur=imap_folders;
	while (cur != NIL ) {
		add_next_index_string(return_value,cur->LTEXT,1);
		cur=cur->next;
	}
	mail_free_stringlist (&imap_folders);
}
/* }}} */

/* {{{ proto object imap_check(int stream_id)
   Get mailbox properties */
PHP_FUNCTION(imap_check)
{
	pval *streamind;
	int ind, ind_type;
	pils *imap_le_struct;
	char date[50];

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	if (mail_ping (imap_le_struct->imap_stream) == NIL ) {
		RETURN_FALSE;
    }
	if (imap_le_struct->imap_stream && imap_le_struct->imap_stream->mailbox) {
		rfc822_date (date);
		object_init(return_value);
		add_property_string(return_value,"Date",date,1);
		add_property_string(return_value,"Driver",imap_le_struct->imap_stream->dtb->name,1);
		add_property_string(return_value,"Mailbox",imap_le_struct->imap_stream->mailbox,1);
		add_property_long(return_value,"Nmsgs",imap_le_struct->imap_stream->nmsgs);
		add_property_long(return_value,"Recent",imap_le_struct->imap_stream->recent);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imap_delete(int stream_id, int msg_no)
   Mark a message for deletion */
PHP_FUNCTION(imap_delete)
{
	pval *streamind, * msgno;
	int ind, ind_type;
	pils *imap_le_struct;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(msgno);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_setflag(imap_le_struct->imap_stream,msgno->value.str.val,"\\DELETED");
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int imap_undelete(int stream_id, int msg_no)
   Remove the delete flag from a message */
PHP_FUNCTION(imap_undelete)
{
	pval *streamind, * msgno;
	int ind, ind_type;
	pils *imap_le_struct;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(msgno);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_clearflag (imap_le_struct->imap_stream,msgno->value.str.val,"\\DELETED");
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto object imap_header(int stream_id, int msg_no [, int from_length [, int subject_length [, string default_host]]])
   Read the header of the message */
PHP_FUNCTION(imap_headerinfo)
{
	pval *streamind, *msgno, *to, *tovals, *from, *fromvals, *reply_to, *reply_tovals, *sender;
	pval *fromlength;
	pval *subjectlength;
	pval *sendervals, *return_path, *return_pathvals;
	pval *cc, *ccvals, *bcc, *bccvals;
	pval *defaulthost;
	int ind, ind_type;
	unsigned long length;
	pils *imap_le_struct;
	MESSAGECACHE * cache;
	char *mystring;
	char dummy[2000];
	char fulladdress[MAILTMPLEN],tempaddress[MAILTMPLEN];
	ENVELOPE *en;
	ADDRESS *addresstmp,*addresstmp2;
	int myargc;

	myargc=ARG_COUNT(ht);
	if (myargc<2 || myargc>5 || getParameters(ht,myargc,&streamind,&msgno,&fromlength,&subjectlength,&defaulthost)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_long(msgno);
	if(myargc >= 3) convert_to_long(fromlength); else fromlength=0x00;;
	if(myargc >= 4) convert_to_long(subjectlength); else subjectlength=0x00;
	if(myargc == 5) convert_to_string(defaulthost);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	if(msgno->value.lval && msgno->value.lval <= imap_le_struct->imap_stream->nmsgs && mail_fetchstructure (imap_le_struct->imap_stream,msgno->value.lval,NIL))
		cache = mail_elt (imap_le_struct->imap_stream,msgno->value.lval);
	else
		RETURN_FALSE;
	object_init(return_value);


    mystring=mail_fetchheader_full(imap_le_struct->imap_stream,msgno->value.lval,NIL,&length,NIL);
	if(myargc ==5)
		rfc822_parse_msg (&en,NULL,mystring,length,NULL,defaulthost->value.str.val,NIL);
	else
		rfc822_parse_msg (&en,NULL,mystring,length,NULL,"UNKNOWN",NIL);

    if (en->remail) add_property_string(return_value,"remail",en->remail,1);
    if (en->date) add_property_string(return_value,"date",en->date,1);
    if (en->date) add_property_string(return_value,"Date",en->date,1);
    if (en->subject)add_property_string(return_value,"subject",en->subject,1);
    if (en->subject)add_property_string(return_value,"Subject",en->subject,1);
    if (en->in_reply_to)add_property_string(return_value,"in_reply_to",en->in_reply_to,1);
    if (en->message_id)add_property_string(return_value,"message_id",en->message_id,1);
    if (en->newsgroups)add_property_string(return_value,"newsgroups",en->newsgroups,1);
    if (en->followup_to)add_property_string(return_value,"followup_to",en->followup_to,1);
    if (en->references)add_property_string(return_value,"references",en->references,1);

	if(en->to) {
		int ok=1;
		addresstmp=en->to;
		fulladdress[0]=0x00;

		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ...");
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}

		if(fulladdress) add_property_string( return_value, "toaddress", fulladdress, 1);
		addresstmp=en->to;
		MAKE_STD_ZVAL(to);
		array_init(to);
		do {
			MAKE_STD_ZVAL(tovals);
			object_init(tovals);
			if(addresstmp->personal) add_property_string(tovals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(tovals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(tovals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(tovals, "host", addresstmp->host, 1);
			add_next_index_object(to, tovals);
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object(return_value, "to", to);
	}

	if(en->from) {
		int ok=1;
		addresstmp=en->from;
		fulladdress[0]=0x00;
  
		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ...");
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}

		if(fulladdress) add_property_string( return_value, "fromaddress", fulladdress, 1);
		addresstmp=en->from;
		MAKE_STD_ZVAL(from);
		array_init(from);
		do {
			MAKE_STD_ZVAL(fromvals);
			object_init(fromvals);
			if(addresstmp->personal) add_property_string(fromvals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(fromvals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(fromvals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(fromvals, "host", addresstmp->host, 1);
			add_next_index_object(from, fromvals);
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object( return_value, "from", from );
	}

	if(en->cc) {
		int ok=1;
		addresstmp=en->cc;
		fulladdress[0]=0x00;
  
		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ..."); 
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}

		if(fulladdress) add_property_string( return_value, "ccaddress", fulladdress, 1);
		addresstmp=en->cc;
		MAKE_STD_ZVAL(cc);
		array_init(cc);
		do {
			MAKE_STD_ZVAL(ccvals);
			object_init(ccvals);
			if(addresstmp->personal) add_property_string(ccvals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(ccvals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(ccvals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(ccvals, "host", addresstmp->host, 1);
			add_next_index_object(cc, ccvals);
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object(return_value, "cc", cc);
	}

	if(en->bcc) {
		int ok=1;
		addresstmp=en->bcc;
		fulladdress[0]=0x00;
		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ..."); 
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}

		if(fulladdress) add_property_string( return_value, "bccaddress", fulladdress, 1);
		addresstmp=en->bcc;
		MAKE_STD_ZVAL(bcc);
		array_init(bcc);
		do {
			MAKE_STD_ZVAL(bccvals);
			object_init(bccvals);
			if(addresstmp->personal) add_property_string(bccvals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(bccvals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(bccvals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(bccvals, "host", addresstmp->host, 1);
			add_next_index_object(bcc, bccvals);
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object( return_value, "bcc", bcc );
	}

	if(en->reply_to) {
		int ok=1;
		addresstmp=en->reply_to;
		fulladdress[0]=0x00;
		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ..."); 
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}

		if(fulladdress) add_property_string( return_value, "reply_toaddress", fulladdress, 1);
		addresstmp=en->reply_to;
		MAKE_STD_ZVAL(reply_to);
		array_init(reply_to);
		do {
			MAKE_STD_ZVAL(reply_tovals);
			object_init(reply_tovals);
			if(addresstmp->personal) add_property_string(reply_tovals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(reply_tovals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(reply_tovals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(reply_tovals, "host", addresstmp->host, 1);
			add_next_index_object(reply_to, reply_tovals );
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object( return_value, "reply_to", reply_to );
	}

	if(en->sender) {
		int ok=1;
		addresstmp=en->sender;
		fulladdress[0]=0x00;
		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ..."); 
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}

		if(fulladdress) add_property_string( return_value, "senderaddress", fulladdress, 1);
		addresstmp=en->sender;
		MAKE_STD_ZVAL(sender);
		array_init(sender);
		do {
			MAKE_STD_ZVAL(sendervals);
			object_init(sendervals);
			if(addresstmp->personal) add_property_string(sendervals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(sendervals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(sendervals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(sendervals, "host", addresstmp->host, 1);
			add_next_index_object(sender, sendervals );
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object( return_value, "sender", sender );
	}

	if(en->return_path) {
		int ok=1;
		addresstmp=en->return_path;
		fulladdress[0]=0x00;
		while(ok && addresstmp) { /* while length < 1000 and we are not at the end of the list */
			addresstmp2=addresstmp->next; /* save the pointer to the next address */
			addresstmp->next=NULL; /* make this address the only one now. */
			tempaddress[0]=0x00; /* reset tempaddress buffer */
			rfc822_write_address(tempaddress,addresstmp); /* ok, write the address into tempaddress string */
			if((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
				if(strlen(fulladdress)) strcat(fulladdress,","); /* put in a comma */ 
				strcat(fulladdress,tempaddress); /* put in the new address */
			} else { /* no */
				ok=0;  /* stop looping */
				strcat(fulladdress,", ..."); 
			}
			addresstmp=addresstmp2; /* reset the pointer to the next address first! */
		}
  
		if(fulladdress) add_property_string( return_value, "return_pathaddress", fulladdress, 1);
		addresstmp=en->return_path;
		MAKE_STD_ZVAL(return_path);
		array_init(return_path);
		do {
			MAKE_STD_ZVAL(return_pathvals);
			object_init(return_pathvals);
			if(addresstmp->personal) add_property_string(return_pathvals, "personal", addresstmp->personal, 1);
			if(addresstmp->adl) add_property_string(return_pathvals, "adl", addresstmp->adl, 1);
			if(addresstmp->mailbox) add_property_string(return_pathvals, "mailbox", addresstmp->mailbox, 1);
			if(addresstmp->host) add_property_string(return_pathvals, "host", addresstmp->host, 1);
			add_next_index_object(return_path, return_pathvals );
		} while ( (addresstmp = addresstmp->next) );
		add_assoc_object( return_value, "return_path", return_path );
	}
	add_property_string(return_value,"Recent",cache->recent ? (cache->seen ? "R": "N") : " ",1);
	add_property_string(return_value,"Unseen",(cache->recent | cache->seen) ? " " : "U",1);
	add_property_string(return_value,"Flagged",cache->flagged ? "F" : " ",1);
	add_property_string(return_value,"Answered",cache->answered ? "A" : " ",1);
	add_property_string(return_value,"Deleted",cache->deleted ? "D" : " ",1);
	sprintf (dummy,"%4ld",cache->msgno);
	add_property_string(return_value,"Msgno",dummy,1);
	mail_date (dummy,cache);
	add_property_string(return_value,"MailDate",dummy,1);
	sprintf (dummy,"%ld",cache->rfc822_size); 
	add_property_string(return_value,"Size",dummy,1);
	add_property_long(return_value,"udate",mail_longdate(cache));
 
	if(en->from && fromlength) {
		fulladdress[0]=0x00;
		mail_fetchfrom(fulladdress,imap_le_struct->imap_stream,msgno->value.lval,fromlength->value.lval);
		add_property_string(return_value,"fetchfrom",fulladdress,1);
	}
	if(en->subject && subjectlength) {
		fulladdress[0]=0x00;
		mail_fetchsubject(fulladdress,imap_le_struct->imap_stream,msgno->value.lval,subjectlength->value.lval);
		add_property_string(return_value,"fetchsubject",fulladdress,1);
	}
}
/* }}} */


/* KMLANG */
/* {{{ proto array imap_lsub(int stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes */
PHP_FUNCTION(imap_lsub)
{
	pval *streamind, *ref, *pat;
	int ind, ind_type;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (ARG_COUNT(ht)!=3 || getParameters(ht,3,&streamind,&ref,&pat) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(ref);
	convert_to_string(pat);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	imap_sfolders = NIL;
	mail_lsub(imap_le_struct->imap_stream,ref->value.str.val,pat->value.str.val);
	if (imap_sfolders == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
	cur=imap_sfolders;
	while (cur != NIL ) {
		add_next_index_string(return_value,cur->LTEXT,1);
		cur=cur->next;
	}
	mail_free_stringlist (&imap_sfolders);
}
/* }}} */

/* {{{ proto int imap_subscribe(int stream_id, string mailbox)
   Subscribe to a mailbox */
PHP_FUNCTION(imap_subscribe)
{
	pval *streamind, *folder;
	int ind, ind_type;
	pils *imap_le_struct;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_subscribe(imap_le_struct->imap_stream,folder->value.str.val)==T ) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imap_unsubscribe(int stream_id, string mailbox)
   Unsubscribe from a mailbox */
PHP_FUNCTION(imap_unsubscribe)
{
	pval *streamind, *folder;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_unsubscribe(imap_le_struct->imap_stream,folder->value.str.val)==T ) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

void imap_add_body( pval *arg, BODY *body )
{
	pval *parametres, *param, *dparametres, *dparam;
	PARAMETER *par, *dpar;
	PART *part;

	if(body->type) add_property_long( arg, "type", body->type );
	if(body->encoding) add_property_long( arg, "encoding", body->encoding );

	if ( body->subtype ){
		add_property_long( arg, "ifsubtype", 1 );
		add_property_string( arg, "subtype",  body->subtype, 1 );
	} else {
		add_property_long( arg, "ifsubtype", 0 );
	}

	if ( body->description ){
		add_property_long( arg, "ifdescription", 1 );
		add_property_string( arg, "description",  body->description, 1 );
	} else {
		add_property_long( arg, "ifdescription", 0 );
	}
	if ( body->id ){
		add_property_long( arg, "ifid", 1 );
		if(body->description) add_property_string( arg, "id",  body->description, 1 );
	} else {
		add_property_long( arg, "ifid", 0 );
	}

	if(body->size.lines) add_property_long( arg, "lines", body->size.lines );
	if(body->size.bytes) add_property_long( arg, "bytes", body->size.bytes );
#ifdef IMAP41
	if ( body->disposition.type ){
		add_property_long( arg, "ifdisposition", 1);
		add_property_string( arg, "disposition", body->disposition.type, 1);
	} else {
		add_property_long( arg, "ifdisposition", 0);
	}

	if ( body->disposition.parameter ) {
		dpar = body->disposition.parameter;
		add_property_long( arg, "ifdparameters", 1);
		MAKE_STD_ZVAL(dparametres);
		array_init(dparametres);
		do {
			MAKE_STD_ZVAL(dparam);
			object_init(dparam);
			add_property_string(dparam, "attribute", dpar->attribute, 1);
			add_property_string(dparam, "value", dpar->value, 1);
			add_next_index_object(dparametres, dparam );
        } while ( (dpar = dpar->next) );
		add_assoc_object( arg, "dparameters", dparametres );
	} else {
		add_property_long( arg, "ifdparameters", 0);
	}
#endif
 
	if ( (par = body->parameter) ) {
		add_property_long( arg, "ifparameters", 1 );

		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		do {
			MAKE_STD_ZVAL(param);
			object_init(param);
			if(par->attribute) add_property_string(param, "attribute", par->attribute, 1 );
			if(par->value) add_property_string(param, "value", par->value, 1 );

			add_next_index_object(parametres, param);
		} while ( (par = par->next) );
	} else {
		MAKE_STD_ZVAL(parametres);
		object_init(parametres);
		add_property_long( arg, "ifparameters", 0 );
	}
	add_assoc_object( arg, "parameters", parametres );

	/* multipart message ? */

	if ( body->type == TYPEMULTIPART ) {
		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		for ( part = body->CONTENT_PART; part; part = part->next ) {
			MAKE_STD_ZVAL(param);
			object_init(param);
			imap_add_body(param, &part->body );
			add_next_index_object(parametres, param );
		}
		add_assoc_object( arg, "parts", parametres );
	}

	/* encapsulated message ? */

	if ((body->type == TYPEMESSAGE) && (!strcasecmp(body->subtype, "rfc822"))) {
		body=body->CONTENT_MSG_BODY;
		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		MAKE_STD_ZVAL(param);
		object_init(param);
		imap_add_body(param, body);
		add_next_index_object(parametres, param );
		add_assoc_object( arg, "parts", parametres );
	}
}

/* {{{ proto object imap_fetchstructure(int stream_id, int msg_no [, int options])
   Read the full structure of a message */
PHP_FUNCTION(imap_fetchstructure)
{
	pval *streamind, *msgno,*flags;
	int ind, ind_type;
	pils *imap_le_struct;
	BODY *body;
	int myargc=ARG_COUNT(ht);
	if ( myargc < 2  || myargc > 3 || getParameters( ht, myargc, &streamind, &msgno ,&flags) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long( streamind );
	convert_to_long( msgno );
	if(myargc == 3) convert_to_long(flags);
	object_init(return_value);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find( ind, &ind_type );

	if ( !imap_le_struct || ind_type != le_imap ) {
		php_error( E_WARNING, "Unable to find stream pointer" );
		RETURN_FALSE;
	}

	mail_fetchstructure_full( imap_le_struct->imap_stream, msgno->value.lval, &body ,myargc == 3 ? flags->value.lval : NIL);

	if ( !body ) {
		php_error( E_WARNING, "No body information available" );
		RETURN_FALSE;
	}

	imap_add_body( return_value, body );
}
/* }}} */

/* {{{ proto string imap_fetchbody(int stream_id, int msg_no, int section [, int options])
   Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	pval *streamind, *msgno, *sec,*flags;
	int ind, ind_type;
	pils *imap_le_struct;
	char *body;
	unsigned long len;
	int myargc=ARG_COUNT(ht);
	if(myargc < 3 || myargc >4 || getParameters( ht, myargc, &streamind, &msgno, &sec,&flags ) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long( streamind );
	convert_to_long( msgno );
	convert_to_string( sec );
	if(myargc == 4) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find( ind, &ind_type );

	if ( !imap_le_struct || ind_type != le_imap ) {
		php_error( E_WARNING, "Unable to find stream pointer" );
		RETURN_FALSE;
	}
 
	body = mail_fetchbody_full( imap_le_struct->imap_stream, msgno->value.lval, sec->value.str.val, &len,myargc == 4 ? flags->value.lval : NIL );

	if ( !body ) {
		php_error( E_WARNING, "No body information available" );
		RETURN_FALSE;
	}
	RETVAL_STRINGL( body ,len,1);
}
/* }}} */

/* {{{ proto string imap_base64(string text)
   Decode BASE64 encoded text */
PHP_FUNCTION(imap_base64)
{
	pval *text;
	char *decode;
	unsigned long newlength;
	if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string( text );
	object_init(return_value);

	decode = (char *) rfc822_base64((unsigned char *) text->value.str.val, text->value.str.len,&newlength);
	RETVAL_STRINGL(decode,newlength,1);
}
/* }}} */

/* {{{ proto string imap_qprint(string text)
   Convert a quoted-printable string to an 8-bit string */
PHP_FUNCTION(imap_qprint)
{
	pval *text;
	char *decode;
	unsigned long newlength;
	if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string( text );
	object_init(return_value);

	decode = (char *) rfc822_qprint((unsigned char *) text->value.str.val, text->value.str.len,&newlength);
	RETVAL_STRINGL(decode,newlength,1);
}
/* }}} */

/* {{{ proto string imap_8bit(string text)
   Convert an 8-bit string to a quoted-printable string */
PHP_FUNCTION(imap_8bit)
{
	pval *text;
	char *decode;
	unsigned long newlength;
	if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string( text );
	object_init(return_value);

	decode = (char *) rfc822_8bit((unsigned char *) text->value.str.val, text->value.str.len,&newlength);
	RETVAL_STRINGL(decode,newlength,1);
}
/* }}} */

/* {{{ proto string imap_binary(string text)
   Convert an 8bit string to a base64 string */
PHP_FUNCTION(imap_binary)
{
	pval *text;
	unsigned long len;
	if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(text);
	RETVAL_STRINGL(rfc822_binary(text->value.str.val,text->value.str.len,&len),len,1);
}
/* }}} */

/* {{{ proto array imap_mailboxmsginfo(int stream_id)
   Returns info about the current mailbox in an associative array */
PHP_FUNCTION(imap_mailboxmsginfo)
{
	pval *streamind;
	char date[50];
	int ind, ind_type;
	unsigned int msgno;
	pils *imap_le_struct;
	unsigned unreadmsg,msize;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if(!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if(object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	unreadmsg = 0;
	msize = 0;
	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream,msgno);
		mail_fetchstructure (imap_le_struct->imap_stream,msgno,NIL);
		unreadmsg = cache->recent ? (cache->seen ? unreadmsg : unreadmsg++) : unreadmsg;
		unreadmsg = (cache->recent | cache->seen) ? unreadmsg : unreadmsg++;
		msize = msize + cache->rfc822_size;
	}
	add_property_long(return_value,"Unread",unreadmsg);
	add_property_long(return_value,"Nmsgs",imap_le_struct->imap_stream->nmsgs);
	add_property_long(return_value,"Size",msize);
	rfc822_date (date);
	add_property_string(return_value,"Date",date,1);
	add_property_string(return_value,"Driver",imap_le_struct->imap_stream->dtb->name,1);
	add_property_string(return_value,"Mailbox",imap_le_struct->imap_stream->mailbox,1);
	add_property_long(return_value,"Recent",imap_le_struct->imap_stream->recent);
}
/* }}} */

/* {{{ proto string imap_rfc822_write_address(string mailbox, string host, string personal)
   Returns a properly formatted email address given the mailbox, host, and personal info */
PHP_FUNCTION(imap_rfc822_write_address)
{
	pval *mailbox,*host,*personal;
	ADDRESS *addr;
	char string[MAILTMPLEN];
	int argc;
	argc=ARG_COUNT(ht);
	if ( argc != 3 || getParameters( ht, argc, &mailbox,&host,&personal) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(mailbox);
	convert_to_string(host);
	convert_to_string(personal);
	addr=mail_newaddr();
	if(mailbox) addr->mailbox = cpystr(mailbox->value.str.val);
	if(host) addr->host = cpystr(host->value.str.val);
	if(personal) addr->personal = cpystr(personal->value.str.val);
	addr->next=NIL;
	addr->error=NIL;
	addr->adl=NIL;
	string[0]=0x00;
  
	rfc822_write_address(string,addr);
	RETVAL_STRING(string,1);
}
/* }}} */

/* {{{ proto array imap_rfc822_parse_adrlist(string address_string, string default_host)
   Parses an address string */
PHP_FUNCTION(imap_rfc822_parse_adrlist)
{
	pval *string,*defaulthost,*tovals;
	ADDRESS *addresstmp;
	ENVELOPE *env;
	int argc;

	env=mail_newenvelope();
	argc=ARG_COUNT(ht);
	if ( argc != 2 || getParameters( ht, argc, &string,&defaulthost) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	convert_to_string(defaulthost);
	rfc822_parse_adrlist(&env->to,string->value.str.val,defaulthost->value.str.val);
	if(array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	addresstmp=env->to;
	if(addresstmp) do {
		MAKE_STD_ZVAL(tovals);
		object_init(tovals);
		if(addresstmp->mailbox) add_property_string(tovals,"mailbox",addresstmp->mailbox,1);
		if(addresstmp->host) add_property_string(tovals,"host",addresstmp->host,1);
		if(addresstmp->personal) add_property_string(tovals,"personal",addresstmp->personal,1);
		if(addresstmp->adl) add_property_string(tovals,"adl",addresstmp->adl,1);
		add_next_index_object(return_value, tovals);
	} while ((addresstmp = addresstmp->next));
}
/* }}} */

/* {{{ proto int imap_setflag_full(int stream_id, string sequence, string flag [, int options])
   Sets flags on messages */
PHP_FUNCTION(imap_setflag_full)
{
	pval *streamind;
	pval *sequence;
	pval *flag;
	pval *flags;
	int ind,ind_type;
	pils *imap_le_struct;
	int myargc=ARG_COUNT(ht);

	if (myargc < 3 || myargc > 4 || getParameters(ht, myargc, &streamind,&sequence,&flag,&flags) ==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_string(sequence);
	convert_to_string(flag);
	if(myargc==4) convert_to_long(flags);

	ind = streamind->value.lval;
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if(!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mail_setflag_full(imap_le_struct->imap_stream,sequence->value.str.val,flag->value.str.val,myargc == 4 ? flags->value.lval : NIL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto imap_clearflag_full(int stream_id, string sequence, string flag [, int options])
   Clears flags on messages */
PHP_FUNCTION(imap_clearflag_full)
{
	pval *streamind;
	pval *sequence;
	pval *flag;
	pval *flags;
	int ind,ind_type;
	pils *imap_le_struct;
	int myargc=ARG_COUNT(ht);
	if (myargc < 3 || myargc > 4 || getParameters(ht, myargc, &streamind,&sequence,&flag,&flags) ==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_string(sequence);
	convert_to_string(flag);
	if(myargc==4) convert_to_long(flags);
	ind = streamind->value.lval;
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if(!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mail_clearflag_full(imap_le_struct->imap_stream,sequence->value.str.val,flag->value.str.val,myargc == 4 ? flags->value.lval : NIL);
}
/* }}} */

/* {{{ proto array imap_sort(int stream_id, int criteria, int reverse [, int options])
   Sort an array of message headers */
PHP_FUNCTION(imap_sort)
{
	pval *streamind;
	pval *pgm;
	pval *rev;
	pval *flags;
	int ind,ind_type;
	unsigned long *slst,*sl;
	SORTPGM *mypgm=NIL;
	SEARCHPGM *spg=NIL;
	pils *imap_le_struct;
	int myargc=ARG_COUNT(ht);
	if (myargc < 3 || myargc > 4 || getParameters(ht, myargc, &streamind,&pgm,&rev,&flags) ==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_long(rev);
	convert_to_long(pgm);
	if(pgm->value.lval>SORTSIZE) {
		php_error(E_WARNING, "Unrecognized sort criteria");
		RETURN_FALSE;
	}
	if(myargc==4) convert_to_long(flags);

	ind = streamind->value.lval;
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if(!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	spg = mail_newsearchpgm();
	mypgm=mail_newsortpgm();
	mypgm->reverse=rev->value.lval;
	mypgm->function=pgm->value.lval;
	mypgm->next=NIL;

	array_init(return_value);
	slst=mail_sort(imap_le_struct->imap_stream,NIL,spg,mypgm,myargc == 4 ? flags->value.lval:NIL);


	for (sl = slst; *sl; sl++) { 
		add_next_index_long(return_value,*sl);
	}
		fs_give ((void **) &slst); 

}
/* }}} */

/* {{{ proto string imap_fetchheader(int stream_id, int msg_no [, int options])
   Get the full unfiltered header for a message */
PHP_FUNCTION(imap_fetchheader)
{
	pval *streamind, * msgno, * flags;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc = ARG_COUNT(ht);
	if (myargc < 2 || myargc > 3 || getParameters(ht,myargc,&streamind,&msgno,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);
	if(myargc == 3) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchheader_full (imap_le_struct->imap_stream,msgno->value.lval,NIL,NIL,myargc == 3 ? flags->value.lval : NIL),1);

}
/* }}} */

/* {{{ proto int imap_uid(int stream_id, int msg_no)
   Get the unique message id associated with a standard sequential message number */
PHP_FUNCTION(imap_uid)
{
 	pval *streamind, *msgno;
 	int ind, ind_type;
	pils *imap_le_struct;
 
 	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_long(msgno);
 
 	ind = streamind->value.lval;
 
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
 
	if (!imap_le_struct || ind_type != le_imap) {
	php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
	RETURN_LONG(mail_uid(imap_le_struct->imap_stream, msgno->value.lval));
}
/* }}} */
 
/* {{{ proto int imap_msgno(int stream_id, int unique_msg_id)
   Get the sequence number associated with a UID */
PHP_FUNCTION(imap_msgno)
{
 	pval *streamind, *msgno;
 	int ind, ind_type;
	pils *imap_le_struct;
 
 	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_long(msgno);
 
 	ind = streamind->value.lval;
 
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
 
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
 	RETURN_LONG(mail_msgno(imap_le_struct->imap_stream, msgno->value.lval));
}
/* }}} */

/* {{{ proto object imap_status(int stream_id, string mailbox, int options)
   Get status info from a mailbox */
PHP_FUNCTION(imap_status)
{
 	pval *streamind, *mbx, *flags;
 	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ARG_COUNT(ht);
 	if (myargc != 3 || getParameters(ht,myargc,&streamind,&mbx,&flags) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_string(mbx);
	convert_to_long(flags);
 	ind = streamind->value.lval;
 
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
 
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
	if(object_init(return_value) == FAILURE){
	  RETURN_FALSE;
	}
 	if(mail_status(imap_le_struct->imap_stream, mbx->value.str.val, flags->value.lval))
	  {
	    add_property_long(return_value,"flags",status_flags);
	    if(status_flags & SA_MESSAGES) add_property_long(return_value,"messages",status_messages);
	    if(status_flags & SA_RECENT) add_property_long(return_value,"recent",status_recent);
	    if(status_flags & SA_UNSEEN) add_property_long(return_value,"unseen",status_unseen);
	    if(status_flags & SA_UIDNEXT) add_property_long(return_value,"uidnext",status_uidnext);
	    if(status_flags & SA_UIDVALIDITY) add_property_long(return_value,"uidvalidity",status_uidvalidity);
	  }
	else
	  {
	  RETURN_FALSE;
	  }
}
/* }}} */
 
/* {{{ proto object imap_bodystruct(int stream_id, int msg_no, int section)
   Read the structure of a specified body section of a specific message */
PHP_FUNCTION(imap_bodystruct)
{
	pval *streamind, *msg, *section;
	int ind, ind_type;
	pils *imap_le_struct;
	pval *parametres, *param, *dparametres, *dparam;
	PARAMETER *par, *dpar;
	BODY *body;
	int myargc=ARG_COUNT(ht);

	if (myargc != 3 || getParameters(ht,myargc,&streamind,&msg,&section) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_long(msg);
	convert_to_string(section);
 	ind = streamind->value.lval;
 
	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if(object_init(return_value) == FAILURE){
		RETURN_FALSE;
	}

	body=mail_body(imap_le_struct->imap_stream, msg->value.lval, section->value.str.val);
	if(body->type) add_property_long( return_value, "type", body->type );
	if(body->encoding) add_property_long( return_value, "encoding", body->encoding );
	
	if ( body->subtype ){
		add_property_long( return_value, "ifsubtype", 1 );
		add_property_string( return_value, "subtype",  body->subtype, 1 );
	} else {
		add_property_long( return_value, "ifsubtype", 0 );
	}
	
	if ( body->description ){
		add_property_long( return_value, "ifdescription", 1 );
		add_property_string( return_value, "description",  body->description, 1 );
	} else {
		add_property_long( return_value, "ifdescription", 0 );
	}
	if ( body->id ){
		add_property_long( return_value, "ifid", 1 );
		if(body->description) add_property_string( return_value, "id",  body->description, 1 );
	} else {
		add_property_long( return_value, "ifid", 0 );
	}
	
	if(body->size.lines) add_property_long( return_value, "lines", body->size.lines );
	if(body->size.bytes) add_property_long( return_value, "bytes", body->size.bytes );
#ifdef IMAP41
	if ( body->disposition.type ){
		add_property_long( return_value, "ifdisposition", 1);
		add_property_string( return_value, "disposition", body->disposition.type, 1);
	} else {
		add_property_long( return_value, "ifdisposition", 0);
	}
	
	if ( body->disposition.parameter ) {
		dpar = body->disposition.parameter;
		add_property_long( return_value, "ifdparameters", 1);
		MAKE_STD_ZVAL(dparametres);
		array_init(dparametres);
		do {
			MAKE_STD_ZVAL(dparam);
			object_init(dparam);
			add_property_string(dparam, "attribute", dpar->attribute, 1);
			add_property_string(dparam, "value", dpar->value, 1);
			add_next_index_object(dparametres, dparam );
		} while ( (dpar = dpar->next) );
		add_assoc_object( return_value, "dparameters", dparametres );
	} else {
		add_property_long( return_value, "ifdparameters", 0);
	}
#endif
	
	if ( (par = body->parameter) ) {
		add_property_long( return_value, "ifparameters", 1 );

		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		do {
			MAKE_STD_ZVAL(param);
			object_init(param);
			if(par->attribute) add_property_string(param, "attribute", par->attribute, 1 );
			if(par->value) add_property_string(param, "value", par->value, 1 );

			add_next_index_object(parametres, param );
		} while ( (par = par->next) );
	} else {
		MAKE_STD_ZVAL(parametres);
		object_init(parametres);
		add_property_long( return_value, "ifparameters", 0 );
	}
	add_assoc_object( return_value, "parameters", parametres );
}
/* }}} */

/* {{{ proto array imap_fetch_overview(int stream_id, int msg_no)
   Read an overview of the information in the headers of the given message */ 
PHP_FUNCTION(imap_fetch_overview)
{
 	pval *streamind, *sequence;
 	int ind, ind_type;
	pils *imap_le_struct;
	pval *myoverview;
	char address[MAILTMPLEN];
	int myargc=ARG_COUNT(ht);

 	if (myargc != 2 || getParameters(ht,myargc,&streamind,&sequence) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_string(sequence);

 	ind = streamind->value.lval;
 	imap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	
	if (!imap_le_struct || ind_type != le_imap) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	array_init(return_value);
	if (mail_uid_sequence (imap_le_struct->imap_stream,(char *)sequence)) {
		MESSAGECACHE *elt;
		ENVELOPE *env;
		unsigned long i;

		for (i = 1; i <= imap_le_struct->imap_stream->nmsgs; i++) {
			if (((elt = mail_elt (imap_le_struct->imap_stream,i))->sequence) &&
				(env = mail_fetch_structure (imap_le_struct->imap_stream,i,NIL,NIL))) {
				MAKE_STD_ZVAL(myoverview);
				object_init(myoverview);
				add_property_string(myoverview,"subject",env->subject,1);
				env->from->next=NULL;
				rfc822_write_address(address,env->from);
				add_property_string(myoverview,"from",address,1);
				add_property_string(myoverview,"date",env->date,1);
				add_property_string(myoverview,"message_id",env->message_id,1);
				add_property_string(myoverview,"references",env->references,1);
				add_property_long(myoverview,"size",elt->rfc822_size);
				add_property_long(myoverview,"uid",mail_uid(imap_le_struct->imap_stream,i));
				add_property_long(myoverview,"msgno",i);
				add_property_long(myoverview,"recent",elt->recent);
				add_property_long(myoverview,"flagged",elt->flagged);
				add_property_long(myoverview,"answered",elt->answered);
				add_property_long(myoverview,"deleted",elt->deleted);
				add_property_long(myoverview,"seen",elt->seen);
				add_next_index_object(return_value,myoverview);
			}
		}
	}
}

/* }}} */

/* {{{ proto string imap_mail_compose(array envelope, array body)
   Create a MIME message based on given envelope and body sections */
PHP_FUNCTION(imap_mail_compose)
{
	pval *envelope, *body;
	char *key;
	pval *data,**pvalue;
	ulong ind;
	char *cookie = NIL;
	ENVELOPE *env;
	BODY *bod=NULL,*topbod=NULL;
	PART *mypart=NULL,*toppart=NULL,*part;
	PARAMETER *param;
	char tmp[8*MAILTMPLEN],*mystring=NULL,*t,*tempstring;
	int myargc=ARG_COUNT(ht);

	if (myargc != 2 || getParameters(ht,myargc,&envelope,&body) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array(envelope);
	convert_to_array(body);
	env=mail_newenvelope();
	if(zend_hash_find(envelope->value.ht,"remail",sizeof("remail"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		env->remail=cpystr((*pvalue)->value.str.val);
	}

	if(zend_hash_find(envelope->value.ht,"return_path",sizeof("return_path"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		rfc822_parse_adrlist (&env->return_path,(*pvalue)->value.str.val,"NO HOST");
	}
	if(zend_hash_find(envelope->value.ht,"date",sizeof("date"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		env->date=cpystr((*pvalue)->value.str.val);
	}
	if(zend_hash_find(envelope->value.ht,"from",sizeof("from"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		rfc822_parse_adrlist (&env->from,(*pvalue)->value.str.val,"NO HOST");
	}
	if(zend_hash_find(envelope->value.ht,"reply_to",sizeof("reply_to"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(pvalue);
		rfc822_parse_adrlist (&env->reply_to,(*pvalue)->value.str.val,"NO HOST");
	}
	if(zend_hash_find(envelope->value.ht,"to",sizeof("to"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		rfc822_parse_adrlist (&env->to,(*pvalue)->value.str.val,"NO HOST");
	}
	if(zend_hash_find(envelope->value.ht,"cc",sizeof("cc"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		rfc822_parse_adrlist (&env->cc,(*pvalue)->value.str.val,"NO HOST");
	}
	if(zend_hash_find(envelope->value.ht,"bcc",sizeof("bcc"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		rfc822_parse_adrlist (&env->bcc,(*pvalue)->value.str.val,"NO HOST");
	}
	if(zend_hash_find(envelope->value.ht,"message_id",sizeof("message_id"),(void **) &pvalue)== SUCCESS){
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		env->message_id=cpystr((*pvalue)->value.str.val);
	}
 
	zend_hash_internal_pointer_reset(body->value.ht);
	zend_hash_get_current_key(body->value.ht, &key, &ind);
	zend_hash_get_current_data(body->value.ht, (void **) &data);
	if(data->type == IS_ARRAY) {
		bod=mail_newbody();
		topbod=bod;
		if(zend_hash_find(data->value.ht,"type",sizeof("type"),(void **) &pvalue)== SUCCESS) {
			SEPARATE_ZVAL(pvalue);
			convert_to_long(*pvalue);
			bod->type = (*pvalue)->value.lval;
		}
		if(zend_hash_find(data->value.ht,"encoding",sizeof("encoding"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_long(*pvalue);
			bod->encoding = (*pvalue)->value.lval;
		}
		if(zend_hash_find(data->value.ht,"subtype",sizeof("subtype"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_string(*pvalue);
			bod->subtype = cpystr((*pvalue)->value.str.val);
		}
		if(zend_hash_find(data->value.ht,"id",sizeof("id"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_string(*pvalue);
			bod->id = cpystr((*pvalue)->value.str.val);
		}
		if(zend_hash_find(data->value.ht,"contents.data",sizeof("contents.data"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_string(*pvalue);     
			bod->contents.text.data=(char *)fs_get((*pvalue)->value.str.len);
			memcpy(bod->contents.text.data,(*pvalue)->value.str.val,(*pvalue)->value.str.len+1);
			bod->contents.text.size = (*pvalue)->value.str.len;
		}
		if(zend_hash_find(data->value.ht,"lines",sizeof("lines"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_long(*pvalue);
			bod->size.lines = (*pvalue)->value.lval;
		}
		if(zend_hash_find(data->value.ht,"bytes",sizeof("bytes"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_long(*pvalue);
			bod->size.bytes = (*pvalue)->value.lval;
		}
		if(zend_hash_find(data->value.ht,"md5",sizeof("md5"),(void **) &pvalue)== SUCCESS){
			SEPARATE_ZVAL(pvalue);
			convert_to_string(*pvalue);
			bod->md5 = cpystr((*pvalue)->value.str.val);
		}
	}
	zend_hash_move_forward(body->value.ht);
	while(zend_hash_get_current_data(body->value.ht, (void **) &data)==SUCCESS) {
		zend_hash_get_current_key(body->value.ht, &key, &ind);
		if(data->type == IS_ARRAY) {
			if(!toppart) {
				bod->nested.part=mail_newbody_part();
				mypart=bod->nested.part;
				toppart=mypart;
				bod=&mypart->body;
			} else {
				 mypart->next=mail_newbody_part();
				 mypart=mypart->next;
				 bod=&mypart->body;
			}
			if(zend_hash_find(data->value.ht,"type",sizeof("type"),(void **) &pvalue)== SUCCESS) {
				SEPARATE_ZVAL(pvalue);
				convert_to_long(pvalue);
				bod->type = (*pvalue)->value.lval;
			}
			if(zend_hash_find(data->value.ht,"encoding",sizeof("encoding"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_long(*pvalue);
				bod->encoding = (*pvalue)->value.lval;
			}
			if(zend_hash_find(data->value.ht,"subtype",sizeof("subtype"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_string(*pvalue);
				bod->subtype=cpystr((*pvalue)->value.str.val);
			}
			if(zend_hash_find(data->value.ht,"id",sizeof("id"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_string(*pvalue);
				bod->id=cpystr((*pvalue)->value.str.val);
			}
			if(zend_hash_find(data->value.ht,"contents.data",sizeof("contents.data"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_string(*pvalue);     
				bod->contents.text.data=(char *)fs_get((*pvalue)->value.str.len);
				memcpy(bod->contents.text.data,(*pvalue)->value.str.val,(*pvalue)->value.str.len+1);
				bod->contents.text.size=(*pvalue)->value.str.len;
			}
			if(zend_hash_find(data->value.ht,"lines",sizeof("lines"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_long(*pvalue);
				bod->size.lines=(*pvalue)->value.lval;
			}
			if(zend_hash_find(data->value.ht,"bytes",sizeof("bytes"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_long(*pvalue);
				bod->size.bytes=(*pvalue)->value.lval;
			}
			if(zend_hash_find(data->value.ht,"md5",sizeof("md5"),(void **) &pvalue)== SUCCESS){
				SEPARATE_ZVAL(pvalue);
				convert_to_string(*pvalue);
				bod->md5=cpystr((*pvalue)->value.str.val);
			}
			zend_hash_move_forward(body->value.ht);
		}
	}

	rfc822_encode_body_7bit (env,topbod); 
	rfc822_header (tmp,env,topbod);  
	mystring=emalloc(strlen(tmp)+1);
	strcpy(mystring,tmp);

	bod=topbod;
	switch (bod->type) {
		case TYPEMULTIPART:           /* multipart gets special handling */
			part = bod->nested.part;   /* first body part */
										/* find cookie */
			for (param = bod->parameter; param && !cookie; param = param->next) {
				if (!strcmp (param->attribute,"BOUNDARY")) {
					cookie = param->value;
				}
			}
			if (!cookie) {
				cookie = "-";  /* yucky default */
			}
			do {                        /* for each part */
                    /* build cookie */
				sprintf (t=tmp,"--%s\015\012",cookie);
										/* append mini-header */
				rfc822_write_body_header (&t,&part->body);
				strcat (t,"\015\012");    /* write terminating blank line */
										/* output cookie, mini-header, and contents */
				tempstring=emalloc(strlen(mystring)+strlen(tmp)+1);
				strcpy(tempstring,mystring);
				efree(mystring);
				mystring=tempstring;
				strcat(mystring,tmp);

				bod=&part->body;

				tempstring=emalloc(strlen(bod->contents.text.data)+strlen(mystring)+1);
				strcpy(tempstring,mystring);
				efree(mystring);
				mystring=tempstring;
				strcat(mystring,bod->contents.text.data);

			} while ((part = part->next));/* until done */
            /* output trailing cookie */

			sprintf(tmp,"--%s--",cookie);
			tempstring=emalloc(strlen(tmp)+strlen(mystring)+1);
			strcpy(tempstring,mystring);
			efree(mystring);
			mystring=tempstring;
			strcat(mystring,tmp);
			break;
		default:                      /* all else is text now */
			tempstring=emalloc(strlen(bod->contents.text.data)+strlen(mystring)+1);
			strcpy(tempstring,mystring);
			efree(mystring);
			mystring=tempstring;
			strcat(mystring,bod->contents.text.data);
			break;
	}

	RETURN_STRINGL(mystring,strlen(mystring),1);  
}
/* }}} */



/* Interfaces to C-client */


void mm_searched (MAILSTREAM *stream,unsigned long number)
{
}


void mm_exists (MAILSTREAM *stream,unsigned long number)
{
}


void mm_expunged (MAILSTREAM *stream,unsigned long number)
{
}


void mm_flags (MAILSTREAM *stream,unsigned long number)
{
}


void mm_notify (MAILSTREAM *stream,char *string,long errflg)
{
}

void mm_list (MAILSTREAM *stream,DTYPE delimiter,char *mailbox,long attributes)
{
	STRINGLIST *cur=NIL;
	if(!(attributes & LATT_NOSELECT)) {
		if (imap_folders == NIL) {
			imap_folders=mail_newstringlist();
			imap_folders->LSIZE=strlen(imap_folders->LTEXT=cpystr(mailbox));
			imap_folders->next=NIL; 
		} else {
			cur=imap_folders;
			while (cur->next != NIL ) {
				cur=cur->next;
			}
			cur->next=mail_newstringlist ();
			cur=cur->next;
			cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
			cur->next = NIL;
		}
	}
}

void mm_lsub (MAILSTREAM *stream,DTYPE delimiter,char *mailbox,long attributes)
{
	STRINGLIST *cur=NIL;
	if (imap_sfolders == NIL) {
		imap_sfolders=mail_newstringlist();
		imap_sfolders->LSIZE=strlen(imap_sfolders->LTEXT=cpystr(mailbox));
		imap_sfolders->next=NIL; 
	} else {
		cur=imap_sfolders;
		while (cur->next != NIL ) {
			cur=cur->next;
		}
		cur->next=mail_newstringlist ();
		cur=cur->next;
		cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
		cur->next = NIL;
	}
}

void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
{
status_flags=status->flags;
  if(status_flags & SA_MESSAGES) status_messages=status->messages;
  if(status_flags & SA_RECENT) status_recent=status->recent;
  if(status_flags & SA_UNSEEN) status_unseen=status->unseen;
  if(status_flags & SA_UIDNEXT) status_uidnext=status->uidnext;
  if(status_flags & SA_UIDVALIDITY) status_uidvalidity=status->uidvalidity;

}

void mm_log (char *string,long errflg)
{
	switch ((short) errflg) {
	case NIL:
		/* php_error(E_NOTICE,string); messages */
		break;
	case PARSE:
	case WARN:
		php_error(E_WARNING,string); /* warnings */
		break;
	case ERROR:
		php_error(E_NOTICE,string);   /* errors */
		break;
	}
}

void mm_dlog (char *string)
{
	php_error(E_NOTICE,string);
}

void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
{
	if (*mb->user) {
		strcpy (user,mb->user);
	} else {
		strcpy (user,imap_user);
	}
	strcpy (pwd,imap_password);
}


void mm_critical (MAILSTREAM *stream)
{
}


void mm_nocritical (MAILSTREAM *stream)
{
}


long mm_diskerror (MAILSTREAM *stream,long errcode,long serious)
{
	return 1;
}


void mm_fatal (char *string)
{
}

#endif
