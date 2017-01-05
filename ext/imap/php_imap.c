/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include "php_streams.h"
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

#if defined(__GNUC__) && __GNUC__ >= 4
# define PHP_IMAP_EXPORT __attribute__ ((visibility("default")))
#else
# define PHP_IMAP_EXPORT
#endif

static void _php_make_header_object(zval *myzvalue, ENVELOPE *en);
static void _php_imap_add_body(zval *arg, BODY *body);
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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_open, 0, 0, 3)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, n_retries)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_reopen, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, n_retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_append, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, folder)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_num_msg, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_ping, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_num_recent, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_get_quota, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, qroot)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_get_quotaroot, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mbox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_set_quota, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, qroot)
	ZEND_ARG_INFO(0, mailbox_size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_setacl, 0, 0, 4)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_INFO(0, rights)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_getacl, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_expunge, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_gc, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_close, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_headers, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_body, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mail_copy, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msglist)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mail_move, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, sequence)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_createmailbox, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_renamemailbox, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, old_name)
	ZEND_ARG_INFO(0, new_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_deletemailbox, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_list, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, ref)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_getmailboxes, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, ref)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_listscan, 0, 0, 4)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, ref)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_check, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_delete, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_undelete, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_headerinfo, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, from_length)
	ZEND_ARG_INFO(0, subject_length)
	ZEND_ARG_INFO(0, default_host)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_rfc822_parse_headers, 0, 0, 1)
	ZEND_ARG_INFO(0, headers)
	ZEND_ARG_INFO(0, default_host)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_lsub, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, ref)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_getsubscribed, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, ref)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_subscribe, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_unsubscribe, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_fetchstructure, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_fetchbody, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, section)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_savebody, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, section)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_base64, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_qprint, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_8bit, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_binary, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mailboxmsginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_rfc822_write_address, 0, 0, 3)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, personal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_rfc822_parse_adrlist, 0, 0, 2)
	ZEND_ARG_INFO(0, address_string)
	ZEND_ARG_INFO(0, default_host)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_utf8, 0, 0, 1)
	ZEND_ARG_INFO(0, mime_encoded_text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_utf7_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, buf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_utf7_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, buf)
ZEND_END_ARG_INFO()

#ifdef HAVE_IMAP_MUTF7
ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_utf8_to_mutf7, 0, 0, 1)
	ZEND_ARG_INFO(0, in)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mutf7_to_utf8, 0, 0, 1)
	ZEND_ARG_INFO(0, in)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_setflag_full, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, sequence)
	ZEND_ARG_INFO(0, flag)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_clearflag_full, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, sequence)
	ZEND_ARG_INFO(0, flag)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_sort, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, criteria)
	ZEND_ARG_INFO(0, reverse)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, search_criteria)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_fetchheader, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_uid, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_msgno, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, unique_msg_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_status, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, mailbox)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_bodystruct, 0, 0, 3)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, msg_no)
	ZEND_ARG_INFO(0, section)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_fetch_overview, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, sequence)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mail_compose, 0, 0, 2)
	ZEND_ARG_INFO(0, envelope)
	ZEND_ARG_INFO(0, body)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mail, 0, 0, 3)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, additional_headers)
	ZEND_ARG_INFO(0, cc)
	ZEND_ARG_INFO(0, bcc)
	ZEND_ARG_INFO(0, rpath)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_search, 0, 0, 2)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, criteria)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_imap_alerts, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_imap_errors, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_imap_last_error, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_mime_header_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_thread, 0, 0, 1)
	ZEND_ARG_INFO(0, stream_id)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_imap_timeout, 0, 0, 1)
	ZEND_ARG_INFO(0, timeout_type)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ imap_functions[]
 */
const zend_function_entry imap_functions[] = {
	PHP_FE(imap_open,								arginfo_imap_open)
	PHP_FE(imap_reopen,								arginfo_imap_reopen)
	PHP_FE(imap_close,								arginfo_imap_close)
	PHP_FE(imap_num_msg,							arginfo_imap_num_msg)
	PHP_FE(imap_num_recent,							arginfo_imap_num_recent)
	PHP_FE(imap_headers,							arginfo_imap_headers)
	PHP_FE(imap_headerinfo,							arginfo_imap_headerinfo)
	PHP_FE(imap_rfc822_parse_headers,				arginfo_imap_rfc822_parse_headers)
	PHP_FE(imap_rfc822_write_address,				arginfo_imap_rfc822_write_address)
	PHP_FE(imap_rfc822_parse_adrlist,				arginfo_imap_rfc822_parse_adrlist)
	PHP_FE(imap_body,								arginfo_imap_body)
	PHP_FE(imap_bodystruct,							arginfo_imap_bodystruct)
	PHP_FE(imap_fetchbody,							arginfo_imap_fetchbody)
	PHP_FE(imap_fetchmime,							arginfo_imap_fetchbody)
	PHP_FE(imap_savebody,							arginfo_imap_savebody)
	PHP_FE(imap_fetchheader,						arginfo_imap_fetchheader)
	PHP_FE(imap_fetchstructure,						arginfo_imap_fetchstructure)
	PHP_FE(imap_gc,										arginfo_imap_gc)
	PHP_FE(imap_expunge,							arginfo_imap_expunge)
	PHP_FE(imap_delete,								arginfo_imap_delete)
	PHP_FE(imap_undelete,							arginfo_imap_undelete)
	PHP_FE(imap_check,								arginfo_imap_check)
	PHP_FE(imap_listscan,							arginfo_imap_listscan)
	PHP_FE(imap_mail_copy,							arginfo_imap_mail_copy)
	PHP_FE(imap_mail_move,							arginfo_imap_mail_move)
	PHP_FE(imap_mail_compose,						arginfo_imap_mail_compose)
	PHP_FE(imap_createmailbox,						arginfo_imap_createmailbox)
	PHP_FE(imap_renamemailbox,						arginfo_imap_renamemailbox)
	PHP_FE(imap_deletemailbox,						arginfo_imap_deletemailbox)
	PHP_FE(imap_subscribe,							arginfo_imap_subscribe)
	PHP_FE(imap_unsubscribe,						arginfo_imap_unsubscribe)
	PHP_FE(imap_append,								arginfo_imap_append)
	PHP_FE(imap_ping,								arginfo_imap_ping)
	PHP_FE(imap_base64,								arginfo_imap_base64)
	PHP_FE(imap_qprint,								arginfo_imap_qprint)
	PHP_FE(imap_8bit,								arginfo_imap_8bit)
	PHP_FE(imap_binary,								arginfo_imap_binary)
	PHP_FE(imap_utf8,								arginfo_imap_utf8)
	PHP_FE(imap_status,								arginfo_imap_status)
	PHP_FE(imap_mailboxmsginfo,						arginfo_imap_mailboxmsginfo)
	PHP_FE(imap_setflag_full,						arginfo_imap_setflag_full)
	PHP_FE(imap_clearflag_full,						arginfo_imap_clearflag_full)
	PHP_FE(imap_sort,								arginfo_imap_sort)
	PHP_FE(imap_uid,								arginfo_imap_uid)
	PHP_FE(imap_msgno,								arginfo_imap_msgno)
	PHP_FE(imap_list,								arginfo_imap_list)
	PHP_FE(imap_lsub,								arginfo_imap_lsub)
	PHP_FE(imap_fetch_overview,						arginfo_imap_fetch_overview)
	PHP_FE(imap_alerts,								arginfo_imap_alerts)
	PHP_FE(imap_errors,								arginfo_imap_errors)
	PHP_FE(imap_last_error,							arginfo_imap_last_error)
	PHP_FE(imap_search,								arginfo_imap_search)
	PHP_FE(imap_utf7_decode,						arginfo_imap_utf7_decode)
	PHP_FE(imap_utf7_encode,						arginfo_imap_utf7_encode)
#ifdef HAVE_IMAP_MUTF7
	PHP_FE(imap_utf8_to_mutf7,						arginfo_imap_utf8_to_mutf7)
	PHP_FE(imap_mutf7_to_utf8,						arginfo_imap_mutf7_to_utf8)
#endif
	PHP_FE(imap_mime_header_decode,					arginfo_imap_mime_header_decode)
	PHP_FE(imap_thread,								arginfo_imap_thread)
	PHP_FE(imap_timeout,								arginfo_imap_timeout)

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
	PHP_FE(imap_get_quota,							arginfo_imap_get_quota)
	PHP_FE(imap_get_quotaroot,						arginfo_imap_get_quotaroot)
	PHP_FE(imap_set_quota,							arginfo_imap_set_quota)
	PHP_FE(imap_setacl,								arginfo_imap_setacl)
	PHP_FE(imap_getacl,								arginfo_imap_getacl)
#endif

	PHP_FE(imap_mail,								arginfo_imap_mail)

	PHP_FALIAS(imap_header,			imap_headerinfo,	arginfo_imap_headerinfo)
	PHP_FALIAS(imap_listmailbox,	imap_list,			arginfo_imap_list)
	PHP_FALIAS(imap_getmailboxes,	imap_list_full,		arginfo_imap_getmailboxes)
	PHP_FALIAS(imap_scanmailbox,	imap_listscan,		arginfo_imap_listscan)
	PHP_FALIAS(imap_listsubscribed,	imap_lsub,			arginfo_imap_lsub)
	PHP_FALIAS(imap_getsubscribed,	imap_lsub_full,		arginfo_imap_getsubscribed)
	PHP_FALIAS(imap_fetchtext,		imap_body,			arginfo_imap_body)
	PHP_FALIAS(imap_scan,			imap_listscan,		arginfo_imap_listscan)
	PHP_FALIAS(imap_create,			imap_createmailbox,	arginfo_imap_createmailbox)
	PHP_FALIAS(imap_rename,			imap_renamemailbox,	arginfo_imap_renamemailbox)
	PHP_FE_END
};
/* }}} */

