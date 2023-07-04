<?php

/** @generate-class-entries */

/** @not-serializable */
class PdoSqlite extends PDO
{
    /**
     * @var int
     * @cname SQLITE_DETERMINISTIC
     */
    public const DETERMINISTIC = UNKNOWN;

    /**
     * @var int
     * @cname SQLITE_OPEN_READONLY
     */
    public const OPEN_READONLY = UNKNOWN;

    /**
     * @var int
     * @cname SQLITE_OPEN_READWRITE
     */
    public const OPEN_READWRITE = UNKNOWN;

    /**
     * @var int
     * @cname SQLITE_OPEN_CREATE
     */
    public const OPEN_CREATE = UNKNOWN;

    /**
     * @var int
     * @cname PDO_SQLITE_ATTR_OPEN_FLAGS
     */
    public const ATTR_OPEN_FLAGS = UNKNOWN;

    /**
     * @var int
     * @cname PDO_SQLITE_ATTR_READONLY_STATEMENT
     */
    public const ATTR_READONLY_STATEMENT = UNKNOWN;

    /**
     * @var int
     * @cname PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES
     */
    public const ATTR_EXTENDED_RESULT_CODES = UNKNOWN;

    // Registers an aggregating User Defined Function for use in SQL statements
    public function createAggregate(
        string $name,
        callable $step,
        callable $finalize,
        int $numArgs = -1
    ): bool {}

    // Registers a User Defined Function for use as a collating function in SQL statements
    public function createCollation(string $name, callable $callback): bool {}

    public function createFunction(
        string $function_name,
        callable $callback,
        int $num_args = -1,
        int $flags = 0
    ): bool {}

// PDO_SQLITE_OMIT_LOAD_EXTENSION might be defined by ext/pdo_sqlite/config.m4
// if Sqlite3 did not have the sqlite3_load_extension function present
// which can deepend on how SQLite was compiled: https://www.sqlite.org/compile.html
#ifndef PDO_SQLITE_OMIT_LOAD_EXTENSION
    public function loadExtension(string $name): bool {}
#endif

    public function openBlob(
        string $table,
        string $column,
        int $rowid,
        ?string $dbname = "main", //null,
        int $flags = PdoSqlite::OPEN_READONLY
    ): mixed /* resource|false */ {}
}
