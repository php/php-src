<?php

/**
 * @return resource|false
 */
function imap_open(string $mailbox, string $user, string $password, int $options = 0, int $n_retries = 0, array $params = UNKNOWN) {}

/**
 * @param resource $stream_id
 */
function imap_reopen($stream_id, string $mailbox, int $options = 0, int $n_retries = 0): bool {}

/**
 * @param resource $stream_id
 */
function imap_close($stream_id, int $options = 0): bool {}

/** @param resource $stream_id */
function imap_num_msg($stream_id): int|false {}

/** @param resource $stream_id */
function imap_num_recent($stream_id): int|false {}

/** @param resource $stream_id */
function imap_headers($stream_id): array|false {}

/** @param resource $stream_id */
function imap_headerinfo($stream_id, int $msg_no, int $from_length = 0, int $subject_length = 0, string $default_host = UNKNOWN): stdClass|false {}

function imap_rfc822_parse_headers(string $headers, string $default_host = 'UNKNOWN'): \stdClass {}

function imap_rfc822_write_address(string $mailbox, string $host, string $personal): string|false {}

function imap_rfc822_parse_adrlist(string $address_string, string $default_host): array {}

/** @param resource $stream_id */
function imap_body($stream_id, int $msg_no, int $options = 0): string|false {}

/**
 * @param resource $stream_id
 * @return \stdClass|false
 */
function imap_bodystruct($stream_id, int $msg_no, string $section) {}

/** @param resource $stream_id */
function imap_fetchbody($stream_id, int $msg_no, string $section, int $options = 0): string|false {}

/** @param resource $stream_id */
function imap_fetchmime($stream_id, int $msg_no, string $section, int $options = 0): string|false {}

/**
 * @param resource $stream_id
 * @todo: should $file be `resouce|string`? it looks like it tries to accept anything?
 */
function imap_savebody($stream_id, $file, int $msg_no, string $section = '', int $options = 0): bool {}

/** @param resource $stream_id */
function imap_fetchheader($stream_id, int $msg_no, int $options = 0): string|false {}

/** @param resource $stream_id */
function imap_fetchstructure($stream_id, int $msg_no, int $options = 0): stdClass|false {}

/**
 * @param resource $stream_id
 */
function imap_gc($stream_id, int $flags): bool {}

/**
 * @param resource $stream_id
 */
function imap_expunge($stream_id): bool {}

/**
 * @param resource $stream_id
 */
function imap_delete($stream_id, string $msg_no, int $options = 0): bool {}

/**
 * @param resource $stream_id
 */
function imap_undelete($stream_id, string $msg_no, int $options = 0): bool {}

/** @param resource $stream_id */
function imap_check($stream_id): stdClass|false {}

/** @param resource $stream_id */
function imap_listscan($stream_id, string $ref, string $pattern, string $content): array|false {}

/**
 * @param resource $stream_id
 */
function imap_mail_copy($stream_id, string $msglist, string $mailbox, int $options = 0): bool {}

/**
 * @param resource $stream_id
 */
function imap_mail_move($stream_id, string $sequence, string $mailbox, int $options = 0): bool {}

function imap_mail_compose(array $envelope, array $body): string|false {}

/**
 * @param resource $stream_id
 */
function imap_createmailbox($stream_id, string $mailbox): bool {}

/**
 * @param resource $stream_id
 */
function imap_renamemailbox($stream_id, string $old_name, string $new_name): bool {}

/**
 * @param resource $stream_id
 */
function imap_deletemailbox($stream_id, string $mailbox): bool {}

/**
 * @param resource $stream_id
 */
function imap_subscribe($stream_id, string $mailbox): bool {}

/**
 * @param resource $stream_id
 */
function imap_unsubscribe($stream_id, string $mailbox): bool {}

/**
 * @param resource $stream_id
 */
function imap_append($stream_id, string $folder, string $message, string $options = UNKNOWN, string $internal_date = UNKNOWN): bool {}

/**
 * @param resource $stream_id
 */
function imap_ping($stream_id): bool {}

function imap_base64(string $text): string|false {}

function imap_qprint(string $text): string|false {}

function imap_8bit(string $text): string|false {}

function imap_binary(string $text): string|false {}


function imap_utf8(string $mime_encoded_text): string {}

/**
 * @param resource $stream_id
 * @return \stdClass|false
 */
function imap_status($stream_id, string $mailbox, int $options) {}

/**
 * @param resource $stream_id
 */
function imap_mailboxmsginfo($stream_id): \stdClass {}

/**
 * @param resource $stream_id
 */
function imap_setflag_full($stream_id, string $sequence, string $flag, int $options = 0): bool {}

/**
 * @param resource $stream_id
 */
function imap_clearflag_full($stream_id, string $sequence, string $flag, int $options = 0): bool {}

/** @param resource $stream_id */
function imap_sort($stream_id, int $criteria, int $reverse, int $options = 0, string $search_criteria = UNKNOWN, string $charset = UNKNOWN): array|false {}


/** @param resource $stream_id */
function imap_uid($stream_id, int $msg_no): int|false {}

/** @param resource $stream_id */
function imap_msgno($stream_id, int $unique_msg_id): int|false {}

/** @param resource $stream_id */
function imap_list($stream_id, string $ref, string $pattern): array|false {}

/** @param resource $stream_id */
function imap_lsub($stream_id, string $ref, string $pattern): array|false {}

/** @param resource $stream_id */
function imap_getsubscribed($stream_id, string $ref, string $pattern): array|false {}

/** @param resource $stream_id */
function imap_getmailboxes($stream_id, string $ref, string $pattern): array|false {}

/** @param resource $stream_id */
function imap_fetch_overview($stream_id, string $sequence, int $options = 0): array|false {}

function imap_alerts(): array|false {}

function imap_errors(): array|false {}

function imap_last_error(): string|false {}

/** @param resource $stream_id */
function imap_search($stream_id, string $criteria, int $options = \SE_FREE, string $charset = ''): array|false {}

function imap_utf7_decode(string $buf): string|false {}

function imap_utf7_encode(string $buf): string {}

#ifdef HAVE_IMAP_MUTF7
function imap_utf8_to_mutf7(string $in): string|false {}

function imap_mutf7_to_utf8(string $in): string|false {}
#endif

function imap_mime_header_decode(string $str): array|false {}

/** @param resource $stream_id */
function imap_thread($stream_id, int $options = \SE_FREE): array|false {}

function imap_timeout(int $timeout_type, int $timeout = -1): int|bool {}

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
/** @param resource $stream_id */
function imap_get_quota($stream_id, string $qroot): array|false {}

/** @param resource $stream_id */
function imap_get_quotaroot($stream_id, string $mbox): array|false {}

/**
 * @param resource $stream_id
 */
function imap_set_quota($stream_id, string $qroot, int $mailbox_size): bool {}

/**
 * @param resource $stream_id
 */
function imap_setacl($stream_id, string $mailbox, string $id, string $rights): bool {}

/** @param resource $stream_id */
function imap_getacl($stream_id, string $mailbox): array|false {}
#endif

function imap_mail(string $to, string $subject, string $message, string $additional_headers = UNKNOWN, string $cc = UNKNOWN, string $bcc = UNKNOWN, string $rpath = UNKNOWN): bool {}
