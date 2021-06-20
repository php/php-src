<?php

/** @generate-class-entries */

class SQLite3
{
    /**
     * @implementation-alias SQLite3::open
     * @no-verify SQLite3::open should really be static
     */
    public function __construct(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryptionKey = "") {}

    /** @tentative-return-type */
    public function open(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryptionKey = ""): void {}

    /** @return bool */
    public function close() {} // TODO make return type void

    /** @tentative-return-type */
    public static function version(): array {}

    /** @tentative-return-type */
    public function lastInsertRowID(): int {}

    /** @tentative-return-type */
    public function lastErrorCode(): int {}

    /** @tentative-return-type */
    public function lastExtendedErrorCode(): int {}

    /** @tentative-return-type */
    public function lastErrorMsg(): string {}

    /** @tentative-return-type */
    public function changes(): int {}

    /** @tentative-return-type */
    public function busyTimeout(int $milliseconds): bool {}

#ifndef SQLITE_OMIT_LOAD_EXTENSION
    /** @tentative-return-type */
    public function loadExtension(string $name): bool {}
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
    /** @tentative-return-type */
    public function backup(SQLite3 $destination, string $sourceDatabase = "main", string $destinationDatabase = "main"): bool {}
#endif

    /** @tentative-return-type */
    public static function escapeString(string $string): string {}

    /** @tentative-return-type */
    public function prepare(string $query): SQLite3Stmt|false {}

    /** @tentative-return-type */
    public function exec(string $query): bool {}

    /** @tentative-return-type */
    public function query(string $query): SQLite3Result|false {}

    /** @tentative-return-type */
    public function querySingle(string $query, bool $entireRow = false): mixed {}

    /** @tentative-return-type */
    public function createFunction(string $name, callable $callback, int $argCount = -1, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function createAggregate(string $name, callable $stepCallback, callable $finalCallback, int $argCount = -1): bool {}

    /** @tentative-return-type */
    public function createCollation(string $name, callable $callback): bool {}

    /** @return resource|false */
    public function openBlob(string $table, string $column, int $rowid, string $database = "main", int $flags = SQLITE3_OPEN_READONLY) {}

    /** @tentative-return-type */
    public function enableExceptions(bool $enable = false): bool {}

    /** @tentative-return-type */
    public function enableExtendedResultCodes(bool $enable = true): bool {}

    /** @tentative-return-type */
    public function setAuthorizer(?callable $callback): bool {}
}

class SQLite3Stmt
{
    private function __construct(SQLite3 $sqlite3, string $query) {}

    /** @tentative-return-type */
    public function bindParam(string|int $param, mixed &$var, int $type = SQLITE3_TEXT): bool {}

    /** @tentative-return-type */
    public function bindValue(string|int $param, mixed $value, int $type = SQLITE3_TEXT): bool {}

    /** @tentative-return-type */
    public function clear(): bool {}

    /** @tentative-return-type */
    public function close(): bool {}

    /** @tentative-return-type */
    public function execute(): SQLite3Result|false {}

    /** @tentative-return-type */
    public function getSQL(bool $expand = false): string|false {}

    /** @tentative-return-type */
    public function paramCount(): int {}

    /** @tentative-return-type */
    public function readOnly(): bool {}

    /** @tentative-return-type */
    public function reset(): bool {}
}

class SQLite3Result
{
    private function __construct() {}

    /** @tentative-return-type */
    public function numColumns(): int {}

    /** @tentative-return-type */
    public function columnName(int $column): string|false {}

    /** @tentative-return-type */
    public function columnType(int $column): int|false {}

    /** @tentative-return-type */
    public function fetchArray(int $mode = SQLITE3_BOTH): array|false {}

    /** @tentative-return-type */
    public function reset(): bool {}

    /** @return bool */
    public function finalize() {} // TODO make return type void
}
