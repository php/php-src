<?php

function grapheme_strlen(string $input): int|false|null {}

function grapheme_strpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_stripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strrpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_substr(string $string, int $start, ?int $length = 0): string|false {}

function grapheme_strstr(string $haystack, string $needle, bool $before_needle = false): string|false {}

function grapheme_stristr(string $haystack, string $needle, bool $before_needle = false): string|false {}

function grapheme_extract(string $haystack, int $size, int $extract_type = GRAPHEME_EXTR_COUNT, int $start = 0, &$next = null): string|false {}
