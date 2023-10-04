<?php

/** @generate-class-entries */

/** @not-serializable */
class PDO
{
    /**
     * @cvalue LONG_CONST(PDO_PARAM_NULL)
     */
    public const int PARAM_NULL = 0;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_BOOL)
     */
    public const int PARAM_BOOL = 5;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_INT)
     */
    public const int PARAM_INT = 1;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_STR)
     */
    public const int PARAM_STR = 2;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_LOB)
     */
    public const int PARAM_LOB = 3;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_STMT)
     */
    public const int PARAM_STMT = 4;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_INPUT_OUTPUT)
     */
    public const int PARAM_INPUT_OUTPUT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_STR_NATL)
     */
    public const int PARAM_STR_NATL = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_STR_CHAR)
     */
    public const int PARAM_STR_CHAR = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_ALLOC)
     */
    public const int PARAM_EVT_ALLOC = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_FREE)
     */
    public const int PARAM_EVT_FREE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_EXEC_PRE)
     */
    public const int PARAM_EVT_EXEC_PRE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_EXEC_POST)
     */
    public const int PARAM_EVT_EXEC_POST = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_FETCH_PRE)
     */
    public const int PARAM_EVT_FETCH_PRE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_FETCH_POST)
     */
    public const int PARAM_EVT_FETCH_POST = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_PARAM_EVT_NORMALIZE)
     */
    public const int PARAM_EVT_NORMALIZE = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_FETCH_USE_DEFAULT)
     */
    public const int FETCH_DEFAULT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_LAZY)
     */
    public const int FETCH_LAZY = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_ASSOC)
     */
    public const int FETCH_ASSOC = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_NUM)
     */
    public const int FETCH_NUM = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_BOTH)
     */
    public const int FETCH_BOTH = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_OBJ)
     */
    public const int FETCH_OBJ = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_BOUND)
     */
    public const int FETCH_BOUND = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_COLUMN)
     */
    public const int FETCH_COLUMN = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_CLASS)
     */
    public const int FETCH_CLASS = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_INTO)
     */
    public const int FETCH_INTO = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_FUNC)
     */
    public const int FETCH_FUNC = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_GROUP)
     */
    public const int FETCH_GROUP = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_UNIQUE)
     */
    public const int FETCH_UNIQUE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_KEY_PAIR)
     */
    public const int FETCH_KEY_PAIR = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_CLASSTYPE)
     */
    public const int FETCH_CLASSTYPE = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_FETCH_SERIALIZE)
     */
    public const int FETCH_SERIALIZE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_PROPS_LATE)
     */
    public const int FETCH_PROPS_LATE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_NAMED)
     */
    public const int FETCH_NAMED = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_ATTR_AUTOCOMMIT)
     */
    public const int ATTR_AUTOCOMMIT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_PREFETCH)
     */
    public const int ATTR_PREFETCH = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_TIMEOUT)
     */
    public const int ATTR_TIMEOUT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_ERRMODE)
     */
    public const int ATTR_ERRMODE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_SERVER_VERSION)
     */
    public const int ATTR_SERVER_VERSION = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_CLIENT_VERSION)
     */
    public const int ATTR_CLIENT_VERSION = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_SERVER_INFO)
     */
    public const int ATTR_SERVER_INFO = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_CONNECTION_STATUS)
     */
    public const int ATTR_CONNECTION_STATUS = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_CASE)
     */
    public const int ATTR_CASE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_CURSOR_NAME)
     */
    public const int ATTR_CURSOR_NAME = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_CURSOR)
     */
    public const int ATTR_CURSOR = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_ORACLE_NULLS)
     */
    public const int ATTR_ORACLE_NULLS = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_PERSISTENT)
     */
    public const int ATTR_PERSISTENT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_STATEMENT_CLASS)
     */
    public const int ATTR_STATEMENT_CLASS = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_FETCH_TABLE_NAMES)
     */
    public const int ATTR_FETCH_TABLE_NAMES = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_FETCH_CATALOG_NAMES)
     */
    public const int ATTR_FETCH_CATALOG_NAMES = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_DRIVER_NAME)
     */
    public const int ATTR_DRIVER_NAME = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_STRINGIFY_FETCHES)
     */
    public const int ATTR_STRINGIFY_FETCHES = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_MAX_COLUMN_LEN)
     */
    public const int ATTR_MAX_COLUMN_LEN = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_EMULATE_PREPARES)
     */
    public const int ATTR_EMULATE_PREPARES = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_DEFAULT_FETCH_MODE)
     */
    public const int ATTR_DEFAULT_FETCH_MODE = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ATTR_DEFAULT_STR_PARAM)
     */
    public const int ATTR_DEFAULT_STR_PARAM = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_ERRMODE_SILENT)
     */
    public const int ERRMODE_SILENT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ERRMODE_WARNING)
     */
    public const int ERRMODE_WARNING = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_ERRMODE_EXCEPTION)
     */
    public const int ERRMODE_EXCEPTION = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_CASE_NATURAL)
     */
    public const int CASE_NATURAL = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_CASE_LOWER)
     */
    public const int CASE_LOWER = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_CASE_UPPER)
     */
    public const int CASE_UPPER = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_NULL_NATURAL)
     */
    public const int NULL_NATURAL = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_NULL_EMPTY_STRING)
     */
    public const int NULL_EMPTY_STRING = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_NULL_TO_STRING)
     */
    public const int NULL_TO_STRING = UNKNOWN;

    /**
     * @cvalue PDO_ERR_NONE
     */
    public const string ERR_NONE = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_FETCH_ORI_NEXT)
     */
    public const int FETCH_ORI_NEXT = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_ORI_PRIOR)
     */
    public const int FETCH_ORI_PRIOR = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_ORI_FIRST)
     */
    public const int FETCH_ORI_FIRST = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_ORI_LAST)
     */
    public const int FETCH_ORI_LAST = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_ORI_ABS)
     */
    public const int FETCH_ORI_ABS = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_FETCH_ORI_REL)
     */
    public const int FETCH_ORI_REL = UNKNOWN;

    /**
     * @cvalue LONG_CONST(PDO_CURSOR_FWDONLY)
     */
    public const int CURSOR_FWDONLY = UNKNOWN;
    /**
     * @cvalue LONG_CONST(PDO_CURSOR_SCROLL)
     */
    public const int CURSOR_SCROLL = UNKNOWN;

    public function __construct(string $dsn, ?string $username = null, #[\SensitiveParameter] ?string $password = null, ?array $options = null) {}

    /** @tentative-return-type */
    public function beginTransaction(): bool {}

    /** @tentative-return-type */
    public function commit(): bool {}

    /** @tentative-return-type */
    public function errorCode(): ?string {}

    /** @tentative-return-type */
    public function errorInfo(): array {}

    /** @tentative-return-type */
    public function exec(string $statement): int|false {}

    /** @tentative-return-type */
    public function getAttribute(int $attribute): mixed {}

    /** @tentative-return-type */
    public static function getAvailableDrivers(): array {}

    /** @tentative-return-type */
    public function inTransaction(): bool {}

    /** @tentative-return-type */
    public function lastInsertId(?string $name = null): string|false {}

    /** @tentative-return-type */
    public function prepare(string $query, array $options = []): PDOStatement|false {}

    /** @tentative-return-type */
    public function query(string $query, ?int $fetchMode = null, mixed ...$fetchModeArgs): PDOStatement|false {}

    /** @tentative-return-type */
    public function quote(string $string, int $type = PDO::PARAM_STR): string|false {}

    /** @tentative-return-type */
    public function rollBack(): bool {}

    /** @tentative-return-type */
    public function setAttribute(int $attribute, mixed $value): bool {}
}
