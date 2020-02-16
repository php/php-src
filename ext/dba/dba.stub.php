<?php

/** @return resource|false */
function dba_popen($path, $mode, $handlername = UNKNOWN, ...$handler_parameters) {}

/** @return resource|false */
function dba_open($path, $mode, $handlername = UNKNOWN, ...$handler_parameters) {}

/** @param resource $handle */
function dba_close($handle): void {}

/**
 * @param resource $handle
 */
function dba_exists(string|array $key, $handle): bool {}

/**
 * @param int|resource $skip actually this parameter is optional, not $handle
 * @param resource $handle
 */
function dba_fetch(string|array $key, $skip, $handle = UNKOWN): string|false {}

function dba_key_split(string $key): array|false {}

/** @param resource $handle */
function dba_firstkey($handle): string|false {}

/** @param resource $handle */
function dba_nextkey($handle): string|false {}

/**
 * @param resource $handle
 */
function dba_delete(string|array $key, $handle): bool {}

/**
 * @param resource $handle
 */
function dba_insert(string|array $key, string $value, $handle): bool {}

/**
 * @param resource $handle
 */
function dba_replace(string|array $key, string $value, $handle): bool {}

/** @param resource $handle */
function dba_optimize($handle): bool {}

/** @param resource $handle */
function dba_sync($handle): bool {}

function dba_handlers(bool $full_info = false): array {}

function dba_list(): array {}
