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
   | Authors: Rex Logan           <veebert@dimensional.com>               |
   |          Mark Musone         <musone@afterfive.com>                  |
   |          Brian Wang          <brian@vividnet.com>                    |
   |          Kaj-Michael Lang    <milang@tal.org>                        |
   |          Antoni Pamies Olive <toni@readysoft.net>                    |
   |          Rasmus Lerdorf      <rasmus@php.net>                        |
   |          Chuck Hagenbuch     <chuck@horde.org>                       |
   |          Andrew Skalski      <askalski@chekinc.com>                  |
   |          Hartmut Holzgraefe  <hholzgra@php.net>                      |
   |          Jani Taskinen       <jani.taskinen@iki.fi>                  |
   |          Daniel R. Kalowsky  <kalowsky@php.net>                      |
   | PHP 4.0 updates:  Zeev Suraski <zeev@php.net>                        |
   +----------------------------------------------------------------------+
 */

#define IMAP41

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_streams.h"
#include "Zend/zend_attributes.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "zend_smart_str.h"
#include "ext/pcre/php_pcre.h"

#ifdef ERROR
#undef ERROR
#endif
#include "php_imap.h"

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#ifdef PHP_WIN32
#include <winsock2.h>
#include <stdlib.h>
#include "win32/sendmail.h"
MAILSTREAM DEFAULTPROTO;
#endif

#define CRLF	"\015\012"
#define CRLF_LEN sizeof("\015\012") - 1
#define PHP_EXPUNGE 32768
#define PHP_IMAP_ADDRESS_SIZE_BUF 10
#ifndef SENDBUFLEN
#define SENDBUFLEN 16385
#endif

#define PHP_IMAP_OPENTIMEOUT 1
#define PHP_IMAP_READTIMEOUT 2
#define PHP_IMAP_WRITETIMEOUT 3
#define PHP_IMAP_CLOSETIMEOUT 4

#define SA_ALL SA_MESSAGES | SA_RECENT | SA_UNSEEN | SA_UIDNEXT | SA_UIDVALIDITY

#include "php_imap_arginfo.h"

#if defined(__GNUC__) && __GNUC__ >= 4
# define PHP_IMAP_EXPORT __attribute__ ((visibility("default")))
#else
# define PHP_IMAP_EXPORT
#endif

static void _php_make_header_object(zval *myzvalue, ENVELOPE *en);
static void _php_imap_add_body(zval *arg, const BODY *body);
static zend_string* _php_imap_parse_address(ADDRESS *addresslist, zval *paddress);
static zend_string* _php_rfc822_write_address(ADDRESS *addresslist);

/* the gets we use */
static char *php_mail_gets(readfn_t f, void *stream, unsigned long size, GETS_DATA *md);

/* These function declarations are missing from the IMAP header files... */
void rfc822_date(char *date);
char *cpystr(const char *str);
char *cpytxt(SIZEDTEXT *dst, char *text, unsigned long size);
#ifndef HAVE_NEW_MIME2TEXT
long utf8_mime2text(SIZEDTEXT *src, SIZEDTEXT *dst);
#else
long utf8_mime2text (SIZEDTEXT *src, SIZEDTEXT *dst, long flags);
#endif
unsigned long find_rightmost_bit(unsigned long *valptr);
void fs_give(void **block);
void *fs_get(size_t size);

ZEND_DECLARE_MODULE_GLOBALS(imap)
static PHP_GINIT_FUNCTION(imap);

/* {{{ imap dependencies */
static const zend_module_dep imap_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_END
};
/* }}} */


/* {{{ PHP_INI */
PHP_INI_BEGIN()
STD_PHP_INI_BOOLEAN("imap.enable_insecure_rsh", "0", PHP_INI_SYSTEM, OnUpdateBool, enable_rsh, zend_imap_globals, imap_globals)
PHP_INI_END()
/* }}} */


/* {{{ imap_module_entry */
zend_module_entry imap_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	imap_deps,
	"imap",
	ext_functions,
	PHP_MINIT(imap),
	NULL,
	PHP_RINIT(imap),
	PHP_RSHUTDOWN(imap),
	PHP_MINFO(imap),
	PHP_IMAP_VERSION,
	PHP_MODULE_GLOBALS(imap),
	PHP_GINIT(imap),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_IMAP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(imap)
#endif

/* Imap class entry definition */
static zend_class_entry *php_imap_ce = NULL;
static zend_object_handlers imap_object_handlers;

typedef struct _php_imap_object {
	MAILSTREAM *imap_stream;
	long flags;
	zend_object std;
} php_imap_object;

static inline zend_object *imap_object_to_zend_object(php_imap_object *obj) {
	return ((zend_object*)(obj + 1)) - 1;
}

static inline php_imap_object *imap_object_from_zend_object(zend_object *zobj) {
	return ((php_imap_object*)(zobj + 1)) - 1;
}

static zend_object* imap_object_create(zend_class_entry* ce) {
	php_imap_object *obj = zend_object_alloc(sizeof(php_imap_object), ce);
	zend_object *zobj = imap_object_to_zend_object(obj);
	obj->imap_stream = NULL;
	obj->flags = 0;
	zend_object_std_init(zobj, ce);
	object_properties_init(zobj, ce);
	zobj->handlers = &imap_object_handlers;

	return zobj;
}

static zend_function *imap_object_get_constructor(zend_object *zobj) {
	zend_throw_error(NULL, "Cannot directly construct IMAP\\Connection, use imap_open() instead");
	return NULL;
}

static void imap_object_destroy(zend_object *zobj) {
	php_imap_object *obj = imap_object_from_zend_object(zobj);

	if (obj->imap_stream) {
		/* Do not try to close prototype streams */
		if (!(obj->flags & OP_PROTOTYPE)) {
			mail_close_full(obj->imap_stream, obj->flags);
		}
	}

	if (IMAPG(imap_user)) {
		efree(IMAPG(imap_user));
		IMAPG(imap_user) = 0;
	}
	if (IMAPG(imap_password)) {
		efree(IMAPG(imap_password));
		IMAPG(imap_password) = 0;
	}

	zend_object_std_dtor(zobj);
}

#define GET_IMAP_STREAM(imap_conn_struct, zval_imap_obj) \
	imap_conn_struct = imap_object_from_zend_object(Z_OBJ_P(zval_imap_obj)); \
	if (imap_conn_struct->imap_stream == NULL) { \
		zend_throw_exception(zend_ce_value_error, "IMAP\\Connection is already closed", 0); \
		RETURN_THROWS(); \
	}

#define PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgindex, arg_pos) \
	if (msgindex < 1) { \
		zend_argument_value_error(arg_pos, "must be greater than 0"); \
		RETURN_THROWS(); \
	} \

#define PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgindex, arg_pos, func_flags, uid_flag) \
	if (func_flags & uid_flag) { \
		/* This should be cached; if it causes an extra RTT to the  IMAP server, */ \
		/* then that's the price we pay for making sure we don't crash. */ \
		unsigned int msg_no_from_uid = mail_msgno(imap_conn_struct->imap_stream, msgindex); \
		if (msg_no_from_uid == 0) { \
			php_error_docref(NULL, E_WARNING, "UID does not exist"); \
			RETURN_FALSE; \
		} \
	} else { \
		if (((unsigned) msgindex) > imap_conn_struct->imap_stream->nmsgs) { \
			php_error_docref(NULL, E_WARNING, "Bad message number"); \
			RETURN_FALSE; \
		} \
	} \

// TODO Promote to ValueError?
#define PHP_IMAP_CHECK_MSGNO(msgindex, arg_pos)	\
	if (msgindex < 1) { \
		zend_argument_value_error(arg_pos, "must be greater than 0"); \
		RETURN_THROWS(); \
	} \
	if (((unsigned) msgindex) > imap_conn_struct->imap_stream->nmsgs) { \
		php_error_docref(NULL, E_WARNING, "Bad message number"); \
		RETURN_FALSE;	\
	}	\

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
	zval t_map, *return_value;

	return_value = *IMAPG(quota_return);

/* put parsing code here */
	for(; qlist; qlist = qlist->next) {
		array_init(&t_map);
		if (strncmp(qlist->name, "STORAGE", 7) == 0)
		{
			/* this is to add backwards compatibility */
			add_assoc_long_ex(return_value, "usage", sizeof("usage") - 1, qlist->usage);
			add_assoc_long_ex(return_value, "limit", sizeof("limit") - 1, qlist->limit);
		}

		add_assoc_long_ex(&t_map, "usage", sizeof("usage") - 1, qlist->usage);
		add_assoc_long_ex(&t_map, "limit", sizeof("limit") - 1, qlist->limit);
		add_assoc_zval_ex(return_value, qlist->name, strlen(qlist->name), &t_map);
	}
}
/* }}} */

/* {{{ mail_getquota
 *
 * Mail GET_ACL callback
 * Called via the mail_parameter function in c-client:src/c-client/mail.c
 */
