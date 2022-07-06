<?php

/** @generate-function-entries */

class SQLite3
{
    /** @implementation-alias SQLite3::open */
    public function __construct(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryptionKey = "") {}

    /** @return void */
    public function open(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryptionKey = "") {}

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
    public function busyTimeout(int $milliseconds) {}

#ifndef SQLITE_OMIT_LOAD_EXTENSION
    /** @return bool */
    public function loadExtension(string $name) {}
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
    /** @return bool */
    public function backup(SQLite3 $destination, string $sourceDatabase = "main", string $destinationDatabase = "main") {}
#endif

    /** @return string */
    public static function escapeString(string $string) {}

    /** @return SQLite3Stmt|false */
    public function prepare(string $query) {}

    /** @return bool */
    public function exec(string $query) {}

    /** @return SQLite3Result|false */
    public function query(string $query) {}

    /** @return mixed */
    public function querySingle(string $query, bool $entireRow = false) {}

    /** @return bool */
    public function createFunction(string $name, callable $callback, int $argCount = -1, int $flags = 0) {}

    /** @return bool */
    public function createAggregate(string $name, callable $stepCallback, callable $finalCallback, int $argCount = -1) {}

    /** @return bool */
    public function createCollation(string $name, callable $callback) {}

    /** @return resource|false */
    public function openBlob(string $table, string $column, int $rowid, string $database = "main", int $flags = SQLITE3_OPEN_READONLY) {}

    /** @return bool */
    public function enableExceptions(bool $enable = false) {}

    /** @return bool */
    public function enableExtendedResultCodes(bool $enable = true) {}

    /** @return bool */
    public function setAuthorizer(?callable $callback) {}
}

class SQLite3Stmt
{
    private function __construct(SQLite3 $sqlite3, string $query) {}

    /** @return bool */
    public function bindParam(string|int $param, mixed &$var, int $type = SQLITE3_TEXT) {}

    /** @return bool */
    public function bindValue(string|int $param, mixed $value, int $type = SQLITE3_TEXT) {}

    /** @return bool */
    public function clear() {}

    /** @return bool */
    public function close() {}

    /** @return SQLite3Result|false */
    public function execute() {}

    /** @return string|false */
    public function getSQL(bool $expand = false) {}

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
    public function columnName(int $column) {}

    /** @return int|false */
    public function columnType(int $column) {}

    /** @return array|false */
    public function fetchArray(int $mode = SQLITE3_BOTH) {}

    /** @return bool */
    public function reset() {}

    /** @return bool */
    public function finalize() {}
}
