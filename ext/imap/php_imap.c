/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
   |          Chuck Hagenbuch     <chuck@horde.org>                       |
   |          Andrew Skalski      <askalski@chekinc.com>                  |
   |          Hartmut Holzgraefe  <hartmut@six.de>                        |
   | PHP 4.0 updates:  Zeev Suraski <zeev@zend.com>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#define IMAP41

#undef OP_RELOGIN

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#ifdef ERROR
#undef ERROR
#endif
#include "php_imap.h"

ZEND_DECLARE_MODULE_GLOBALS(imap)

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#ifdef PHP_WIN32
#include "winsock.h"
MAILSTREAM DEFAULTPROTO;
#endif


#define PHP_EXPUNGE 32768

static void _php_make_header_object(zval *myzvalue, ENVELOPE *en);
static void _php_imap_add_body(zval *arg, BODY *body);
static void _php_imap_parse_address(ADDRESS *addresslist, char *fulladdress, zval *paddress);


/* These function declarations are missing from the IMAP header files... */
void rfc822_date(char *date);
char *cpystr(const char *str);
char *cpytxt(SIZEDTEXT *dst, char *text, unsigned long size);
long utf8_mime2text(SIZEDTEXT *src, SIZEDTEXT *dst);
unsigned long find_rightmost_bit(unsigned long *valptr);
void fs_give(void **block);
void *fs_get(size_t size);
int imap_mail(char *to, char *subject, char *message, char *headers, char *cc, char *bcc, char *rpath);


void mail_close_it(pils *imap_le_struct);
#ifdef OP_RELOGIN
/* AJS: close persistent connection */
void mail_userlogout_it(pils *imap_le_struct);
void mail_nuke_chain(pils **headp);
#endif
 
function_entry imap_functions[] = {
	PHP_FE(imap_open,			NULL)
	PHP_FE(imap_popen,			NULL)
	PHP_FE(imap_reopen,			NULL)
	PHP_FE(imap_num_msg,		NULL)
	PHP_FE(imap_num_recent,		NULL)
	PHP_FE(imap_headers,		NULL)
	PHP_FALIAS(imap_header, imap_headerinfo, NULL)
	PHP_FE(imap_headerinfo,		NULL)
	PHP_FE(imap_rfc822_parse_headers,	NULL)
	PHP_FE(imap_body,			NULL)
	PHP_FE(imap_fetchstructure,	NULL)
	PHP_FE(imap_fetchbody,		NULL)
	PHP_FE(imap_expunge,		NULL)
	PHP_FE(imap_delete,			NULL)
	PHP_FE(imap_undelete,		NULL)
	PHP_FE(imap_check,			NULL)
	PHP_FE(imap_close,			NULL)
	PHP_FE(imap_mail_copy,		NULL)
	PHP_FE(imap_mail_move,		NULL)
	PHP_FE(imap_createmailbox,	NULL)
	PHP_FE(imap_renamemailbox,	NULL)
	PHP_FE(imap_deletemailbox,	NULL)
	PHP_FALIAS(imap_listmailbox, imap_list,	NULL)
	PHP_FALIAS(imap_getmailboxes, imap_list_full,	NULL)
	PHP_FALIAS(imap_scanmailbox, imap_listscan,	NULL)
	PHP_FALIAS(imap_listsubscribed, imap_lsub,	NULL)
	PHP_FALIAS(imap_getsubscribed, imap_lsub_full,	NULL)
	PHP_FE(imap_subscribe,		NULL)
	PHP_FE(imap_unsubscribe,	NULL)
	PHP_FE(imap_append,			NULL)
	PHP_FE(imap_ping,			NULL)
	PHP_FE(imap_base64,			NULL)
	PHP_FE(imap_qprint,			NULL)
	PHP_FE(imap_8bit,			NULL)
	PHP_FE(imap_binary,			NULL)
	PHP_FE(imap_mailboxmsginfo,	NULL)
	PHP_FE(imap_rfc822_write_address,		NULL)
	PHP_FE(imap_rfc822_parse_adrlist,		NULL)
	PHP_FE(imap_setflag_full,	NULL)
	PHP_FE(imap_clearflag_full,	NULL)
	PHP_FE(imap_sort,			NULL)
	PHP_FE(imap_fetchheader,	NULL)
	PHP_FALIAS(imap_fetchtext,	imap_body,		NULL)
	PHP_FE(imap_uid,			NULL)
	PHP_FE(imap_msgno,			NULL)
	PHP_FE(imap_list,			NULL)
	PHP_FALIAS(imap_scan,		imap_listscan,			NULL)
	PHP_FE(imap_lsub,			NULL)
	PHP_FALIAS(imap_create,		imap_createmailbox,			NULL)
	PHP_FALIAS(imap_rename,		imap_renamemailbox,			NULL)
	PHP_FE(imap_status,			NULL)
	PHP_FE(imap_bodystruct,		NULL)
	PHP_FE(imap_fetch_overview,	NULL)
	PHP_FE(imap_mail_compose,	NULL)
	PHP_FE(imap_alerts,			NULL)
	PHP_FE(imap_errors,			NULL)
	PHP_FE(imap_last_error,		NULL)
#ifndef PHP_WIN32
	PHP_FE(imap_mail,           NULL)
#endif
	PHP_FE(imap_search,			NULL)
	PHP_FE(imap_utf7_decode,	NULL)
	PHP_FE(imap_utf7_encode,	NULL)
	PHP_FE(imap_utf8,       	NULL)
	PHP_FE(imap_mime_header_decode,	NULL)
	{NULL, NULL, NULL}
};

#ifdef OP_RELOGIN
#define IS_STREAM(ind_type)     ((ind_type)==le_imap || (ind_type)==le_pimap)
#else
#define IS_STREAM(ind_type)     ((ind_type)==le_imap)
#endif


zend_module_entry imap_module_entry = {
    "imap", imap_functions, PHP_MINIT(imap), NULL, PHP_RINIT(imap), PHP_RSHUTDOWN(imap), PHP_MINFO(imap), STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_IMAP
ZEND_GET_MODULE(imap)
#endif


static int le_imap;
#ifdef OP_RELOGIN
/* AJS: persistent connection type, chain pointer type */
static int le_pimap;
static int le_pimapchain;
#endif

#if HAVE_IMSP
/* This makes IMSP non thread safe */
extern char imsp_user[80];
extern char imsp_password[80];
#endif


void mail_close_it(pils *imap_le_struct)
{
	mail_close_full(imap_le_struct->imap_stream, imap_le_struct->flags);
	efree(imap_le_struct);
}

#ifdef OP_RELOGIN
/* AJS: stream close functions for persistent connections */
void mail_userlogout_it(pils *imap_le_struct)
{
	/* Close this user's session, putting the stream back
	 * into AUTHENTICATE state.  (Note that IMAP does not
	 * support this behavior... yet)
	 */
	imap_le_struct->busy = 0;
	mail_close_full(imap_le_struct->imap_stream, imap_le_struct->flags | CL_HALF);
}

void mail_nuke_chain(pils **headp)
{
	pils		*node, *next;

	for (node = *headp; node; node = next) {
		next = node->next;
		mail_close(node->imap_stream);
		free(node);
	}

	free(headp);
}
#endif

static int add_assoc_object(zval *arg, char *key, zval *tmp)
{
	HashTable *symtable;
	
	if (arg->type == IS_OBJECT) {
		symtable = arg->value.obj.properties;
	} else {
		symtable = arg->value.ht;
	}
	return zend_hash_update(symtable, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}

/* Mail instantiate FOBJECTLIST
 * Returns: new FOBJECTLIST list
 * Author: CJH
 */
FOBJECTLIST *mail_newfolderobjectlist(void)
{
  return (FOBJECTLIST *) memset(fs_get(sizeof(FOBJECTLIST)),0,
				sizeof(FOBJECTLIST));
}


/* Mail garbage collect FOBJECTLIST
 * Accepts: pointer to FOBJECTLIST pointer
 * Author: CJH
 */
void mail_free_foblist(FOBJECTLIST **foblist)
{
	if (*foblist) {		/* only free if exists */
		if ((*foblist)->text.data) {
			fs_give ((void **) &(*foblist)->text.data);
		}
		mail_free_foblist (&(*foblist)->next);
		fs_give ((void **) foblist);	/* return string to free storage */
	}
}


/* Mail instantiate ERRORLIST
 * Returns: new ERRORLIST list
 * Author: CJH
 */
ERRORLIST *mail_newerrorlist(void)
{
	return (ERRORLIST *) memset(fs_get(sizeof(ERRORLIST)), 0, sizeof(ERRORLIST));
}

/* Mail garbage collect FOBJECTLIST
 * Accepts: pointer to FOBJECTLIST pointer
 * Author: CJH
 */
void mail_free_errorlist(ERRORLIST **errlist)
{
	if (*errlist) {		/* only free if exists */
		if ((*errlist)->text.data) {
			fs_give((void **) &(*errlist)->text.data);
		}
		mail_free_errorlist (&(*errlist)->next);
		fs_give((void **) errlist);	/* return string to free storage */
	}
}


/* Author: CJH */
PHP_RINIT_FUNCTION(imap)
{
	IMAPLS_FETCH();

	IMAPG(imap_errorstack) = NIL;
	IMAPG(imap_alertstack) = NIL;
	return SUCCESS;
}

/* Author: CJH */
PHP_RSHUTDOWN_FUNCTION(imap)
{
	ERRORLIST *ecur = NIL;
	STRINGLIST *acur = NIL;
	IMAPLS_FETCH();

	if (IMAPG(imap_errorstack) != NIL) {
		/* output any remaining errors at their original error level */
		ecur = IMAPG(imap_errorstack);
		while (ecur != NIL) {
			php_error(E_NOTICE, "%s (errflg=%d)", ecur->LTEXT, ecur->errflg);
			ecur = ecur->next;
		}
		mail_free_errorlist(&IMAPG(imap_errorstack));
	}

	if (IMAPG(imap_alertstack) != NIL) {
		/* output any remaining alerts at E_NOTICE level */
		acur = IMAPG(imap_alertstack);
		while (acur != NIL) {
			php_error(E_NOTICE, acur->LTEXT);
			acur = acur->next;
		}
		mail_free_stringlist(&IMAPG(imap_alertstack));
	}
	return SUCCESS;
}


inline int add_next_index_object(zval *arg, zval *tmp)
{
	HashTable *symtable;
	
	if (arg->type == IS_OBJECT) {
		symtable = arg->value.obj.properties;
	} else {
		symtable = arg->value.ht;
	}

	return zend_hash_next_index_insert(symtable, (void *) &tmp, sizeof(zval *), NULL); 
}


/* Mail instantiate MESSAGELIST
 * Returns: new MESSAGELIST list
 * Author: CJH
 */
MESSAGELIST *mail_newmessagelist(void)
{
	return (MESSAGELIST *) memset(fs_get(sizeof(MESSAGELIST)),0,
								  sizeof(MESSAGELIST));
}

/* Mail garbage collect MESSAGELIST
 * Accepts: pointer to MESSAGELIST pointer
 * Author: CJH
 */
void mail_free_messagelist(MESSAGELIST **msglist)
{
	if (*msglist) {		/* only free if exists */
		mail_free_messagelist (&(*msglist)->next);
		fs_give ((void **) msglist);	/* return string to free storage */
	}
}


PHP_MINFO_FUNCTION(imap)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "IMAP Support", "enabled");    
#ifdef IMAP41
	php_info_print_table_row(2, "IMAP c-Client Version", "4.1");               
#else
	php_info_print_table_row(2, "IMAP c-Client Version", "4.0"); 
#endif
	php_info_print_table_end();
}


static void php_imap_init_globals(zend_imap_globals *imap_globals)
{
	imap_globals->imap_user[0] = 0;
	imap_globals->imap_password[0] = 0;
	imap_globals->imap_folders=NIL;
	imap_globals->imap_sfolders=NIL;
	imap_globals->imap_alertstack=NIL;
	imap_globals->imap_errorstack=NIL;
	imap_globals->imap_messages=NIL;
	imap_globals->imap_folder_objects=NIL;
	imap_globals->imap_sfolder_objects=NIL;
	imap_globals->folderlist_style = FLIST_ARRAY;
}