/* {{{ imap dependencies */
static const zend_module_dep imap_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_END
};
/* }}} */

/* {{{ imap_module_entry
 */
zend_module_entry imap_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	imap_deps,
	"imap",
	imap_functions,
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
ZEND_GET_MODULE(imap)
#endif

/* True globals, no need for thread safety */
static int le_imap;

#define PHP_IMAP_CHECK_MSGNO(msgindex)	\
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {	\
		php_error_docref(NULL, E_WARNING, "Bad message number");	\
		RETURN_FALSE;	\
	}	\

/* {{{ mail_close_it
 */
static void mail_close_it(zend_resource *rsrc)
{
	pils *imap_le_struct = (pils *)rsrc->ptr;

	/* Do not try to close prototype streams */
	if (!(imap_le_struct->flags & OP_PROTOTYPE)) {
		mail_close_full(imap_le_struct->imap_stream, imap_le_struct->flags);
	}

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
static zval *add_assoc_object(zval *arg, char *key, zval *tmp)
{
	HashTable *symtable;

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}
	return zend_hash_str_update(symtable, key, strlen(key), tmp);
}
/* }}} */

/* {{{ add_next_index_object
 */
static inline zval *add_next_index_object(zval *arg, zval *tmp)
{
	HashTable *symtable;

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}

	return zend_hash_next_index_insert(symtable, tmp);
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

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(imap)
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
	imap_globals->quota_return = NIL;
	imap_globals->imap_acl_list = NIL;
#endif
	imap_globals->gets_stream = NIL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(imap)
{
	unsigned long sa_all =	SA_MESSAGES | SA_RECENT | SA_UNSEEN | SA_UIDNEXT | SA_UIDVALIDITY;

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
#if HAVE_IMAP_KRB && defined(HAVE_IMAP_AUTH_GSS)
	auth_link(&auth_gss);		/* link in the gss authenticator */
#endif
	auth_link(&auth_pla);		/* link in the plain authenticator */
#endif

#ifdef HAVE_IMAP_SSL
	ssl_onceonlyinit ();
#endif

	/* lets allow NIL */
	REGISTER_LONG_CONSTANT("NIL", NIL, CONST_PERSISTENT | CONST_CS);

	/* plug in our gets */
	mail_parameters(NIL, SET_GETS, (void *) NIL);

	/* set default timeout values */
	mail_parameters(NIL, SET_OPENTIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_READTIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_WRITETIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_CLOSETIMEOUT, (void *) FG(default_socket_timeout));

	/* timeout constants */
	REGISTER_LONG_CONSTANT("IMAP_OPENTIMEOUT", 1, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_READTIMEOUT", 2, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_WRITETIMEOUT", 3, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_CLOSETIMEOUT", 4, CONST_PERSISTENT | CONST_CS);

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

#ifdef LATT_REFERRAL
	REGISTER_LONG_CONSTANT("LATT_REFERRAL", LATT_REFERRAL, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LATT_HASCHILDREN
	REGISTER_LONG_CONSTANT("LATT_HASCHILDREN", LATT_HASCHILDREN, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LATT_HASNOCHILDREN
	REGISTER_LONG_CONSTANT("LATT_HASNOCHILDREN", LATT_HASNOCHILDREN, CONST_PERSISTENT | CONST_CS);
#endif

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
	REGISTER_LONG_CONSTANT("TYPEMODEL", TYPEMODEL , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEOTHER", TYPEOTHER , CONST_PERSISTENT | CONST_CS);
	/*
	TYPETEXT                unformatted text
	TYPEMULTIPART           multiple part
	TYPEMESSAGE             encapsulated message
	TYPEAPPLICATION         application data
	TYPEAUDIO               audio
	TYPEIMAGE               static image (GIF, JPEG, etc.)
	TYPEVIDEO               video
	TYPEMODEL               model
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

	REGISTER_LONG_CONSTANT("IMAP_GC_ELT", GC_ELT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_GC_ENV", GC_ENV , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_GC_TEXTS", GC_TEXTS , CONST_PERSISTENT | CONST_CS);
	/*
	GC_ELT                 message cache elements
	GC_ENV                 ENVELOPEs and BODYs
	GC_TEXTS               texts
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
	IMAPG(gets_stream) = NIL;
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
				php_error_docref(NULL, E_NOTICE, "%s (errflg=%ld)", ecur->LTEXT, ecur->errflg);
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
				php_error_docref(NULL, E_NOTICE, "%s", acur->LTEXT);
				acur = acur->next;
			}
		}
		mail_free_stringlist(&IMAPG(imap_alertstack));
		IMAPG(imap_alertstack) = NIL;
	}
	return SUCCESS;
}
/* }}} */

#if !defined(CCLIENTVERSION)
#if HAVE_IMAP2007e
#define CCLIENTVERSION "2007e"
#elif HAVE_IMAP2007d
#define CCLIENTVERSION "2007d"
#elif HAVE_IMAP2007b
#define CCLIENTVERSION "2007b"
#elif HAVE_IMAP2007a
#define CCLIENTVERSION "2007a"
#elif HAVE_IMAP2004
#define CCLIENTVERSION "2004"
#elif HAVE_IMAP2001
#define CCLIENTVERSION "2001"
#elif HAVE_IMAP2000
#define CCLIENTVERSION "2000"
#elif defined(IMAP41)
#define CCLIENTVERSION "4.1"
#else
#define CCLIENTVERSION "4.0"
#endif
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(imap)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "IMAP c-Client Version", CCLIENTVERSION);
#if HAVE_IMAP_SSL
	php_info_print_table_row(2, "SSL Support", "enabled");
#endif
#if HAVE_IMAP_KRB && HAVE_IMAP_AUTH_GSS
	php_info_print_table_row(2, "Kerberos Support", "enabled");
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ imap_do_open
 */
static void php_imap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zend_string *mailbox, *user, *passwd;
	zend_long retries = 0, flags = NIL, cl_flags = NIL;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct;
	zval *params = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "PSS|lla", &mailbox, &user,
		&passwd, &flags, &retries, &params) == FAILURE) {
		return;
	}

	if (argc >= 4) {
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

		if ((disabled_auth_method = zend_hash_str_find(Z_ARRVAL_P(params), "DISABLE_AUTHENTICATOR", sizeof("DISABLE_AUTHENTICATOR") - 1)) != NULL) {
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
									php_error_docref(NULL, E_WARNING, "Invalid argument, expect string or array of strings");
								}
							}
						}
					}
					break;
				case IS_LONG:
				default:
					php_error_docref(NULL, E_WARNING, "Invalid argument, expect string or array of strings");
					break;
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
	if (argc >= 5) {
		if (retries < 0) {
			php_error_docref(NULL, E_WARNING ,"Retries must be greater or equal to 0");
		} else {
			mail_parameters(NIL, SET_MAXLOGINTRIALS, (void *) retries);
		}
	}
#endif

	imap_stream = mail_open(NIL, ZSTR_VAL(mailbox), flags);

	if (imap_stream == NIL) {
		php_error_docref(NULL, E_WARNING, "Couldn't open stream %s", ZSTR_VAL(mailbox));
		efree(IMAPG(imap_user)); IMAPG(imap_user) = 0;
		efree(IMAPG(imap_password)); IMAPG(imap_password) = 0;
		RETURN_FALSE;
	}

	imap_le_struct = emalloc(sizeof(pils));
	imap_le_struct->imap_stream = imap_stream;
	imap_le_struct->flags = cl_flags;

	RETURN_RES(zend_register_resource(imap_le_struct, le_imap));
}
/* }}} */

/* {{{ proto resource imap_open(string mailbox, string user, string password [, int options [, int n_retries]])
   Open an IMAP stream to a mailbox */
