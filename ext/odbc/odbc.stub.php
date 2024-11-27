<?php

/** @generate-class-entries */

namespace Odbc {
    /**
     * @strict-properties
     * @not-serializable
     */
    class Connection
    {
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    class Result
    {
    }
}

namespace {
    /**
     * @var string
     * @cvalue PHP_ODBC_TYPE
     */
    const ODBC_TYPE = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_ODBC_BINMODE_PASSTHRU
     */
    const ODBC_BINMODE_PASSTHRU = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_ODBC_BINMODE_RETURN
     */
    const ODBC_BINMODE_RETURN = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_ODBC_BINMODE_CONVERT
     */
    const ODBC_BINMODE_CONVERT = UNKNOWN;

    /* Define Constants for options. These Constants are defined in <sqlext.h> */

    /**
     * @var int
     * @cvalue SQL_ODBC_CURSORS
     */
    const SQL_ODBC_CURSORS = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CUR_USE_DRIVER
     */
    const SQL_CUR_USE_DRIVER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CUR_USE_IF_NEEDED
     */
    const SQL_CUR_USE_IF_NEEDED = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CUR_USE_ODBC
     */
    const SQL_CUR_USE_ODBC = UNKNOWN;

    /**
     * @var int
     * @cvalue SQL_CONCURRENCY
     */
    const SQL_CONCURRENCY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CONCUR_READ_ONLY
     */
    const SQL_CONCUR_READ_ONLY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CONCUR_LOCK
     */
    const SQL_CONCUR_LOCK = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CONCUR_ROWVER
     */
    const SQL_CONCUR_ROWVER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CONCUR_VALUES
     */
    const SQL_CONCUR_VALUES = UNKNOWN;

    /**
     * @var int
     * @cvalue SQL_CURSOR_TYPE
     */
    const SQL_CURSOR_TYPE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CURSOR_FORWARD_ONLY
     */
    const SQL_CURSOR_FORWARD_ONLY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CURSOR_KEYSET_DRIVEN
     */
    const SQL_CURSOR_KEYSET_DRIVEN = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CURSOR_DYNAMIC
     */
    const SQL_CURSOR_DYNAMIC = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_CURSOR_STATIC
     */
    const SQL_CURSOR_STATIC = UNKNOWN;

    /**
     * @var int
     * @cvalue SQL_KEYSET_SIZE
     */
    const SQL_KEYSET_SIZE = UNKNOWN;

    /* these are for the Data Source type */

    /**
     * @var int
     * @cvalue SQL_FETCH_FIRST
     */
    const SQL_FETCH_FIRST = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_FETCH_NEXT
     */
    const SQL_FETCH_NEXT = UNKNOWN;

    /* register the standard data types */

    /**
     * @var int
     * @cvalue SQL_CHAR
     */
    const SQL_CHAR = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_VARCHAR
     */
    const SQL_VARCHAR = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_LONGVARCHAR
     */
    const SQL_LONGVARCHAR = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_DECIMAL
     */
    const SQL_DECIMAL = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_NUMERIC
     */
    const SQL_NUMERIC = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_BIT
     */
    const SQL_BIT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_TINYINT
     */
    const SQL_TINYINT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_SMALLINT
     */
    const SQL_SMALLINT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_INTEGER
     */
    const SQL_INTEGER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_BIGINT
     */
    const SQL_BIGINT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_REAL
     */
    const SQL_REAL = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_FLOAT
     */
    const SQL_FLOAT = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_DOUBLE
     */
    const SQL_DOUBLE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_BINARY
     */
    const SQL_BINARY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_VARBINARY
     */
    const SQL_VARBINARY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_LONGVARBINARY
     */
    const SQL_LONGVARBINARY = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_DATE
     */
    const SQL_DATE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_TIME
     */
    const SQL_TIME = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_TIMESTAMP
     */
    const SQL_TIMESTAMP = UNKNOWN;

#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
    /**
     * @var int
     * @cvalue SQL_TYPE_DATE
     */
    const SQL_TYPE_DATE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_TYPE_TIME
     */
    const SQL_TYPE_TIME = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_TYPE_TIMESTAMP
     */
    const SQL_TYPE_TIMESTAMP = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_WCHAR
     */
    const SQL_WCHAR = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_WVARCHAR
     */
    const SQL_WVARCHAR = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_WLONGVARCHAR
     */
    const SQL_WLONGVARCHAR = UNKNOWN;

    /* SQLSpecialColumns values */

    /**
     * @var int
     * @cvalue SQL_BEST_ROWID
     */
    const SQL_BEST_ROWID = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_ROWVER
     */
    const SQL_ROWVER = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_SCOPE_CURROW
     */
    const SQL_SCOPE_CURROW = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_SCOPE_TRANSACTION
     */
    const SQL_SCOPE_TRANSACTION = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_SCOPE_SESSION
     */
    const SQL_SCOPE_SESSION = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_NO_NULLS
     */
    const SQL_NO_NULLS = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_NULLABLE
     */
    const SQL_NULLABLE = UNKNOWN;

    /* SQLStatistics values */

    /**
     * @var int
     * @cvalue SQL_INDEX_UNIQUE
     */
    const SQL_INDEX_UNIQUE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_INDEX_ALL
     */
    const SQL_INDEX_ALL = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_ENSURE
     */
    const SQL_ENSURE = UNKNOWN;
    /**
     * @var int
     * @cvalue SQL_QUICK
     */
    const SQL_QUICK = UNKNOWN;

#endif

