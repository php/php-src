<?php

/* main/main.c */

function set_time_limit(int $seconds): bool {}

/* main/SAPI.c */

// TODO: Make this a proper callable argument?
function header_register_callback($callback): bool {}

/* main/output.c */

function ob_start(
    $user_function = null, int $chunk_size = 0, int $flags = PHP_OUTPUT_HANDLER_STDFLAGS): bool {}

function ob_flush(): bool {}

function ob_clean(): bool {}

function ob_end_flush(): bool {}

function ob_end_clean(): bool {}

/** @return string|false */
function ob_get_flush() {}

/** @return string|false */
function ob_get_clean() {}

/** @return string|false */
function ob_get_contents() {}

function ob_get_level(): int {}

/** @return int|false */
function ob_get_length() {}

function ob_list_handlers(): array {}

function ob_get_status(bool $full_status = false): array {}

// TODO: Shouldn't this be a bool argument?
function ob_implicit_flush(int $flag = 1): void {}

function output_reset_rewrite_vars(): bool {}

function output_add_rewrite_var(string $name, string $value): bool {}

/* main/streams/userspace.c */

function stream_wrapper_register(string $protocol, string $classname, int $flags = 0): bool {}

function stream_wrapper_unregister(string $protocol): bool {}

function stream_wrapper_restore(string $protocol): bool {}

/* array.c */

/** @return int|false */
function array_push(array &$stack, ...$args) {}

function krsort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function ksort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

/** @param array|Countable $array */
function count($var, int $mode = COUNT_NORAML): int {}

function natsort(array &$arg): bool {}

function natcasesort(array &$arg): bool {}

function asort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function arsort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function sort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function rsort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

/* base64.c */

function base64_encode(string $str): string {}

/** @return string|false */
function base64_decode(string $str, bool $strict = false) {}

/* crc32.c */

function crc32(string $str): int {}

/* crypt.c */

function crypt(string $str, string $salt = UNKNOWN): string {}

/* ftok.c */

#if HAVE_FTOK
function ftok(string $pathname, string $proj): int {}
#endif

/* hrtime.c */

/** @return array|int|float|false */
function hrtime(bool $get_as_number = false) {}

/* lcg.c */

function lcg_value(): float {}

/* md5.c */

function md5(string $str, bool $raw_output = false): string {}

/** @return string|false */
function md5_file(string $filename, bool $raw_output = false) {}

/* pageinfo.c */

/** @return int|false */
function getmyuid() {}

/** @return int|false */
function getmygid() {}

/** @return int|false */
function getmypid() {}

/** @return int|false */
function getmyinode() {}

function getlastmod(): int {}

/* sha1.c */

function sha1(string $str, bool $raw_output = false): string {}

/** @return string|false */
function sha1_file(string $filename, bool $raw_output = false) {}

/* syslog.c */

#ifdef HAVE_SYSLOG_H
function openlog (string $ident, int $option, int $facility): bool {}

function closelog (): bool {}

function syslog (int $priority, string $message): bool {}
#endif

#ifdef HAVE_INET_NTOP
/** @return string|false */
function inet_ntop (string $in_addr) {}
#endif

#ifdef HAVE_INET_PTON
/** @return string|false */
function inet_pton (string $ip_address) {}
#endif

/* metaphone.c */

/** @return string|false */
function metaphone(string $text, int $phones = 0) {}
