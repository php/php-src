<?php

/** @generate-class-entries */

namespace {
    /* libpq version */

    /**
     * @var string
     * @cvalue pgsql_libpq_version
     */
    const PGSQL_LIBPQ_VERSION = UNKNOWN;
    /**
     * @var string
     * @cvalue pgsql_libpq_version
     * @deprecated
     */
    const PGSQL_LIBPQ_VERSION_STR = UNKNOWN;

    /* For connection option */

    /**
     * @var int
     * @cvalue PGSQL_CONNECT_FORCE_NEW
     */
    const PGSQL_CONNECT_FORCE_NEW = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_CONNECT_ASYNC
     */
    const PGSQL_CONNECT_ASYNC = UNKNOWN;

    /* For pg_fetch_array() */

    /**
     * @var int
     * @cvalue PGSQL_ASSOC
     */
    const PGSQL_ASSOC = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_NUM
     */
    const PGSQL_NUM = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_BOTH
     */
    const PGSQL_BOTH = UNKNOWN;

    /* For pg_last_notice() */

    /**
     * @var int
     * @cvalue PGSQL_NOTICE_LAST
     */
    const PGSQL_NOTICE_LAST = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_NOTICE_ALL
     */
    const PGSQL_NOTICE_ALL = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_NOTICE_CLEAR
     */
    const PGSQL_NOTICE_CLEAR = UNKNOWN;

    /* For pg_connection_status() */

    /**
     * @var int
     * @cvalue CONNECTION_BAD
     */
    const PGSQL_CONNECTION_BAD = UNKNOWN;
    /**
     * @var int
     * @cvalue CONNECTION_OK
     */
    const PGSQL_CONNECTION_OK = UNKNOWN;
    /**
     * @var int
     * @cvalue CONNECTION_STARTED
     */
    const PGSQL_CONNECTION_STARTED = UNKNOWN;
    /**
     * @var int
     * @cvalue CONNECTION_MADE
     */
    const PGSQL_CONNECTION_MADE = UNKNOWN;
    /**
     * @var int
     * @cvalue CONNECTION_AWAITING_RESPONSE
     */
    const PGSQL_CONNECTION_AWAITING_RESPONSE = UNKNOWN;
    /**
     * @var int
     * @cvalue CONNECTION_AUTH_OK
     */
    const PGSQL_CONNECTION_AUTH_OK = UNKNOWN;
#ifdef CONNECTION_SSL_STARTUP
    /**
     * @var int
     * @cvalue CONNECTION_SSL_STARTUP
     */
    const PGSQL_CONNECTION_SSL_STARTUP = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue CONNECTION_SETENV
     */
    const PGSQL_CONNECTION_SETENV = UNKNOWN;

    /* For pg_connect_poll() */
    /**
     * @var int
     * @cvalue PGRES_POLLING_FAILED
     */
    const PGSQL_POLLING_FAILED = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_POLLING_READING
     */
    const PGSQL_POLLING_READING = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_POLLING_WRITING
     */
    const PGSQL_POLLING_WRITING = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_POLLING_OK
     */
    const PGSQL_POLLING_OK = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_POLLING_ACTIVE
     */
    const PGSQL_POLLING_ACTIVE = UNKNOWN;

    /* For pg_transaction_status() */

    /**
     * @var int
     * @cvalue PQTRANS_IDLE
     */
    const PGSQL_TRANSACTION_IDLE = UNKNOWN;
    /**
     * @var int
     * @cvalue PQTRANS_ACTIVE
     */
    const PGSQL_TRANSACTION_ACTIVE = UNKNOWN;
    /**
     * @var int
     * @cvalue PQTRANS_INTRANS
     */
    const PGSQL_TRANSACTION_INTRANS = UNKNOWN;
    /**
     * @var int
     * @cvalue PQTRANS_INERROR
     */
    const PGSQL_TRANSACTION_INERROR = UNKNOWN;
    /**
     * @var int
     * @cvalue PQTRANS_UNKNOWN
     */
    const PGSQL_TRANSACTION_UNKNOWN = UNKNOWN;

    /* For pg_set_error_verbosity() */

