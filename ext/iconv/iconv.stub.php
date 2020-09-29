<?php

/** @generate-function-entries */

function iconv_strlen(string $string, ?string $charset = null): int|false {}

function iconv_substr(string $string, int $offset, ?int $length = null, ?string $charset = null): string|false {}

function iconv_strpos(string $haystack, string $needle, int $offset = 0, ?string $charset = null): int|false {}

function iconv_strrpos(string $haystack, string $needle, ?string $charset = null): int|false {}

function iconv_mime_encode(string $field_name, string $field_value, array $options = []): string|false {}

function iconv_mime_decode(string $string, int $mode = 0, ?string $charset = null): string|false {}

function iconv_mime_decode_headers(string $headers, int $mode = 0, ?string $charset = null): array|false {}

function iconv(string $from_charset, string $to_charset, string $string): string|false {}

function iconv_set_encoding(string $type, string $charset): bool {}

function iconv_get_encoding(string $type = "all"): array|string|false {}
