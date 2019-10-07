<?php

/** @return string|false */
function mb_language(string $language = UNKNOWN) {}

/** @return string|false */
function mb_internal_encoding(string $encoding = UNKNOWN) {}

/** @return array|string|false */
function mb_http_input(string $type = UNKNOWN) {}

/** @return string|false */
function mb_http_output(string $encoding = UNKNOWN) {}

/** @return array|bool */
function mb_detect_order($encoding = UNKNOWN) {}

/** @return string|int|bool */
function mb_substitute_character($substchar = UNKNOWN) {}

/** @return string|false */
function mb_preferred_mime_name(string $encoding) {}

function mb_parse_str(string $encoded_string, &$result): bool {}

function mb_output_handler(string $contents, int $status): string {}

/** @return array|false */
function mb_str_split(string $str, int $split_length = 1, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_strlen(string $str, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_strpos(string $haystack, string $needle, int $offset = 0, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_strrpos(string $haystack, string $needle, $offset = UNBEK, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_stripos(string $haystack, string $needle, int $offset = 0, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_strripos(string $haystack, string $needle, int $offset = 0, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_strstr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_strrchr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_stristr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_strrichr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_substr_count(string $haystack, string $needle, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_substr(string $str, int $start, ?int $length = null, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_strcut(string $str, int $start, ?int $length = null, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_strwidth(string $str, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_strimwidth(string $str, int $start, int $width, string $trimmarker = UNKNOWN, string $encoding = UNKNOWN) {}

/**
 * @param array|string $str
 * @return array|string|false
 */
function mb_convert_encoding($str, string $to, $from = UNKNOWN) {}

/** @return string|false */
function mb_convert_case(string $sourcestring, int $mode, ?string $encoding = null) {}

/** @return string|false */
function mb_strtoupper(string $sourcestring, ?string $encoding = null) {}

/** @return string|false */
function mb_strtolower(string $sourcestring, ?string $encoding = null) {}

/**
 * @param array|string $encoding_list
 * @return string|false
 */
function mb_detect_encoding(string $str, $encoding_list = null, bool $strict = false) {}

function mb_list_encodings(): array {}

/** @return array|false */
function mb_encoding_aliases(string $encoding) {}

/** @return string|false */
function mb_encode_mimeheader(string $str, string $charset = UNKNOWN, string $transfer = UNKNOWN, string $linefeed = UNKNOWN, int $indent = 0) {}

/** @return string|false */
function mb_decode_mimeheader(string $string) {}

/** @return string|false */
function mb_convert_kana(string $str, string $option = UNKNOWN, string $encoding = UNKNOWN) {}

/**
 * @param array|string $from
 * @return string|false
 */
function mb_convert_variables(string $to, $from, &$var, &...$vars) {}

/** @return string|false */
function mb_encode_numericentity(string $string, array $convmap, string $encoding = UNKNOWN, bool $is_hex = false) {}

/** @return string|false */
function mb_decode_numericentity(string $string, array $convmap, string $encoding = UNKNOWN) {}

function mb_send_mail(string $to, string $subject, string $message, $additional_headers = UNKNOWN, string $additional_parameters = UNKNOWN): bool {}

/** @return array|string|int|false */
function mb_get_info(string $type = UNKNOWN) {}

function mb_check_encoding($var = UNBEK, string $encoding = UNKNOWN): bool {}

/** @return string|false */
function mb_scrub(string $str, string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_ord(string $str, string $encoding = UNKNOWN) {}

/** @return string|false */
function mb_chr(int $cp, string $encoding = UNKNOWN) {}

#if HAVE_MBREGEX
/** @return string|bool */
function mb_regex_encoding(string $encoding = UNKNOWN) {}

/** @return int|false */
function mb_ereg(string $pattern, string $string, &$registers = UNKNOWN) {}

/** @return int|false */
function mb_eregi(string $pattern, string $string, &$registers = UNKNOWN) {}

/** @return string|false|null */
function mb_ereg_replace(string $pattern, string $replacement, string $string, string $option = UNKNOWN) {}

/** @return string|false|null */
function mb_eregi_replace(string $pattern, string $replacement, string $string, string $option = UNKNOWN) {}

/** @return string|false|null */
function mb_ereg_replace_callback(string $pattern, callable $callback, string $string, string $option = UNKNOWN) {}

/** @return array|false */
function mb_split(string $pattern, string $string, int $limit = -1) {}

function mb_ereg_match(string $pattern, string $string, string $option = UNKNOWN): bool {}

/** @return array|bool */
function mb_ereg_search(string $pattern = UNKNOWN, string $option = UNKNOWN) {}

/** @return array|bool */
function mb_ereg_search_pos(string $pattern = UNKNOWN, string $option = UNKNOWN) {}

/** @return array|bool */
function mb_ereg_search_regs(string $pattern = UNKNOWN, string $option = UNKNOWN) {}

function mb_ereg_search_init(string $string, string $pattern = UNKNOWN, string $option = UNKNOWN): bool {}

/** @return array|false */
function mb_ereg_search_getregs() {}

function mb_ereg_search_getpos(): int {}

function mb_ereg_search_setpos(int $position): bool {}

function mb_regex_set_options(string $options = UNKNOWN): string {}
#endif