    /**
     * @var int
     * @cvalue PQERRORS_TERSE
     */
    const PGSQL_ERRORS_TERSE = UNKNOWN;
    /**
     * @var int
     * @cvalue PQERRORS_DEFAULT
     */
    const PGSQL_ERRORS_DEFAULT = UNKNOWN;
    /**
     * @var int
     * @cvalue PQERRORS_VERBOSE
     */
    const PGSQL_ERRORS_VERBOSE = UNKNOWN;
    #if PGVERSION_NUM > 110000
    /**
     * @var int
     * @cvalue PQERRORS_SQLSTATE
     */
    const PGSQL_ERRORS_SQLSTATE = UNKNOWN;
    #else
    /**
     * @var int
     * @cvalue PQERRORS_TERSE
     */
    const PGSQL_ERRORS_SQLSTATE = UNKNOWN;
    #endif

    /* For lo_seek() */

    /**
     * @var int
     * @cvalue SEEK_SET
     */
    const PGSQL_SEEK_SET = UNKNOWN;
    /**
     * @var int
     * @cvalue SEEK_CUR
     */
    const PGSQL_SEEK_CUR = UNKNOWN;
    /**
     * @var int
     * @cvalue SEEK_END
     */
    const PGSQL_SEEK_END = UNKNOWN;

    /* For pg_result_status() return value type */

    /**
     * @var int
     * @cvalue PGSQL_STATUS_LONG
     */
    const PGSQL_STATUS_LONG = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_STATUS_STRING
     */
    const PGSQL_STATUS_STRING = UNKNOWN;

    /* For pg_result_status() return value */

    /**
     * @var int
     * @cvalue PGRES_EMPTY_QUERY
     */
    const PGSQL_EMPTY_QUERY = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_COMMAND_OK
     */
    const PGSQL_COMMAND_OK = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_TUPLES_OK
     */
    const PGSQL_TUPLES_OK = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_COPY_OUT
     */
    const PGSQL_COPY_OUT = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_COPY_IN
     */
    const PGSQL_COPY_IN = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_BAD_RESPONSE
     */
    const PGSQL_BAD_RESPONSE = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_NONFATAL_ERROR
     */
    const PGSQL_NONFATAL_ERROR = UNKNOWN;
    /**
     * @var int
     * @cvalue PGRES_FATAL_ERROR
     */
    const PGSQL_FATAL_ERROR = UNKNOWN;

    /* For pg_result_error_field() field codes */

    /**
     * @var int
     * @cvalue PG_DIAG_SEVERITY
     */
    const PGSQL_DIAG_SEVERITY = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_SQLSTATE
     */
    const PGSQL_DIAG_SQLSTATE = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_MESSAGE_PRIMARY
     */
    const PGSQL_DIAG_MESSAGE_PRIMARY = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_MESSAGE_DETAIL
     */
    const PGSQL_DIAG_MESSAGE_DETAIL = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_MESSAGE_HINT
     */
    const PGSQL_DIAG_MESSAGE_HINT = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_STATEMENT_POSITION
     */
    const PGSQL_DIAG_STATEMENT_POSITION = UNKNOWN;
#ifdef PG_DIAG_INTERNAL_POSITION
    /**
     * @var int
     * @cvalue PG_DIAG_INTERNAL_POSITION
     */
    const PGSQL_DIAG_INTERNAL_POSITION = UNKNOWN;
#endif
#ifdef PG_DIAG_INTERNAL_QUERY
    /**
     * @var int
     * @cvalue PG_DIAG_INTERNAL_QUERY
     */
    const PGSQL_DIAG_INTERNAL_QUERY = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue PG_DIAG_CONTEXT
     */
    const PGSQL_DIAG_CONTEXT = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_SOURCE_FILE
     */
    const PGSQL_DIAG_SOURCE_FILE = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_SOURCE_LINE
     */
    const PGSQL_DIAG_SOURCE_LINE = UNKNOWN;
    /**
     * @var int
     * @cvalue PG_DIAG_SOURCE_FUNCTION
     */
    const PGSQL_DIAG_SOURCE_FUNCTION = UNKNOWN;
#ifdef PG_DIAG_SCHEMA_NAME
    /**
     * @var int
     * @cvalue PG_DIAG_SCHEMA_NAME
     */
    const PGSQL_DIAG_SCHEMA_NAME = UNKNOWN;
#endif
#ifdef PG_DIAG_TABLE_NAME
    /**
     * @var int
     * @cvalue PG_DIAG_TABLE_NAME
     */
    const PGSQL_DIAG_TABLE_NAME = UNKNOWN;
#endif
#ifdef PG_DIAG_COLUMN_NAME
    /**
     * @var int
     * @cvalue PG_DIAG_COLUMN_NAME
     */
    const PGSQL_DIAG_COLUMN_NAME = UNKNOWN;
#endif
#ifdef PG_DIAG_DATATYPE_NAME
    /**
     * @var int
     * @cvalue PG_DIAG_DATATYPE_NAME
     */
    const PGSQL_DIAG_DATATYPE_NAME = UNKNOWN;
#endif
#ifdef PG_DIAG_CONSTRAINT_NAME
    /**
     * @var int
     * @cvalue PG_DIAG_CONSTRAINT_NAME
     */
    const PGSQL_DIAG_CONSTRAINT_NAME = UNKNOWN;
#endif
#ifdef PG_DIAG_SEVERITY_NONLOCALIZED
    /**
     * @var int
     * @cvalue PG_DIAG_SEVERITY_NONLOCALIZED
     */
    const PGSQL_DIAG_SEVERITY_NONLOCALIZED = UNKNOWN;
#endif

