<?php

/** @generate-class-entries */

namespace {
    /**
     * @var int
     * @cvalue NIL
     * @deprecated
     */
    const NIL = UNKNOWN;

    /**
     * @var int
     * @cvalue PHP_IMAP_OPENTIMEOUT
     */
    const IMAP_OPENTIMEOUT = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_IMAP_READTIMEOUT
     */
    const IMAP_READTIMEOUT = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_IMAP_WRITETIMEOUT
     */
    const IMAP_WRITETIMEOUT = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_IMAP_CLOSETIMEOUT
     */
    const IMAP_CLOSETIMEOUT = UNKNOWN;

    /**
     * debug protocol negotiations
     * @var int
     * @cvalue OP_DEBUG
     */
    const OP_DEBUG = UNKNOWN;
    /**
     * read-only open
     * @var int
     * @cvalue OP_READONLY
     */
    const OP_READONLY = UNKNOWN;
    /**
     * anonymous open of newsgroup
     * @var int
     * @cvalue OP_ANONYMOUS
     */
    const OP_ANONYMOUS = UNKNOWN;
    /**
     * short (elt-only) caching
     * @var int
     * @cvalue OP_SHORTCACHE
     */
    const OP_SHORTCACHE = UNKNOWN;
    /**
     * don't pass up events (internal use)
     * @var int
     * @cvalue OP_SILENT
     */
    const OP_SILENT = UNKNOWN;
    /**
     * return driver prototype
     * @var int
     * @cvalue OP_PROTOTYPE
     */
    const OP_PROTOTYPE = UNKNOWN;
    /**
     * half-open (IMAP connect but no select)
     * @var int
     * @cvalue OP_HALFOPEN
     */
    const OP_HALFOPEN = UNKNOWN;
    /**
     * silently expunge recycle stream
     * @var int
     * @cvalue OP_EXPUNGE
     */
    const OP_EXPUNGE = UNKNOWN;
    /**
     * don't do non-secure authentication
     * @var int
     * @cvalue OP_SECURE
     */
    const OP_SECURE = UNKNOWN;

    /**
     * @var int
     * @cvalue PHP_EXPUNGE
     */
    const CL_EXPUNGE = UNKNOWN;

    /**
     * argument is a UID
     * @var int
     * @cvalue FT_UID
     */
    const FT_UID = UNKNOWN;
    /**
     * peek at data
     * @var int
     * @cvalue FT_PEEK
     */
    const FT_PEEK = UNKNOWN;
    /**
     * NOT flag for header lines fetch
     * @var int
     * @cvalue FT_NOT
     */
    const FT_NOT = UNKNOWN;
    /**
     * text can be internal strings
     * @var int
     * @cvalue FT_INTERNAL
     */
    const FT_INTERNAL = UNKNOWN;
    /**
     * IMAP prefetch text when fetching header
     * @var int
     * @cvalue FT_PREFETCHTEXT
     */
    const FT_PREFETCHTEXT = UNKNOWN;

    /**
     * argument is a UID sequence
     * @var int
     * @cvalue ST_UID
     */
    const ST_UID = UNKNOWN;
    /**
     * don't return results
     * @var int
     * @cvalue ST_SILENT
     */
    const ST_SILENT = UNKNOWN;
    /**
     * set vs. clear
     * @var int
     * @cvalue ST_SET
     */
    const ST_SET = UNKNOWN;

    /**
     * argument is a UID sequence
     * @var int
     * @cvalue CP_UID
     */
    const CP_UID = UNKNOWN;
    /**
     * delete from source after copying
     * @var int
     * @cvalue CP_MOVE
     */
    const CP_MOVE = UNKNOWN;

    /**
     * return UID
     * @var int
     * @cvalue SE_UID
     */
    const SE_UID = UNKNOWN;
    /**
     * free search program after finished
     * @var int
     * @cvalue SE_FREE
     */
    const SE_FREE = UNKNOWN;
    /**
     * no search prefetchin
     * @var int
     * @cvalue SE_NOPREFETCH
     */
    const SE_NOPREFETCH = UNKNOWN;
    /**
     * free sort program after finished
     * @var int
     * @cvalue SO_FREE
     */
    const SO_FREE = UNKNOWN;
    /**
     * don't do server-based sort
     * @var int
     * @cvalue SO_FREE
     */
    const SO_NOSERVER = UNKNOWN;

