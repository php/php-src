#ifndef _INCLUDED_IMAP_H
#define _INCLUDED_IMAP_H

#if COMPILE_DL
#undef HAVE_IMAP
#define HAVE_IMAP 1
#endif

#if HAVE_IMAP

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#ifndef MSVC5
#include "build-defs.h"
#endif

/* Functions accessable to PHP */
extern php3_module_entry php3_imap_module_entry;
#define php3_imap_module_ptr &php3_imap_module_entry

extern int imap_init(INIT_FUNC_ARGS);
extern void imap_info(void);
void php3_imap_open(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_reopen(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_close(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_num_msg(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_headers(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_body(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_expunge(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_delete(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_undelete(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_check(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_headerinfo(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_mail_copy(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_mail_move(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_createmailbox(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_renamemailbox(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_deletemailbox(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_list(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_listscan(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_lsub(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_subscribe(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_unsubscribe(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_num_recent(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchstructure(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchbody(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_append(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_ping(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_base64(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_qprint(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_8bit(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_binary(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_mailboxmsginfo(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_rfc822_write_address(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_rfc822_parse_adrlist(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_setflag_full(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_clearflag_full(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_sort(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchtext(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchheader(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_uid(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_msgno(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchtext_full(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_status(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_bodystruct(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetch_overview(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_mail_compose(INTERNAL_FUNCTION_PARAMETERS);

#else
#define php3_imap_module_ptr NULL
#endif /* HAVE_IMAP */

#endif















