<?php

/** @generate-class-entries */

/** @strict-properties */
final class PgSql
{
}

/** @strict-properties */
final class PgSqlResult
{
}

/** @strict-properties */
final class PgSqlLob
{
}

function pg_connect(string $connection_string, int $flags = 0): PgSql|false {}

function pg_pconnect(string $connection_string, int $flags = 0): PgSql|false {}

function pg_connect_poll(PgSql $connection): int {}

function pg_close(?PgSql $connection = null): bool {}

function pg_dbname(?PgSql $connection = null): string {}

function pg_last_error(?PgSql $connection = null): string {}

/**
 * @alias pg_last_error
 * @deprecated
 */
function pg_errormessage(?PgSql $connection = null): string {}

function pg_options(?PgSql $connection = null): string {}

function pg_port(?PgSql $connection = null): string {}

function pg_tty(?PgSql $connection = null): string {}

function pg_host(?PgSql $connection = null): string {}

function pg_version(?PgSql $connection = null): array {}

/** @param PgSql|string $connection */
function pg_parameter_status($connection, string $name = UNKNOWN): string|false {}

function pg_ping(?PgSql $connection = null): bool {}

/** @param PgSql|string $connection */
function pg_query($connection, string $query = UNKNOWN): PgSqlResult|false {}

/**
 * @param PgSql|string $connection
 * @alias pg_query
 */
function pg_exec($connection, string $query = UNKNOWN): PgSqlResult|false {}

/**
 * @param PgSql|string $connection
 * @param string|array $query
 */
function pg_query_params($connection, $query, array $params = UNKNOWN): PgSqlResult|false {}

/** @param PgSql|string $connection */
function pg_prepare($connection, string $statement_name, string $query = UNKNOWN): PgSqlResult|false {}

/**
 * @param PgSql|string $connection
 * @param string|array $statement_name
 */
function pg_execute($connection, $statement_name, array $params = UNKNOWN): PgSqlResult|false {}

function pg_num_rows(PgSqlResult $result): int {}

/**
 * @alias pg_num_rows
 * @deprecated
 */
function pg_numrows(PgSqlResult $result): int {}

function pg_num_fields(PgSqlResult $result): int {}

/**
 * @alias pg_num_fields
 * @deprecated
 */
function pg_numfields(PgSqlResult $result): int {}

function pg_affected_rows(PgSqlResult $result): int {}

/**
 * @alias pg_affected_rows
 * @deprecated
 */
function pg_cmdtuples(PgSqlResult $result): int {}

function pg_last_notice(PgSql $connection, int $mode = PGSQL_NOTICE_LAST): array|string|bool {}

function pg_field_table(PgSqlResult $result, int $field, bool $oid_only = false): string|int|false {}

function pg_field_name(PgSqlResult $result, int $field): string {}

/**
 * @alias pg_field_name
 * @deprecated
 */
function pg_fieldname(PgSqlResult $result, int $field): string {}

function pg_field_size(PgSqlResult $result, int $field): int {}

/**
 * @alias pg_field_size
 * @deprecated
 */
function pg_fieldsize(PgSqlResult $result, int $field): int {}

function pg_field_type(PgSqlResult $result, int $field): string {}

/**
 * @alias pg_field_type
 * @deprecated
 */
function pg_fieldtype(PgSqlResult $result, int $field): string {}

function pg_field_type_oid(PgSqlResult $result, int $field): string|int {}

function pg_field_num(PgSqlResult $result, string $field): int {}

/**
 * @alias pg_field_num
 * @deprecated
 */
function pg_fieldnum(PgSqlResult $result, string $field): int {}

/** @param string|int $row */
function pg_fetch_result(PgSqlResult $result, $row, string|int $field = UNKNOWN): string|false|null {}

/**
 * @param string|int $row
 * @alias pg_fetch_result
 * @deprecated
 */
function pg_result(PgSqlResult $result, $row, string|int $field = UNKNOWN): string|false|null {}

function pg_fetch_row(PgSqlResult $result, ?int $row = null, int $mode = PGSQL_NUM): array|false {}

