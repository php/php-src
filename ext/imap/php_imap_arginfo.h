/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_open, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, n_retries, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_reopen, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, n_retries, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_num_msg, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

#define arginfo_imap_num_recent arginfo_imap_num_msg

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_headers, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_headerinfo, 0, 2, stdClass, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, from_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, subject_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, default_host, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_imap_rfc822_parse_headers, 0, 1, stdClass, 0)
	ZEND_ARG_TYPE_INFO(0, headers, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, default_host, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_rfc822_write_address, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, personal, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_rfc822_parse_adrlist, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, address_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, default_host, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_body, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_bodystruct, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, section, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_fetchbody, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, section, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_fetchmime arginfo_imap_fetchbody

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_savebody, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, section, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_fetchheader arginfo_imap_body

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_fetchstructure, 0, 2, stdClass, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_gc, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_expunge, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_delete, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_undelete arginfo_imap_delete

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_check, 0, 1, stdClass, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_listscan, 0, 4, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, ref, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_mail_copy, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msglist, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_mail_move, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, sequence, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_mail_compose, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, envelope, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, body, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_createmailbox, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_renamemailbox, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, old_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_deletemailbox arginfo_imap_createmailbox

#define arginfo_imap_subscribe arginfo_imap_createmailbox

#define arginfo_imap_unsubscribe arginfo_imap_createmailbox

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_append, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, folder, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, internal_date, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_ping arginfo_imap_expunge

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_base64, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_qprint arginfo_imap_base64

#define arginfo_imap_8bit arginfo_imap_base64

#define arginfo_imap_binary arginfo_imap_base64

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_utf8, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mime_encoded_text, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_status, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_imap_mailboxmsginfo, 0, 1, stdClass, 0)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_setflag_full, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, sequence, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_clearflag_full arginfo_imap_setflag_full

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_sort, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, criteria, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, reverse, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, search_criteria, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, charset, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_uid, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, msg_no, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_msgno, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, unique_msg_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_list, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, ref, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_lsub arginfo_imap_list

#define arginfo_imap_getsubscribed arginfo_imap_list

#define arginfo_imap_getmailboxes arginfo_imap_list

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_fetch_overview, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, sequence, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_alerts, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_imap_errors arginfo_imap_alerts

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_last_error, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_search, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, criteria, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, charset, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_utf7_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, buf, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_utf7_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, buf, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_IMAP_MUTF7)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_utf8_to_mutf7, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, in, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_IMAP_MUTF7)
#define arginfo_imap_mutf7_to_utf8 arginfo_imap_utf8_to_mutf7
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_mime_header_decode, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_thread, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_timeout, 0, 1, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, timeout_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_get_quota, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, qroot, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_get_quotaroot, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, mbox, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_set_quota, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, qroot, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox_size, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_setacl, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rights, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_getacl, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_mail, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, additional_headers, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cc, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, bcc, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rpath, IS_STRING, 0)
ZEND_END_ARG_INFO()
