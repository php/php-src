#ifndef _INCLUDED_IMAP_H
#define _INCLUDED_IMAP_H

#if COMPILE_DL
#undef HAVE_IMAP
#define HAVE_IMAP 1
#endif

#if HAVE_IMAP

#ifndef PHP_WIN32
#include "build-defs.h"
#endif

/* Functions accessable to PHP */
extern zend_module_entry imap_module_entry;
#define imap_module_ptr &imap_module_entry

extern PHP_MINIT_FUNCTION(imap);
extern PHP_RINIT_FUNCTION(imap);
extern PHP_RSHUTDOWN_FUNCTION(imap);
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
#ifndef PHP_WIN32
PHP_FUNCTION(imap_mail);
#endif
PHP_FUNCTION(imap_search);
PHP_FUNCTION(imap_utf8);
PHP_FUNCTION(imap_utf7_decode);
PHP_FUNCTION(imap_utf7_encode);
PHP_FUNCTION(imap_mime_header_decode);
#else
#define imap_module_ptr NULL
#endif /* HAVE_IMAP */

#endif






#define phpext_imap_ptr imap_module_ptr








