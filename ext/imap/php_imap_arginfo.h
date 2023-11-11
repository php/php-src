/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c7ef736ea5c4121a4694c24af33fa1672f502c25 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_open, 0, 3, IMAP\\Connection, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_reopen, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_is_open, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_num_msg, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_num_recent, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_headers, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_headerinfo, 0, 2, stdClass, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, from_length, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, subject_length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_imap_rfc822_parse_headers, 0, 1, stdClass, 0)
	ZEND_ARG_TYPE_INFO(0, headers, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default_hostname, IS_STRING, 0, "\"UNKNOWN\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_rfc822_write_address, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, personal, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_rfc822_parse_adrlist, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, default_hostname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_body, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_imap_fetchtext arginfo_imap_body

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_bodystruct, 0, 3, stdClass, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, section, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_fetchbody, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, section, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_imap_fetchmime arginfo_imap_fetchbody

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_savebody, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, section, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_imap_fetchheader arginfo_imap_body

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_fetchstructure, 0, 2, stdClass, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_gc, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_expunge arginfo_imap_is_open

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_delete, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_nums, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_imap_undelete arginfo_imap_delete

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_check, 0, 1, stdClass, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_listscan, 0, 4, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, reference, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_scan arginfo_imap_listscan

#define arginfo_imap_scanmailbox arginfo_imap_listscan

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_mail_copy, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_nums, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_imap_mail_move arginfo_imap_mail_copy

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_mail_compose, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, envelope, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, bodies, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_createmailbox, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_create arginfo_imap_createmailbox

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_renamemailbox, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_rename arginfo_imap_renamemailbox

#define arginfo_imap_deletemailbox arginfo_imap_createmailbox

#define arginfo_imap_subscribe arginfo_imap_createmailbox

#define arginfo_imap_unsubscribe arginfo_imap_createmailbox

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_append, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, folder, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, internal_date, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_imap_ping arginfo_imap_is_open

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_base64, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_qprint arginfo_imap_base64

#define arginfo_imap_8bit arginfo_imap_base64

#define arginfo_imap_binary arginfo_imap_base64

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_utf8, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mime_encoded_text, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_imap_status, 0, 3, stdClass, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_imap_mailboxmsginfo, 0, 1, stdClass, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_setflag_full, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, sequence, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_imap_clearflag_full arginfo_imap_setflag_full

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_sort, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, criteria, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, reverse, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, search_criteria, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, charset, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_uid, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_num, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_msgno, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, message_uid, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_list, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, reference, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_imap_listmailbox arginfo_imap_list

#define arginfo_imap_lsub arginfo_imap_list

#define arginfo_imap_listsubscribed arginfo_imap_list

#define arginfo_imap_getsubscribed arginfo_imap_list

#define arginfo_imap_getmailboxes arginfo_imap_list

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_fetch_overview, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, sequence, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_alerts, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_imap_errors arginfo_imap_alerts

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_last_error, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_search, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, criteria, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SE_FREE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, charset, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_imap_utf7_decode arginfo_imap_base64

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_utf7_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_IMAP_MUTF7)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_utf8_to_mutf7, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_IMAP_MUTF7)
#define arginfo_imap_mutf7_to_utf8 arginfo_imap_utf8_to_mutf7
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_mime_header_decode, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_thread, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SE_FREE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_timeout, 0, 1, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, timeout_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_get_quota, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, quota_root, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_imap_get_quotaroot, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_set_quota, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, quota_root, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox_size, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_setacl, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, imap, IMAP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, mailbox, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, user_id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rights, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
#define arginfo_imap_getacl arginfo_imap_get_quotaroot
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imap_mail, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, additional_headers, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cc, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bcc, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return_path, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(imap_open);
ZEND_FUNCTION(imap_reopen);
ZEND_FUNCTION(imap_close);
ZEND_FUNCTION(imap_is_open);
ZEND_FUNCTION(imap_num_msg);
ZEND_FUNCTION(imap_num_recent);
ZEND_FUNCTION(imap_headers);
ZEND_FUNCTION(imap_headerinfo);
ZEND_FUNCTION(imap_rfc822_parse_headers);
ZEND_FUNCTION(imap_rfc822_write_address);
ZEND_FUNCTION(imap_rfc822_parse_adrlist);
ZEND_FUNCTION(imap_body);
ZEND_FUNCTION(imap_bodystruct);
ZEND_FUNCTION(imap_fetchbody);
ZEND_FUNCTION(imap_fetchmime);
ZEND_FUNCTION(imap_savebody);
ZEND_FUNCTION(imap_fetchheader);
ZEND_FUNCTION(imap_fetchstructure);
ZEND_FUNCTION(imap_gc);
ZEND_FUNCTION(imap_expunge);
ZEND_FUNCTION(imap_delete);
ZEND_FUNCTION(imap_undelete);
ZEND_FUNCTION(imap_check);
ZEND_FUNCTION(imap_listscan);
ZEND_FUNCTION(imap_mail_copy);
ZEND_FUNCTION(imap_mail_move);
ZEND_FUNCTION(imap_mail_compose);
ZEND_FUNCTION(imap_createmailbox);
ZEND_FUNCTION(imap_renamemailbox);
ZEND_FUNCTION(imap_deletemailbox);
ZEND_FUNCTION(imap_subscribe);
ZEND_FUNCTION(imap_unsubscribe);
ZEND_FUNCTION(imap_append);
ZEND_FUNCTION(imap_ping);
ZEND_FUNCTION(imap_base64);
ZEND_FUNCTION(imap_qprint);
ZEND_FUNCTION(imap_8bit);
ZEND_FUNCTION(imap_binary);
ZEND_FUNCTION(imap_utf8);
ZEND_FUNCTION(imap_status);
ZEND_FUNCTION(imap_mailboxmsginfo);
ZEND_FUNCTION(imap_setflag_full);
ZEND_FUNCTION(imap_clearflag_full);
ZEND_FUNCTION(imap_sort);
ZEND_FUNCTION(imap_uid);
ZEND_FUNCTION(imap_msgno);
ZEND_FUNCTION(imap_list);
ZEND_FUNCTION(imap_lsub);
ZEND_FUNCTION(imap_getsubscribed);
ZEND_FUNCTION(imap_getmailboxes);
ZEND_FUNCTION(imap_fetch_overview);
ZEND_FUNCTION(imap_alerts);
ZEND_FUNCTION(imap_errors);
ZEND_FUNCTION(imap_last_error);
ZEND_FUNCTION(imap_search);
ZEND_FUNCTION(imap_utf7_decode);
ZEND_FUNCTION(imap_utf7_encode);
#if defined(HAVE_IMAP_MUTF7)
ZEND_FUNCTION(imap_utf8_to_mutf7);
#endif
#if defined(HAVE_IMAP_MUTF7)
ZEND_FUNCTION(imap_mutf7_to_utf8);
#endif
ZEND_FUNCTION(imap_mime_header_decode);
ZEND_FUNCTION(imap_thread);
ZEND_FUNCTION(imap_timeout);
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_FUNCTION(imap_get_quota);
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_FUNCTION(imap_get_quotaroot);
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_FUNCTION(imap_set_quota);
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_FUNCTION(imap_setacl);
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
ZEND_FUNCTION(imap_getacl);
#endif
ZEND_FUNCTION(imap_mail);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(imap_open, arginfo_imap_open)
	ZEND_FE(imap_reopen, arginfo_imap_reopen)
	ZEND_FE(imap_close, arginfo_imap_close)
	ZEND_FE(imap_is_open, arginfo_imap_is_open)
	ZEND_FE(imap_num_msg, arginfo_imap_num_msg)
	ZEND_FE(imap_num_recent, arginfo_imap_num_recent)
	ZEND_FE(imap_headers, arginfo_imap_headers)
	ZEND_FE(imap_headerinfo, arginfo_imap_headerinfo)
	ZEND_FE(imap_rfc822_parse_headers, arginfo_imap_rfc822_parse_headers)
	ZEND_FE(imap_rfc822_write_address, arginfo_imap_rfc822_write_address)
	ZEND_FE(imap_rfc822_parse_adrlist, arginfo_imap_rfc822_parse_adrlist)
	ZEND_FE(imap_body, arginfo_imap_body)
	ZEND_FALIAS(imap_fetchtext, imap_body, arginfo_imap_fetchtext)
	ZEND_FE(imap_bodystruct, arginfo_imap_bodystruct)
	ZEND_FE(imap_fetchbody, arginfo_imap_fetchbody)
	ZEND_FE(imap_fetchmime, arginfo_imap_fetchmime)
	ZEND_FE(imap_savebody, arginfo_imap_savebody)
	ZEND_FE(imap_fetchheader, arginfo_imap_fetchheader)
	ZEND_FE(imap_fetchstructure, arginfo_imap_fetchstructure)
	ZEND_FE(imap_gc, arginfo_imap_gc)
	ZEND_FE(imap_expunge, arginfo_imap_expunge)
	ZEND_FE(imap_delete, arginfo_imap_delete)
	ZEND_FE(imap_undelete, arginfo_imap_undelete)
	ZEND_FE(imap_check, arginfo_imap_check)
	ZEND_FE(imap_listscan, arginfo_imap_listscan)
	ZEND_FALIAS(imap_scan, imap_listscan, arginfo_imap_scan)
	ZEND_FALIAS(imap_scanmailbox, imap_listscan, arginfo_imap_scanmailbox)
	ZEND_FE(imap_mail_copy, arginfo_imap_mail_copy)
	ZEND_FE(imap_mail_move, arginfo_imap_mail_move)
	ZEND_FE(imap_mail_compose, arginfo_imap_mail_compose)
	ZEND_FE(imap_createmailbox, arginfo_imap_createmailbox)
	ZEND_FALIAS(imap_create, imap_createmailbox, arginfo_imap_create)
	ZEND_FE(imap_renamemailbox, arginfo_imap_renamemailbox)
	ZEND_FALIAS(imap_rename, imap_renamemailbox, arginfo_imap_rename)
	ZEND_FE(imap_deletemailbox, arginfo_imap_deletemailbox)
	ZEND_FE(imap_subscribe, arginfo_imap_subscribe)
	ZEND_FE(imap_unsubscribe, arginfo_imap_unsubscribe)
	ZEND_FE(imap_append, arginfo_imap_append)
	ZEND_FE(imap_ping, arginfo_imap_ping)
	ZEND_FE(imap_base64, arginfo_imap_base64)
	ZEND_FE(imap_qprint, arginfo_imap_qprint)
	ZEND_FE(imap_8bit, arginfo_imap_8bit)
	ZEND_FE(imap_binary, arginfo_imap_binary)
	ZEND_FE(imap_utf8, arginfo_imap_utf8)
	ZEND_FE(imap_status, arginfo_imap_status)
	ZEND_FE(imap_mailboxmsginfo, arginfo_imap_mailboxmsginfo)
	ZEND_FE(imap_setflag_full, arginfo_imap_setflag_full)
	ZEND_FE(imap_clearflag_full, arginfo_imap_clearflag_full)
	ZEND_FE(imap_sort, arginfo_imap_sort)
	ZEND_FE(imap_uid, arginfo_imap_uid)
	ZEND_FE(imap_msgno, arginfo_imap_msgno)
	ZEND_FE(imap_list, arginfo_imap_list)
	ZEND_FALIAS(imap_listmailbox, imap_list, arginfo_imap_listmailbox)
	ZEND_FE(imap_lsub, arginfo_imap_lsub)
	ZEND_FALIAS(imap_listsubscribed, imap_lsub, arginfo_imap_listsubscribed)
	ZEND_FE(imap_getsubscribed, arginfo_imap_getsubscribed)
	ZEND_FE(imap_getmailboxes, arginfo_imap_getmailboxes)
	ZEND_FE(imap_fetch_overview, arginfo_imap_fetch_overview)
	ZEND_FE(imap_alerts, arginfo_imap_alerts)
	ZEND_FE(imap_errors, arginfo_imap_errors)
	ZEND_FE(imap_last_error, arginfo_imap_last_error)
	ZEND_FE(imap_search, arginfo_imap_search)
	ZEND_FE(imap_utf7_decode, arginfo_imap_utf7_decode)
	ZEND_FE(imap_utf7_encode, arginfo_imap_utf7_encode)
