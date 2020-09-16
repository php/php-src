<?php

/** @generate-function-entries */

/** @return resource|false */
function pg_connect(string $connection_string, int $connection_type = 0) {}

/** @return resource|false */
function pg_pconnect(string $connection_string, int $connection_type = 0) {}

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
function pg_parameter_status($connection, string $param_name = UNKNOWN): string|false {}

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
function pg_prepare($connection, string $stmt_name, string $query = UNKNOWN) {}

/**
 * @param resource|string $connection
 * @param string|array $stmt_name
 * @return resource|false
 */
function pg_execute($connection, $stmt_name, array $params = UNKNOWN) {}

/** @param resource $result */
function pg_num_rows($result): int {}

/**
 * @param resource $result
 * @alias pg_num_rows
 */
function pg_numrows($result): int {}

/** @param resource $result */
function pg_num_fields($result): int {}

/**
 * @param resource $result
 * @alias pg_num_fields
 */
function pg_numfields($result): int {}

/** @param resource $result */
function pg_affected_rows($result): int {}

/**
 * @param resource $result
 * @alias pg_affected_rows
 */
function pg_cmdtuples($result): int {}

/** @param resource $connection */
function pg_last_notice($connection, int $option = PGSQL_NOTICE_LAST): array|string|bool {}

/** @param resource $result */
function pg_field_table($result, int $field_number, bool $oid_only = false): string|int|false {}

/** @param resource $result */
function pg_field_name($result, int $field_number): string|false {}

/**
 * @param resource $result
 * @alias pg_field_name
 */
function pg_fieldname($result, int $field_number): string|false {}

/** @param resource $result */
function pg_field_size($result, int $field_number): int|false {}

/**
 * @param resource $result
 * @alias pg_field_size
 */
function pg_fieldsize($result, int $field_number): int|false {}

/** @param resource $result */
function pg_field_type($result, int $field_number): string|false {}

/**
 * @param resource $result
 * @alias pg_field_type
 */
function pg_fieldtype($result, int $field_number): string|false {}

/** @param resource $result */
function pg_field_type_oid($result, int $field_number): string|int|false {}

/** @param resource $result */
function pg_field_num($result, string $field_name): int {}

/**
 * @param resource $result
 * @alias pg_field_num
 */
function pg_fieldnum($result, string $field_name): int {}

/**
 * @param resource $result
 * @param string|int $row_number
 */
function pg_fetch_result($result, $row_number, string|int $field = UNKNOWN): string|false|null {}

/**
 * @param resource $result
 * @param string|int $row_number
 * @alias pg_fetch_result
 */
function pg_result($result, $row_number, string|int $field = UNKNOWN): string|false|null {}

/**
 * @param resource $result
 */
function pg_fetch_row($result, ?int $row_number = null, int $result_type = PGSQL_NUM): array|false {}

/**
 * @param resource $result
 */
function pg_fetch_assoc($result, ?int $row_number = null): array|false {}

/**
 * @param resource $result
 */
function pg_fetch_array($result, ?int $row_number = null, int $result_type = PGSQL_BOTH): array|false {}

/** @param resource $result */
function pg_fetch_object($result, ?int $row_number = null, string $class_name = "stdClass", ?array $ctor_params = null): object|false {}

/** @param resource $result */
function pg_fetch_all($result, int $result_type = PGSQL_ASSOC): array|false {}

/** @param resource $result */
function pg_fetch_all_columns($result, int $column_number = 0): array|false {}

/** @param resource $result */
function pg_result_seek($result, int $row_number): bool {}

/**
 * @param resource $result
 * @param string|int $row_number
 */
function pg_field_prtlen($result, $row_number, string|int $field = UNKNOWN): int|false {}

/**
 * @param resource $result
 * @param string|int $row_number
 * @alias pg_field_prtlen
 */
function pg_fieldprtlen($result, $row_number, string|int $field = UNKNOWN): int|false {}

/**
 * @param resource $result
 * @param string|int $row_number
 */
function pg_field_is_null($result, $row_number, string|int $field = UNKNOWN): int|false {}

/**
 * @param resource $result
 * @param string|int $row_number
 * @alias pg_field_is_null
 */
function pg_fieldisnull($result, $row_number, string|int $field = UNKNOWN): int|false {}

/** @param resource $result */
function pg_free_result($result): bool {}

/**
 * @param resource $result
 * @alias pg_free_result
 */
function pg_freeresult($result): bool {}

/** @param resource $result */
function pg_last_oid($result): string|int|false {}

/**
 * @param resource $result
 * @alias pg_last_oid
 */
function pg_getlastoid($result): string|int|false {}

/** @param resource|null $connection */
function pg_trace(string $filename, string $mode = "w", $connection = null): bool {}

/** @param resource|null $connection */
function pg_untrace($connection = null): bool {}

/**
 * @param resource $connection
 * @param string|int $large_object_id
 */
function pg_lo_create($connection = UNKNOWN, $large_object_id = UNKNOWN): string|int|false {}

/**
 * @param resource $connection
 * @param string|int $large_object_id
 * @alias pg_lo_create
 */
function pg_locreate($connection = UNKNOWN, $large_object_id = UNKNOWN): string|int|false {}

/**
 * @param resource $connection
 * @param string|int $large_object_id
 */
function pg_lo_unlink($connection, $large_object_id = UNKNOWN): bool {}

/**
 * @param resource $connection
 * @param string|int $large_object_id
 * @alias pg_lo_unlink
 */
function pg_lounlink($connection, $large_object_id = UNKNOWN): bool {}

