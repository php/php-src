<?php

/** @generate-function-entries */

/** @return resource|false */
function pg_connect(string $connection_string, int $flags = 0) {}

/** @return resource|false */
function pg_pconnect(string $connection_string, int $flags = 0) {}

/** @param resource $connection */
function pg_connect_poll($connection): int {}

/** @param resource|null $connection */
function pg_close($connection = null): bool {}

/** @param resource|null $connection */
function pg_dbname($connection = null): string {}

/** @param resource|null $connection */
function pg_last_error($connection = null): string {}

/**
 * @param resource|null $connection
 * @alias pg_last_error
 * @deprecated
 */
function pg_errormessage($connection = null): string {}

/** @param resource|null $connection */
function pg_options($connection = null): string {}

/** @param resource|null $connection */
function pg_port($connection = null): string {}

/** @param resource|null $connection */
function pg_tty($connection = null): string {}

/** @param resource|null $connection */
function pg_host($connection = null): string {}

/** @param resource|null $connection */
function pg_version($connection = null): array {}

/** @param resource|string $connection */
function pg_parameter_status($connection, string $name = UNKNOWN): string|false {}

/** @param resource|null $connection */
function pg_ping($connection = null): bool {}

/**
 * @param resource|string $connection
 * @return resource|false
 */
function pg_query($connection, string $query = UNKNOWN) {}

/**
 * @param resource|string $connection
 * @return resource|false
 * @alias pg_query
 */
function pg_exec($connection, string $query = UNKNOWN) {}

/**
 * @param resource|string $connection
 * @param string|array $query
 * @return resource|false
 */
function pg_query_params($connection, $query, array $params = UNKNOWN) {}

/**
 * @param resource|string $connection
 * @return resource|false
 */
function pg_prepare($connection, string $statement_name, string $query = UNKNOWN) {}

/**
 * @param resource|string $connection
 * @param string|array $statement_name
 * @return resource|false
 */
function pg_execute($connection, $statement_name, array $params = UNKNOWN) {}

/** @param resource $result */
function pg_num_rows($result): int {}

/**
 * @param resource $result
 * @alias pg_num_rows
 * @deprecated
 */
function pg_numrows($result): int {}

/** @param resource $result */
function pg_num_fields($result): int {}

/**
 * @param resource $result
 * @alias pg_num_fields
 * @deprecated
 */
function pg_numfields($result): int {}

/** @param resource $result */
function pg_affected_rows($result): int {}

/**
 * @param resource $result
 * @alias pg_affected_rows
 * @deprecated
 */
function pg_cmdtuples($result): int {}

/** @param resource $connection */
function pg_last_notice($connection, int $mode = PGSQL_NOTICE_LAST): array|string|bool {}

/** @param resource $result */
function pg_field_table($result, int $field, bool $oid_only = false): string|int|false {}

/** @param resource $result */
function pg_field_name($result, int $field): string {}

/**
 * @param resource $result
 * @alias pg_field_name
 * @deprecated
 */
function pg_fieldname($result, int $field): string {}

/** @param resource $result */
function pg_field_size($result, int $field): int {}

/**
 * @param resource $result
 * @alias pg_field_size
 * @deprecated
 */
function pg_fieldsize($result, int $field): int {}

/** @param resource $result */
function pg_field_type($result, int $field): string {}

/**
 * @param resource $result
 * @alias pg_field_type
 * @deprecated
 */
function pg_fieldtype($result, int $field): string {}

/** @param resource $result */
function pg_field_type_oid($result, int $field): string|int {}

/** @param resource $result */
function pg_field_num($result, string $field): int {}

/**
 * @param resource $result
 * @alias pg_field_num
 * @deprecated
 */
function pg_fieldnum($result, string $field): int {}

/**
 * @param resource $result
 * @param string|int $row
 */
function pg_fetch_result($result, $row, string|int $field = UNKNOWN): string|false|null {}

/**
 * @param resource $result
 * @param string|int $row
 * @alias pg_fetch_result
 * @deprecated
 */
function pg_result($result, $row, string|int $field = UNKNOWN): string|false|null {}

/**
 * @param resource $result
 */
function pg_fetch_row($result, ?int $row = null, int $mode = PGSQL_NUM): array|false {}

/**
 * @param resource $result
 */
function pg_fetch_assoc($result, ?int $row = null): array|false {}

/**
 * @param resource $result
 */
