<?php

/** @generate-function-entries */

function preg_match(string $pattern, string $subject, &$subpatterns = null, int $flags = 0, int $offset = 0): int|false {}

function preg_match_all(string $pattern, string $subject, &$subpatterns = null, int $flags = 0, int $offset = 0): int|false|null {}

function preg_replace(string|array $regex, string|array $replace, string|array $subject, int $limit = -1, &$count = null): string|array|null {}

function preg_filter(string|array $regex, string|array $replace, string|array $subject, int $limit = -1, &$count = null): string|array|null {}

function preg_replace_callback(string|array $regex, callable $callback, string|array $subject, int $limit = -1, &$count = null, int $flags = 0): string|array|null {}

/** @param string|array $subject */
function preg_replace_callback_array(array $pattern, $subject, int $limit = -1, &$count = null, int $flags = 0): string|array|null {}

function preg_split(string $pattern, string $subject, int $limit = -1, int $flags = 0): array|false {}

function preg_quote(string $str, ?string $delim_char = null): string {}

function preg_grep(string $regex, array $input, int $flags = 0): array|false {}

function preg_last_error(): int {}

function preg_last_error_msg(): string {}