#if defined(HAVE_IMAP_MUTF7)
	ZEND_FE(imap_utf8_to_mutf7, arginfo_imap_utf8_to_mutf7)
#endif
#if defined(HAVE_IMAP_MUTF7)
	ZEND_FE(imap_mutf7_to_utf8, arginfo_imap_mutf7_to_utf8)
#endif
	ZEND_FE(imap_mime_header_decode, arginfo_imap_mime_header_decode)
	ZEND_FE(imap_thread, arginfo_imap_thread)
	ZEND_FE(imap_timeout, arginfo_imap_timeout)
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
	ZEND_FE(imap_get_quota, arginfo_imap_get_quota)
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
	ZEND_FE(imap_get_quotaroot, arginfo_imap_get_quotaroot)
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
	ZEND_FE(imap_set_quota, arginfo_imap_set_quota)
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
	ZEND_FE(imap_setacl, arginfo_imap_setacl)
#endif
#if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
	ZEND_FE(imap_getacl, arginfo_imap_getacl)
#endif
	ZEND_FE(imap_mail, arginfo_imap_mail)
	ZEND_FE_END
};


static const zend_function_entry class_IMAP_Connection_methods[] = {
	ZEND_FE_END
};

static void register_php_imap_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("NIL", NIL, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("IMAP_OPENTIMEOUT", PHP_IMAP_OPENTIMEOUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAP_READTIMEOUT", PHP_IMAP_READTIMEOUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAP_WRITETIMEOUT", PHP_IMAP_WRITETIMEOUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAP_CLOSETIMEOUT", PHP_IMAP_CLOSETIMEOUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_DEBUG", OP_DEBUG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_READONLY", OP_READONLY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_ANONYMOUS", OP_ANONYMOUS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_SHORTCACHE", OP_SHORTCACHE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_SILENT", OP_SILENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_PROTOTYPE", OP_PROTOTYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_HALFOPEN", OP_HALFOPEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_EXPUNGE", OP_EXPUNGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OP_SECURE", OP_SECURE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CL_EXPUNGE", PHP_EXPUNGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FT_UID", FT_UID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FT_PEEK", FT_PEEK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FT_NOT", FT_NOT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FT_INTERNAL", FT_INTERNAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FT_PREFETCHTEXT", FT_PREFETCHTEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ST_UID", ST_UID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ST_SILENT", ST_SILENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ST_SET", ST_SET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_UID", CP_UID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_MOVE", CP_MOVE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SE_UID", SE_UID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SE_FREE", SE_FREE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SE_NOPREFETCH", SE_NOPREFETCH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_FREE", SO_FREE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_NOSERVER", SO_FREE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SA_MESSAGES", SA_MESSAGES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SA_RECENT", SA_RECENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SA_UNSEEN", SA_UNSEEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SA_UIDNEXT", SA_UIDNEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SA_UIDVALIDITY", SA_UIDVALIDITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SA_ALL", SA_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LATT_NOINFERIORS", LATT_NOINFERIORS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LATT_NOSELECT", LATT_NOSELECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LATT_MARKED", LATT_MARKED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LATT_UNMARKED", LATT_UNMARKED, CONST_PERSISTENT);
#if defined(LATT_REFERRAL)
	REGISTER_LONG_CONSTANT("LATT_REFERRAL", LATT_REFERRAL, CONST_PERSISTENT);
#endif
#if defined(LATT_HASCHILDREN)
	REGISTER_LONG_CONSTANT("LATT_HASCHILDREN", LATT_HASCHILDREN, CONST_PERSISTENT);
#endif
#if defined(LATT_HASNOCHILDREN)
	REGISTER_LONG_CONSTANT("LATT_HASNOCHILDREN", LATT_HASNOCHILDREN, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SORTDATE", SORTDATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORTARRIVAL", SORTARRIVAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORTFROM", SORTFROM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORTSUBJECT", SORTSUBJECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORTTO", SORTTO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORTCC", SORTCC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORTSIZE", SORTSIZE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPETEXT", TYPETEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEMULTIPART", TYPEMULTIPART, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEMESSAGE", TYPEMESSAGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEAPPLICATION", TYPEAPPLICATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEAUDIO", TYPEAUDIO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEIMAGE", TYPEIMAGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEVIDEO", TYPEVIDEO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEMODEL", TYPEMODEL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPEOTHER", TYPEOTHER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENC7BIT", ENC7BIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENC8BIT", ENC8BIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENCBINARY", ENCBINARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENCBASE64", ENCBASE64, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENCQUOTEDPRINTABLE", ENCQUOTEDPRINTABLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENCOTHER", ENCOTHER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAP_GC_ELT", GC_ELT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAP_GC_ENV", GC_ENV, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAP_GC_TEXTS", GC_TEXTS, CONST_PERSISTENT);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "imap_open", sizeof("imap_open") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class_IMAP_Connection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "IMAP", "Connection", class_IMAP_Connection_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
