<?php

function iconv_strlen(string $str, string $charset = UNKNOWN): int|false {}

function iconv_substr(string $str, int $offset, ?int $length = null, string $charset = UNKNOWN): string|false {}

function iconv_strpos(string $haystack, string $needle, int $offset = 0, string $charset = UNKNOWN): int|false {}

function iconv_strrpos(string $haystack, string $needle, string $charset = UNKNOWN): int|false {}

function iconv_mime_encode(string $field_name, string $field_value, array $preference = []): string|false {}

function iconv_mime_decode(string $encoded_string, int $mode = 0, string $charset = UNKNOWN): string|false {}

function iconv_mime_decode_headers(string $headers, int $mode = 0, string $charset = UNKNOWN): array|false {}

function iconv(string $in_charset, string $out_charset, string $str): string|false {}

function iconv_set_encoding(string $type, string $charset): bool {}

function iconv_get_encoding(string $type = 'all'): array|string|false {}
