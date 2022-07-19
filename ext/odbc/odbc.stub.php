<?php

/** @generate-class-entries */

/**
 * @var string
 * @cname PHP_ODBC_TYPE
 */
const ODBC_TYPE = UNKNOWN;
/**
 * @var int
 * @cname PHP_ODBC_BINMODE_PASSTHRU
 */
const ODBC_BINMODE_PASSTHRU = UNKNOWN;
/**
 * @var int
 * @cname PHP_ODBC_BINMODE_RETURN
 */
const ODBC_BINMODE_RETURN = UNKNOWN;
/**
 * @var int
 * @cname PHP_ODBC_BINMODE_CONVERT
 */
const ODBC_BINMODE_CONVERT = UNKNOWN;

/* Define Constants for options. These Constants are defined in <sqlext.h> */

/**
 * @var int
 * @cname SQL_ODBC_CURSORS
 */
const SQL_ODBC_CURSORS = UNKNOWN;
/**
 * @var int
 * @cname SQL_CUR_USE_DRIVER
 */
const SQL_CUR_USE_DRIVER = UNKNOWN;
/**
 * @var int
 * @cname SQL_CUR_USE_IF_NEEDED
 */
const SQL_CUR_USE_IF_NEEDED = UNKNOWN;
/**
 * @var int
 * @cname SQL_CUR_USE_ODBC
 */
const SQL_CUR_USE_ODBC = UNKNOWN;

/**
 * @var int
 * @cname SQL_CONCURRENCY
 */
const SQL_CONCURRENCY = UNKNOWN;
/**
 * @var int
 * @cname SQL_CONCUR_READ_ONLY
 */
const SQL_CONCUR_READ_ONLY = UNKNOWN;
/**
 * @var int
 * @cname SQL_CONCUR_LOCK
 */
const SQL_CONCUR_LOCK = UNKNOWN;
/**
 * @var int
 * @cname SQL_CONCUR_ROWVER
 */
const SQL_CONCUR_ROWVER = UNKNOWN;
/**
 * @var int
 * @cname SQL_CONCUR_VALUES
 */
const SQL_CONCUR_VALUES = UNKNOWN;

/**
 * @var int
 * @cname SQL_CURSOR_TYPE
 */
const SQL_CURSOR_TYPE = UNKNOWN;
/**
 * @var int
 * @cname SQL_CURSOR_FORWARD_ONLY
 */
const SQL_CURSOR_FORWARD_ONLY = UNKNOWN;
/**
 * @var int
 * @cname SQL_CURSOR_KEYSET_DRIVEN
 */
const SQL_CURSOR_KEYSET_DRIVEN = UNKNOWN;
/**
 * @var int
 * @cname SQL_CURSOR_DYNAMIC
 */
const SQL_CURSOR_DYNAMIC = UNKNOWN;
/**
 * @var int
 * @cname SQL_CURSOR_STATIC
 */
const SQL_CURSOR_STATIC = UNKNOWN;

/**
 * @var int
 * @cname SQL_KEYSET_SIZE
 */
const SQL_KEYSET_SIZE = UNKNOWN;

/* these are for the Data Source type */

/**
 * @var int
 * @cname SQL_FETCH_FIRST
 */
const SQL_FETCH_FIRST = UNKNOWN;
/**
 * @var int
 * @cname SQL_FETCH_NEXT
 */
const SQL_FETCH_NEXT = UNKNOWN;

/* register the standard data types */

/**
 * @var int
 * @cname SQL_CHAR
 */
const SQL_CHAR = UNKNOWN;
/**
 * @var int
 * @cname SQL_VARCHAR
 */
const SQL_VARCHAR = UNKNOWN;
/**
 * @var int
 * @cname SQL_LONGVARCHAR
 */
const SQL_LONGVARCHAR = UNKNOWN;
/**
 * @var int
 * @cname SQL_DECIMAL
 */
const SQL_DECIMAL = UNKNOWN;
/**
 * @var int
 * @cname SQL_NUMERIC
 */
const SQL_NUMERIC = UNKNOWN;
/**
 * @var int
 * @cname SQL_BIT
 */
const SQL_BIT = UNKNOWN;
/**
 * @var int
 * @cname SQL_TINYINT
 */
const SQL_TINYINT = UNKNOWN;
/**
 * @var int
 * @cname SQL_SMALLINT
 */
