/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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
   |          Rasmus Lerdorf      <rasmus@php.net>                        |
   |          Chuck Hagenbuch     <chuck@horde.org>                       |
   |          Andrew Skalski      <askalski@chekinc.com>                  |
   |          Hartmut Holzgraefe  <hartmut@six.de>                        |
   |          Jani Taskinen       <sniper@iki.fi>                         |
   |          Daniel R. Kalowsky  <kalowsky@php.net>                      |
   | PHP 4.0 updates:  Zeev Suraski <zeev@zend.com>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#define IMAP41

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#ifdef ERROR
#undef ERROR
#endif
#include "php_imap.h"

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#ifdef PHP_WIN32
#include <winsock.h>
#include <stdlib.h>
#include "win32/sendmail.h"
MAILSTREAM DEFAULTPROTO;
#endif

#define CRLF	"\015\012"
#define PHP_EXPUNGE 32768

static void _php_make_header_object(zval *myzvalue, ENVELOPE *en TSRMLS_DC);
static void _php_imap_add_body(zval *arg, BODY *body TSRMLS_DC);
static void _php_imap_parse_address(ADDRESS *addresslist, char **fulladdress, zval *paddress TSRMLS_DC);
static int _php_imap_address_size(ADDRESS *addresslist);

/* These function declarations are missing from the IMAP header files... */
void rfc822_date(char *date);
char *cpystr(const char *str);
char *cpytxt(SIZEDTEXT *dst, char *text, unsigned long size);
long utf8_mime2text(SIZEDTEXT *src, SIZEDTEXT *dst);
unsigned long find_rightmost_bit(unsigned long *valptr);
void fs_give(void **block);
void *fs_get(size_t size);


/* {{{ imap_functions[]
 */