    /**
     * number of messages
     * @var int
     * @cvalue SA_MESSAGES
     */
    const SA_MESSAGES = UNKNOWN;
    /**
     * number of recent messages
     * @var int
     * @cvalue SA_RECENT
     */
    const SA_RECENT = UNKNOWN;
    /**
     * number of unseen messages
     * @var int
     * @cvalue SA_UNSEEN
     */
    const SA_UNSEEN = UNKNOWN;
    /**
     * next UID to be assigned
     * @var int
     * @cvalue SA_UIDNEXT
     */
    const SA_UIDNEXT = UNKNOWN;
    /**
     * UID validity value
     * @var int
     * @cvalue SA_UIDVALIDITY
     */
    const SA_UIDVALIDITY = UNKNOWN;
    /**
     * get all status information
     * @var int
     * @cvalue SA_ALL
     */
    const SA_ALL = UNKNOWN;

    /**
     * @var int
     * @cvalue LATT_NOINFERIORS
     */
    const LATT_NOINFERIORS = UNKNOWN;
    /**
     * @var int
     * @cvalue LATT_NOSELECT
     */
    const LATT_NOSELECT = UNKNOWN;
    /**
     * @var int
     * @cvalue LATT_MARKED
     */
    const LATT_MARKED = UNKNOWN;
    /**
     * @var int
     * @cvalue LATT_UNMARKED
     */
    const LATT_UNMARKED = UNKNOWN;

#ifdef LATT_REFERRAL
    /**
     * @var int
     * @cvalue LATT_REFERRAL
     */
    const LATT_REFERRAL = UNKNOWN;
#endif
#ifdef LATT_HASCHILDREN
    /**
     * @var int
     * @cvalue LATT_HASCHILDREN
     */
    const LATT_HASCHILDREN = UNKNOWN;
#endif
#ifdef LATT_HASNOCHILDREN
    /**
     * @var int
     * @cvalue LATT_HASNOCHILDREN
     */
    const LATT_HASNOCHILDREN = UNKNOWN;
#endif

    /**
     * date
     * @var int
     * @cvalue SORTDATE
     */
    const SORTDATE = UNKNOWN;
    /**
     * arrival date
     * @var int
     * @cvalue SORTARRIVAL
     */
    const SORTARRIVAL = UNKNOWN;
    /**
     * from
     * @var int
     * @cvalue SORTFROM
     */
    const SORTFROM = UNKNOWN;
    /**
     * subject
     * @var int
     * @cvalue SORTSUBJECT
     */
    const SORTSUBJECT = UNKNOWN;
    /**
     * to
     * @var int
     * @cvalue SORTTO
     */
    const SORTTO = UNKNOWN;
    /**
     * cc
     * @var int
     * @cvalue SORTCC
     */
    const SORTCC = UNKNOWN;
    /**
     * size
     * @var int
     * @cvalue SORTSIZE
     */
    const SORTSIZE = UNKNOWN;

    /**
     * @var int
     * @cvalue TYPETEXT
     */
    const TYPETEXT = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEMULTIPART
     */
    const TYPEMULTIPART = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEMESSAGE
     */
    const TYPEMESSAGE = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEAPPLICATION
     */
    const TYPEAPPLICATION = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEAUDIO
     */
    const TYPEAUDIO = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEIMAGE
     */
    const TYPEIMAGE = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEVIDEO
     */
    const TYPEVIDEO = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEMODEL
     */
    const TYPEMODEL = UNKNOWN;
    /**
     * @var int
     * @cvalue TYPEOTHER
     */
    const TYPEOTHER = UNKNOWN;

    /**
     * @var int
     * @cvalue ENC7BIT
     */
    const ENC7BIT = UNKNOWN;
    /**
     * @var int
     * @cvalue ENC8BIT
     */
    const ENC8BIT = UNKNOWN;
    /**
     * @var int
     * @cvalue ENCBINARY
     */
    const ENCBINARY = UNKNOWN;
    /**
     * @var int
     * @cvalue ENCBASE64
     */
    const ENCBASE64 = UNKNOWN;
    /**
     * @var int
     * @cvalue ENCQUOTEDPRINTABLE
     */
    const ENCQUOTEDPRINTABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue ENCOTHER
     */
    const ENCOTHER = UNKNOWN;

    /**
     * message cache elements
     * @var int
     * @cvalue GC_ELT
     */
    const IMAP_GC_ELT = UNKNOWN;
    /**
     * ENVELOPEs and BODYs
     * @var int
     * @cvalue GC_ENV
     */
    const IMAP_GC_ENV = UNKNOWN;
    /**
     * texts
     * @var int
     * @cvalue GC_TEXTS
     */
    const IMAP_GC_TEXTS = UNKNOWN;

