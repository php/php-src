<?php

/** @generate-function-entries */

/**
 * @return resource|false
 */
function imap_open(string $mailbox, string $user, string $password, int $flags = 0, int $retries = 0, array $options = []) {}

/**
 * @param resource $imap
 */
function imap_reopen($imap, string $mailbox, int $flags = 0, int $retries = 0): bool {}

/**
 * @param resource $imap
 */
function imap_close($imap, int $flags = 0): bool {}

/** @param resource $imap */
function imap_num_msg($imap): int|false {}

/** @param resource $imap */
function imap_num_recent($imap): int {}

/** @param resource $imap */
function imap_headers($imap): array|false {}

/** @param resource $imap */
function imap_headerinfo($imap, int $message_num, int $from_length = 0, int $subject_length = 0): stdClass|false {}

function imap_rfc822_parse_headers(string $headers, string $default_hostname = "UNKNOWN"): stdClass {}

function imap_rfc822_write_address(string $mailbox, string $hostname, string $personal): string|false {}

function imap_rfc822_parse_adrlist(string $string, string $default_hostname): array {}

/** @param resource $imap */
function imap_body($imap, int $message_num, int $flags = 0): string|false {}

/**
 * @param resource $imap
 * @alias imap_body
 */
function imap_fetchtext($imap, int $message_num, int $flags = 0): string|false {}

/**
 * @param resource $imap
 * @return stdClass|false
 */
function imap_bodystruct($imap, int $message_num, string $section) {}

/** @param resource $imap */
function imap_fetchbody($imap, int $message_num, string $section, int $flags = 0): string|false {}

/** @param resource $imap */
function imap_fetchmime($imap, int $message_num, string $section, int $flags = 0): string|false {}

/**
 * @param resource $imap
 * @param resource|string|int $file
 */
function imap_savebody($imap, $file, int $message_num, string $section = "", int $flags = 0): bool {}

/** @param resource $imap */
function imap_fetchheader($imap, int $message_num, int $flags = 0): string|false {}

/** @param resource $imap */
function imap_fetchstructure($imap, int $message_num, int $flags = 0): stdClass|false {}

/**
 * @param resource $imap
 */
function imap_gc($imap, int $flags): bool {}

/**
 * @param resource $imap
 */
function imap_expunge($imap): bool {}

/**
 * @param resource $imap
 */
function imap_delete($imap, string $message_nums, int $flags = 0): bool {}

/**
 * @param resource $imap
 */
function imap_undelete($imap, string $message_nums, int $flags = 0): bool {}

/** @param resource $imap */
function imap_check($imap): stdClass|false {}

/** @param resource $imap */
function imap_listscan($imap, string $reference, string $pattern, string $content): array|false {}

/**
 * @param resource $imap
 * @alias imap_listscan
 */
function imap_scan($imap, string $reference, string $pattern, string $content): array|false {}

/**
 * @param resource $imap
 * @alias imap_listscan
 */
function imap_scanmailbox($imap, string $reference, string $pattern, string $content): array|false {}

/** @param resource $imap */
function imap_mail_copy($imap, string $message_nums, string $mailbox, int $flags = 0): bool {}

/** @param resource $imap */
function imap_mail_move($imap, string $message_nums, string $mailbox, int $flags = 0): bool {}

function imap_mail_compose(array $envelope, array $bodies): string|false {}

/** @param resource $imap */
function imap_createmailbox($imap, string $mailbox): bool {}

/**
 * @param resource $imap
 * @alias imap_createmailbox
 */
function imap_create($imap, string $mailbox): bool {}

/** @param resource $imap */
function imap_renamemailbox($imap, string $from, string $to): bool {}

/**
 * @param resource $imap
 * @alias imap_renamemailbox
 */
function imap_rename($imap, string $from, string $to): bool {}

/** @param resource $imap */
function imap_deletemailbox($imap, string $mailbox): bool {}

/** @param resource $imap */
function imap_subscribe($imap, string $mailbox): bool {}

/** @param resource $imap */
function imap_unsubscribe($imap, string $mailbox): bool {}

/** @param resource $imap */
function imap_append($imap, string $folder, string $message, ?string $options = null, ?string $internal_date = null): bool {}

/** @param resource $imap */
function imap_ping($imap): bool {}

function imap_base64(string $string): string|false {}

function imap_qprint(string $string): string|false {}

function imap_8bit(string $string): string|false {}

function imap_binary(string $string): string|false {}

function imap_utf8(string $mime_encoded_text): string {}

/**
 * @param resource $imap
 * @return stdClass|false
 */
function imap_status($imap, string $mailbox, int $flags) {}

/** @param resource $imap */
function imap_mailboxmsginfo($imap): stdClass {}

/** @param resource $imap */
function imap_setflag_full($imap, string $sequence, string $flag, int $options = 0): bool {}

/** @param resource $imap */
function imap_clearflag_full($imap, string $sequence, string $flag, int $options = 0): bool {}

/** @param resource $imap */
function imap_sort($imap, int $criteria, bool $reverse, int $flags = 0, ?string $search_criteria = null, ?string $charset = null): array|false {}

/** @param resource $imap */
function imap_uid($imap, int $message_num): int|false {}

/** @param resource $imap */
function imap_msgno($imap, int $message_uid): int {}

/** @param resource $imap */
function imap_list($imap, string $reference, string $pattern): array|false {}

/**
 * @param resource $imap
 * @alias imap_list
 */
function imap_listmailbox($imap, string $reference, string $pattern): array|false {}

/** @param resource $imap */
function imap_lsub($imap, string $reference, string $pattern): array|false {}

/**
 * @param resource $imap
 * @alias imap_lsub
 */
function imap_listsubscribed($imap, string $reference, string $pattern): array|false {}

/** @param resource $imap */
function imap_getsubscribed($imap, string $reference, string $pattern): array|false {}

/** @param resource $imap */
function imap_getmailboxes($imap, string $reference, string $pattern): array|false {}

/** @param resource $imap */
function imap_fetch_overview($imap, string $sequence, int $flags = 0): array|false {}

function imap_alerts(): array|false {}

function imap_errors(): array|false {}

function imap_last_error(): string|false {}

/** @param resource $imap */
function imap_search($imap, string $criteria, int $flags = SE_FREE, string $charset = ""): array|false {}

function imap_utf7_decode(string $string): string|false {}

function imap_utf7_encode(string $string): string {}

#ifdef HAVE_IMAP_MUTF7
function imap_utf8_to_mutf7(string $string): string|false {}

function imap_mutf7_to_utf8(string $string): string|false {}
#endif

function imap_mime_header_decode(string $string): array|false {}

/** @param resource $imap */
function imap_thread($imap, int $flags = SE_FREE): array|false {}

function imap_timeout(int $timeout_type, int $timeout = -1): int|bool {}

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
/** @param resource $imap */
function imap_get_quota($imap, string $quota_root): array|false {}

/** @param resource $imap */
function imap_get_quotaroot($imap, string $mailbox): array|false {}

/** @param resource $imap */
function imap_set_quota($imap, string $quota_root, int $mailbox_size): bool {}

/** @param resource $imap */
function imap_setacl($imap, string $mailbox, string $user_id, string $rights): bool {}

/** @param resource $imap */
function imap_getacl($imap, string $mailbox): array|false {}
#endif

function imap_mail(string $to, string $subject, string $message, ?string $additional_headers = null, ?string $cc = null, ?string $bcc = null, ?string $return_path = null): bool {}