void mail_getacl(MAILSTREAM *stream, char *mailbox, ACLLIST *alist)
{

	/* walk through the ACLLIST */
	for(; alist; alist = alist->next) {
		add_assoc_stringl(IMAPG(imap_acl_list), alist->identifier, alist->rights, strlen(alist->rights));
	}
}
/* }}} */
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(imap)
{
#if defined(COMPILE_DL_IMAP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
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
	imap_globals->quota_return = NIL;
	imap_globals->imap_acl_list = NIL;
#endif
	imap_globals->gets_stream = NIL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(imap)
{
	REGISTER_INI_ENTRIES();

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
	auth_link(&auth_md5);		/* link in the cram-md5 authenticator */
#if defined(HAVE_IMAP_KRB) && defined(HAVE_IMAP_AUTH_GSS)
	auth_link(&auth_gss);		/* link in the gss authenticator */
#endif
	auth_link(&auth_pla);		/* link in the plain authenticator */
#endif

#ifdef HAVE_IMAP_SSL
	ssl_onceonlyinit ();
#endif

	php_imap_ce = register_class_IMAP_Connection();
	php_imap_ce->create_object = imap_object_create;

	memcpy(&imap_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	imap_object_handlers.offset = XtOffsetOf(php_imap_object, std);
	imap_object_handlers.get_constructor = imap_object_get_constructor;
	imap_object_handlers.free_obj = imap_object_destroy;
	imap_object_handlers.clone_obj = NULL;

	register_php_imap_symbols(module_number);

	/* plug in our gets */
	mail_parameters(NIL, SET_GETS, (void *) NIL);

	/* set default timeout values */
	mail_parameters(NIL, SET_OPENTIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_READTIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_WRITETIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_CLOSETIMEOUT, (void *) FG(default_socket_timeout));

	if (!IMAPG(enable_rsh)) {
		/* disable SSH and RSH, see https://bugs.php.net/bug.php?id=77153 */
		mail_parameters (NIL, SET_RSHTIMEOUT, 0);
		mail_parameters (NIL, SET_SSHTIMEOUT, 0);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(imap)
{
	IMAPG(imap_errorstack) = NIL;
	IMAPG(imap_alertstack) = NIL;
	IMAPG(gets_stream) = NIL;
	return SUCCESS;
}
/* }}} */

static void free_errorlist(void)
{
	ERRORLIST *ecur = NIL;

	if (IMAPG(imap_errorstack) != NIL) {
		/* output any remaining errors at their original error level */
		if (EG(error_reporting) & E_NOTICE) {
			zend_try {
				ecur = IMAPG(imap_errorstack);
				while (ecur != NIL) {
					php_error_docref(NULL, E_NOTICE, "%s (errflg=%ld)", ecur->LTEXT, ecur->errflg);
					ecur = ecur->next;
				}
			} zend_end_try();
		}
		mail_free_errorlist(&IMAPG(imap_errorstack));
		IMAPG(imap_errorstack) = NIL;
	}
}

static void free_stringlist(void)
{
	STRINGLIST *acur = NIL;

	if (IMAPG(imap_alertstack) != NIL) {
		/* output any remaining alerts at E_NOTICE level */
		if (EG(error_reporting) & E_NOTICE) {
			zend_try {
				acur = IMAPG(imap_alertstack);
				while (acur != NIL) {
					php_error_docref(NULL, E_NOTICE, "%s", acur->LTEXT);
					acur = acur->next;
				}
			} zend_end_try();
		}
		mail_free_stringlist(&IMAPG(imap_alertstack));
		IMAPG(imap_alertstack) = NIL;
	}
}

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(imap)
{
	free_errorlist();
	free_stringlist();
	return SUCCESS;
}
/* }}} */

#ifndef CCLIENTVERSION
# if defined(HAVE_IMAP2007e)
#  define CCLIENTVERSION "2007e"
# elif defined(HAVE_IMAP2007d)
#  define CCLIENTVERSION "2007d"
# elif defined(HAVE_IMAP2007b)
#  define CCLIENTVERSION "2007b"
# elif defined(HAVE_IMAP2007a)
#  define CCLIENTVERSION "2007a"
# elif defined(HAVE_IMAP2004)
#  define CCLIENTVERSION "2004"
# elif defined(HAVE_IMAP2001)
#  define CCLIENTVERSION "2001"
# elif defined(HAVE_IMAP2000)
#  define CCLIENTVERSION "2000"
# elif defined(IMAP41)
#  define CCLIENTVERSION "4.1"
# else
#  define CCLIENTVERSION "4.0"
# endif
#endif

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(imap)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "IMAP c-Client Version", CCLIENTVERSION);
#ifdef HAVE_IMAP_SSL
	php_info_print_table_row(2, "SSL Support", "enabled");
#endif
#if defined(HAVE_IMAP_KRB) && defined(HAVE_IMAP_AUTH_GSS)
	php_info_print_table_row(2, "Kerberos Support", "enabled");
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Open an IMAP stream to a mailbox */
PHP_FUNCTION(imap_open)
{
	zend_string *mailbox, *user, *passwd;
	zend_long retries = 0, flags = 0, cl_flags = 0;
	MAILSTREAM *imap_stream;
	HashTable *params = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "PSS|llh", &mailbox, &user, &passwd, &flags, &retries, &params) == FAILURE) {
		RETURN_THROWS();
	}

	/* Check for PHP_EXPUNGE and not CL_EXPUNGE as the user land facing CL_EXPUNGE constant is defined
	 * to something different to prevent clashes between CL_EXPUNGE and an OP_* constant allowing setting
	 * the CL_EXPUNGE flag which will expunge when the mailbox is closed (be that manually, or via the
	 * IMAP\Connection object being destroyed naturally at the end of the PHP script */
	if (flags && ((flags & ~(OP_READONLY | OP_ANONYMOUS | OP_HALFOPEN | PHP_EXPUNGE | OP_DEBUG | OP_SHORTCACHE
	 		| OP_SILENT | OP_PROTOTYPE | OP_SECURE)) != 0)) {
		zend_argument_value_error(4, "must be a bitmask of the OP_* constants, and CL_EXPUNGE");
		RETURN_THROWS();
	}

	if (retries < 0) {
		zend_argument_value_error(5, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (flags) {
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		if (flags & OP_PROTOTYPE) {
			cl_flags |= OP_PROTOTYPE;
		}
	}

	if (params) {
		zval *disabled_auth_method;

		if ((disabled_auth_method = zend_hash_str_find(params, "DISABLE_AUTHENTICATOR", sizeof("DISABLE_AUTHENTICATOR") - 1)) != NULL) {
			switch (Z_TYPE_P(disabled_auth_method)) {
				case IS_STRING:
					if (Z_STRLEN_P(disabled_auth_method) > 1) {
						mail_parameters(NIL, DISABLE_AUTHENTICATOR, (void *)Z_STRVAL_P(disabled_auth_method));
					}
					break;
				case IS_ARRAY:
					{
						zval *z_auth_method;
						int i;
						int nelems = zend_hash_num_elements(Z_ARRVAL_P(disabled_auth_method));

						if (nelems == 0 ) {
							break;
						}
						for (i = 0; i < nelems; i++) {
							if ((z_auth_method = zend_hash_index_find(Z_ARRVAL_P(disabled_auth_method), i)) != NULL) {
								if (Z_TYPE_P(z_auth_method) == IS_STRING) {
									if (Z_STRLEN_P(z_auth_method) > 1) {
										mail_parameters(NIL, DISABLE_AUTHENTICATOR, (void *)Z_STRVAL_P(z_auth_method));
									}
								} else {
									zend_argument_type_error(6, "option \"DISABLE_AUTHENTICATOR\" must be a string or an array of strings");
									RETURN_THROWS();
								}
							}
						}
					}
					break;
				default:
					zend_argument_type_error(6, "option \"DISABLE_AUTHENTICATOR\" must be a string or an array of strings");
					RETURN_THROWS();
			}
		}
	}

	if (IMAPG(imap_user)) {
		efree(IMAPG(imap_user));
		IMAPG(imap_user) = 0;
	}

	if (IMAPG(imap_password)) {
		efree(IMAPG(imap_password));
		IMAPG(imap_password) = 0;
	}

	/* local filename, need to perform open_basedir check */
	if (ZSTR_VAL(mailbox)[0] != '{' && php_check_open_basedir(ZSTR_VAL(mailbox))) {
		RETURN_FALSE;
	}

	IMAPG(imap_user)     = estrndup(ZSTR_VAL(user), ZSTR_LEN(user));
	IMAPG(imap_password) = estrndup(ZSTR_VAL(passwd), ZSTR_LEN(passwd));

#ifdef SET_MAXLOGINTRIALS
	if (retries) {
		mail_parameters(NIL, SET_MAXLOGINTRIALS, (void *) retries);
	}
#endif

	ZEND_IGNORE_LEAKS_BEGIN();
	imap_stream = mail_open(NIL, ZSTR_VAL(mailbox), flags);
	ZEND_IGNORE_LEAKS_END();

	if (imap_stream == NIL) {
		php_error_docref(NULL, E_WARNING, "Couldn't open stream %s", ZSTR_VAL(mailbox));
		efree(IMAPG(imap_user)); IMAPG(imap_user) = 0;
		efree(IMAPG(imap_password)); IMAPG(imap_password) = 0;
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_imap_ce);
	imap_object_from_zend_object(Z_OBJ_P(return_value))->imap_stream = imap_stream;
	imap_object_from_zend_object(Z_OBJ_P(return_value))->flags = cl_flags;
}
/* }}} */

/* {{{ Reopen an IMAP stream to a new mailbox */
PHP_FUNCTION(imap_reopen)
{
	zval *imap_conn_obj;
	zend_string *mailbox;
	zend_long options = 0, retries = 0;
	php_imap_object *imap_conn_struct;
	long flags = 0;
	long cl_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS|ll", &imap_conn_obj, php_imap_ce, &mailbox, &options, &retries) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* TODO Verify these are the only options available as they are pulled from the php.net documentation */
	/* Check for PHP_EXPUNGE and not CL_EXPUNGE as the user land facing CL_EXPUNGE constant is defined
     * to something different to prevent clashes between CL_EXPUNGE and an OP_* constant allowing setting
     * the CL_EXPUNGE flag which will expunge when the mailbox is closed (be that manually, or via the
     * IMAP\Connection object being destroyed naturally at the end of the PHP script */
	if (options && ((options & ~(OP_READONLY | OP_ANONYMOUS | OP_HALFOPEN | OP_EXPUNGE | PHP_EXPUNGE)) != 0)) {
		zend_argument_value_error(3, "must be a bitmask of OP_READONLY, OP_ANONYMOUS, OP_HALFOPEN, "
			"OP_EXPUNGE, and CL_EXPUNGE");
		RETURN_THROWS();
	}

	if (retries < 0) {
		zend_argument_value_error(4, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (options) {
		flags = options;
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		imap_conn_struct->flags = cl_flags;
	}
#ifdef SET_MAXLOGINTRIALS
	if (retries) {
		mail_parameters(NIL, SET_MAXLOGINTRIALS, (void *) retries);
	}
#endif
	/* local filename, need to perform open_basedir check */
	if (ZSTR_VAL(mailbox)[0] != '{' && php_check_open_basedir(ZSTR_VAL(mailbox))) {
		RETURN_FALSE;
	}

	imap_conn_struct->imap_stream = mail_open(imap_conn_struct->imap_stream, ZSTR_VAL(mailbox), flags);
	if (imap_conn_struct->imap_stream == NIL) {
		/* IMAP\Connection object will release it self. */
		php_error_docref(NULL, E_WARNING, "Couldn't re-open stream");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

PHP_FUNCTION(imap_is_open)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	/* Manual reimplementation of the GET_IMAP_STREAM() macro that doesn't throw */
	imap_conn_struct = imap_object_from_zend_object(Z_OBJ_P(imap_conn_obj));
	/* Stream was closed */
	if (imap_conn_struct->imap_stream == NULL) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* {{{ Append a new message to a specified mailbox */
PHP_FUNCTION(imap_append)
{
	zval *imap_conn_obj;
	zend_string *folder, *message, *internal_date = NULL, *flags = NULL;
	php_imap_object *imap_conn_struct;
	STRING st;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS|S!S!", &imap_conn_obj, php_imap_ce, &folder, &message, &flags, &internal_date) == FAILURE) {
		RETURN_THROWS();
	}

	if (internal_date) {
		zend_string *regex  = ZSTR_INIT_LITERAL("/[0-3][0-9]-((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))-[0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] [+-][0-9]{4}/", 0);
		pcre_cache_entry *pce;				/* Compiled regex */
		zval *subpats = NULL;				/* Parts (not used) */
		int global = 0;

		/* Make sure the given internal_date string matches the RFC specifiedformat */
		if ((pce = pcre_get_compiled_regex_cache(regex))== NULL) {
			zend_string_release(regex);
			RETURN_FALSE;
		}

		zend_string_release(regex);
		php_pcre_match_impl(pce, internal_date, return_value, subpats, global,
			0, Z_L(0), Z_L(0));

		if (!Z_LVAL_P(return_value)) {
			// TODO Promoto to error?
			php_error_docref(NULL, E_WARNING, "Internal date not correctly formatted");
			internal_date = NULL;
		}
	}

	/* TODO Check if flags are valid (documentation is not present on php.net so need to check this first) */

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	INIT (&st, mail_string, (void *) ZSTR_VAL(message), ZSTR_LEN(message));

	if (mail_append_full(imap_conn_struct->imap_stream, ZSTR_VAL(folder), (flags ? ZSTR_VAL(flags) : NIL), (internal_date ? ZSTR_VAL(internal_date) : NIL), &st)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Gives the number of messages in the current mailbox */
PHP_FUNCTION(imap_num_msg)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	RETURN_LONG(imap_conn_struct->imap_stream->nmsgs);
}
/* }}} */

/* {{{ Check if the IMAP stream is still active */
PHP_FUNCTION(imap_ping)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	RETURN_BOOL(mail_ping(imap_conn_struct->imap_stream));
}
/* }}} */

/* {{{ Gives the number of recent messages in current mailbox */
PHP_FUNCTION(imap_num_recent)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	RETURN_LONG(imap_conn_struct->imap_stream->recent);
}
/* }}} */

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
/* {{{ Returns the quota set to the mailbox account qroot */
PHP_FUNCTION(imap_get_quota)
{
	zval *imap_conn_obj;
	zend_string *qroot;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &qroot) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	array_init(return_value);
	IMAPG(quota_return) = &return_value;

	/* set the callback for the GET_QUOTA function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if (!imap_getquota(imap_conn_struct->imap_stream, ZSTR_VAL(qroot))) {
		php_error_docref(NULL, E_WARNING, "C-client imap_getquota failed");
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns the quota set to the mailbox account mbox */
PHP_FUNCTION(imap_get_quotaroot)
{
	zval *imap_conn_obj;
	zend_string *mbox;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &mbox) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	array_init(return_value);
	IMAPG(quota_return) = &return_value;

	/* set the callback for the GET_QUOTAROOT function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if (!imap_getquotaroot(imap_conn_struct->imap_stream, ZSTR_VAL(mbox))) {
		php_error_docref(NULL, E_WARNING, "C-client imap_getquotaroot failed");
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Will set the quota for qroot mailbox */
PHP_FUNCTION(imap_set_quota)
{
	zval *imap_conn_obj;
	zend_string *qroot;
	zend_long mailbox_size;
	php_imap_object *imap_conn_struct;
	STRINGLIST	limits;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSl", &imap_conn_obj, php_imap_ce, &qroot, &mailbox_size) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	limits.text.data = (unsigned char*)"STORAGE";
	limits.text.size = mailbox_size;
	limits.next = NIL;

	RETURN_BOOL(imap_setquota(imap_conn_struct->imap_stream, ZSTR_VAL(qroot), &limits));
}
/* }}} */

/* {{{ Sets the ACL for a given mailbox */
PHP_FUNCTION(imap_setacl)
{
	zval *imap_conn_obj;
	zend_string *mailbox, *id, *rights;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSSS", &imap_conn_obj, php_imap_ce, &mailbox, &id, &rights) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	RETURN_BOOL(imap_setacl(imap_conn_struct->imap_stream, ZSTR_VAL(mailbox), ZSTR_VAL(id), ZSTR_VAL(rights)));
}
/* }}} */

/* {{{ Gets the ACL for a given mailbox */
PHP_FUNCTION(imap_getacl)
{
	zval *imap_conn_obj;
	zend_string *mailbox;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &mailbox) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* initializing the special array for the return values */
	array_init(return_value);

	IMAPG(imap_acl_list) = return_value;

	/* set the callback for the GET_ACL function */
	mail_parameters(NIL, SET_ACL, (void *) mail_getacl);
	if (!imap_getacl(imap_conn_struct->imap_stream, ZSTR_VAL(mailbox))) {
		php_error(E_WARNING, "c-client imap_getacl failed");
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}

	IMAPG(imap_acl_list) = NIL;
}
/* }}} */
#endif /* HAVE_IMAP2000 || HAVE_IMAP2001 */

/* {{{ Permanently delete all messages marked for deletion */
PHP_FUNCTION(imap_expunge)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	mail_expunge (imap_conn_struct->imap_stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ This function garbage collects (purges) the cache of entries of a specific type. */
PHP_FUNCTION(imap_gc)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &imap_conn_obj, php_imap_ce, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (flags && ((flags & ~(GC_TEXTS | GC_ELT | GC_ENV)) != 0)) {
		zend_argument_value_error(2, "must be a bitmask of IMAP_GC_TEXTS, IMAP_GC_ELT, and IMAP_GC_ENV");
		RETURN_THROWS();
	}

	mail_gc(imap_conn_struct->imap_stream, flags);

	// TODO Return void?
	RETURN_TRUE;
}
/* }}} */

/* {{{ Close an IMAP stream */
PHP_FUNCTION(imap_close)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct = NULL;
	zend_long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &imap_conn_obj, php_imap_ce, &options) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (options) {
		/* Check that flags is exactly equal to PHP_EXPUNGE or zero */
		if (options && ((options & ~PHP_EXPUNGE) != 0)) {
			zend_argument_value_error(2, "must be CL_EXPUNGE or 0");
			RETURN_THROWS();
		}

		/* Do the translation from PHP's internal PHP_EXPUNGE define to c-client's CL_EXPUNGE */
		if (options & PHP_EXPUNGE) {
			options ^= PHP_EXPUNGE;
			options |= CL_EXPUNGE;
		}
		imap_conn_struct->flags = options;
	}

	/* Do not try to close prototype streams */
	if (!(imap_conn_struct->flags & OP_PROTOTYPE)) {
		mail_close_full(imap_conn_struct->imap_stream, imap_conn_struct->flags);
		imap_conn_struct->imap_stream = NULL;
	}

	// TODO Return void?
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns headers for all messages in a mailbox */
PHP_FUNCTION(imap_headers)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;
	unsigned long i;
	char *t;
	unsigned int msgno;
	char tmp[MAILTMPLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* Initialize return array */
	array_init(return_value);

	for (msgno = 1; msgno <= imap_conn_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_conn_struct->imap_stream, msgno);
		mail_fetchstructure(imap_conn_struct->imap_stream, msgno, NIL);
		tmp[0] = cache->recent ? (cache->seen ? 'R': 'N') : ' ';
		tmp[1] = (cache->recent | cache->seen) ? ' ' : 'U';
		tmp[2] = cache->flagged ? 'F' : ' ';
		tmp[3] = cache->answered ? 'A' : ' ';
		tmp[4] = cache->deleted ? 'D' : ' ';
		tmp[5] = cache->draft ? 'X' : ' ';
		snprintf(tmp + 6, sizeof(tmp) - 6, "%4ld) ", cache->msgno);
		mail_date(tmp+11, cache);
		tmp[22] = ' ';
		tmp[23] = '\0';
		mail_fetchfrom(tmp+23, imap_conn_struct->imap_stream, msgno, (long)20);
		strcat(tmp, " ");
		if ((i = cache->user_flags)) {
			strcat(tmp, "{");
			while (i) {
				strlcat(tmp, imap_conn_struct->imap_stream->user_flags[find_rightmost_bit (&i)], sizeof(tmp));
				if (i) strlcat(tmp, " ", sizeof(tmp));
			}
			strlcat(tmp, "} ", sizeof(tmp));
		}
		mail_fetchsubject(t = tmp + strlen(tmp), imap_conn_struct->imap_stream, msgno, (long)25);
		snprintf(t += strlen(t), sizeof(tmp) - strlen(tmp), " (%ld chars)", cache->rfc822_size);
		add_next_index_string(return_value, tmp);
	}
}
/* }}} */

/* {{{ Read the message body */
PHP_FUNCTION(imap_body)
{
	zval *imap_conn_obj;
	zend_long msgno, flags = 0;
	php_imap_object *imap_conn_struct;
	char *body;
	unsigned long body_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|l", &imap_conn_obj, php_imap_ce, &msgno, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgno, 2);

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		zend_argument_value_error(3, "must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL");
		RETURN_THROWS();
	}

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgno, 2, flags, FT_UID);

	body = mail_fetchtext_full (imap_conn_struct->imap_stream, msgno, &body_len, flags);
	if (body_len == 0) {
		RETVAL_EMPTY_STRING();
	} else {
		RETVAL_STRINGL(body, body_len);
	}
}
/* }}} */

/* TODO UID Tests */
/* {{{ Copy specified message to a mailbox */
PHP_FUNCTION(imap_mail_copy)
{
	zval *imap_conn_obj;
	zend_long options = 0;
	zend_string *seq, *folder;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS|l", &imap_conn_obj, php_imap_ce, &seq, &folder, &options) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (options && ((options & ~(CP_UID | CP_MOVE)) != 0)) {
		zend_argument_value_error(4, "must be a bitmask of CP_UID, and CP_MOVE");
		RETURN_THROWS();
	}

	if (mail_copy_full(imap_conn_struct->imap_stream, ZSTR_VAL(seq), ZSTR_VAL(folder), options) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* TODO UID Tests */
/* {{{ Move specified message to a mailbox */
PHP_FUNCTION(imap_mail_move)
{
	zval *imap_conn_obj;
	zend_string *seq, *folder;
	zend_long options = 0;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS|l", &imap_conn_obj, php_imap_ce, &seq, &folder, &options) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (options && ((options & ~CP_UID) != 0)) {
		zend_argument_value_error(4, "must be CP_UID or 0");
		RETURN_THROWS();
	}

	/* Add CP_MOVE flag */
	options = (options | CP_MOVE);

	if (mail_copy_full(imap_conn_struct->imap_stream, ZSTR_VAL(seq), ZSTR_VAL(folder), options) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Create a new mailbox */
PHP_FUNCTION(imap_createmailbox)
{
	zval *imap_conn_obj;
	zend_string *folder;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &folder) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (mail_create(imap_conn_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Rename a mailbox */
PHP_FUNCTION(imap_renamemailbox)
{
	zval *imap_conn_obj;
	zend_string *old_mailbox, *new_mailbox;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS", &imap_conn_obj, php_imap_ce, &old_mailbox, &new_mailbox) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (mail_rename(imap_conn_struct->imap_stream, ZSTR_VAL(old_mailbox), ZSTR_VAL(new_mailbox)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Delete a mailbox */
PHP_FUNCTION(imap_deletemailbox)
{
	zval *imap_conn_obj;
	zend_string *folder;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &folder) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (mail_delete(imap_conn_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Read the list of mailboxes */
PHP_FUNCTION(imap_list)
{
	zval *imap_conn_obj;
	zend_string *ref, *pat;
	php_imap_object *imap_conn_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS", &imap_conn_obj, php_imap_ce, &ref, &pat) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;

	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
	mail_list(imap_conn_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	if (IMAPG(imap_folders) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	cur=IMAPG(imap_folders);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur=cur->next;
	}
	mail_free_stringlist (&IMAPG(imap_folders));
	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
}

/* }}} */

static void php_imap_populate_mailbox_object(zval *z_object, const FOBJECTLIST *mailbox)
{
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"name", strlen("name"),
		(char*)mailbox->LTEXT
	);
	zend_update_property_long(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"attributes", strlen("attributes"),
		mailbox->attributes
	);
#ifdef IMAP41
	zend_update_property_str(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"delimiter", strlen("delimiter"),
		ZSTR_CHAR((unsigned char)mailbox->delimiter)
	);
#else
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"delimiter", strlen("delimiter"),
		mailbox->delimiter
	);
#endif
}

/* {{{ Reads the list of mailboxes and returns a full array of objects containing name, attributes, and delimiter */
/* Author: CJH */
PHP_FUNCTION(imap_getmailboxes)
{
	zval *imap_conn_obj;
	zend_string *ref, *pat;
	php_imap_object *imap_conn_struct;
	FOBJECTLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS", &imap_conn_obj, php_imap_ce, &ref, &pat) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* set flag for new, improved array of objects mailbox list */
	IMAPG(folderlist_style) = FLIST_OBJECT;

	IMAPG(imap_folder_objects) = IMAPG(imap_folder_objects_tail) = NIL;
	mail_list(imap_conn_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	if (IMAPG(imap_folder_objects) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	cur=IMAPG(imap_folder_objects);
	while (cur != NIL) {
		zval mboxob;
		object_init(&mboxob);
		php_imap_populate_mailbox_object(&mboxob, cur);
		zend_hash_next_index_insert_new(Z_ARR_P(return_value), &mboxob);
		cur=cur->next;
	}
	mail_free_foblist(&IMAPG(imap_folder_objects), &IMAPG(imap_folder_objects_tail));
	IMAPG(folderlist_style) = FLIST_ARRAY;		/* reset to default */
}
/* }}} */

/* {{{ Read list of mailboxes containing a certain string */
PHP_FUNCTION(imap_listscan)
{
	zval *imap_conn_obj;
	zend_string *ref, *pat, *content;
	php_imap_object *imap_conn_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSSS", &imap_conn_obj, php_imap_ce, &ref, &pat, &content) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	IMAPG(imap_folders) = NIL;
	mail_scan(imap_conn_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat), ZSTR_VAL(content));
	if (IMAPG(imap_folders) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	cur=IMAPG(imap_folders);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur=cur->next;
	}
	mail_free_stringlist (&IMAPG(imap_folders));
	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
}

/* }}} */

static void php_imap_populate_mailbox_properties_object(zval *z_object, const MAILSTREAM *imap_stream)
{
	char date[100];
	rfc822_date(date);
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"Date", strlen("Date"),
		date
	);
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"Driver", strlen("Driver"),
		imap_stream->dtb->name
	);
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"Mailbox", strlen("Mailbox"),
		imap_stream->mailbox
	);
	zend_update_property_long(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"Nmsgs", strlen("Nmsgs"),
		imap_stream->nmsgs
	);
	zend_update_property_long(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"Recent", strlen("Recent"),
		imap_stream->recent
	);
}

/* {{{ Get mailbox properties */
PHP_FUNCTION(imap_check)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (mail_ping (imap_conn_struct->imap_stream) == NIL) {
		RETURN_FALSE;
	}

	if (imap_conn_struct->imap_stream->mailbox) {
		object_init(return_value);
		php_imap_populate_mailbox_properties_object(return_value, imap_conn_struct->imap_stream);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Mark a message for deletion */
PHP_FUNCTION(imap_delete)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;
	zend_string *sequence;
	zend_long flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS|l", &imap_conn_obj, php_imap_ce, &sequence, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	// TODO Check sequence validity?

	if (flags && ((flags & ~FT_UID) != 0)) {
		zend_argument_value_error(3, "must be FT_UID or 0");
		RETURN_THROWS();
	}

	mail_setflag_full(imap_conn_struct->imap_stream, ZSTR_VAL(sequence), "\\DELETED", flags);

	// TODO Return void?
	RETURN_TRUE;
}
/* }}} */

/* {{{ Remove the delete flag from a message */
PHP_FUNCTION(imap_undelete)
{
	zval *imap_conn_obj;
	zend_string *sequence;
	zend_long flags = 0;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS|l", &imap_conn_obj, php_imap_ce, &sequence, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* TODO Check if flags are valid (documentation is not present on php.net so need to check this first) */

	mail_clearflag_full(imap_conn_struct->imap_stream, ZSTR_VAL(sequence), "\\DELETED", flags);

	// TODO Return void?
	RETURN_TRUE;
}
/* }}} */

/* {{{ Read the headers of the message */
PHP_FUNCTION(imap_headerinfo)
{
	zval *imap_conn_obj;
	zend_long msgno, fromlength = 0, subjectlength = 0;
	php_imap_object *imap_conn_struct;
	MESSAGECACHE *cache;
	ENVELOPE *en;
	char dummy[2000], fulladdress[MAILTMPLEN + 1];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|ll", &imap_conn_obj, php_imap_ce, &msgno, &fromlength, &subjectlength) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO(msgno, 2);

	if (fromlength < 0 || fromlength > MAILTMPLEN) {
		zend_argument_value_error(3, "must be between 0 and %d", MAILTMPLEN);
		RETURN_THROWS();
	}

	if (subjectlength < 0 || subjectlength > MAILTMPLEN) {
		zend_argument_value_error(4, "must be between 0 and %d", MAILTMPLEN);
		RETURN_THROWS();
	}

	if (mail_fetchstructure(imap_conn_struct->imap_stream, msgno, NIL)) {
		cache = mail_elt(imap_conn_struct->imap_stream, msgno);
	} else {
		RETURN_FALSE;
	}

	en = mail_fetchenvelope(imap_conn_struct->imap_stream, msgno);

	/* call a function to parse all the text, so that we can use the
	   same function to parse text from other sources */
	_php_make_header_object(return_value, en);

	/* now run through properties that are only going to be returned
	   from a server, not text headers */
	zend_update_property_str(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Recent", strlen("Recent"),
		cache->recent ? (
			cache->seen ? ZSTR_CHAR('R') : ZSTR_CHAR('N')
		) : ZSTR_CHAR(' ')
	);
	zend_update_property_str(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Unseen", strlen("Unseen"),
		(cache->recent | cache->seen) ? ZSTR_CHAR(' ') : ZSTR_CHAR('U')
	);
	zend_update_property_str(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Flagged", strlen("Flagged"),
		cache->flagged ? ZSTR_CHAR('F') : ZSTR_CHAR(' ')
	);
	zend_update_property_str(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Answered", strlen("Answered"),
		cache->answered ? ZSTR_CHAR('A') : ZSTR_CHAR(' ')
	);
	zend_update_property_str(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Deleted", strlen("Deleted"),
		cache->deleted ? ZSTR_CHAR('D') : ZSTR_CHAR(' ')
	);
	zend_update_property_str(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Draft", strlen("Draft"),
		cache->draft ? ZSTR_CHAR('X') : ZSTR_CHAR(' ')
	);

	snprintf(dummy, sizeof(dummy), "%4ld", cache->msgno);
	zend_update_property_string(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Msgno", strlen("Msgno"),
		dummy
	);

	mail_date(dummy, cache);
	zend_update_property_string(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"MailDate", strlen("MailDate"),
		dummy
	);

	snprintf(dummy, sizeof(dummy), "%ld", cache->rfc822_size);
	zend_update_property_string(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Size", strlen("Size"),
		dummy
	);

	zend_update_property_long(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"udate", strlen("udate"),
		mail_longdate(cache)
	);

	if (en->from && fromlength) {
		fulladdress[0] = 0x00;
		mail_fetchfrom(fulladdress, imap_conn_struct->imap_stream, msgno, fromlength);
		zend_update_property_string(
			Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
			"fetchfrom", strlen("fetchfrom"),
			fulladdress
		);
	}
	if (en->subject && subjectlength) {
		fulladdress[0] = 0x00;
		mail_fetchsubject(fulladdress, imap_conn_struct->imap_stream, msgno, subjectlength);
		zend_update_property_string(
			Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
			"fetchsubject", strlen("fetchsubject"),
			fulladdress
		);
	}
}
/* }}} */

/* {{{ Parse a set of mail headers contained in a string, and return an object similar to imap_headerinfo() */
PHP_FUNCTION(imap_rfc822_parse_headers)
{
	zend_string *headers, *defaulthost = NULL;
	ENVELOPE *en;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &headers, &defaulthost) == FAILURE) {
		RETURN_THROWS();
	}

	if (defaulthost) {
		rfc822_parse_msg(&en, NULL, ZSTR_VAL(headers), ZSTR_LEN(headers), NULL, ZSTR_VAL(defaulthost), NIL);
	} else {
		rfc822_parse_msg(&en, NULL, ZSTR_VAL(headers), ZSTR_LEN(headers), NULL, "UNKNOWN", NIL);
	}

	/* call a function to parse all the text, so that we can use the
	   same function no matter where the headers are from */
	_php_make_header_object(return_value, en);
	mail_free_envelope(&en);
}
/* }}} */

/* KMLANG */
/* {{{ Return a list of subscribed mailboxes */
PHP_FUNCTION(imap_lsub)
{
	zval *imap_conn_obj;
	zend_string *ref, *pat;
	php_imap_object *imap_conn_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS", &imap_conn_obj, php_imap_ce, &ref, &pat) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;

	IMAPG(imap_sfolders) = NIL;
	ZEND_IGNORE_LEAKS_BEGIN();
	mail_lsub(imap_conn_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	ZEND_IGNORE_LEAKS_END();
	if (IMAPG(imap_sfolders) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	cur=IMAPG(imap_sfolders);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur=cur->next;
	}
	mail_free_stringlist (&IMAPG(imap_sfolders));
	IMAPG(imap_sfolders) = IMAPG(imap_sfolders_tail) = NIL;
}
/* }}} */

/* {{{ Return a list of subscribed mailboxes, in the same format as imap_getmailboxes() */
/* Author: CJH */
PHP_FUNCTION(imap_getsubscribed)
{
	zval *imap_conn_obj;
	zend_string *ref, *pat;
	php_imap_object *imap_conn_struct;
	FOBJECTLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS", &imap_conn_obj, php_imap_ce, &ref, &pat) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* set flag for new, improved array of objects list */
	IMAPG(folderlist_style) = FLIST_OBJECT;

	IMAPG(imap_sfolder_objects) = IMAPG(imap_sfolder_objects_tail) = NIL;
	ZEND_IGNORE_LEAKS_BEGIN();
	mail_lsub(imap_conn_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	ZEND_IGNORE_LEAKS_END();
	if (IMAPG(imap_sfolder_objects) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	cur = IMAPG(imap_sfolder_objects);
	while (cur != NIL) {
		zval mboxob;
		object_init(&mboxob);
		php_imap_populate_mailbox_object(&mboxob, cur);
		zend_hash_next_index_insert_new(Z_ARR_P(return_value), &mboxob);

		cur = cur->next;
	}
	mail_free_foblist (&IMAPG(imap_sfolder_objects), &IMAPG(imap_sfolder_objects_tail));
	IMAPG(folderlist_style) = FLIST_ARRAY; /* reset to default */
}
/* }}} */

/* {{{ Subscribe to a mailbox */
PHP_FUNCTION(imap_subscribe)
{
	zval *imap_conn_obj;
	zend_string *folder;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &folder) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (mail_subscribe(imap_conn_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Unsubscribe from a mailbox */
PHP_FUNCTION(imap_unsubscribe)
{
	zval *imap_conn_obj;
	zend_string *folder;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &imap_conn_obj, php_imap_ce, &folder) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (mail_unsubscribe(imap_conn_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Read the full structure of a message */
PHP_FUNCTION(imap_fetchstructure)
{
	zval *imap_conn_obj;
	zend_long msgno, flags = 0;
	php_imap_object *imap_conn_struct;
	BODY *body;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|l", &imap_conn_obj, php_imap_ce, &msgno, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgno, 2);

	if (flags && ((flags & ~FT_UID) != 0)) {
		zend_argument_value_error(3, "must be FT_UID or 0");
		RETURN_THROWS();
	}

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgno, 2, flags, FT_UID);

	object_init(return_value);

	mail_fetchstructure_full(imap_conn_struct->imap_stream, msgno, &body , flags);

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body information available");
		RETURN_FALSE;
	}

	_php_imap_add_body(return_value, body);
}
/* }}} */

/* {{{ Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	zval *imap_conn_obj;
	zend_long msgno, flags = 0;
	php_imap_object *imap_conn_struct;
	char *body;
	zend_string *sec;
	unsigned long len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OlS|l", &imap_conn_obj, php_imap_ce, &msgno, &sec, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgno, 2);

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		zend_argument_value_error(4, "must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL");
		RETURN_THROWS();
	}

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgno, 2, flags, FT_UID);

	body = mail_fetchbody_full(imap_conn_struct->imap_stream, msgno, ZSTR_VAL(sec), &len, flags);

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL(body, len);
}

/* }}} */


/* {{{ Get a specific body section's MIME headers */
PHP_FUNCTION(imap_fetchmime)
{
	zval *imap_conn_obj;
	zend_long msgno, flags = 0;
	php_imap_object *imap_conn_struct;
	char *body;
	zend_string *sec;
	unsigned long len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OlS|l", &imap_conn_obj, php_imap_ce, &msgno, &sec, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgno, 2);

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		zend_argument_value_error(4, "must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL");
		RETURN_THROWS();
	}

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgno, 2, flags, FT_UID);

	body = mail_fetch_mime(imap_conn_struct->imap_stream, msgno, ZSTR_VAL(sec), &len, flags);

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body MIME information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL(body, len);
}

/* }}} */

/* {{{ Save a specific body section to a file */
PHP_FUNCTION(imap_savebody)
{
	zval *imap_conn_obj;
	zval *out;
	php_imap_object *imap_conn_struct = NULL;
	php_stream *writer = NULL;
	zend_string *section = NULL;
	int close_stream = 1;
	zend_long msgno, flags = 0;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "Ozl|Sl", &imap_conn_obj, php_imap_ce, &out, &msgno, &section, &flags)) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgno, 3)

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		zend_argument_value_error(5, "must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL");
		RETURN_THROWS();
	}

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgno, 3, flags, FT_UID);

	switch (Z_TYPE_P(out))
	{
		case IS_LONG:
		case IS_RESOURCE:
			close_stream = 0;
			php_stream_from_zval(writer, out);
		break;

		default:
			if (!try_convert_to_string(out)) {
				RETURN_THROWS();
			}
			// TODO Need to check for null bytes?
			writer = php_stream_open_wrapper(Z_STRVAL_P(out), "wb", REPORT_ERRORS, NULL);
		break;
	}

	if (!writer) {
		RETURN_FALSE;
	}

	IMAPG(gets_stream) = writer;
	mail_parameters(NIL, SET_GETS, (void *) php_mail_gets);
	mail_fetchbody_full(imap_conn_struct->imap_stream, msgno, section?ZSTR_VAL(section):"", NULL, flags);
	mail_parameters(NIL, SET_GETS, (void *) NULL);
	IMAPG(gets_stream) = NULL;

	if (close_stream) {
		php_stream_close(writer);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Decode BASE64 encoded text */
PHP_FUNCTION(imap_base64)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		RETURN_THROWS();
	}

	decode = (char *) rfc822_base64((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ Convert a quoted-printable string to an 8-bit string */
PHP_FUNCTION(imap_qprint)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		RETURN_THROWS();
	}

	decode = (char *) rfc822_qprint((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ Convert an 8-bit string to a quoted-printable string */
PHP_FUNCTION(imap_8bit)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		RETURN_THROWS();
	}

	decode = (char *) rfc822_8bit((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ Convert an 8bit string to a base64 string */
PHP_FUNCTION(imap_binary)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		RETURN_THROWS();
	}

	decode = (char*)rfc822_binary(ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ Returns info about the current mailbox */
PHP_FUNCTION(imap_mailboxmsginfo)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;
	unsigned long msgno;
	zend_ulong unreadmsg = 0, deletedmsg = 0, msize = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &imap_conn_obj, php_imap_ce) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* Initialize return object */
	object_init(return_value);

	for (msgno = 1; msgno <= imap_conn_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_conn_struct->imap_stream, msgno);
		mail_fetchstructure (imap_conn_struct->imap_stream, msgno, NIL);

		if (!cache->seen || cache->recent) {
			unreadmsg++;
		}

		if (cache->deleted) {
			deletedmsg++;
		}
		msize = msize + cache->rfc822_size;
	}
	zend_update_property_long(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Unread", strlen("Unread"),
		unreadmsg
	);
	zend_update_property_long(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Deleted", strlen("Deleted"),
		deletedmsg
	);
	zend_update_property_long(
		Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
		"Size", strlen("Size"),
		msize
	);

	php_imap_populate_mailbox_properties_object(return_value, imap_conn_struct->imap_stream);
}
/* }}} */

/* {{{ Returns a properly formatted email address given the mailbox, host, and personal info */
PHP_FUNCTION(imap_rfc822_write_address)
{
	zend_string *mailbox, *host, *personal;
	ADDRESS *addr;
	zend_string *string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS", &mailbox, &host, &personal) == FAILURE) {
		RETURN_THROWS();
	}

	addr=mail_newaddr();

	if (mailbox) {
		addr->mailbox = cpystr(ZSTR_VAL(mailbox));
	}

	if (host) {
		addr->host = cpystr(ZSTR_VAL(host));
	}

	if (personal) {
		addr->personal = cpystr(ZSTR_VAL(personal));
	}

	addr->next=NIL;
	addr->error=NIL;
	addr->adl=NIL;

	string = _php_rfc822_write_address(addr);
	if (string) {
		RETVAL_STR(string);
	} else {
		RETVAL_FALSE;
	}
	mail_free_address(&addr);
}
/* }}} */

static void php_imap_construct_address_object(zval *z_object, const ADDRESS *address)
{
	if (address->mailbox) {
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"mailbox", strlen("mailbox"),
			address->mailbox
		);
	}
	if (address->host) {
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"host", strlen("host"),
			address->host
		);
	}
	if (address->personal) {
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"personal", strlen("personal"),
			address->personal
		);
	}
	if (address->adl) {
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"adl", strlen("adl"),
			address->adl
		);
	}
}