    function imap_open(string $mailbox, string $user, #[\SensitiveParameter] string $password, int $flags = 0, int $retries = 0, array $options = []): IMAP\Connection|false {}

    function imap_reopen(IMAP\Connection $imap, string $mailbox, int $flags = 0, int $retries = 0): bool {}

    function imap_close(IMAP\Connection $imap, int $flags = 0): bool {}

    function imap_is_open(IMAP\Connection $imap): bool {}

    function imap_num_msg(IMAP\Connection $imap): int|false {}

    function imap_num_recent(IMAP\Connection $imap): int {}

    function imap_headers(IMAP\Connection $imap): array|false {}

    function imap_headerinfo(IMAP\Connection $imap, int $message_num, int $from_length = 0, int $subject_length = 0): \stdClass|false {}

    function imap_rfc822_parse_headers(string $headers, string $default_hostname = "UNKNOWN"): \stdClass {}

    function imap_rfc822_write_address(string $mailbox, string $hostname, string $personal): string|false {}

    function imap_rfc822_parse_adrlist(string $string, string $default_hostname): array {}

    function imap_body(IMAP\Connection $imap, int $message_num, int $flags = 0): string|false {}

    /** @alias imap_body */
    function imap_fetchtext(IMAP\Connection $imap, int $message_num, int $flags = 0): string|false {}

    function imap_bodystruct(IMAP\Connection $imap, int $message_num, string $section): \stdClass|false {}

    function imap_fetchbody(IMAP\Connection $imap, int $message_num, string $section, int $flags = 0): string|false {}

    function imap_fetchmime(IMAP\Connection $imap, int $message_num, string $section, int $flags = 0): string|false {}

    /** @param resource|string|int $file */
    function imap_savebody(IMAP\Connection $imap, $file, int $message_num, string $section = "", int $flags = 0): bool {}

    function imap_fetchheader(IMAP\Connection $imap, int $message_num, int $flags = 0): string|false {}

    function imap_fetchstructure(IMAP\Connection $imap, int $message_num, int $flags = 0): \stdClass|false {}

    function imap_gc(IMAP\Connection $imap, int $flags): bool {}

    function imap_expunge(IMAP\Connection $imap): bool {}

    function imap_delete(IMAP\Connection $imap, string $message_nums, int $flags = 0): bool {}

    function imap_undelete(IMAP\Connection $imap, string $message_nums, int $flags = 0): bool {}

    function imap_check(IMAP\Connection $imap): \stdClass|false {}

    function imap_listscan(IMAP\Connection $imap, string $reference, string $pattern, string $content): array|false {}

    /** @alias imap_listscan */
    function imap_scan(IMAP\Connection $imap, string $reference, string $pattern, string $content): array|false {}

    /** @alias imap_listscan */
    function imap_scanmailbox(IMAP\Connection $imap, string $reference, string $pattern, string $content): array|false {}

    function imap_mail_copy(IMAP\Connection $imap, string $message_nums, string $mailbox, int $flags = 0): bool {}

    function imap_mail_move(IMAP\Connection $imap, string $message_nums, string $mailbox, int $flags = 0): bool {}

    function imap_mail_compose(array $envelope, array $bodies): string|false {}

    function imap_createmailbox(IMAP\Connection $imap, string $mailbox): bool {}

    /** @alias imap_createmailbox */
    function imap_create(IMAP\Connection $imap, string $mailbox): bool {}

    function imap_renamemailbox(IMAP\Connection $imap, string $from, string $to): bool {}

    /** @alias imap_renamemailbox */
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

    /** @alias imap_list */
    function imap_listmailbox(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

    function imap_lsub(IMAP\Connection $imap, string $reference, string $pattern): array|false {}

    /** @alias imap_lsub */
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

    #if (defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001))
    function imap_get_quota(IMAP\Connection $imap, string $quota_root): array|false {}

    function imap_get_quotaroot(IMAP\Connection $imap, string $mailbox): array|false {}

    function imap_set_quota(IMAP\Connection $imap, string $quota_root, int $mailbox_size): bool {}

    function imap_setacl(IMAP\Connection $imap, string $mailbox, string $user_id, string $rights): bool {}

    function imap_getacl(IMAP\Connection $imap, string $mailbox): array|false {}
    #endif

    function imap_mail(string $to, string $subject, string $message, ?string $additional_headers = null, ?string $cc = null, ?string $bcc = null, ?string $return_path = null): bool {}
}

namespace IMAP {
    /**
     * @strict-properties
     * @not-serializable
     */
    final class Connection {
    }
}
