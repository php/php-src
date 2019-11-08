<?php

function mb_language(string $language = UNKNOWN): string|bool {}

function mb_internal_encoding(string $encoding = UNKNOWN): string|bool {}

function mb_http_input(string $type = UNKNOWN): array|string|false {}

function mb_http_output(string $encoding = UNKNOWN): string|bool {}

function mb_detect_order($encoding = UNKNOWN): array|bool {}

function mb_substitute_character($substchar = UNKNOWN): string|int|bool {}

function mb_preferred_mime_name(string $encoding): string|false {}

function mb_parse_str(string $encoded_string, &$result): bool {}

function mb_output_handler(string $contents, int $status): string {}

function mb_str_split(string $str, int $split_length = 1, string $encoding = UNKNOWN): array|false {}

function mb_strlen(string $str, string $encoding = UNKNOWN): int|false {}

function mb_strpos(string $haystack, string $needle, int $offset = 0, string $encoding = UNKNOWN): int|false {}

function mb_strrpos(string $haystack, string $needle, $offset = UNBEK, string $encoding = UNKNOWN): int|false {}

function mb_stripos(string $haystack, string $needle, int $offset = 0, string $encoding = UNKNOWN): int|false {}

function mb_strripos(string $haystack, string $needle, int $offset = 0, string $encoding = UNKNOWN): int|false {}

function mb_strstr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN): string|false {}

function mb_strrchr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN): string|false {}

function mb_stristr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN): string|false {}

function mb_strrichr(string $haystack, string $needle, bool $part = false, string $encoding = UNKNOWN): string|false {}

function mb_substr_count(string $haystack, string $needle, string $encoding = UNKNOWN): int|false {}

function mb_substr(string $str, int $start, ?int $length = null, string $encoding = UNKNOWN): string|false {}

function mb_strcut(string $str, int $start, ?int $length = null, string $encoding = UNKNOWN): string|false {}

function mb_strwidth(string $str, string $encoding = UNKNOWN): int|false {}

function mb_strimwidth(string $str, int $start, int $width, string $trimmarker = UNKNOWN, string $encoding = UNKNOWN): string|false {}

/** @param array|string $str */
function mb_convert_encoding($str, string $to, $from = UNKNOWN): array|string|false {}

function mb_convert_case(string $sourcestring, int $mode, ?string $encoding = null): string|false {}

function mb_strtoupper(string $sourcestring, ?string $encoding = null): string|false {}

function mb_strtolower(string $sourcestring, ?string $encoding = null): string|false {}

/** @param array|string $encoding_list */
function mb_detect_encoding(string $str, $encoding_list = null, bool $strict = false): string|false {}

function mb_list_encodings(): array {}

function mb_encoding_aliases(string $encoding): array|false {}

function mb_encode_mimeheader(string $str, string $charset = UNKNOWN, string $transfer = UNKNOWN, string $linefeed = UNKNOWN, int $indent = 0): string|false {}

function mb_decode_mimeheader(string $string): string|false {}

function mb_convert_kana(string $str, string $option = UNKNOWN, string $encoding = UNKNOWN): string|false {}

/** @param array|string $from */
function mb_convert_variables(string $to, $from, &$var, &...$vars): string|false {}

function mb_encode_numericentity(string $string, array $convmap, string $encoding = UNKNOWN, bool $is_hex = false): string|false {}

function mb_decode_numericentity(string $string, array $convmap, string $encoding = UNKNOWN): string|false {}

function mb_send_mail(string $to, string $subject, string $message, $additional_headers = UNKNOWN, string $additional_parameters = UNKNOWN): bool {}

function mb_get_info(string $type = UNKNOWN): array|string|int|false {}

function mb_check_encoding($var = UNBEK, string $encoding = UNKNOWN): bool {}

function mb_scrub(string $str, string $encoding = UNKNOWN): string|false {}

function mb_ord(string $str, string $encoding = UNKNOWN): int|false {}

function mb_chr(int $cp, string $encoding = UNKNOWN): string|false {}

#if HAVE_MBREGEX
function mb_regex_encoding(string $encoding = UNKNOWN): string|bool {}

function mb_ereg(string $pattern, string $string, &$registers = UNKNOWN): int|false {}

function mb_eregi(string $pattern, string $string, &$registers = UNKNOWN): int|false {}

function mb_ereg_replace(string $pattern, string $replacement, string $string, string $option = UNKNOWN): string|false|null {}

function mb_eregi_replace(string $pattern, string $replacement, string $string, string $option = UNKNOWN): string|false|null {}

function mb_ereg_replace_callback(string $pattern, callable $callback, string $string, string $option = UNKNOWN): string|false|null {}

function mb_split(string $pattern, string $string, int $limit = -1): array|false {}

function mb_ereg_match(string $pattern, string $string, string $option = UNKNOWN): bool {}

function mb_ereg_search(string $pattern = UNKNOWN, string $option = UNKNOWN): array|bool {}

function mb_ereg_search_pos(string $pattern = UNKNOWN, string $option = UNKNOWN): array|bool {}

function mb_ereg_search_regs(string $pattern = UNKNOWN, string $option = UNKNOWN): array|bool {}

function mb_ereg_search_init(string $string, string $pattern = UNKNOWN, string $option = UNKNOWN): bool {}

function mb_ereg_search_getregs(): array|false {}

function mb_ereg_search_getpos(): int {}

function mb_ereg_search_setpos(int $position): bool {}

function mb_regex_set_options(string $options = UNKNOWN): string {}
#endif