static void php_imap_construct_list_of_addresses(HashTable *list, const ADDRESS *const address_list)
{
	const ADDRESS *current_address = address_list;
	do {
		zval tmp_object;
		object_init(&tmp_object);
		php_imap_construct_address_object(&tmp_object, current_address);
		zend_hash_next_index_insert_new(list, &tmp_object);
	} while ((current_address = current_address->next));
}

/* {{{ Parses an address string */
PHP_FUNCTION(imap_rfc822_parse_adrlist)
{
	zend_string *str, *defaulthost;
	char *str_copy;
	ADDRESS *address_list;
	ENVELOPE *env;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &str, &defaulthost) == FAILURE) {
		RETURN_THROWS();
	}

	env = mail_newenvelope();

	/* rfc822_parse_adrlist() modifies passed string. Copy it. */
	str_copy = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
	rfc822_parse_adrlist(&env->to, str_copy, ZSTR_VAL(defaulthost));
	efree(str_copy);

	array_init(return_value);

	address_list = env->to;

	if (address_list) {
		php_imap_construct_list_of_addresses(Z_ARR_P(return_value), address_list);
	}

	mail_free_envelope(&env);
}

/* {{{ Convert a mime-encoded text to UTF-8 */
PHP_FUNCTION(imap_utf8)
{
	zend_string *str;
	SIZEDTEXT src, dest;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	src.data  = NULL;
	src.size  = 0;
	dest.data = NULL;
	dest.size = 0;

	cpytxt(&src, ZSTR_VAL(str), ZSTR_LEN(str));

#ifndef HAVE_NEW_MIME2TEXT
	utf8_mime2text(&src, &dest);
#else
	utf8_mime2text(&src, &dest, U8T_DECOMPOSE);
#endif
	RETVAL_STRINGL((char*)dest.data, dest.size);
	if (dest.data) {
		free(dest.data);
	}
	if (src.data && src.data != dest.data) {
		free(src.data);
	}
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
#define B64(n)	("ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
				"abcdefghijklmnopqrstuvwxyz0123456789+,"[(n) & 0x3f])

