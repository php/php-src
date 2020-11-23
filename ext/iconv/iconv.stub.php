<?php

/** @generate-function-entries */

function iconv_strlen(string $string, ?string $encoding = null): int|false {}

function iconv_substr(string $string, int $offset, ?int $length = null, ?string $encoding = null): string|false {}

function iconv_strpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function iconv_strrpos(string $haystack, string $needle, ?string $encoding = null): int|false {}

function iconv_mime_encode(string $field_name, string $field_value, array $options = []): string|false {}

function iconv_mime_decode(string $string, int $mode = 0, ?string $encoding = null): string|false {}

function iconv_mime_decode_headers(string $headers, int $mode = 0, ?string $encoding = null): array|false {}

function iconv(string $from_encoding, string $to_encoding, string $string): string|false {}

function iconv_set_encoding(string $type, string $encoding): bool {}

function iconv_get_encoding(string $type = "all"): array|string|false {}
