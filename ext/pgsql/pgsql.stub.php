<?php

/** @generate-class-entries */

namespace PgSql {
    /** @strict-properties */
    final class Connection
    {
    }

    /** @strict-properties */
    final class Result
    {
    }

    /** @strict-properties */
    final class Lob
    {
    }
}

namespace {
    function pg_connect(string $connection_string, int $flags = 0): PgSql\Connection|false {}

    function pg_pconnect(string $connection_string, int $flags = 0): PgSql\Connection|false {}

    function pg_connect_poll(PgSql\Connection $connection): int {}

    function pg_close(?PgSql\Connection $connection = null): bool {}

    function pg_dbname(?PgSql\Connection $connection = null): string {}

    function pg_last_error(?PgSql\Connection $connection = null): string {}

    /**
     * @alias pg_last_error
     * @deprecated
     */
    function pg_errormessage(?PgSql\Connection $connection = null): string {}

    function pg_options(?PgSql\Connection $connection = null): string {}

    function pg_port(?PgSql\Connection $connection = null): string {}

    function pg_tty(?PgSql\Connection $connection = null): string {}

    function pg_host(?PgSql\Connection $connection = null): string {}

    function pg_version(?PgSql\Connection $connection = null): array {}

    /** @param PgSql\Connection|string $connection */
    function pg_parameter_status($connection, string $name = UNKNOWN): string|false {}

    function pg_ping(?PgSql\Connection $connection = null): bool {}

    /** @param PgSql\Connection|string $connection */
    function pg_query($connection, string $query = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @alias pg_query
     */
    function pg_exec($connection, string $query = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @param string|array $query
     */
    function pg_query_params($connection, $query, array $params = UNKNOWN): PgSql\Result|false {}

    /** @param PgSql\Connection|string $connection */
    function pg_prepare($connection, string $statement_name, string $query = UNKNOWN): PgSql\Result|false {}

    /**
     * @param PgSql\Connection|string $connection
     * @param string|array $statement_name
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

    function pg_field_type_oid(PgSql\Result $result, int $field): string|int {}

    function pg_field_num(PgSql\Result $result, string $field): int {}

    /**
     * @alias pg_field_num
     * @deprecated
     */
    function pg_fieldnum(PgSql\Result $result, string $field): int {}

    /** @param string|int $row */
    function pg_fetch_result(PgSql\Result $result, $row, string|int $field = UNKNOWN): string|false|null {}

    /**
     * @param string|int $row
     * @alias pg_fetch_result
     * @deprecated
     */
    function pg_result(PgSql\Result $result, $row, string|int $field = UNKNOWN): string|false|null {}

    function pg_fetch_row(PgSql\Result $result, ?int $row = null, int $mode = PGSQL_NUM): array|false {}

    function pg_fetch_assoc(PgSql\Result $result, ?int $row = null): array|false {}

    function pg_fetch_array(PgSql\Result $result, ?int $row = null, int $mode = PGSQL_BOTH): array|false {}

    function pg_fetch_object(PgSql\Result $result, ?int $row = null, string $class = "stdClass", array $constructor_args = []): object|false {}

    function pg_fetch_all(PgSql\Result $result, int $mode = PGSQL_ASSOC): array {}

    function pg_fetch_all_columns(PgSql\Result $result, int $field = 0): array {}

    function pg_result_seek(PgSql\Result $result, int $row): bool {}

    /** @param string|int $row */
    function pg_field_prtlen(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    /**
     * @param string|int $row
     * @alias pg_field_prtlen
     * @deprecated
     */
    function pg_fieldprtlen(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    /**
     * @param string|int $row
     */
    function pg_field_is_null(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    /**
     * @param string|int $row
     * @alias pg_field_is_null
     * @deprecated
     */
    function pg_fieldisnull(PgSql\Result $result, $row, string|int $field = UNKNOWN): int|false {}

    function pg_free_result(PgSql\Result $result): bool {}

    /**
     * @alias pg_free_result
     * @deprecated
     */
    function pg_freeresult(PgSql\Result $result): bool {}

    function pg_last_oid(PgSql\Result $result): string|int|false {}

    /**
     * @alias pg_last_oid
     * @deprecated
     */
    function pg_getlastoid(PgSql\Result $result): string|int|false {}

    function pg_trace(string $filename, string $mode = "w", ?PgSql\Connection $connection = null): bool {}

    function pg_untrace(?PgSql\Connection $connection = null): bool {}

    /**
     * @param PgSql\Connection $connection
     * @param string|int $oid
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

    function pg_copy_to(PgSql\Connection $connection, string $table_name, string $separator = "\t", string $null_as = "\\\\N"): array|false {}

    function pg_copy_from(PgSql\Connection $connection, string $table_name, array $rows, string $separator = "\t", string $null_as = "\\\\N"): bool {}

    /** @param PgSql\Connection|string $connection */
    function pg_escape_string($connection, string $string = UNKNOWN): string {}

    /** @param PgSql\Connection|string $connection */
    function pg_escape_bytea($connection, string $string = UNKNOWN): string {}

    function pg_unescape_bytea(string $string): string {}

    /** @param PgSql\Connection|string $connection */
    function pg_escape_literal($connection, string $string = UNKNOWN): string|false {}

    /** @param PgSql\Connection|string $connection */
    function pg_escape_identifier($connection, string $string = UNKNOWN): string|false {}

    function pg_result_error(PgSql\Result $result): string|false {}

    function pg_result_error_field(PgSql\Result $result, int $field_code): string|false|null {}

    function pg_connection_status(PgSql\Connection $connection): int {}

    function pg_transaction_status(PgSql\Connection $connection): int {}

    function pg_connection_reset(PgSql\Connection $connection): bool {}

    function pg_cancel_query(PgSql\Connection $connection): bool {}

    function pg_connection_busy(PgSql\Connection $connection): bool {}

    function pg_send_query(PgSql\Connection $connection, string $query): int|bool {}

    function pg_send_query_params(PgSql\Connection $connection, string $query, array $params): int|bool {}

    function pg_send_prepare(PgSql\Connection $connection, string $statement_name, string $query): int|bool {}

    function pg_send_execute(PgSql\Connection $connection, string $query, array $params): int|bool {}

    function pg_get_result(PgSql\Connection $connection): PgSql\Result|false {}

    function pg_result_status(PgSql\Result $result, int $mode = PGSQL_STATUS_LONG): string|int {}

    function pg_get_notify(PgSql\Connection $connection, int $mode = PGSQL_ASSOC): array|false {}

    function pg_get_pid(PgSql\Connection $connection): int {}

    /** @return resource|false */
    function pg_socket(PgSql\Connection $connection) {}

    function pg_consume_input(PgSql\Connection $connection): bool {}

    function pg_flush(PgSql\Connection $connection): int|bool {}

    function pg_meta_data(PgSql\Connection $connection, string $table_name, bool $extended = false): array|false {}

    function pg_convert(PgSql\Connection $connection, string $table_name, array $values, int $flags = 0): array|false {}

    function pg_insert(PgSql\Connection $connection, string $table_name, array $values, int $flags = PGSQL_DML_EXEC): PgSql\Result|string|bool {}

    function pg_update(PgSql\Connection $connection, string $table_name, array $values, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

    function pg_delete(PgSql\Connection $connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

    function pg_select(PgSql\Connection $connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC, int $mode = PGSQL_ASSOC): array|string|false {}
}
