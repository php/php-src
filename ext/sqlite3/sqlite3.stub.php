<?php

/** @generate-function-entries */

class SQLite3
{
    /** @alias SQLite3::open */
    public function __construct(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryption_key = '') {}

    /** @return void */
    public function open(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryption_key = '') {}

    /** @return bool */
    public function close() {}

    /** @return array */
    public static function version() {}

    /** @return int */
    public function lastInsertRowID() {}

    /** @return int */
    public function lastErrorCode() {}

    /** @return int */
    public function lastExtendedErrorCode() {}

    /** @return string */
    public function lastErrorMsg() {}

    /** @return int */
    public function changes() {}

    /** @return bool */
    public function busyTimeout(int $ms) {}

#ifndef SQLITE_OMIT_LOAD_EXTENSION
    /** @return bool */
    public function loadExtension(string $shared_library) {}
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
    /** @return bool */
    public function backup(SQLite3 $destination_db, string $source_dbname = "main", string $destination_dbname = "main") {}
#endif

    /** @return string */
    public static function escapeString(string $value) {}

    /** @return SQLite3Stmt|false */
    public function prepare(string $query) {}

    /** @return bool */
    public function exec(string $query) {}

    /** @return SQLite3Result|false|null */
    public function query(string $query) {}

    /** @return mixed */
    public function querySingle(string $query, bool $entire_row = false) {}

    /** @return bool */
    public function createFunction(string $name, $callback, int $argument_count = -1, int $flags = 0) {}

    /** @return bool */
    public function createAggregate(string $name, $step_callback, $final_callback, int $argument_count = -1) {}

    /** @return bool */
    public function createCollation(string $name, $callback) {}

    /** @return resource|false */
    public function openBlob(string $table, string $column, int $rowid, string $dbname = "main", int $flags = SQLITE3_OPEN_READONLY) {}

    /** @return bool */
    public function enableExceptions(bool $enableExceptions = false) {}

    /** @return bool */
    public function enableExtendedResultCodes(bool $enable = true) {}

    /** @return bool */
    public function setAuthorizer(?callable $callback) {}
}

class SQLite3Stmt
{
    private function __construct(SQLite3 $sqlite3, string $sql) {}

    /** @return bool */
    public function bindParam($param_number, &$param, int $type = SQLITE3_TEXT) {}

    /** @return bool */
    public function bindValue($param_number, $param, int $type = SQLITE3_TEXT) {}

    /** @return bool */
    public function clear() {}

    /** @return bool */
    public function close() {}

    /** @return SQLite3Result|false */
    public function execute() {}

    /** @return string|false */
    public function getSQL(bool $expanded = false) {}

    /** @return int */
    public function paramCount() {}

    /** @return bool */
    public function readOnly() {}

    /** @return bool */
    public function reset() {}
}

class SQLite3Result
{
    private function __construct() {}

    /** @return int */
    public function numColumns() {}

    /** @return string|false */
    public function columnName(int $column_number) {}

    /** @return int|false */
    public function columnType(int $column_number) {}

    /** @return array|false */
    public function fetchArray(int $mode = SQLITE3_BOTH) {}

    /** @return bool */
    public function reset() {}

    /** @return bool */
    public function finalize() {}
}