function pg_fetch_assoc(PgSqlResult $result, ?int $row = null): array|false {}

function pg_fetch_array(PgSqlResult $result, ?int $row = null, int $mode = PGSQL_BOTH): array|false {}

function pg_fetch_object(PgSqlResult $result, ?int $row = null, string $class = "stdClass", array $constructor_args = []): object|false {}

function pg_fetch_all(PgSqlResult $result, int $mode = PGSQL_ASSOC): array {}

function pg_fetch_all_columns(PgSqlResult $result, int $field = 0): array {}

function pg_result_seek(PgSqlResult $result, int $row): bool {}

/** @param string|int $row */
function pg_field_prtlen(PgSqlResult $result, $row, string|int $field = UNKNOWN): int|false {}

/**
 * @param string|int $row
 * @alias pg_field_prtlen
 * @deprecated
 */
function pg_fieldprtlen(PgSqlResult $result, $row, string|int $field = UNKNOWN): int|false {}

/**
 * @param string|int $row
 */
function pg_field_is_null(PgSqlResult $result, $row, string|int $field = UNKNOWN): int|false {}

/**
 * @param string|int $row
 * @alias pg_field_is_null
 * @deprecated
 */
function pg_fieldisnull(PgSqlResult $result, $row, string|int $field = UNKNOWN): int|false {}

function pg_free_result(PgSqlResult $result): bool {}

/**
 * @alias pg_free_result
 * @deprecated
 */
function pg_freeresult(PgSqlResult $result): bool {}

function pg_last_oid(PgSqlResult $result): string|int|false {}

/**
 * @alias pg_last_oid
 * @deprecated
 */
function pg_getlastoid(PgSqlResult $result): string|int|false {}

function pg_trace(string $filename, string $mode = "w", ?PgSql $connection = null): bool {}

function pg_untrace(?PgSql $connection = null): bool {}

/**
 * @param PgSql $connection
 * @param string|int $oid
 */