/* map the modified-base64 character `c' to its 64 bit value */
#define UNB64(c)	((c) == '+' ? 62 : (c) == ',' ? 63 : (c) >= 'a' ? \
					(c) - 71 : (c) >= 'A' ? (c) - 65 : (c) + 4)
/* }}} */

/* {{{ Decode a modified UTF-7 string */
PHP_FUNCTION(imap_utf7_decode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	zend_string *arg;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		RETURN_THROWS();
	}

	in = (const unsigned char *) ZSTR_VAL(arg);
	inlen = ZSTR_LEN(arg);

	/* validate and compute length of output string */
	outlen = 0;
	state = ST_NORMAL;
	for (endp = (inp = in) + inlen; inp < endp; inp++) {
		if (state == ST_NORMAL) {
			/* process printable character */
			if (SPECIAL(*inp)) {
				php_error_docref(NULL, E_WARNING, "Invalid modified UTF-7 character: `%c'", *inp);
				RETURN_FALSE;
			} else if (*inp != '&') {
				outlen++;
			} else if (inp + 1 == endp) {
				php_error_docref(NULL, E_WARNING, "Unexpected end of string");
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
				php_error_docref(NULL, E_WARNING, "Stray modified base64 character: `%c'", *--inp);
				RETURN_FALSE;
			}
			state = ST_NORMAL;
		} else if (!B64CHAR(*inp)) {
			php_error_docref(NULL, E_WARNING, "Invalid modified base64 character: `%c'", *inp);
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
					ZEND_FALLTHROUGH;
				case ST_DECODE0:
					state++;
				case ST_NORMAL:
					break;
			}
		}
	}

	/* enforce end state */
	if (state != ST_NORMAL) {
		php_error_docref(NULL, E_WARNING, "Unexpected end of string");
		RETURN_FALSE;
	}

	/* allocate output buffer */
	out = emalloc(outlen + 1);

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
		php_error_docref(NULL, E_WARNING, "outp - out [%zd] != outlen [%d]", outp - out, outlen);
	}
#endif

	RETURN_STRINGL((char*)out, outlen);
}
/* }}} */

/* {{{ Encode a string in modified UTF-7 */
PHP_FUNCTION(imap_utf7_encode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	zend_string *arg;
	const unsigned char *in, *inp, *endp;
	zend_string *out;
	unsigned char *outp;
	unsigned char c;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_ENCODE0,	/* encoded text rotation... */
		ST_ENCODE1,
		ST_ENCODE2
	} state;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		RETURN_THROWS();
	}

	in = (const unsigned char *) ZSTR_VAL(arg);
	inlen = ZSTR_LEN(arg);

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
	out = zend_string_safe_alloc(1, outlen, 0, 0);

	/* encode input string */
	outp = (unsigned char*)ZSTR_VAL(out);
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
				c = B64(*outp);
				*outp++ = c;
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

	RETURN_STR(out);
}
/* }}} */

#undef SPECIAL
#undef B64CHAR
#undef B64
#undef UNB64

#ifdef HAVE_IMAP_MUTF7
static void php_imap_mutf7(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zend_string *in;
	unsigned char *out;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &in) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(in) < 1) {
		RETURN_EMPTY_STRING();
	}

	if (mode == 0) {
		out = utf8_to_mutf7((unsigned char *) ZSTR_VAL(in));
	} else {
		out = utf8_from_mutf7((unsigned char *) ZSTR_VAL(in));
	}

	if (out == NIL) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING((char *)out);
		fs_give((void**) &out);
	}
}
/* }}} */

/* {{{ Encode a UTF-8 string to modified UTF-7 */
PHP_FUNCTION(imap_utf8_to_mutf7)
{
	php_imap_mutf7(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Decode a modified UTF-7 string to UTF-8 */
PHP_FUNCTION(imap_mutf7_to_utf8)
{
	php_imap_mutf7(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */
#endif

/* {{{ Sets flags on messages */
PHP_FUNCTION(imap_setflag_full)
{
	zval *imap_conn_obj;
	zend_string *sequence, *flag;
	zend_long flags = 0;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS|l", &imap_conn_obj, php_imap_ce, &sequence, &flag, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (flags && ((flags & ~ST_UID) != 0)) {
		zend_argument_value_error(4, "must be ST_UID or 0");
		RETURN_THROWS();
	}

	mail_setflag_full(imap_conn_struct->imap_stream, ZSTR_VAL(sequence), ZSTR_VAL(flag), (flags ? flags : NIL));
	RETURN_TRUE;
}
/* }}} */

/* {{{ Clears flags on messages */
PHP_FUNCTION(imap_clearflag_full)
{
	zval *imap_conn_obj;
	zend_string *sequence, *flag;
	zend_long flags = 0;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSS|l", &imap_conn_obj, php_imap_ce, &sequence, &flag, &flags) ==FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (flags && ((flags & ~ST_UID) != 0)) {
		zend_argument_value_error(4, "must be ST_UID or 0");
		RETURN_THROWS();
	}

	mail_clearflag_full(imap_conn_struct->imap_stream, ZSTR_VAL(sequence), ZSTR_VAL(flag), flags);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array of message headers, optionally including only messages that meet specified criteria. */
PHP_FUNCTION(imap_sort)
{
	zval *imap_conn_obj;
	zend_string *criteria = NULL, *charset = NULL;
	zend_long sort, flags = 0;
	bool rev;
	php_imap_object *imap_conn_struct;
	unsigned long *slst, *sl;
	char *search_criteria;
	SORTPGM *mypgm=NIL;
	SEARCHPGM *spg=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olb|lS!S!", &imap_conn_obj, php_imap_ce, &sort, &rev, &flags, &criteria, &charset) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (!(sort == SORTDATE || sort == SORTARRIVAL || sort == SORTFROM || sort == SORTSUBJECT || sort == SORTTO ||
			sort == SORTCC || sort == SORTSIZE) ) {
		zend_argument_value_error(2, "must be one of the SORT* constants");
		RETURN_THROWS();
	}

	if (flags && ((flags & ~(SE_UID|SE_NOPREFETCH )) != 0)) {
		zend_argument_value_error(4, "must be a bitmask of SE_UID, and SE_NOPREFETCH");
		RETURN_THROWS();
	}

	if (criteria) {
		search_criteria = estrndup(ZSTR_VAL(criteria), ZSTR_LEN(criteria));
		spg = mail_criteria(search_criteria);
		efree(search_criteria);
	} else {
		spg = mail_newsearchpgm();
	}
	if (spg == NIL) {
		RETURN_FALSE;
	}

	mypgm = mail_newsortpgm();
	mypgm->reverse = rev;
	mypgm->function = (short) sort;
	mypgm->next = NIL;

	slst = mail_sort(imap_conn_struct->imap_stream, (charset ? ZSTR_VAL(charset) : NIL), spg, mypgm, flags);

	mail_free_sortpgm(&mypgm);
	if (spg && !(flags & SE_FREE)) {
		mail_free_searchpgm(&spg);
	}

	array_init(return_value);
	if (slst != NULL) {
		for (sl = slst; *sl; sl++) {
			add_next_index_long(return_value, *sl);
		}
		fs_give ((void **) &slst);
	}
}
/* }}} */

/* {{{ Get the full unfiltered header for a message */
PHP_FUNCTION(imap_fetchheader)
{
	zval *imap_conn_obj;
	zend_long msgno, flags = 0;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|l", &imap_conn_obj, php_imap_ce, &msgno, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_PRE_FLAG_CHECKS(msgno, 2);

	if (flags && ((flags & ~(FT_UID|FT_INTERNAL|FT_PREFETCHTEXT)) != 0)) {
		zend_argument_value_error(3, "must be a bitmask of FT_UID, FT_PREFETCHTEXT, and FT_INTERNAL");
		RETURN_THROWS();
	}

	PHP_IMAP_CHECK_MSGNO_MAYBE_UID_POST_FLAG_CHECKS(msgno, 2, flags, FT_UID);

	RETVAL_STRING(mail_fetchheader_full(imap_conn_struct->imap_stream, msgno, NIL, NIL, flags));
}
/* }}} */

/* {{{ Get the unique message id associated with a standard sequential message number */
PHP_FUNCTION(imap_uid)
{
	zval *imap_conn_obj;
	zend_long msgno;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &imap_conn_obj, php_imap_ce, &msgno) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO(msgno, 2);

	RETURN_LONG(mail_uid(imap_conn_struct->imap_stream, msgno));
}
/* }}} */

