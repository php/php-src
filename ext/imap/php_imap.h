#ifndef PHP_IMAP_H
#define PHP_IMAP_H


#ifndef PHP_WIN32
#include "build-defs.h"
#endif

#include "mail.h"
#include "rfc822.h"
#include "modules.h"

extern zend_module_entry imap_module_entry;
#define imap_module_ptr &imap_module_entry


/* Data types */

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


/* Determines how mm_list() and mm_lsub() are to return their results. */
typedef enum {
	FLIST_ARRAY,
	FLIST_OBJECT
} folderlist_style_t;

typedef struct php_imap_le_struct {
	MAILSTREAM *imap_stream;
	long flags;
#ifdef OP_RELOGIN
	/* AJS: busy flag for persistent connections, pointers for chaining */
	struct php_imap_le_struct *next;
	struct php_imap_le_struct **prev;
	char busy;
#endif
} pils;

typedef struct php_imap_mailbox_struct {
	SIZEDTEXT text;
	DTYPE delimiter;
	long attributes;
	struct php_imap_mailbox_struct *next;
} FOBJECTLIST;

typedef struct php_imap_error_struct {
	SIZEDTEXT text;
	long errflg;
	struct php_imap_error_struct *next;
} ERRORLIST;


typedef struct _php_imap_message_struct {
	unsigned long msgid;
	struct _php_imap_message_struct *next;
} MESSAGELIST;
 


/* Functions */

PHP_MINIT_FUNCTION(imap);
PHP_RINIT_FUNCTION(imap);
PHP_RSHUTDOWN_FUNCTION(imap);
PHP_MINFO_FUNCTION(imap);
PHP_FUNCTION(imap_open);
PHP_FUNCTION(imap_popen);
PHP_FUNCTION(imap_reopen);
PHP_FUNCTION(imap_num_msg);
PHP_FUNCTION(imap_num_recent);
PHP_FUNCTION(imap_headers);
PHP_FUNCTION(imap_headerinfo);
PHP_FUNCTION(imap_rfc822_parse_headers);
PHP_FUNCTION(imap_body);
PHP_FUNCTION(imap_fetchstructure);
PHP_FUNCTION(imap_fetchbody);
PHP_FUNCTION(imap_expunge);
PHP_FUNCTION(imap_delete);
PHP_FUNCTION(imap_undelete);
PHP_FUNCTION(imap_check);
PHP_FUNCTION(imap_close);
PHP_FUNCTION(imap_mail_copy);
PHP_FUNCTION(imap_mail_move);
PHP_FUNCTION(imap_createmailbox);
PHP_FUNCTION(imap_renamemailbox);
PHP_FUNCTION(imap_deletemailbox);
PHP_FUNCTION(imap_listmailbox);
PHP_FUNCTION(imap_scanmailbox);
PHP_FUNCTION(imap_subscribe);
PHP_FUNCTION(imap_unsubscribe);
PHP_FUNCTION(imap_append);
PHP_FUNCTION(imap_ping);
PHP_FUNCTION(imap_base64);
PHP_FUNCTION(imap_qprint);
PHP_FUNCTION(imap_8bit);
PHP_FUNCTION(imap_binary);
PHP_FUNCTION(imap_mailboxmsginfo);
PHP_FUNCTION(imap_rfc822_write_address);
PHP_FUNCTION(imap_rfc822_parse_adrlist);
PHP_FUNCTION(imap_setflag_full);
PHP_FUNCTION(imap_clearflag_full);
PHP_FUNCTION(imap_sort);
PHP_FUNCTION(imap_fetchheader);
PHP_FUNCTION(imap_fetchtext);
PHP_FUNCTION(imap_uid);
PHP_FUNCTION(imap_msgno);
PHP_FUNCTION(imap_list);
PHP_FUNCTION(imap_list_full);
PHP_FUNCTION(imap_listscan);
PHP_FUNCTION(imap_lsub);
PHP_FUNCTION(imap_lsub_full);
PHP_FUNCTION(imap_create);
PHP_FUNCTION(imap_rename);
PHP_FUNCTION(imap_status);
PHP_FUNCTION(imap_bodystruct);
PHP_FUNCTION(imap_fetch_overview);
PHP_FUNCTION(imap_mail_compose);
PHP_FUNCTION(imap_alerts);
PHP_FUNCTION(imap_errors);
PHP_FUNCTION(imap_last_error);
PHP_FUNCTION(imap_mail);
PHP_FUNCTION(imap_search);
PHP_FUNCTION(imap_utf8);
PHP_FUNCTION(imap_utf7_decode);
PHP_FUNCTION(imap_utf7_encode);
PHP_FUNCTION(imap_mime_header_decode);

ZEND_BEGIN_MODULE_GLOBALS(imap)
	char *imap_user;
	char *imap_password;
	STRINGLIST *imap_folders;
	STRINGLIST *imap_sfolders;
	STRINGLIST *imap_alertstack;
	ERRORLIST *imap_errorstack;
	MESSAGELIST *imap_messages;
	FOBJECTLIST *imap_folder_objects;
	FOBJECTLIST *imap_sfolder_objects;
	folderlist_style_t folderlist_style;
	long status_flags;
	unsigned long status_messages;
	unsigned long status_recent;
	unsigned long status_unseen;
	unsigned long status_uidnext;
	unsigned long status_uidvalidity;
#ifdef SA_QUOTA
	unsigned long status_quota;
#endif
#ifdef SA_QUOTA_ALL
	unsigned long status_quota_all;
#endif
ZEND_END_MODULE_GLOBALS(imap)


#ifdef ZTS
# define IMAPLS_D	zend_imap_globals *imap_globals
# define IMAPLS_DC	, IMAPLS_D
# define IMAPLS_C	imap_globals
# define IMAPLS_CC , IMAPLS_C
# define IMAPG(v) (imap_globals->v)
# define IMAPLS_FETCH()	zend_imap_globals *imap_globals = ts_resource(imap_globals_id)
#else
# define IMAPLS_D
# define IMAPLS_DC
# define IMAPLS_C
# define IMAPLS_CC
# define IMAPG(v) (imap_globals.v)
# define IMAPLS_FETCH()
#endif

#define phpext_imap_ptr imap_module_ptr

#endif
