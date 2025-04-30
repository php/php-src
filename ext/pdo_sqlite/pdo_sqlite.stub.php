<?php

/** @generate-class-entries */

namespace Pdo;

/**
 * @strict-properties
 * @not-serializable
 */
class Sqlite extends \PDO
{
#ifdef SQLITE_DETERMINISTIC
    /** @cvalue SQLITE_DETERMINISTIC */
    public const int DETERMINISTIC = UNKNOWN;
#endif

    /** @cvalue SQLITE_OPEN_READONLY */
    public const int OPEN_READONLY = UNKNOWN;

    /** @cvalue SQLITE_OPEN_READWRITE */
    public const int OPEN_READWRITE = UNKNOWN;

    /** @cvalue SQLITE_OPEN_CREATE */
    public const int OPEN_CREATE = UNKNOWN;

    /** @cvalue PDO_SQLITE_ATTR_OPEN_FLAGS */
    public const int ATTR_OPEN_FLAGS = UNKNOWN;

    /** @cvalue PDO_SQLITE_ATTR_READONLY_STATEMENT */
    public const int ATTR_READONLY_STATEMENT = UNKNOWN;

    /** @cvalue PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES */
    public const int ATTR_EXTENDED_RESULT_CODES = UNKNOWN;

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

#ifndef PDO_SQLITE_OMIT_LOAD_EXTENSION
    public function loadExtension(string $name): void {}
#endif

    /** @return resource|false */
    public function openBlob(
        string $table,
        string $column,
        int $rowid,
        ?string $dbname = "main",
        int $flags = \Pdo\Sqlite::OPEN_READONLY
    ) {}
}
