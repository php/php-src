<?php

/** @generate-function-entries */

/** @return resource|false */
function pg_connect(string $connection_string, int $pgsql_type = 0) {}

/** @return resource|false */
function pg_pconnect(string $connection_string, int $pgsql_type = 0) {}

/** @param resource $pgsql */
function pg_connect_poll($pgsql): int {}

/** @param resource|null $pgsql */
function pg_close($pgsql = null): bool {}

/** @param resource|null $pgsql */
function pg_dbname($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_last_error($pgsql = null): string {}

/**
 * @param resource|null $pgsql
 * @alias pg_last_error
 * @deprecated
 */
function pg_errormessage($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_options($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_port($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_tty($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_host($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_version($pgsql = null): array {}

/** @param resource|string $pgsql */
function pg_parameter_status($pgsql, string $param_name = UNKNOWN): string|false {}

/** @param resource|null $pgsql */
function pg_ping($pgsql = null): bool {}

/**
 * @param resource|string $pgsql
 * @return resource|false
 */
function pg_query($pgsql, string $query = UNKNOWN) {}

/**
 * @param resource|string $pgsql
 * @return resource|false
 * @alias pg_query
 */
function pg_exec($pgsql, string $query = UNKNOWN) {}

/**
 * @param resource|string $pgsql
 * @param string|array $query
 * @return resource|false
 */
function pg_query_params($pgsql, $query, array $params = UNKNOWN) {}

/**
 * @param resource|string $pgsql
 * @return resource|false
 */
function pg_prepare($pgsql, string $stmt, string $query = UNKNOWN) {}

/**
 * @param resource|string $pgsql
 * @param string|array $stmt
 * @return resource|false
 */
function pg_execute($pgsql, $stmt, array $params = UNKNOWN) {}

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

/** @param resource $pgsql */
function pg_last_notice($pgsql, int $type = PGSQL_NOTICE_LAST): array|string|bool {}

/** @param resource $result */
function pg_field_table($result, int $index, bool $oid_only = false): string|int|false {}

/** @param resource $result */
function pg_field_name($result, int $index): string {}

/**
 * @param resource $result
 * @alias pg_field_name
 * @deprecated
 */
function pg_fieldname($result, int $index): string {}

/** @param resource $result */
function pg_field_size($result, int $index): int {}

/**
 * @param resource $result
 * @alias pg_field_size
 * @deprecated
 */
function pg_fieldsize($result, int $index): int {}

/** @param resource $result */
function pg_field_type($result, int $index): string {}

/**
 * @param resource $result
 * @alias pg_field_type
 * @deprecated
 */
function pg_fieldtype($result, int $index): string {}

/** @param resource $result */
function pg_field_type_oid($result, int $index): string|int {}

/** @param resource $result */
function pg_field_num($result, string $field_name): int {}

/**
 * @param resource $result
 * @alias pg_field_num
 * @deprecated
 */
function pg_fieldnum($result, string $field_name): int {}

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
function pg_fetch_row($result, ?int $row = null, int $result_type = PGSQL_NUM): array|false {}

/**
 * @param resource $result
 */
function pg_fetch_assoc($result, ?int $row = null): array|false {}

/**
 * @param resource $result
 */
function pg_fetch_array($result, ?int $row = null, int $mode = PGSQL_BOTH): array|false {}

/** @param resource $result */
function pg_fetch_object($result, ?int $row = null, string $class = "stdClass", ?array $constructor_args = null): object|false {}

/** @param resource $result */
function pg_fetch_all($result, int $mode = PGSQL_ASSOC): array {}

/** @param resource $result */
function pg_fetch_all_columns($result, int $index = 0): array {}

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

/** @param resource|null $pgsql */
function pg_trace(string $filename, string $mode = "w", $pgsql = null): bool {}

/** @param resource|null $pgsql */
function pg_untrace($pgsql = null): bool {}

/**
 * @param resource $pgsql
 * @param string|int $object_id
 */
function pg_lo_create($pgsql = UNKNOWN, $object_id = UNKNOWN): string|int|false {}

/**
 * @param resource $pgsql
 * @param string|int $object_id
 * @alias pg_lo_create
 * @deprecated
 */
function pg_locreate($pgsql = UNKNOWN, $object_id = UNKNOWN): string|int|false {}

/**
 * @param resource $pgsql
 * @param string|int $object_id
 */
function pg_lo_unlink($pgsql, $object_id = UNKNOWN): bool {}

/**
 * @param resource $pgsql
 * @param string|int $object_id
 * @alias pg_lo_unlink
 * @deprecated
 */
function pg_lounlink($pgsql, $object_id = UNKNOWN): bool {}

/**
 * @param resource $pgsql
 * @param string|int $object_id
 * @return resource|false
 */
function pg_lo_open($pgsql, $object_id = UNKNOWN, string $mode = UNKNOWN) {}

/**
 * @param resource $pgsql
 * @param string|int $object_id
 * @return resource|false
 * @alias pg_lo_open
 * @deprecated
 */
function pg_loopen($pgsql, $object_id = UNKNOWN, string $mode = UNKNOWN) {}

/** @param resource $large_object */
function pg_lo_close($large_object): bool {}

/**
 * @param resource $large_object
 * @alias pg_lo_close
 * @deprecated
 */
function pg_loclose($large_object): bool {}

/** @param resource $large_object */
function pg_lo_read($large_object, int $len = 8192): string|false {}

/**
 * @param resource $large_object
 * @alias pg_lo_read
 * @deprecated
 */
function pg_loread($large_object, int $len = 8192): string|false {}

/** @param resource $large_object */
function pg_lo_write($large_object, string $buf, ?int $len = null): int|false {}

/**
 * @param resource $large_object
 * @alias pg_lo_write
 * @deprecated
 */
function pg_lowrite($large_object, string $buf, ?int $len = null): int|false {}

/** @param resource $large_object */
function pg_lo_read_all($large_object): int {}

/**
 * @param resource $large_object
 * @alias pg_lo_read_all
 * @deprecated
 */
function pg_loreadall($large_object): int {}

/**
 * @param resource|string $pgsql
 * @param string|int $filename
 * @param string|int $object_id
 * @return resource|false
 */
function pg_lo_import($pgsql, $filename = UNKNOWN, $object_id = UNKNOWN): string|int|false {}

/**
 * @param resource|string $pgsql
 * @param string|int $filename
 * @param string|int $object_id
 * @return resource|false
 * @alias pg_lo_import
 * @deprecated
 */
function pg_loimport($pgsql, $filename = UNKNOWN, $object_id = UNKNOWN): string|int|false {}

/**
 * @param resource|string|int $pgsql
 * @param string|int $object_id
 * @param string|int $filename
 * @return resource|false
 */
function pg_lo_export($pgsql, $object_id = UNKNOWN, $filename = UNKNOWN): bool {}

/**
 * @param resource|string|int $pgsql
 * @param string|int $object_id
 * @param string|int $filename
 * @return resource|false
 * @alias pg_lo_export
 * @deprecated
 */
function pg_loexport($pgsql, $object_id = UNKNOWN, $filename = UNKNOWN): bool {}

/** @param resource $large_object */
function pg_lo_seek($large_object, int $offset, int $start = SEEK_CUR): bool {}

/** @param resource $large_object */
function pg_lo_tell($large_object): int {}

/** @param resource $large_object */
function pg_lo_truncate($large_object, int $size): bool {}

/** @param resource|int $pgsql */
function pg_set_error_verbosity($pgsql, int $verbosity = UNKNOWN): int|false {}

/** @param resource|string $pgsql */
function pg_set_client_encoding($pgsql, string $encoding = UNKNOWN): int {}

/**
 * @param resource|string $pgsql
 * @alias pg_set_client_encoding
 * @deprecated
 */
function pg_setclientencoding($pgsql, string $encoding = UNKNOWN): int {}

/** @param resource|null $pgsql */
function pg_client_encoding($pgsql = null): string {}

/**
 * @param resource|null $pgsql
 * @alias pg_client_encoding
 * @deprecated
 */
function pg_clientencoding($pgsql = null): string {}

/** @param resource|null $pgsql */
function pg_end_copy($pgsql = null): bool {}

/** @param resource|string $pgsql */
function pg_put_line($pgsql, string $query = UNKNOWN): bool {}

/** @param resource $pgsql */
function pg_copy_to($pgsql, string $table_name, string $delimiter = "\t", string $null_as = "\\\\N"): array|false {}

/** @param resource $pgsql */
function pg_copy_from($pgsql, string $table_name, array $rows, string $delimiter = "\t", string $null_as = "\\\\N"): bool {}

/** @param resource|string $pgsql */
function pg_escape_string($pgsql, string $string = UNKNOWN): string {}

/** @param resource|string $pgsql */
function pg_escape_bytea($pgsql, string $data = UNKNOWN): string {}

function pg_unescape_bytea(?string $data = null): string {}

/** @param resource|string $pgsql */
function pg_escape_literal($pgsql, string $string = UNKNOWN): string|false {}

/** @param resource|string $pgsql */
function pg_escape_identifier($pgsql, string $identifier = UNKNOWN): string|false {}

/** @param resource $result */
function pg_result_error($result): string|false {}

/** @param resource $result */
function pg_result_error_field($result, int $fieldcode): string|false|null {}

/** @param resource $pgsql */
function pg_connection_status($pgsql): int {}

/** @param resource $pgsql */
function pg_transaction_status($pgsql): int {}

/** @param resource $pgsql */
function pg_connection_reset($pgsql): bool {}

/** @param resource $pgsql */
function pg_cancel_query($pgsql): bool {}

/** @param resource $pgsql */
function pg_connection_busy($pgsql): bool {}

/** @param resource $pgsql */
function pg_send_query($pgsql, string $query): int|bool {}

/** @param resource $pgsql */
function pg_send_query_params($pgsql, string $query, array $params): int|bool {}

/** @param resource $pgsql */
function pg_send_prepare($pgsql, string $stmtname, string $query): int|bool {}

/** @param resource $pgsql */
function pg_send_execute($pgsql, string $query, array $params): int|bool {}

/**
 * @param resource $pgsql
 * @return resource|false
 */
function pg_get_result($pgsql) {}

/** @param resource $result */
function pg_result_status($result, int $type = PGSQL_STATUS_LONG): string|int {}

/** @param resource $result */
function pg_get_notify($result, int $type = PGSQL_ASSOC): array|false {}

/** @param resource $pgsql */
function pg_get_pid($pgsql): int {}

/**
 * @param resource $pgsql
 * @return resource|false
 */
function pg_socket($pgsql) {}

/** @param resource $pgsql */
function pg_consume_input($pgsql): bool {}

/** @param resource $pgsql */
function pg_flush($pgsql): int|bool {}

/** @param resource $pgsql */
function pg_meta_data($pgsql, string $table_name, bool $extended = false): array|false {}

/** @param resource $pgsql */
function pg_convert($pgsql, string $table_name, array $values, int $options = 0): array|false {}

/**
 * @param resource $pgsql
 * @return resource|string|bool
 */
function pg_insert($pgsql, string $table, array $values, int $flags = 0) {}

/** @param resource $pgsql */
function pg_update($pgsql, string $table, array $values, array $ids, int $flags = 0): string|bool {}

/** @param resource $pgsql */
function pg_delete($pgsql, string $table, array $ids, int $flags = 0): string|bool {}

/** @param resource $pgsql */
function pg_select($pgsql, string $table, array $ids, int $flags = 0, int $result_type = PGSQL_ASSOC): array|string|false {}