/* {{{ Get the sequence number associated with a UID */
PHP_FUNCTION(imap_msgno)
{
	zval *imap_conn_obj;
	zend_long msg_uid;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &imap_conn_obj, php_imap_ce, &msg_uid) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* Do NOT use the PHP_IMAP_CHECK_MSGNO() macro as UID cannot be checked for their upper bound. */
	if (msg_uid < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	RETURN_LONG(mail_msgno(imap_conn_struct->imap_stream, msg_uid));
}
/* }}} */

/* {{{ Get status info from a mailbox */
PHP_FUNCTION(imap_status)
{
	zval *imap_conn_obj;
	zend_string *mbx;
	zend_long flags;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OSl", &imap_conn_obj, php_imap_ce, &mbx, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (flags && ((flags & ~(SA_ALL)) != 0)) {
		zend_argument_value_error(3, "must be a bitmask of SA_* constants");
		RETURN_THROWS();
	}

	if (mail_status(imap_conn_struct->imap_stream, ZSTR_VAL(mbx), flags)) {
		object_init(return_value);

		zend_update_property_long(
			Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
			"flags", strlen("flags"),
			IMAPG(status_flags)
		);
		if (IMAPG(status_flags) & SA_MESSAGES) {
			zend_update_property_long(
				Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
				"messages", strlen("messages"),
				IMAPG(status_messages)
			);
		}
		if (IMAPG(status_flags) & SA_RECENT) {
			zend_update_property_long(
				Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
				"recent", strlen("recent"),
				IMAPG(status_recent)
			);
		}
		if (IMAPG(status_flags) & SA_UNSEEN) {
			zend_update_property_long(
				Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
				"unseen", strlen("unseen"),
				IMAPG(status_unseen)
			);
		}
		if (IMAPG(status_flags) & SA_UIDNEXT) {
			zend_update_property_long(
				Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
				"uidnext", strlen("uidnext"),
				IMAPG(status_uidnext)
			);
		}
		if (IMAPG(status_flags) & SA_UIDVALIDITY) {
			zend_update_property_long(
				Z_OBJCE_P(return_value), Z_OBJ_P(return_value),
				"uidvalidity", strlen("uidvalidity"),
				IMAPG(status_uidvalidity)
			);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static void php_imap_populate_body_struct_object(zval *z_object, const BODY *body)
{
	if (body->type <= TYPEMAX) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"type", strlen("type"),
			body->type
		);
	}

	if (body->encoding <= ENCMAX) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"encoding", strlen("encoding"),
			body->encoding
		);
	}

	if (body->subtype) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifsubtype", strlen("ifsubtype"),
			1
		);
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"subtype", strlen("subtype"),
			body->subtype
		);
	} else {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifsubtype", strlen("ifsubtype"),
			0
		);
	}

	if (body->description) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifdescription", strlen("ifdescription"),
			1
		);
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"description", strlen("description"),
			body->description
		);
	} else {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifdescription", strlen("ifdescription"),
			0
		);
	}

	if (body->id) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifid", strlen("ifid"),
			1
		);
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"id", strlen("id"),
			body->id
		);
	} else {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifid", strlen("ifid"),
			0
		);
	}

	if (body->size.lines) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"lines", strlen("lines"),
			body->size.lines
		);
	}

	if (body->size.bytes) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"bytes", strlen("bytes"),
			body->size.bytes
		);
	}

#ifdef IMAP41
	if (body->disposition.type) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifdisposition", strlen("ifdisposition"),
			1
		);
		zend_update_property_string(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"disposition", strlen("disposition"),
			body->disposition.type
		);
	} else {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifdisposition", strlen("ifdisposition"),
			0
		);
	}

	if (body->disposition.parameter) {
		PARAMETER *disposition_parameter = body->disposition.parameter;
		zval z_disposition_parameter_list;

		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifdparameters", strlen("ifdparameters"),
			1
		);
		array_init(&z_disposition_parameter_list);
		do {
			zval z_disposition_parameter;
			object_init(&z_disposition_parameter);
			zend_update_property_string(
				Z_OBJCE_P(&z_disposition_parameter), Z_OBJ_P(&z_disposition_parameter),
				"attribute", strlen("attribute"),
				disposition_parameter->attribute
			);
			zend_update_property_string(
				Z_OBJCE_P(&z_disposition_parameter), Z_OBJ_P(&z_disposition_parameter),
				"value", strlen("value"),
				disposition_parameter->value
			);
			zend_hash_next_index_insert_new(Z_ARR(z_disposition_parameter_list), &z_disposition_parameter);
		} while ((disposition_parameter = disposition_parameter->next));

		zend_update_property(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"dparameters", strlen("dparameters"),
			&z_disposition_parameter_list
		);
		zval_ptr_dtor(&z_disposition_parameter_list);
	} else {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifdparameters", strlen("ifdparameters"),
			0
		);
	}
#endif

	PARAMETER *body_parameters = body->parameter;
	zval z_body_parameter_list;

	if (body_parameters) {
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifparameters", strlen("ifparameters"),
			1
		);

		array_init(&z_body_parameter_list);
		do {
			zval z_body_parameter;
			object_init(&z_body_parameter);
			zend_update_property_string(
				Z_OBJCE_P(&z_body_parameter), Z_OBJ_P(&z_body_parameter),
				"attribute", strlen("attribute"),
				body_parameters->attribute
			);
			zend_update_property_string(
				Z_OBJCE_P(&z_body_parameter), Z_OBJ_P(&z_body_parameter),
				"value", strlen("value"),
				body_parameters->value
			);

			zend_hash_next_index_insert_new(Z_ARR(z_body_parameter_list), &z_body_parameter);
		} while ((body_parameters = body_parameters->next));
	} else {
		object_init(&z_body_parameter_list);
		zend_update_property_long(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			"ifparameters", strlen("ifparameters"),
			0
		);
	}

	zend_update_property(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"parameters", strlen("parameters"),
		&z_body_parameter_list
	);
	zval_ptr_dtor(&z_body_parameter_list);
}

/* {{{ Read the structure of a specified body section of a specific message */
PHP_FUNCTION(imap_bodystruct)
{
	zval *imap_conn_obj;
	zend_long msgno;
	zend_string *section;
	php_imap_object *imap_conn_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OlS", &imap_conn_obj, php_imap_ce, &msgno, &section) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	PHP_IMAP_CHECK_MSGNO(msgno, 2);

	const BODY *body = mail_body(imap_conn_struct->imap_stream, msgno, (unsigned char*)ZSTR_VAL(section));
	if (body == NULL) {
		RETURN_FALSE;
	}

	object_init(return_value);
	php_imap_populate_body_struct_object(return_value, body);
}

/* }}} */

static void php_imap_update_property_with_full_address_str(
	zval *z_object, const char *property, size_t property_len, ADDRESS *address)
{
	ADDRESS *current = address;
	current->next = NULL;

	zend_string *full_address = _php_rfc822_write_address(current);
	if (full_address) {
		zend_update_property_str(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			property, property_len,
			full_address
		);
		zend_string_release_ex(full_address, false);
	}
}

#define UPDATE_PROPERTY_FULL_STR_ADDRESS(z_object, name, envelope) if ((envelope)->name) { php_imap_update_property_with_full_address_str((z_object), #name, strlen(#name), (envelope)->name); }

/* {{{ Read an overview of the information in the headers of the given message sequence */
PHP_FUNCTION(imap_fetch_overview)
{
	zval *imap_conn_obj;
	zend_string *sequence;
	php_imap_object *imap_conn_struct;
	zend_long status, flags = 0L;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS|l", &imap_conn_obj, php_imap_ce, &sequence, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	if (flags && ((flags & ~FT_UID) != 0)) {
		zend_argument_value_error(3, "must be FT_UID or 0");
		RETURN_THROWS();
	}

	status = (flags & FT_UID)
		? mail_uid_sequence(imap_conn_struct->imap_stream, (unsigned char*)ZSTR_VAL(sequence))
		: mail_sequence(imap_conn_struct->imap_stream, (unsigned char*)ZSTR_VAL(sequence));

	if (!status) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);
	for (unsigned long  i = 1; i <= imap_conn_struct->imap_stream->nmsgs; i++) {
		MESSAGECACHE *elt;
		ENVELOPE *env;

		elt = mail_elt(imap_conn_struct->imap_stream, i);
		if (!elt->sequence) {
			continue;
		}

		env = mail_fetch_structure(imap_conn_struct->imap_stream, i, NIL, NIL);
		if (!env) {
			continue;
		}

		// TODO Use part _php_make_header_object function?
		zval myoverview;
		object_init(&myoverview);
		if (env->subject) {
			zend_update_property_string(
				Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
				"subject", strlen("subject"),
				env->subject
			);
		}
		UPDATE_PROPERTY_FULL_STR_ADDRESS(&myoverview, from, env);
		UPDATE_PROPERTY_FULL_STR_ADDRESS(&myoverview, to, env);
		if (env->date) {
			zend_update_property_string(
				Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
				"date", strlen("date"),
				(char*)env->date
			);
		}
		if (env->message_id) {
			zend_update_property_string(
				Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
				"message_id", strlen("message_id"),
				env->message_id
			);
		}
		if (env->references) {
			zend_update_property_string(
				Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
				"references", strlen("references"),
				env->references
			);
		}
		if (env->in_reply_to) {
			zend_update_property_string(
				Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
				"in_reply_to", strlen("in_reply_to"),
				env->in_reply_to
			);
		}
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"size", strlen("size"),
			elt->rfc822_size
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"uid", strlen("uid"),
			mail_uid(imap_conn_struct->imap_stream, i)
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"msgno", strlen("msgno"),
			i
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"recent", strlen("recent"),
			elt->recent
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"flagged", strlen("flagged"),
			elt->flagged
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"answered", strlen("answered"),
			elt->answered
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"deleted", strlen("deleted"),
			elt->deleted
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"seen", strlen("seen"),
			elt->seen
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"draft", strlen("draft"),
			elt->draft
		);
		zend_update_property_long(
			Z_OBJCE_P(&myoverview), Z_OBJ_P(&myoverview),
			"udate", strlen("udate"),
			mail_longdate(elt)
		);

		zend_hash_next_index_insert_new(Z_ARR_P(return_value), &myoverview);
	}
}
/* }}} */

static bool header_injection(const zend_string *str, bool adrlist)
{
	const char *p = ZSTR_VAL(str);

	while ((p = strpbrk(p, "\r\n")) != NULL) {
		if (
			!(p[0] == '\r' && p[1] == '\n')
			/* adrlists do not support folding, but swallow trailing line breaks */
			&& !(
				(adrlist && p[1] == '\0')
				/* other headers support folding */
				|| (!adrlist && (p[1] == ' ' || p[1] == '\t'))
			)
		) {
			return true;
		}
		p++;
	}
	return false;
}

/* {{{ Create a MIME message based on given envelope and body sections */
PHP_FUNCTION(imap_mail_compose)
{
	HashTable *envelope, *body;
	zend_string *key;
	zval *data, *pvalue, *disp_data, *env_data;
	char *cookie = NIL;
	ENVELOPE *env;
	BODY *bod=NULL, *topbod=NULL;
	PART *mypart=NULL, *part;
	PARAMETER *param, *disp_param = NULL, *custom_headers_param = NULL, *tmp_param = NULL;
	char *tmp=NULL, *mystring=NULL, *t=NULL, *tempstring=NULL, *str_copy = NULL;
	int toppart = 0;
	int first;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h/h/", &envelope, &body) == FAILURE) {
		RETURN_THROWS();
	}

	if (zend_hash_num_elements(body) == 0) {
		zend_argument_value_error(2, "cannot be empty");
	}

#define CHECK_HEADER_INJECTION(zstr, adrlist, header) \
	if (header_injection(zstr, adrlist)) { \
		php_error_docref(NULL, E_WARNING, "header injection attempt in " header); \
		RETVAL_FALSE; \
		goto done; \
	}