PHP_FUNCTION(imap_open)
{
	php_imap_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool imap_reopen(resource stream_id, string mailbox [, int options [, int n_retries]])
   Reopen an IMAP stream to a new mailbox */
PHP_FUNCTION(imap_reopen)
{
	zval *streamind;
	zend_string *mailbox;
	zend_long options = 0, retries = 0;
	pils *imap_le_struct;
	long flags=NIL;
	long cl_flags=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS|ll", &streamind, &mailbox, &options, &retries) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (options) {
		flags = options;
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		imap_le_struct->flags = cl_flags;
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

	imap_le_struct->imap_stream = mail_open(imap_le_struct->imap_stream, ZSTR_VAL(mailbox), flags);
	if (imap_le_struct->imap_stream == NIL) {
		zend_list_delete(Z_RES_P(streamind));
		php_error_docref(NULL, E_WARNING, "Couldn't re-open stream");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imap_append(resource stream_id, string folder, string message [, string options [, string internal_date]])
   Append a new message to a specified mailbox */
PHP_FUNCTION(imap_append)
{
	zval *streamind;
	zend_string *folder, *message, *internal_date = NULL, *flags = NULL;
	pils *imap_le_struct;
	STRING st;
	zend_string* regex;
	pcre_cache_entry *pce;				/* Compiled regex */
	zval *subpats = NULL;				/* Parts (not used) */
	int global = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS|SS", &streamind, &folder, &message, &flags, &internal_date) == FAILURE) {
		return;
	}

	regex  = zend_string_init("/[0-3][0-9]-((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))-[0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] [+-][0-9]{4}/", sizeof("/[0-3][0-9]-((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))-[0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] [+-][0-9]{4}/") - 1, 0);

	if (internal_date) {
		/* Make sure the given internal_date string matches the RFC specifiedformat */
		if ((pce = pcre_get_compiled_regex_cache(regex))== NULL) {
			zend_string_free(regex);
			RETURN_FALSE;
		}

		zend_string_free(regex);
		php_pcre_match_impl(pce, ZSTR_VAL(internal_date), ZSTR_LEN(internal_date), return_value, subpats, global,
			0, Z_L(0), Z_L(0));

		if (!Z_LVAL_P(return_value)) {
			php_error_docref(NULL, E_WARNING, "internal date not correctly formatted");
			internal_date = NULL;
		}
	}

	zend_string_free(regex);
	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	INIT (&st, mail_string, (void *) ZSTR_VAL(message), ZSTR_LEN(message));

	if (mail_append_full(imap_le_struct->imap_stream, ZSTR_VAL(folder), (flags ? ZSTR_VAL(flags) : NIL), (internal_date ? ZSTR_VAL(internal_date) : NIL), &st)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imap_num_msg(resource stream_id)
   Gives the number of messages in the current mailbox */
PHP_FUNCTION(imap_num_msg)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(imap_le_struct->imap_stream->nmsgs);
}
/* }}} */

/* {{{ proto bool imap_ping(resource stream_id)
   Check if the IMAP stream is still active */
PHP_FUNCTION(imap_ping)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_BOOL(mail_ping(imap_le_struct->imap_stream));
}
/* }}} */

/* {{{ proto int imap_num_recent(resource stream_id)
   Gives the number of recent messages in current mailbox */
PHP_FUNCTION(imap_num_recent)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(imap_le_struct->imap_stream->recent);
}
/* }}} */

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
/* {{{ proto array imap_get_quota(resource stream_id, string qroot)
	Returns the quota set to the mailbox account qroot */
PHP_FUNCTION(imap_get_quota)
{
	zval *streamind;
	zend_string *qroot;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &qroot) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	IMAPG(quota_return) = &return_value;

	/* set the callback for the GET_QUOTA function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if (!imap_getquota(imap_le_struct->imap_stream, ZSTR_VAL(qroot))) {
		php_error_docref(NULL, E_WARNING, "c-client imap_getquota failed");
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imap_get_quotaroot(resource stream_id, string mbox)
	Returns the quota set to the mailbox account mbox */
PHP_FUNCTION(imap_get_quotaroot)
{
	zval *streamind;
	zend_string *mbox;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &mbox) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	IMAPG(quota_return) = &return_value;

	/* set the callback for the GET_QUOTAROOT function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if (!imap_getquotaroot(imap_le_struct->imap_stream, ZSTR_VAL(mbox))) {
		php_error_docref(NULL, E_WARNING, "c-client imap_getquotaroot failed");
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_set_quota(resource stream_id, string qroot, int mailbox_size)
   Will set the quota for qroot mailbox */
PHP_FUNCTION(imap_set_quota)
{
	zval *streamind;
	zend_string *qroot;
	zend_long mailbox_size;
	pils *imap_le_struct;
	STRINGLIST	limits;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSl", &streamind, &qroot, &mailbox_size) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	limits.text.data = (unsigned char*)"STORAGE";
	limits.text.size = mailbox_size;
	limits.next = NIL;

	RETURN_BOOL(imap_setquota(imap_le_struct->imap_stream, ZSTR_VAL(qroot), &limits));
}
/* }}} */

/* {{{ proto bool imap_setacl(resource stream_id, string mailbox, string id, string rights)
	Sets the ACL for a given mailbox */
PHP_FUNCTION(imap_setacl)
{
	zval *streamind;
	zend_string *mailbox, *id, *rights;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSSS", &streamind, &mailbox, &id, &rights) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_BOOL(imap_setacl(imap_le_struct->imap_stream, ZSTR_VAL(mailbox), ZSTR_VAL(id), ZSTR_VAL(rights)));
}
/* }}} */

/* {{{ proto array imap_getacl(resource stream_id, string mailbox)
	Gets the ACL for a given mailbox */
PHP_FUNCTION(imap_getacl)
{
	zval *streamind;
	zend_string *mailbox;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &mailbox) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* initializing the special array for the return values */
	array_init(return_value);

	IMAPG(imap_acl_list) = return_value;

	/* set the callback for the GET_ACL function */
	mail_parameters(NIL, SET_ACL, (void *) mail_getacl);
	if (!imap_getacl(imap_le_struct->imap_stream, ZSTR_VAL(mailbox))) {
		php_error(E_WARNING, "c-client imap_getacl failed");
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	IMAPG(imap_acl_list) = NIL;
}
/* }}} */
#endif /* HAVE_IMAP2000 || HAVE_IMAP2001 */

/* {{{ proto bool imap_expunge(resource stream_id)
   Permanently delete all messages marked for deletion */
PHP_FUNCTION(imap_expunge)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_expunge (imap_le_struct->imap_stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imap_gc(resource stream_id, int flags)
   This function garbage collects (purges) the cache of entries of a specific type. */
PHP_FUNCTION(imap_gc)
{
	zval *streamind;
	pils *imap_le_struct;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &streamind, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(GC_TEXTS | GC_ELT | GC_ENV)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the flags parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_gc(imap_le_struct->imap_stream, flags);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imap_close(resource stream_id [, int options])
   Close an IMAP stream */
PHP_FUNCTION(imap_close)
{
	zval *streamind;
	pils *imap_le_struct=NULL;
	zend_long options = 0, flags = NIL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "r|l", &streamind, &options) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (argc == 2) {
		flags = options;

		/* Check that flags is exactly equal to PHP_EXPUNGE or zero */
		if (flags && ((flags & ~PHP_EXPUNGE) != 0)) {
			php_error_docref(NULL, E_WARNING, "invalid value for the flags parameter");
			RETURN_FALSE;
		}

		/* Do the translation from PHP's internal PHP_EXPUNGE define to c-client's CL_EXPUNGE */
		if (flags & PHP_EXPUNGE) {
			flags ^= PHP_EXPUNGE;
			flags |= CL_EXPUNGE;
		}
		imap_le_struct->flags = flags;
	}

	zend_list_close(Z_RES_P(streamind));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imap_headers(resource stream_id)
   Returns headers for all messages in a mailbox */
PHP_FUNCTION(imap_headers)
{
	zval *streamind;
	pils *imap_le_struct;
	unsigned long i;
	char *t;
	unsigned int msgno;
	char tmp[MAILTMPLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream, msgno);
		mail_fetchstructure(imap_le_struct->imap_stream, msgno, NIL);
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
		mail_fetchfrom(tmp+23, imap_le_struct->imap_stream, msgno, (long)20);
		strcat(tmp, " ");
		if ((i = cache->user_flags)) {
			strcat(tmp, "{");
			while (i) {
				strlcat(tmp, imap_le_struct->imap_stream->user_flags[find_rightmost_bit (&i)], sizeof(tmp));
				if (i) strlcat(tmp, " ", sizeof(tmp));
			}
			strlcat(tmp, "} ", sizeof(tmp));
		}
		mail_fetchsubject(t = tmp + strlen(tmp), imap_le_struct->imap_stream, msgno, (long)25);
		snprintf(t += strlen(t), sizeof(tmp) - strlen(tmp), " (%ld chars)", cache->rfc822_size);
		add_next_index_string(return_value, tmp);
	}
}
/* }}} */

/* {{{ proto string imap_body(resource stream_id, int msg_no [, int options])
   Read the message body */
PHP_FUNCTION(imap_body)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	int msgindex, argc = ZEND_NUM_ARGS();
	char *body;
	unsigned long body_len = 0;

	if (zend_parse_parameters(argc, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error_docref(NULL, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	body = mail_fetchtext_full (imap_le_struct->imap_stream, msgno, &body_len, (argc == 3 ? flags : NIL));
	if (body_len == 0) {
		RETVAL_EMPTY_STRING();
	} else {
		RETVAL_STRINGL(body, body_len);
	}
}
/* }}} */

/* {{{ proto bool imap_mail_copy(resource stream_id, string msglist, string mailbox [, int options])
   Copy specified message to a mailbox */
PHP_FUNCTION(imap_mail_copy)
{
	zval *streamind;
	zend_long options = 0;
	zend_string *seq, *folder;
	int argc = ZEND_NUM_ARGS();
	pils *imap_le_struct;

	if (zend_parse_parameters(argc, "rSS|l", &streamind, &seq, &folder, &options) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_copy_full(imap_le_struct->imap_stream, ZSTR_VAL(seq), ZSTR_VAL(folder), (argc == 4 ? options : NIL)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_mail_move(resource stream_id, string sequence, string mailbox [, int options])
   Move specified message to a mailbox */
PHP_FUNCTION(imap_mail_move)
{
	zval *streamind;
	zend_string *seq, *folder;
	zend_long options = 0;
	pils *imap_le_struct;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rSS|l", &streamind, &seq, &folder, &options) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_copy_full(imap_le_struct->imap_stream, ZSTR_VAL(seq), ZSTR_VAL(folder), (argc == 4 ? (options | CP_MOVE) : CP_MOVE)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_createmailbox(resource stream_id, string mailbox)
   Create a new mailbox */
PHP_FUNCTION(imap_createmailbox)
{
	zval *streamind;
	zend_string *folder;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &folder) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_create(imap_le_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_renamemailbox(resource stream_id, string old_name, string new_name)
   Rename a mailbox */
PHP_FUNCTION(imap_renamemailbox)
{
	zval *streamind;
	zend_string *old_mailbox, *new_mailbox;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &old_mailbox, &new_mailbox) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_rename(imap_le_struct->imap_stream, ZSTR_VAL(old_mailbox), ZSTR_VAL(new_mailbox)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_deletemailbox(resource stream_id, string mailbox)
   Delete a mailbox */
PHP_FUNCTION(imap_deletemailbox)
{
	zval *streamind;
	zend_string *folder;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &folder) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_delete(imap_le_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imap_list(resource stream_id, string ref, string pattern)
   Read the list of mailboxes */
PHP_FUNCTION(imap_list)
{
	zval *streamind;
	zend_string *ref, *pat;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &ref, &pat) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;

	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
	mail_list(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
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

/* {{{ proto array imap_getmailboxes(resource stream_id, string ref, string pattern)
   Reads the list of mailboxes and returns a full array of objects containing name, attributes, and delimiter */
/* Author: CJH */
PHP_FUNCTION(imap_list_full)
{
	zval *streamind, mboxob;
	zend_string *ref, *pat;
	pils *imap_le_struct;
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &ref, &pat) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* set flag for new, improved array of objects mailbox list */
	IMAPG(folderlist_style) = FLIST_OBJECT;

	IMAPG(imap_folder_objects) = IMAPG(imap_folder_objects_tail) = NIL;
	mail_list(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	if (IMAPG(imap_folder_objects) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	delim = safe_emalloc(2, sizeof(char), 0);
	cur=IMAPG(imap_folder_objects);
	while (cur != NIL) {
		object_init(&mboxob);
		add_property_string(&mboxob, "name", (char*)cur->LTEXT);
		add_property_long(&mboxob, "attributes", cur->attributes);
#ifdef IMAP41
		delim[0] = (char)cur->delimiter;
		delim[1] = 0;
		add_property_string(&mboxob, "delimiter", delim);
#else
		add_property_string(&mboxob, "delimiter", cur->delimiter);
#endif
		add_next_index_object(return_value, &mboxob);
		cur=cur->next;
	}
	mail_free_foblist(&IMAPG(imap_folder_objects), &IMAPG(imap_folder_objects_tail));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY;		/* reset to default */
}
/* }}} */

/* {{{ proto array imap_listscan(resource stream_id, string ref, string pattern, string content)
   Read list of mailboxes containing a certain string */
PHP_FUNCTION(imap_listscan)
{
	zval *streamind;
	zend_string *ref, *pat, *content;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSSS", &streamind, &ref, &pat, &content) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	IMAPG(imap_folders) = NIL;
	mail_scan(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat), ZSTR_VAL(content));
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

/* {{{ proto object imap_check(resource stream_id)
   Get mailbox properties */
PHP_FUNCTION(imap_check)
{
	zval *streamind;
	pils *imap_le_struct;
	char date[100];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_ping (imap_le_struct->imap_stream) == NIL) {
		RETURN_FALSE;
	}

	if (imap_le_struct->imap_stream && imap_le_struct->imap_stream->mailbox) {
		rfc822_date(date);
		object_init(return_value);
		add_property_string(return_value, "Date", date);
		add_property_string(return_value, "Driver", imap_le_struct->imap_stream->dtb->name);
		add_property_string(return_value, "Mailbox", imap_le_struct->imap_stream->mailbox);
		add_property_long(return_value, "Nmsgs", imap_le_struct->imap_stream->nmsgs);
		add_property_long(return_value, "Recent", imap_le_struct->imap_stream->recent);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_delete(resource stream_id, int msg_no [, int options])
   Mark a message for deletion */
PHP_FUNCTION(imap_delete)
{
	zval *streamind, *sequence;
	pils *imap_le_struct;
	zend_long flags = 0;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rz|l", &streamind, &sequence, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_string_ex(sequence);

	mail_setflag_full(imap_le_struct->imap_stream, Z_STRVAL_P(sequence), "\\DELETED", (argc == 3 ? flags : NIL));
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool imap_undelete(resource stream_id, int msg_no [, int flags])
   Remove the delete flag from a message */
PHP_FUNCTION(imap_undelete)
{
	zval *streamind, *sequence;
	zend_long flags = 0;
	pils *imap_le_struct;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rz|l", &streamind, &sequence, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_string_ex(sequence);

	mail_clearflag_full(imap_le_struct->imap_stream, Z_STRVAL_P(sequence), "\\DELETED", (argc == 3 ? flags : NIL));
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto object imap_headerinfo(resource stream_id, int msg_no [, int from_length [, int subject_length [, string default_host]]])
   Read the headers of the message */
PHP_FUNCTION(imap_headerinfo)
{
	zval *streamind;
	zend_string *defaulthost = NULL;
	int argc = ZEND_NUM_ARGS();
	zend_long msgno, fromlength, subjectlength;
	pils *imap_le_struct;
	MESSAGECACHE *cache;
	ENVELOPE *en;
	char dummy[2000], fulladdress[MAILTMPLEN + 1];

	if (zend_parse_parameters(argc, "rl|llS", &streamind, &msgno, &fromlength, &subjectlength, &defaulthost) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (argc >= 3) {
		if (fromlength < 0 || fromlength > MAILTMPLEN) {
			php_error_docref(NULL, E_WARNING, "From length has to be between 0 and %d", MAILTMPLEN);
			RETURN_FALSE;
		}
	} else {
		fromlength = 0x00;
	}
	if (argc >= 4) {
		if (subjectlength < 0 || subjectlength > MAILTMPLEN) {
			php_error_docref(NULL, E_WARNING, "Subject length has to be between 0 and %d", MAILTMPLEN);
			RETURN_FALSE;
		}
	} else {
		subjectlength = 0x00;
	}

	PHP_IMAP_CHECK_MSGNO(msgno);

	if (mail_fetchstructure(imap_le_struct->imap_stream, msgno, NIL)) {
		cache = mail_elt(imap_le_struct->imap_stream, msgno);
	} else {
		RETURN_FALSE;
	}

	en = mail_fetchenvelope(imap_le_struct->imap_stream, msgno);

	/* call a function to parse all the text, so that we can use the
	   same function to parse text from other sources */
	_php_make_header_object(return_value, en);

	/* now run through properties that are only going to be returned
	   from a server, not text headers */
	add_property_string(return_value, "Recent", cache->recent ? (cache->seen ? "R": "N") : " ");
	add_property_string(return_value, "Unseen", (cache->recent | cache->seen) ? " " : "U");
	add_property_string(return_value, "Flagged", cache->flagged ? "F" : " ");
	add_property_string(return_value, "Answered", cache->answered ? "A" : " ");
	add_property_string(return_value, "Deleted", cache->deleted ? "D" : " ");
	add_property_string(return_value, "Draft", cache->draft ? "X" : " ");

	snprintf(dummy, sizeof(dummy), "%4ld", cache->msgno);
	add_property_string(return_value, "Msgno", dummy);

	mail_date(dummy, cache);
	add_property_string(return_value, "MailDate", dummy);

	snprintf(dummy, sizeof(dummy), "%ld", cache->rfc822_size);
	add_property_string(return_value, "Size", dummy);

	add_property_long(return_value, "udate", mail_longdate(cache));

	if (en->from && fromlength) {
		fulladdress[0] = 0x00;
		mail_fetchfrom(fulladdress, imap_le_struct->imap_stream, msgno, fromlength);
		add_property_string(return_value, "fetchfrom", fulladdress);
	}
	if (en->subject && subjectlength) {
		fulladdress[0] = 0x00;
		mail_fetchsubject(fulladdress, imap_le_struct->imap_stream, msgno, subjectlength);
		add_property_string(return_value, "fetchsubject", fulladdress);
	}
}
/* }}} */

/* {{{ proto object imap_rfc822_parse_headers(string headers [, string default_host])
   Parse a set of mail headers contained in a string, and return an object similar to imap_headerinfo() */
PHP_FUNCTION(imap_rfc822_parse_headers)
{
	zend_string *headers, *defaulthost = NULL;
	ENVELOPE *en;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "S|S", &headers, &defaulthost) == FAILURE) {
		return;
	}

	if (argc == 2) {
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
/* {{{ proto array imap_lsub(resource stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes */
PHP_FUNCTION(imap_lsub)
{
	zval *streamind;
	zend_string *ref, *pat;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &ref, &pat) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;

	IMAPG(imap_sfolders) = NIL;
	mail_lsub(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
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

/* {{{ proto array imap_getsubscribed(resource stream_id, string ref, string pattern)
   Return a list of subscribed mailboxes, in the same format as imap_getmailboxes() */
/* Author: CJH */
PHP_FUNCTION(imap_lsub_full)
{
	zval *streamind, mboxob;
	zend_string *ref, *pat;
	pils *imap_le_struct;
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &ref, &pat) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* set flag for new, improved array of objects list */
	IMAPG(folderlist_style) = FLIST_OBJECT;

	IMAPG(imap_sfolder_objects) = IMAPG(imap_sfolder_objects_tail) = NIL;
	mail_lsub(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	if (IMAPG(imap_sfolder_objects) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	delim = safe_emalloc(2, sizeof(char), 0);
	cur=IMAPG(imap_sfolder_objects);
	while (cur != NIL) {
		object_init(&mboxob);
		add_property_string(&mboxob, "name", (char*)cur->LTEXT);
		add_property_long(&mboxob, "attributes", cur->attributes);
#ifdef IMAP41
		delim[0] = (char)cur->delimiter;
		delim[1] = 0;
		add_property_string(&mboxob, "delimiter", delim);
#else
		add_property_string(&mboxob, "delimiter", cur->delimiter);
#endif
		add_next_index_object(return_value, &mboxob);
		cur=cur->next;
	}
	mail_free_foblist (&IMAPG(imap_sfolder_objects), &IMAPG(imap_sfolder_objects_tail));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY; /* reset to default */
}
/* }}} */

/* {{{ proto bool imap_subscribe(resource stream_id, string mailbox)
   Subscribe to a mailbox */
PHP_FUNCTION(imap_subscribe)
{
	zval *streamind;
	zend_string *folder;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &folder) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_subscribe(imap_le_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool imap_unsubscribe(resource stream_id, string mailbox)
   Unsubscribe from a mailbox */
PHP_FUNCTION(imap_unsubscribe)
{
	zval *streamind;
	zend_string *folder;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &folder) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_unsubscribe(imap_le_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object imap_fetchstructure(resource stream_id, int msg_no [, int options])
   Read the full structure of a message */
PHP_FUNCTION(imap_fetchstructure)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	BODY *body;
	int msgindex, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~FT_UID) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (msgno < 1) {
		RETURN_FALSE;
	}

	object_init(return_value);

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}
	PHP_IMAP_CHECK_MSGNO(msgindex);

	mail_fetchstructure_full(imap_le_struct->imap_stream, msgno, &body , (argc == 3 ? flags : NIL));

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body information available");
		RETURN_FALSE;
	}

	_php_imap_add_body(return_value, body);
}
/* }}} */

/* {{{ proto string imap_fetchbody(resource stream_id, int msg_no, string section [, int options])
   Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	char *body;
	zend_string *sec;
	unsigned long len;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rlS|l", &streamind, &msgno, &sec, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (argc < 4 || !(flags & FT_UID)) {
		/* only perform the check if the msgno is a message number and not a UID */
		PHP_IMAP_CHECK_MSGNO(msgno);
	}

	body = mail_fetchbody_full(imap_le_struct->imap_stream, msgno, ZSTR_VAL(sec), &len, (argc == 4 ? flags : NIL));

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL(body, len);
}

/* }}} */


/* {{{ proto string imap_fetchmime(resource stream_id, int msg_no, string section [, int options])
   Get a specific body section's MIME headers */
PHP_FUNCTION(imap_fetchmime)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	char *body;
	zend_string *sec;
	unsigned long len;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rlS|l", &streamind, &msgno, &sec, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (argc < 4 || !(flags & FT_UID)) {
		/* only perform the check if the msgno is a message number and not a UID */
		PHP_IMAP_CHECK_MSGNO(msgno);
	}

	body = mail_fetch_mime(imap_le_struct->imap_stream, msgno, ZSTR_VAL(sec), &len, (argc == 4 ? flags : NIL));

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body MIME information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL(body, len);
}

/* }}} */

/* {{{ proto bool imap_savebody(resource stream_id, string|resource file, int msg_no[, string section = ""[, int options = 0]])
	Save a specific body section to a file */
PHP_FUNCTION(imap_savebody)
{
	zval *stream, *out;
	pils *imap_ptr = NULL;
	php_stream *writer = NULL;
	zend_string *section = NULL;
	int close_stream = 1;
	zend_long msgno, flags = 0;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rzl|Sl", &stream, &out, &msgno, &section, &flags)) {
		RETURN_FALSE;
	}

	if ((imap_ptr = (pils *)zend_fetch_resource(Z_RES_P(stream), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (!imap_ptr) {
		RETURN_FALSE;
	}

	switch (Z_TYPE_P(out))
	{
		case IS_LONG:
		case IS_RESOURCE:
			close_stream = 0;
			php_stream_from_zval(writer, out);
		break;

		default:
			convert_to_string_ex(out);
			writer = php_stream_open_wrapper(Z_STRVAL_P(out), "wb", REPORT_ERRORS, NULL);
		break;
	}

	if (!writer) {
		RETURN_FALSE;
	}

	IMAPG(gets_stream) = writer;
	mail_parameters(NIL, SET_GETS, (void *) php_mail_gets);
	mail_fetchbody_full(imap_ptr->imap_stream, msgno, section?ZSTR_VAL(section):"", NULL, flags);
	mail_parameters(NIL, SET_GETS, (void *) NULL);
	IMAPG(gets_stream) = NULL;

	if (close_stream) {
		php_stream_close(writer);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string imap_base64(string text)
   Decode BASE64 encoded text */
PHP_FUNCTION(imap_base64)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		return;
	}

	decode = (char *) rfc822_base64((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ proto string imap_qprint(string text)
   Convert a quoted-printable string to an 8-bit string */
PHP_FUNCTION(imap_qprint)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		return;
	}

	decode = (char *) rfc822_qprint((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ proto string imap_8bit(string text)
   Convert an 8-bit string to a quoted-printable string */
PHP_FUNCTION(imap_8bit)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		return;
	}

	decode = (char *) rfc822_8bit((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ proto string imap_binary(string text)
   Convert an 8bit string to a base64 string */
PHP_FUNCTION(imap_binary)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		return;
	}

	decode = (char*)rfc822_binary(ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);
}
/* }}} */

/* {{{ proto object imap_mailboxmsginfo(resource stream_id)
   Returns info about the current mailbox */
PHP_FUNCTION(imap_mailboxmsginfo)
{
	zval *streamind;
	pils *imap_le_struct;
	char date[100];
	unsigned int msgno, unreadmsg, deletedmsg, msize;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* Initialize return object */
	object_init(return_value);

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
	add_property_string(return_value, "Date", date);
	add_property_string(return_value, "Driver", imap_le_struct->imap_stream->dtb->name);
	add_property_string(return_value, "Mailbox", imap_le_struct->imap_stream->mailbox);
	add_property_long(return_value, "Recent", imap_le_struct->imap_stream->recent);
}
/* }}} */

/* {{{ proto string imap_rfc822_write_address(string mailbox, string host, string personal)
   Returns a properly formatted email address given the mailbox, host, and personal info */
PHP_FUNCTION(imap_rfc822_write_address)
{
	zend_string *mailbox, *host, *personal;
	ADDRESS *addr;
	zend_string *string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS", &mailbox, &host, &personal) == FAILURE) {
		return;
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
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imap_rfc822_parse_adrlist(string address_string, string default_host)
   Parses an address string */
PHP_FUNCTION(imap_rfc822_parse_adrlist)
{
	zval tovals;
	zend_string *str, *defaulthost;
	char *str_copy;
	ADDRESS *addresstmp;
	ENVELOPE *env;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &str, &defaulthost) == FAILURE) {
		return;
	}

	env = mail_newenvelope();

	/* rfc822_parse_adrlist() modifies passed string. Copy it. */
	str_copy = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
	rfc822_parse_adrlist(&env->to, str_copy, ZSTR_VAL(defaulthost));
	efree(str_copy);

	array_init(return_value);

	addresstmp = env->to;

	if (addresstmp) do {
		object_init(&tovals);
		if (addresstmp->mailbox) {
			add_property_string(&tovals, "mailbox", addresstmp->mailbox);
		}
		if (addresstmp->host) {
			add_property_string(&tovals, "host", addresstmp->host);
		}
		if (addresstmp->personal) {
			add_property_string(&tovals, "personal", addresstmp->personal);
		}
		if (addresstmp->adl) {
			add_property_string(&tovals, "adl", addresstmp->adl);
		}
		add_next_index_object(return_value, &tovals);
	} while ((addresstmp = addresstmp->next));

	mail_free_envelope(&env);
}
/* }}} */

/* {{{ proto string imap_utf8(string mime_encoded_text)
   Convert a mime-encoded text to UTF-8 */
PHP_FUNCTION(imap_utf8)
{
	zend_string *str;
	SIZEDTEXT src, dest;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
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

/* {{{ proto string imap_utf7_decode(string buf)
   Decode a modified UTF-7 string */
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
		return;
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
		php_error_docref(NULL, E_WARNING, "outp - out [%ld] != outlen [%d]", outp - out, outlen);
	}
#endif

	RETURN_STRINGL((char*)out, outlen);
}
/* }}} */

/* {{{ proto string imap_utf7_encode(string buf)
   Encode a string in modified UTF-7 */
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
		return;
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
		return;
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
		RETURN_STRING((char *)out);
	}
}
/* }}} */

/* {{{ proto string imap_utf8_to_mutf7(string in)
   Encode a UTF-8 string to modified UTF-7 */
PHP_FUNCTION(imap_utf8_to_mutf7)
{
	php_imap_mutf7(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string imap_mutf7_to_utf8(string in)
   Decode a modified UTF-7 string to UTF-8 */
PHP_FUNCTION(imap_mutf7_to_utf8)
{
	php_imap_mutf7(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */
#endif

/* {{{ proto bool imap_setflag_full(resource stream_id, string sequence, string flag [, int options])
   Sets flags on messages */
PHP_FUNCTION(imap_setflag_full)
{
	zval *streamind;
	zend_string *sequence, *flag;
	zend_long flags = 0;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS|l", &streamind, &sequence, &flag, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_setflag_full(imap_le_struct->imap_stream, ZSTR_VAL(sequence), ZSTR_VAL(flag), (flags ? flags : NIL));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imap_clearflag_full(resource stream_id, string sequence, string flag [, int options])
   Clears flags on messages */
PHP_FUNCTION(imap_clearflag_full)
{
	zval *streamind;
	zend_string *sequence, *flag;
	zend_long flags = 0;
	pils *imap_le_struct;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rSS|l", &streamind, &sequence, &flag, &flags) ==FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_clearflag_full(imap_le_struct->imap_stream, ZSTR_VAL(sequence), ZSTR_VAL(flag), (argc == 4 ? flags : NIL));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imap_sort(resource stream_id, int criteria, int reverse [, int options [, string search_criteria [, string charset]]])
   Sort an array of message headers, optionally including only messages that meet specified criteria. */
PHP_FUNCTION(imap_sort)
{
	zval *streamind;
	zend_string *criteria = NULL, *charset = NULL;
	zend_long pgm, rev, flags = 0;
	pils *imap_le_struct;
	unsigned long *slst, *sl;
	char *search_criteria;
	SORTPGM *mypgm=NIL;
	SEARCHPGM *spg=NIL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rll|lSS", &streamind, &pgm, &rev, &flags, &criteria, &charset) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (pgm > SORTSIZE) {
		php_error_docref(NULL, E_WARNING, "Unrecognized sort criteria");
		RETURN_FALSE;
	}
	if (argc >= 4) {
		if (flags < 0) {
			php_error_docref(NULL, E_WARNING, "Search options parameter has to be greater than or equal to 0");
			RETURN_FALSE;
		}
	}
	if (argc >= 5) {
		search_criteria = estrndup(ZSTR_VAL(criteria), ZSTR_LEN(criteria));
		spg = mail_criteria(search_criteria);
		efree(search_criteria);
	} else {
		spg = mail_newsearchpgm();
	}

	mypgm = mail_newsortpgm();
	mypgm->reverse = rev;
	mypgm->function = (short) pgm;
	mypgm->next = NIL;

	slst = mail_sort(imap_le_struct->imap_stream, (argc == 6 ? ZSTR_VAL(charset) : NIL), spg, mypgm, (argc >= 4 ? flags : NIL));

	if (spg && !(flags & SE_FREE)) {
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

/* {{{ proto string imap_fetchheader(resource stream_id, int msg_no [, int options])
   Get the full unfiltered header for a message */
PHP_FUNCTION(imap_fetchheader)
{
	zval *streamind;
	zend_long msgno, flags = 0L;
	pils *imap_le_struct;
	int msgindex, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_INTERNAL|FT_PREFETCHTEXT)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making sure
		   we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}

	PHP_IMAP_CHECK_MSGNO(msgindex);

	RETVAL_STRING(mail_fetchheader_full(imap_le_struct->imap_stream, msgno, NIL, NIL, (argc == 3 ? flags : NIL)));
}
/* }}} */

/* {{{ proto int imap_uid(resource stream_id, int msg_no)
   Get the unique message id associated with a standard sequential message number */
PHP_FUNCTION(imap_uid)
{
	zval *streamind;
	zend_long msgno;
	pils *imap_le_struct;
	int msgindex;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &streamind, &msgno) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	msgindex = msgno;
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error_docref(NULL, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	RETURN_LONG(mail_uid(imap_le_struct->imap_stream, msgno));
}
/* }}} */

/* {{{ proto int imap_msgno(resource stream_id, int unique_msg_id)
   Get the sequence number associated with a UID */
PHP_FUNCTION(imap_msgno)
{
	zval *streamind;
	zend_long msgno;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &streamind, &msgno) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(mail_msgno(imap_le_struct->imap_stream, msgno));
}
/* }}} */

/* {{{ proto object imap_status(resource stream_id, string mailbox, int options)
   Get status info from a mailbox */
PHP_FUNCTION(imap_status)
{
	zval *streamind;
	zend_string *mbx;
	zend_long flags;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSl", &streamind, &mbx, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	object_init(return_value);

	if (mail_status(imap_le_struct->imap_stream, ZSTR_VAL(mbx), flags)) {
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

/* {{{ proto object imap_bodystruct(resource stream_id, int msg_no, string section)
   Read the structure of a specified body section of a specific message */
PHP_FUNCTION(imap_bodystruct)
{
	zval *streamind;
	zend_long msg;
	zend_string *section;
	pils *imap_le_struct;
	zval parametres, param, dparametres, dparam;
	PARAMETER *par, *dpar;
	BODY *body;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlS", &streamind, &msg, &section) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (!msg || msg < 1 || (unsigned) msg > imap_le_struct->imap_stream->nmsgs) {
		php_error_docref(NULL, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	object_init(return_value);

	body=mail_body(imap_le_struct->imap_stream, msg, (unsigned char*)ZSTR_VAL(section));
	if (body == NULL) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	if (body->type <= TYPEMAX) {
		add_property_long(return_value, "type", body->type);
	}
	if (body->encoding <= ENCMAX) {
		add_property_long(return_value, "encoding", body->encoding);
	}

	if (body->subtype) {
		add_property_long(return_value, "ifsubtype", 1);
		add_property_string(return_value, "subtype", body->subtype);
	} else {
		add_property_long(return_value, "ifsubtype", 0);
	}

	if (body->description) {
		add_property_long(return_value, "ifdescription", 1);
		add_property_string(return_value, "description", body->description);
	} else {
		add_property_long(return_value, "ifdescription", 0);
	}
	if (body->id) {
		add_property_long(return_value, "ifid", 1);
		add_property_string(return_value, "id", body->id);
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
		add_property_string(return_value, "disposition", body->disposition.type);
	} else {
		add_property_long(return_value, "ifdisposition", 0);
	}

	if (body->disposition.parameter) {
		dpar = body->disposition.parameter;
		add_property_long(return_value, "ifdparameters", 1);
		array_init(&dparametres);
		do {
			object_init(&dparam);
			add_property_string(&dparam, "attribute", dpar->attribute);
			add_property_string(&dparam, "value", dpar->value);
			add_next_index_object(&dparametres, &dparam);
		} while ((dpar = dpar->next));
		add_assoc_object(return_value, "dparameters", &dparametres);
	} else {
		add_property_long(return_value, "ifdparameters", 0);
	}
#endif

	if ((par = body->parameter)) {
		add_property_long(return_value, "ifparameters", 1);

		array_init(&parametres);
		do {
			object_init(&param);
			if (par->attribute) {
				add_property_string(&param, "attribute", par->attribute);
			}
			if (par->value) {
				add_property_string(&param, "value", par->value);
			}

			add_next_index_object(&parametres, &param);
		} while ((par = par->next));
	} else {
		object_init(&parametres);
		add_property_long(return_value, "ifparameters", 0);
	}
	add_assoc_object(return_value, "parameters", &parametres);
}

/* }}} */

/* {{{ proto array imap_fetch_overview(resource stream_id, string sequence [, int options])
   Read an overview of the information in the headers of the given message sequence */
PHP_FUNCTION(imap_fetch_overview)
{
	zval *streamind;
	zend_string *sequence;
	pils *imap_le_struct;
	zval myoverview;
	zend_string *address;
	zend_long status, flags = 0L;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rS|l", &streamind, &sequence, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~FT_UID) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	status = (flags & FT_UID)
		? mail_uid_sequence(imap_le_struct->imap_stream, (unsigned char*)ZSTR_VAL(sequence))
		: mail_sequence(imap_le_struct->imap_stream, (unsigned char*)ZSTR_VAL(sequence));

	if (status) {
		MESSAGECACHE *elt;
		ENVELOPE *env;
		unsigned long i;

		for (i = 1; i <= imap_le_struct->imap_stream->nmsgs; i++) {
			if (((elt = mail_elt (imap_le_struct->imap_stream, i))->sequence) &&
				(env = mail_fetch_structure (imap_le_struct->imap_stream, i, NIL, NIL))) {
				object_init(&myoverview);
				if (env->subject) {
					add_property_string(&myoverview, "subject", env->subject);
				}
				if (env->from) {
					env->from->next=NULL;
					address =_php_rfc822_write_address(env->from);
					if (address) {
						add_property_str(&myoverview, "from", address);
					}
				}
				if (env->to) {
					env->to->next = NULL;
					address = _php_rfc822_write_address(env->to);
					if (address) {
						add_property_str(&myoverview, "to", address);
					}
				}
				if (env->date) {
					add_property_string(&myoverview, "date", (char*)env->date);
				}
				if (env->message_id) {
					add_property_string(&myoverview, "message_id", env->message_id);
				}
				if (env->references) {
					add_property_string(&myoverview, "references", env->references);
				}
				if (env->in_reply_to) {
					add_property_string(&myoverview, "in_reply_to", env->in_reply_to);
				}
				add_property_long(&myoverview, "size", elt->rfc822_size);
				add_property_long(&myoverview, "uid", mail_uid(imap_le_struct->imap_stream, i));
				add_property_long(&myoverview, "msgno", i);
				add_property_long(&myoverview, "recent", elt->recent);
				add_property_long(&myoverview, "flagged", elt->flagged);
				add_property_long(&myoverview, "answered", elt->answered);
				add_property_long(&myoverview, "deleted", elt->deleted);
				add_property_long(&myoverview, "seen", elt->seen);
				add_property_long(&myoverview, "draft", elt->draft);
				add_property_long(&myoverview, "udate", mail_longdate(elt));
				add_next_index_object(return_value, &myoverview);
			}
		}
	}
}
/* }}} */

/* {{{ proto string imap_mail_compose(array envelope, array body)
   Create a MIME message based on given envelope and body sections */
PHP_FUNCTION(imap_mail_compose)
{
	zval *envelope, *body;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "aa", &envelope, &body) == FAILURE) {
		return;
	}

#define PHP_RFC822_PARSE_ADRLIST(target, value) \
	str_copy = estrndup(Z_STRVAL_P(value), Z_STRLEN_P(value)); \
	rfc822_parse_adrlist(target, str_copy, "NO HOST"); \
	efree(str_copy);

	env = mail_newenvelope();
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "remail", sizeof("remail") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->remail = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "return_path", sizeof("return_path") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->return_path, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "date", sizeof("date") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->date = (unsigned char*)cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "from", sizeof("from") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->from, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "reply_to", sizeof("reply_to") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->reply_to, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "in_reply_to", sizeof("in_reply_to") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->in_reply_to = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "subject", sizeof("subject") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->subject = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "to", sizeof("to") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->to, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "cc", sizeof("cc") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->cc, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "bcc", sizeof("bcc") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->bcc, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "message_id", sizeof("message_id") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->message_id=cpystr(Z_STRVAL_P(pvalue));
	}

	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "custom_headers", sizeof("custom_headers") - 1)) != NULL) {
		if (Z_TYPE_P(pvalue) == IS_ARRAY) {
			custom_headers_param = tmp_param = NULL;
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pvalue), env_data) {
				custom_headers_param = mail_newbody_parameter();
				convert_to_string_ex(env_data);
				custom_headers_param->value = (char *) fs_get(Z_STRLEN_P(env_data) + 1);
				custom_headers_param->attribute = NULL;
				memcpy(custom_headers_param->value, Z_STRVAL_P(env_data), Z_STRLEN_P(env_data) + 1);
				custom_headers_param->next = tmp_param;
				tmp_param = custom_headers_param;
			} ZEND_HASH_FOREACH_END();
		}
	}

	first = 1;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(body), data) {
		if (first) {
			first = 0;

			if (Z_TYPE_P(data) != IS_ARRAY) {
				php_error_docref(NULL, E_WARNING, "body parameter must be a non-empty array");
				RETURN_FALSE;
			}

			bod = mail_newbody();
			topbod = bod;

			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type", sizeof("type") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->type = (short) Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "encoding", sizeof("encoding") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->encoding = (short) Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "charset", sizeof("charset") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				tmp_param = mail_newbody_parameter();
				tmp_param->value = cpystr(Z_STRVAL_P(pvalue));
				tmp_param->attribute = cpystr("CHARSET");
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type.parameters", sizeof("type.parameters") - 1)) != NULL) {
				if(Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
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
				bod->subtype = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "id", sizeof("id") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->id = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "description", sizeof("description") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->description = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition.type", sizeof("disposition.type") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->disposition.type = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition", sizeof("disposition") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->disposition.parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "contents.data", sizeof("contents.data") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->contents.text.data = fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->contents.text.data, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
				bod->contents.text.size = Z_STRLEN_P(pvalue);
			} else {
				bod->contents.text.data = fs_get(1);
				memcpy(bod->contents.text.data, "", 1);
				bod->contents.text.size = 0;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "lines", sizeof("lines") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.lines = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "bytes", sizeof("bytes") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.bytes = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "md5", sizeof("md5") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->md5 = cpystr(Z_STRVAL_P(pvalue));
			}
		} else if (Z_TYPE_P(data) == IS_ARRAY) {
			short type = -1;
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type", sizeof("type") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				type = (short) Z_LVAL_P(pvalue);
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

			if (type != TYPEMULTIPART) {
				bod->type = type;
			}

			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "encoding", sizeof("encoding") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->encoding = (short) Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "charset", sizeof("charset") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				tmp_param = mail_newbody_parameter();
				tmp_param->value = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(tmp_param->value, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue) + 1);
				tmp_param->attribute = cpystr("CHARSET");
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type.parameters", sizeof("type.parameters") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
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
				bod->subtype = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "id", sizeof("id") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->id = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "description", sizeof("description") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->description = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition.type", sizeof("disposition.type") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->disposition.type = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition", sizeof("disposition") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->disposition.parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "contents.data", sizeof("contents.data") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->contents.text.data = fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->contents.text.data, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue) + 1);
				bod->contents.text.size = Z_STRLEN_P(pvalue);
			} else {
				bod->contents.text.data = fs_get(1);
				memcpy(bod->contents.text.data, "", 1);
				bod->contents.text.size = 0;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "lines", sizeof("lines") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.lines = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "bytes", sizeof("bytes") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.bytes = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "md5", sizeof("md5") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->md5 = cpystr(Z_STRVAL_P(pvalue));
			}
		}
	} ZEND_HASH_FOREACH_END();

	if (first) {
		php_error_docref(NULL, E_WARNING, "body parameter must be a non-empty array");
		RETURN_FALSE;
	}

	if (bod && bod->type == TYPEMULTIPART && (!bod->nested.part || !bod->nested.part->next)) {
		php_error_docref(NULL, E_WARNING, "cannot generate multipart e-mail without components.");
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

				spprintf(&tempstring, 0, "%s%s%s", mystring, bod->contents.text.data, CRLF);
				efree(mystring);
				mystring=tempstring;
			} while ((part = part->next)); /* until done */

			/* output trailing cookie */
			spprintf(&tempstring, 0, "%s--%s--%s", mystring, cookie, CRLF);
			efree(mystring);
			mystring=tempstring;
	} else if (bod) {
		spprintf(&tempstring, 0, "%s%s%s", mystring, bod->contents.text.data, CRLF);
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

/* {{{ _php_imap_mail
 */
int _php_imap_mail(char *to, char *subject, char *message, char *headers, char *cc, char *bcc, char* rpath)
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
	char *bufferTo = NULL, *bufferCc = NULL, *bufferBcc = NULL, *bufferHeader = NULL;
	size_t offset, bufferLen = 0;
	size_t bt_len;

	if (headers) {
		bufferLen += strlen(headers);
	}
	if (to) {
		bufferLen += strlen(to) + 6;
	}
	if (cc) {
		bufferLen += strlen(cc) + 6;
	}

#define PHP_IMAP_CLEAN	if (bufferTo) efree(bufferTo); if (bufferCc) efree(bufferCc); if (bufferBcc) efree(bufferBcc); if (bufferHeader) efree(bufferHeader);
#define PHP_IMAP_BAD_DEST PHP_IMAP_CLEAN; efree(tempMailTo); return (BAD_MSG_DESTINATION);

	bufferHeader = (char *)safe_emalloc(bufferLen, 1, 1);
	memset(bufferHeader, 0, bufferLen);
	if (to && *to) {
		strlcat(bufferHeader, "To: ", bufferLen + 1);
		strlcat(bufferHeader, to, bufferLen + 1);
		strlcat(bufferHeader, "\r\n", bufferLen + 1);
		tempMailTo = estrdup(to);
		bt_len = strlen(to);
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
	}

	if (cc && *cc) {
		strlcat(bufferHeader, "Cc: ", bufferLen + 1);
		strlcat(bufferHeader, cc, bufferLen + 1);
		strlcat(bufferHeader, "\r\n", bufferLen + 1);
		tempMailTo = estrdup(cc);
		bt_len = strlen(cc);
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

	if (bcc && *bcc) {
		tempMailTo = estrdup(bcc);
		bt_len = strlen(bcc);
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

	if (headers && *headers) {
		strlcat(bufferHeader, headers, bufferLen + 1);
	}

	if (TSendMail(INI_STR("SMTP"), &tsm_err, &tsm_errmsg, bufferHeader, subject, bufferTo, message, bufferCc, bufferBcc, rpath) != SUCCESS) {
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
		php_error_docref(NULL, E_WARNING, "Could not execute mail delivery program");
		return 0;
	}
#endif
	return 1;
}
/* }}} */

/* {{{ proto bool imap_mail(string to, string subject, string message [, string additional_headers [, string cc [, string bcc [, string rpath]]]])
   Send an email message */
PHP_FUNCTION(imap_mail)
{
	zend_string *to=NULL, *message=NULL, *headers=NULL, *subject=NULL, *cc=NULL, *bcc=NULL, *rpath=NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "SSS|SSSS", &to, &subject, &message,
		&headers, &cc, &bcc, &rpath) == FAILURE) {
		return;
	}

	/* To: */
	if (!ZSTR_LEN(to)) {
		php_error_docref(NULL, E_WARNING, "No to field in mail command");
		RETURN_FALSE;
	}

	/* Subject: */
	if (!ZSTR_LEN(subject)) {
		php_error_docref(NULL, E_WARNING, "No subject field in mail command");
		RETURN_FALSE;
	}

	/* message body */
	if (!ZSTR_LEN(message)) {
		/* this is not really an error, so it is allowed. */
		php_error_docref(NULL, E_WARNING, "No message string in mail command");
		message = NULL;
	}

	if (_php_imap_mail(ZSTR_VAL(to), ZSTR_VAL(subject), ZSTR_VAL(message), headers?ZSTR_VAL(headers):NULL, cc?ZSTR_VAL(cc):NULL,
			bcc?ZSTR_VAL(bcc):NULL, rpath?ZSTR_VAL(rpath):NULL)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imap_search(resource stream_id, string criteria [, int options [, string charset]])
   Return a list of messages matching the given criteria */
PHP_FUNCTION(imap_search)
{
	zval *streamind;
	zend_string *criteria, *charset = NULL;
	zend_long flags = SE_FREE;
	pils *imap_le_struct;
	char *search_criteria;
	MESSAGELIST *cur;
	int argc = ZEND_NUM_ARGS();
	SEARCHPGM *pgm = NIL;

	if (zend_parse_parameters(argc, "rS|lS", &streamind, &criteria, &flags, &charset) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	search_criteria = estrndup(ZSTR_VAL(criteria), ZSTR_LEN(criteria));

	IMAPG(imap_messages) = IMAPG(imap_messages_tail) = NIL;
	pgm = mail_criteria(search_criteria);

	mail_search_full(imap_le_struct->imap_stream, (argc == 4 ? ZSTR_VAL(charset) : NIL), pgm, flags);

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

/* {{{ proto array imap_alerts(void)
   Returns an array of all IMAP alerts that have been generated since the last page load or since the last imap_alerts() call, whichever came last. The alert stack is cleared after imap_alerts() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_alerts)
{
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

/* {{{ proto array imap_errors(void)
   Returns an array of all IMAP errors generated since the last page load, or since the last imap_errors() call, whichever came last. The error stack is cleared after imap_errors() is called. */
/* Author: CJH */
PHP_FUNCTION(imap_errors)
{
	ERRORLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

/* {{{ proto string imap_last_error(void)
   Returns the last error that was generated by an IMAP function. The error stack is NOT cleared after this call. */
/* Author: CJH */
PHP_FUNCTION(imap_last_error)
{
	ERRORLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

/* {{{ proto array imap_mime_header_decode(string str)
   Decode mime header element in accordance with RFC 2047 and return array of objects containing 'charset' encoding and decoded 'text' */
PHP_FUNCTION(imap_mime_header_decode)
{
	/* Author: Ted Parnefors <ted@mtv.se> */
	zval myobject;
	zend_string *str;
	char *string, *charset, encoding, *text, *decode;
	zend_long charset_token, encoding_token, end_token, end, offset=0, i;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
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
				add_property_string(&myobject, "charset", "default");
				add_property_string(&myobject, "text", text);
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
						zval_dtor(return_value);
						RETURN_FALSE;
					}
					object_init(&myobject);
					add_property_string(&myobject, "charset", charset);
					add_property_string(&myobject, "text", decode);
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
		add_property_string(&myobject, "charset", "default");
		add_property_string(&myobject, "text", text);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &myobject);

		offset = end;	/* We have reached the end of the string. */
	}
	efree(charset);
}
/* }}} */

/* Support Functions */

#ifdef HAVE_RFC822_OUTPUT_ADDRESS_LIST
/* {{{ _php_rfc822_soutr
 */
static long _php_rfc822_soutr (void *stream, char *string)
{
	smart_str *ret = (smart_str*)stream;
	int len = strlen(string);

	smart_str_appendl(ret, string, len);
	return LONGT;
}
/* }}} */

/* {{{ _php_rfc822_write_address
 */
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

/* {{{ _php_rfc822_len
 * Calculate string length based on imap's rfc822_cat function.
 */
static int _php_rfc822_len(char *str)
{
	int len;
	char *p;

	if (!str || !*str) {
		return 0;
	}

	/* strings with special characters will need to be quoted, as a safety measure we
	 * add 2 bytes for the quotes just in case.
	 */
	len = strlen(str) + 2;
	p = str;
	/* rfc822_cat() will escape all " and \ characters, therefor we need to increase
	 * our buffer length to account for these characters.
	 */
	while ((p = strpbrk(p, "\\\""))) {
		p++;
		len++;
	}

	return len;
}
/* }}} */

/* {{{ _php_imap_get_address_size
 */
static int _php_imap_address_size (ADDRESS *addresslist)
{
	ADDRESS *tmp;
	int ret=0, num_ent=0;

	tmp = addresslist;

	if (tmp) do {
		ret += _php_rfc822_len(tmp->personal);
		ret += _php_rfc822_len(tmp->adl);
		ret += _php_rfc822_len(tmp->mailbox);
		ret += _php_rfc822_len(tmp->host);
		num_ent++;
	} while ((tmp = tmp->next));

	/*
	 * rfc822_write_address_full() needs some extra space for '<>,', etc.
	 * for this perpouse we allocate additional PHP_IMAP_ADDRESS_SIZE_BUF bytes
	 * by default this buffer is 10 bytes long
	*/
	ret += (ret) ? num_ent*PHP_IMAP_ADDRESS_SIZE_BUF : 0;

	return ret;
}

/* }}} */

/* {{{ _php_rfc822_write_address
 */
static zend_string* _php_rfc822_write_address(ADDRESS *addresslist)
{
	char address[SENDBUFLEN];

	if (_php_imap_address_size(addresslist) >= SENDBUFLEN) {
		php_error_docref(NULL, E_ERROR, "Address buffer overflow");
		return NULL;
	}
	address[0] = 0;
	rfc822_write_address(address, addresslist);
	return zend_string_init(address, strlen(address), 0);
}
/* }}} */
#endif
/* {{{ _php_imap_parse_address
 */
static zend_string* _php_imap_parse_address (ADDRESS *addresslist, zval *paddress)
{
	zend_string *fulladdress;
	ADDRESS *addresstmp;
	zval tmpvals;

	addresstmp = addresslist;

	fulladdress = _php_rfc822_write_address(addresstmp);

	addresstmp = addresslist;
	do {
		object_init(&tmpvals);
		if (addresstmp->personal) add_property_string(&tmpvals, "personal", addresstmp->personal);
		if (addresstmp->adl) add_property_string(&tmpvals, "adl", addresstmp->adl);
		if (addresstmp->mailbox) add_property_string(&tmpvals, "mailbox", addresstmp->mailbox);
		if (addresstmp->host) add_property_string(&tmpvals, "host", addresstmp->host);
		add_next_index_object(paddress, &tmpvals);
	} while ((addresstmp = addresstmp->next));
	return fulladdress;
}
/* }}} */

/* {{{ _php_make_header_object
 */
static void _php_make_header_object(zval *myzvalue, ENVELOPE *en)
{
	zval paddress;
	zend_string *fulladdress=NULL;

	object_init(myzvalue);

	if (en->remail) add_property_string(myzvalue, "remail", en->remail);
	if (en->date) add_property_string(myzvalue, "date", (char*)en->date);
	if (en->date) add_property_string(myzvalue, "Date", (char*)en->date);
	if (en->subject) add_property_string(myzvalue, "subject", en->subject);
	if (en->subject) add_property_string(myzvalue, "Subject", en->subject);
	if (en->in_reply_to) add_property_string(myzvalue, "in_reply_to", en->in_reply_to);
	if (en->message_id) add_property_string(myzvalue, "message_id", en->message_id);
	if (en->newsgroups) add_property_string(myzvalue, "newsgroups", en->newsgroups);
	if (en->followup_to) add_property_string(myzvalue, "followup_to", en->followup_to);
	if (en->references) add_property_string(myzvalue, "references", en->references);

	if (en->to) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->to, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "toaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "to", &paddress);
	}

	if (en->from) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->from, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "fromaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "from", &paddress);
	}

	if (en->cc) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->cc, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "ccaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "cc", &paddress);
	}

	if (en->bcc) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->bcc, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "bccaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "bcc", &paddress);
	}

	if (en->reply_to) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->reply_to, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "reply_toaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "reply_to", &paddress);
	}

	if (en->sender) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->sender, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "senderaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "sender", &paddress);
	}

	if (en->return_path) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->return_path, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "return_pathaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "return_path", &paddress);
	}
}
/* }}} */

/* {{{ _php_imap_add_body
 */
void _php_imap_add_body(zval *arg, BODY *body)
{
	zval parametres, param, dparametres, dparam;
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
		add_property_string(arg, "subtype", body->subtype);
	} else {
		add_property_long(arg, "ifsubtype", 0);
	}

	if (body->description) {
		add_property_long(arg, "ifdescription", 1);
		add_property_string(arg, "description", body->description);
	} else {
		add_property_long(arg, "ifdescription", 0);
	}

	if (body->id) {
		add_property_long(arg, "ifid", 1);
		add_property_string(arg, "id", body->id);
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
		add_property_string(arg, "disposition", body->disposition.type);
	} else {
		add_property_long(arg, "ifdisposition", 0);
	}

	if (body->disposition.parameter) {
		dpar = body->disposition.parameter;
		add_property_long(arg, "ifdparameters", 1);
		array_init(&dparametres);
		do {
			object_init(&dparam);
			add_property_string(&dparam, "attribute", dpar->attribute);
			add_property_string(&dparam, "value", dpar->value);
			add_next_index_object(&dparametres, &dparam);
		} while ((dpar = dpar->next));
		add_assoc_object(arg, "dparameters", &dparametres);
	} else {
		add_property_long(arg, "ifdparameters", 0);
	}
#endif

	if ((par = body->parameter)) {
		add_property_long(arg, "ifparameters", 1);

		array_init(&parametres);
		do {
			object_init(&param);
			if (par->attribute) {
				add_property_string(&param, "attribute", par->attribute);
			}
			if (par->value) {
				add_property_string(&param, "value", par->value);
			}

			add_next_index_object(&parametres, &param);
		} while ((par = par->next));
	} else {
		object_init(&parametres);
		add_property_long(arg, "ifparameters", 0);
	}
	add_assoc_object(arg, "parameters", &parametres);

	/* multipart message ? */
	if (body->type == TYPEMULTIPART) {
		array_init(&parametres);
		for (part = body->CONTENT_PART; part; part = part->next) {
			object_init(&param);
			_php_imap_add_body(&param, &part->body);
			add_next_index_object(&parametres, &param);
		}
		add_assoc_object(arg, "parts", &parametres);
	}

	/* encapsulated message ? */
	if ((body->type == TYPEMESSAGE) && (!strcasecmp(body->subtype, "rfc822"))) {
		body = body->CONTENT_MSG_BODY;
		array_init(&parametres);
		object_init(&param);
		_php_imap_add_body(&param, body);
		add_next_index_object(&parametres, &param);
		add_assoc_object(arg, "parts", &parametres);
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

	array_init(*tree);

	build_thread_tree_helper(top, *tree, &numNodes, buf);

	return SUCCESS;
}
/* }}} */

/* {{{ proto array imap_thread(resource stream_id [, int options])
   Return threaded by REFERENCES tree */
PHP_FUNCTION(imap_thread)
{
	zval *streamind;
	pils *imap_le_struct;
	zend_long flags = SE_FREE;
	char criteria[] = "ALL";
	THREADNODE *top;
	int argc = ZEND_NUM_ARGS();
	SEARCHPGM *pgm = NIL;

	if (zend_parse_parameters(argc, "r|l", &streamind, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	pgm = mail_criteria(criteria);
	top = mail_thread(imap_le_struct->imap_stream, "REFERENCES", NIL, pgm, flags);
	if (pgm && !(flags & SE_FREE)) {
		mail_free_searchpgm(&pgm);
	}

	if(top == NIL) {
		php_error_docref(NULL, E_WARNING, "Function returned an empty tree");
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

/* {{{ proto mixed imap_timeout(int timeout_type [, int timeout])
   Set or fetch imap timeout */
PHP_FUNCTION(imap_timeout)
{
	zend_long ttype, timeout=-1;
	int timeout_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &ttype, &timeout) == FAILURE) {
		RETURN_FALSE;
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

/* {{{ Interfaces to C-client
 */
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