    /* pg_convert options */

    /**
     * @var int
     * @cvalue PGSQL_CONV_IGNORE_DEFAULT
     */
    const PGSQL_CONV_IGNORE_DEFAULT = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_CONV_FORCE_NULL
     */
    const PGSQL_CONV_FORCE_NULL = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_CONV_IGNORE_NOT_NULL
     */
    const PGSQL_CONV_IGNORE_NOT_NULL = UNKNOWN;

    /* pg_insert/update/delete/select options */

    /**
     * @var int
     * @cvalue PGSQL_DML_ESCAPE
     */
    const PGSQL_DML_ESCAPE = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_DML_NO_CONV
     */
    const PGSQL_DML_NO_CONV = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_DML_EXEC
     */
    const PGSQL_DML_EXEC = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_DML_ASYNC
     */
    const PGSQL_DML_ASYNC = UNKNOWN;
    /**
     * @var int
     * @cvalue PGSQL_DML_STRING
     */
    const PGSQL_DML_STRING = UNKNOWN;
#ifdef PQTRACE_SUPPPRESS_TIMESTAMPS
    /**
     * @var int
     * @cvalue PQTRACE_SUPPRESS_TIMESTAMPS
     */
    const PGSQL_TRACE_SUPPRESS_TIMESTAMPS = UNKNOWN;
#endif
#ifdef PQTRACE_REGRESS_MODE
    /**
     * @var int
     * @cvalue PQTRACE_REGRESS_MODE
     */
    const PGSQL_TRACE_REGRESS_MODE = UNKNOWN;
#endif

#ifdef LIBPQ_HAS_PIPELINING
    /**
     * @var int
     * @cvalue PGRES_PIPELINE_SYNC
     */
    const PGSQL_PIPELINE_SYNC = UNKNOWN;
    /**
     * @var int
     * @cvalue PQ_PIPELINE_ON
     */
    const PGSQL_PIPELINE_ON = UNKNOWN;
    /**
     * @var int
     * @cvalue PQ_PIPELINE_OFF
     */
    const PGSQL_PIPELINE_OFF = UNKNOWN;
    /**
     * @var int
     * @cvalue PQ_PIPELINE_ABORTED
     */
    const PGSQL_PIPELINE_ABORTED = UNKNOWN;
#endif
    
#ifdef HAVE_PG_CONTEXT_VISIBILITY
    /* For pg_set_error_context_visibility() */

    /**
     * @var int
     * @cvalue PQSHOW_CONTEXT_NEVER
     */
    const PGSQL_SHOW_CONTEXT_NEVER = UNKNOWN;
    /**
     * @var int
     * @cvalue PQSHOW_CONTEXT_ERRORS
     */
    const PGSQL_SHOW_CONTEXT_ERRORS = UNKNOWN;
    /**
     * @var int
     * @cvalue PQSHOW_CONTEXT_ALWAYS
     */
    const PGSQL_SHOW_CONTEXT_ALWAYS = UNKNOWN;
#endif

    function pg_connect(string $connection_string, int $flags = 0): PgSql\Connection|false {}

    function pg_pconnect(string $connection_string, int $flags = 0): PgSql\Connection|false {}

    function pg_connect_poll(PgSql\Connection $connection): int {}

    function pg_close(?PgSql\Connection $connection = null): true {}

    /** @refcount 1 */
    function pg_dbname(?PgSql\Connection $connection = null): string {}

    function pg_last_error(?PgSql\Connection $connection = null): string {}

    /**
     * @alias pg_last_error
     * @deprecated
     */
    function pg_errormessage(?PgSql\Connection $connection = null): string {}

    /** @refcount 1 */
    function pg_options(?PgSql\Connection $connection = null): string {}

