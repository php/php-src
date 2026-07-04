<?php

/** @generate-class-entries */

function grapheme_strlen(string $string): int|false {}

function grapheme_strpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_stripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strrpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_substr(string $string, int $offset, ?int $length = null): string|false {}

function grapheme_strstr(string $haystack, string $needle, bool $beforeNeedle = false): string|false {}

function grapheme_stristr(string $haystack, string $needle, bool $beforeNeedle = false): string|false {}

function grapheme_extract(string $haystack, int $size, int $extractType = 0, int $start = 0, ?int &$next = null): string|false {}

function grapheme_mask(string $string, string $maskChars, int $start = 0, int $length = 0, string $maskChar = "*"): string|false {}