#define PHP_RFC822_PARSE_ADRLIST(target, value) \
	str_copy = estrndup(Z_STRVAL_P(value), Z_STRLEN_P(value)); \
	rfc822_parse_adrlist(target, str_copy, "NO HOST"); \
	efree(str_copy);

	env = mail_newenvelope();
	if ((pvalue = zend_hash_str_find(envelope, "remail", sizeof("remail") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "remail");
		env->remail = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(envelope, "return_path", sizeof("return_path") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 1, "return_path");
		PHP_RFC822_PARSE_ADRLIST(&env->return_path, pvalue);
	}
	if ((pvalue = zend_hash_str_find(envelope, "date", sizeof("date") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "date");
		env->date = (unsigned char*)cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(envelope, "from", sizeof("from") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 1, "from");
		PHP_RFC822_PARSE_ADRLIST(&env->from, pvalue);
	}
	if ((pvalue = zend_hash_str_find(envelope, "reply_to", sizeof("reply_to") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 1, "reply_to");
		PHP_RFC822_PARSE_ADRLIST(&env->reply_to, pvalue);
	}
	if ((pvalue = zend_hash_str_find(envelope, "in_reply_to", sizeof("in_reply_to") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "in_reply_to");
		env->in_reply_to = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(envelope, "subject", sizeof("subject") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "subject");
		env->subject = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(envelope, "to", sizeof("to") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 1, "to");
		PHP_RFC822_PARSE_ADRLIST(&env->to, pvalue);
	}
	if ((pvalue = zend_hash_str_find(envelope, "cc", sizeof("cc") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 1, "cc");
		PHP_RFC822_PARSE_ADRLIST(&env->cc, pvalue);
	}
	if ((pvalue = zend_hash_str_find(envelope, "bcc", sizeof("bcc") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 1, "bcc");
		PHP_RFC822_PARSE_ADRLIST(&env->bcc, pvalue);
	}
	if ((pvalue = zend_hash_str_find(envelope, "message_id", sizeof("message_id") - 1)) != NULL) {
		convert_to_string(pvalue);
		CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "message_id");
		env->message_id=cpystr(Z_STRVAL_P(pvalue));
	}

	if ((pvalue = zend_hash_str_find(envelope, "custom_headers", sizeof("custom_headers") - 1)) != NULL) {
		if (Z_TYPE_P(pvalue) == IS_ARRAY) {
			custom_headers_param = tmp_param = NULL;
			SEPARATE_ARRAY(pvalue);
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pvalue), env_data) {
				custom_headers_param = mail_newbody_parameter();
				convert_to_string(env_data);
				CHECK_HEADER_INJECTION(Z_STR_P(env_data), 0, "custom_headers");
				custom_headers_param->value = (char *) fs_get(Z_STRLEN_P(env_data) + 1);
				custom_headers_param->attribute = NULL;
				memcpy(custom_headers_param->value, Z_STRVAL_P(env_data), Z_STRLEN_P(env_data) + 1);
				custom_headers_param->next = tmp_param;
				tmp_param = custom_headers_param;
			} ZEND_HASH_FOREACH_END();
		}
	}

	first = 1;
	ZEND_HASH_FOREACH_VAL(body, data) {
		if (first) {
			first = 0;

			if (Z_TYPE_P(data) != IS_ARRAY) {
				zend_argument_type_error(2, "individual body must be of type array, %s given",
					zend_zval_value_name(data));
				goto done;
			}
			if (zend_hash_num_elements(Z_ARRVAL_P(data)) == 0) {
				zend_argument_value_error(2, "individual body cannot be empty");
				goto done;
			}
			SEPARATE_ARRAY(data);

			bod = mail_newbody();
			topbod = bod;

			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type", sizeof("type") - 1)) != NULL) {
				zend_long type = zval_get_long(pvalue);
				if (type >= 0 && type <= TYPEMAX && body_types[type] != NULL) {
					bod->type = (short) type;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "encoding", sizeof("encoding") - 1)) != NULL) {
				zend_long encoding = zval_get_long(pvalue);
				if (encoding >= 0 && encoding <= ENCMAX && body_encodings[encoding] != NULL) {
					bod->encoding = (short) encoding;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "charset", sizeof("charset") - 1)) != NULL) {
				convert_to_string(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body charset");
				tmp_param = mail_newbody_parameter();
				tmp_param->value = cpystr(Z_STRVAL_P(pvalue));
				tmp_param->attribute = cpystr("CHARSET");
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type.parameters", sizeof("type.parameters") - 1)) != NULL) {
				if(Z_TYPE_P(pvalue) == IS_ARRAY && !HT_IS_PACKED(Z_ARRVAL_P(pvalue))) {
					disp_param = tmp_param = NULL;
					SEPARATE_ARRAY(pvalue);
					ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						if (key == NULL) continue;
						CHECK_HEADER_INJECTION(key, 0, "body disposition key");
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string(disp_data);
						CHECK_HEADER_INJECTION(Z_STR_P(disp_data), 0, "body disposition value");
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "subtype", sizeof("subtype") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body subtype");
				bod->subtype = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "id", sizeof("id") - 1)) != NULL) {
				convert_to_string(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body id");
				bod->id = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "description", sizeof("description") - 1)) != NULL) {
				convert_to_string(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body description");
				bod->description = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition.type", sizeof("disposition.type") - 1)) != NULL) {
				convert_to_string(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body disposition.type");
				bod->disposition.type = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition", sizeof("disposition") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY && !HT_IS_PACKED(Z_ARRVAL_P(pvalue))) {
					disp_param = tmp_param = NULL;
					SEPARATE_ARRAY(pvalue);
					ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						if (key == NULL) continue;
						CHECK_HEADER_INJECTION(key, 0, "body type.parameters key");
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string(disp_data);
						CHECK_HEADER_INJECTION(Z_STR_P(disp_data), 0, "body type.parameters value");
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->disposition.parameter = disp_param;
				}
			}
			if (bod->type == TYPEMESSAGE && bod->subtype && !strcmp(bod->subtype, "RFC822")) {
				bod->nested.msg = mail_newmsg();
			} else {
				if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "contents.data", sizeof("contents.data") - 1)) != NULL) {
					convert_to_string(pvalue);
					bod->contents.text.data = fs_get(Z_STRLEN_P(pvalue) + 1);
					memcpy(bod->contents.text.data, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
					bod->contents.text.size = Z_STRLEN_P(pvalue);
				} else {
					bod->contents.text.data = fs_get(1);
					memcpy(bod->contents.text.data, "", 1);
					bod->contents.text.size = 0;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "lines", sizeof("lines") - 1)) != NULL) {
				bod->size.lines = zval_get_long(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "bytes", sizeof("bytes") - 1)) != NULL) {
				bod->size.bytes = zval_get_long(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "md5", sizeof("md5") - 1)) != NULL) {
				convert_to_string(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body md5");
				bod->md5 = cpystr(Z_STRVAL_P(pvalue));
			}
		} else if (Z_TYPE_P(data) == IS_ARRAY && topbod->type == TYPEMULTIPART) {
			short type = 0;
			SEPARATE_ARRAY(data);
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type", sizeof("type") - 1)) != NULL) {
				zend_long tmp_type = zval_get_long(pvalue);
				if (tmp_type >= 0 && tmp_type <= TYPEMAX && tmp_type != TYPEMULTIPART && body_types[tmp_type] != NULL) {
					type = (short) tmp_type;
				}
			}

			if (!toppart) {
				bod->nested.part = mail_newbody_part();
				mypart = bod->nested.part;
				toppart = 1;
			} else {
				mypart->next = mail_newbody_part();
				mypart = mypart->next;
			}

			bod = &mypart->body;
			bod->type = type;

			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "encoding", sizeof("encoding") - 1)) != NULL) {
				zend_long encoding = zval_get_long(pvalue);
				if (encoding >= 0 && encoding <= ENCMAX && body_encodings[encoding] != NULL) {
					bod->encoding = (short) encoding;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "charset", sizeof("charset") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body charset");
				tmp_param = mail_newbody_parameter();
				tmp_param->value = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(tmp_param->value, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue) + 1);
				tmp_param->attribute = cpystr("CHARSET");
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type.parameters", sizeof("type.parameters") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY && !HT_IS_PACKED(Z_ARRVAL_P(pvalue))) {
					disp_param = tmp_param = NULL;
					SEPARATE_ARRAY(pvalue);
					ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						if (key == NULL) continue;
						CHECK_HEADER_INJECTION(key, 0, "body type.parameters key");
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						CHECK_HEADER_INJECTION(Z_STR_P(disp_data), 0, "body type.parameters value");
						disp_param->value = (char *)fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "subtype", sizeof("subtype") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body subtype");
				bod->subtype = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "id", sizeof("id") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body id");
				bod->id = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "description", sizeof("description") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body description");
				bod->description = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition.type", sizeof("disposition.type") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body disposition.type");
				bod->disposition.type = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition", sizeof("disposition") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY && !HT_IS_PACKED(Z_ARRVAL_P(pvalue))) {
					disp_param = tmp_param = NULL;
					SEPARATE_ARRAY(pvalue);
					ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						if (key == NULL) continue;
						CHECK_HEADER_INJECTION(key, 0, "body disposition key");
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						CHECK_HEADER_INJECTION(Z_STR_P(disp_data), 0, "body disposition value");
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->disposition.parameter = disp_param;
				}
			}
			if (bod->type == TYPEMESSAGE && bod->subtype && !strcmp(bod->subtype, "RFC822")) {
				bod->nested.msg = mail_newmsg();
			} else {
				if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "contents.data", sizeof("contents.data") - 1)) != NULL) {
					convert_to_string(pvalue);
					bod->contents.text.data = fs_get(Z_STRLEN_P(pvalue) + 1);
					memcpy(bod->contents.text.data, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue) + 1);
					bod->contents.text.size = Z_STRLEN_P(pvalue);
				} else {
					bod->contents.text.data = fs_get(1);
					memcpy(bod->contents.text.data, "", 1);
					bod->contents.text.size = 0;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "lines", sizeof("lines") - 1)) != NULL) {
				bod->size.lines = zval_get_long(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "bytes", sizeof("bytes") - 1)) != NULL) {
				bod->size.bytes = zval_get_long(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "md5", sizeof("md5") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				CHECK_HEADER_INJECTION(Z_STR_P(pvalue), 0, "body md5");
				bod->md5 = cpystr(Z_STRVAL_P(pvalue));
			}
		}
	} ZEND_HASH_FOREACH_END();

	if (bod && bod->type == TYPEMULTIPART && (!bod->nested.part || !bod->nested.part->next)) {
		php_error_docref(NULL, E_WARNING, "Cannot generate multipart e-mail without components.");
		RETVAL_FALSE;
		goto done;
	}

	rfc822_encode_body_7bit(env, topbod);

	tmp = emalloc(SENDBUFLEN + 1);

	rfc822_header(tmp, env, topbod);

	/* add custom envelope headers */
	if (custom_headers_param) {
		int l = strlen(tmp) - 2, l2;
		PARAMETER *tp = custom_headers_param;

		/* remove last CRLF from tmp */
		tmp[l] = '\0';
		tempstring = emalloc(l);
		memcpy(tempstring, tmp, l);

		do {
			l2 = strlen(custom_headers_param->value);
			tempstring = erealloc(tempstring, l + l2 + CRLF_LEN + 1);
			memcpy(tempstring + l, custom_headers_param->value, l2);
			memcpy(tempstring + l + l2, CRLF, CRLF_LEN);
			l += l2 + CRLF_LEN;
		} while ((custom_headers_param = custom_headers_param->next));

		mail_free_body_parameter(&tp);

		mystring = emalloc(l + CRLF_LEN + 1);
		memcpy(mystring, tempstring, l);
		memcpy(mystring + l , CRLF, CRLF_LEN);
		mystring[l + CRLF_LEN] = '\0';

		efree(tempstring);
	} else {
		mystring = estrdup(tmp);
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
			} else if (strlen(cookie) > (SENDBUFLEN - 2 - 2 - 2)) { /* validate cookie length -- + CRLF * 2 */
				php_error_docref(NULL, E_WARNING, "The boundary should be no longer than 4kb");
				RETVAL_FALSE;
				goto done;
			}

		/* for each part */
			do {
				t = tmp;

			/* append mini-header */
				*t = '\0';
				rfc822_write_body_header(&t, &part->body);

			/* output cookie, mini-header, and contents */
				spprintf(&tempstring, 0, "%s--%s%s%s%s", mystring, cookie, CRLF, tmp, CRLF);
				efree(mystring);
				mystring=tempstring;

				bod=&part->body;

				spprintf(&tempstring, 0, "%s%s%s", mystring, bod->contents.text.data ? (char *) bod->contents.text.data : "", CRLF);
				efree(mystring);
				mystring=tempstring;
			} while ((part = part->next)); /* until done */

			/* output trailing cookie */
			spprintf(&tempstring, 0, "%s--%s--%s", mystring, cookie, CRLF);
			efree(mystring);
			mystring=tempstring;
	} else if (bod) {
		spprintf(&tempstring, 0, "%s%s%s", mystring, bod->contents.text.data ? (char *) bod->contents.text.data : "", CRLF);
		efree(mystring);
		mystring=tempstring;
	} else {
		efree(mystring);
		RETVAL_FALSE;
		goto done;
	}

	RETVAL_STRING(tempstring);
	efree(tempstring);
done:
	if (tmp) {
		efree(tmp);
	}
	mail_free_body(&topbod);
	mail_free_envelope(&env);
}
/* }}} */