PHP_MINIT_FUNCTION(imap)
{
	unsigned long sa_all =	SA_MESSAGES | SA_RECENT | SA_UNSEEN |
				SA_UIDNEXT | SA_UIDVALIDITY;

	ZEND_INIT_MODULE_GLOBALS(imap, php_imap_init_globals, NULL)


#if 1
#ifndef PHP_WIN32
	mail_link(&unixdriver);   /* link in the unix driver */
#endif
	mail_link(&imapdriver);      /* link in the imap driver */
	mail_link(&nntpdriver);      /* link in the nntp driver */
	mail_link(&pop3driver);      /* link in the pop3 driver */
#ifndef PHP_WIN32
	mail_link(&mhdriver);        /* link in the mh driver */
	mail_link(&mxdriver);        /* link in the mx driver */
#endif
	mail_link(&mbxdriver);       /* link in the mbx driver */
	mail_link(&tenexdriver);     /* link in the tenex driver */
	mail_link(&mtxdriver);       /* link in the mtx driver */
#ifndef PHP_WIN32
	mail_link(&mmdfdriver);      /* link in the mmdf driver */
	mail_link(&newsdriver);      /* link in the news driver */
	mail_link(&philedriver);     /* link in the phile driver */
	auth_link(&auth_log);        /* link in the log authenticator */
#endif
	mail_link(&dummydriver);     /* link in the dummy driver */
#else
	/* link in the c-client mail and auth drivers */
#include "linkage.c"
#endif

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

#ifdef SA_QUOTA
	sa_all |= SA_QUOTA;
        REGISTER_MAIN_LONG_CONSTANT("SA_QUOTA",SA_QUOTA , CONST_PERSISTENT | CONST_CS);
     /* Disk space taken up by mailbox. */
#endif
#ifdef SA_QUOTA_ALL
	sa_all |= SA_QUOTA_ALL;
        REGISTER_MAIN_LONG_CONSTANT("SA_QUOTA_ALL",SA_QUOTA_ALL , CONST_PERSISTENT | CONST_CS);
     /* Disk space taken up by all mailboxes owned by user. */
#endif
	REGISTER_MAIN_LONG_CONSTANT("SA_ALL", sa_all, CONST_PERSISTENT | CONST_CS);
     /* get all status information */
		


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
#ifdef OP_RELOGIN
    /* AJS: destructors for persistent connections */
    le_pimap = register_list_destructors(mail_userlogout_it, NULL);
    le_pimapchain = register_list_destructors(NULL, mail_nuke_chain);
#endif
	return SUCCESS;
}

void imap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval **mailbox;
	zval **user;
	zval **passwd;
	zval **options;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct;
	long flags=NIL;
	long cl_flags=NIL;
#ifdef OP_RELOGIN
	NETMBX netmbx;
	char *hashed_details = NULL;
	int hashed_details_length = 0;
