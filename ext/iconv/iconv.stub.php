<?php

/** @return string|false */
function iconv_strlen(string $str, string $charset = UNKNOWN) {}

/** @return string|false */
function iconv_substr(string $str, int $offset, int $length = UNKNOWN, string $charset = UNKNOWN) {}

/** @return int|false */
function iconv_strpos(string $haystack, string $needle, int $offset = 0, string $charset = UNKNOWN) {}

/** @return int|false */
function iconv_strrpos(string $haystack, string $needle, string $charset = UNKNOWN) {}

/** @return string|false */
function iconv_mime_encode(string $field_name, string $field_value, array $preference = []) {}

/** @return string|false */
function iconv_mime_decode(string $encoded_string, int $mode = 0, string $charset = UNKNOWN) {}

/** @return array|false */
function iconv_mime_decode_headers(string $headers, int $mode = 0, string $charset = UNKNOWN) {}

/** @return string|false */
function iconv(string $in_charset, string $out_charset, string $str) {}

function iconv_set_encoding(string $type, string $charset): bool {}

/** @return array|string|false */
function iconv_get_encoding(string $type = 'all') {}