/* {{{ _php_imap_mail */
bool _php_imap_mail(zend_string *to, zend_string *subject, zend_string *message, zend_string *headers,
	zend_string *cc, zend_string *bcc, zend_string* rpath)
{
#ifdef PHP_WIN32
	int tsm_err;
#else
	FILE *sendmail;
	int ret;
#endif

	ZEND_ASSERT(to && ZSTR_LEN(to) != 0);
	ZEND_ASSERT(subject && ZSTR_LEN(subject) != 0);
	ZEND_ASSERT(message);

#ifdef PHP_WIN32
	char *tempMailTo;
	char *tsm_errmsg = NULL;
	ADDRESS *addr;
	char *bufferTo = NULL, *bufferCc = NULL, *bufferBcc = NULL, *bufferHeader = NULL;
	size_t offset, bufferLen = 0;
	size_t bt_len;

	/* Add "To" field's necessary buffer length */
	bufferLen += ZSTR_LEN(to) + 6;
	if (headers) {
		bufferLen += ZSTR_LEN(headers);
	}
	if (cc) {
		bufferLen += ZSTR_LEN(cc) + 6;
	}

#define PHP_IMAP_CLEAN	if (bufferTo) efree(bufferTo); if (bufferCc) efree(bufferCc); if (bufferBcc) efree(bufferBcc); if (bufferHeader) efree(bufferHeader);
#define PHP_IMAP_BAD_DEST PHP_IMAP_CLEAN; efree(tempMailTo); return (BAD_MSG_DESTINATION);

	bufferHeader = (char *)safe_emalloc(bufferLen, 1, 1);
	memset(bufferHeader, 0, bufferLen);

	/* Handle "To" Field */
	strlcat(bufferHeader, "To: ", bufferLen + 1);
	strlcat(bufferHeader, ZSTR_VAL(to), bufferLen + 1);
	strlcat(bufferHeader, "\r\n", bufferLen + 1);
	tempMailTo = estrdup(ZSTR_VAL(to));
	bt_len = ZSTR_LEN(to);
	bufferTo = (char *)safe_emalloc(bt_len, 1, 1);
	bt_len++;
	offset = 0;
	addr = NULL;
	rfc822_parse_adrlist(&addr, tempMailTo, "NO HOST");
	while (addr) {
		if (addr->host == NULL || strcmp(addr->host, ERRHOST) == 0) {
			PHP_IMAP_BAD_DEST;
		} else {
			bufferTo = safe_erealloc(bufferTo, bt_len, 1, strlen(addr->mailbox));
			bt_len += strlen(addr->mailbox);
			bufferTo = safe_erealloc(bufferTo, bt_len, 1, strlen(addr->host));
			bt_len += strlen(addr->host);
			offset += slprintf(bufferTo + offset, bt_len - offset, "%s@%s,", addr->mailbox, addr->host);
		}
		addr = addr->next;
	}
	efree(tempMailTo);
	if (offset>0) {
		bufferTo[offset-1] = 0;
	}

	if (cc && ZSTR_LEN(cc) != 0) {
		strlcat(bufferHeader, "Cc: ", bufferLen + 1);
		strlcat(bufferHeader, ZSTR_VAL(cc), bufferLen + 1);
		strlcat(bufferHeader, "\r\n", bufferLen + 1);
		tempMailTo = estrdup(ZSTR_VAL(cc));
		bt_len = ZSTR_LEN(cc);
		bufferCc = (char *)safe_emalloc(bt_len, 1, 1);
		bt_len++;
		offset = 0;
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, "NO HOST");
		while (addr) {
			if (addr->host == NULL || strcmp(addr->host, ERRHOST) == 0) {
				PHP_IMAP_BAD_DEST;
			} else {
				bufferCc = safe_erealloc(bufferCc, bt_len, 1, strlen(addr->mailbox));
				bt_len += strlen(addr->mailbox);
				bufferCc = safe_erealloc(bufferCc, bt_len, 1, strlen(addr->host));
				bt_len += strlen(addr->host);
				offset += slprintf(bufferCc + offset, bt_len - offset, "%s@%s,", addr->mailbox, addr->host);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
		if (offset>0) {
			bufferCc[offset-1] = 0;
		}
	}

	if (bcc && ZSTR_LEN(bcc)) {
		tempMailTo = estrdup(ZSTR_VAL(bcc));
		bt_len = ZSTR_LEN(bcc);
		bufferBcc = (char *)safe_emalloc(bt_len, 1, 1);
		bt_len++;
		offset = 0;
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, "NO HOST");
		while (addr) {
			if (addr->host == NULL || strcmp(addr->host, ERRHOST) == 0) {
				PHP_IMAP_BAD_DEST;
			} else {
				bufferBcc = safe_erealloc(bufferBcc, bt_len, 1, strlen(addr->mailbox));
				bt_len += strlen(addr->mailbox);
				bufferBcc = safe_erealloc(bufferBcc, bt_len, 1, strlen(addr->host));
				bt_len += strlen(addr->host);
				offset += slprintf(bufferBcc + offset, bt_len - offset, "%s@%s,", addr->mailbox, addr->host);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
		if (offset>0) {
			bufferBcc[offset-1] = 0;
		}
	}

	if (headers && ZSTR_LEN(headers)) {
		strlcat(bufferHeader, ZSTR_VAL(headers), bufferLen + 1);
	}

	if (TSendMail(INI_STR("SMTP"), &tsm_err, &tsm_errmsg, bufferHeader, ZSTR_VAL(subject),
			bufferTo, ZSTR_VAL(message), bufferCc, bufferBcc, rpath ? ZSTR_VAL(rpath) : NULL) != SUCCESS) {
		if (tsm_errmsg) {
			php_error_docref(NULL, E_WARNING, "%s", tsm_errmsg);
			efree(tsm_errmsg);
		} else {
			php_error_docref(NULL, E_WARNING, "%s", GetSMErrorText(tsm_err));
		}
		PHP_IMAP_CLEAN;
		return 0;
	}
	PHP_IMAP_CLEAN;
#else
	if (!INI_STR("sendmail_path")) {
		return 0;
	}
	sendmail = popen(INI_STR("sendmail_path"), "w");
	if (sendmail) {
		if (rpath && ZSTR_LEN(rpath) != 0) {
			fprintf(sendmail, "From: %s\n", ZSTR_VAL(rpath));
		}
		/* to cannot be a null pointer, asserted earlier on */
		fprintf(sendmail, "To: %s\n", ZSTR_VAL(to));
		if (cc && ZSTR_LEN(cc) != 0) {
			fprintf(sendmail, "Cc: %s\n", ZSTR_VAL(cc));
		}
		if (bcc && ZSTR_LEN(bcc) != 0) {
			fprintf(sendmail, "Bcc: %s\n", ZSTR_VAL(bcc));
		}
		/* subject cannot be a null pointer, asserted earlier on */
		fprintf(sendmail, "Subject: %s\n", ZSTR_VAL(subject));
		if (headers && ZSTR_LEN(headers) != 0) {
			fprintf(sendmail, "%s\n", ZSTR_VAL(headers));
		}
		/* message cannot be a null pointer, asserted earlier on */
		fprintf(sendmail, "\n%s\n", ZSTR_VAL(message));
		ret = pclose(sendmail);

		return ret != -1;
	} else {
		php_error_docref(NULL, E_WARNING, "Could not execute mail delivery program");
		return 0;
	}
#endif
	return 1;
}
/* }}} */

/* {{{ Send an email message */
PHP_FUNCTION(imap_mail)
{
	zend_string *to=NULL, *message=NULL, *headers=NULL, *subject=NULL, *cc=NULL, *bcc=NULL, *rpath=NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "PPP|P!P!P!P!", &to, &subject, &message,
		&headers, &cc, &bcc, &rpath) == FAILURE) {
		RETURN_THROWS();
	}

	/* To: */
	if (ZSTR_LEN(to) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	/* Subject: */
	if (ZSTR_LEN(subject) == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	/* message body */
	if (ZSTR_LEN(message) == 0) {
		/* this is not really an error, so it is allowed. */
		// TODO Drop warning or emit ValueError?
		php_error_docref(NULL, E_WARNING, "No message string in mail command");
	}

	if (_php_imap_mail(to, subject, message, headers, cc, bcc, rpath)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Return a list of messages matching the given criteria */
PHP_FUNCTION(imap_search)
{
	zval *imap_conn_obj;
	zend_string *criteria, *charset = NULL;
	zend_long flags = SE_FREE;
	php_imap_object *imap_conn_struct;
	char *search_criteria;
	MESSAGELIST *cur;
	SEARCHPGM *pgm = NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS|lS", &imap_conn_obj, php_imap_ce, &criteria, &flags, &charset) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* TODO Update docs to allow SE_FREE as an option */
	if (flags && ((flags & ~(SE_FREE | SE_UID)) != 0)) {
		zend_argument_value_error(3, "must be a bitmask of SE_FREE, and SE_UID");
		RETURN_THROWS();
	}

	search_criteria = estrndup(ZSTR_VAL(criteria), ZSTR_LEN(criteria));

	IMAPG(imap_messages) = IMAPG(imap_messages_tail) = NIL;
	pgm = mail_criteria(search_criteria);

	mail_search_full(imap_conn_struct->imap_stream, (charset ? ZSTR_VAL(charset) : NIL), pgm, flags);

	if (pgm && !(flags & SE_FREE)) {
		mail_free_searchpgm(&pgm);
	}

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

/* {{{ Returns an array of all IMAP alerts that have been generated since the last page load or since the last imap_alerts() call, whichever came last. The alert stack is cleared after imap_alerts() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_alerts)
{
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (IMAPG(imap_alertstack) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	cur = IMAPG(imap_alertstack);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur = cur->next;
	}
	mail_free_stringlist(&IMAPG(imap_alertstack));
	IMAPG(imap_alertstack) = NIL;
}
/* }}} */

/* {{{ Returns an array of all IMAP errors generated since the last page load, or since the last imap_errors() call, whichever came last. The error stack is cleared after imap_errors() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_errors)
{
	ERRORLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (IMAPG(imap_errorstack) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	cur = IMAPG(imap_errorstack);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur = cur->next;
	}
	mail_free_errorlist(&IMAPG(imap_errorstack));
	IMAPG(imap_errorstack) = NIL;
}
/* }}} */

/* {{{ Returns the last error that was generated by an IMAP function. The error stack is NOT cleared after this call. */
/* Author: CJH */
PHP_FUNCTION(imap_last_error)
{
	ERRORLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (IMAPG(imap_errorstack) == NIL) {
		RETURN_FALSE;
	}

	cur = IMAPG(imap_errorstack);
	while (cur != NIL) {
		if (cur->next == NIL) {
			RETURN_STRING((char*)cur->LTEXT);
		}
		cur = cur->next;
	}
}
/* }}} */

static void php_imap_populate_mime_header_object(zval *z_object, const char *charset, const char *content)
{
	ZEND_ASSERT(z_object && "Object must be initialized");
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"charset", strlen("charset"),
		charset
	);
	zend_update_property_string(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		"text", strlen("text"),
		content
	);
}

/* {{{ Decode mime header element in accordance with RFC 2047 and return array of objects containing 'charset' encoding and decoded 'text' */
PHP_FUNCTION(imap_mime_header_decode)
{
	/* Author: Ted Parnefors <ted@mtv.se> */
	zval myobject;
	zend_string *str;
	char *string, *charset, encoding, *text, *decode;
	zend_long charset_token, encoding_token, end_token, end, offset=0, i;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	string = ZSTR_VAL(str);
	end = ZSTR_LEN(str);

	charset = (char *) safe_emalloc((end + 1), 2, 0);
	text = &charset[end + 1];
	while (offset < end) {	/* Reached end of the string? */
		if ((charset_token = (zend_long)php_memnstr(&string[offset], "=?", 2, string + end))) {	/* Is there anything encoded in the string? */
			charset_token -= (zend_long)string;
			if (offset != charset_token) {	/* Is there anything before the encoded data? */
				/* Retrieve unencoded data that is found before encoded data */
				memcpy(text, &string[offset], charset_token-offset);
				text[charset_token - offset] = 0x00;
				object_init(&myobject);
				php_imap_populate_mime_header_object(&myobject, "default", text);
				zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &myobject);
			}
			if ((encoding_token = (zend_long)php_memnstr(&string[charset_token+2], "?", 1, string+end))) {		/* Find token for encoding */
				encoding_token -= (zend_long)string;
				if ((end_token = (zend_long)php_memnstr(&string[encoding_token+3], "?=", 2, string+end))) {	/* Find token for end of encoded data */
					end_token -= (zend_long)string;
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
						zend_array_destroy(Z_ARR_P(return_value));
						RETURN_FALSE;
					}
					object_init(&myobject);
					php_imap_populate_mime_header_object(&myobject, charset, decode);
					zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &myobject);

					/* only free decode if it was allocated by rfc822_qprint or rfc822_base64 */
					if (decode != text) {
						fs_give((void**)&decode);
					}

					offset = end_token+2;
					for (i = 0; (string[offset + i] == ' ') || (string[offset + i] == 0x0a) || (string[offset + i] == 0x0d) || (string[offset + i] == '\t'); i++);
					if ((string[offset + i] == '=') && (string[offset + i + 1] == '?') && (offset + i < end)) {
						offset += i;
					}
					continue;	/*/ Iterate the loop again please. */
				}
			}
		} else {
			/* Just some tweaking to optimize the code, and get the end statements work in a general manner.
			 * If we end up here we didn't find a position for "charset_token",
			 * so we need to set it to the start of the yet unextracted data.
			 */
			charset_token = offset;
		}
		/* Return the rest of the data as unencoded, as it was either unencoded or was missing separators
		   which rendered the remainder of the string impossible for us to decode. */
		memcpy(text, &string[charset_token], end - charset_token);	/* Extract unencoded text from string */
		text[end - charset_token] = 0x00;
		object_init(&myobject);
		php_imap_populate_mime_header_object(&myobject, "default", text);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &myobject);

		offset = end;	/* We have reached the end of the string. */
	}
	efree(charset);
}
/* }}} */

/* Support Functions */

#ifdef HAVE_RFC822_OUTPUT_ADDRESS_LIST
/* {{{ _php_rfc822_soutr */
static long _php_rfc822_soutr (void *stream, char *string)
{
	smart_str *ret = (smart_str*)stream;
	size_t len = strlen(string);

	smart_str_appendl(ret, string, len);
	return LONGT;
}
/* }}} */

/* {{{ _php_rfc822_write_address */
static zend_string* _php_rfc822_write_address(ADDRESS *addresslist)
{
	char address[MAILTMPLEN];
	smart_str ret = {0};
	RFC822BUFFER buf;

	buf.beg = address;
	buf.cur = buf.beg;
	buf.end = buf.beg + sizeof(address) - 1;
	buf.s = &ret;
	buf.f = _php_rfc822_soutr;
	rfc822_output_address_list(&buf, addresslist, 0, NULL);
	rfc822_output_flush(&buf);
	smart_str_0(&ret);
	return ret.s;
}
/* }}} */

#else

/* Calculate string length based on imap's rfc822_cat function. */
static size_t _php_rfc822_len(const char *const str)
{
	/* Non existent or empty string */
	if (!str || !*str) {
		return 0;
	}

	/* strings with special characters will need to be quoted, as a safety measure we
	 * add 2 bytes for the quotes just in case.
	 */
	size_t len = strlen(str) + 2;

	/* rfc822_cat() will escape all " and \ characters, therefore we need to increase
	 * our buffer length to account for these characters.
	 */
	const char *p = str;
	while ((p = strpbrk(p, "\\\""))) {
		p++;
		len++;
	}

	return len;
}

static size_t _php_imap_address_size(const ADDRESS *const address_list)
{
	size_t total_size = 0;
	unsigned int nb_addresses = 0;
	const ADDRESS *current_address = address_list;

	if (current_address) do {
		total_size += _php_rfc822_len(current_address->personal);
		total_size += _php_rfc822_len(current_address->adl);
		total_size += _php_rfc822_len(current_address->mailbox);
		total_size += _php_rfc822_len(current_address->host);
		nb_addresses++;
	} while ((current_address = current_address->next));

	/*
	 * rfc822_write_address_full() needs some extra space for '<>,', etc.
	 * for this purpose we allocate additional PHP_IMAP_ADDRESS_SIZE_BUF bytes
	 * by default this buffer is 10 bytes long
	 */
	total_size += nb_addresses * PHP_IMAP_ADDRESS_SIZE_BUF;

	return total_size;
}

/* {{{ _php_rfc822_write_address */
static zend_string* _php_rfc822_write_address(ADDRESS *addresslist)
{
	char address[SENDBUFLEN];

	if (_php_imap_address_size(addresslist) >= SENDBUFLEN) {
		zend_throw_error(NULL, "Address buffer overflow");
		return NULL;
	}
	address[0] = 0;
	rfc822_write_address(address, addresslist);
	return zend_string_init(address, strlen(address), 0);
}
/* }}} */
#endif
/* {{{ _php_imap_parse_address */
static zend_string* _php_imap_parse_address (ADDRESS *address_list, zval *paddress)
{
	zend_string *fulladdress;
	ADDRESS *addresstmp;

	addresstmp = address_list;

	fulladdress = _php_rfc822_write_address(addresstmp);

	php_imap_construct_list_of_addresses(Z_ARR_P(paddress), address_list);
	return fulladdress;
}
/* }}} */

