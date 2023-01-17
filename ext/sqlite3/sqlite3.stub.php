<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_SQLITE3_ASSOC
 */
const SQLITE3_ASSOC = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_SQLITE3_NUM
 */
const SQLITE3_NUM = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_SQLITE3_BOTH
 */
const SQLITE3_BOTH = UNKNOWN;

/**
 * @var int
 * @cvalue SQLITE_INTEGER
 */
const SQLITE3_INTEGER = UNKNOWN;
/**
 * @var int
 * @cvalue SQLITE_FLOAT
 */
const SQLITE3_FLOAT = UNKNOWN;
/**
 * @var int
 * @cvalue SQLITE3_TEXT
 */
const SQLITE3_TEXT = UNKNOWN;
/**
 * @var int
 * @cvalue SQLITE_BLOB
 */
const SQLITE3_BLOB = UNKNOWN;
/**
 * @var int
 * @cvalue SQLITE_NULL
 */
const SQLITE3_NULL = UNKNOWN;

/**
 * @var int
 * @cvalue SQLITE_OPEN_READONLY
 */
const SQLITE3_OPEN_READONLY = UNKNOWN;
/**
 * @var int
 * @cvalue SQLITE_OPEN_READWRITE
 */
const SQLITE3_OPEN_READWRITE = UNKNOWN;
/**
 * @var int
 * @cvalue SQLITE_OPEN_CREATE
 */
const SQLITE3_OPEN_CREATE = UNKNOWN;

#ifdef SQLITE_DETERMINISTIC
/**
 * @var int
 * @cvalue SQLITE_DETERMINISTIC
 */
const SQLITE3_DETERMINISTIC = UNKNOWN;
#endif

/** @not-serializable */
class SQLite3
{
    /**
     * @var int
     * @cvalue SQLITE_OK
     * @link sqlite3.class.constants.ok
     */
    public const OK = UNKNOWN;

    /* Constants for authorizer return */

    /**
     * @var int
     * @cvalue SQLITE_DENY
     * @link sqlite3.class.constants.deny
     */
    public const DENY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_IGNORE
     * @link sqlite3.class.constants.ignore
     */
    public const IGNORE = UNKNOWN;

    /* Constants for authorizer actions */

    /**
     * @var int
     * @cvalue SQLITE_CREATE_INDEX
     * @link sqlite3.class.constants.create-index
     */
    public const CREATE_INDEX = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_TABLE
     * @link sqlite3.class.constants.create-table
     */
    public const CREATE_TABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_TEMP_INDEX
     * @link sqlite3.class.constants.create-temp-index
     */
    public const CREATE_TEMP_INDEX = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_TEMP_TABLE
     * @link sqlite3.class.constants.create-temp-table
     */
    public const CREATE_TEMP_TABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_TEMP_TRIGGER
     * @link sqlite3.class.constants.create-temp-trigger
     */
    public const CREATE_TEMP_TRIGGER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_TEMP_VIEW
     * @link sqlite3.class.constants.create-temp-view
     */
    public const CREATE_TEMP_VIEW = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_TRIGGER
     * @link sqlite3.class.constants.create-trigger
     */
    public const CREATE_TRIGGER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_VIEW
     * @link sqlite3.class.constants.create-view
     */
    public const CREATE_VIEW = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DELETE
     * @link sqlite3.class.constants.delete
     */
    public const DELETE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_INDEX
     * @link sqlite3.class.constants.drop-index
     */
    public const DROP_INDEX = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_TABLE
     * @link sqlite3.class.constants.drop-table
     */
    public const DROP_TABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_TEMP_INDEX
     * @link sqlite3.class.constants.drop-temp-index
     */
    public const DROP_TEMP_INDEX = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_TEMP_TABLE
     * @link sqlite3.class.constants.drop-temp-table
     */
    public const DROP_TEMP_TABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_TEMP_TRIGGER
     * @link sqlite3.class.constants.drop-temp-trigger
     */
    public const DROP_TEMP_TRIGGER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_TEMP_VIEW
     * @link sqlite3.class.constants.drop-temp-view
     */
    public const DROP_TEMP_VIEW = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_TRIGGER
     * @link sqlite3.class.constants.drop-trigger
     */
    public const DROP_TRIGGER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_VIEW
     * @link sqlite3.class.constants.drop-view
     */
    public const DROP_VIEW = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_INSERT
     * @link sqlite3.class.constants.insert
     */
    public const INSERT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_PRAGMA
     * @link sqlite3.class.constants.pragma
     */
    public const PRAGMA = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_READ
     * @link sqlite3.class.constants.read
     */
    public const READ = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_SELECT
     * @link sqlite3.class.constants.select
     */
    public const SELECT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_TRANSACTION
     * @link sqlite3.class.constants.transaction
     */
    public const TRANSACTION = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_UPDATE
     * @link sqlite3.class.constants.update
     */
    public const UPDATE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_ATTACH
     * @link sqlite3.class.constants.attach
     */
    public const ATTACH = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DETACH
     * @link sqlite3.class.constants.detach
     */
    public const DETACH = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_ALTER_TABLE
     * @link sqlite3.class.constants.alter-table
     */
    public const ALTER_TABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_REINDEX
     * @link sqlite3.class.constants.reindex
     */
    public const REINDEX = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_ANALYZE
     * @link sqlite3.class.constants.analyze
     */
    public const ANALYZE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_CREATE_VTABLE
     * @link sqlite3.class.constants.create-vtable
     */
    public const CREATE_VTABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_DROP_VTABLE
     * @link sqlite3.class.constants.drop-vtable
     */
    public const DROP_VTABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_FUNCTION
     * @link sqlite3.class.constants.function
     */
    public const FUNCTION = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_SAVEPOINT
     * @link sqlite3.class.constants.savepoint
     */
    public const SAVEPOINT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQLITE_COPY
     * @link sqlite3.class.constants.copy
     */
    public const COPY = UNKNOWN;
#ifdef SQLITE_RECURSIVE
    /**
     * @var int
     * @cvalue SQLITE_RECURSIVE
     * @link sqlite3.class.constants.recursive
     */
    public const RECURSIVE = UNKNOWN;
#endif

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

/** @not-serializable */
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

/** @not-serializable */
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