function_entry imap_functions[] = {
	PHP_FE(imap_open,								NULL)
	PHP_FE(imap_reopen,								NULL)
	PHP_FE(imap_close,								NULL)
	PHP_FE(imap_num_msg,							NULL)
	PHP_FE(imap_num_recent,							NULL)
	PHP_FE(imap_headers,							NULL)
	PHP_FE(imap_headerinfo,							NULL)
	PHP_FE(imap_rfc822_parse_headers,				NULL)
	PHP_FE(imap_rfc822_write_address,				NULL)
	PHP_FE(imap_rfc822_parse_adrlist,				NULL)
	PHP_FE(imap_body,								NULL)
	PHP_FE(imap_bodystruct,							NULL)
	PHP_FE(imap_fetchbody,							NULL)
	PHP_FE(imap_fetchheader,						NULL)
	PHP_FE(imap_fetchstructure,						NULL)
	PHP_FE(imap_expunge,							NULL)
	PHP_FE(imap_delete,								NULL)
	PHP_FE(imap_undelete,							NULL)
	PHP_FE(imap_check,								NULL)
	PHP_FE(imap_mail_copy,							NULL)
	PHP_FE(imap_mail_move,							NULL)
	PHP_FE(imap_mail_compose,						NULL)
	PHP_FE(imap_createmailbox,						NULL)
	PHP_FE(imap_renamemailbox,						NULL)
	PHP_FE(imap_deletemailbox,						NULL)
	PHP_FE(imap_subscribe,							NULL)
	PHP_FE(imap_unsubscribe,						NULL)
	PHP_FE(imap_append,								NULL)
	PHP_FE(imap_ping,								NULL)
	PHP_FE(imap_base64,								NULL)
	PHP_FE(imap_qprint,								NULL)
	PHP_FE(imap_8bit,								NULL)
	PHP_FE(imap_binary,								NULL)
	PHP_FE(imap_utf8,      						 	NULL)
	PHP_FE(imap_status,								NULL)
	PHP_FE(imap_mailboxmsginfo,						NULL)
	PHP_FE(imap_setflag_full,						NULL)
	PHP_FE(imap_clearflag_full,						NULL)
	PHP_FE(imap_sort,								NULL)
	PHP_FE(imap_uid,								NULL)
	PHP_FE(imap_msgno,								NULL)
	PHP_FE(imap_list,								NULL)
	PHP_FE(imap_lsub,								NULL)
	PHP_FE(imap_fetch_overview,						NULL)
	PHP_FE(imap_alerts,								NULL)
	PHP_FE(imap_errors,								NULL)
	PHP_FE(imap_last_error,							NULL)
	PHP_FE(imap_search,								NULL)
	PHP_FE(imap_utf7_decode,						NULL)
	PHP_FE(imap_utf7_encode,						NULL)
	PHP_FE(imap_mime_header_decode,					NULL)
	PHP_FE(imap_thread,								NULL)

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
	PHP_FE(imap_get_quota,							NULL)
	PHP_FE(imap_get_quotaroot,						NULL)
	PHP_FE(imap_set_quota,							NULL)
 	PHP_FE(imap_setacl,								NULL)
#endif

	PHP_FE(imap_mail,								NULL)

	PHP_FALIAS(imap_header, 		imap_headerinfo,	NULL)
	PHP_FALIAS(imap_listmailbox, 	imap_list,			NULL)
	PHP_FALIAS(imap_getmailboxes, 	imap_list_full,		NULL)
	PHP_FALIAS(imap_scanmailbox, 	imap_listscan,		NULL)
	PHP_FALIAS(imap_listsubscribed, imap_lsub,			NULL)
	PHP_FALIAS(imap_getsubscribed, 	imap_lsub_full,		NULL)
	PHP_FALIAS(imap_fetchtext,		imap_body,			NULL)
	PHP_FALIAS(imap_scan,			imap_listscan,		NULL)
	PHP_FALIAS(imap_create,			imap_createmailbox,	NULL)
	PHP_FALIAS(imap_rename,			imap_renamemailbox,	NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ imap_module_entry
 */
zend_module_entry imap_module_entry = {
	STANDARD_MODULE_HEADER,
	"imap",
	imap_functions,
	PHP_MINIT(imap),
	NULL,
	PHP_RINIT(imap),
	PHP_RSHUTDOWN(imap),
	PHP_MINFO(imap),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(imap)

#ifdef COMPILE_DL_IMAP
ZEND_GET_MODULE(imap)
#endif

/* True globals, no need for thread safety */
static int le_imap;

/* {{{ mail_close_it
 */
static void mail_close_it(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pils *imap_le_struct = (pils *)rsrc->ptr;

	mail_close_full(imap_le_struct->imap_stream, imap_le_struct->flags);

	if (IMAPG(imap_user)) {
		efree(IMAPG(imap_user));
		IMAPG(imap_user) = 0;
	}
	if (IMAPG(imap_password)) {
		efree(IMAPG(imap_password));
		IMAPG(imap_password) = 0;
	}

	efree(imap_le_struct);
}
/* }}} */

/* {{{ add_assoc_object
 */
static int add_assoc_object(zval *arg, char *key, zval *tmp)
{
	HashTable *symtable;
	
	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}
	return zend_hash_update(symtable, key, strlen(key)+1, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ add_next_index_object
 */
static inline int add_next_index_object(zval *arg, zval *tmp)
{
	HashTable *symtable;
	
	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}

	return zend_hash_next_index_insert(symtable, (void *) &tmp, sizeof(zval *), NULL); 
}
/* }}} */

/* {{{ mail_newfolderobjectlist
 *
 * Mail instantiate FOBJECTLIST
 * Returns: new FOBJECTLIST list
 * Author: CJH
 */
FOBJECTLIST *mail_newfolderobjectlist(void)
{
  return (FOBJECTLIST *) memset(fs_get(sizeof(FOBJECTLIST)), 0, sizeof(FOBJECTLIST));
}
/* }}} */

/* {{{ mail_free_foblist
 *
 * Mail garbage collect FOBJECTLIST
 * Accepts: pointer to FOBJECTLIST pointer
 * Author: CJH
 */
void mail_free_foblist(FOBJECTLIST **foblist, FOBJECTLIST **tail)
{
	FOBJECTLIST *cur, *next;

	for (cur=*foblist, next=cur->next; cur; cur=next) {
		next = cur->next;

		if(cur->text.data)
			fs_give((void **)&(cur->text.data));

		fs_give((void **)&cur);
	}

	*tail = NIL;
	*foblist = NIL;
}
/* }}} */

/* {{{ mail_newerrorlist
 *
 * Mail instantiate ERRORLIST
 * Returns: new ERRORLIST list
 * Author: CJH
 */
ERRORLIST *mail_newerrorlist(void)
{
	return (ERRORLIST *) memset(fs_get(sizeof(ERRORLIST)), 0, sizeof(ERRORLIST));
}
/* }}} */

/* {{{ mail_free_errorlist
 *
 * Mail garbage collect FOBJECTLIST
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
/* }}} */

/* {{{ mail_newmessagelist
 * 
 * Mail instantiate MESSAGELIST
 * Returns: new MESSAGELIST list
 * Author: CJH
 */
MESSAGELIST *mail_newmessagelist(void)
{
	return (MESSAGELIST *) memset(fs_get(sizeof(MESSAGELIST)), 0, sizeof(MESSAGELIST));
}
/* }}} */

/* {{{ mail_free_messagelist
 *
 * Mail garbage collect MESSAGELIST
 * Accepts: pointer to MESSAGELIST pointer
 * Author: CJH
 */
void mail_free_messagelist(MESSAGELIST **msglist, MESSAGELIST **tail)
{
	MESSAGELIST *cur, *next;

	for (cur = *msglist, next = cur->next; cur; cur = next) {
		next = cur->next;
		fs_give((void **)&cur);
	}

	*tail = NIL;
	*msglist = NIL;
}
/* }}} */

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001) 
/* {{{ mail_getquota 
 *
 * Mail GET_QUOTA callback
 * Called via the mail_parameter function in c-client:src/c-client/mail.c
 * Author DRK
 */
void mail_getquota(MAILSTREAM *stream, char *qroot, QUOTALIST *qlist)
{
	zval *t_map;
	TSRMLS_FETCH();
/* put parsing code here */
	for(; qlist; qlist = qlist->next) {
		MAKE_STD_ZVAL(t_map);
		if (array_init(t_map) == FAILURE) {
			php_error(E_WARNING, "Unable to allocate t_map memory");
			FREE_ZVAL(t_map);
			FREE_ZVAL(IMAPG(quota_return));
			return;
		}
		if (strncmp(qlist->name, "STORAGE", 7) == 0)
		{
			/* this is to add backwards compatibility */
			add_assoc_long_ex(IMAPG(quota_return), "usage", sizeof("usage"), qlist->usage);
			add_assoc_long_ex(IMAPG(quota_return), "limit", sizeof("limit"), qlist->limit);
		}

		add_assoc_long_ex(t_map, "usage", sizeof("usage"), qlist->usage);
		add_assoc_long_ex(t_map, "limit", sizeof("limit"), qlist->limit);
		add_assoc_zval_ex(IMAPG(quota_return), qlist->name, strlen(qlist->name)+1, t_map);
	}
}
/* }}} */
#endif


/* {{{ php_imap_init_globals
 */
static void php_imap_init_globals(zend_imap_globals *imap_globals)
{
	imap_globals->imap_user = NIL;
	imap_globals->imap_password = NIL;

	imap_globals->imap_alertstack = NIL;
	imap_globals->imap_errorstack = NIL;

	imap_globals->imap_folders = NIL;
	imap_globals->imap_folders_tail = NIL;
	imap_globals->imap_sfolders = NIL;
	imap_globals->imap_sfolders_tail = NIL;
	imap_globals->imap_messages = NIL;
	imap_globals->imap_messages_tail = NIL;
	imap_globals->imap_folder_objects = NIL;
	imap_globals->imap_folder_objects_tail = NIL;
	imap_globals->imap_sfolder_objects = NIL;
	imap_globals->imap_sfolder_objects_tail = NIL;

	imap_globals->folderlist_style = FLIST_ARRAY;
#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
	imap_globals->quota_return = NULL;
#endif
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(imap)
{
	unsigned long sa_all =	SA_MESSAGES | SA_RECENT | SA_UNSEEN | SA_UIDNEXT | SA_UIDVALIDITY;

	ZEND_INIT_MODULE_GLOBALS(imap, php_imap_init_globals, NULL)

#ifndef PHP_WIN32
	mail_link(&unixdriver);		/* link in the unix driver */
	mail_link(&mhdriver);		/* link in the mh driver */
	/* mail_link(&mxdriver); */	/* According to c-client docs (internal.txt) this shouldn't be used. */
	mail_link(&mmdfdriver);		/* link in the mmdf driver */
	mail_link(&newsdriver);		/* link in the news driver */
	mail_link(&philedriver);	/* link in the phile driver */
#endif
	mail_link(&imapdriver);		/* link in the imap driver */
	mail_link(&nntpdriver);		/* link in the nntp driver */
	mail_link(&pop3driver);		/* link in the pop3 driver */
	mail_link(&mbxdriver);		/* link in the mbx driver */
	mail_link(&tenexdriver);	/* link in the tenex driver */
	mail_link(&mtxdriver);		/* link in the mtx driver */
	mail_link(&dummydriver);	/* link in the dummy driver */

#ifndef PHP_WIN32
	auth_link(&auth_log);		/* link in the log authenticator */
	auth_link(&auth_md5);       /* link in the cram-md5 authenticator */ 
#ifdef  HAVE_IMAP_SSL
	ssl_onceonlyinit ();
#endif
#endif

	/* lets allow NIL */
	REGISTER_LONG_CONSTANT("NIL", NIL, CONST_PERSISTENT | CONST_CS);


	/* Open Options */

	REGISTER_LONG_CONSTANT("OP_DEBUG", OP_DEBUG, CONST_PERSISTENT | CONST_CS);
	/* debug protocol negotiations */
	REGISTER_LONG_CONSTANT("OP_READONLY", OP_READONLY, CONST_PERSISTENT | CONST_CS);
	/* read-only open */
	REGISTER_LONG_CONSTANT("OP_ANONYMOUS", OP_ANONYMOUS, CONST_PERSISTENT | CONST_CS);
	/* anonymous open of newsgroup */
	REGISTER_LONG_CONSTANT("OP_SHORTCACHE", OP_SHORTCACHE, CONST_PERSISTENT | CONST_CS);
	/* short (elt-only) caching */
	REGISTER_LONG_CONSTANT("OP_SILENT", OP_SILENT, CONST_PERSISTENT | CONST_CS);
	/* don't pass up events (internal use) */
	REGISTER_LONG_CONSTANT("OP_PROTOTYPE", OP_PROTOTYPE, CONST_PERSISTENT | CONST_CS);
	/* return driver prototype */
	REGISTER_LONG_CONSTANT("OP_HALFOPEN", OP_HALFOPEN, CONST_PERSISTENT | CONST_CS);
	/* half-open (IMAP connect but no select) */
	REGISTER_LONG_CONSTANT("OP_EXPUNGE", OP_EXPUNGE, CONST_PERSISTENT | CONST_CS);
	/* silently expunge recycle stream */
	REGISTER_LONG_CONSTANT("OP_SECURE", OP_SECURE, CONST_PERSISTENT | CONST_CS);
	/* don't do non-secure authentication */

	/* 
	PHP re-assigns CL_EXPUNGE a custom value that can be used as part of the imap_open() bitfield
	because it seems like a good idea to be able to indicate that the mailbox should be 
	automatically expunged during imap_open in case the script get interrupted and it doesn't get
	to the imap_close() where this option is normally placed.  If the c-client library adds other
	options and the value for this one conflicts, simply make PHP_EXPUNGE higher at the top of 
	this file 
	*/
	REGISTER_LONG_CONSTANT("CL_EXPUNGE", PHP_EXPUNGE, CONST_PERSISTENT | CONST_CS);
	/* expunge silently */


	/* Fetch options */

	REGISTER_LONG_CONSTANT("FT_UID", FT_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID */
	REGISTER_LONG_CONSTANT("FT_PEEK", FT_PEEK, CONST_PERSISTENT | CONST_CS);
	/* peek at data */
	REGISTER_LONG_CONSTANT("FT_NOT", FT_NOT, CONST_PERSISTENT | CONST_CS);
	/* NOT flag for header lines fetch */
	REGISTER_LONG_CONSTANT("FT_INTERNAL", FT_INTERNAL, CONST_PERSISTENT | CONST_CS);
	/* text can be internal strings */
	REGISTER_LONG_CONSTANT("FT_PREFETCHTEXT", FT_PREFETCHTEXT, CONST_PERSISTENT | CONST_CS);
	/* IMAP prefetch text when fetching header */


	/* Flagging options */

	REGISTER_LONG_CONSTANT("ST_UID", ST_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID sequence */
	REGISTER_LONG_CONSTANT("ST_SILENT", ST_SILENT, CONST_PERSISTENT | CONST_CS);
	/* don't return results */
	REGISTER_LONG_CONSTANT("ST_SET", ST_SET, CONST_PERSISTENT | CONST_CS);
	/* set vs. clear */


	/* Copy options */

	REGISTER_LONG_CONSTANT("CP_UID", CP_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID sequence */
	REGISTER_LONG_CONSTANT("CP_MOVE", CP_MOVE, CONST_PERSISTENT | CONST_CS);
	/* delete from source after copying */


	/* Search/sort options */

	REGISTER_LONG_CONSTANT("SE_UID", SE_UID, CONST_PERSISTENT | CONST_CS);
	/* return UID */
	REGISTER_LONG_CONSTANT("SE_FREE", SE_FREE, CONST_PERSISTENT | CONST_CS);
	/* free search program after finished */
	REGISTER_LONG_CONSTANT("SE_NOPREFETCH", SE_NOPREFETCH, CONST_PERSISTENT | CONST_CS);
	/* no search prefetching */
	REGISTER_LONG_CONSTANT("SO_FREE", SO_FREE, CONST_PERSISTENT | CONST_CS);
	/* free sort program after finished */
	REGISTER_LONG_CONSTANT("SO_NOSERVER", SO_NOSERVER, CONST_PERSISTENT | CONST_CS);
	/* don't do server-based sort */


	/* Status options */

	REGISTER_LONG_CONSTANT("SA_MESSAGES", SA_MESSAGES , CONST_PERSISTENT | CONST_CS);
	/* number of messages */
	REGISTER_LONG_CONSTANT("SA_RECENT", SA_RECENT, CONST_PERSISTENT | CONST_CS);
	/* number of recent messages */
	REGISTER_LONG_CONSTANT("SA_UNSEEN", SA_UNSEEN , CONST_PERSISTENT | CONST_CS);
	/* number of unseen messages */
	REGISTER_LONG_CONSTANT("SA_UIDNEXT", SA_UIDNEXT, CONST_PERSISTENT | CONST_CS);
	/* next UID to be assigned */
	REGISTER_LONG_CONSTANT("SA_UIDVALIDITY", SA_UIDVALIDITY , CONST_PERSISTENT | CONST_CS);
	/* UID validity value */
	REGISTER_LONG_CONSTANT("SA_ALL", sa_all, CONST_PERSISTENT | CONST_CS);
	/* get all status information */


	/* Bits for mm_list() and mm_lsub() */

	REGISTER_LONG_CONSTANT("LATT_NOINFERIORS", LATT_NOINFERIORS , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LATT_NOSELECT", LATT_NOSELECT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LATT_MARKED", LATT_MARKED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LATT_UNMARKED", LATT_UNMARKED , CONST_PERSISTENT | CONST_CS);


	/* Sort functions */

	REGISTER_LONG_CONSTANT("SORTDATE", SORTDATE , CONST_PERSISTENT | CONST_CS);
	/* date */
	REGISTER_LONG_CONSTANT("SORTARRIVAL", SORTARRIVAL , CONST_PERSISTENT | CONST_CS);
	/* arrival date */
	REGISTER_LONG_CONSTANT("SORTFROM", SORTFROM , CONST_PERSISTENT | CONST_CS);
	/* from */
	REGISTER_LONG_CONSTANT("SORTSUBJECT", SORTSUBJECT , CONST_PERSISTENT | CONST_CS);
	/* subject */
	REGISTER_LONG_CONSTANT("SORTTO", SORTTO , CONST_PERSISTENT | CONST_CS);
	/* to */
	REGISTER_LONG_CONSTANT("SORTCC", SORTCC , CONST_PERSISTENT | CONST_CS);
	/* cc */
	REGISTER_LONG_CONSTANT("SORTSIZE", SORTSIZE , CONST_PERSISTENT | CONST_CS);
	/* size */

	REGISTER_LONG_CONSTANT("TYPETEXT", TYPETEXT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEMULTIPART", TYPEMULTIPART , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEMESSAGE", TYPEMESSAGE , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEAPPLICATION", TYPEAPPLICATION , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEAUDIO", TYPEAUDIO , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEIMAGE", TYPEIMAGE , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEVIDEO", TYPEVIDEO , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEOTHER", TYPEOTHER , CONST_PERSISTENT | CONST_CS);
	/*
	TYPETEXT                unformatted text
	TYPEMULTIPART           multiple part
	TYPEMESSAGE             encapsulated message
	TYPEAPPLICATION         application data
	TYPEAUDIO               audio
	TYPEIMAGE               static image (GIF, JPEG, etc.)
	TYPEVIDEO               video
	TYPEOTHER               unknown
	*/

	REGISTER_LONG_CONSTANT("ENC7BIT", ENC7BIT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENC8BIT", ENC8BIT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCBINARY", ENCBINARY , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCBASE64", ENCBASE64, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCQUOTEDPRINTABLE", ENCQUOTEDPRINTABLE , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCOTHER", ENCOTHER , CONST_PERSISTENT | CONST_CS);
	/*
	ENC7BIT                 7 bit SMTP semantic data
	ENC8BIT                 8 bit SMTP semantic data
	ENCBINARY               8 bit binary data
	ENCBASE64               base-64 encoded data
	ENCQUOTEDPRINTABLE      human-readable 8-as-7 bit data
	ENCOTHER                unknown
	*/

	le_imap = zend_register_list_destructors_ex(mail_close_it, NULL, "imap", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(imap)
{
	IMAPG(imap_errorstack) = NIL;
	IMAPG(imap_alertstack) = NIL;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(imap)
{
	ERRORLIST *ecur = NIL;
	STRINGLIST *acur = NIL;

	if (IMAPG(imap_errorstack) != NIL) {
		/* output any remaining errors at their original error level */
		if (EG(error_reporting) & E_NOTICE) {
			ecur = IMAPG(imap_errorstack);
			while (ecur != NIL) {
				php_error(E_NOTICE, "%s(): %s (errflg=%d)", get_active_function_name(TSRMLS_C), ecur->LTEXT, ecur->errflg);
				ecur = ecur->next;
			}
		}
		mail_free_errorlist(&IMAPG(imap_errorstack));
	}

	if (IMAPG(imap_alertstack) != NIL) {
		/* output any remaining alerts at E_NOTICE level */
		if (EG(error_reporting) & E_NOTICE) {
			acur = IMAPG(imap_alertstack);
			while (acur != NIL) {
				php_error(E_NOTICE, "%s(): %s", get_active_function_name(TSRMLS_C), acur->LTEXT);
				acur = acur->next;
			}
		}
		mail_free_stringlist(&IMAPG(imap_alertstack));
		IMAPG(imap_alertstack) = NIL;
	}
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(imap)
{
	php_info_print_table_start();
#if HAVE_IMAP2001
	php_info_print_table_row(2, "IMAP c-Client Version", "2001");
#elif HAVE_IMAP2000
	php_info_print_table_row(2, "IMAP c-Client Version", "2000");
#elif defined(IMAP41)
	php_info_print_table_row(2, "IMAP c-Client Version", "4.1");
#else
	php_info_print_table_row(2, "IMAP c-Client Version", "4.0");
#endif
#if HAVE_IMAP_SSL
	php_info_print_table_row(2, "SSL Support", "enabled");
#endif
#if HAVE_IMAP_KRB
	php_info_print_table_row(2, "Kerberos Support", "enabled");
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ imap_do_open
 */
static void php_imap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval **mailbox, **user, **passwd, **options;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct;
	long flags=NIL;
	long cl_flags=NIL;
	int myargc = ZEND_NUM_ARGS();
	
	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &mailbox, &user, &passwd, &options) == FAILURE) {
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

	if (IMAPG(imap_user)) { 
		efree(IMAPG(imap_user));
	}

	if (IMAPG(imap_password)) { 
		efree(IMAPG(imap_password));
	}

	IMAPG(imap_user)     = estrndup(Z_STRVAL_PP(user), Z_STRLEN_PP(user));
	IMAPG(imap_password) = estrndup(Z_STRVAL_PP(passwd), Z_STRLEN_PP(passwd));
	
	imap_stream = mail_open(NIL, Z_STRVAL_PP(mailbox), flags);

	if (imap_stream == NIL) {
		php_error(E_WARNING, "%s(): Couldn't open stream %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(mailbox));
		efree(IMAPG(imap_user)); IMAPG(imap_user) = 0;
		efree(IMAPG(imap_password)); IMAPG(imap_password) = 0;
		RETURN_FALSE;
	}

	imap_le_struct = emalloc(sizeof(pils));
	imap_le_struct->imap_stream = imap_stream;
	imap_le_struct->flags = cl_flags;	

	ZEND_REGISTER_RESOURCE(return_value, imap_le_struct, le_imap);
}
/* }}} */

/* {{{ proto int imap_open(string mailbox, string user, string password [, int options])
   Open an IMAP stream to a mailbox */
PHP_FUNCTION(imap_open)
{
	php_imap_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imap_reopen(int stream_id, string mailbox [, int options])
   Reopen an IMAP stream to a new mailbox */
PHP_FUNCTION(imap_reopen)
{
	zval **streamind, **mailbox, **options;
	pils *imap_le_struct; 
	MAILSTREAM *imap_stream;
	long flags=NIL;
	long cl_flags=NIL;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &mailbox, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

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
	imap_stream = mail_open(imap_le_struct->imap_stream, Z_STRVAL_PP(mailbox), flags);
	if (imap_stream == NIL) {
		php_error(E_WARNING, "%s(): Couldn't re-open stream", get_active_function_name(TSRMLS_C));
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
	pils *imap_le_struct; 
	STRING st;
	int myargc=ZEND_NUM_ARGS();
  
	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &folder, &message, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
  
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(folder);
	convert_to_string_ex(message);

	if (myargc == 4) {
		convert_to_string_ex(flags);
	}

	INIT (&st, mail_string, (void *) Z_STRVAL_PP(message), Z_STRLEN_PP(message));

	if (mail_append_full(imap_le_struct->imap_stream, Z_STRVAL_PP(folder), myargc==4 ? Z_STRVAL_PP(flags) : NIL, NIL, &st)) {
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
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	RETURN_LONG(imap_le_struct->imap_stream->nmsgs);
}
/* }}} */

/* {{{ proto int imap_ping(int stream_id)
   Check if the IMAP stream is still active */
PHP_FUNCTION(imap_ping)
{
	zval **streamind;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	RETURN_LONG(mail_ping(imap_le_struct->imap_stream));
}
/* }}} */

/* {{{ proto int imap_num_recent(int stream_id)
   Gives the number of recent messages in current mailbox */
PHP_FUNCTION(imap_num_recent)
{
	zval **streamind;
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	RETURN_LONG(imap_le_struct->imap_stream->recent);
}
/* }}} */

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
/* {{{ proto array imap_get_quota(int stream_id, string qroot)
	Returns the quota set to the mailbox account qroot */
PHP_FUNCTION(imap_get_quota)
{
	zval **streamind, **qroot;
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &qroot) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(qroot);

	MAKE_STD_ZVAL(IMAPG(quota_return));
	if (array_init(IMAPG(quota_return)) == FAILURE) {
		php_error(E_WARNING, "%s(): Unable to allocate array memory", get_active_function_name(TSRMLS_C));
		FREE_ZVAL(IMAPG(quota_return));
		RETURN_FALSE;
	}

	/* set the callback for the GET_QUOTA function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if(!imap_getquota(imap_le_struct->imap_stream, Z_STRVAL_PP(qroot))) {
		php_error(E_WARNING, "%s(): c-client imap_getquota failed", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	*return_value = *IMAPG(quota_return);
	FREE_ZVAL(IMAPG(quota_return));
}
/* }}} */

/* {{{ proto array imap_get_quotaroot(int stream_id, string mbox)
	Returns the quota set to the mailbox account mbox */
PHP_FUNCTION(imap_get_quotaroot)
{
	zval **streamind, **mbox;
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &mbox) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(mbox);

	MAKE_STD_ZVAL(IMAPG(quota_return));
	if (array_init(IMAPG(quota_return)) == FAILURE) {
		php_error(E_WARNING, "%s(): Unable to allocate array memory", get_active_function_name(TSRMLS_C));
		FREE_ZVAL(IMAPG(quota_return));
		RETURN_FALSE;
	}

	/* set the callback for the GET_QUOTAROOT function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if(!imap_getquotaroot(imap_le_struct->imap_stream, Z_STRVAL_PP(mbox))) {
		php_error(E_WARNING, "%s(): c-client imap_getquotaroot failed", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	*return_value = *IMAPG(quota_return);
	FREE_ZVAL(IMAPG(quota_return));
}
/* }}} */

/* {{{ proto int imap_set_quota(int stream_id, string qroot, int mailbox_size)
   Will set the quota for qroot mailbox */
PHP_FUNCTION(imap_set_quota)
{
	zval **streamind, **qroot, **mailbox_size;
	pils *imap_le_struct;
	STRINGLIST	limits;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &qroot, &mailbox_size) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(qroot);
	convert_to_long_ex(mailbox_size);

	limits.text.data = "STORAGE";
	limits.text.size = Z_LVAL_PP(mailbox_size);
	limits.next = NIL;

	RETURN_LONG(imap_setquota(imap_le_struct->imap_stream, Z_STRVAL_PP(qroot), &limits)); 
}
/* }}} */

/* {{{ proto int imap_setacl(int stream_id, string mailbox, string id, string rights)
	Sets the ACL for a given mailbox */
PHP_FUNCTION(imap_setacl)
{
	zval **streamind, **mailbox, **id, **rights;
	pils *imap_le_struct;
	
	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &streamind, &mailbox, &id, &rights) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(mailbox);
	convert_to_string_ex(rights);

	RETURN_LONG(imap_setacl(imap_le_struct->imap_stream, Z_STRVAL_PP(mailbox), Z_STRVAL_PP(id), Z_STRVAL_PP(rights)));
}
/* }}} */

#endif /* HAVE_IMAP2000 || HAVE_IMAP2001 */


/* {{{ proto int imap_expunge(int stream_id)
   Permanently delete all messages marked for deletion */
PHP_FUNCTION(imap_expunge)
{
	zval **streamind;
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	mail_expunge (imap_le_struct->imap_stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imap_close(int stream_id [, int options])
   Close an IMAP stream */
PHP_FUNCTION(imap_close)
{
	zval **options, **streamind=NULL;
	pils *imap_le_struct=NULL; 
	long flags = NIL;
	int myargcount=ZEND_NUM_ARGS();

	if (myargcount < 1 || myargcount > 2 || zend_get_parameters_ex(myargcount, &streamind, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	if (myargcount == 2) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
		/* Do the translation from PHP's internal PHP_EXPUNGE define to c-client's CL_EXPUNGE */
		if (flags & PHP_EXPUNGE) {
			flags ^= PHP_EXPUNGE;
			flags |= CL_EXPUNGE;
		}	
		imap_le_struct->flags = flags;
	}

	zend_list_delete(Z_RESVAL_PP(streamind));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imap_headers(int stream_id)
   Returns headers for all messages in a mailbox */
PHP_FUNCTION(imap_headers)
{
	zval **streamind;
	pils *imap_le_struct; 
	unsigned long i;
	char *t;
	unsigned int msgno;
	char tmp[MAILTMPLEN];
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream, msgno);
		mail_fetchstructure(imap_le_struct->imap_stream, msgno, NIL);
		tmp[0] = cache->recent ? (cache->seen ? 'R': 'N') : ' ';
		tmp[1] = (cache->recent | cache->seen) ? ' ' : 'U';
		tmp[2] = cache->flagged ? 'F' : ' ';
		tmp[3] = cache->answered ? 'A' : ' ';
		tmp[4] = cache->deleted ? 'D' : ' ';
		tmp[5] = cache->draft ? 'X' : ' ';
		sprintf(tmp + 6, "%4ld) ", cache->msgno);
		mail_date(tmp+11, cache);
		tmp[22] = ' ';
		tmp[23] = '\0';
		mail_fetchfrom(tmp+23, imap_le_struct->imap_stream, msgno, (long)20);
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
	pils *imap_le_struct; 
	int msgindex, myargc=ZEND_NUM_ARGS();

	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
	
	convert_to_long_ex(msgno);
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}

	if ((myargc == 3) && (Z_LVAL_PP(flags) & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, Z_LVAL_PP(msgno));
	} else {
		msgindex = Z_LVAL_PP(msgno);
	}
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error(E_WARNING, "%s(): Bad message number", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	RETVAL_STRING(mail_fetchtext_full (imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL, myargc==3 ? Z_LVAL_PP(flags) : NIL), 1);
}
/* }}} */

/* {{{ proto int imap_mail_copy(int stream_id, int msg_no, string mailbox [, int options])
   Copy specified message to a mailbox */
PHP_FUNCTION(imap_mail_copy)
{
	zval **streamind, **seq, **folder, **options;
	pils *imap_le_struct; 
	int myargcount = ZEND_NUM_ARGS();

	if (myargcount > 4 || myargcount < 3 || zend_get_parameters_ex(myargcount, &streamind, &seq, &folder, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(seq);
	convert_to_string_ex(folder);
	if (myargcount == 4) {
		convert_to_long_ex(options);
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
	pils *imap_le_struct; 
	int myargcount = ZEND_NUM_ARGS();

	if (myargcount > 4 || myargcount < 3 || zend_get_parameters_ex(myargcount, &streamind, &seq, &folder, &options) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(seq);
	convert_to_string_ex(folder);
	if (myargcount == 4) {
		convert_to_long_ex(options);
	}

	if (mail_copy_full(imap_le_struct->imap_stream, Z_STRVAL_PP(seq), Z_STRVAL_PP(folder), myargcount == 4 ? (Z_LVAL_PP(options) | CP_MOVE) : CP_MOVE) == T) {
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
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(folder);

	if (mail_create(imap_le_struct->imap_stream, Z_STRVAL_PP(folder)) == T) {
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
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &old_mailbox, &new_mailbox) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(old_mailbox);
	convert_to_string_ex(new_mailbox);

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
	pils *imap_le_struct; 

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(folder);

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
	pils *imap_le_struct; 
	STRINGLIST *cur=NIL;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(ref);
	convert_to_string_ex(pat);

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;
	
	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
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
	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
}

/* }}} */

/* {{{ proto array imap_getmailboxes(int stream_id, string ref, string pattern)
   Reads the list of mailboxes and returns a full array of objects containing name, attributes, and delimiter */
/* Author: CJH */
PHP_FUNCTION(imap_list_full)
{
	zval **streamind, **ref, **pat, *mboxob;
	pils *imap_le_struct; 
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(ref);
	convert_to_string_ex(pat);

	/* set flag for new, improved array of objects mailbox list */
	IMAPG(folderlist_style) = FLIST_OBJECT;
	
	IMAPG(imap_folder_objects) = IMAPG(imap_folder_objects_tail) = NIL;
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
	mail_free_foblist(&IMAPG(imap_folder_objects), &IMAPG(imap_folder_objects_tail));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY;		/* reset to default */
}
/* }}} */

/* {{{ proto array imap_scan(int stream_id, string ref, string pattern, string content)
   Read list of mailboxes containing a certain string */
PHP_FUNCTION(imap_listscan)
{
	zval **streamind, **ref, **pat, **content;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &streamind, &ref, &pat, &content) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(ref);
	convert_to_string_ex(pat);
	convert_to_string_ex(content);

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
	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
}

/* }}} */

/* {{{ proto object imap_check(int stream_id)
   Get mailbox properties */
PHP_FUNCTION(imap_check)
{
	zval **streamind;
	pils *imap_le_struct;
	char date[100];

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

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
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();
	
	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(sequence);
	if (myargc == 3) {
		convert_to_long_ex(flags);
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
	pils *imap_le_struct;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
	
	convert_to_string_ex(sequence);
	if (myargc == 3) {
		convert_to_long_ex(flags);
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
	pils *imap_le_struct;
	MESSAGECACHE *cache;
	ENVELOPE *en;
	char dummy[2000], fulladdress[MAILTMPLEN];
	int myargc = ZEND_NUM_ARGS();
	
	if (myargc < 2 || myargc > 5 || zend_get_parameters_ex(myargc, &streamind, &msgno, &fromlength, &subjectlength, &defaulthost) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

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
	
	if (!Z_LVAL_PP(msgno) || Z_LVAL_PP(msgno) < 1 || (unsigned) Z_LVAL_PP(msgno) > imap_le_struct->imap_stream->nmsgs) {
		php_error(E_WARNING, "%s(): Bad message number", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	if (mail_fetchstructure(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL)) {
		cache = mail_elt(imap_le_struct->imap_stream, Z_LVAL_PP(msgno));
	} else {
		RETURN_FALSE;
	}
	
	en = mail_fetchenvelope(imap_le_struct->imap_stream, Z_LVAL_PP(msgno));

	/* call a function to parse all the text, so that we can use the
	   same function to parse text from other sources */
	_php_make_header_object(return_value, en TSRMLS_CC);
	
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
		rfc822_parse_msg(&en, NULL, Z_STRVAL_PP(headers), Z_STRLEN_PP(headers), NULL, Z_STRVAL_PP(defaulthost), NIL);
	} else {
		rfc822_parse_msg(&en, NULL, Z_STRVAL_PP(headers), Z_STRLEN_PP(headers), NULL, "UNKNOWN", NIL);
	}
	
	/* call a function to parse all the text, so that we can use the
	   same function no matter where the headers are from */
	_php_make_header_object(return_value, en TSRMLS_CC);
	mail_free_envelope(&en);
}
/* }}} */


/* KMLANG */
/* {{{ proto array imap_lsub(int stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes */
PHP_FUNCTION(imap_lsub)
{
	zval **streamind, **ref, **pat;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
	
	convert_to_string_ex(ref);
	convert_to_string_ex(pat);

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;
	
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
	IMAPG(imap_sfolders) = IMAPG(imap_sfolders_tail) = NIL;
}
/* }}} */

/* {{{ proto array imap_getsubscribed(int stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes, in the same format as imap_getmailboxes() */
/* Author: CJH */
PHP_FUNCTION(imap_lsub_full)
{
	zval **streamind, **ref, **pat, *mboxob;
	pils *imap_le_struct;
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &ref, &pat) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(ref);
	convert_to_string_ex(pat);
	
	delim = emalloc(2 * sizeof(char));
	
	/* set flag for new, improved array of objects list */
	IMAPG(folderlist_style) = FLIST_OBJECT;
	
	IMAPG(imap_sfolder_objects) = IMAPG(imap_sfolder_objects_tail) = NIL;
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
	mail_free_foblist (&IMAPG(imap_sfolder_objects), &IMAPG(imap_sfolder_objects_tail));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY; /* reset to default */
}
/* }}} */

/* {{{ proto int imap_subscribe(int stream_id, string mailbox)
   Subscribe to a mailbox */
PHP_FUNCTION(imap_subscribe)
{
	zval **streamind, **folder;
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(folder);

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
	pils *imap_le_struct;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &folder) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(folder);

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
	pils *imap_le_struct;
	BODY *body;
	int msgindex, myargc=ZEND_NUM_ARGS();

	if (myargc < 2  || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_long_ex(msgno);
	if (Z_LVAL_PP(msgno) < 1) {
		RETURN_FALSE;
	}
	if (myargc == 3) {
		convert_to_long_ex(flags);
	}

	object_init(return_value);

	if ((myargc == 3) && (Z_LVAL_PP(flags) & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, Z_LVAL_PP(msgno));
	} else {
		msgindex = Z_LVAL_PP(msgno);
	}
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error(E_WARNING, "%s(): Bad message number", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	mail_fetchstructure_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), &body , myargc == 3 ? Z_LVAL_PP(flags) : NIL);
	
	if (!body) {
		php_error(E_WARNING, "%s(): No body information available", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	_php_imap_add_body(return_value, body TSRMLS_CC);
}
/* }}} */

/* {{{ proto string imap_fetchbody(int stream_id, int msg_no, int section [, int options])
   Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	zval **streamind, **msgno, **sec, **flags;
	pils *imap_le_struct;
	char *body;
	unsigned long len;
	int myargc=ZEND_NUM_ARGS();

	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &msgno, &sec, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_long_ex(msgno);
	convert_to_string_ex(sec);
	if (myargc == 4) {
		convert_to_long_ex(flags);
	}
 
	body = mail_fetchbody_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), Z_STRVAL_PP(sec), &len, myargc==4 ? Z_LVAL_PP(flags) : NIL);

	if (!body) {
		php_error(E_WARNING, "%s(): No body information available", get_active_function_name(TSRMLS_C));
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

	if (decode == NULL) {
		RETURN_FALSE;
	}

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

	if (decode == NULL) {
		RETURN_FALSE;
	}

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

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength, 1);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ proto string imap_binary(string text)
   Convert an 8bit string to a base64 string */
PHP_FUNCTION(imap_binary)
{
	zval **text;
	char *decode;
	unsigned long newlength;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &text) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(text);

	decode = rfc822_binary(Z_STRVAL_PP(text), Z_STRLEN_PP(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength, 1);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ proto object imap_mailboxmsginfo(int stream_id)
   Returns info about the current mailbox */
PHP_FUNCTION(imap_mailboxmsginfo)
{
	zval **streamind;
	pils *imap_le_struct;
	char date[100];
	unsigned int msgno, unreadmsg, deletedmsg, msize;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	/* Initialize return object */
	if (object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	unreadmsg = 0;
	deletedmsg = 0;
	msize = 0;

	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream, msgno);
		mail_fetchstructure (imap_le_struct->imap_stream, msgno, NIL);

		if (!cache->seen || cache->recent) {
			unreadmsg++;
		}

		if (cache->deleted) {
			deletedmsg++;
		}
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

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &mailbox, &host, &personal) == FAILURE) {
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
	zval **str, **defaulthost, *tovals;
	ADDRESS *addresstmp;
	ENVELOPE *env;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &str, &defaulthost) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	SEPARATE_ZVAL(str);
	convert_to_string_ex(str);
	convert_to_string_ex(defaulthost);

	env = mail_newenvelope();

	rfc822_parse_adrlist(&env->to, Z_STRVAL_PP(str), Z_STRVAL_PP(defaulthost));

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	addresstmp = env->to;

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
	SIZEDTEXT src, dest;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(str);
	
	src.data  = NULL;
	src.size  = 0;
	dest.data = NULL;
	dest.size = 0;

	cpytxt(&src, Z_STRVAL_PP(str), Z_STRLEN_PP(str));
	utf8_mime2text(&src, &dest);
	RETURN_STRINGL(dest.data, strlen(dest.data), 1);
}
/* }}} */


/* {{{ macros for the modified utf7 conversion functions 
 *
 * author: Andrew Skalski <askalski@chek.com> 
 */

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
/* }}} */

/* {{{ proto string imap_utf7_decode(string buf)
   Decode a modified UTF-7 string */
PHP_FUNCTION(imap_utf7_decode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	zval **arg;
	const unsigned char *in, *inp, *endp;
	unsigned char *out, *outp;
	unsigned char c;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_DECODE0,	/* encoded text rotation... */
		ST_DECODE1,
		ST_DECODE2,
		ST_DECODE3
	} state;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
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
				php_error(E_WARNING, "%s(): Invalid modified UTF-7 character: `%c'", get_active_function_name(TSRMLS_C), *inp);
				RETURN_FALSE;
			} else if (*inp != '&') {
				outlen++;
			} else if (inp + 1 == endp) {
				php_error(E_WARNING, "%s(): Unexpected end of string", get_active_function_name(TSRMLS_C));
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
				php_error(E_WARNING, "%s(): Stray modified base64 character: `%c'", get_active_function_name(TSRMLS_C), *--inp);
				RETURN_FALSE;
			}
			state = ST_NORMAL;
		} else if (!B64CHAR(*inp)) {
			php_error(E_WARNING, "%s(): Invalid modified base64 character: `%c'", get_active_function_name(TSRMLS_C), *inp);
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
		php_error(E_WARNING, "%s(): Unexpected end of string", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* allocate output buffer */
	if ((out = emalloc(outlen + 1)) == NULL) {
		php_error(E_WARNING, "%s(): Unable to allocate result string", get_active_function_name(TSRMLS_C));
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
				c = outp[1] >> 4;
				*outp++ |= c;
				*outp <<= 4;
				state = ST_DECODE2;
				break;
			case ST_DECODE2:
				outp[1] = UNB64(*inp);
				c = outp[1] >> 2;
				*outp++ |= c;
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
		php_error(E_WARNING, "%s(): outp - out [%d] != outlen [%d]", get_active_function_name(TSRMLS_C), outp - out, outlen);
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
	zval **arg;
	const unsigned char *in, *inp, *endp;
	unsigned char *out, *outp;
	unsigned char c;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_ENCODE0,	/* encoded text rotation... */
		ST_ENCODE1,
		ST_ENCODE2
	} state;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
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
		php_error(E_WARNING, "%s(): Unable to allocate result string", get_active_function_name(TSRMLS_C));
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
					c = B64(*outp | *inp >> 4);
					*outp++ = c;
					*outp = *inp++ << 2;
					state = ST_ENCODE2;
					break;
				case ST_ENCODE2:
					c = B64(*outp | *inp >> 6);
					*outp++ = c;
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
		php_error(E_WARNING, "%s(): outp - out [%d] != outlen [%d]", get_active_function_name(TSRMLS_C), outp - out, outlen);
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
	pils *imap_le_struct;
	int myargc = ZEND_NUM_ARGS();

	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flag, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(sequence);
	convert_to_string_ex(flag);
	if (myargc==4) {
		convert_to_long_ex(flags);
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
	pils *imap_le_struct;
	int myargc = ZEND_NUM_ARGS();

	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &streamind, &sequence, &flag, &flags) ==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(sequence);
	convert_to_string_ex(flag);
	if (myargc==4) {
		convert_to_long_ex(flags);
	}

	mail_clearflag_full(imap_le_struct->imap_stream, Z_STRVAL_PP(sequence), Z_STRVAL_PP(flag), myargc==4 ? Z_LVAL_PP(flags) : NIL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imap_sort(int stream_id, int criteria, int reverse [, int options [, string search_criteria]])
   Sort an array of message headers, optionally including only messages that meet specified criteria. */
PHP_FUNCTION(imap_sort)
{
	zval **streamind, **pgm, **rev, **flags, **criteria;
	pils *imap_le_struct;
	unsigned long *slst, *sl;
	char *search_criteria;
	SORTPGM *mypgm=NIL;
	SEARCHPGM *spg=NIL;
	int myargc = ZEND_NUM_ARGS();
	
	if (myargc < 3 || myargc > 5 || zend_get_parameters_ex(myargc, &streamind, &pgm, &rev, &flags, &criteria) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_long_ex(rev);
	convert_to_long_ex(pgm);
	if (Z_LVAL_PP(pgm) > SORTSIZE) {
		php_error(E_WARNING, "%s(): Unrecognized sort criteria", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	if (myargc >= 4) {
		convert_to_long_ex(flags);
	}
	if (myargc == 5) {
		search_criteria = estrndup(Z_STRVAL_PP(criteria), Z_STRLEN_PP(criteria));
		spg = mail_criteria(search_criteria);
		efree(search_criteria);
	} else {
		spg = mail_newsearchpgm();
	}
	
	mypgm = mail_newsortpgm();
	mypgm->reverse = Z_LVAL_PP(rev);
	mypgm->function = (short) Z_LVAL_PP(pgm);
	mypgm->next = NIL;
	
	slst = mail_sort(imap_le_struct->imap_stream, NIL, spg, mypgm, myargc >= 4 ? Z_LVAL_PP(flags) : NIL);

	if (spg) {
		mail_free_searchpgm(&spg);
	}

	array_init(return_value);
	if (slst != NIL && slst != 0) {
		for (sl = slst; *sl; sl++) { 
			add_next_index_long(return_value, *sl);
		}
		fs_give ((void **) &slst);
	}
}
/* }}} */

/* {{{ proto string imap_fetchheader(int stream_id, int msg_no [, int options])
   Get the full unfiltered header for a message */
PHP_FUNCTION(imap_fetchheader)
{
	zval **streamind, **msgno, **flags;
	pils *imap_le_struct;
	int msgindex, myargc = ZEND_NUM_ARGS();
	
	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &msgno, &flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_long_ex(msgno);
	if (myargc == 3) {
		convert_to_long_ex(flags);
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
		php_error(E_WARNING, "%s(): Bad message number", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	RETVAL_STRING(mail_fetchheader_full(imap_le_struct->imap_stream, Z_LVAL_PP(msgno), NIL, NIL, (myargc == 3 ? Z_LVAL_PP(flags) : NIL)), 1);
}
/* }}} */

/* {{{ proto int imap_uid(int stream_id, int msg_no)
   Get the unique message id associated with a standard sequential message number */
PHP_FUNCTION(imap_uid)
{
 	zval **streamind, **msgno;
	pils *imap_le_struct;
 	int msgindex;
 
 	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &msgno) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
 	
 	convert_to_long_ex(msgno);
 
	msgindex = Z_LVAL_PP(msgno);
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error(E_WARNING, "%s(): Bad message number", get_active_function_name(TSRMLS_C));
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
	pils *imap_le_struct;
	 
 	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &streamind, &msgno) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
 	
 	convert_to_long_ex(msgno);
 
 	RETURN_LONG(mail_msgno(imap_le_struct->imap_stream, Z_LVAL_PP(msgno)));
}
/* }}} */

/* {{{ proto object imap_status(int stream_id, string mailbox, int options)
   Get status info from a mailbox */
PHP_FUNCTION(imap_status)
{
 	zval **streamind, **mbx, **flags;
	pils *imap_le_struct;

 	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &mbx, &flags) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

 	convert_to_string_ex(mbx);
	convert_to_long_ex(flags);

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
	pils *imap_le_struct;
	zval *parametres, *param, *dparametres, *dparam;
	PARAMETER *par, *dpar;
	BODY *body;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &streamind, &msg, &section) == FAILURE) {
 		ZEND_WRONG_PARAM_COUNT();
 	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
 	
 	convert_to_long_ex(msg);
	convert_to_string_ex(section);

	if (!Z_LVAL_PP(msg) || Z_LVAL_PP(msg) < 1 || (unsigned) Z_LVAL_PP(msg) > imap_le_struct->imap_stream->nmsgs) {
		php_error(E_WARNING, "%s(): Bad message number", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	if (object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	body=mail_body(imap_le_struct->imap_stream, Z_LVAL_PP(msg), Z_STRVAL_PP(section));
	if (body->type <= TYPEMAX) {
		add_property_long(return_value, "type", body->type);
	}
	if (body->encoding <= ENCMAX) {
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

/* {{{ proto array imap_fetch_overview(int stream_id, int msg_no [, int flags])
   Read an overview of the information in the headers of the given message sequence */ 
PHP_FUNCTION(imap_fetch_overview)
{
 	zval **streamind, **sequence, **pflags;
	pils *imap_le_struct;
	zval *myoverview;
	char address[MAILTMPLEN];
	long status, flags=0L;
	int myargc = ZEND_NUM_ARGS();
	
	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &sequence, &pflags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

 	convert_to_string_ex(sequence);
	if(myargc == 3) {
		convert_to_long_ex(pflags);
		flags = Z_LVAL_PP(pflags);
	}

	array_init(return_value);
	
	status = (flags & FT_UID) 
		? mail_uid_sequence (imap_le_struct->imap_stream, Z_STRVAL_PP(sequence))
		: mail_sequence (imap_le_struct->imap_stream, Z_STRVAL_PP(sequence));
	
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
				if (env->to) {
					env->to->next = NULL;
					address[0] = '\0';
					rfc822_write_address(address, env->to);
					add_property_string(myoverview, "to", address, 1);
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
				if (env->in_reply_to) {
					add_property_string(myoverview, "in_reply_to", env->in_reply_to, 1);
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
	zval **data, **pvalue, **disp_data, **env_data;
	ulong ind;
	char *cookie = NIL;
	ENVELOPE *env;
	BODY *bod=NULL, *topbod=NULL;
	PART *mypart=NULL, *toppart=NULL, *part;
	PARAMETER *param, *disp_param = NULL, *custom_headers_param = NULL, *tmp_param = NULL;
	char tmp[8 * MAILTMPLEN], *mystring=NULL, *t=NULL, *tempstring=NULL;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &envelope, &body) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(envelope) != IS_ARRAY) {
		php_error(E_WARNING, "%s(): Expected Array as envelope parameter", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
 	}

	if (Z_TYPE_PP(body) != IS_ARRAY) {
		php_error(E_WARNING, "%s(): Expected Array as body parameter", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
 	}

	env = mail_newenvelope();
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
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "in_reply_to", sizeof("in_reply_to"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		env->in_reply_to=cpystr(Z_STRVAL_PP(pvalue));
	}
	if (zend_hash_find(Z_ARRVAL_PP(envelope), "subject", sizeof("subject"), (void **) &pvalue)== SUCCESS) {
		convert_to_string_ex(pvalue);
		env->subject=cpystr(Z_STRVAL_PP(pvalue));
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

	if (zend_hash_find(Z_ARRVAL_PP(envelope), "custom_headers", sizeof("custom_headers"), (void **) &pvalue)== SUCCESS) {
		if (Z_TYPE_PP(pvalue) == IS_ARRAY) {
			custom_headers_param = tmp_param = NULL;
			while (zend_hash_get_current_data(Z_ARRVAL_PP(pvalue), (void **) &env_data) == SUCCESS) {
				custom_headers_param = mail_newbody_parameter();
				convert_to_string_ex(env_data);
				custom_headers_param->value = (char *) fs_get(Z_STRLEN_PP(env_data) + 1);
				memcpy(custom_headers_param->value, Z_STRVAL_PP(env_data), Z_STRLEN_PP(env_data) + 1);
				zend_hash_move_forward(Z_ARRVAL_PP(pvalue));
				custom_headers_param->next = tmp_param;
				tmp_param = custom_headers_param;
			}
		}
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(body));
	zend_hash_get_current_data(Z_ARRVAL_PP(body), (void **) &data);
	zend_hash_get_current_key(Z_ARRVAL_PP(body), &key, &ind, 0); /* FIXME: is this necessary?  we're not using key/ind */

	if (Z_TYPE_PP(data) == IS_ARRAY) {
		bod = mail_newbody();
		topbod = bod;

		if (zend_hash_find(Z_ARRVAL_PP(data), "type", sizeof("type"), (void **) &pvalue)== SUCCESS) {
			convert_to_long_ex(pvalue);
			bod->type = (short) Z_LVAL_PP(pvalue);
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "encoding", sizeof("encoding"), (void **) &pvalue)== SUCCESS) {
			convert_to_long_ex(pvalue);
			bod->encoding = (short) Z_LVAL_PP(pvalue);
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "charset", sizeof("charset"), (void **) &pvalue)== SUCCESS) {
			convert_to_string_ex(pvalue);
			tmp_param = mail_newbody_parameter();
			tmp_param->value = cpystr(Z_STRVAL_PP(pvalue));
			tmp_param->attribute = "CHARSET";
			tmp_param->next = bod->parameter;
			bod->parameter = tmp_param;
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "subtype", sizeof("subtype"), (void **) &pvalue)== SUCCESS) {
			convert_to_string_ex(pvalue);
			bod->subtype = cpystr(Z_STRVAL_PP(pvalue));
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "id", sizeof("id"), (void **) &pvalue)== SUCCESS) {
			convert_to_string_ex(pvalue);
			bod->id = cpystr(Z_STRVAL_PP(pvalue));
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "description", sizeof("description"), (void **) &pvalue)== SUCCESS) {
			convert_to_string_ex(pvalue);
			bod->description = cpystr(Z_STRVAL_PP(pvalue));
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "disposition.type", sizeof("disposition.type"), (void **) &pvalue)== SUCCESS) {
			convert_to_string_ex(pvalue);
			bod->disposition.type = (char *) fs_get(Z_STRLEN_PP(pvalue) + 1);
			memcpy(bod->disposition.type, Z_STRVAL_PP(pvalue), Z_STRLEN_PP(pvalue)+1);
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "disposition", sizeof("disposition"), (void **) &pvalue)== SUCCESS) {
			if (Z_TYPE_PP(pvalue) == IS_ARRAY) {
				disp_param = tmp_param = NULL;
				while (zend_hash_get_current_data(Z_ARRVAL_PP(pvalue), (void **) &disp_data) == SUCCESS) {
					disp_param = mail_newbody_parameter();
					zend_hash_get_current_key(Z_ARRVAL_PP(pvalue), &key, &ind, 0);
					disp_param->attribute = key;
					convert_to_string_ex(disp_data);
					disp_param->value = (char *) fs_get(Z_STRLEN_PP(disp_data) + 1);
					memcpy(disp_param->value, Z_STRVAL_PP(disp_data), Z_STRLEN_PP(disp_data) + 1);
					zend_hash_move_forward(Z_ARRVAL_PP(pvalue));
					disp_param->next = tmp_param;
					tmp_param = disp_param;
				}
				bod->disposition.parameter = disp_param;
			}
		}
		if (zend_hash_find(Z_ARRVAL_PP(data), "contents.data", sizeof("contents.data"), (void **) &pvalue)== SUCCESS) {
			convert_to_string_ex(pvalue);
			bod->contents.text.data = (char *) fs_get(Z_STRLEN_PP(pvalue) + 1);
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

	while (zend_hash_get_current_data(Z_ARRVAL_PP(body), (void **) &data) == SUCCESS) {
		zend_hash_get_current_key(Z_ARRVAL_PP(body), &key, &ind, 0);  /* FIXME: Is this necessary?  We're not using key/ind */
		if (Z_TYPE_PP(data) == IS_ARRAY) {
			if (!toppart) {
				bod->nested.part = mail_newbody_part();
				mypart = bod->nested.part;
				toppart = mypart;
				bod=&mypart->body;
			} else {
				 mypart->next = mail_newbody_part();
				 mypart = mypart->next;
				 bod = &mypart->body;
			}

			if (zend_hash_find(Z_ARRVAL_PP(data), "type", sizeof("type"), (void **) &pvalue)== SUCCESS) {
				convert_to_long_ex(pvalue);
				bod->type = (short) Z_LVAL_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "encoding", sizeof("encoding"), (void **) &pvalue)== SUCCESS) {
				convert_to_long_ex(pvalue);
				bod->encoding = (short) Z_LVAL_PP(pvalue);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "charset", sizeof("charset"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				tmp_param = mail_newbody_parameter();
				tmp_param->value = cpystr(Z_STRVAL_PP(pvalue));
				tmp_param->attribute = "CHARSET";
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "subtype", sizeof("subtype"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->subtype = cpystr(Z_STRVAL_PP(pvalue));	
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "id", sizeof("id"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->id = cpystr(Z_STRVAL_PP(pvalue));
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "description", sizeof("description"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->description = cpystr(Z_STRVAL_PP(pvalue));
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "disposition.type", sizeof("disposition.type"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->disposition.type = (char *) fs_get(Z_STRLEN_PP(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_PP(pvalue), Z_STRLEN_PP(pvalue)+1);
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "disposition", sizeof("disposition"), (void **) &pvalue)== SUCCESS) {
				if (Z_TYPE_PP(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					while (zend_hash_get_current_data(Z_ARRVAL_PP(pvalue), (void **) &disp_data) == SUCCESS) {
						disp_param = mail_newbody_parameter();
						zend_hash_get_current_key(Z_ARRVAL_PP(pvalue), &key, &ind, 0);
						disp_param->attribute = key;
						convert_to_string_ex(disp_data);
						disp_param->value = (char *) fs_get(Z_STRLEN_PP(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_PP(disp_data), Z_STRLEN_PP(disp_data) + 1);
						zend_hash_move_forward(Z_ARRVAL_PP(pvalue));
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					}
					bod->disposition.parameter = disp_param;
				}
			}
			if (zend_hash_find(Z_ARRVAL_PP(data), "contents.data", sizeof("contents.data"), (void **) &pvalue)== SUCCESS) {
				convert_to_string_ex(pvalue);
				bod->contents.text.data = (char *) fs_get(Z_STRLEN_PP(pvalue) + 1);
				memcpy(bod->contents.text.data, Z_STRVAL_PP(pvalue), Z_STRLEN_PP(pvalue) + 1);
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

			zend_hash_move_forward(Z_ARRVAL_PP(body));
		}
	}

	rfc822_encode_body_7bit(env, topbod); 
	rfc822_header (tmp, env, topbod);

	/* add custom envelope headers */
	if (custom_headers_param) {
		/* remove last CRLF from tmp */
		tmp[strlen(tmp) - 2] = '\0';
		tempstring = emalloc(strlen(tmp) + 1);
		strcpy(tempstring, tmp);
		do {
			tempstring = erealloc(tempstring, strlen(tempstring) + strlen(custom_headers_param->value) + strlen(CRLF) + 1);
			sprintf(tempstring, "%s%s%s", tempstring, custom_headers_param->value, CRLF);
		} while ((custom_headers_param = custom_headers_param->next));

		mystring = emalloc(strlen(tempstring) + strlen(CRLF) + 1);
		sprintf(mystring, "%s%s", tempstring, CRLF);
		efree(tempstring);
	} else {
		mystring = emalloc(strlen(tmp) + 1);
		strcpy(mystring, tmp);
	}

	bod = topbod;

	if (bod && bod->type == TYPEMULTIPART) {	

		/* first body part */
			part = bod->nested.part;	

		/* find cookie */
			for (param = bod->parameter; param && !cookie; param = param->next) {
				if (!strcmp (param->attribute, "BOUNDARY")) {
					cookie = param->value;
				}
			}

		/* yucky default */
			if (!cookie) {
				cookie = "-";  
			}

		/* for each part */
			do {
			/* build cookie */
				sprintf (t=tmp, "--%s%s", cookie, CRLF);

			/* append mini-header */
				rfc822_write_body_header(&t, &part->body);

			/* write terminating blank line */
				strcat (t, CRLF);

			/* output cookie, mini-header, and contents */
				tempstring=emalloc(strlen(mystring)+strlen(tmp)+1);
				strcpy(tempstring, mystring);
				efree(mystring);
				mystring=tempstring;
				strcat(mystring, tmp);

				bod=&part->body;

				tempstring=emalloc(strlen(bod->contents.text.data)+strlen(CRLF)+strlen(mystring)+1);
				strcpy(tempstring, mystring);
				efree(mystring);
				mystring=tempstring;
				sprintf(mystring, "%s%s%s", mystring, bod->contents.text.data, CRLF);

			} while ((part = part->next)); /* until done */

			/* output trailing cookie */
			sprintf(tmp, "--%s--", cookie);
			tempstring=emalloc(strlen(tmp)+strlen(CRLF)+strlen(mystring)+1);
			strcpy(tempstring, mystring);
			efree(mystring);
			mystring=tempstring;
			sprintf(mystring, "%s%s%s", mystring, tmp, CRLF);

	} else if (bod) {

			tempstring=emalloc(strlen(bod->contents.text.data)+strlen(CRLF)+strlen(mystring)+1);
			strcpy(tempstring, mystring);
			efree(mystring);
			mystring=tempstring;
			sprintf(mystring, "%s%s%s", mystring, bod->contents.text.data, CRLF);

	} else {
		efree(mystring);
		RETURN_FALSE;
	}

	RETVAL_STRINGL(mystring, strlen(mystring), 1);  
	efree(tempstring);
}
/* }}} */


/* {{{ _php_imap_mail
 */
int _php_imap_mail(char *to, char *subject, char *message, char *headers, char *cc, char *bcc, char* rpath TSRMLS_DC)
{
#ifdef PHP_WIN32
	int tsm_err;
#else
	FILE *sendmail;
	int ret;
#endif

#ifdef PHP_WIN32
	char *tempMailTo;
	char *tsm_errmsg = NULL;
	ADDRESS *addr;
	char *bufferTo = NULL, *bufferCc = NULL, *bufferBcc = NULL;
	int offset;

	if (to && *to) {
		tempMailTo = estrdup(to);
		bufferTo = (char *)emalloc(strlen(to));
		offset = 0;
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, NULL);
		while (addr) {
			if (strcmp(addr->host, ERRHOST) == 0)
				return (BAD_MSG_DESTINATION);
			else {
				offset += sprintf(bufferTo + offset, "%s@%s,", addr->mailbox, addr->host);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
		bufferTo[offset] = 0;
	}

	if (cc && *cc) {
		tempMailTo = estrdup(cc);
		bufferCc = (char *)emalloc(strlen(cc));
		offset = 0;
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, NULL);
		while (addr) {
			if (strcmp(addr->host, ERRHOST) == 0)
				return (BAD_MSG_DESTINATION);
			else {
				offset += sprintf(bufferCc + offset, "%s@%s,", addr->mailbox, addr->host);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
		bufferCc[offset] = 0;
	}

	if (bcc && *bcc) {
		tempMailTo = estrdup(bcc);
		bufferBcc = (char *)emalloc(strlen(bcc));
		offset = 0;
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, NULL);
		while (addr) {
			if (strcmp(addr->host, ERRHOST) == 0)
				return (BAD_MSG_DESTINATION);
			else {
				offset += sprintf(bufferBcc + offset, "%s@%s,", addr->mailbox, addr->host);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
		bufferBcc[offset] = 0;
	}


	if (TSendMail(INI_STR("SMTP"), &tsm_err, &tsm_errmsg, headers, subject, bufferTo, message, bufferCc, bufferBcc, rpath) != SUCCESS) {
		if (tsm_errmsg) {
			php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), tsm_errmsg);
			efree(tsm_errmsg);
		} else {
			php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), GetSMErrorText(tsm_err));
		}
		return 0;
	}
	if (bufferCc) efree(bufferCc);
	if (bufferBcc) efree(bufferBcc);
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
		php_error(E_WARNING, "%s(): Could not execute mail delivery program", get_active_function_name(TSRMLS_C));
		return 0;
	}
#endif
	return 1;
}
/* }}} */

/* {{{ proto int imap_mail(string to, string subject, string message [, string additional_headers [, string cc [, string bcc [, string rpath]]]])
   Send an email message */
PHP_FUNCTION(imap_mail)
{
	zval **argv[7];
	char *to=NULL, *message=NULL, *headers=NULL, *subject=NULL, *cc=NULL, *bcc=NULL, *rpath=NULL;
	int argc = ZEND_NUM_ARGS();

	if (argc < 3 || argc > 7 || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	/* To: */
	convert_to_string_ex(argv[0]);
	if (Z_STRVAL_PP(argv[0])) {
		to = Z_STRVAL_PP(argv[0]);
	} else {
		php_error(E_WARNING, "%s(): No to field in mail command", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* Subject: */
	convert_to_string_ex(argv[1]);
	if (Z_STRVAL_PP(argv[1])) {
		subject = Z_STRVAL_PP(argv[1]);
	} else {
		php_error(E_WARNING, "%s(): No subject field in mail command", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* message body */
	convert_to_string_ex(argv[2]);
	if (Z_STRVAL_PP(argv[2])) {
		message = Z_STRVAL_PP(argv[2]);
	} else {
		/* this is not really an error, so it is allowed. */
		php_error(E_WARNING, "%s(): No message string in mail command", get_active_function_name(TSRMLS_C));
		message = NULL;
	}

	/* other headers */
	if (argc > 3) {
		convert_to_string_ex(argv[3]);
		headers = Z_STRVAL_PP(argv[3]);
	}
	
	/* cc */
	if (argc > 4) {
		convert_to_string_ex(argv[4]);
		cc = Z_STRVAL_PP(argv[4]);
	}

	/* bcc */
	if (argc > 5) {
		convert_to_string_ex(argv[5]);
		bcc = Z_STRVAL_PP(argv[5]);
	}

	/* rpath */
	if (argc > 6) {
		convert_to_string_ex(argv[6]);
		rpath = Z_STRVAL_PP(argv[6]);
	}

	if (_php_imap_mail(to, subject, message, headers, cc, bcc, rpath TSRMLS_CC)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imap_search(int stream_id, string criteria [, long flags])
   Return a list of messages matching the given criteria */
PHP_FUNCTION(imap_search)
{
	zval **streamind, **criteria, **search_flags;
	pils *imap_le_struct;
	long flags;
	char *search_criteria;
	MESSAGELIST *cur;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &streamind, &criteria, &search_flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);

	convert_to_string_ex(criteria);
	search_criteria = estrndup(Z_STRVAL_PP(criteria), Z_STRLEN_PP(criteria));
	
	if (argc == 2) {
		flags = SE_FREE;
	} else {
		convert_to_long_ex(search_flags);
		flags = Z_LVAL_PP(search_flags);
	}
	
	IMAPG(imap_messages) = IMAPG(imap_messages_tail) = NIL;
	mail_search_full(imap_le_struct->imap_stream, NIL, mail_criteria(search_criteria), flags);
	if (IMAPG(imap_messages) == NIL) {
		efree(search_criteria);
		RETURN_FALSE;
	}
	
	array_init(return_value);

	cur = IMAPG(imap_messages);
	while (cur != NIL) {
		add_next_index_long(return_value, cur->msgid);
		cur = cur->next;
	}
	mail_free_messagelist(&IMAPG(imap_messages), &IMAPG(imap_messages_tail));
	efree(search_criteria);
}
/* }}} */

/* {{{ proto array imap_alerts(void)
   Returns an array of all IMAP alerts that have been generated since the last page load or since the last imap_alerts() call, whichever came last. The alert stack is cleared after imap_alerts() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_alerts)
{
	STRINGLIST *cur=NIL;

	if (ZEND_NUM_ARGS() > 0) {
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
   Returns an array of all IMAP errors generated since the last page load, or since the last imap_errors() call, whichever came last. The error stack is cleared after imap_errors() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_errors)
{
	ERRORLIST *cur=NIL;

	if (ZEND_NUM_ARGS() > 0) {
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

	if (ZEND_NUM_ARGS() > 0) {
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

	string = Z_STRVAL_PP(str);
	end = Z_STRLEN_PP(str);
			
	if ((charset = ((char *)emalloc((end + 1) * 2)))) {
		text = &charset[end + 1];
		while (offset < end) {	/* Reached end of the string? */
			if ((charset_token = (long)php_memnstr(&string[offset], "=?", 2, string + end))) {	/* Is there anything encoded in the string? */
				charset_token -= (long)string;
				if (offset != charset_token) {	/* Is there anything before the encoded data? */
					/* Retrieve unencoded data that is found before encoded data */
					memcpy(text, &string[offset], charset_token-offset);
					text[charset_token - offset] = 0x00;
					MAKE_STD_ZVAL(myobject);
					object_init(myobject);
					add_property_string(myobject, "charset", "default", 1);
					add_property_string(myobject, "text", text, 1);
					zend_hash_next_index_insert(Z_ARRVAL_P(return_value), (void *)&myobject, sizeof(zval *), NULL);
				}
				if ((encoding_token = (long)php_memnstr(&string[charset_token+2], "?", 1, string+end))) {		/* Find token for encoding */
					encoding_token -= (long)string;
					if ((end_token = (long)php_memnstr(&string[encoding_token+3], "?=", 2, string+end))) {	/* Find token for end of encoded data */
						end_token -= (long)string;
						memcpy(charset, &string[charset_token + 2], encoding_token - (charset_token + 2));	/* Extract charset encoding */
						charset[encoding_token-(charset_token + 2)] = 0x00;
						encoding=string[encoding_token + 1];	/* Extract encoding from string */
						memcpy(text, &string[encoding_token + 3], end_token - (encoding_token + 3));	/* Extract text */
						text[end_token - (encoding_token + 3)] = 0x00;
						decode = text;
						if (encoding == 'q' || encoding == 'Q') {	/* Decode 'q' encoded data */
							for(i=0; text[i] != 0x00; i++) if (text[i] == '_') text[i] = ' ';	/* Replace all *_' with space. */
							decode = (char *)rfc822_qprint((unsigned char *) text, strlen(text), &newlength);
						} else if (encoding == 'b' || encoding == 'B') {
							decode = (char *)rfc822_base64((unsigned char *) text, strlen(text), &newlength); /* Decode 'B' encoded data */
						}
						if (decode == NULL) {
							efree(charset);
							zval_dtor(return_value);
							RETURN_FALSE;
						}
						MAKE_STD_ZVAL(myobject);
						object_init(myobject);
						add_property_string(myobject, "charset", charset, 1);
						add_property_string(myobject, "text", decode, 1);
						zend_hash_next_index_insert(Z_ARRVAL_P(return_value), (void *)&myobject, sizeof(zval *), NULL);
						fs_give((void**)&decode);
						
						offset = end_token+2;
						for (i = 0; (string[offset + i] == ' ') || (string[offset + i] == 0x0a) || (string[offset + i] == 0x0d); i++);
						if ((string[offset + i] == '=') && (string[offset + i + 1] == '?') && (offset + i < end)) {
							offset += i;
						}
						continue;	/*/ Iterate the loop again please. */
					}
				}
			} else {
				/* Just some tweaking to optimize the code, and get the end statements work in a general manner.
				   If we end up here we didn't find a position for "charset_token",
				   so we need to set it to the start of the yet unextracted data. */
				charset_token = offset;
			}
			/* Return the rest of the data as unencoded, as it was either unencoded or was missing separators
			   which rendered the the remainder of the string impossible for us to decode. */
			memcpy(text, &string[charset_token], end - charset_token);	/* Extract unencoded text from string */
			text[end - charset_token] = 0x00;
			MAKE_STD_ZVAL(myobject);
			object_init(myobject);
			add_property_string(myobject, "charset", "default", 1);
			add_property_string(myobject, "text", text, 1);
			zend_hash_next_index_insert(Z_ARRVAL_P(return_value), (void *)&myobject, sizeof(zval *), NULL);
			
			offset = end;	/* We have reached the end of the string. */
		}
		efree(charset);
	} else {
		php_error(E_WARNING, "%s(): Unable to allocate temporary memory buffer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
}
/* }}} */


/* Support Functions */
/* {{{ _php_imap_get_address_size
 */
static int _php_imap_address_size (ADDRESS *addresslist)
{
	ADDRESS *tmp;
	int ret=0;

	tmp = addresslist;

	if (tmp) do {
		ret += (tmp->personal) ? strlen(tmp->personal) : 0;
		ret += (tmp->adl)      ? strlen(tmp->adl)      : 0;
		ret += (tmp->mailbox)  ? strlen(tmp->mailbox)  : 0;
		ret += (tmp->host)     ? strlen(tmp->host)     : 0;
	} while ((tmp = tmp->next));

	/* rfc822_write_address_full() needs some extra space for '<>,', etc. */
	ret += (ret) ? MAILTMPLEN : 0;

	return ret;
}

/* }}} */


/* {{{ _php_imap_parse_address
 */
static void _php_imap_parse_address (ADDRESS *addresslist, char **fulladdress, zval *paddress TSRMLS_DC)
{
	ADDRESS *addresstmp;
	zval *tmpvals;
	char *tmpstr;
	int len=0;
		
	addresstmp = addresslist;

	if ((len = _php_imap_address_size(addresstmp))) {
		tmpstr = (char *) malloc (len);
		tmpstr[0] = '\0';
		rfc822_write_address(tmpstr, addresstmp);
		*fulladdress = tmpstr;
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
/* }}} */

/* {{{ _php_make_header_object
 */
static void _php_make_header_object(zval *myzvalue, ENVELOPE *en TSRMLS_DC)
{
	zval *paddress;
	char *fulladdress=NULL;
	
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
		_php_imap_parse_address(en->to, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "toaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "to", paddress);
	}
	
	if (en->from) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->from, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "fromaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "from", paddress);
	}
	
	if (en->cc) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->cc, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "ccaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "cc", paddress);
	}
	
	if (en->bcc) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->bcc, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "bccaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "bcc", paddress);
	}
	
	if (en->reply_to) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->reply_to, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "reply_toaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "reply_to", paddress);
	}

	if (en->sender) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->sender, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "senderaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "sender", paddress);
	}

	if (en->return_path) {
		MAKE_STD_ZVAL(paddress);
		array_init(paddress);
		_php_imap_parse_address(en->return_path, &fulladdress, paddress TSRMLS_CC);
		if (fulladdress) {
			add_property_string(myzvalue, "return_pathaddress", fulladdress, 1);
			free(fulladdress);
		}
		add_assoc_object(myzvalue, "return_path", paddress);
	}
}
/* }}} */

/* {{{ _php_imap_add_body
 */
void _php_imap_add_body(zval *arg, BODY *body TSRMLS_DC)
{
	zval *parametres, *param, *dparametres, *dparam;
	PARAMETER *par, *dpar;
	PART *part;
	
	if (body->type <= TYPEMAX) {
		add_property_long(arg, "type", body->type);
	}

	if (body->encoding <= ENCMAX) {
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
			_php_imap_add_body(param, &part->body TSRMLS_CC);
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
		_php_imap_add_body(param, body TSRMLS_CC);
		add_next_index_object(parametres, param);
		add_assoc_object(arg, "parts", parametres);
	}
}
/* }}} */


/* imap_thread, stealing this from header cclient -rjs3 */
/* {{{ build_thread_tree_helper
 */
static void build_thread_tree_helper(THREADNODE *cur, zval *tree, long *numNodes, char *buf)
{
	unsigned long thisNode = *numNodes;

	/* define "#.num" */
	snprintf(buf, 25, "%ld.num", thisNode);

	add_assoc_long(tree, buf, cur->num);

	snprintf(buf, 25, "%ld.next", thisNode);
	if(cur->next) {
		(*numNodes)++;
		add_assoc_long(tree, buf, *numNodes);
		build_thread_tree_helper(cur->next, tree, numNodes, buf);
	} else { /* "null pointer" */
		add_assoc_long(tree, buf, 0);
	}

	snprintf(buf, 25, "%ld.branch", thisNode);
	if(cur->branch) {
		(*numNodes)++;
		add_assoc_long(tree, buf, *numNodes);
		build_thread_tree_helper(cur->branch, tree, numNodes, buf);	    
	} else { /* "null pointer" */
		add_assoc_long(tree, buf, 0);
	}
}
/* }}} */

/* {{{ build_thread_tree 
 */
static int build_thread_tree(THREADNODE *top, zval **tree)
{
	long numNodes = 0;
	char buf[25];

	if(array_init(*tree) != SUCCESS) return FAILURE;
	
	build_thread_tree_helper(top, *tree, &numNodes, buf);

	return SUCCESS;
}
/* }}} */

/* {{{ proto int imap_thread(int stream_id [, int flags])
   Return threaded by REFERENCES tree */
PHP_FUNCTION (imap_thread)
{
	zval **streamind, **search_flags;
	pils *imap_le_struct;
	long flags;
	char criteria[] = "ALL";
	THREADNODE *top;
	int argc = ZEND_NUM_ARGS();

	if ( argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &streamind, &search_flags) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, streamind, -1, "imap", le_imap);
	
	if (argc == 1) {
		flags = SE_FREE;
	} else {
		convert_to_long_ex(search_flags);
		flags = Z_LVAL_PP(search_flags);
	}
	
	top = mail_thread(imap_le_struct->imap_stream, "REFERENCES", NIL, mail_criteria(criteria), flags);

	if(top == NIL) {
		php_error(E_WARNING, "%s(): Function returned an empty tree", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* Populate our return value data structure here. */
	if(build_thread_tree(top, &return_value) == FAILURE) {
		mail_free_threadnode(&top);
		RETURN_FALSE;
	}
	mail_free_threadnode(&top);
}
/* }}} */


/* {{{ Interfaces to C-client 
 */
void mm_searched(MAILSTREAM *stream, unsigned long number)
{
	MESSAGELIST *cur = NIL;
	TSRMLS_FETCH();

	if (IMAPG(imap_messages) == NIL) {
		IMAPG(imap_messages) = mail_newmessagelist();
		IMAPG(imap_messages)->msgid = number;
		IMAPG(imap_messages)->next = NIL;
		IMAPG(imap_messages_tail) = IMAPG(imap_messages);
	} else {
		cur = IMAPG(imap_messages_tail);
		cur->next = mail_newmessagelist();
		cur = cur->next;
		cur->msgid = number;
		cur->next = NIL;
		IMAPG(imap_messages_tail) = cur;
	}
}

void mm_exists(MAILSTREAM *stream, unsigned long number)
{
}

void mm_expunged(MAILSTREAM *stream, unsigned long number)
{
}

void mm_flags(MAILSTREAM *stream, unsigned long number)
{
}

/* Author: CJH */
void mm_notify(MAILSTREAM *stream, char *str, long errflg)
{
	STRINGLIST *cur = NIL;
	TSRMLS_FETCH();
  
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

void mm_list(MAILSTREAM *stream, DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;
	TSRMLS_FETCH();
	
	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build up a the new array of objects */
		/* Author: CJH */
		if (IMAPG(imap_folder_objects) == NIL) {
			IMAPG(imap_folder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_folder_objects)->LSIZE=strlen(IMAPG(imap_folder_objects)->LTEXT=cpystr(mailbox));
			IMAPG(imap_folder_objects)->delimiter = delimiter;
			IMAPG(imap_folder_objects)->attributes = attributes;
			IMAPG(imap_folder_objects)->next = NIL;
			IMAPG(imap_folder_objects_tail) = IMAPG(imap_folder_objects);
		} else {
			ocur=IMAPG(imap_folder_objects_tail);
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE = strlen(ocur->LTEXT = cpystr(mailbox));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
			IMAPG(imap_folder_objects_tail) = ocur;
		}
		
	} else {
		/* build the old IMAPG(imap_folders) variable to allow old imap_listmailbox() to work */
		if (!(attributes & LATT_NOSELECT)) {
			if (IMAPG(imap_folders) == NIL) {
				IMAPG(imap_folders)=mail_newstringlist();
				IMAPG(imap_folders)->LSIZE=strlen(IMAPG(imap_folders)->LTEXT=cpystr(mailbox));
				IMAPG(imap_folders)->next=NIL; 
				IMAPG(imap_folders_tail) = IMAPG(imap_folders);
			} else {
				cur=IMAPG(imap_folders_tail);
				cur->next=mail_newstringlist ();
				cur=cur->next;
				cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
				cur->next = NIL;
				IMAPG(imap_folders_tail) = cur;
			}
		}
	}
}

void mm_lsub(MAILSTREAM *stream, DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;
	TSRMLS_FETCH();
	
	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build the array of objects */
		/* Author: CJH */
		if (IMAPG(imap_sfolder_objects) == NIL) {
			IMAPG(imap_sfolder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_sfolder_objects)->LSIZE=strlen(IMAPG(imap_sfolder_objects)->LTEXT=cpystr(mailbox));
			IMAPG(imap_sfolder_objects)->delimiter = delimiter;
			IMAPG(imap_sfolder_objects)->attributes = attributes;
			IMAPG(imap_sfolder_objects)->next = NIL;
			IMAPG(imap_sfolder_objects_tail) = IMAPG(imap_sfolder_objects);
		} else {
			ocur=IMAPG(imap_sfolder_objects_tail);
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE=strlen(ocur->LTEXT = cpystr(mailbox));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
			IMAPG(imap_sfolder_objects_tail) = ocur;
		}
	} else {
		/* build the old simple array for imap_listsubscribed() */
		if (IMAPG(imap_sfolders) == NIL) {
			IMAPG(imap_sfolders)=mail_newstringlist();
			IMAPG(imap_sfolders)->LSIZE=strlen(IMAPG(imap_sfolders)->LTEXT=cpystr(mailbox));
			IMAPG(imap_sfolders)->next=NIL; 
			IMAPG(imap_sfolders_tail) = IMAPG(imap_sfolders);
		} else {
			cur=IMAPG(imap_sfolders_tail);
			cur->next=mail_newstringlist ();
			cur=cur->next;
			cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
			cur->next = NIL;
			IMAPG(imap_sfolders_tail) = cur;
		}
	}
}

void mm_status(MAILSTREAM *stream, char *mailbox, MAILSTATUS *status)
{
	TSRMLS_FETCH();

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
}

void mm_log(char *str, long errflg)
{
	ERRORLIST *cur = NIL;
	TSRMLS_FETCH();
  
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
	TSRMLS_FETCH();

	if (*mb->user) {
		strlcpy (user, mb->user, MAILTMPLEN);
	} else {
		strlcpy (user, IMAPG(imap_user), MAILTMPLEN);
	}
	strlcpy (pwd, IMAPG(imap_password), MAILTMPLEN);
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
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