#endif
	int ind;
	int myargc=ZEND_NUM_ARGS();
	IMAPLS_FETCH();
	
	if (myargc <3 || myargc >4 || zend_get_parameters_ex(myargc, &mailbox, &user, &passwd, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(mailbox);
	convert_to_string_ex(user);
	convert_to_string_ex(passwd);
	if (myargc ==4) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
	}
	strcpy(IMAPG(imap_user), Z_STRVAL_PP(user));
	strcpy(IMAPG(imap_password), Z_STRVAL_PP(passwd));

#ifdef OP_RELOGIN
	/* AJS: persistent connection handling */
	/* Cannot use a persistent connection if we cannot parse
	 * out the server's hostname.
	 */
	if (persistent
		&& !mail_valid_net_parse(Z_STRVAL_PP(mailbox), &netmbx)) {
		persistent = 0;
	}

	imap_stream = NIL;
	if (persistent) {
		list_entry	*le = NULL;
		list_entry	new_le;
		pils		**headp, *node;
		int		need_update = 0;

		hashed_details_length = sizeof("imap_") + strlen(netmbx.host);
		hashed_details = (char*) emalloc(hashed_details_length);
		sprintf(hashed_details, "imap_%s", netmbx.host);

		/* Check for an existing connection. */
		if (	(zend_hash_find(&EG(persistent_list),
					hashed_details,
					hashed_details_length,
					(void*) &le) == FAILURE) ||
			(le->type != le_pimapchain))
		{
			le = NULL;
		}

		/* Re-use existing connection if available. */
		node = NULL;
		headp = NULL;
		if (le) {
			headp = (pils**) le->ptr;

			/* find a non-busy connection */
			for (node=*headp; node; node=node->next)
				if (!node->busy)
					break;
		}

		/* If we found a node, do a relogin.  */
		if (node) {
			imap_stream = mail_open(node->imap_stream, Z_STRVAL_PP(mailbox), flags | OP_RELOGIN);
			if (imap_stream) {
				/* Ping the stream to see if it is
				 * still good. 
				 */
				if (!mail_ping(imap_stream)) {
					mail_close(imap_stream);
					imap_stream = NIL;
				}
			}
		}

		/* Get a fresh stream if we don't have one yet. */
		if (imap_stream == NIL) {
			/* Open a new connection. */
			imap_stream = mail_open(NIL, Z_STRVAL_PP(mailbox), flags | OP_RELOGIN);
		}

		/* Do we have a stream yet?  If not, bail. */
		if (imap_stream == NIL) {
			if (node) {
				/* unlink the node */
				if ((*node->prev = node->next))
					node->next->prev = node->prev;
				free(node);
				/* delete the hash entry if empty */
				if (*headp == NULL) {
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_length);
				}
 			}
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* Allocate a new node if none. */
		if (node == NULL) {
			/* Alloc new hash entry. */
			node = malloc(sizeof(pils));
			if (node == NULL) {
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* Allocate headp if it does not exist. */
			if (headp == NULL) {
				headp = calloc(1, sizeof(*headp));
				need_update = 1;
			}

			node->prev = headp;
			node->next = *headp;
			*headp = node;
		}


		/* Initialize the node. */
		node->busy = 1;
		node->imap_stream = imap_stream;
		node->flags = cl_flags;

		/* Update the hash. */
		new_le.type = le_pimapchain;
		new_le.ptr = headp;
		if (need_update
			&& zend_hash_update(&EG(persistent_list), hashed_details,
				hashed_details_length, &new_le,
				sizeof(new_le), NULL) == FAILURE) {
			/* unlink and free the new node */
			if ((*node->prev = node->next)) {
				node->next->prev = node->prev;
			}
			mail_close(node->imap_stream);
			free(node);

			free(headp);
			efree(hashed_details);
 			RETURN_FALSE;
		}

		efree(hashed_details);

		imap_le_struct = node;
	} else {
#endif
		imap_stream = mail_open(NIL, Z_STRVAL_PP(mailbox), flags);
		if (imap_stream == NIL) {
			php_error(E_WARNING, "Couldn't open stream %s\n", (*mailbox)->value.str.val);
			RETURN_FALSE;
		}

		imap_le_struct = emalloc(sizeof(pils));
		imap_le_struct->imap_stream = imap_stream;
		imap_le_struct->flags = cl_flags;	
#ifdef OP_RELOGIN
	}

	if (persistent)
		ind = zend_list_insert(imap_le_struct, le_pimap);
	else
#endif
		ind = zend_list_insert(imap_le_struct, le_imap);

	RETURN_LONG(ind);
}

/* {{{ proto int imap_open(string mailbox, string user, string password [, int options])
   Open an IMAP stream to a mailbox */
PHP_FUNCTION(imap_open)
{
	imap_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto int imap_popen(string mailbox, string user, string password [, int options])
   Open a persistant IMAP stream to a mailbox */
PHP_FUNCTION(imap_popen)
{
#ifdef OP_RELOGIN
	imap_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
	RETURN_TRUE;
#else
	php_error(E_WARNING, "Persistent IMAP connections are not yet supported.\n");
	RETURN_FALSE;
#endif
}
/* }}} */

 
/* {{{ proto int imap_reopen(int stream_id, string mailbox [, int options])
   Reopen an IMAP stream to a new mailbox */
PHP_FUNCTION(imap_reopen)
{
	zval **streamind;
	zval **mailbox;
	zval **options;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct; 
	int ind, ind_type;
	long flags=NIL;
	long cl_flags=NIL;
	int myargc=ZEND_NUM_ARGS();

	if (myargc<2 || myargc>3 || zend_get_parameters_ex(myargc, &streamind, &mailbox, &options) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
    }

	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	convert_to_string_ex(mailbox);
	if (myargc == 3) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		imap_le_struct->flags = cl_flags;	
	}
	imap_stream = mail_open(imap_le_struct->imap_stream, (*mailbox)->value.str.val, flags);
	if (imap_stream == NIL) {
		php_error(E_WARNING, "Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	imap_le_struct->imap_stream = imap_stream;
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int imap_append(int stream_id, string folder, string message [, string flags])
   Append a new message to a specified mailbox */
PHP_FUNCTION(imap_append)
{
	zval **streamind, **folder, **message, **flags;
	int ind, ind_type;
	pils *imap_le_struct; 
	STRING st;
	int myargc=ZEND_NUM_ARGS();
  
	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &folder, &message, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
  
	convert_to_long_ex(streamind);
	convert_to_string_ex(folder);
	convert_to_string_ex(message);
	if (myargc == 4) {
		convert_to_string_ex(flags);
	}
	ind = Z_LVAL_PP(streamind);
  
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);

	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	INIT (&st, mail_string, (void *) (*message)->value.str.val, (*message)->value.str.len);
	if (mail_append_full(imap_le_struct->imap_stream, (*folder)->value.str.val, myargc==4?(*flags)->value.str.val:NIL, NIL, &st)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int imap_num_msg(int stream_id)
   Gives the number of messages in the current mailbox */
PHP_FUNCTION(imap_num_msg)
{
	zval **streamind;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);

	if (!imap_le_struct || !IS_STREAM(ind_type)) {
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
	zval **streamind;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETURN_LONG(mail_ping(imap_le_struct->imap_stream));
}
/* }}} */


/* {{{ proto int imap_num_recent(int stream_id)
   Gives the number of recent messages in current mailbox */
PHP_FUNCTION(imap_num_recent)
{
	zval **streamind;
	int ind, ind_type;
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETURN_LONG(imap_le_struct->imap_stream->recent);
}
/* }}} */


/* {{{ proto int imap_expunge(int stream_id)
   Permanently delete all messages marked for deletion */
PHP_FUNCTION(imap_expunge)
{
	zval **streamind;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);

	if (!imap_le_struct || !IS_STREAM(ind_type)) {
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
	zval **options, **streamind;
	int ind, ind_type;
	pils *imap_le_struct=NULL; 
	int myargcount=ZEND_NUM_ARGS();
	long flags = NIL;

	if (myargcount < 1 || myargcount > 2 || zend_get_parameters_ex(myargcount, &streamind, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
    }
	if (myargcount==2) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
		/* Do the translation from PHP's internal PHP_EXPUNGE define to c-client's CL_EXPUNGE */
		if (flags & PHP_EXPUNGE) {
			flags ^= PHP_EXPUNGE;
			flags |= CL_EXPUNGE;
		}	
		imap_le_struct->flags = flags;
	}

	zend_list_delete(ind);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto array imap_headers(int stream_id)
   Returns headers for all messages in a mailbox */
PHP_FUNCTION(imap_headers)
{
	zval **streamind;
	int ind, ind_type;
	unsigned long i;
	char *t;
	unsigned int msgno;
	pils *imap_le_struct; 
	char tmp[MAILTMPLEN];
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream,msgno);
		mail_fetchstructure(imap_le_struct->imap_stream,msgno,NIL);
		tmp[0] = cache->recent ? (cache->seen ? 'R': 'N') : ' ';
		tmp[1] = (cache->recent | cache->seen) ? ' ' : 'U';
		tmp[2] = cache->flagged ? 'F' : ' ';
		tmp[3] = cache->answered ? 'A' : ' ';
		tmp[4] = cache->deleted ? 'D' : ' ';
		tmp[5] = cache->draft ? 'X' : ' ';
		sprintf(tmp + 6, "%4ld) ", cache->msgno);
		mail_date(tmp+11, cache);
		tmp[18] = ' ';
		tmp[19] = '\0';
		mail_fetchfrom(tmp+19, imap_le_struct->imap_stream, msgno, (long)20);
		strcat(tmp, " ");
		if ((i = cache->user_flags)) {
			strcat(tmp, "{");
			while (i) {
				strcat(tmp, imap_le_struct->imap_stream->user_flags[find_rightmost_bit (&i)]);
				if (i) strcat(tmp, " ");
			}
			strcat(tmp, "} ");
		}
		mail_fetchsubject(t = tmp + strlen(tmp), imap_le_struct->imap_stream, msgno, (long)25);
		sprintf(t += strlen(t), " (%ld chars)", cache->rfc822_size);
		add_next_index_string(return_value, tmp, 1);
	}
}
/* }}} */


/* {{{ proto string imap_body(int stream_id, int msg_no [, int options])
   Read the message body */
PHP_FUNCTION(imap_body)
{
	zval **streamind, **msgno, **flags;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc <2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_long_ex(msgno);
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}
	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext_full (imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL, myargc==3 ? Z_LVAL_PP(flags) : NIL), 1);
}
/* }}} */


/* {{{ proto string imap_fetchtext_full(int stream_id, int msg_no [, int options])
   Read the full text of a message */
PHP_FUNCTION(imap_fetchtext_full)
{
	zval **streamind, **msgno, **flags;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargcount = ZEND_NUM_ARGS();
	if (myargcount >3 || myargcount <2 || zend_get_parameters_ex(myargcount, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_long_ex(msgno);
	if (myargcount == 3) {
		convert_to_long_ex(flags);
	}
	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL, myargcount==3 ? Z_LVAL_PP(flags) : NIL), 1);
}
/* }}} */


/* {{{ proto int imap_mail_copy(int stream_id, int msg_no, string mailbox [, int options])
   Copy specified message to a mailbox */
PHP_FUNCTION(imap_mail_copy)
{
	zval **streamind, **seq, **folder, **options;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargcount = ZEND_NUM_ARGS();

	if (myargcount > 4 || myargcount < 3 
		|| zend_get_parameters_ex(myargcount, &streamind, &seq, &folder, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(seq);
	convert_to_string_ex(folder);
	if (myargcount == 4) {
		convert_to_long_ex(options);
	}
	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_copy_full(imap_le_struct->imap_stream, Z_STRVAL_PP(seq), Z_STRVAL_PP(folder), myargcount==4 ? Z_LVAL_PP(options) : NIL)==T) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int imap_mail_move(int stream_id, int msg_no, string mailbox [, int options])
   Move specified message to a mailbox */
PHP_FUNCTION(imap_mail_move)
{
	zval **streamind, **seq, **folder, **options;
	int ind, ind_type;
	pils *imap_le_struct; 
	int myargcount = ZEND_NUM_ARGS();

	if (myargcount > 4 || myargcount < 3 
		|| zend_get_parameters_ex(myargcount, &streamind, &seq, &folder, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(seq);
	convert_to_string_ex(folder);
	if (myargcount == 4) {
		convert_to_long_ex(options);
	}
		
	ind = Z_LVAL_PP(streamind);
	
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_copy_full(imap_le_struct->imap_stream, Z_STRVAL_PP(seq), Z_STRVAL_PP(folder), myargcount==4 ? (Z_LVAL_PP(options) | CP_MOVE) : CP_MOVE) == T) {
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
	zval **streamind, **folder;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(folder);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_create(imap_le_struct->imap_stream, Z_STRVAL_PP(folder))==T) {
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
	zval **streamind, **old_mailbox, **new_mailbox;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS()!=3 || zend_get_parameters_ex(3, &streamind, &old_mailbox, &new_mailbox)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(old_mailbox);
	convert_to_string_ex(new_mailbox);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_rename(imap_le_struct->imap_stream, Z_STRVAL_PP(old_mailbox), Z_STRVAL_PP(new_mailbox))==T) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int imap_deletemailbox(int stream_id, string mailbox)
   Delete a mailbox */
PHP_FUNCTION(imap_deletemailbox)
{
	zval **streamind, **folder;
	int ind, ind_type;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(folder);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_delete(imap_le_struct->imap_stream, Z_STRVAL_PP(folder))==T) {
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
	zval **streamind, **ref, **pat;
	int ind, ind_type;
	pils *imap_le_struct; 
	STRINGLIST *cur=NIL;
	IMAPLS_FETCH();

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;
	
	if (ZEND_NUM_ARGS()!=3 
		|| zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(ref);
	convert_to_string_ex(pat);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	IMAPG(imap_folders) = NIL;
	mail_list(imap_le_struct->imap_stream, Z_STRVAL_PP(ref), Z_STRVAL_PP(pat));
	if (IMAPG(imap_folders) == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
    cur=IMAPG(imap_folders);
    while (cur != NIL) {
		add_next_index_string(return_value, cur->LTEXT, 1);
        cur=cur->next;
    }
	mail_free_stringlist (&IMAPG(imap_folders));
}

/* }}} */


/* {{{ proto array imap_getmailboxes(int stream_id, string ref, string pattern)
   Reads the list of mailboxes and returns a full array of objects containing name, attributes, and delimiter */
/* Author: CJH */
PHP_FUNCTION(imap_list_full)
{
	zval **streamind, **ref, **pat, *mboxob;
	int ind, ind_type;
	pils *imap_le_struct; 
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;
	IMAPLS_FETCH();
	
	/* set flag for new, improved array of objects mailbox list */
	IMAPG(folderlist_style) = FLIST_OBJECT;
	
	if (ZEND_NUM_ARGS()!=3 
		|| zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(ref);
	convert_to_string_ex(pat);
	
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	IMAPG(imap_folder_objects) = NIL;
	mail_list(imap_le_struct->imap_stream, Z_STRVAL_PP(ref), Z_STRVAL_PP(pat));
	if (IMAPG(imap_folder_objects) == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	delim = emalloc(2 * sizeof(char));
	cur=IMAPG(imap_folder_objects);
	while (cur != NIL) {
		MAKE_STD_ZVAL(mboxob);
		object_init(mboxob);
		add_property_string(mboxob, "name", cur->LTEXT, 1);
		add_property_long(mboxob, "attributes", cur->attributes);
#ifdef IMAP41
		delim[0] = (char)cur->delimiter;
		delim[1] = 0;
		add_property_string(mboxob, "delimiter", delim, 1);
#else
		add_property_string(mboxob, "delimiter", cur->delimiter, 1);
#endif
		add_next_index_object(return_value, mboxob);
		cur=cur->next;
	}
	mail_free_foblist(&IMAPG(imap_folder_objects));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY;		/* reset to default */
}
/* }}} */


/* {{{ proto array imap_scan(int stream_id, string ref, string pattern, string content)
   Read list of mailboxes containing a certain string */
PHP_FUNCTION(imap_listscan)
{
	zval **streamind, **ref, **pat, **content;
	int ind, ind_type;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;
	IMAPLS_FETCH();

	if (ZEND_NUM_ARGS()!=4 || zend_get_parameters_ex(4, &streamind, &ref, &pat, &content) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(ref);
	convert_to_string_ex(pat);
	convert_to_string_ex(content);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	IMAPG(imap_folders) = NIL;
	mail_scan(imap_le_struct->imap_stream, Z_STRVAL_PP(ref), Z_STRVAL_PP(pat), Z_STRVAL_PP(content));
	if (IMAPG(imap_folders) == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
	cur=IMAPG(imap_folders);
	while (cur != NIL) {
		add_next_index_string(return_value, cur->LTEXT, 1);
		cur=cur->next;
	}
	mail_free_stringlist (&IMAPG(imap_folders));
}

/* }}} */


/* {{{ proto object imap_check(int stream_id)
   Get mailbox properties */
PHP_FUNCTION(imap_check)
{
	zval **streamind;
	int ind, ind_type;
	pils *imap_le_struct;
	char date[100];

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	if (mail_ping (imap_le_struct->imap_stream) == NIL) {
		RETURN_FALSE;
    }
	if (imap_le_struct->imap_stream && imap_le_struct->imap_stream->mailbox) {
		rfc822_date(date);
		object_init(return_value);
		add_property_string(return_value, "Date", date, 1);
		add_property_string(return_value, "Driver", imap_le_struct->imap_stream->dtb->name, 1);
		add_property_string(return_value, "Mailbox", imap_le_struct->imap_stream->mailbox, 1);
		add_property_long(return_value, "Nmsgs", imap_le_struct->imap_stream->nmsgs);
		add_property_long(return_value, "Recent", imap_le_struct->imap_stream->recent);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int imap_delete(int stream_id, int msg_no [, int flags])
   Mark a message for deletion */
PHP_FUNCTION(imap_delete)
{
	zval **streamind, **sequence, **flags;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();
	
	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(sequence);
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}
	
	ind = Z_LVAL_PP(streamind);
	
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_setflag_full(imap_le_struct->imap_stream, Z_STRVAL_PP(sequence), "\\DELETED", myargc==3 ? Z_LVAL_PP(flags) : NIL);
	RETVAL_TRUE;
}
/* }}} */


/* {{{ proto int imap_undelete(int stream_id, int msg_no)
   Remove the delete flag from a message */
PHP_FUNCTION(imap_undelete)
{
	zval **streamind, **sequence, **flags;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(sequence);
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_clearflag_full(imap_le_struct->imap_stream, Z_STRVAL_PP(sequence), "\\DELETED", myargc==3 ? Z_LVAL_PP(flags) : NIL);
	RETVAL_TRUE;
}
/* }}} */


/* {{{ proto object imap_headerinfo(int stream_id, int msg_no [, int from_length [, int subject_length [, string default_host]]])
   Read the headers of the message */
PHP_FUNCTION(imap_headerinfo)
{
	zval **streamind, **msgno, **fromlength, **subjectlength, **defaulthost;
	int ind, ind_type;
	unsigned long length;
	pils *imap_le_struct;
	MESSAGECACHE *cache;
	char *mystring;
	char dummy[2000];
	ENVELOPE *en;
	char fulladdress[MAILTMPLEN];
	
	int myargc = ZEND_NUM_ARGS();
	if (myargc < 2 || myargc > 5 || zend_get_parameters_ex(myargc, &streamind, &msgno, &fromlength, &subjectlength, &defaulthost)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(msgno);
	if (myargc >= 3) {
		convert_to_long_ex(fromlength); 
	} else {
		fromlength = 0x00;
	}
	if (myargc >= 4) {
		convert_to_long_ex(subjectlength);
	} else {
		subjectlength = 0x00;
	}
	if (myargc == 5) {
		convert_to_string_ex(defaulthost);
	}
	
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if (!Z_LVAL_PP(msgno) || Z_LVAL_PP(msgno) < 1 || (unsigned) Z_LVAL_PP(msgno) > imap_le_struct->imap_stream->nmsgs) {
		php_error(E_WARNING, "Bad message number");
		RETURN_FALSE;
	}
	
	if (mail_fetchstructure(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL)) {
		cache = mail_elt(imap_le_struct->imap_stream, Z_LVAL_PP(msgno));
	} else {
		RETURN_FALSE;
	}
	
    mystring = mail_fetchheader_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL, &length, NIL);
	if (myargc == 5) {
		rfc822_parse_msg(&en, NULL, mystring, length, NULL, Z_STRVAL_PP(defaulthost), NIL);
	} else {
		rfc822_parse_msg(&en, NULL, mystring, length, NULL, "UNKNOWN", NIL);
	}
	
	/* call a function to parse all the text, so that we can use the
       same function to parse text from other sources */
	_php_make_header_object(return_value, en);
	
	/* now run through properties that are only going to be returned
       from a server, not text headers */
	add_property_string(return_value, "Recent", cache->recent ? (cache->seen ? "R": "N") : " ", 1);
	add_property_string(return_value, "Unseen", (cache->recent | cache->seen) ? " " : "U", 1);
	add_property_string(return_value, "Flagged", cache->flagged ? "F" : " ", 1);
	add_property_string(return_value, "Answered", cache->answered ? "A" : " ", 1);
	add_property_string(return_value, "Deleted", cache->deleted ? "D" : " ", 1);
	add_property_string(return_value, "Draft", cache->draft ? "X" : " ", 1);
	
	sprintf(dummy, "%4ld", cache->msgno);
	add_property_string(return_value, "Msgno", dummy, 1);
	
	mail_date(dummy, cache);
	add_property_string(return_value, "MailDate", dummy, 1);
	
	sprintf(dummy, "%ld", cache->rfc822_size); 
	add_property_string(return_value, "Size", dummy, 1);
	
	add_property_long(return_value, "udate", mail_longdate(cache));
	
	if (en->from && fromlength) {
		fulladdress[0] = 0x00;
		mail_fetchfrom(fulladdress, imap_le_struct->imap_stream, Z_LVAL_PP(msgno), Z_LVAL_PP(fromlength));
		add_property_string(return_value, "fetchfrom", fulladdress, 1);
	}
	if (en->subject && subjectlength) {
		fulladdress[0] = 0x00;
		mail_fetchsubject(fulladdress, imap_le_struct->imap_stream, Z_LVAL_PP(msgno), Z_LVAL_PP(subjectlength));
		add_property_string(return_value, "fetchsubject", fulladdress, 1);
	}
	mail_free_envelope(&en);
}
/* }}} */


/* {{{ proto object imap_rfc822_parse_headers(string headers [, string default_host])
   Parse a set of mail headers contained in a string, and return an object similar to imap_headerinfo() */
PHP_FUNCTION(imap_rfc822_parse_headers)
{
	zval **headers, **defaulthost;
	ENVELOPE *en;
	
	int myargc = ZEND_NUM_ARGS();
	if (myargc < 1 || myargc > 2 || zend_get_parameters_ex(myargc, &headers, &defaulthost) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_string_ex(headers);
	if (myargc == 2) {
		convert_to_string_ex(defaulthost);
	}
	
	if (myargc == 2) {
		rfc822_parse_msg(&en, NULL, (*headers)->value.str.val, (*headers)->value.str.len, NULL, (*defaulthost)->value.str.val, NIL);
	} else {
		rfc822_parse_msg(&en, NULL, (*headers)->value.str.val, (*headers)->value.str.len, NULL, "UNKNOWN", NIL);
	}
	
	/* call a function to parse all the text, so that we can use the
       same function no matter where the headers are from */
	_php_make_header_object(return_value, en);
	mail_free_envelope(&en);
}
/* }}} */


/* KMLANG */
/* {{{ proto array imap_lsub(int stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes */
PHP_FUNCTION(imap_lsub)
{
	zval **streamind, **ref, **pat;
	int ind, ind_type;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;
	IMAPLS_FETCH();

	
	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;
	
	if (ZEND_NUM_ARGS()!=3 || zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(ref);
	convert_to_string_ex(pat);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	IMAPG(imap_sfolders) = NIL;
	mail_lsub(imap_le_struct->imap_stream, Z_STRVAL_PP(ref), Z_STRVAL_PP(pat));
	if (IMAPG(imap_sfolders) == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
	cur=IMAPG(imap_sfolders);
	while (cur != NIL) {
		add_next_index_string(return_value, cur->LTEXT, 1);
		cur=cur->next;
	}
	mail_free_stringlist (&IMAPG(imap_sfolders));
}
/* }}} */


/* {{{ proto array imap_getsubscribed(int stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes, in the same format as imap_getmailboxes() */
/* Author: CJH */
PHP_FUNCTION(imap_lsub_full)
{
	zval **streamind, **ref, **pat, *mboxob;
	int ind, ind_type;
	pils *imap_le_struct;
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;
	IMAPLS_FETCH();
	
	delim = emalloc(2 * sizeof(char));
	
	/* set flag for new, improved array of objects list */
	IMAPG(folderlist_style) = FLIST_OBJECT;
	
	if (ZEND_NUM_ARGS()!=3 || zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(ref);
	convert_to_string_ex(pat);
	
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	IMAPG(imap_sfolder_objects) = NIL;
	mail_lsub(imap_le_struct->imap_stream, Z_STRVAL_PP(ref), Z_STRVAL_PP(pat));
	if (IMAPG(imap_sfolder_objects) == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	cur=IMAPG(imap_sfolder_objects);
	while (cur != NIL) {
		MAKE_STD_ZVAL(mboxob);
		object_init(mboxob);
		add_property_string(mboxob, "name", cur->LTEXT, 1);
		add_property_long(mboxob, "attributes", cur->attributes);
#ifdef IMAP41
		delim[0] = (char)cur->delimiter;
		delim[1] = 0;
		add_property_string(mboxob, "delimiter", delim, 1);
#else
		add_property_string(mboxob, "delimiter", cur->delimiter, 1);
#endif
		add_next_index_object(return_value, mboxob);
		cur=cur->next;
	}
	mail_free_foblist (&IMAPG(imap_sfolder_objects));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY; /* reset to default */
}
/* }}} */


/* {{{ proto int imap_subscribe(int stream_id, string mailbox)
   Subscribe to a mailbox */
PHP_FUNCTION(imap_subscribe)
{
	zval **streamind, **folder;
	int ind, ind_type;
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(folder);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_subscribe(imap_le_struct->imap_stream, Z_STRVAL_PP(folder))==T) {
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
	zval **streamind, **folder;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();
	if (myargc !=2 || zend_get_parameters_ex(myargc, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_string_ex(folder);
	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (mail_unsubscribe(imap_le_struct->imap_stream, Z_STRVAL_PP(folder))==T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto object imap_fetchstructure(int stream_id, int msg_no [, int options])
   Read the full structure of a message */
PHP_FUNCTION(imap_fetchstructure)
{
	zval **streamind, **msgno, **flags;
	int ind, ind_type;
	pils *imap_le_struct;
	BODY *body;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 2  || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(msgno);
	if (Z_LVAL_PP(msgno) < 1) {
		RETURN_FALSE;
	}
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}
	object_init(return_value);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);

	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	mail_fetchstructure_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), &body ,myargc == 3 ? Z_LVAL_PP(flags) : NIL);
	
	if (!body) {
		php_error(E_WARNING, "No body information available");
		RETURN_FALSE;
	}
	
	_php_imap_add_body(return_value, body);
}
/* }}} */


/* {{{ proto string imap_fetchbody(int stream_id, int msg_no, int section [, int options])
   Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	zval **streamind, **msgno, **sec, **flags;
	int ind, ind_type;
	pils *imap_le_struct;
	char *body;
	unsigned long len;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 3 || myargc >4 || zend_get_parameters_ex(myargc, &streamind, &msgno, &sec, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);
	convert_to_long_ex(msgno);
	convert_to_string_ex(sec);
	if (myargc == 4) {
		convert_to_long_ex(flags);
	}
	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);

	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
	body = mail_fetchbody_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), Z_STRVAL_PP(sec), &len, myargc==4 ? Z_LVAL_PP(flags) : NIL);

	if (!body) {
		php_error(E_WARNING, "No body information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL(body, len, 1);
}

/* }}} */


/* {{{ proto string imap_base64(string text)
   Decode BASE64 encoded text */
PHP_FUNCTION(imap_base64)
{
	zval **text;
	char *decode;
	unsigned long newlength;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &text) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(text);

	decode = (char *) rfc822_base64((unsigned char *) Z_STRVAL_PP(text), Z_STRLEN_PP(text), &newlength);
	RETVAL_STRINGL(decode, newlength, 1);
	fs_give((void**) &decode);
}
/* }}} */


/* {{{ proto string imap_qprint(string text)
   Convert a quoted-printable string to an 8-bit string */
PHP_FUNCTION(imap_qprint)
{
	zval **text;
	char *decode;
	unsigned long newlength;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &text) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(text);

	decode = (char *) rfc822_qprint((unsigned char *) Z_STRVAL_PP(text), Z_STRLEN_PP(text), &newlength);
	RETVAL_STRINGL(decode, newlength, 1);
	fs_give((void**) &decode);
}
/* }}} */


/* {{{ proto string imap_8bit(string text)
   Convert an 8-bit string to a quoted-printable string */
PHP_FUNCTION(imap_8bit)
{
	zval **text;
	char *decode;
	unsigned long newlength;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &text) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(text);

	decode = (char *) rfc822_8bit((unsigned char *) Z_STRVAL_PP(text), Z_STRLEN_PP(text), &newlength);
	RETVAL_STRINGL(decode, newlength, 1);
	fs_give((void**) &decode);
}
/* }}} */


/* {{{ proto string imap_binary(string text)
   Convert an 8bit string to a base64 string */
PHP_FUNCTION(imap_binary)
{
	zval **text;
	unsigned long len;
	char *decode;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &text) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(text);
	decode = rfc822_binary(Z_STRVAL_PP(text), Z_STRLEN_PP(text), &len);
	RETVAL_STRINGL(decode, len, 1);
	fs_give((void**) &decode);
}
/* }}} */


/* {{{ proto object imap_mailboxmsginfo(int stream_id)
   Returns info about the current mailbox */
PHP_FUNCTION(imap_mailboxmsginfo)
{
	zval **streamind;
	char date[100];
	int ind, ind_type;
	unsigned int msgno;
	pils *imap_le_struct;
	unsigned unreadmsg, deletedmsg, msize;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(streamind);

	ind = Z_LVAL_PP(streamind);

	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);

	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return object */
	if (object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	unreadmsg = 0;
	deletedmsg = 0;
	msize = 0;
	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream,msgno);
		mail_fetchstructure (imap_le_struct->imap_stream,msgno,NIL);
                if (!cache->seen || cache->recent) unreadmsg++;
                if (cache->deleted) deletedmsg++;
		msize = msize + cache->rfc822_size;
	}
	add_property_long(return_value, "Unread", unreadmsg);
        add_property_long(return_value, "Deleted", deletedmsg);
	add_property_long(return_value, "Nmsgs", imap_le_struct->imap_stream->nmsgs);
	add_property_long(return_value, "Size", msize);
	rfc822_date(date);
	add_property_string(return_value, "Date", date, 1);
	add_property_string(return_value, "Driver", imap_le_struct->imap_stream->dtb->name, 1);
	add_property_string(return_value, "Mailbox", imap_le_struct->imap_stream->mailbox, 1);
	add_property_long(return_value, "Recent", imap_le_struct->imap_stream->recent);
}
/* }}} */


/* {{{ proto string imap_rfc822_write_address(string mailbox, string host, string personal)
   Returns a properly formatted email address given the mailbox, host, and personal info */
PHP_FUNCTION(imap_rfc822_write_address)
{
	zval **mailbox, **host, **personal;
	ADDRESS *addr;
	char string[MAILTMPLEN];
	int argc;
	argc=ZEND_NUM_ARGS();

	if (argc != 3 || zend_get_parameters_ex(argc, &mailbox, &host, &personal) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(mailbox);
	convert_to_string_ex(host);
	convert_to_string_ex(personal);
	addr=mail_newaddr();
	if (mailbox) {
		addr->mailbox = cpystr(Z_STRVAL_PP(mailbox));
	}
	if (host) {
		addr->host = cpystr(Z_STRVAL_PP(host));
	}
	if (personal) {
		addr->personal = cpystr(Z_STRVAL_PP(personal));
	}
	addr->next=NIL;
	addr->error=NIL;
	addr->adl=NIL;

	string[0]='\0';
	rfc822_write_address(string, addr);
	RETVAL_STRING(string, 1);
}
/* }}} */


/* {{{ proto array imap_rfc822_parse_adrlist(string address_string, string default_host)
   Parses an address string */
PHP_FUNCTION(imap_rfc822_parse_adrlist)
{
	zval *str, *defaulthost, *tovals;
	ADDRESS *addresstmp;
	ENVELOPE *env;
	int argc;
	
	env=mail_newenvelope();
	argc=ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters(ht, argc, &str, &defaulthost) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string(str);
	convert_to_string(defaulthost);
	rfc822_parse_adrlist(&env->to, str->value.str.val, defaulthost->value.str.val);
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	addresstmp=env->to;
	if (addresstmp) do {
		MAKE_STD_ZVAL(tovals);
		object_init(tovals);
		if (addresstmp->mailbox) {
			add_property_string(tovals, "mailbox", addresstmp->mailbox, 1);
		}
		if (addresstmp->host) {
			add_property_string(tovals, "host", addresstmp->host, 1);
		}
		if (addresstmp->personal) {
			add_property_string(tovals, "personal", addresstmp->personal, 1);
		}
		if (addresstmp->adl) {
			add_property_string(tovals, "adl", addresstmp->adl, 1);
		}
		add_next_index_object(return_value, tovals);
	} while ((addresstmp = addresstmp->next));
}
/* }}} */


/* {{{ proto string imap_utf8(string string)
   Convert a string to UTF-8 */
PHP_FUNCTION(imap_utf8)
{
	zval **str;
	int argc;
	SIZEDTEXT src, dest;
	
	src.data  = NULL;
	src.size  = 0;
	dest.data = NULL;
	dest.size = 0;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc, &str) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_string_ex(str);
	cpytxt(&src, Z_STRVAL_PP(str), Z_STRLEN_PP(str));
	utf8_mime2text(&src, &dest);
	RETURN_STRINGL(dest.data, strlen(dest.data), 1);
}
/* }}} */


/* macros for the modified utf7 conversion functions */
/* author: Andrew Skalski <askalski@chek.com> */

/* tests `c' and returns true if it is a special character */
#define SPECIAL(c) ((c) <= 0x1f || (c) >= 0x7f)
/* validate a modified-base64 character */
#define B64CHAR(c) (isalnum(c) || (c) == '+' || (c) == ',')
/* map the low 64 bits of `n' to the modified-base64 characters */
#define B64(n)  ("ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                "abcdefghijklmnopqrstuvwxyz0123456789+,"[(n) & 0x3f])
/* map the modified-base64 character `c' to its 64 bit value */
#define UNB64(c)        ((c) == '+' ? 62 : (c) == ',' ? 63 : (c) >= 'a' ? \
                        (c) - 71 : (c) >= 'A' ? (c) - 65 : (c) + 4)

/* {{{ proto string imap_utf7_decode(string buf)
   Decode a modified UTF-7 string */
PHP_FUNCTION(imap_utf7_decode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	int argc;
	zval **arg;
	const unsigned char *in, *inp, *endp;
	unsigned char *out, *outp;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_DECODE0,	/* encoded text rotation... */
		ST_DECODE1,
		ST_DECODE2,
		ST_DECODE3
	} state;

	/* collect arguments */
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc, &arg) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_writable_string_ex(arg);		/* Is this string really modified? */
	in = (const unsigned char *) Z_STRVAL_PP(arg);
	inlen = Z_STRLEN_PP(arg);
	
	/* validate and compute length of output string */
	outlen = 0;
	state = ST_NORMAL;
	for (endp = (inp = in) + inlen; inp < endp; inp++) {
		if (state == ST_NORMAL) {
			/* process printable character */
			if (SPECIAL(*inp)) {
				php_error(E_WARNING, "imap_utf7_decode: "
					"Invalid modified UTF-7 character: "
					"`%c'", *inp);
				RETURN_FALSE;
			} else if (*inp != '&') {
				outlen++;
			} else if (inp + 1 == endp) {
				php_error(E_WARNING, "imap_utf7_decode: "
					"Unexpected end of string");
				RETURN_FALSE;
			} else if (inp[1] != '-') {
				state = ST_DECODE0;
			} else {
				outlen++;
				inp++;
			}
		} else if (*inp == '-') {
			/* return to NORMAL mode */
			if (state == ST_DECODE1) {
				php_error(E_WARNING, "imap_utf7_decode: "
					"Stray modified base64 character: "
					"`%c'", *--inp);
				RETURN_FALSE;
			}
			state = ST_NORMAL;
		} else if (!B64CHAR(*inp)) {
			php_error(E_WARNING, "imap_utf7_decode: "
				"Invalid modified base64 character: "
				"`%c'", *inp);
			RETURN_FALSE;
		} else {
			switch (state) {
				case ST_DECODE3:
					outlen++;
					state = ST_DECODE0;
					break;
				case ST_DECODE2:
				case ST_DECODE1:
					outlen++;
				case ST_DECODE0:
					state++;
				case ST_NORMAL:
					break;
			}
		}
	}

	/* enforce end state */
	if (state != ST_NORMAL) {
		php_error(E_WARNING, "imap_utf7_decode: "
			"Unexpected end of string");
		RETURN_FALSE;
	}

	/* allocate output buffer */
	if ((out = emalloc(outlen + 1)) == NULL) {
		php_error(E_WARNING, "imap_utf7_decode: "
			"Unable to allocate result string");
		RETURN_FALSE;
	}

	/* decode input string */
	outp = out;
	state = ST_NORMAL;
	for (endp = (inp = in) + inlen; inp < endp; inp++) {
		if (state == ST_NORMAL) {
			if (*inp == '&' && inp[1] != '-') {
				state = ST_DECODE0;
			}
			else if ((*outp++ = *inp) == '&') {
				inp++;
			}
		}
		else if (*inp == '-') {
			state = ST_NORMAL;
		}
		else {
			/* decode input character */
			switch (state) {
			case ST_DECODE0:
				*outp = UNB64(*inp) << 2;
				state = ST_DECODE1;
				break;
			case ST_DECODE1:
				outp[1] = UNB64(*inp);
				*outp++ |= outp[1] >> 4;
				*outp <<= 4;
				state = ST_DECODE2;
				break;
			case ST_DECODE2:
				outp[1] = UNB64(*inp);
				*outp++ |= outp[1] >> 2;
				*outp <<= 6;
				state = ST_DECODE3;
				break;
			case ST_DECODE3:
				*outp++ |= UNB64(*inp);
				state = ST_DECODE0;
			case ST_NORMAL:
				break;
			}
		}
	}

	*outp = 0;

#if PHP_DEBUG
	/* warn if we computed outlen incorrectly */
	if (outp - out != outlen) {
		php_error(E_WARNING,
			"imap_utf7_decode: outp - out [%d] != outlen [%d]",
			outp - out, outlen);
	}
#endif

	RETURN_STRINGL(out, outlen, 0);
}
/* }}} */


/* {{{ proto string imap_utf7_encode(string buf)
   Encode a string in modified UTF-7 */
PHP_FUNCTION(imap_utf7_encode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	int argc;
	zval **arg;
	const unsigned char *in, *inp, *endp;
	unsigned char *out, *outp;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_ENCODE0,	/* encoded text rotation... */
		ST_ENCODE1,
		ST_ENCODE2
	} state;

	/* collect arguments */
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc, &arg) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(arg);
	in = (const unsigned char *) Z_STRVAL_PP(arg);
	inlen = Z_STRLEN_PP(arg);

	/* compute the length of the result string */
	outlen = 0;
	state = ST_NORMAL;
	endp = (inp = in) + inlen;
	while (inp < endp) {
		if (state == ST_NORMAL) {
			if (SPECIAL(*inp)) {
				state = ST_ENCODE0;
				outlen++;
			} else if (*inp++ == '&') {
				outlen++;
			}
			outlen++;
		} else if (!SPECIAL(*inp)) {
			state = ST_NORMAL;
		} else {
			/* ST_ENCODE0 -> ST_ENCODE1	- two chars
			 * ST_ENCODE1 -> ST_ENCODE2	- one char
			 * ST_ENCODE2 -> ST_ENCODE0	- one char
			 */
			if (state == ST_ENCODE2) {
				state = ST_ENCODE0;
			}
			else if (state++ == ST_ENCODE0) {
				outlen++;
			}
			outlen++;
			inp++;
		}
	}

	/* allocate output buffer */
	if ((out = emalloc(outlen + 1)) == NULL) {
		php_error(E_WARNING, "imap_utf7_encode: "
			"Unable to allocate result string");
		RETURN_FALSE;
	}

	/* encode input string */
	outp = out;
	state = ST_NORMAL;
	endp = (inp = in) + inlen;
	while (inp < endp || state != ST_NORMAL) {
		if (state == ST_NORMAL) {
			if (SPECIAL(*inp)) {
				/* begin encoding */
				*outp++ = '&';
				state = ST_ENCODE0;
			} else if ((*outp++ = *inp++) == '&') {
				*outp++ = '-';
			}
		} else if (inp == endp || !SPECIAL(*inp)) {
			/* flush overflow and terminate region */
			if (state != ST_ENCODE0) {
				*outp++ = B64(*outp);
			}
			*outp++ = '-';
			state = ST_NORMAL;
		} else {
			/* encode input character */
			switch (state) {
				case ST_ENCODE0:
					*outp++ = B64(*inp >> 2);
					*outp = *inp++ << 4;
					state = ST_ENCODE1;
					break;
				case ST_ENCODE1:
					*outp++ = B64(*outp | *inp >> 4);
					*outp = *inp++ << 2;
					state = ST_ENCODE2;
					break;
				case ST_ENCODE2:
					*outp++ = B64(*outp | *inp >> 6);
					*outp++ = B64(*inp++);
					state = ST_ENCODE0;
				case ST_NORMAL:
					break;
			}
		}
	}

	*outp = 0;

#if PHP_DEBUG
	/* warn if we computed outlen incorrectly */
	if (outp - out != outlen) {
		php_error(E_WARNING,
			"imap_utf7_encode: outp - out [%d] != outlen [%d]",
			outp - out, outlen);
	}
#endif

	RETURN_STRINGL(out, outlen, 0);
}
/* }}} */

#undef SPECIAL
#undef B64CHAR
#undef B64
#undef UNB64

/* {{{ proto int imap_setflag_full(int stream_id, string sequence, string flag [, int options])
   Sets flags on messages */
PHP_FUNCTION(imap_setflag_full)
{
	zval **streamind, **sequence, **flag, **flags;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flag, &flags) ==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_long_ex(streamind);
	convert_to_string_ex(sequence);
	convert_to_string_ex(flag);
	if (myargc==4) {
		convert_to_long_ex(flags);
	}

	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mail_setflag_full(imap_le_struct->imap_stream, Z_STRVAL_PP(sequence), Z_STRVAL_PP(flag), myargc==4 ? Z_LVAL_PP(flags) : NIL);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int imap_clearflag_full(int stream_id, string sequence, string flag [, int options])
   Clears flags on messages */
PHP_FUNCTION(imap_clearflag_full)
{
	zval **streamind, **sequence, **flag, **flags;
	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flag, &flags) ==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_long_ex(streamind);
	convert_to_string_ex(sequence);
	convert_to_string_ex(flag);
	if (myargc==4) {
		convert_to_long_ex(flags);
	}
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mail_clearflag_full(imap_le_struct->imap_stream, Z_STRVAL_PP(sequence), Z_STRVAL_PP(flag), myargc==4 ? Z_LVAL_PP(flags) : NIL);
    RETURN_TRUE;
}
/* }}} */


/* {{{ proto array imap_sort(int stream_id, int criteria, int reverse [, int options])
   Sort an array of message headers */
PHP_FUNCTION(imap_sort)
{
	zval **streamind, **pgm, **rev, **flags;
	int ind, ind_type;
	unsigned long *slst, *sl;
	SORTPGM *mypgm=NIL;
	SEARCHPGM *spg=NIL;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &pgm, &rev, &flags) ==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_long_ex(streamind);
	convert_to_long_ex(rev);
	convert_to_long_ex(pgm);
	if (Z_LVAL_PP(pgm)>SORTSIZE) {
		php_error(E_WARNING, "Unrecognized sort criteria");
		RETURN_FALSE;
	}
	if (myargc==4) {
		convert_to_long_ex(flags);
	}
	
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	spg = mail_newsearchpgm();
	mypgm = mail_newsortpgm();
	mypgm->reverse = Z_LVAL_PP(rev);
	mypgm->function = (short) Z_LVAL_PP(pgm);
	mypgm->next = NIL;
	
	array_init(return_value);
	slst = mail_sort(imap_le_struct->imap_stream, NIL, spg, mypgm, myargc==4 ? Z_LVAL_PP(flags) : NIL);
	
	for (sl = slst; *sl; sl++) { 
		add_next_index_long(return_value, *sl);
	}
	fs_give ((void **) &slst); 
}
/* }}} */


/* {{{ proto string imap_fetchheader(int stream_id, int msg_no [, int options])
   Get the full unfiltered header for a message */
PHP_FUNCTION(imap_fetchheader)
{
	zval **streamind, **msgno, **flags;
	int ind, ind_type, msgindex;
	pils *imap_le_struct;
	int myargc = ZEND_NUM_ARGS();

	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(msgno);
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}
	ind = Z_LVAL_PP(streamind);
	
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if ((myargc == 3) && (Z_LVAL_PP(flags) & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
           IMAP server, then that's the price we pay for making sure
           we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, Z_LVAL_PP(msgno));
	} else {
		msgindex = Z_LVAL_PP(msgno);
	}
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error(E_WARNING, "Bad message number");
		RETURN_FALSE;
	}
	
	RETVAL_STRING(mail_fetchheader_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL, NIL, myargc==3 ? Z_LVAL_PP(flags) : NIL), 1);
}
/* }}} */


/* {{{ proto int imap_uid(int stream_id, int msg_no)
   Get the unique message id associated with a standard sequential message number */
PHP_FUNCTION(imap_uid)
{
 	zval **streamind, **msgno;
 	int ind, ind_type;
	pils *imap_le_struct;
 
 	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &msgno) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}
 	
 	convert_to_long_ex(streamind);
 	convert_to_long_ex(msgno);
 
 	ind = Z_LVAL_PP(streamind);
 
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
 
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
	RETURN_LONG(mail_uid(imap_le_struct->imap_stream, Z_LVAL_PP(msgno)));
}
/* }}} */
 