static void php_imap_update_property_with_parsed_full_address(
	zval *z_object, ADDRESS *address,
	const char *str_address_property, size_t str_address_property_len,
	const char *parts_address_property, size_t parts_address_property_len
)
{
	zval address_parts;
	zend_string *full_address = NULL;

	array_init(&address_parts);
	full_address = _php_imap_parse_address(address, &address_parts);
	if (full_address) {
		zend_update_property_str(
			Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
			str_address_property, str_address_property_len,
			full_address
		);
		zend_string_release_ex(full_address, false);
	}
	zend_update_property(
		Z_OBJCE_P(z_object), Z_OBJ_P(z_object),
		parts_address_property, parts_address_property_len,
		&address_parts
	);
	zval_ptr_dtor(&address_parts);
}

#define UPDATE_PROPERTY_PARSED_ADDRESS(z_object, name, envelope) if ((envelope)->name) { \
	php_imap_update_property_with_parsed_full_address( \
		(z_object), (envelope)->name, \
		#name "address", strlen(#name "address"), \
		#name, strlen(#name) \
	); }

/* {{{ _php_make_header_object */
static void _php_make_header_object(zval *myzvalue, ENVELOPE *en)
{
	object_init(myzvalue);

	if (en->remail) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"remail", strlen("remail"),
			en->remail
		);
	}
	if (en->date) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"date", strlen("date"),
			(char*)en->date
		);
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"Date", strlen("Date"),
			(char*)en->date
		);
	}
	if (en->subject) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"subject", strlen("subject"),
			en->subject
		);
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"Subject", strlen("Subject"),
			en->subject
		);
	}
	if (en->in_reply_to) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"in_reply_to", strlen("in_reply_to"),
			en->in_reply_to
		);
	}
	if (en->message_id) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"message_id", strlen("message_id"),
			en->message_id
		);
	}
	if (en->newsgroups) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"newsgroups", strlen("newsgroups"),
			en->newsgroups
		);
	}
	if (en->followup_to) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"followup_to", strlen("followup_to"),
			en->followup_to
		);
	}
	if (en->references) {
		zend_update_property_string(
			Z_OBJCE_P(myzvalue), Z_OBJ_P(myzvalue),
			"references", strlen("references"),
			en->references
		);
	}

	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, to, en);
	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, from, en);
	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, cc, en);
	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, bcc, en);
	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, reply_to, en);
	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, sender, en);
	UPDATE_PROPERTY_PARSED_ADDRESS(myzvalue, return_path, en);
}
/* }}} */

/* {{{ _php_imap_add_body */
void _php_imap_add_body(zval *arg, const BODY *body)
{
	php_imap_populate_body_struct_object(arg, body);

	/* multipart message ? */
	if (body->type == TYPEMULTIPART) {
		zval z_content_part_list;

		array_init(&z_content_part_list);
		for (const PART *content_part = body->CONTENT_PART; content_part; content_part = content_part->next) {
			zval z_content_part;
			object_init(&z_content_part);
			_php_imap_add_body(&z_content_part, &content_part->body);
			zend_hash_next_index_insert_new(Z_ARR(z_content_part_list), &z_content_part);
		}

		zend_update_property(
			Z_OBJCE_P(arg), Z_OBJ_P(arg),
			"parts", strlen("parts"),
			&z_content_part_list
		);
		zval_ptr_dtor(&z_content_part_list);
	}

	/* encapsulated message ? */
	if ((body->type == TYPEMESSAGE) && (!strcasecmp(body->subtype, "rfc822"))) {
		zval message_list, message;

		const BODY *message_body = body->CONTENT_MSG_BODY;
		array_init(&message_list);
		object_init(&message);
		_php_imap_add_body(&message, message_body);
		zend_hash_next_index_insert_new(Z_ARR(message_list), &message);

		zend_update_property(
			Z_OBJCE_P(arg), Z_OBJ_P(arg),
			"parts", strlen("parts"),
			&message_list
		);
		zval_ptr_dtor(&message_list);
	}
}
/* }}} */

/* imap_thread, stealing this from header cclient -rjs3 */
/* {{{ build_thread_tree_helper */
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

/* {{{ build_thread_tree */
static int build_thread_tree(THREADNODE *top, zval **tree)
{
	long numNodes = 0;
	char buf[25];

	array_init(*tree);

	build_thread_tree_helper(top, *tree, &numNodes, buf);

	return SUCCESS;
}
/* }}} */

/* {{{ Return threaded by REFERENCES tree */
PHP_FUNCTION(imap_thread)
{
	zval *imap_conn_obj;
	php_imap_object *imap_conn_struct;
	zend_long flags = SE_FREE;
	char criteria[] = "ALL";
	THREADNODE *top;
	SEARCHPGM *pgm = NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &imap_conn_obj, php_imap_ce, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	GET_IMAP_STREAM(imap_conn_struct, imap_conn_obj);

	/* TODO Check if flags are valid (documentation is not present on php.net so need to check this first) */

	pgm = mail_criteria(criteria);
	top = mail_thread(imap_conn_struct->imap_stream, "REFERENCES", NIL, pgm, flags);
	if (pgm && !(flags & SE_FREE)) {
		mail_free_searchpgm(&pgm);
	}

	if (top == NIL) {
		php_error_docref(NULL, E_WARNING, "Function returned an empty tree");
		RETURN_FALSE;
	}

	/* Populate our return value data structure here. */
	if (build_thread_tree(top, &return_value) == FAILURE) {
		mail_free_threadnode(&top);
		RETURN_FALSE;
	}
	mail_free_threadnode(&top);
}
/* }}} */

/* {{{ Set or fetch imap timeout */
PHP_FUNCTION(imap_timeout)
{
	zend_long ttype, timeout=-1;
	int timeout_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &ttype, &timeout) == FAILURE) {
		RETURN_THROWS();
	}

	if (timeout == -1) {
		switch (ttype) {
			case 1:
				timeout_type = GET_OPENTIMEOUT;
				break;
			case 2:
				timeout_type = GET_READTIMEOUT;
				break;
			case 3:
				timeout_type = GET_WRITETIMEOUT;
				break;
			case 4:
				timeout_type = GET_CLOSETIMEOUT;
				break;
			default:
				RETURN_FALSE;
				break;
		}

		timeout = (zend_long) mail_parameters(NIL, timeout_type, NIL);
		RETURN_LONG(timeout);
	} else if (timeout >= 0) {
		switch (ttype) {
			case 1:
				timeout_type = SET_OPENTIMEOUT;
				break;
			case 2:
				timeout_type = SET_READTIMEOUT;
				break;
			case 3:
				timeout_type = SET_WRITETIMEOUT;
				break;
			case 4:
				timeout_type = SET_CLOSETIMEOUT;
				break;
			default:
				RETURN_FALSE;
				break;
		}

		timeout = (zend_long) mail_parameters(NIL, timeout_type, (void *) timeout);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#define GETS_FETCH_SIZE 8196LU
static char *php_mail_gets(readfn_t f, void *stream, unsigned long size, GETS_DATA *md) /* {{{ */
{

	/*	write to the gets stream if it is set,
		otherwise forward to c-clients gets */
	if (IMAPG(gets_stream)) {
		char buf[GETS_FETCH_SIZE];

		while (size) {
			unsigned long read;

			if (size > GETS_FETCH_SIZE) {
				read = GETS_FETCH_SIZE;
				size -=GETS_FETCH_SIZE;
			} else {
				read = size;
				size = 0;
			}

			if (!f(stream, read, buf)) {
				php_error_docref(NULL, E_WARNING, "Failed to read from socket");
				break;
			} else if (read != php_stream_write(IMAPG(gets_stream), buf, read)) {
				php_error_docref(NULL, E_WARNING, "Failed to write to stream");
				break;
			}
		}
		return NULL;
	} else {
		char *buf = pemalloc(size + 1, 1);

		if (f(stream, size, buf)) {
			buf[size] = '\0';
		} else {
			php_error_docref(NULL, E_WARNING, "Failed to read from socket");
			free(buf);
			buf = NULL;
		}
		return buf;
	}
}
/* }}} */

/* {{{ Interfaces to C-client */
PHP_IMAP_EXPORT void mm_searched(MAILSTREAM *stream, unsigned long number)
{
	MESSAGELIST *cur = NIL;

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

PHP_IMAP_EXPORT void mm_exists(MAILSTREAM *stream, unsigned long number)
{
}

PHP_IMAP_EXPORT void mm_expunged(MAILSTREAM *stream, unsigned long number)
{
}

PHP_IMAP_EXPORT void mm_flags(MAILSTREAM *stream, unsigned long number)
{
}

/* Author: CJH */
PHP_IMAP_EXPORT void mm_notify(MAILSTREAM *stream, char *str, long errflg)
{
	STRINGLIST *cur = NIL;

	if (strncmp(str, "[ALERT] ", 8) == 0) {
		if (IMAPG(imap_alertstack) == NIL) {
			IMAPG(imap_alertstack) = mail_newstringlist();
			IMAPG(imap_alertstack)->LSIZE = strlen((char*)(IMAPG(imap_alertstack)->LTEXT = (unsigned char*)cpystr(str)));
			IMAPG(imap_alertstack)->next = NIL;
		} else {
			cur = IMAPG(imap_alertstack);
			while (cur->next != NIL) {
				cur = cur->next;
			}
			cur->next = mail_newstringlist ();
			cur = cur->next;
			cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(str)));
			cur->next = NIL;
		}
	}
}

PHP_IMAP_EXPORT void mm_list(MAILSTREAM *stream, DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;

	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build up a the new array of objects */
		/* Author: CJH */
		if (IMAPG(imap_folder_objects) == NIL) {
			IMAPG(imap_folder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_folder_objects)->LSIZE=strlen((char*)(IMAPG(imap_folder_objects)->LTEXT = (unsigned char*)cpystr(mailbox)));
			IMAPG(imap_folder_objects)->delimiter = delimiter;
			IMAPG(imap_folder_objects)->attributes = attributes;
			IMAPG(imap_folder_objects)->next = NIL;
			IMAPG(imap_folder_objects_tail) = IMAPG(imap_folder_objects);
		} else {
			ocur=IMAPG(imap_folder_objects_tail);
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE = strlen((char*)(ocur->LTEXT = (unsigned char*)cpystr(mailbox)));
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
				IMAPG(imap_folders)->LSIZE=strlen((char*)(IMAPG(imap_folders)->LTEXT = (unsigned char*)cpystr(mailbox)));
				IMAPG(imap_folders)->next=NIL;
				IMAPG(imap_folders_tail) = IMAPG(imap_folders);
			} else {
				cur=IMAPG(imap_folders_tail);
				cur->next=mail_newstringlist ();
				cur=cur->next;
				cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(mailbox)));
				cur->next = NIL;
				IMAPG(imap_folders_tail) = cur;
			}
		}
	}
}

PHP_IMAP_EXPORT void mm_lsub(MAILSTREAM *stream, DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;

	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build the array of objects */
		/* Author: CJH */
		if (IMAPG(imap_sfolder_objects) == NIL) {
			IMAPG(imap_sfolder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_sfolder_objects)->LSIZE = strlen((char*)(IMAPG(imap_sfolder_objects)->LTEXT = (unsigned char*)cpystr(mailbox)));
			IMAPG(imap_sfolder_objects)->delimiter = delimiter;
			IMAPG(imap_sfolder_objects)->attributes = attributes;
			IMAPG(imap_sfolder_objects)->next = NIL;
			IMAPG(imap_sfolder_objects_tail) = IMAPG(imap_sfolder_objects);
		} else {
			ocur=IMAPG(imap_sfolder_objects_tail);
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE=strlen((char*)(ocur->LTEXT = (unsigned char*)cpystr(mailbox)));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
			IMAPG(imap_sfolder_objects_tail) = ocur;
		}
	} else {
		/* build the old simple array for imap_listsubscribed() */
		if (IMAPG(imap_sfolders) == NIL) {
			IMAPG(imap_sfolders)=mail_newstringlist();
			IMAPG(imap_sfolders)->LSIZE=strlen((char*)(IMAPG(imap_sfolders)->LTEXT = (unsigned char*)cpystr(mailbox)));
			IMAPG(imap_sfolders)->next=NIL;
			IMAPG(imap_sfolders_tail) = IMAPG(imap_sfolders);
		} else {
			cur=IMAPG(imap_sfolders_tail);
			cur->next=mail_newstringlist ();
			cur=cur->next;
			cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(mailbox)));
			cur->next = NIL;
			IMAPG(imap_sfolders_tail) = cur;
		}
	}
}

PHP_IMAP_EXPORT void mm_status(MAILSTREAM *stream, char *mailbox, MAILSTATUS *status)
{

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

PHP_IMAP_EXPORT void mm_log(char *str, long errflg)
{
	ERRORLIST *cur = NIL;

	/* Author: CJH */
	if (errflg != NIL) { /* CJH: maybe put these into a more comprehensive log for debugging purposes? */
		if (IMAPG(imap_errorstack) == NIL) {
			IMAPG(imap_errorstack) = mail_newerrorlist();
			IMAPG(imap_errorstack)->LSIZE = strlen((char*)(IMAPG(imap_errorstack)->LTEXT = (unsigned char*)cpystr(str)));
			IMAPG(imap_errorstack)->errflg = errflg;
			IMAPG(imap_errorstack)->next = NIL;
		} else {
			cur = IMAPG(imap_errorstack);
			while (cur->next != NIL) {
				cur = cur->next;
			}
			cur->next = mail_newerrorlist();
			cur = cur->next;
			cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(str)));
			cur->errflg = errflg;
			cur->next = NIL;
		}
	}
}

PHP_IMAP_EXPORT void mm_dlog(char *str)
{
	/* CJH: this is for debugging; it might be useful to allow setting
	   the stream to debug mode and capturing this somewhere - syslog?
	   php debugger? */
}

PHP_IMAP_EXPORT void mm_login(NETMBX *mb, char *user, char *pwd, long trial)
{

	if (*mb->user) {
		strlcpy (user, mb->user, MAILTMPLEN);
	} else {
		strlcpy (user, IMAPG(imap_user), MAILTMPLEN);
	}
	strlcpy (pwd, IMAPG(imap_password), MAILTMPLEN);
}

PHP_IMAP_EXPORT void mm_critical(MAILSTREAM *stream)
{
}

PHP_IMAP_EXPORT void mm_nocritical(MAILSTREAM *stream)
{
}

PHP_IMAP_EXPORT long mm_diskerror(MAILSTREAM *stream, long errcode, long serious)
{
	return 1;
}

PHP_IMAP_EXPORT void mm_fatal(char *str)
{
}
/* }}} */