/**
 * @param resource $connection
 * @param string|int $large_object_id
 * @return resource|false
 */
function pg_lo_open($connection, $large_object_id = UNKNOWN, string $mode = UNKNOWN) {}

/**
 * @param resource $connection
 * @param string|int $large_object_id
 * @return resource|false
 * @alias pg_lo_open
 */
function pg_loopen($connection, $large_object_id = UNKNOWN, string $mode = UNKNOWN) {}

/** @param resource $large_object */
function pg_lo_close($large_object): bool {}

/**
 * @param resource $large_object
 * @alias pg_lo_close
 */
function pg_loclose($large_object): bool {}

/** @param resource $large_object */
function pg_lo_read($large_object, int $len = 8192): string|false {}

/**
 * @param resource $large_object
 * @alias pg_lo_read
 */
function pg_loread($large_object, int $len = 8192): string|false {}

/** @param resource $large_object */
function pg_lo_write($large_object, string $buf, ?int $len = null): int|false {}

/**
 * @param resource $large_object
 * @alias pg_lo_write
 */
function pg_lowrite($large_object, string $buf, ?int $len = null): int|false {}

/** @param resource $large_object */
function pg_lo_read_all($large_object): int {}

/**
 * @param resource $large_object
 * @alias pg_lo_read_all
 */
function pg_loreadall($large_object): int {}

/**
 * @param resource|string $connection
 * @param string|int $filename
 * @param string|int $large_object_id
 * @return resource|false
 */
function pg_lo_import($connection, $filename = UNKNOWN, $large_object_id = UNKNOWN): string|int|false {}

/**
 * @param resource|string $connection
 * @param string|int $filename
 * @param string|int $large_object_id
 * @return resource|false
 * @alias pg_lo_import
 */
function pg_loimport($connection, $filename = UNKNOWN, $large_object_id = UNKNOWN): string|int|false {}

/**
 * @param resource|string|int $connection
 * @param string|int $large_object_id
 * @param string|int $filename
 * @return resource|false
 */
function pg_lo_export($connection, $large_object_id = UNKNOWN, $filename = UNKNOWN): bool {}

/**
 * @param resource|string|int $connection
 * @param string|int $large_object_id
 * @param string|int $filename
 * @return resource|false
 * @alias pg_lo_export
 */
function pg_loexport($connection, $large_object_id = UNKNOWN, $filename = UNKNOWN): bool {}

/** @param resource $large_object */
function pg_lo_seek($large_object, int $offset, int $whence = SEEK_CUR): bool {}

/** @param resource $large_object */
function pg_lo_tell($large_object): int {}

/** @param resource $large_object */
function pg_lo_truncate($large_object, int $size): bool {}

/** @param resource|int $connection */
function pg_set_error_verbosity($connection, int $verbosity = UNKNOWN): int|false {}

/** @param resource|string $connection */
function pg_set_client_encoding($connection, string $encoding = UNKNOWN): int {}

/**
 * @param resource|string $connection
 * @alias pg_set_client_encoding
 */
function pg_setclientencoding($connection, string $encoding = UNKNOWN): int {}

/** @param resource|null $connection */
function pg_client_encoding($connection = null): string {}

/**
 * @param resource|null $connection
 * @alias pg_client_encoding
 */
function pg_clientencoding($connection = null): string {}

/** @param resource|null $connection */
function pg_end_copy($connection = null): bool {}

/** @param resource|string $connection */
function pg_put_line($connection, string $query = UNKNOWN): bool {}

/** @param resource $connection */
function pg_copy_to($connection, string $table_name, string $delimiter = "\t", string $null_as = "\\\\N"): array|false {}

/** @param resource $connection */
function pg_copy_from($connection, string $table_name, array $rows, string $delimiter = "\t", string $null_as = "\\\\N"): bool {}

/** @param resource|string $connection */
function pg_escape_string($connection, string $data = UNKNOWN): string {}

/** @param resource|string $connection */
function pg_escape_bytea($connection, string $data = UNKNOWN): string {}

function pg_unescape_bytea(?string $data = null): string|false {}

/** @param resource|string $connection */
function pg_escape_literal($connection, string $data = UNKNOWN): string|false {}

/** @param resource|string $connection */
function pg_escape_identifier($connection, string $data = UNKNOWN): string|false {}

/** @param resource $result */
function pg_result_error($result): string|false {}

/** @param resource $result */
function pg_result_error_field($result, int $fieldcode): string|false|null {}

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
function pg_send_prepare($connection, string $stmtname, string $query): int|bool {}

/** @param resource $connection */
function pg_send_execute($connection, string $query, array $params): int|bool {}

/**
 * @param resource $connection
 * @return resource|false
 */
function pg_get_result($connection) {}

/** @param resource $result */
function pg_result_status($result, int $result_type = PGSQL_STATUS_LONG): string|int {}

/** @param resource $result */
function pg_get_notify($result, int $result_type = PGSQL_ASSOC): array|false {}

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
function pg_convert($connection, string $table_name, array $values, int $options = 0): array|false {}

/**
 * @param resource $connection
 * @return resource|string|bool
 */
function pg_insert($connection, string $table_name, array $values, int $options = 0) {}

/** @param resource $connection */
function pg_update($connection, string $table_name, array $values, array $ids, int $options = 0): string|bool {}

/** @param resource $connection */
function pg_delete($connection, string $table_name, array $ids, int $options = 0): string|bool {}

/** @param resource $connection */
function pg_select($connection, string $table_name, array $ids, int $options = 0, int $result_type = PGSQL_ASSOC): array|string|false {}