/* {{{ proto int imap_msgno(int stream_id, int unique_msg_id)
   Get the sequence number associated with a UID */
PHP_FUNCTION(imap_msgno)
{
 	zval **streamind, **msgno;
 	int ind, ind_type;
	pils *imap_le_struct;
 
 	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &msgno) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}
 	
 	convert_to_long_ex(streamind);
 	convert_to_long_ex(msgno);
 
 	ind = Z_LVAL_PP(streamind);
 
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
 
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
 	RETURN_LONG(mail_msgno(imap_le_struct->imap_stream, Z_LVAL_PP(msgno)));
}
/* }}} */


/* {{{ proto object imap_status(int stream_id, string mailbox, int options)
   Get status info from a mailbox */
PHP_FUNCTION(imap_status)
{
 	zval **streamind, **mbx, **flags;
 	int ind, ind_type;
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();
	IMAPLS_FETCH();

 	if (myargc != 3 || zend_get_parameters_ex(myargc, &streamind, &mbx, &flags) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}
 	
 	convert_to_long_ex(streamind);
 	convert_to_string_ex(mbx);
	convert_to_long_ex(flags);
 	ind = Z_LVAL_PP(streamind);
 
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
 
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
 
	if (object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
 	if (mail_status(imap_le_struct->imap_stream, Z_STRVAL_PP(mbx), Z_LVAL_PP(flags))) {
	    add_property_long(return_value, "flags", IMAPG(status_flags));
		if (IMAPG(status_flags) & SA_MESSAGES) {
			add_property_long(return_value, "messages", IMAPG(status_messages));
		}
		if (IMAPG(status_flags) & SA_RECENT) {
			add_property_long(return_value, "recent", IMAPG(status_recent));
		}
		if (IMAPG(status_flags) & SA_UNSEEN) {
			add_property_long(return_value, "unseen", IMAPG(status_unseen));
		}
		if (IMAPG(status_flags) & SA_UIDNEXT) {
			add_property_long(return_value, "uidnext", IMAPG(status_uidnext));
		}
		if (IMAPG(status_flags) & SA_UIDVALIDITY) {
			add_property_long(return_value, "uidvalidity", IMAPG(status_uidvalidity));
		}
#ifdef SA_QUOTA
		if (IMAPG(status_flags) & SA_QUOTA) {
			add_property_long(return_value, "quota", IMAPG(status_quota));
		}
#endif
#ifdef SA_QUOTA
		if (IMAPG(status_flags) & SA_QUOTA_ALL) {
			add_property_long(return_value, "quota_all", IMAPG(status_quota_all));
		}
#endif
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
 

/* {{{ proto object imap_bodystruct(int stream_id, int msg_no, int section)
   Read the structure of a specified body section of a specific message */
PHP_FUNCTION(imap_bodystruct)
{
	zval **streamind, **msg, **section;
	int ind, ind_type;
	pils *imap_le_struct;
	zval *parametres, *param, *dparametres, *dparam;
	PARAMETER *par, *dpar;
	BODY *body;
	int myargc=ZEND_NUM_ARGS();

	if (myargc != 3 || zend_get_parameters_ex(myargc, &streamind, &msg, &section) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}
 	
 	convert_to_long_ex(streamind);
 	convert_to_long_ex(msg);
	convert_to_string_ex(section);
 	ind = Z_LVAL_PP(streamind);
 
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if (object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	body=mail_body(imap_le_struct->imap_stream, Z_LVAL_PP(msg), Z_STRVAL_PP(section));
	if (body->type) {
		add_property_long(return_value, "type", body->type);
	}
	if (body->encoding) {
		add_property_long(return_value, "encoding", body->encoding);
	}
	
	if (body->subtype) {
		add_property_long(return_value, "ifsubtype", 1);
		add_property_string(return_value, "subtype",  body->subtype, 1);
	} else {
		add_property_long(return_value, "ifsubtype", 0);
	}
	
	if (body->description) {
		add_property_long(return_value, "ifdescription", 1);
		add_property_string(return_value, "description",  body->description, 1);
	} else {
		add_property_long(return_value, "ifdescription", 0);
	}
	if (body->id) {
		add_property_long(return_value, "ifid", 1);
		add_property_string(return_value, "id",  body->id, 1);
	} else {
		add_property_long(return_value, "ifid", 0);
	}

	
	if (body->size.lines) {
		add_property_long(return_value, "lines", body->size.lines);
	}
	if (body->size.bytes) {
		add_property_long(return_value, "bytes", body->size.bytes);
	}
#ifdef IMAP41
	if (body->disposition.type) {
		add_property_long(return_value, "ifdisposition", 1);
		add_property_string(return_value, "disposition", body->disposition.type, 1);
	} else {
		add_property_long(return_value, "ifdisposition", 0);
	}
	
	if (body->disposition.parameter) {
		dpar = body->disposition.parameter;
		add_property_long(return_value, "ifdparameters", 1);
		MAKE_STD_ZVAL(dparametres);
		array_init(dparametres);
		do {
			MAKE_STD_ZVAL(dparam);
			object_init(dparam);
			add_property_string(dparam, "attribute", dpar->attribute, 1);
			add_property_string(dparam, "value", dpar->value, 1);
			add_next_index_object(dparametres, dparam);
		} while ((dpar = dpar->next));
		add_assoc_object(return_value, "dparameters", dparametres);
	} else {
		add_property_long(return_value, "ifdparameters", 0);
	}
#endif
	
	if ((par = body->parameter)) {
		add_property_long(return_value, "ifparameters", 1);
		
		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		do {
			MAKE_STD_ZVAL(param);
			object_init(param);
			if (par->attribute) {
				add_property_string(param, "attribute", par->attribute, 1);
			}
			if (par->value) {
				add_property_string(param, "value", par->value, 1);
			}
			
			add_next_index_object(parametres, param);
		} while ((par = par->next));
	} else {
		MAKE_STD_ZVAL(parametres);
		object_init(parametres);
		add_property_long(return_value, "ifparameters", 0);
	}
	add_assoc_object(return_value, "parameters", parametres);
}

/* }}} */


/* {{{ proto array imap_fetch_overview(int stream_id, int msg_no)
   Read an overview of the information in the headers of the given message sequence */ 
PHP_FUNCTION(imap_fetch_overview)
{
 	zval **streamind, **sequence, **pflags;
 	int ind, ind_type;
	pils *imap_le_struct;
	zval *myoverview;
	char address[MAILTMPLEN];
	long status, flags=0L;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &streamind, &sequence) == FAILURE) {
				ZEND_WRONG_PARAM_COUNT();
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &streamind, &sequence, &pflags) == FAILURE) {
				ZEND_WRONG_PARAM_COUNT();
			} else {
				convert_to_long_ex(pflags);
				flags = Z_LVAL_PP(pflags);
			}
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}
 	
 	convert_to_long_ex(streamind);
 	convert_to_string_ex(sequence);
	
 	ind = Z_LVAL_PP(streamind);
 	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	array_init(return_value);
	
	status = (flags & FT_UID)
		? mail_uid_sequence (imap_le_struct->imap_stream, Z_STRVAL_PP(sequence))
		: mail_sequence (imap_le_struct->imap_stream, Z_STRVAL_PP(sequence))
		;
	
	
	if (status) {  
		MESSAGECACHE *elt;
		ENVELOPE *env;
		unsigned long i;
		
		for (i = 1; i <= imap_le_struct->imap_stream->nmsgs; i++) {
			if (((elt = mail_elt (imap_le_struct->imap_stream, i))->sequence) &&
				(env = mail_fetch_structure (imap_le_struct->imap_stream, i, NIL, NIL))) {
				MAKE_STD_ZVAL(myoverview);
				object_init(myoverview);
				if (env->subject) {
					add_property_string(myoverview, "subject", env->subject, 1);
				}
				if (env->from) {
					env->from->next=NULL;
					address[0] = '\0';
					rfc822_write_address(address, env->from);
					add_property_string(myoverview, "from", address, 1);
				}
				if (env->date) {
					add_property_string(myoverview, "date", env->date, 1);
				}
				if (env->message_id) {
					add_property_string(myoverview, "message_id", env->message_id, 1);
				}
				if (env->references) {
					add_property_string(myoverview, "references", env->references, 1);
				}
				add_property_long(myoverview, "size", elt->rfc822_size);
				add_property_long(myoverview, "uid", mail_uid(imap_le_struct->imap_stream, i));
				add_property_long(myoverview, "msgno", i);
				add_property_long(myoverview, "recent", elt->recent);
				add_property_long(myoverview, "flagged", elt->flagged);
				add_property_long(myoverview, "answered", elt->answered);
				add_property_long(myoverview, "deleted", elt->deleted);
				add_property_long(myoverview, "seen", elt->seen);
				add_property_long(myoverview, "draft", elt->draft);
				add_next_index_object(return_value, myoverview);
			}
		}
	}
}
/* }}} */


