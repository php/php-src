<?php

class SQLite3
{
    function __construct(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryption_key = '') {}

    /** @return void */
    function open(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryption_key = '') {}

    /** @return bool */
    function close() {}

    /** @return array */
    function version() {}

    /** @return int */
    function lastInsertRowID() {}

    /** @return int */
    function lastErrorCode() {}

    /** @return int */
    function lastExtendedErrorCode() {}

    /** @return string */
    function lastErrorMsg() {}

    /** @return int */
    function changes() {}

    /** @return bool */
    function busyTimeout(int $ms) {}

#ifndef SQLITE_OMIT_LOAD_EXTENSION
    /** @return bool */
    function loadExtension(string $shared_library) {}
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
    /** @return bool */
    function backup(SQLite3 $destination_db, string $source_dbname = "main", string $destination_dbname = "main") {}
#endif

    /** @return string */
    function escapeString(string $value) {}

    /** @return SQLite3Stmt|false */
    function prepare(string $query) {}

    /** @return SQLite3Result|false|null */
    function query(string $query) {}

    /** @return mixed */
    function querySingle(string $query, bool $entire_row = false) {}

    /** @return bool */
    function createFunction(string $name, $callback, int $argument_count = -1, int $flags = 0) {}

    /** @return bool */
    function createAggregate(string $name, $step_callback, $final_callback, int $argument_count = -1) {}

    /** @return bool */
    function createCollation(string $name, $callback) {}

    /** @return resource|false */
    function openBlob(string $table, string $column, int $rowid, string $dbname = "main", int $flags = SQLITE3_OPEN_READONLY) {}

    /** @return bool */
    function enableExceptions(bool $enableExceptions = false) {}

    /** @return bool */
    function enableExtendedResultCodes(bool $enable = true) {}
}

class SQLite3Stmt
{
    function __construct(SQLite3 $sqlite3, string $sql) {}

    /** @return bool */
    function bindParam($param_number, &$param, int $type = UNKNOWN) {}

    /** @return bool */
    function bindValue($param_number, $param, int $type = UNKNOWN) {}

    /** @return bool */
    function clear() {}

    /** @return bool */
    function close() {}

    /** @return SQLite3Result|false */
    function execute() {}

    /** @return string|false */
    function getSQL(bool $expanded = false) {}

    /** @return int */
    function paramCount() {}

    /** @return bool */
    function readOnly() {}

    /** @return bool */
    function reset() {}
}

class SQLite3Result
{
    function __construct() {}

    /** @return int */
    function numColumns() {}

    /** @return string|false */
    function columnName(int $column_number) {}

    /** @return int|false */
    function columnType(int $column_number) {}

    /** @return array|false */
    function fetchArray(int $mode = SQLITE3_BOTH) {}

    /** @return bool */
    function reset() {}

    /** @return bool */
    function finalize() {}
}
