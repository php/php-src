<?php

class SQLite3
{
    function __construct(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, ?string $encryption_key = null);

    function open(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, ?string $encryption_key = null): void;

    function close(): void;

    function version(): void;

    function lastInsertRowID(): void;

    function lastErrorCode(): void;

    function lastExtendedErrorCode(): void;

    function lastErrorMsg(): void;

    function changes(): void;

    function busyTimeout(int $ms): bool;

#ifndef SQLITE_OMIT_LOAD_EXTENSION
    function loadExtension(string $shared_library): bool;
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
    function backup(SQLite3 $destination_db, string $source_dbname = "main", string $destination_dbname = "main"): bool;
#endif

    function escapeString(string $value): string;

    /** @return SQLite3Result|false */
    function query(string $query);

    /** @return mixed */
    function querySingle(string $query, bool $entire_row = false);

    function createFunction(string $name, $callback, int $argument_count = -1, int $flags = 0): bool;

    function createAggregate(string $name, $step_callback, $final_callback, int $argument_count = -1): bool;

    function createCollation(string $name, $callback): bool;

    /** @return resource|false */
    function openBlob(string $table, string $column, int $rowid, string $dbname = "main", int $flags = SQLITE3_OPEN_READONLY);

    function enableExceptions(bool $enableExceptions = false): bool;

    function enableExtendedResultCodes(bool $enable = true): bool;
}

class SQLite3Stmt
{
    function __construct(SQLite3 $sqlite3);

    function bindParam($param_number, &$param, int $type = SQLITE3_TEXT): bool;

    function bindValue($param_number, $param, int $type = SQLITE3_TEXT): bool;

    function clear(): bool;

    function close(): bool;

    /** @return SQLite3Result|false */
    function execute();

    /** @return string|false */
    function getSQL(bool $expanded = false);

    function paramCount(): int;

    function readOnly(): bool;

    function reset(): bool;
}

class SQLite3Result
{
    function __construct();

    function numColumns(): void;

    /** @return int|false */
    function columnName(int $column_number);

    /** @return int|false */
    function columnType(int $column_number);

    /** @return array|false */
    function fetchArray(int $mode = SQLITE3_BOTH);

    function reset(): void;

    function finalize(): void;
}