const SQL_SMALLINT = UNKNOWN;
/**
 * @var int
 * @cname SQL_INTEGER
 */
const SQL_INTEGER = UNKNOWN;
/**
 * @var int
 * @cname SQL_BIGINT
 */
const SQL_BIGINT = UNKNOWN;
/**
 * @var int
 * @cname SQL_REAL
 */
const SQL_REAL = UNKNOWN;
/**
 * @var int
 * @cname SQL_FLOAT
 */
const SQL_FLOAT = UNKNOWN;
/**
 * @var int
 * @cname SQL_DOUBLE
 */
const SQL_DOUBLE = UNKNOWN;
/**
 * @var int
 * @cname SQL_BINARY
 */
const SQL_BINARY = UNKNOWN;
/**
 * @var int
 * @cname SQL_VARBINARY
 */
const SQL_VARBINARY = UNKNOWN;
/**
 * @var int
 * @cname SQL_LONGVARBINARY
 */
const SQL_LONGVARBINARY = UNKNOWN;
/**
 * @var int
 * @cname SQL_DATE
 */
const SQL_DATE = UNKNOWN;
/**
 * @var int
 * @cname SQL_TIME
 */
const SQL_TIME = UNKNOWN;
/**
 * @var int
 * @cname SQL_TIMESTAMP
 */
const SQL_TIMESTAMP = UNKNOWN;

#if defined(ODBCVER) && (ODBCVER >= 0x0300)
/**
 * @var int
 * @cname SQL_TYPE_DATE
 */
const SQL_TYPE_DATE = UNKNOWN;
/**
 * @var int
 * @cname SQL_TYPE_TIME
 */
const SQL_TYPE_TIME = UNKNOWN;
/**
 * @var int
 * @cname SQL_TYPE_TIMESTAMP
 */
const SQL_TYPE_TIMESTAMP = UNKNOWN;
/**
 * @var int
 * @cname SQL_WCHAR
 */
const SQL_WCHAR = UNKNOWN;
/**
 * @var int
 * @cname SQL_WVARCHAR
 */
const SQL_WVARCHAR = UNKNOWN;
/**
 * @var int
 * @cname SQL_WLONGVARCHAR
 */
const SQL_WLONGVARCHAR = UNKNOWN;

/* SQLSpecialColumns values */

/**
 * @var int
 * @cname SQL_BEST_ROWID
 */
const SQL_BEST_ROWID = UNKNOWN;
/**
 * @var int
 * @cname SQL_ROWVER
 */
const SQL_ROWVER = UNKNOWN;
/**
 * @var int
 * @cname SQL_SCOPE_CURROW
 */
const SQL_SCOPE_CURROW = UNKNOWN;
/**
 * @var int
 * @cname SQL_SCOPE_TRANSACTION
 */
const SQL_SCOPE_TRANSACTION = UNKNOWN;
/**
 * @var int
 * @cname SQL_SCOPE_SESSION
 */
const SQL_SCOPE_SESSION = UNKNOWN;
/**
 * @var int
 * @cname SQL_NO_NULLS
 */
const SQL_NO_NULLS = UNKNOWN;
/**
 * @var int
 * @cname SQL_NULLABLE
 */
const SQL_NULLABLE = UNKNOWN;

/* SQLStatistics values */

/**
 * @var int
 * @cname SQL_INDEX_UNIQUE
 */
const SQL_INDEX_UNIQUE = UNKNOWN;
/**
 * @var int
 * @cname SQL_INDEX_ALL
 */
const SQL_INDEX_ALL = UNKNOWN;
/**
 * @var int
 * @cname SQL_ENSURE
 */
const SQL_ENSURE = UNKNOWN;
/**
 * @var int
 * @cname SQL_QUICK
 */
const SQL_QUICK = UNKNOWN;

#endif



function odbc_close_all(): void {}

/** @param resource $statement */
function odbc_binmode($statement, int $mode): bool {}

