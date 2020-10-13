<?php

/** @generate-function-entries */

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

/** @param resource $statement */
function odbc_result_all($statement, string $format = ""): int|false {}

/** @param resource $statement */
function odbc_free_result($statement): bool {}

/** @return resource|false */
function odbc_connect(string $dsn, string $user, string $password, int $cursor_option = SQL_CUR_USE_DRIVER) {}

/** @return resource|false */
function odbc_pconnect(string $dsn, string $user, string $password, int $cursor_option = SQL_CUR_USE_DRIVER) {}

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