function pg_fetch_array($result, ?int $row = null, int $mode = PGSQL_BOTH): array|false {}

/** @param resource $result */
function pg_fetch_object($result, ?int $row = null, string $class = "stdClass", array $constructor_args = []): object|false {}

/** @param resource $result */
function pg_fetch_all($result, int $mode = PGSQL_ASSOC): array {}

/** @param resource $result */
function pg_fetch_all_columns($result, int $field = 0): array {}

/** @param resource $result */
function pg_result_seek($result, int $row): bool {}

/**
 * @param resource $result
 * @param string|int $row
 */
function pg_field_prtlen($result, $row, string|int $field = UNKNOWN): int|false {}

/**
 * @param resource $result
 * @param string|int $row
 * @alias pg_field_prtlen
 * @deprecated
 */
function pg_fieldprtlen($result, $row, string|int $field = UNKNOWN): int|false {}

/**
 * @param resource $result
 * @param string|int $row
 */
function pg_field_is_null($result, $row, string|int $field = UNKNOWN): int|false {}

/**
 * @param resource $result
 * @param string|int $row
 * @alias pg_field_is_null
 * @deprecated
 */
function pg_fieldisnull($result, $row, string|int $field = UNKNOWN): int|false {}

/** @param resource $result */
function pg_free_result($result): bool {}

/**
 * @param resource $result
 * @alias pg_free_result
 * @deprecated
 */
function pg_freeresult($result): bool {}

/** @param resource $result */
function pg_last_oid($result): string|int|false {}

/**
 * @param resource $result
 * @alias pg_last_oid
 * @deprecated
 */
function pg_getlastoid($result): string|int|false {}

/** @param resource|null $connection */
function pg_trace(string $filename, string $mode = "w", $connection = null): bool {}

/** @param resource|null $connection */
function pg_untrace($connection = null): bool {}

/**
 * @param resource $connection
 * @param string|int $oid
 */
