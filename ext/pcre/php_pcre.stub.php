<?php

/** @generate-function-entries */

/** @param array $matches */
function preg_match(string $pattern, string $subject, &$matches = null, int $flags = 0, int $offset = 0): int|false {}

/** @param array $matches */
function preg_match_all(string $pattern, string $subject, &$matches = null, int $flags = 0, int $offset = 0): int|false {}

/** @param int $count */
function preg_replace(string|array $pattern, string|array $replacement, string|array $subject, int $limit = -1, &$count = null): string|array|null {}

/** @param int $count */
function preg_filter(string|array $pattern, string|array $replacement, string|array $subject, int $limit = -1, &$count = null): string|array|null {}

/** @param int $count */
function preg_replace_callback(string|array $pattern, callable $callback, string|array $subject, int $limit = -1, &$count = null, int $flags = 0): string|array|null {}

/** @param int $count */
function preg_replace_callback_array(array $pattern, string|array $subject, int $limit = -1, &$count = null, int $flags = 0): string|array|null {}

function preg_split(string $pattern, string $subject, int $limit = -1, int $flags = 0): array|false {}

function preg_quote(string $str, ?string $delimiter = null): string {}

function preg_grep(string $pattern, array $array, int $flags = 0): array|false {}

function preg_last_error(): int {}

function preg_last_error_msg(): string {}
