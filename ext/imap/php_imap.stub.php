<?php

/** @generate-class-entries */

namespace IMAP {
    /** @strict-properties */
    final class Connection {
    }
}

namespace {

function imap_open(string $mailbox, string $user, string $password, int $flags = 0, int $retries = 0, array $options = []): IMAP\Connection|false {}

function imap_reopen(IMAP\Connection $imap, string $mailbox, int $flags = 0, int $retries = 0): bool {}

function imap_close(IMAP\Connection $imap, int $flags = 0): bool {}

function imap_num_msg(IMAP\Connection $imap): int|false {}

function imap_num_recent(IMAP\Connection $imap): int {}

function imap_headers(IMAP\Connection $imap): array|false {}

function imap_headerinfo(IMAP\Connection $imap, int $message_num, int $from_length = 0, int $subject_length = 0): \stdClass|false {}

function imap_rfc822_parse_headers(string $headers, string $default_hostname = "UNKNOWN"): \stdClass {}

function imap_rfc822_write_address(string $mailbox, string $hostname, string $personal): string|false {}

function imap_rfc822_parse_adrlist(string $string, string $default_hostname): array {}

function imap_body(IMAP\Connection $imap, int $message_num, int $flags = 0): string|false {}

/**
 * @alias imap_body
 */
function imap_fetchtext(IMAP\Connection $imap, int $message_num, int $flags = 0): string|false {}

function imap_bodystruct(IMAP\Connection $imap, int $message_num, string $section): \stdClass|false {}

function imap_fetchbody(IMAP\Connection $imap, int $message_num, string $section, int $flags = 0): string|false {}

function imap_fetchmime(IMAP\Connection $imap, int $message_num, string $section, int $flags = 0): string|false {}

/**
 * @param resource|string|int $file
 */
function imap_savebody(IMAP\Connection $imap, $file, int $message_num, string $section = "", int $flags = 0): bool {}

function imap_fetchheader(IMAP\Connection $imap, int $message_num, int $flags = 0): string|false {}

function imap_fetchstructure(IMAP\Connection $imap, int $message_num, int $flags = 0): \stdClass|false {}

/**
 */
function imap_gc(IMAP\Connection $imap, int $flags): bool {}

/**
 */
function imap_expunge(IMAP\Connection $imap): bool {}

/**
 */
function imap_delete(IMAP\Connection $imap, string $message_num, int $flags = 0): bool {}

/**
 */
function imap_undelete(IMAP\Connection $imap, string $message_num, int $flags = 0): bool {}

function imap_check(IMAP\Connection $imap): \stdClass|false {}

function imap_listscan(IMAP\Connection $imap, string $reference, string $pattern, string $content): array|false {}

/**
 * @alias imap_listscan
 */
function imap_scan(IMAP\Connection $imap, string $reference, string $pattern, string $content): array|false {}

/**
 * @alias imap_listscan
 */
function imap_scanmailbox(IMAP\Connection $imap, string $reference, string $pattern, string $content): array|false {}

function imap_mail_copy(IMAP\Connection $imap, string $message_nums, string $mailbox, int $flags = 0): bool {}

function imap_mail_move(IMAP\Connection $imap, string $message_nums, string $mailbox, int $flags = 0): bool {}

function imap_mail_compose(array $envelope, array $bodies): string|false {}

function imap_createmailbox(IMAP\Connection $imap, string $mailbox): bool {}

/**
 * @alias imap_createmailbox
 */
function imap_create(IMAP\Connection $imap, string $mailbox): bool {}

function imap_renamemailbox(IMAP\Connection $imap, string $from, string $to): bool {}

/**
 * @alias imap_renamemailbox
 */
function imap_rename(IMAP\Connection $imap, string $from, string $to): bool {}

function imap_deletemailbox(IMAP\Connection $imap, string $mailbox): bool {}

function imap_subscribe(IMAP\Connection $imap, string $mailbox): bool {}

function imap_unsubscribe(IMAP\Connection $imap, string $mailbox): bool {}

function imap_append(IMAP\Connection $imap, string $folder, string $message, ?string $options = null, ?string $internal_date = null): bool {}

function imap_ping(IMAP\Connection $imap): bool {}

function imap_base64(string $string): string|false {}

function imap_qprint(string $string): string|false {}

function imap_8bit(string $string): string|false {}

function imap_binary(string $string): string|false {}

function imap_utf8(string $mime_encoded_text): string {}

function imap_status(IMAP\Connection $imap, string $mailbox, int $flags): \stdClass|false {}

function imap_mailboxmsginfo(IMAP\Connection $imap): \stdClass {}

function imap_setflag_full(IMAP\Connection $imap, string $sequence, string $flag, int $options = 0): bool {}

function imap_clearflag_full(IMAP\Connection $imap, string $sequence, string $flag, int $options = 0): bool {}

function imap_sort(IMAP\Connection $imap, int $criteria, bool $reverse, int $flags = 0, ?string $search_criteria = null, ?string $charset = null): array|false {}

function imap_uid(IMAP\Connection $imap, int $message_num): int|false {}

function imap_msgno(IMAP\Connection $imap, int $message_uid): int {}

function imap_list(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

/**
 * @alias imap_list
 */
function imap_listmailbox(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

function imap_lsub(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

/**
 * @alias imap_lsub
 */
function imap_listsubscribed(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

function imap_getsubscribed(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

function imap_getmailboxes(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

function imap_fetch_overview(IMAP\Connection $imap, string $sequence, int $flags = 0): array|false {}

function imap_alerts(): array|false {}

function imap_errors(): array|false {}

function imap_last_error(): string|false {}

function imap_search(IMAP\Connection $imap, string $criteria, int $flags = SE_FREE, string $charset = ""): array|false {}

function imap_utf7_decode(string $string): string|false {}

function imap_utf7_encode(string $string): string {}

#ifdef HAVE_IMAP_MUTF7
function imap_utf8_to_mutf7(string $string): string|false {}

function imap_mutf7_to_utf8(string $string): string|false {}
#endif

function imap_mime_header_decode(string $string): array|false {}

function imap_thread(IMAP\Connection $imap, int $flags = SE_FREE): array|false {}

function imap_timeout(int $timeout_type, int $timeout = -1): int|bool {}

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
function imap_get_quota(IMAP\Connection $imap, string $quota_root): array|false {}

function imap_get_quotaroot(IMAP\Connection $imap, string $mailbox): array|false {}

function imap_set_quota(IMAP\Connection $imap, string $quota_root, int $mailbox_size): bool {}

function imap_setacl(IMAP\Connection $imap, string $mailbox, string $user_id, string $rights): bool {}

function imap_getacl(IMAP\Connection $imap, string $mailbox): array|false {}
#endif

function imap_mail(string $to, string $subject, string $message, ?string $additional_headers = null, ?string $cc = null, ?string $bcc = null, ?string $return_path = null): bool {}
}
