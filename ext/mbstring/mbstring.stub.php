<?php

/** @generate-class-entries */

#ifdef HAVE_MBREGEX
/**
 * @var string
 * @cvalue php_mb_oniguruma_version
 */
const MB_ONIGURUMA_VERSION = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_UPPER
 */
const MB_CASE_UPPER = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_LOWER
 */
const MB_CASE_LOWER = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_TITLE
 */
const MB_CASE_TITLE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_FOLD
 */
const MB_CASE_FOLD = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_UPPER_SIMPLE
 */
const MB_CASE_UPPER_SIMPLE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_LOWER_SIMPLE
 */
const MB_CASE_LOWER_SIMPLE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_TITLE_SIMPLE
 */
const MB_CASE_TITLE_SIMPLE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_UNICODE_CASE_FOLD_SIMPLE
 */
const MB_CASE_FOLD_SIMPLE = UNKNOWN;

/** @refcount 1 */
function mb_language(?string $language = null): string|bool {}

/** @refcount 1 */
function mb_internal_encoding(?string $encoding = null): string|bool {}

/**
 * @return array<int, string>|string|false
 * @refcount 1
 */
function mb_http_input(?string $type = null): array|string|false {}

/** @refcount 1 */
function mb_http_output(?string $encoding = null): string|bool {}

/**
 * @return array<int, string>|true
 * @refcount 1
 */
function mb_detect_order(array|string|null $encoding = null): array|bool {}

/** @refcount 1 */
function mb_substitute_character(string|int|null $substitute_character = null): string|int|bool {}

/** @refcount 1 */
function mb_preferred_mime_name(string $encoding): string|false {}

/** @param array $result */
function mb_parse_str(string $string, &$result): bool {}

/** @refcount 1 */
function mb_output_handler(string $string, int $status): string {}

function mb_str_split(string $string, int $length = 1, ?string $encoding = null): array {}

function mb_strlen(string $string, ?string $encoding = null): int {}

function mb_strpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_strrpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_stripos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_strripos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

/** @refcount 1 */
function mb_strstr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

/** @refcount 1 */
function mb_strrchr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

/** @refcount 1 */
function mb_stristr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

/** @refcount 1 */
function mb_strrichr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

function mb_substr_count(string $haystack, string $needle, ?string $encoding = null): int {}

/** @refcount 1 */
function mb_substr(string $string, int $start, ?int $length = null, ?string $encoding = null): string {}

/** @refcount 1 */
function mb_strcut(string $string, int $start, ?int $length = null, ?string $encoding = null): string {}

function mb_strwidth(string $string, ?string $encoding = null): int {}

function mb_strimwidth(string $string, int $start, int $width, string $trim_marker = "", ?string $encoding = null): string {}

/**
 * @return array<int|string, mixed>|string|false
 * @refcount 1
 */
function mb_convert_encoding(array|string $string, string $to_encoding, array|string|null $from_encoding = null): array|string|false {}

/** @refcount 1 */
function mb_convert_case(string $string, int $mode, ?string $encoding = null): string {}

/** @refcount 1 */
function mb_strtoupper(string $string, ?string $encoding = null): string {}

/** @refcount 1 */
function mb_strtolower(string $string, ?string $encoding = null): string {}

function mb_trim(string $string, string $characters = " \f\n\r\t\v\x00\u{00A0}\u{1680}\u{2000}\u{2001}\u{2002}\u{2003}\u{2004}\u{2005}\u{2006}\u{2007}\u{2008}\u{2009}\u{200A}\u{2028}\u{2029}\u{202F}\u{205F}\u{3000}\u{0085}\u{180E}", ?string $encoding = null): string {}

function mb_ltrim(string $string, string $characters = " \f\n\r\t\v\x00\u{00A0}\u{1680}\u{2000}\u{2001}\u{2002}\u{2003}\u{2004}\u{2005}\u{2006}\u{2007}\u{2008}\u{2009}\u{200A}\u{2028}\u{2029}\u{202F}\u{205F}\u{3000}\u{0085}\u{180E}", ?string $encoding = null): string {}