    function odbc_close_all(): void {}

    function odbc_binmode(Odbc\Result $statement, int $mode): true {}

    function odbc_longreadlen(Odbc\Result $statement, int $length): true {}

    function odbc_prepare(Odbc\Connection $odbc, string $query): Odbc\Result|false {}

    function odbc_execute(Odbc\Result $statement, array $params = []): bool {}

    function odbc_cursor(Odbc\Result $statement): string|false {}

#ifdef HAVE_SQLDATASOURCES
    function odbc_data_source(Odbc\Connection $odbc, int $fetch_type): array|null|false {}
#endif

    function odbc_exec(Odbc\Connection $odbc, string $query): Odbc\Result|false {}

    /** @alias odbc_exec */
    function odbc_do(Odbc\Connection $odbc, string $query): Odbc\Result|false {}

#ifdef PHP_ODBC_HAVE_FETCH_HASH
    function odbc_fetch_object(Odbc\Result $statement, ?int $row = null): stdClass|false {}

    function odbc_fetch_array(Odbc\Result $statement, ?int $row = null): array|false {}
#endif

    /**
     * @param array $array
     */
    function odbc_fetch_into(Odbc\Result $statement, &$array, ?int $row = null): int|false {}

    function odbc_fetch_row(Odbc\Result $statement, ?int $row = null): bool {}

    function odbc_result(Odbc\Result $statement, string|int $field): string|bool|null {}

    #[\Deprecated(since: '8.1')]
    function odbc_result_all(Odbc\Result $statement, string $format = ""): int|false {}

    function odbc_free_result(Odbc\Result $statement): true {}

    function odbc_connect(string $dsn, ?string $user = null, #[\SensitiveParameter] ?string $password = null, int $cursor_option = SQL_CUR_USE_DRIVER): Odbc\Connection|false {}

    function odbc_pconnect(string $dsn, ?string $user = null, #[\SensitiveParameter] ?string $password = null, int $cursor_option = SQL_CUR_USE_DRIVER): Odbc\Connection|false {}

    function odbc_close(Odbc\Connection $odbc): void {}

    function odbc_num_rows(Odbc\Result $statement): int {}

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
    function odbc_next_result(Odbc\Result $statement): bool {}
#endif

    function odbc_num_fields(Odbc\Result $statement): int {}

    function odbc_field_name(Odbc\Result $statement, int $field): string|false {}

    function odbc_field_type(Odbc\Result $statement, int $field): string|false {}

    function odbc_field_len(Odbc\Result $statement, int $field): int|false {}

    /** @alias odbc_field_len */
    function odbc_field_precision(Odbc\Result $statement, int $field): int|false {}

    function odbc_field_scale(Odbc\Result $statement, int $field): int|false {}

    function odbc_field_num(Odbc\Result $statement, string $field): int|false {}

    function odbc_autocommit(Odbc\Connection $odbc, ?bool $enable = null): int|bool {}

    function odbc_commit(Odbc\Connection $odbc): bool {}

    function odbc_rollback(Odbc\Connection $odbc): bool {}

    function odbc_error(?Odbc\Connection $odbc = null): string {}

    function odbc_errormsg(?Odbc\Connection $odbc = null): string {}

    function odbc_setoption(Odbc\Connection|Odbc\Result $odbc, int $which, int $option, int $value): bool {}

    function odbc_tables(Odbc\Connection $odbc, ?string $catalog = null, ?string $schema = null, ?string $table = null, ?string $types = null): Odbc\Result|false {}

    function odbc_columns(Odbc\Connection $odbc, ?string $catalog = null, ?string $schema = null, ?string $table = null, ?string $column = null): Odbc\Result|false {}

    function odbc_gettypeinfo(Odbc\Connection $odbc, int $data_type = 0): Odbc\Result|false {}

    function odbc_primarykeys(Odbc\Connection $odbc, ?string $catalog, string $schema, string $table): Odbc\Result|false {}

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
    function odbc_procedurecolumns(Odbc\Connection $odbc, ?string $catalog = null, ?string $schema = null, ?string $procedure = null, ?string $column = null): Odbc\Result|false {}

    function odbc_procedures(Odbc\Connection $odbc, ?string $catalog = null, ?string $schema = null, ?string $procedure = null): Odbc\Result|false {}

    function odbc_foreignkeys(Odbc\Connection $odbc, ?string $pk_catalog, string $pk_schema, string $pk_table, string $fk_catalog, string $fk_schema, string $fk_table): Odbc\Result|false {}
#endif

    function odbc_specialcolumns(Odbc\Connection $odbc, int $type, ?string $catalog, string $schema, string $table, int $scope, int $nullable): Odbc\Result|false {}

    function odbc_statistics(Odbc\Connection $odbc, ?string $catalog, string $schema, string $table, int $unique, int $accuracy): Odbc\Result|false {}

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
    function odbc_tableprivileges(Odbc\Connection $odbc, ?string $catalog, string $schema, string $table): Odbc\Result|false {}

    function odbc_columnprivileges(Odbc\Connection $odbc, ?string $catalog, string $schema, string $table, string $column): Odbc\Result|false {}
#endif

    /* odbc_utils.c */

    function odbc_connection_string_is_quoted(string $str): bool {}

    function odbc_connection_string_should_quote(string $str): bool {}

    function odbc_connection_string_quote(string $str): string {}
}
