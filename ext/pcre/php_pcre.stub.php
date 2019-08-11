<?php

/** @return int|false */
function preg_match(string $pattern, string $subject, &$subpatterns = null, int $flags = 0, int $offset = 0) {}

/** @return int|false */
function preg_match_all(string $pattern, string $subject, &$subpatterns = null, int $flags = 0, int $offset = 0) {}

/**
 * @param string|array $regex
 * @param string|array $replace
 * @param string|array $subject
 * @return string|array|null
 */
function preg_replace($regex, $replace, $subject, int $limit = -1, &$count = null) {}

/**
 * @param string|array $regex
 * @param string|array $replace
 * @param string|array $subject
 * @return string|array|null
 */
function preg_filter($regex, $replace, $subject, int $limit = -1, &$count = null) {}

/**
 * @param string|array $regex
 * @param string|array $subject
 * @return string|array|null
 *
 * TODO: $callback should be `callable`
 */
function preg_replace_callback($regex, $callback, $subject, int $limit = -1, &$count = null, int $flags = 0) {}

/**
 * @param string|array $subject
 * @return string|array|null
 */
function preg_replace_callback_array(array $pattern, $subject, int $limit = -1, &$count = null, int $flags = 0) {}

/**
 * @return array|false
 */
function preg_split(string $pattern, string $subject, int $limit = -1, int $flags = 0) {}


function preg_quote(string $str, ?string $delim_char = null): string {}

/** @return array|false */
function preg_grep(string $regex, array $input, int $flags = 0) {}


function preg_last_error(): int {}