function mb_rtrim(string $string, string $characters = " \f\n\r\t\v\x00\u{00A0}\u{1680}\u{2000}\u{2001}\u{2002}\u{2003}\u{2004}\u{2005}\u{2006}\u{2007}\u{2008}\u{2009}\u{200A}\u{2028}\u{2029}\u{202F}\u{205F}\u{3000}\u{0085}\u{180E}", ?string $encoding = null): string {}

/** @refcount 1 */
function mb_detect_encoding(string $string, array|string|null $encodings = null, bool $strict = false): string|false {}

/**
 * @return array<int, string>
 */
function mb_list_encodings(): array {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function mb_encoding_aliases(string $encoding): array {}

/** @refcount 1 */
function mb_encode_mimeheader(string $string, ?string $charset = null, ?string $transfer_encoding = null, string $newline = "\r\n", int $indent = 0): string {}

/** @refcount 1 */
function mb_decode_mimeheader(string $string): string {}

/** @refcount 1 */
function mb_convert_kana(string $string, string $mode = "KV", ?string $encoding = null): string {}

/** @refcount 1 */
function mb_convert_variables(string $to_encoding, array|string $from_encoding, mixed &$var, mixed &...$vars): string|false {}

/** @refcount 1 */
function mb_encode_numericentity(string $string, array $map, ?string $encoding = null, bool $hex = false): string {}

/** @refcount 1 */
function mb_decode_numericentity(string $string, array $map, ?string $encoding = null): string {}

function mb_send_mail(string $to, string $subject, string $message, array|string $additional_headers = [], ?string $additional_params = null): bool {}

/**
 * @return array<int|string, int|string|array>|string|int|false
 * @refcount 1
 */
function mb_get_info(string $type = "all"): array|string|int|false {}

function mb_check_encoding(array|string|null $value = null, ?string $encoding = null): bool {}

function mb_scrub(string $string, ?string $encoding = null): string {}

function mb_ord(string $string, ?string $encoding = null): int|false {}

function mb_chr(int $codepoint, ?string $encoding = null): string|false {}

function mb_str_pad(string $string, int $length, string $pad_string = " ", int $pad_type = STR_PAD_RIGHT, ?string $encoding = null): string {}

#ifdef HAVE_MBREGEX
/** @refcount 1 */
function mb_regex_encoding(?string $encoding = null): string|bool {}

/** @param array $matches */
function mb_ereg(string $pattern, string $string, &$matches = null): bool {}

/** @param array $matches */
function mb_eregi(string $pattern, string $string, &$matches = null): bool {}

/** @refcount 1 */
function mb_ereg_replace(string $pattern, string $replacement, string $string, ?string $options = null): string|false|null {}

/** @refcount 1 */
function mb_eregi_replace(string $pattern, string $replacement, string $string, ?string $options = null): string|false|null {}

/** @refcount 1 */
function mb_ereg_replace_callback(string $pattern, callable $callback, string $string, ?string $options = null): string|false|null {}

/**
 * @return array<int, string>|false
 * @refcount 1
 */
function mb_split(string $pattern, string $string, int $limit = -1): array|false {}

function mb_ereg_match(string $pattern, string $string, ?string $options = null): bool {}

function mb_ereg_search(?string $pattern = null, ?string $options = null): bool {}

/**
 * @return array<int, int>|false
 * @refcount 1
 */
function mb_ereg_search_pos(?string $pattern = null, ?string $options = null): array|false {}

/**
 * @return array<int|string, string|false>|false
 * @refcount 1
 */
function mb_ereg_search_regs(?string $pattern = null, ?string $options = null): array|false {}

function mb_ereg_search_init(string $string, ?string $pattern = null, ?string $options = null): bool {}

/**
 * @return array<int|string, string|false>|false
 * @refcount 1
 */
function mb_ereg_search_getregs(): array|false {}

function mb_ereg_search_getpos(): int {}

function mb_ereg_search_setpos(int $offset): bool {}

/** @refcount 1 */
function mb_regex_set_options(?string $options = null): string {}
#endif