function pg_lo_create($connection = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param PgSql $connection
 * @param string|int $oid
 * @alias pg_lo_create
 * @deprecated
 */
function pg_locreate($connection = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param PgSql $connection
 * @param string|int $oid
 */
function pg_lo_unlink($connection, $oid = UNKNOWN): bool {}

/**
 * @param PgSql $connection
 * @param string|int $oid
 * @alias pg_lo_unlink
 * @deprecated
 */
function pg_lounlink($connection, $oid = UNKNOWN): bool {}

/**
 * @param PgSql $connection
 * @param string|int $oid
 */
function pg_lo_open($connection, $oid = UNKNOWN, string $mode = UNKNOWN): PgSqlLob|false {}

/**
 * @param PgSql $connection
 * @param string|int $oid
 * @alias pg_lo_open
 * @deprecated
 */
function pg_loopen($connection, $oid = UNKNOWN, string $mode = UNKNOWN): PgSqlLob|false {}

function pg_lo_close(PgSqlLob $lob): bool {}

/**
 * @alias pg_lo_close
 * @deprecated
 */
function pg_loclose(PgSqlLob $lob): bool {}

function pg_lo_read(PgSqlLob $lob, int $length = 8192): string|false {}

/**
 * @alias pg_lo_read
 * @deprecated
 */
function pg_loread(PgSqlLob $lob, int $length = 8192): string|false {}

function pg_lo_write(PgSqlLob $lob, string $data, ?int $length = null): int|false {}

/**
 * @alias pg_lo_write
 * @deprecated
 */
function pg_lowrite(PgSqlLob $lob, string $data, ?int $length = null): int|false {}

function pg_lo_read_all(PgSqlLob $lob): int {}

/**
 * @alias pg_lo_read_all
 * @deprecated
 */
function pg_loreadall(PgSqlLob $lob): int {}

/**
 * @param PgSql|string $connection
 * @param string|int $filename
 * @param string|int $oid
 */
function pg_lo_import($connection, $filename = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param PgSql|string $connection
 * @param string|int $filename
 * @param string|int $oid
 * @alias pg_lo_import
 * @deprecated
 */
function pg_loimport($connection, $filename = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param PgSql|string|int $connection
 * @param string|int $oid
 * @param string|int $filename
 */
function pg_lo_export($connection, $oid = UNKNOWN, $filename = UNKNOWN): bool {}

/**
 * @param PgSql|string|int $connection
 * @param string|int $oid
 * @param string|int $filename
 * @alias pg_lo_export
 * @deprecated
 */
function pg_loexport($connection, $oid = UNKNOWN, $filename = UNKNOWN): bool {}

function pg_lo_seek(PgSqlLob $lob, int $offset, int $whence = SEEK_CUR): bool {}

function pg_lo_tell(PgSqlLob $lob): int {}

function pg_lo_truncate(PgSqlLob $lob, int $size): bool {}

/** @param PgSql|int $connection */
function pg_set_error_verbosity($connection, int $verbosity = UNKNOWN): int|false {}

/** @param PgSql|string $connection */
function pg_set_client_encoding($connection, string $encoding = UNKNOWN): int {}

/**
 * @param PgSql|string $connection
 * @alias pg_set_client_encoding
 * @deprecated
 */
function pg_setclientencoding($connection, string $encoding = UNKNOWN): int {}

function pg_client_encoding(?PgSql $connection = null): string {}

/**
 * @alias pg_client_encoding
 * @deprecated
 */
function pg_clientencoding(?PgSql $connection = null): string {}

function pg_end_copy(?PgSql $connection = null): bool {}

/** @param PgSql|string $connection */
function pg_put_line($connection, string $query = UNKNOWN): bool {}

function pg_copy_to(PgSql $connection, string $table_name, string $separator = "\t", string $null_as = "\\\\N"): array|false {}

function pg_copy_from(PgSql $connection, string $table_name, array $rows, string $separator = "\t", string $null_as = "\\\\N"): bool {}

/** @param PgSql|string $connection */
function pg_escape_string($connection, string $string = UNKNOWN): string {}

/** @param PgSql|string $connection */
function pg_escape_bytea($connection, string $string = UNKNOWN): string {}

function pg_unescape_bytea(string $string): string {}

/** @param PgSql|string $connection */
function pg_escape_literal($connection, string $string = UNKNOWN): string|false {}

/** @param PgSql|string $connection */
function pg_escape_identifier($connection, string $string = UNKNOWN): string|false {}

function pg_result_error(PgSqlResult $result): string|false {}

function pg_result_error_field(PgSqlResult $result, int $field_code): string|false|null {}

function pg_connection_status(PgSql $connection): int {}

function pg_transaction_status(PgSql $connection): int {}

function pg_connection_reset(PgSql $connection): bool {}

function pg_cancel_query(PgSql $connection): bool {}

function pg_connection_busy(PgSql $connection): bool {}

function pg_send_query(PgSql $connection, string $query): int|bool {}

function pg_send_query_params(PgSql $connection, string $query, array $params): int|bool {}

function pg_send_prepare(PgSql $connection, string $statement_name, string $query): int|bool {}

function pg_send_execute(PgSql $connection, string $query, array $params): int|bool {}

function pg_get_result(PgSql $connection): PgSqlResult|false {}

function pg_result_status(PgSqlResult $result, int $mode = PGSQL_STATUS_LONG): string|int {}

function pg_get_notify(PgSql $connection, int $mode = PGSQL_ASSOC): array|false {}

function pg_get_pid(PgSql $connection): int {}

/** @return resource|false */
function pg_socket(PgSql $connection) {}

function pg_consume_input(PgSql $connection): bool {}

function pg_flush(PgSql $connection): int|bool {}

function pg_meta_data(PgSql $connection, string $table_name, bool $extended = false): array|false {}

function pg_convert(PgSql $connection, string $table_name, array $values, int $flags = 0): array|false {}

function pg_insert(PgSql $connection, string $table_name, array $values, int $flags = PGSQL_DML_EXEC): PgSqlResult|string|bool {}

function pg_update(PgSql $connection, string $table_name, array $values, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

function pg_delete(PgSql $connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

function pg_select(PgSql $connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC, int $mode = PGSQL_ASSOC): array|string|false {}
