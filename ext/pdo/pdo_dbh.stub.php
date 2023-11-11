<?php

/** @generate-class-entries */

/** @not-serializable */
class PDO
{
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_NULL)
     */
    public const PARAM_NULL = 0;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_BOOL)
     */
    public const PARAM_BOOL = 5;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_INT)
     */
    public const PARAM_INT = 1;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_STR)
     */
    public const PARAM_STR = 2;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_LOB)
     */
    public const PARAM_LOB = 3;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_STMT)
     */
    public const PARAM_STMT = 4;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_INPUT_OUTPUT)
     */
    public const PARAM_INPUT_OUTPUT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_STR_NATL)
     */
    public const PARAM_STR_NATL = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_STR_CHAR)
     */
    public const PARAM_STR_CHAR = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_ALLOC)
     */
    public const PARAM_EVT_ALLOC = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_FREE)
     */
    public const PARAM_EVT_FREE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_EXEC_PRE)
     */
    public const PARAM_EVT_EXEC_PRE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_EXEC_POST)
     */
    public const PARAM_EVT_EXEC_POST = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_FETCH_PRE)
     */
    public const PARAM_EVT_FETCH_PRE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_FETCH_POST)
     */
    public const PARAM_EVT_FETCH_POST = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_PARAM_EVT_NORMALIZE)
     */
    public const PARAM_EVT_NORMALIZE = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_USE_DEFAULT)
     */
    public const FETCH_DEFAULT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_LAZY)
     */
    public const FETCH_LAZY = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ASSOC)
     */
    public const FETCH_ASSOC = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_NUM)
     */
    public const FETCH_NUM = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_BOTH)
     */
    public const FETCH_BOTH = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_OBJ)
     */
    public const FETCH_OBJ = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_BOUND)
     */
    public const FETCH_BOUND = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_COLUMN)
     */
    public const FETCH_COLUMN = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_CLASS)
     */
    public const FETCH_CLASS = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_INTO)
     */
    public const FETCH_INTO = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_FUNC)
     */
    public const FETCH_FUNC = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_GROUP)
     */
    public const FETCH_GROUP = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_UNIQUE)
     */
    public const FETCH_UNIQUE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_KEY_PAIR)
     */
    public const FETCH_KEY_PAIR = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_CLASSTYPE)
     */
    public const FETCH_CLASSTYPE = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_SERIALIZE)
     */
    public const FETCH_SERIALIZE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_PROPS_LATE)
     */
    public const FETCH_PROPS_LATE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_NAMED)
     */
    public const FETCH_NAMED = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_AUTOCOMMIT)
     */
    public const ATTR_AUTOCOMMIT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_PREFETCH)
     */
    public const ATTR_PREFETCH = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_TIMEOUT)
     */
    public const ATTR_TIMEOUT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_ERRMODE)
     */
    public const ATTR_ERRMODE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_SERVER_VERSION)
     */
    public const ATTR_SERVER_VERSION = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_CLIENT_VERSION)
     */
    public const ATTR_CLIENT_VERSION = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_SERVER_INFO)
     */
    public const ATTR_SERVER_INFO = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_CONNECTION_STATUS)
     */
    public const ATTR_CONNECTION_STATUS = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_CASE)
     */
    public const ATTR_CASE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_CURSOR_NAME)
     */
    public const ATTR_CURSOR_NAME = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_CURSOR)
     */
    public const ATTR_CURSOR = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_ORACLE_NULLS)
     */
    public const ATTR_ORACLE_NULLS = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_PERSISTENT)
     */
    public const ATTR_PERSISTENT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_STATEMENT_CLASS)
     */
    public const ATTR_STATEMENT_CLASS = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_FETCH_TABLE_NAMES)
     */
    public const ATTR_FETCH_TABLE_NAMES = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_FETCH_CATALOG_NAMES)
     */
    public const ATTR_FETCH_CATALOG_NAMES = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_DRIVER_NAME)
     */
    public const ATTR_DRIVER_NAME = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_STRINGIFY_FETCHES)
     */
    public const ATTR_STRINGIFY_FETCHES = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_MAX_COLUMN_LEN)
     */
    public const ATTR_MAX_COLUMN_LEN = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_EMULATE_PREPARES)
     */
    public const ATTR_EMULATE_PREPARES = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_DEFAULT_FETCH_MODE)
     */
    public const ATTR_DEFAULT_FETCH_MODE = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ATTR_DEFAULT_STR_PARAM)
     */
    public const ATTR_DEFAULT_STR_PARAM = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ERRMODE_SILENT)
     */
    public const ERRMODE_SILENT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ERRMODE_WARNING)
     */
    public const ERRMODE_WARNING = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_ERRMODE_EXCEPTION)
     */
    public const ERRMODE_EXCEPTION = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_CASE_NATURAL)
     */
    public const CASE_NATURAL = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_CASE_LOWER)
     */
    public const CASE_LOWER = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_CASE_UPPER)
     */
    public const CASE_UPPER = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_NULL_NATURAL)
     */
    public const NULL_NATURAL = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_NULL_EMPTY_STRING)
     */
    public const NULL_EMPTY_STRING = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_NULL_TO_STRING)
     */
    public const NULL_TO_STRING = UNKNOWN;

    /**
     * @var string
     * @cvalue PDO_ERR_NONE
     */
    public const ERR_NONE = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ORI_NEXT)
     */
    public const FETCH_ORI_NEXT = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ORI_PRIOR)
     */
    public const FETCH_ORI_PRIOR = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ORI_FIRST)
     */
    public const FETCH_ORI_FIRST = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ORI_LAST)
     */
    public const FETCH_ORI_LAST = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ORI_ABS)
     */
    public const FETCH_ORI_ABS = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_FETCH_ORI_REL)
     */
    public const FETCH_ORI_REL = UNKNOWN;

    /**
     * @var int
     * @cvalue LONG_CONST(PDO_CURSOR_FWDONLY)
     */
    public const CURSOR_FWDONLY = UNKNOWN;
    /**
     * @var int
     * @cvalue LONG_CONST(PDO_CURSOR_SCROLL)
     */
    public const CURSOR_SCROLL = UNKNOWN;

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