/** @param resource $statement */
function odbc_longreadlen($statement, int $length): bool {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_prepare($odbc, string $query) {}

/** @param resource $statement */
function odbc_execute($statement, array $params = []): bool {}

/** @param resource $statement */
function odbc_cursor($statement): string|false {}

#ifdef HAVE_SQLDATASOURCES
/** @param resource $odbc */
function odbc_data_source($odbc, int $fetch_type): array|false {}
#endif

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_exec($odbc, string $query) {}

/**
 * @param resource $odbc
 * @return resource|false
 * @alias odbc_exec
 */
function odbc_do($odbc, string $query) {}

#ifdef PHP_ODBC_HAVE_FETCH_HASH
/** @param resource $statement */
function odbc_fetch_object($statement, int $row = -1): stdClass|false {}

/** @param resource $statement */
function odbc_fetch_array($statement, int $row = -1): array|false {}
#endif

/**
 * @param resource $statement
 * @param array $array
 */
function odbc_fetch_into($statement, &$array, int $row = 0): int|false {}

/** @param resource $statement */
function odbc_fetch_row($statement, ?int $row = null): bool {}

/** @param resource $statement */
function odbc_result($statement, string|int $field): string|bool|null {}

/**
 * @param resource $statement
 * @deprecated
 */
function odbc_result_all($statement, string $format = ""): int|false {}

/** @param resource $statement */
function odbc_free_result($statement): bool {}

/**
 * @return resource|false
 */
function odbc_connect(string $dsn, string $user, #[\SensitiveParameter] string $password, int $cursor_option = SQL_CUR_USE_DRIVER) {}

/**
 * @return resource|false
 */
function odbc_pconnect(string $dsn, string $user, #[\SensitiveParameter] string $password, int $cursor_option = SQL_CUR_USE_DRIVER) {}

/** @param resource $odbc */
function odbc_close($odbc): void {}

/** @param resource $statement */
function odbc_num_rows($statement): int {}

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
/** @param resource $statement */
function odbc_next_result($statement): bool {}
#endif

/** @param resource $statement */
function odbc_num_fields($statement): int {}

/** @param resource $statement */
function odbc_field_name($statement, int $field): string|false {}

/** @param resource $statement */
function odbc_field_type($statement, int $field): string|false {}

/** @param resource $statement */
function odbc_field_len($statement, int $field): int|false {}

/**
 * @param resource $statement
 * @alias odbc_field_len
 */
function odbc_field_precision($statement, int $field): int|false {}

/** @param resource $statement */
function odbc_field_scale($statement, int $field): int|false {}

/** @param resource $statement */
function odbc_field_num($statement, string $field): int|false {}

/** @param resource $odbc */
function odbc_autocommit($odbc, bool $enable = false): int|bool {}

/** @param resource $odbc */
function odbc_commit($odbc): bool {}

/** @param resource $odbc */
function odbc_rollback($odbc): bool {}

/** @param resource|null $odbc */
function odbc_error($odbc = null): string {}

/** @param resource|null $odbc */
function odbc_errormsg($odbc = null): string {}

/** @param resource $odbc */
function odbc_setoption($odbc, int $which, int $option, int $value): bool {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_tables($odbc, ?string $catalog = null, ?string $schema = null, ?string $table = null, ?string $types = null) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_columns($odbc, ?string $catalog = null, ?string $schema = null, ?string $table = null, ?string $column = null) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_gettypeinfo($odbc, int $data_type = 0) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_primarykeys($odbc, ?string $catalog, string $schema, string $table) {}

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_procedurecolumns($odbc, ?string $catalog = null, ?string $schema = null, ?string $procedure = null, ?string $column = null) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_procedures($odbc, ?string $catalog = null, ?string $schema = null, ?string $procedure = null) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_foreignkeys($odbc, ?string $pk_catalog, string $pk_schema, string $pk_table, string $fk_catalog, string $fk_schema, string $fk_table) {}
#endif

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_specialcolumns($odbc, int $type, ?string $catalog, string $schema, string $table, int $scope, int $nullable) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_statistics($odbc, ?string $catalog, string $schema, string $table, int $unique, int $accuracy) {}

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_tableprivileges($odbc, ?string $catalog, string $schema, string $table) {}

/**
 * @param resource $odbc
 * @return resource|false
 */
function odbc_columnprivileges($odbc, ?string $catalog, string $schema, string $table, string $column) {}
#endif

/* odbc_utils.c */

function odbc_connection_string_is_quoted(string $str): bool {}

function odbc_connection_string_should_quote(string $str): bool {}

function odbc_connection_string_quote(string $str): string {}
