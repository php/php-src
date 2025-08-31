<?php

/** @generate-class-entries */

/**
 * @var string
 * @cvalue PHP_ICONV_IMPL_VALUE
 */
const ICONV_IMPL = UNKNOWN;
/**
 * @var string
 * @cvalue get_iconv_version()
 */
const ICONV_VERSION = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_ICONV_MIME_DECODE_STRICT
 */
const ICONV_MIME_DECODE_STRICT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR
 */
const ICONV_MIME_DECODE_CONTINUE_ON_ERROR = UNKNOWN;

function iconv_strlen(string $string, ?string $encoding = null): int|false {}

/** @refcount 1 */
function iconv_substr(string $string, int $offset, ?int $length = null, ?string $encoding = null): string|false {}

function iconv_strpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false {}

function iconv_strrpos(string $haystack, string $needle, ?string $encoding = null): int|false {}

/** @refcount 1 */
function iconv_mime_encode(string $field_name, string $field_value, array $options = []): string|false {}

/** @refcount 1 */
function iconv_mime_decode(string $string, int $mode = 0, ?string $encoding = null): string|false {}

/**
 * @return array<string, string|array>|false
 * @refcount 1
 */
function iconv_mime_decode_headers(string $headers, int $mode = 0, ?string $encoding = null): array|false {}

/** @refcount 1 */
function iconv(string $from_encoding, string $to_encoding, string $string): string|false {}

function iconv_set_encoding(string $type, string $encoding): bool {}

/**
 * @return array<string, string>|string|false
 * @refcount 1
 */
function iconv_get_encoding(string $type = "all"): array|string|false {}
