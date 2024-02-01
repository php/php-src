<?php

/** @generate-class-entries */

#ifdef DBA_LMDB
/** @var int */
const DBA_LMDB_USE_SUB_DIR = 0;
/**
 * @var int
 * @cvalue MDB_NOSUBDIR
 */
const DBA_LMDB_NO_SUB_DIR = UNKNOWN;
#endif

/** @return resource|false */
function dba_popen(string $path, string $mode, ?string $handler = null, int $permission = 0o644, int $map_size = 0, ?int $flags = null) {}

/** @return resource|false */
function dba_open(string $path, string $mode, ?string $handler = null, int $permission = 0o644, int $map_size = 0, ?int $flags = null) {}

/** @param resource $dba */
function dba_close($dba): void {}

/** @param resource $dba */
function dba_exists(string|array $key, $dba): bool {}

/**
 * @param resource|int $dba overloaded legacy signature has params flipped
 * @param resource|int $skip overloaded legacy signature has params flipped
 */
function dba_fetch(string|array $key, $dba, $skip = 0): string|false {}

/** @return array<int, string>|false */
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

/**
 * @return array<int|string, string>
 * @refcount 1
 */
function dba_handlers(bool $full_info = false): array {}

/** @return array<int, string> */
function dba_list(): array {}