/* {{{ proto string imap_mail_compose(array envelope, array body)
   Create a MIME message based on given envelope and body sections */
PHP_FUNCTION(imap_mail_compose)
{
	zval **envelope, **body;
	char *key;
	zval **data, **pvalue;
	ulong ind;
	char *cookie = NIL;
	ENVELOPE *env;
	BODY *bod=NULL, *topbod=NULL;
	PART *mypart=NULL, *toppart=NULL, *part;
	PARAMETER *param;
	char tmp[8*MAILTMPLEN], *mystring=NULL, *t, *tempstring;
	
	int myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(myargc, &envelope, &body) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_writable_array_ex(envelope);
	convert_to_writable_array_ex(body);
	env=mail_newenvelope();
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "remail", sizeof("remail"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		env->remail=cpystr(Z_STRVAL_PP(pvalue));
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "return_path", sizeof("return_path"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue)
		rfc822_parse_adrlist(&env->return_path, Z_STRVAL_PP(pvalue), "NO HOST");
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "date", sizeof("date"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		env->date=cpystr(Z_STRVAL_PP(pvalue));
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "from", sizeof("from"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		rfc822_parse_adrlist (&env->from, Z_STRVAL_PP(pvalue), "NO HOST");
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "reply_to", sizeof("reply_to"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		rfc822_parse_adrlist (&env->reply_to, Z_STRVAL_PP(pvalue), "NO HOST");
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "to", sizeof("to"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		rfc822_parse_adrlist (&env->to, Z_STRVAL_PP(pvalue), "NO HOST");
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "cc", sizeof("cc"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		rfc822_parse_adrlist (&env->cc, Z_STRVAL_PP(pvalue), "NO HOST");
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "bcc", sizeof("bcc"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		rfc822_parse_adrlist (&env->bcc, Z_STRVAL_PP(pvalue), "NO HOST");
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "message_id", sizeof("message_id"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		env->message_id=cpystr(Z_STRVAL_PP(pvalue));
	}
 
	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(body));
	for (;;) {
		if (zend_hash_get_current_data(Z_ARRVAL_PP(body), (void **) &data)==FAILURE) {
			break;
		}
		zend_hash_get_current_key(Z_ARRVAL_PP(body), &key, &ind);
		if (Z_TYPE_PP(data) == IS_ARRAY) {
			bod=mail_newbody();
			topbod=bod;
			if (zend_hash_find(Z_ARRVAL_PP(data), "type", sizeof("type"), (void **) &pvalue)== SUCCESS) {
				convert_to_long_ex(pvalue);
				bod->type = (short) Z_LVAL_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "encoding", sizeof("encoding"), (void **) &pvalue)== SUCCESS) {
				convert_to_long_ex(pvalue);
				bod->encoding = (short) Z_LVAL_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "subtype", sizeof("subtype"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->subtype = cpystr(Z_STRVAL_PP(pvalue));
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "id", sizeof("id"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->id = cpystr(Z_STRVAL_PP(pvalue));
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "contents.data", sizeof("contents.data"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->contents.text.data = (char *) fs_get(Z_STRLEN_PP(pvalue));
				memcpy(bod->contents.text.data, Z_STRVAL_PP(pvalue), Z_STRLEN_PP(pvalue)+1);
				bod->contents.text.size = Z_STRLEN_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "lines", sizeof("lines"), (void **) &pvalue)== SUCCESS) {
				convert_to_long_ex(pvalue);
				bod->size.lines = Z_LVAL_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "bytes", sizeof("bytes"), (void **) &pvalue)== SUCCESS) {
				convert_to_long_ex(pvalue);
				bod->size.bytes = Z_LVAL_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "md5", sizeof("md5"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->md5 = cpystr(Z_STRVAL_PP(pvalue));
			}
		}
		zend_hash_move_forward(Z_ARRVAL_PP(body));
		if (!toppart) {
			bod->nested.part=mail_newbody_part();
			mypart=bod->nested.part;
			toppart=mypart;
			bod=&mypart->body;
		} else {
			 mypart->next=mail_newbody_part();
			 mypart=mypart->next;
			 bod=&mypart->body;
		}
	}

	rfc822_encode_body_7bit(env, topbod); 
	rfc822_header (tmp, env, topbod);  
	mystring=emalloc(strlen(tmp)+1);
	strcpy(mystring, tmp);

	bod=topbod;
	switch (bod->type) {
		case TYPEMULTIPART:           /* multipart gets special handling */
			part = bod->nested.part;   /* first body part */
										/* find cookie */
			for (param = bod->parameter; param && !cookie; param = param->next) {
				if (!strcmp (param->attribute, "BOUNDARY")) {
					cookie = param->value;
				}
			}
			if (!cookie) {
				cookie = "-";  /* yucky default */
			}
			do {                        /* for each part */
                    /* build cookie */
				sprintf (t=tmp, "--%s\015\012", cookie);
										/* append mini-header */
				rfc822_write_body_header(&t, &part->body);
				strcat (t, "\015\012");    /* write terminating blank line */
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
				strcat(mystring, bod->contents.text.data);

			} while ((part = part->next));/* until done */
            /* output trailing cookie */

			sprintf(tmp, "--%s--", cookie);
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
			strcat(mystring, bod->contents.text.data);
			break;
	}

	RETURN_STRINGL(mystring,strlen(mystring), 1);  
}
/* }}} */


#ifndef PHP_WIN32
int _php_imap_mail(char *to, char *subject, char *message, char *headers, char *cc, char *bcc, char* rpath)
{
#ifdef PHP_WIN32
	int tsm_err;
#else
	FILE *sendmail;
	int ret;
#endif

#ifdef PHP_WIN32
	if (imap_TSendMail(INI_STR("smtp"), &tsm_err, headers, subject, to, message, cc, bcc, rpath) != SUCCESS) {
		php_error(E_WARNING, GetSMErrorText(tsm_err));
		return 0;
	}
#else
	if (!INI_STR("sendmail_path")) {
		return 0;
	}
	sendmail = popen(INI_STR("sendmail_path"), "w");
	if (sendmail) {
		if (rpath && rpath[0]) fprintf(sendmail, "From: %s\n", rpath);
		fprintf(sendmail, "To: %s\n", to);
		if (cc && cc[0]) fprintf(sendmail, "Cc: %s\n", cc);
		if (bcc && bcc[0]) fprintf(sendmail, "Bcc: %s\n", bcc);
		fprintf(sendmail, "Subject: %s\n", subject);
		if (headers != NULL) {
			fprintf(sendmail, "%s\n", headers);
		}
		fprintf(sendmail, "\n%s\n", message);
		ret = pclose(sendmail);
		if (ret == -1) {
			return 0;
		} else {
			return 1;
		}
	} else {
		php_error(E_WARNING, "Could not execute mail delivery program");
		return 0;
	}
#endif
	return 1;
}

/* {{{ proto int imap_mail(string to, string subject, string message [, string additional_headers [, string cc [, string bcc [, string rpath]]]])
   Send an email message */
PHP_FUNCTION(imap_mail)
{
	zval *argv[6];
	char *to=NULL, *message=NULL, *headers=NULL, *subject=NULL, *cc=NULL, *bcc=NULL, *rpath=NULL;
	int argc;
	
	argc = ZEND_NUM_ARGS();
	if (argc < 3 || argc > 7 || zend_get_parameters_array(ht, argc, argv) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	/* To: */
	convert_to_string(argv[0]);
	if (argv[0]->value.str.val) {
		to = argv[0]->value.str.val;
	} else {
		php_error(E_WARNING, "No to field in mail command");
		RETURN_FALSE;
	}

	/* Subject: */
	convert_to_string(argv[1]);
	if (argv[1]->value.str.val) {
		subject = argv[1]->value.str.val;
	} else {
		php_error(E_WARNING, "No subject field in mail command");
		RETURN_FALSE;
	}

	/* message body */
	convert_to_string(argv[2]);
	if (argv[2]->value.str.val) {
		message = argv[2]->value.str.val;
	} else {
		/* this is not really an error, so it is allowed. */
		php_error(E_WARNING, "No message string in mail command");
		message = NULL;
	}

	/* other headers */
	if (argc > 3) {
		convert_to_string(argv[3]);
		headers = argv[3]->value.str.val;
	}
	
	/* cc */
	if (argc > 4) {
		convert_to_string(argv[4]);
		cc = argv[4]->value.str.val;
	}

	/* bcc */
	if (argc > 5) {
		convert_to_string(argv[5]);
		bcc = argv[5]->value.str.val;
	}

	/* rpath */
	if (argc > 6) {
		convert_to_string(argv[6]);
		rpath = argv[6]->value.str.val;
	}

	if (_php_imap_mail(to, subject, message, headers, cc, bcc, rpath)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ proto array imap_search(int stream_id, string criteria [, long flags])
   Return a list of messages matching the given criteria */
PHP_FUNCTION(imap_search)
{
	zval **streamind, **criteria, **search_flags;
	int ind, ind_type, args;
	pils *imap_le_struct;
	long flags;
	MESSAGELIST *cur;
	IMAPLS_FETCH();
    
	args = ZEND_NUM_ARGS();
	if (args < 2 || args > 3 || zend_get_parameters_ex(args, &streamind, &criteria, &search_flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(criteria);
	
	if (args == 2) {
		flags = SE_FREE;
	} else {
		convert_to_long_ex(search_flags);
		flags = Z_LVAL_PP(search_flags);
	}
	
	ind = Z_LVAL_PP(streamind);
	imap_le_struct = (pils *) zend_list_find(ind, &ind_type);
	if (!imap_le_struct || !IS_STREAM(ind_type)) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	IMAPG(imap_messages) = NIL;
	mail_search_full(imap_le_struct->imap_stream, NIL, mail_criteria(Z_STRVAL_PP(criteria)), flags);
	if (IMAPG(imap_messages) == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	cur = IMAPG(imap_messages);
	while (cur != NIL) {
		add_next_index_long(return_value, cur->msgid);
		cur = cur->next;
	}
	mail_free_messagelist(&IMAPG(imap_messages));
}
/* }}} */


/* {{{ proto array imap_alerts(void)
   Returns an array of all IMAP alerts that have been generated since the last page load or since the last imap_alerts() call, whichever came last. The alert stack is cleared after imap_alerts() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_alerts)
{
	STRINGLIST *cur=NIL;
	int arg_count = ZEND_NUM_ARGS();
	IMAPLS_FETCH();
    
	if (arg_count > 0) {
		ZEND_WRONG_PARAM_COUNT();
	} 
  
	if (IMAPG(imap_alertstack) == NIL) {
		RETURN_FALSE;
	}
  
	array_init(return_value);
	cur = IMAPG(imap_alertstack);
	while (cur != NIL) {
		add_next_index_string(return_value, cur->LTEXT, 1);
		cur = cur->next;
	}
	mail_free_stringlist(&IMAPG(imap_alertstack));
	IMAPG(imap_alertstack) = NIL;
}
/* }}} */


/* {{{ proto array imap_errors(void)
   Returns an array of all IMAP errors generated since the last  page load, or since the last imap_errors() call, whichever came last. The error stack is cleared after imap_errors() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_errors)
{
	ERRORLIST *cur=NIL;
	int arg_count = ZEND_NUM_ARGS();
	IMAPLS_FETCH();
    
	if (arg_count > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}
  
	if (IMAPG(imap_errorstack) == NIL) {
		RETURN_FALSE;
	}
  
	array_init(return_value);
	cur = IMAPG(imap_errorstack);
	while (cur != NIL) {
		add_next_index_string(return_value, cur->LTEXT, 1);
		cur = cur->next;
	}
	mail_free_errorlist(&IMAPG(imap_errorstack));
	IMAPG(imap_errorstack) = NIL;
}
/* }}} */


/* {{{ proto string imap_last_error(void) 
   Returns the last error that was generated by an IMAP function. The error stack is NOT cleared after this call. */
/* Author: CJH */
PHP_FUNCTION(imap_last_error)
{
	ERRORLIST *cur=NIL;
	int arg_count = ZEND_NUM_ARGS();
	IMAPLS_FETCH();
    
	if (arg_count > 0) {
		ZEND_WRONG_PARAM_COUNT();
	}
  
	if (IMAPG(imap_errorstack) == NIL) {
		RETURN_FALSE;
	}
  
	cur = IMAPG(imap_errorstack);
	while (cur != NIL) {
		if (cur->next == NIL) {
			RETURN_STRING(cur->LTEXT, 1);
		}
		cur = cur->next;
	}
}
/* }}} */


/* {{{ proto array imap_mime_header_decode(string str)
   Decode mime header element in accordance with RFC 2047 and return array of objects containing 'charset' encoding and decoded 'text' */
PHP_FUNCTION(imap_mime_header_decode)
{
	/* Author: Ted Parnefors <ted@mtv.se> */
	zval **str, *myobject;
	char *string, *charset, encoding, *text, *decode;
	long charset_token, encoding_token, end_token, end, offset=0, i;
	unsigned long newlength;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(str);

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	string=(*str)->value.str.val;
	end=(*str)->value.str.len;
			
	if ((charset=((char *)emalloc((end+1)*2)))) {
		text=&charset[end+1];
		while(offset<end) {	/* Reached end of the string? */
			if ((charset_token=(long) php_memnstr(&string[offset], "=?", 2, string+end))) {	/* Is there anything encoded in the string? */
				charset_token -= (long)string;
				if (offset!=charset_token) {	/* Is there anything before the encoded data? */
					/* Retrieve unencoded data that is found before encoded data */
					memcpy(text, &string[offset], charset_token-offset);
					text[charset_token-offset]=0x00;
					MAKE_STD_ZVAL(myobject);
					object_init(myobject);
					add_property_string(myobject, "charset", "default", 1);
					add_property_string(myobject, "text", text, 1);
					zend_hash_next_index_insert(return_value->value.ht,(void *)&myobject,sizeof(zval *),NULL);
				}
				if ((encoding_token=(long) php_memnstr(&string[charset_token+2], "?", 1, string+end))) {		/* Find token for encoding */
					encoding_token -= (long) string;
					if ((end_token=(long) php_memnstr(&string[encoding_token+3], "?=", 2, string+end))) {	/* Find token for end of encoded data */
						end_token -= (long) string;
						memcpy(charset, &string[charset_token+2], encoding_token-(charset_token+2));	/* Extract charset encoding */
						charset[encoding_token-(charset_token+2)]=0x00;
						encoding=string[encoding_token+1];	/* Extract encoding from string */
						memcpy(text, &string[encoding_token+3], end_token-(encoding_token+3));	/* Extract text */
						text[end_token-(encoding_token+3)]=0x00;
						decode=text;
						if (encoding=='q' || encoding=='Q') {	/* Decode 'q' encoded data */
							for(i=0;text[i]!=0x00;i++) if (text[i]=='_') text[i]=' ';	/* Replace all *_' with space. */
							decode = (char *) rfc822_qprint((unsigned char *) text, strlen(text), &newlength);
						} else if (encoding=='b' || encoding=='B') {
							decode = (char *) rfc822_base64((unsigned char *) text, strlen(text), &newlength); /* Decode 'B' encoded data */
						}
						MAKE_STD_ZVAL(myobject);
						object_init(myobject);
						add_property_string(myobject, "charset", charset, 1);
						add_property_string(myobject, "text", decode, 1);
						zend_hash_next_index_insert(return_value->value.ht,(void *)&myobject,sizeof(zval *),NULL);
						fs_give((void**) &decode);
						
						offset=end_token+2;
						for(i=0;(string[offset+i]==' ') || (string[offset+i]==0x0a) || (string[offset+i]==0x0d);i++);
						if((string[offset+i]=='=') && (string[offset+i+1]=='?') && (offset+i<end)) offset+=i;
						continue;	/*/ Iterate the loop again please. */
					}
				}
			} else {
				/* Just some tweaking to optimize the code, and get the end statements work in a general manner.
				   If we end up here we didn't find a position for "charset_token",
				   so we need to set it to the start of the yet unextracted data. */
				charset_token=offset;
			}
			/* Return the rest of the data as unencoded, as it was either unencoded or was missing separators
			   which rendered the the remainder of the string impossible for us to decode. */
			memcpy(text, &string[charset_token], end-charset_token);	/* Extract unencoded text from string */
			text[end-charset_token]=0x00;
			MAKE_STD_ZVAL(myobject);
			object_init(myobject);
			add_property_string(myobject, "charset", "default", 1);
			add_property_string(myobject, "text", text, 1);
			zend_hash_next_index_insert(return_value->value.ht,(void *)&myobject,sizeof(zval *),NULL);
			
			offset=end;	/* We have reached the end of the string. */
		}
		efree(charset);
	} else {
		php_error(E_WARNING, "Unable to allocate temporary memory buffer for imap_mime_header_decode");
		RETURN_FALSE;
	}
}
/* }}} */


/* Support Functions */
void _php_imap_parse_address (ADDRESS *addresslist, char *fulladdress, zval *paddress)
{
	ADDRESS *addresstmp, *addresstmp2;
	char tempaddress[MAILTMPLEN];
	zval *tmpvals;
	int ok = 1;
	
	addresstmp = addresslist;
	fulladdress[0] = 0x00;
	while (ok && addresstmp) {                                    /* while length < 1000 and we are not at the end of the list */
		addresstmp2 = addresstmp->next;                           /* save the pointer to the next address */
		addresstmp->next = NULL;                                  /* make this address the only one now. */
		tempaddress[0] = '\0';                                    /* reset tempaddress buffer */
		rfc822_write_address(tempaddress, addresstmp);            /* ok, write the address into tempaddress string */
		if ((strlen(tempaddress) + strlen(fulladdress)) < 1000) { /* is the new address + total address < 1000 */
			if (strlen(fulladdress)) {
				strcat(fulladdress, ",");	                      /* put in a comma */ 
			}
			strcat(fulladdress, tempaddress);                     /* put in the new address */
		} else {                                                  /* no */
			ok = 0;                                               /* stop looping */
			strcat(fulladdress, ", ..."); 
		}		/* DO NOT optimize this out - changing it breaks things */
		addresstmp->next = addresstmp2; /* reset the pointer to the next address first! */
		addresstmp = addresstmp->next; 
	}
	
	addresstmp = addresslist;
	do {
		MAKE_STD_ZVAL(tmpvals);
		object_init(tmpvals);
		if (addresstmp->personal) add_property_string(tmpvals, "personal", addresstmp->personal, 1);
		if (addresstmp->adl) add_property_string(tmpvals, "adl", addresstmp->adl, 1);
		if (addresstmp->mailbox) add_property_string(tmpvals, "mailbox", addresstmp->mailbox, 1);
		if (addresstmp->host) add_property_string(tmpvals, "host", addresstmp->host, 1);
		add_next_index_object(paddress, tmpvals);
	} while ((addresstmp = addresstmp->next));
}

void _php_make_header_object(zval *myzvalue, ENVELOPE *en)
{
	zval *paddress;
	char fulladdress[MAILTMPLEN];
	
	object_init(myzvalue);
	
    if (en->remail) add_property_string(myzvalue, "remail", en->remail, 1);
    if (en->date) add_property_string(myzvalue, "date", en->date, 1);
    if (en->date) add_property_string(myzvalue, "Date", en->date, 1);
    if (en->subject) add_property_string(myzvalue, "subject", en->subject, 1);
    if (en->subject) add_property_string(myzvalue, "Subject", en->subject, 1);
    if (en->in_reply_to) add_property_string(myzvalue, "in_reply_to", en->in_reply_to, 1);
    if (en->message_id) add_property_string(myzvalue, "message_id", en->message_id, 1);
    if (en->newsgroups) add_property_string(myzvalue, "newsgroups", en->newsgroups, 1);
    if (en->followup_to) add_property_string(myzvalue, "followup_to", en->followup_to, 1);
    if (en->references) add_property_string(myzvalue, "references", en->references, 1);
	
	if (en->to) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->to, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "toaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "to", paddress);
	}
	
	if (en->from) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->from, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "fromaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "from", paddress);
	}
	
	if (en->cc) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->cc, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "ccaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "cc", paddress);
	}
	
	if (en->bcc) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->bcc, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "bccaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "bcc", paddress);
	}
	
	if (en->reply_to) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->reply_to, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "reply_toaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "reply_to", paddress);
	}

	if (en->sender) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->sender, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "senderaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "sender", paddress);
	}

	if (en->return_path) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->return_path, fulladdress, paddress);
		if (fulladdress) add_property_string(myzvalue, "return_pathaddress", fulladdress, 1);
		add_assoc_object(myzvalue, "return_path", paddress);
	}
	
}

void _php_imap_add_body(zval *arg, BODY *body)
{
	zval *parametres, *param, *dparametres, *dparam;
	PARAMETER *par, *dpar;
	PART *part;
	
	if (body->type) {
		add_property_long(arg, "type", body->type);
	}
	if (body->encoding) {
		add_property_long(arg, "encoding", body->encoding);
	}

	if (body->subtype) {
		add_property_long(arg, "ifsubtype", 1);
		add_property_string(arg, "subtype",  body->subtype, 1);
	} else {
		add_property_long(arg, "ifsubtype", 0);
	}

	if (body->description) {
		add_property_long(arg, "ifdescription", 1);
		add_property_string(arg, "description",  body->description, 1);
	} else {
		add_property_long(arg, "ifdescription", 0);
	}
	if (body->id) {
		add_property_long(arg, "ifid", 1);
		add_property_string(arg, "id",  body->id, 1);
	} else {
		add_property_long(arg, "ifid", 0);
	}
	
	if (body->size.lines) {
		add_property_long(arg, "lines", body->size.lines);
	}
	if (body->size.bytes) {
		add_property_long(arg, "bytes", body->size.bytes);
	}
#ifdef IMAP41
	if (body->disposition.type) {
		add_property_long(arg, "ifdisposition", 1);
		add_property_string(arg, "disposition", body->disposition.type, 1);
	} else {
		add_property_long(arg, "ifdisposition", 0);
	}

	if (body->disposition.parameter) {
		dpar = body->disposition.parameter;
		add_property_long(arg, "ifdparameters", 1);
		MAKE_STD_ZVAL(dparametres);
		array_init(dparametres);
		do {
			MAKE_STD_ZVAL(dparam);
			object_init(dparam);
			add_property_string(dparam, "attribute", dpar->attribute, 1);
			add_property_string(dparam, "value", dpar->value, 1);
			add_next_index_object(dparametres, dparam);
        } while ((dpar = dpar->next));
		add_assoc_object(arg, "dparameters", dparametres);
	} else {
		add_property_long(arg, "ifdparameters", 0);
	}
#endif
 
	if ((par = body->parameter)) {
		add_property_long(arg, "ifparameters", 1);

		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		do {
			MAKE_STD_ZVAL(param);
			object_init(param);
			if (par->attribute) {
				add_property_string(param, "attribute", par->attribute, 1);
			}
			if (par->value) {
				add_property_string(param, "value", par->value, 1);
			}

			add_next_index_object(parametres, param);
		} while ((par = par->next));
	} else {
		MAKE_STD_ZVAL(parametres);
		object_init(parametres);
		add_property_long(arg, "ifparameters", 0);
	}
	add_assoc_object(arg, "parameters", parametres);

	/* multipart message ? */
	if (body->type == TYPEMULTIPART) {
		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		for (part = body->CONTENT_PART; part; part = part->next) {
			MAKE_STD_ZVAL(param);
			object_init(param);
			_php_imap_add_body(param, &part->body);
			add_next_index_object(parametres, param);
		}
		add_assoc_object(arg, "parts", parametres);
	}
	
	/* encapsulated message ? */
	if ((body->type == TYPEMESSAGE) && (!strcasecmp(body->subtype, "rfc822"))) {
		body = body->CONTENT_MSG_BODY;
		MAKE_STD_ZVAL(parametres);
		array_init(parametres);
		MAKE_STD_ZVAL(param);
		object_init(param);
		_php_imap_add_body(param, body);
		add_next_index_object(parametres, param);
		add_assoc_object(arg, "parts", parametres);
	}
}


/* Interfaces to C-client */
void mm_searched(MAILSTREAM *stream,unsigned long number)
{
	MESSAGELIST *cur = NIL;
	IMAPLS_FETCH();
  
	if (IMAPG(imap_messages) == NIL) {
		IMAPG(imap_messages) = mail_newmessagelist();
		IMAPG(imap_messages)->msgid = number;
		IMAPG(imap_messages)->next = NIL;
	} else {
		cur = IMAPG(imap_messages);
		while (cur->next != NIL) {
			cur = cur->next;
		}
		cur->next = mail_newmessagelist();
		cur = cur->next;
		cur->msgid = number;
		cur->next = NIL;
	}
}

void mm_exists(MAILSTREAM *stream,unsigned long number)
{
}

void mm_expunged(MAILSTREAM *stream,unsigned long number)
{
}

void mm_flags(MAILSTREAM *stream,unsigned long number)
{
}

/* Author: CJH */
void mm_notify(MAILSTREAM *stream, char *str, long errflg)
{
	STRINGLIST *cur = NIL;
	IMAPLS_FETCH();
  
	if (strncmp(str, "[ALERT] ", 8) == 0) {
		if (IMAPG(imap_alertstack) == NIL) {
			IMAPG(imap_alertstack) = mail_newstringlist();
			IMAPG(imap_alertstack)->LSIZE = strlen(IMAPG(imap_alertstack)->LTEXT = cpystr(str));
			IMAPG(imap_alertstack)->next = NIL; 
		} else {
			cur = IMAPG(imap_alertstack);
			while (cur->next != NIL) {
				cur = cur->next;
			}
			cur->next = mail_newstringlist ();
			cur = cur->next;
			cur->LSIZE = strlen(cur->LTEXT = cpystr(str));
			cur->next = NIL;
		}
	}
}

void mm_list(MAILSTREAM *stream,DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;
	IMAPLS_FETCH();
	
	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build up a the new array of objects */
		/* Author: CJH */
		if (IMAPG(imap_folder_objects) == NIL) {
			IMAPG(imap_folder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_folder_objects)->LSIZE=strlen(IMAPG(imap_folder_objects)->LTEXT=cpystr(mailbox));
			IMAPG(imap_folder_objects)->delimiter = delimiter;
			IMAPG(imap_folder_objects)->attributes = attributes;
			IMAPG(imap_folder_objects)->next = NIL;
		} else {
			ocur=IMAPG(imap_folder_objects);
			while (ocur->next != NIL) {
				ocur=ocur->next;
			}
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE = strlen(ocur->LTEXT = cpystr(mailbox));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
		}
		
	} else {
		/* build the old IMAPG(imap_folders) variable to allow old imap_listmailbox() to work */
		if (!(attributes & LATT_NOSELECT)) {
			if (IMAPG(imap_folders) == NIL) {
				IMAPG(imap_folders)=mail_newstringlist();
				IMAPG(imap_folders)->LSIZE=strlen(IMAPG(imap_folders)->LTEXT=cpystr(mailbox));
				IMAPG(imap_folders)->next=NIL; 
			} else {
				cur=IMAPG(imap_folders);
				while (cur->next != NIL) {
					cur=cur->next;
				}
				cur->next=mail_newstringlist ();
				cur=cur->next;
				cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
				cur->next = NIL;
			}
		}
	}
}

void mm_lsub(MAILSTREAM *stream,DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;
	IMAPLS_FETCH();
	
	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build the array of objects */
		/* Author: CJH */
		if (IMAPG(imap_sfolder_objects) == NIL) {
			IMAPG(imap_sfolder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_sfolder_objects)->LSIZE=strlen(IMAPG(imap_sfolder_objects)->LTEXT=cpystr(mailbox));
			IMAPG(imap_sfolder_objects)->delimiter = delimiter;
			IMAPG(imap_sfolder_objects)->attributes = attributes;
			IMAPG(imap_sfolder_objects)->next = NIL;
		} else {
			ocur=IMAPG(imap_sfolder_objects);
			while (ocur->next != NIL) {
				ocur=ocur->next;
			}
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE=strlen(ocur->LTEXT = cpystr(mailbox));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
		}
	} else {
		/* build the old simple array for imap_listsubscribed() */
		if (IMAPG(imap_sfolders) == NIL) {
			IMAPG(imap_sfolders)=mail_newstringlist();
			IMAPG(imap_sfolders)->LSIZE=strlen(IMAPG(imap_sfolders)->LTEXT=cpystr(mailbox));
			IMAPG(imap_sfolders)->next=NIL; 
		} else {
			cur=IMAPG(imap_sfolders);
			while (cur->next != NIL) {
				cur=cur->next;
			}
			cur->next=mail_newstringlist ();
			cur=cur->next;
			cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
			cur->next = NIL;
		}
	}
}

void mm_status(MAILSTREAM *stream, char *mailbox,MAILSTATUS *status)
{
	IMAPLS_FETCH();

	IMAPG(status_flags)=status->flags;
	if (IMAPG(status_flags) & SA_MESSAGES) {
		IMAPG(status_messages)=status->messages;
	}
	if (IMAPG(status_flags) & SA_RECENT) {
		IMAPG(status_recent)=status->recent;
	}
	if (IMAPG(status_flags) & SA_UNSEEN) {
		IMAPG(status_unseen)=status->unseen;
	}
	if (IMAPG(status_flags) & SA_UIDNEXT) {
		IMAPG(status_uidnext)=status->uidnext;
	}
	if (IMAPG(status_flags) & SA_UIDVALIDITY) {
		IMAPG(status_uidvalidity)=status->uidvalidity;
	}

#ifdef SA_QUOTA
	if (IMAPG(status_flags) & SA_QUOTA) {
		IMAPG(status_quota)=status->quota;
	}
#endif
#ifdef SA_QUOTA_ALL
	if (IMAPG(status_flags) & SA_QUOTA_ALL) { 
		IMAPG(status_quota_all)=status->quota_all;
	}
#endif
}

void mm_log(char *str, long errflg)
{
	ERRORLIST *cur = NIL;
	IMAPLS_FETCH();
  
	/* Author: CJH */
	if (errflg != NIL) { /* CJH: maybe put these into a more comprehensive log for debugging purposes? */
		if (IMAPG(imap_errorstack) == NIL) {
			IMAPG(imap_errorstack) = mail_newerrorlist();
			IMAPG(imap_errorstack)->LSIZE = strlen(IMAPG(imap_errorstack)->LTEXT = cpystr(str));
			IMAPG(imap_errorstack)->errflg = errflg;
			IMAPG(imap_errorstack)->next = NIL; 
		} else {
			cur = IMAPG(imap_errorstack);
			while (cur->next != NIL) {
				cur = cur->next;
			}
			cur->next = mail_newerrorlist();
			cur = cur->next;
			cur->LSIZE = strlen(cur->LTEXT = cpystr(str));
			cur->errflg = errflg;
			cur->next = NIL;
		}
	}
}

void mm_dlog(char *str)
{
	/* CJH: this is for debugging; it might be useful to allow setting
	   the stream to debug mode and capturing this somewhere - syslog?
	   php debugger? */
}

void mm_login(NETMBX *mb, char *user, char *pwd, long trial)
{
	IMAPLS_FETCH();

#if HAVE_IMSP
	if (*mb->service && strcmp(mb->service, "imsp") == 0) {
		if (*mb->user) {
			strcpy(user, mb->user);
		} else {
			strcpy(user, imsp_user);
		}
		strcpy (pwd, imsp_password);
	} else {
#endif
		if (*mb->user) {
			strcpy (user,mb->user);
		} else {
			strcpy (user, IMAPG(imap_user));
		}
		strcpy (pwd, IMAPG(imap_password));
#if HAVE_IMSP
	}
#endif
}

void mm_critical(MAILSTREAM *stream)
{
}

void mm_nocritical(MAILSTREAM *stream)
{
}

long mm_diskerror(MAILSTREAM *stream, long errcode, long serious)
{
	return 1;
}

void mm_fatal(char *str)
{
}