    /** @refcount 1 */
    function pg_port(?PgSql\Connection $connection = null): string {}

    /** @refcount 1 */
    function pg_tty(?PgSql\Connection $connection = null): string {}

    /** @refcount 1 */
    function pg_host(?PgSql\Connection $connection = null): string {}

    /**
     * @return array<string, int|string|null>
     * @refcount 1
     */
    function pg_version(?PgSql\Connection $connection = null): array {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_parameter_status($connection, string $name = UNKNOWN): string|false {}

    function pg_ping(?PgSql\Connection $connection = null): bool {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_query($connection, string $query = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @alias pg_query
     */
    function pg_exec($connection, string $query = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @param string|array $query
     * @refcount 1
     */
    function pg_query_params($connection, $query, array $params = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_prepare($connection, string $statement_name, string $query = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @param string|array $statement_name
     * @refcount 1
     */
    function pg_execute($connection, $statement_name, array $params = UNKNOWN): PgSql\Result|false {}

    function pg_num_rows(PgSql\Result $result): int {}

    /**
     * @alias pg_num_rows
     * @deprecated
     */
    function pg_numrows(PgSql\Result $result): int {}

    function pg_num_fields(PgSql\Result $result): int {}

    /**
     * @alias pg_num_fields
     * @deprecated
     */
    function pg_numfields(PgSql\Result $result): int {}

    function pg_affected_rows(PgSql\Result $result): int {}

    /**
     * @alias pg_affected_rows
     * @deprecated
     */
    function pg_cmdtuples(PgSql\Result $result): int {}

    function pg_last_notice(PgSql\Connection $connection, int $mode = PGSQL_NOTICE_LAST): array|string|bool {}

    function pg_field_table(PgSql\Result $result, int $field, bool $oid_only = false): string|int|false {}

    /** @refcount 1 */
    function pg_field_name(PgSql\Result $result, int $field): string {}

    /**
     * @alias pg_field_name
     * @deprecated
     */
    function pg_fieldname(PgSql\Result $result, int $field): string {}

    function pg_field_size(PgSql\Result $result, int $field): int {}

    /**
     * @alias pg_field_size
     * @deprecated
     */
    function pg_fieldsize(PgSql\Result $result, int $field): int {}

    function pg_field_type(PgSql\Result $result, int $field): string {}

    /**
     * @alias pg_field_type
     * @deprecated
     */
    function pg_fieldtype(PgSql\Result $result, int $field): string {}

    /** @refcount 1 */
    function pg_field_type_oid(PgSql\Result $result, int $field): string|int {}

    function pg_field_num(PgSql\Result $result, string $field): int {}

    /**
     * @alias pg_field_num
     * @deprecated
     */
    function pg_fieldnum(PgSql\Result $result, string $field): int {}

    /**
     * @param string|int|null $row
     * @refcount 1
     */
    function pg_fetch_result(PgSql\Result $result, $row, string|int $field = UNKNOWN): string|false|null {}

    /**
     * @param string|int $row
     * @alias pg_fetch_result
     * @deprecated
     */
    function pg_result(PgSql\Result $result, $row, string|int $field = UNKNOWN): string|false|null {}

    /**
     * @return array<int|string, string|null>|false
     * @refcount 1
     */
    function pg_fetch_row(PgSql\Result $result, ?int $row = null, int $mode = PGSQL_NUM): array|false {}

    /**
     * @return array<int|string, string|null>|false
     * @refcount 1
     */
    function pg_fetch_assoc(PgSql\Result $result, ?int $row = null): array|false {}

    /**
     * @return array<int|string, string|null>|false
     * @refcount 1
     */
    function pg_fetch_array(PgSql\Result $result, ?int $row = null, int $mode = PGSQL_BOTH): array|false {}

    /** @refcount 1 */
    function pg_fetch_object(PgSql\Result $result, ?int $row = null, string $class = "stdClass", array $constructor_args = []): object|false {}

    /**
     * @return array<int, array>
     * @refcount 1
     */
    function pg_fetch_all(PgSql\Result $result, int $mode = PGSQL_ASSOC): array {}

    /**
     * @return array<int, string|null>
     * @refcount 1
     */
    function pg_fetch_all_columns(PgSql\Result $result, int $field = 0): array {}

    function pg_result_seek(PgSql\Result $result, int $row): bool {}

    /** @param string|int|null $row */
    function pg_field_prtlen(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    /**
     * @param string|int $row
     * @deprecated
     */
    function pg_fieldprtlen(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    /** @param string|int|null $row */
    function pg_field_is_null(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    /**
     * @param string|int $row
     * @deprecated
     */
    function pg_fieldisnull(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    function pg_free_result(PgSql\Result $result): bool {}

    /**
     * @alias pg_free_result
     * @deprecated
     */
    function pg_freeresult(PgSql\Result $result): bool {}

    /** @refcount 1 */
    function pg_last_oid(PgSql\Result $result): string|int|false {}

    /**
     * @alias pg_last_oid
     * @deprecated
     */
    function pg_getlastoid(PgSql\Result $result): string|int|false {}

    function pg_trace(string $filename, string $mode = "w", ?PgSql\Connection $connection = null, int $trace_mode = 0): bool {}

    function pg_untrace(?PgSql\Connection $connection = null): true {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
     * @refcount 1
     */
    function pg_lo_create($connection = UNKNOWN, $oid = UNKNOWN): string|int|false {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
     * @alias pg_lo_create
     * @deprecated
     */
    function pg_locreate($connection = UNKNOWN, $oid = UNKNOWN): string|int|false {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
     */
    function pg_lo_unlink($connection, $oid = UNKNOWN): bool {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
     * @alias pg_lo_unlink
     * @deprecated
     */
    function pg_lounlink($connection, $oid = UNKNOWN): bool {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
     * @refcount 1
     */
    function pg_lo_open($connection, $oid = UNKNOWN, string $mode = UNKNOWN): PgSql\Lob|false {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
     * @alias pg_lo_open
     * @deprecated
     */
    function pg_loopen($connection, $oid = UNKNOWN, string $mode = UNKNOWN): PgSql\Lob|false {}

    function pg_lo_close(PgSql\Lob $lob): bool {}

    /**
     * @alias pg_lo_close
     * @deprecated
     */
    function pg_loclose(PgSql\Lob $lob): bool {}

    /** @refcount 1 */
    function pg_lo_read(PgSql\Lob $lob, int $length = 8192): string|false {}

    /**
     * @alias pg_lo_read
     * @deprecated
     */
    function pg_loread(PgSql\Lob $lob, int $length = 8192): string|false {}

    function pg_lo_write(PgSql\Lob $lob, string $data, ?int $length = null): int|false {}

    /**
     * @alias pg_lo_write
     * @deprecated
     */
    function pg_lowrite(PgSql\Lob $lob, string $data, ?int $length = null): int|false {}

    function pg_lo_read_all(PgSql\Lob $lob): int {}

    /**
     * @alias pg_lo_read_all
     * @deprecated
     */
    function pg_loreadall(PgSql\Lob $lob): int {}

    /**
     * @param PgSql\Connection|string $connection
     * @param string|int $filename
     * @param string|int $oid
     * @refcount 1
     */
    function pg_lo_import($connection, $filename = UNKNOWN, $oid = UNKNOWN): string|int|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @param string|int $filename
     * @param string|int $oid
     * @alias pg_lo_import
     * @deprecated
     */
    function pg_loimport($connection, $filename = UNKNOWN, $oid = UNKNOWN): string|int|false {}

    /**
     * @param PgSql\Connection|string|int $connection
     * @param string|int $oid
     * @param string|int $filename
     */
    function pg_lo_export($connection, $oid = UNKNOWN, $filename = UNKNOWN): bool {}

    /**
     * @param PgSql\Connection|string|int $connection
     * @param string|int $oid
     * @param string|int $filename
     * @alias pg_lo_export
     * @deprecated
     */
    function pg_loexport($connection, $oid = UNKNOWN, $filename = UNKNOWN): bool {}

    function pg_lo_seek(PgSql\Lob $lob, int $offset, int $whence = SEEK_CUR): bool {}

    function pg_lo_tell(PgSql\Lob $lob): int {}

    function pg_lo_truncate(PgSql\Lob $lob, int $size): bool {}

    /** @param PgSql\Connection|int $connection */
    function pg_set_error_verbosity($connection, int $verbosity = UNKNOWN): int|false {}

    /** @param PgSql\Connection|string $connection */
    function pg_set_client_encoding($connection, string $encoding = UNKNOWN): int {}

    /**
     * @param PgSql\Connection|string $connection
     * @alias pg_set_client_encoding
     * @deprecated
     */
    function pg_setclientencoding($connection, string $encoding = UNKNOWN): int {}

    function pg_client_encoding(?PgSql\Connection $connection = null): string {}

    /**
     * @alias pg_client_encoding
     * @deprecated
     */
    function pg_clientencoding(?PgSql\Connection $connection = null): string {}

    function pg_end_copy(?PgSql\Connection $connection = null): bool {}

    /** @param PgSql\Connection|string $connection */
    function pg_put_line($connection, string $query = UNKNOWN): bool {}

    /**
     * @return array<int, string>|false
     * @refcount 1
     */
    function pg_copy_to(PgSql\Connection $connection, string $table_name, string $separator = "\t", string $null_as = "\\\\N"): array|false {}

    function pg_copy_from(PgSql\Connection $connection, string $table_name, array $rows, string $separator = "\t", string $null_as = "\\\\N"): bool {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_escape_string($connection, string $string = UNKNOWN): string {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_escape_bytea($connection, string $string = UNKNOWN): string {}

    /** @refcount 1 */
    function pg_unescape_bytea(string $string): string {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_escape_literal($connection, string $string = UNKNOWN): string|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @refcount 1
     */
    function pg_escape_identifier($connection, string $string = UNKNOWN): string|false {}

    /** @refcount 1 */
    function pg_result_error(PgSql\Result $result): string|false {}

    /** @refcount 1 */
    function pg_result_error_field(PgSql\Result $result, int $field_code): string|false|null {}

    function pg_connection_status(PgSql\Connection $connection): int {}

    function pg_transaction_status(PgSql\Connection $connection): int {}

    function pg_connection_reset(PgSql\Connection $connection): bool {}

    function pg_cancel_query(PgSql\Connection $connection): bool {}

    function pg_connection_busy(PgSql\Connection $connection): bool {}

    function pg_send_query(PgSql\Connection $connection, string $query): int|bool {}

    function pg_send_query_params(PgSql\Connection $connection, string $query, array $params): int|bool {}

    function pg_send_prepare(PgSql\Connection $connection, string $statement_name, string $query): int|bool {}

    function pg_send_execute(PgSql\Connection $connection, string $statement_name, array $params): int|bool {}

    /** @refcount 1 */
    function pg_get_result(PgSql\Connection $connection): PgSql\Result|false {}

    /** @refcount 1 */
    function pg_result_status(PgSql\Result $result, int $mode = PGSQL_STATUS_LONG): string|int {}

    /**
     * @return array<int|string, int|string>
     * @refcount 1
     */
    function pg_get_notify(PgSql\Connection $connection, int $mode = PGSQL_ASSOC): array|false {}

    function pg_get_pid(PgSql\Connection $connection): int {}

    /**
     * @return resource|false
     * @refcount 1
     */
    function pg_socket(PgSql\Connection $connection) {}

    function pg_consume_input(PgSql\Connection $connection): bool {}

    function pg_flush(PgSql\Connection $connection): int|bool {}

    /**
     * @return array<string, array>|false
     * @refcount 1
     */
    function pg_meta_data(PgSql\Connection $connection, string $table_name, bool $extended = false): array|false {}

    /**
     * @return array<string, mixed>|false
     * @refcount 1
     */
    function pg_convert(PgSql\Connection $connection, string $table_name, array $values, int $flags = 0): array|false {}

    /** @refcount 1 */
    function pg_insert(PgSql\Connection $connection, string $table_name, array $values, int $flags = PGSQL_DML_EXEC): PgSql\Result|string|bool {}

    /** @refcount 1 */
    function pg_update(PgSql\Connection $connection, string $table_name, array $values, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

    /** @refcount 1 */
    function pg_delete(PgSql\Connection $connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

    /**
     * @return array<int, array>|string|false
     * @refcount 1
     */
    function pg_select(PgSql\Connection $connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC, int $mode = PGSQL_ASSOC): array|string|false {}

#ifdef LIBPQ_HAS_PIPELINING
    function pg_enter_pipeline_mode(PgSql\Connection $connection): bool {}
    function pg_exit_pipeline_mode(PgSql\Connection $connection): bool {}
    function pg_pipeline_sync(PgSql\Connection $connection): bool {}
    function pg_pipeline_status(PgSql\Connection $connection): int {}
#endif

#ifdef HAVE_PG_CONTEXT_VISIBILITY
    function pg_set_error_context_visibility(PgSql\Connection $connection, int $visibility): int {}
#endif
}

namespace PgSql {
    /**
     * @strict-properties
     * @not-serializable
     */
    final class Connection
    {
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Result
    {
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Lob
    {
    }

}
