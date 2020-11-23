<?php

/** @generate-function-entries */

function mb_language(?string $language = null): string|bool {}

function mb_internal_encoding(?string $encoding = null): string|bool {}

function mb_http_input(?string $type = null): array|string|false {}

function mb_http_output(?string $encoding = null): string|bool {}

function mb_detect_order(array|string|null $encoding = null): array|bool {}

function mb_substitute_character(string|int|null $substitute_character = null): string|int|bool {}

function mb_preferred_mime_name(string $encoding): string|false {}

/** @param array $result */
function mb_parse_str(string $string, &$result): bool {}

function mb_output_handler(string $string, int $status): string {}

function mb_str_split(string $string, int $length = 1, ?string $encoding = null): array {}

function mb_strlen(string $string, ?string $encoding = null): int {}

function mb_strpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_strrpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_stripos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_strripos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function mb_strstr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

function mb_strrchr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

function mb_stristr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

function mb_strrichr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false {}

function mb_substr_count(string $haystack, string $needle, ?string $encoding = null): int {}

function mb_substr(string $string, int $start, ?int $length = null, ?string $encoding = null): string {}

function mb_strcut(string $string, int $start, ?int $length = null, ?string $encoding = null): string {}

function mb_strwidth(string $string, ?string $encoding = null): int {}

function mb_strimwidth(string $string, int $start, int $width, string $trim_marker = "", ?string $encoding = null): string {}

function mb_convert_encoding(array|string $string, string $to_encoding, array|string|null $from_encoding = null): array|string|false {}

function mb_convert_case(string $string, int $mode, ?string $encoding = null): string {}

function mb_strtoupper(string $string, ?string $encoding = null): string {}

function mb_strtolower(string $string, ?string $encoding = null): string {}

function mb_detect_encoding(string $string, array|string|null $encodings = null, bool $strict = false): string|false {}

function mb_list_encodings(): array {}

function mb_encoding_aliases(string $encoding): array {}

function mb_encode_mimeheader(string $string, ?string $charset = null, ?string $transfer_encoding = null, string $newline = "\r\n", int $indent = 0): string {}

function mb_decode_mimeheader(string $string): string {}

function mb_convert_kana(string $string, string $mode = "KV", ?string $encoding = null): string {}

function mb_convert_variables(string $to_encoding, array|string $from_encoding, mixed &$var, mixed &...$vars): string|false {}

function mb_encode_numericentity(string $string, array $map, ?string $encoding = null, bool $hex = false): string {}

function mb_decode_numericentity(string $string, array $map, ?string $encoding = null): string {}

function mb_send_mail(string $to, string $subject, string $message, array|string $additional_headers = [], ?string $additional_params = null): bool {}

function mb_get_info(string $type = "all"): array|string|int|false {}

function mb_check_encoding(array|string|null $value = null, ?string $encoding = null): bool {}

function mb_scrub(string $string, ?string $encoding = null): string {}

function mb_ord(string $string, ?string $encoding = null): int|false {}

function mb_chr(int $codepoint, ?string $encoding = null): string|false {}

#ifdef HAVE_MBREGEX
function mb_regex_encoding(?string $encoding = null): string|bool {}

/** @param array $matches */
function mb_ereg(string $pattern, string $string, &$matches = null): bool {}

/** @param array $matches */
function mb_eregi(string $pattern, string $string, &$matches = null): bool {}

function mb_ereg_replace(string $pattern, string $replacement, string $string, ?string $options = null): string|false|null {}

function mb_eregi_replace(string $pattern, string $replacement, string $string, ?string $options = null): string|false|null {}

function mb_ereg_replace_callback(string $pattern, callable $callback, string $string, ?string $options = null): string|false|null {}

function mb_split(string $pattern, string $string, int $limit = -1): array|false {}

function mb_ereg_match(string $pattern, string $string, ?string $options = null): bool {}

function mb_ereg_search(?string $pattern = null, ?string $options = null): bool {}

function mb_ereg_search_pos(?string $pattern = null, ?string $options = null): array|false {}

function mb_ereg_search_regs(?string $pattern = null, ?string $options = null): array|false {}

function mb_ereg_search_init(string $string, ?string $pattern = null, ?string $options = null): bool {}

function mb_ereg_search_getregs(): array|false {}

function mb_ereg_search_getpos(): int {}

function mb_ereg_search_setpos(int $offset): bool {}

function mb_regex_set_options(?string $options = null): string {}
#endif
