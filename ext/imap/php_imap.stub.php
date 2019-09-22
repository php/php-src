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

/**
 * @param resource $stream_id
 * @return int|false
 */
function imap_num_msg($stream_id) {}

/**
 * @param resource $stream_id
 * @return int|false
 */
function imap_num_recent($stream_id) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_headers($stream_id) {}

/**
 * @param resource $stream_id
 * @return \stdClass|false
 */
function imap_headerinfo($stream_id, int $msg_no, int $from_length = 0, int $subject_length = 0, string $default_host = UNKNOWN) {}

function imap_rfc822_parse_headers(string $headers, string $default_host = 'UNKNOWN'): \stdClass {}

/**
 * @return string|false
 */
function imap_rfc822_write_address(string $mailbox, string $host, string $personal) {}

function imap_rfc822_parse_adrlist(string $address_string, string $default_host): array {}


/**
 * @param resource $stream_id
 * @return string|false
 */
function imap_body($stream_id, int $msg_no, int $options = 0) {}


/**
 * @param resource $stream_id
 * @return \stdClass|false
 */
function imap_bodystruct($stream_id, int $msg_no, string $section) {}

/**
 * @param resource $stream_id
 * @return string|false
 */
function imap_fetchbody($stream_id, int $msg_no, string $section, int $options = 0) {}

/**
 * @param resource $stream_id
 * @return string|false
 */
function imap_fetchmime($stream_id, int $msg_no, string $section, int $options = 0) {}


/**
 * @param resource $stream_id
 * @todo: should $file be `resouce|string`? it looks like it tries to accept anything?
 */
function imap_savebody($stream_id, $file, int $msg_no, string $section = '', int $options = 0): bool {}

/**
 * @param resource $stream_id
 * @return string|false
 */
function imap_fetchheader($stream_id, int $msg_no, int $options = 0) {}

/**
 * @param resource $stream_id
 * @return \stdClass|false
 */
function imap_fetchstructure($stream_id, int $msg_no, int $options = 0) {}

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

/**
 * @param resource $stream_id
 * @return \stdClass|false
 */
function imap_check($stream_id) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_listscan($stream_id, string $ref, string $pattern, string $content) {}

/**
 * @param resource $stream_id
 */
function imap_mail_copy($stream_id, string $msglist, string $mailbox, int $options = 0): bool {}

/**
 * @param resource $stream_id
 */
function imap_mail_move($stream_id, string $sequence, string $mailbox, int $options = 0): bool {}

/**
 * @return string|false
 */
function imap_mail_compose(array $envelope, array $body) {}

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

/**
 * @return string|false
 */
function imap_base64(string $text) {}

/**
 * @return string|false
 */
function imap_qprint(string $text) {}

/**
 * @return string|false
 */
function imap_8bit(string $text) {}

/**
 * @return string|false
 */
function imap_binary(string $text) {}


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

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_sort($stream_id, int $criteria, int $reverse, int $options = 0, string $search_criteria = UNKNOWN, string $charset = UNKNOWN) {}


/**
 * @param resource $stream_id
 * @return int|false
 */
function imap_uid($stream_id, int $msg_no) {}

/**
 * @param resource $stream_id
 * @return int|false
 */
function imap_msgno($stream_id, int $unique_msg_id) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_list($stream_id, string $ref, string $pattern) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_lsub($stream_id, string $ref, string $pattern) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_getsubscribed($stream_id, string $ref, string $pattern) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_getmailboxes($stream_id, string $ref, string $pattern) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_fetch_overview($stream_id, string $sequence, int $options = 0) {}

/**
 * @return array|false
 */
function imap_alerts() {}

/**
 * @return array|false
 */
function imap_errors() {}

/**
 * @return string|false
 */
function imap_last_error() {}


/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_search($stream_id, string $criteria, int $options = \SE_FREE, string $charset = '') {}

/**
 * @return string|false
 */
function imap_utf7_decode(string $buf) {}

function imap_utf7_encode(string $buf): string {}

#ifdef HAVE_IMAP_MUTF7
/**
 * @return string|false
 */
function imap_utf8_to_mutf7(string $in) {}

/**
 * @return string|false
 */
function imap_mutf7_to_utf8(string $in) {}
#endif

/**
 * @return array|false
 */
function imap_mime_header_decode(string $str) {}


/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_thread($stream_id, int $options = \SE_FREE) {}

/**
 * @return int|bool
 */
function imap_timeout(int $timeout_type, int $timeout = -1) {}

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_get_quota($stream_id, string $qroot) {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_get_quotaroot($stream_id, string $mbox) {}

/**
 * @param resource $stream_id
 */
function imap_set_quota($stream_id, string $qroot, int $mailbox_size): bool {}

/**
 * @param resource $stream_id
 */
function imap_setacl($stream_id, string $mailbox, string $id, string $rights): bool {}

/**
 * @param resource $stream_id
 * @return array|false
 */
function imap_getacl($stream_id, string $mailbox) {}
#endif

function imap_mail(string $to, string $subject, string $message, string $additional_headers = UNKNOWN, string $cc = UNKNOWN, string $bcc = UNKNOWN, string $rpath = UNKNOWN): bool {}
