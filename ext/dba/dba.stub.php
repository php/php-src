<?php

/** @generate-class-entries */

/**
 * @param string $path
 * @param string $mode
 * @param string $handler
 * @param string $handler_params
 * @return resource|false
 */
function dba_popen($path, $mode, $handler = UNKNOWN, ...$handler_params) {}

/**
 * @param string $path
 * @param string $mode
 * @param string $handler
 * @param string $handler_params
 * @return resource|false
 */
function dba_open($path, $mode, $handler = UNKNOWN, ...$handler_params) {}

/** @param resource $dba */
function dba_close($dba): void {}

/** @param resource $dba */
function dba_exists(string|array $key, $dba): bool {}

/**
 * @param resource|int $dba overloaded legacy signature has params flipped
 * @param resource|int $skip overloaded legacy signature has params flipped
 */
function dba_fetch(string|array $key, $dba, $skip = 0): string|false {}

function dba_key_split(string|false|null $key): array|false {}

/** @param resource $dba */
function dba_firstkey($dba): string|false {}

/** @param resource $dba */
function dba_nextkey($dba): string|false {}

/** @param resource $dba */
function dba_delete(string|array $key, $dba): bool {}

/** @param resource $dba */
function dba_insert(string|array $key, string $value, $dba): bool {}

/** @param resource $dba */
function dba_replace(string|array $key, string $value, $dba): bool {}

/** @param resource $dba */
function dba_optimize($dba): bool {}

/** @param resource $dba */
function dba_sync($dba): bool {}

function dba_handlers(bool $full_info = false): array {}

function dba_list(): array {}
