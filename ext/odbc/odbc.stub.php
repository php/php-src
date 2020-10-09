<?php

/** @generate-function-entries */

function odbc_close_all(): void {}

/** @param resource $result_id */
function odbc_binmode($result_id, int $mode): bool {}

/** @param resource $result_id */
function odbc_longreadlen($result_id, int $length): bool {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_prepare($connection_id, string $query) {}

/** @param resource $result_id */
function odbc_execute($result_id, array $parameters_array = []): bool {}

/** @param resource $result_id */
function odbc_cursor($result_id): string|false {}

#ifdef HAVE_SQLDATASOURCES
/** @param resource $connection_id */
function odbc_data_source($connection_id, int $fetch_type): array|false {}
#endif

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_exec($connection_id, string $query) {}

/**
 * @param resource $connection_id
 * @return resource|false
 * @alias odbc_exec
 */
function odbc_do($connection_id, string $query) {}

#ifdef PHP_ODBC_HAVE_FETCH_HASH
/** @param resource $result */
function odbc_fetch_object($result, int $rownumber = -1): stdClass|false {}

/** @param resource $result */
function odbc_fetch_array($result, int $rownumber = -1): array|false {}
#endif

/**
 * @param resource $result_id
 * @param array $result_array
 */
function odbc_fetch_into($result_id, &$result_array, int $rownumber = 0): int|false {}

/** @param resource $result_id */
function odbc_fetch_row($result_id, ?int $row_number = null): bool {}

/** @param resource $result_id */
function odbc_result($result_id, string|int $field): string|bool|null {}

/** @param resource $result_id */
function odbc_result_all($result_id, string $format = ""): int|false {}

/** @param resource $result_id */
function odbc_free_result($result_id): bool {}

/** @return resource|false */
function odbc_connect(string $dsn, string $user, string $password, int $cursor_option = SQL_CUR_USE_DRIVER) {}

/** @return resource|false */
function odbc_pconnect(string $dsn, string $user, string $password, int $cursor_option = SQL_CUR_USE_DRIVER) {}

/** @param resource $connection_id */
function odbc_close($connection_id): void {}

/** @param resource $result_id */
function odbc_num_rows($result_id): int {}

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
/** @param resource $result_id */
function odbc_next_result($result_id): bool {}
#endif

/** @param resource $result_id */
function odbc_num_fields($result_id): int {}

/** @param resource $result_id */
function odbc_field_name($result_id, int $field_number): string|false {}

/** @param resource $result_id */
function odbc_field_type($result_id, int $field_number): string|false {}

/** @param resource $result_id */
function odbc_field_len($result_id, int $field_number): int|false {}

/**
 * @param resource $result_id
 * @alias odbc_field_len
 */
function odbc_field_precision($result_id, int $field_number): int|false {}

/** @param resource $result_id */
function odbc_field_scale($result_id, int $field_number): int|false {}

/** @param resource $result_id */
function odbc_field_num($result_id, string $field_name): int|false {}

/** @param resource $connection_id */
function odbc_autocommit($connection_id, int $onoff = 0): int|bool {}

/** @param resource $connection_id */
function odbc_commit($connection_id): bool {}

/** @param resource $connection_id */
function odbc_rollback($connection_id): bool {}

/** @param resource|null $connection_id */
function odbc_error($connection_id = null): string {}

/** @param resource|null $connection_id */
function odbc_errormsg($connection_id = null): string {}

/** @param resource $conn_id */
function odbc_setoption($conn_id, int $which, int $option, int $value): bool {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_tables($connection_id, ?string $catalog = null, ?string $schema = null, ?string $name = null, ?string $table_types = null) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_columns($connection_id, ?string $catalog = null, ?string $schema = null, ?string $table_name = null, ?string $column_name = null) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_gettypeinfo($connection_id, int $data_type = 0) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_primarykeys($connection_id, ?string $catalog, string $schema, string $table) {}

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_procedurecolumns($connection_id, ?string $catalog = null, ?string $schema = null, ?string $proc = null, ?string $column = null) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_procedures($connection_id, ?string $catalog = null, ?string $schema = null, ?string $name = null) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_foreignkeys($connection_id, ?string $pk_catalog, string $pk_schema, string $pk_table, string $fk_catalog, string $fk_schema, string $fk_table) {}
#endif

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_specialcolumns($connection_id, int $type, ?string $catalog, string $schema, string $table, int $scope, int $nullable) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_statistics($connection_id, ?string $catalog, string $schema, string $name, int $unique, int $accuracy) {}

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_tableprivileges($connection_id, ?string $catalog, string $schema, string $name) {}

/**
 * @param resource $connection_id
 * @return resource|false
 */
function odbc_columnprivileges($connection_id, ?string $catalog, string $schema, string $table, string $column) {}
#endif