function pg_lo_create($connection = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param resource $connection
 * @param string|int $oid
 * @alias pg_lo_create
 * @deprecated
 */
function pg_locreate($connection = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param resource $connection
 * @param string|int $oid
 */
function pg_lo_unlink($connection, $oid = UNKNOWN): bool {}

/**
 * @param resource $connection
 * @param string|int $oid
 * @alias pg_lo_unlink
 * @deprecated
 */
function pg_lounlink($connection, $oid = UNKNOWN): bool {}

/**
 * @param resource $connection
 * @param string|int $oid
 * @return resource|false
 */
function pg_lo_open($connection, $oid = UNKNOWN, string $mode = UNKNOWN) {}

/**
 * @param resource $connection
 * @param string|int $oid
 * @return resource|false
 * @alias pg_lo_open
 * @deprecated
 */
function pg_loopen($connection, $oid = UNKNOWN, string $mode = UNKNOWN) {}

/** @param resource $lob */
function pg_lo_close($lob): bool {}

/**
 * @param resource $lob
 * @alias pg_lo_close
 * @deprecated
 */
function pg_loclose($lob): bool {}

/** @param resource $lob */
function pg_lo_read($lob, int $length = 8192): string|false {}

/**
 * @param resource $lob
 * @alias pg_lo_read
 * @deprecated
 */
function pg_loread($lob, int $length = 8192): string|false {}

/** @param resource $lob */
function pg_lo_write($lob, string $data, ?int $length = null): int|false {}

/**
 * @param resource $lob
 * @alias pg_lo_write
 * @deprecated
 */
function pg_lowrite($lob, string $data, ?int $length = null): int|false {}

/** @param resource $lob */
function pg_lo_read_all($lob): int {}

/**
 * @param resource $lob
 * @alias pg_lo_read_all
 * @deprecated
 */
function pg_loreadall($lob): int {}

/**
 * @param resource|string $connection
 * @param string|int $filename
 * @param string|int $oid
 * @return resource|false
 */
function pg_lo_import($connection, $filename = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param resource|string $connection
 * @param string|int $filename
 * @param string|int $oid
 * @return resource|false
 * @alias pg_lo_import
 * @deprecated
 */
function pg_loimport($connection, $filename = UNKNOWN, $oid = UNKNOWN): string|int|false {}

/**
 * @param resource|string|int $connection
 * @param string|int $oid
 * @param string|int $filename
 * @return resource|false
 */
function pg_lo_export($connection, $oid = UNKNOWN, $filename = UNKNOWN): bool {}

/**
 * @param resource|string|int $connection
 * @param string|int $oid
 * @param string|int $filename
 * @return resource|false
 * @alias pg_lo_export
 * @deprecated
 */
function pg_loexport($connection, $oid = UNKNOWN, $filename = UNKNOWN): bool {}

/** @param resource $lob */
function pg_lo_seek($lob, int $offset, int $whence = SEEK_CUR): bool {}

/** @param resource $lob */
function pg_lo_tell($lob): int {}

/** @param resource $lob */
function pg_lo_truncate($lob, int $size): bool {}

/** @param resource|int $connection */
function pg_set_error_verbosity($connection, int $verbosity = UNKNOWN): int|false {}

/** @param resource|string $connection */
function pg_set_client_encoding($connection, string $encoding = UNKNOWN): int {}

/**
 * @param resource|string $connection
 * @alias pg_set_client_encoding
 * @deprecated
 */
function pg_setclientencoding($connection, string $encoding = UNKNOWN): int {}

/** @param resource|null $connection */
function pg_client_encoding($connection = null): string {}

/**
 * @param resource|null $connection
 * @alias pg_client_encoding
 * @deprecated
 */
function pg_clientencoding($connection = null): string {}

/** @param resource|null $connection */
function pg_end_copy($connection = null): bool {}

/** @param resource|string $connection */
function pg_put_line($connection, string $query = UNKNOWN): bool {}

/** @param resource $connection */
function pg_copy_to($connection, string $table_name, string $separator = "\t", string $null_as = "\\\\N"): array|false {}

/** @param resource $connection */
function pg_copy_from($connection, string $table_name, array $rows, string $separator = "\t", string $null_as = "\\\\N"): bool {}

/** @param resource|string $connection */
function pg_escape_string($connection, string $string = UNKNOWN): string {}

/** @param resource|string $connection */
function pg_escape_bytea($connection, string $string = UNKNOWN): string {}

function pg_unescape_bytea(string $string): string {}

/** @param resource|string $connection */
function pg_escape_literal($connection, string $string = UNKNOWN): string|false {}

/** @param resource|string $connection */
function pg_escape_identifier($connection, string $string = UNKNOWN): string|false {}

/** @param resource $result */
function pg_result_error($result): string|false {}

/** @param resource $result */
function pg_result_error_field($result, int $field_code): string|false|null {}

/** @param resource $connection */
function pg_connection_status($connection): int {}

/** @param resource $connection */
function pg_transaction_status($connection): int {}

/** @param resource $connection */
function pg_connection_reset($connection): bool {}

/** @param resource $connection */
function pg_cancel_query($connection): bool {}

/** @param resource $connection */
function pg_connection_busy($connection): bool {}

/** @param resource $connection */
function pg_send_query($connection, string $query): int|bool {}

/** @param resource $connection */
function pg_send_query_params($connection, string $query, array $params): int|bool {}

/** @param resource $connection */
function pg_send_prepare($connection, string $statement_name, string $query): int|bool {}

/** @param resource $connection */
function pg_send_execute($connection, string $statement_name, array $params): int|bool {}

/**
 * @param resource $connection
 * @return resource|false
 */
function pg_get_result($connection) {}

/** @param resource $result */
function pg_result_status($result, int $mode = PGSQL_STATUS_LONG): string|int {}

/** @param resource $connection */
function pg_get_notify($connection, int $mode = PGSQL_ASSOC): array|false {}

/** @param resource $connection */
function pg_get_pid($connection): int {}

/**
 * @param resource $connection
 * @return resource|false
 */
function pg_socket($connection) {}

/** @param resource $connection */
function pg_consume_input($connection): bool {}

/** @param resource $connection */
function pg_flush($connection): int|bool {}

/** @param resource $connection */
function pg_meta_data($connection, string $table_name, bool $extended = false): array|false {}

/** @param resource $connection */
function pg_convert($connection, string $table_name, array $values, int $flags = 0): array|false {}

/**
 * @param resource $connection
 * @return resource|string|bool
 */
function pg_insert($connection, string $table_name, array $values, int $flags = PGSQL_DML_EXEC) {}

/** @param resource $connection */
function pg_update($connection, string $table_name, array $values, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

/** @param resource $connection */
function pg_delete($connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC): string|bool {}

/** @param resource $connection */
function pg_select($connection, string $table_name, array $conditions, int $flags = PGSQL_DML_EXEC, int $mode = PGSQL_ASSOC): array|string|false {}
