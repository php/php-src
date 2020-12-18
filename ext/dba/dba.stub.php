<?php

/** @generate-function-entries */

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

/**
 * @param string|array $key
 * @param resource $dba
 */
function dba_exists($key, $dba): bool {}

/**
 * @param string|array $key
 * @param int|resource $skip actually this parameter is optional, not $dba
 * @param resource $dba
 */
function dba_fetch($key, $skip, $dba = UNKNOWN): string|false {}

function dba_key_split(string|false|null $key): array|false {}

/** @param resource $dba */
function dba_firstkey($dba): string|false {}

/** @param resource $dba */
function dba_nextkey($dba): string|false {}

/**
 * @param string|array $key
 * @param resource $dba
 */
function dba_delete($key, $dba): bool {}

/**
 * @param string|array $key
 * @param resource $dba
 */
function dba_insert($key, string $value, $dba): bool {}

/**
 * @param string|array $key
 * @param resource $dba
 */
function dba_replace($key, string $value, $dba): bool {}

/** @param resource $dba */
function dba_optimize($dba): bool {}

/** @param resource $dba */
function dba_sync($dba): bool {}

function dba_handlers(bool $full_info = false): array {}

function dba_list(): array {}
