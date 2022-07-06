<?php

/** @generate-function-entries */

final class mysqli_driver
{
}

class mysqli
{
    public function __construct(
        ?string $hostname = null,
        ?string $username = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null
    ) {}

    /**
     * @return bool
     * @alias mysqli_autocommit
     */
    public function autocommit(bool $enable) {}

    /**
     * @return bool
     * @alias mysqli_begin_transaction
     */
    public function begin_transaction(int $flags = 0, ?string $name = null) {}

    /**
     * @return bool
     * @alias mysqli_change_user
     */
    public function change_user(string $username, string $password, ?string $database) {}

    /**
     * @return string
     * @alias mysqli_character_set_name
     */
    public function character_set_name() {}

    /**
     * @return bool
     * @alias mysqli_close
     */
    public function close() {}

    /**
     * @return bool
     * @alias mysqli_commit
     */
    public function commit(int $flags = 0, ?string $name = null) {}

    /**
     * @return bool|null
     * @alias mysqli_connect
     * @no-verify
     */
    public function connect(
        ?string $hostname = null,
        ?string $username = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null
    ) {}

    /**
     * @return bool
     * @alias mysqli_dump_debug_info
     */
    public function dump_debug_info() {}

    /**
     * @return bool
     * @alias mysqli_debug
     * @no-verify Should really be a static method
     */
    public function debug(string $options) {}

    /**
     * @return object|null
     * @alias mysqli_get_charset
     */
    public function get_charset() {}

    /**
     * @return string
     * @alias mysqli_get_client_info
     */
    public function get_client_info() {}

#if defined(MYSQLI_USE_MYSQLND)
    /**
     * @return array
     * @alias mysqli_get_connection_stats
     */
    public function get_connection_stats() {}
#endif

    /**
     * @return string
     * @alias mysqli_get_server_info
     */
    public function get_server_info() {}

    /**
     * @return mysqli_warning|false
     * @alias mysqli_get_warnings
     */
    public function get_warnings() {}

    /**
     * @return null|false
     */
    public function init() {}

    /**
     * @return bool
     * @alias mysqli_kill
     */
    public function kill(int $process_id) {}

    /**
     * @return bool
     * @alias mysqli_multi_query
     */
    public function multi_query(string $query) {}

    /**
     * @return bool
     * @alias mysqli_more_results
     */
    public function more_results() {}

    /**
     * @return bool
     * @alias mysqli_next_result
     */
    public function next_result() {}

    /**
     * @return bool
     * @alias mysqli_ping
     */
    public function ping() {}

#if defined(MYSQLI_USE_MYSQLND)
    /**
     * @return int|false
     * @alias mysqli_poll
     */
    public static function poll(?array &$read, ?array &$error, array &$reject, int $seconds, int $microseconds = 0) {}
#endif

    /**
     * @return mysqli_stmt|false
     * @alias mysqli_prepare
     */
    public function prepare(string $query) {}

    /**
     * @return mysqli_result|bool
     * @alias mysqli_query
     */
    public function query(string $query, int $result_mode = MYSQLI_STORE_RESULT) {}

    /**
     * @return bool
     * @alias mysqli_real_connect
     */
    public function real_connect(
        ?string $hostname = null,
        ?string $username = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null,
        int $flags = 0
    ) {}

    /**
     * @return string
     * @alias mysqli_real_escape_string
     */
    public function real_escape_string(string $string) {}

#if defined(MYSQLI_USE_MYSQLND)
    /**
     * @return mysqli_result|bool
     * @alias mysqli_reap_async_query
     */
    public function reap_async_query() {}
#endif

    /**
     * @return string
     * @alias mysqli_real_escape_string
     */
    public function escape_string(string $string) {}

    /**
     * @return bool
     * @alias mysqli_real_query
     */
    public function real_query(string $query) {}

    /**
     * @return bool
     * @alias mysqli_release_savepoint
     */
    public function release_savepoint(string $name) {}

    /**
     * @return bool
     * @alias mysqli_rollback
     */
    public function rollback(int $flags = 0, ?string $name = null) {}

    /**
     * @return bool
     * @alias mysqli_savepoint
     */
    public function savepoint(string $name) {}

    /**
     * @return bool
     * @alias mysqli_select_db
     */
    public function select_db(string $database) {}

    /**
     * @return bool
     * @alias mysqli_set_charset
     */
    public function set_charset(string $charset) {}

    /**
     * @param string|int $value
     * @return bool
     * @alias mysqli_options
     */
    public function options(int $option, $value) {}

    /**
     * @param string|int $value
     * @return bool
     * @alias mysqli_options
     */
    public function set_opt(int $option, $value) {}

    /**
     * @return bool
     * @alias mysqli_ssl_set
     */
    public function ssl_set(
        ?string $key,
        ?string $certificate,
        ?string $ca_certificate,
        ?string $ca_path,
        ?string $cipher_algos
    ) {}

    /**
     * @return string|false
     * @alias mysqli_stat
     */
    public function stat() {}

    /**
     * @return mysqli_stmt|false
     * @alias mysqli_stmt_init
     */
    public function stmt_init() {}

    /**
     * @return mysqli_result|false
     * @alias mysqli_store_result
     */
    public function store_result(int $mode = 0) {}

    /**
     * @return bool
     * @alias mysqli_thread_safe
     */
    public function thread_safe() {}

    /**
     * @return mysqli_result|false
     * @alias mysqli_use_result
     */
    public function use_result() {}

    /**
     * @return bool
     * @alias mysqli_refresh
     */
    public function refresh(int $flags) {}
}

class mysqli_result implements IteratorAggregate
{
    public function __construct(mysqli $mysql, int $result_mode = MYSQLI_STORE_RESULT) {}

    /**
     * @return void
     * @alias mysqli_free_result
     */
    public function close() {}

    /**
     * @return void
     * @alias mysqli_free_result
     */
    public function free() {}

    /**
     * @return bool
     * @alias mysqli_data_seek
     */
    public function data_seek(int $offset) {}

    /**
     * @return object|false
     * @alias mysqli_fetch_field
     */
    public function fetch_field() {}

    /**
     * @return array
     * @alias mysqli_fetch_fields
     */
    public function fetch_fields() {}

    /**
     * @return object|false
     * @alias mysqli_fetch_field_direct
     */
    public function fetch_field_direct(int $index) {}

#if defined(MYSQLI_USE_MYSQLND)
    /**
     * @return array
     * @alias mysqli_fetch_all
     */
    public function fetch_all(int $mode = MYSQLI_NUM) {}
#endif

    /**
     * @return array|null|false
     * @alias mysqli_fetch_array
     */
    public function fetch_array(int $mode = MYSQLI_BOTH) {}

    /**
     * @return array|null|false
     * @alias mysqli_fetch_assoc
     */
    public function fetch_assoc() {}

    /**
     * @return object|null|false
     * @alias mysqli_fetch_object
     */
    public function fetch_object(string $class = "stdClass", array $constructor_args = []) {}

    /**
     * @return array|null|false
     * @alias mysqli_fetch_row
     */
    public function fetch_row() {}

    /**
     * @return bool
     * @alias mysqli_field_seek
     */
    public function field_seek(int $index) {}

    /**
     * @return void
     * @alias mysqli_free_result
     */
    public function free_result() {}

    public function getIterator(): Iterator;
}

class mysqli_stmt
{
    public function __construct(mysqli $mysql, ?string $query = null) {}

    /**
     * @return int
     * @alias mysqli_stmt_attr_get
     */
    public function attr_get(int $attribute) {}

    /**
     * @return bool
     * @alias mysqli_stmt_attr_set
     */
    public function attr_set(int $attribute, int $value) {}

    /**
     * @return bool
     * @alias mysqli_stmt_bind_param
     */
    public function bind_param(string $types, mixed &...$vars) {}

    /**
     * @return bool
     * @alias mysqli_stmt_bind_result
     */
    public function bind_result(mixed &...$vars) {}

    /**
     * @return bool
     * @alias mysqli_stmt_close
     */
    public function close() {}

    /**
     * @return void
     * @alias mysqli_stmt_data_seek
     */
    public function data_seek(int $offset) {}

    /**
     * @return bool
     * @alias mysqli_stmt_execute
     */
    public function execute() {}

    /**
     * @return bool|null
     * @alias mysqli_stmt_fetch
     */
    public function fetch() {}

    /**
     * @return mysqli_warning|false
     * @alias mysqli_stmt_get_warnings
     */
    public function get_warnings() {}

    /**
     * @return mysqli_result|false
     * @alias mysqli_stmt_result_metadata
     */
    public function result_metadata() {}

#if defined(MYSQLI_USE_MYSQLND)
    /**
     * @return bool
     * @alias mysqli_stmt_more_results
     */
    public function more_results() {}

    /**
     * @return bool
     * @alias mysqli_stmt_next_result
     */
    public function next_result() {}
#endif

    /**
     * @return int|string
     * @alias mysqli_stmt_num_rows
     */
    public function num_rows() {}

    /**
     * @return bool
     * @alias mysqli_stmt_send_long_data
     */
    public function send_long_data(int $param_num, string $data) {}

    /**
     * @return void
     * @alias mysqli_stmt_free_result
     */
    public function free_result() {}

    /**
     * @return bool
     * @alias mysqli_stmt_reset
     */
    public function reset() {}

    /**
     * @return bool
     * @alias mysqli_stmt_prepare
     */
    public function prepare(string $query) {}

    /**
     * @return bool
     * @alias mysqli_stmt_store_result
     */
    public function store_result() {}

#if defined(MYSQLI_USE_MYSQLND)
    /**
     * @return mysqli_result|false
     * @alias mysqli_stmt_get_result
     */
    public function get_result() {}
#endif
}

final class mysqli_warning
{
    private function __construct() {}

    public function next(): bool {}
}

final class mysqli_sql_exception extends RuntimeException
{
}

function mysqli_affected_rows(mysqli $mysql): int|string {}

function mysqli_autocommit(mysqli $mysql, bool $enable): bool {}

function mysqli_begin_transaction(mysqli $mysql, int $flags = 0, ?string $name = null): bool {}

function mysqli_change_user(mysqli $mysql, string $username, string $password, ?string $database): bool {}

function mysqli_character_set_name(mysqli $mysql): string {}

function mysqli_close(mysqli $mysql): bool {}

function mysqli_commit(mysqli $mysql, int $flags = 0, ?string $name = null): bool {}

function mysqli_connect(
    ?string $hostname = null,
    ?string $username = null,
    ?string $password = null,
    ?string $database = null,
    ?int $port = null,
    ?string $socket = null
): mysqli|false {}

function mysqli_connect_errno(): int {}

function mysqli_connect_error(): ?string {}

function mysqli_data_seek(mysqli_result $result, int $offset): bool {}

function mysqli_dump_debug_info(mysqli $mysql): bool {}

function mysqli_debug(string $options): bool {}

function mysqli_errno(mysqli $mysql): int {}

function mysqli_error(mysqli $mysql): string {}

function mysqli_error_list(mysqli $mysql): array {}

function mysqli_stmt_execute(mysqli_stmt $statement): bool {}

/** @alias mysqli_stmt_execute */
function mysqli_execute(mysqli_stmt $statement): bool {}

function mysqli_fetch_field(mysqli_result $result): object|false {}

function mysqli_fetch_fields(mysqli_result $result): array {}

function mysqli_fetch_field_direct(mysqli_result $result, int $index): object|false {}

function mysqli_fetch_lengths(mysqli_result $result): array|false {}

#if defined(MYSQLI_USE_MYSQLND)
function mysqli_fetch_all(mysqli_result $result, int $mode = MYSQLI_NUM): array {}
#endif

function mysqli_fetch_array(mysqli_result $result, int $mode = MYSQLI_BOTH): array|null|false {}

function mysqli_fetch_assoc(mysqli_result $result): array|null|false {}

function mysqli_fetch_object(mysqli_result $result, string $class = "stdClass", array $constructor_args = []): object|null|false {}

function mysqli_fetch_row(mysqli_result $result): array|null|false {}

function mysqli_field_count(mysqli $mysql): int {}

function mysqli_field_seek(mysqli_result $result, int $index): bool {}

function mysqli_field_tell(mysqli_result $result): int {}

function mysqli_free_result(mysqli_result $result): void {}

#if defined(MYSQLI_USE_MYSQLND)
function mysqli_get_connection_stats(mysqli $mysql): array {}

function mysqli_get_client_stats(): array {}
#endif

function mysqli_get_charset(mysqli $mysql): ?object {}

function mysqli_get_client_info(?mysqli $mysql = null): string {}

function mysqli_get_client_version(): int {}

function mysqli_get_links_stats(): array {}

function mysqli_get_host_info(mysqli $mysql): string {}

function mysqli_get_proto_info(mysqli $mysql): int {}

function mysqli_get_server_info(mysqli $mysql): string {}

function mysqli_get_server_version(mysqli $mysql): int {}

function mysqli_get_warnings(mysqli $mysql): mysqli_warning|false {}

function mysqli_init(): mysqli|false {}

function mysqli_info(mysqli $mysql): ?string {}

function mysqli_insert_id(mysqli $mysql): int|string {}

function mysqli_kill(mysqli $mysql, int $process_id): bool {}

function mysqli_more_results(mysqli $mysql): bool {}

function mysqli_multi_query(mysqli $mysql, string $query): bool {}

function mysqli_next_result(mysqli $mysql): bool {}

function mysqli_num_fields(mysqli_result $result): int {}

function mysqli_num_rows(mysqli_result $result): int|string {}

/** @param string|int $value */
function mysqli_options(mysqli $mysql, int $option, $value): bool {}

/**
 * @param string|int $value
 * @alias mysqli_options
 */
function mysqli_set_opt(mysqli $mysql, int $option, $value): bool {}

function mysqli_ping(mysqli $mysql): bool {}

#if defined(MYSQLI_USE_MYSQLND)
function mysqli_poll(?array &$read, ?array &$error, array &$reject, int $seconds, int $microseconds = 0): int|false {}
#endif

function mysqli_prepare(mysqli $mysql, string $query): mysqli_stmt|false {}

function mysqli_report(int $flags): bool {}

function mysqli_query(mysqli $mysql, string $query, int $result_mode = MYSQLI_STORE_RESULT): mysqli_result|bool {}

function mysqli_real_connect(
    mysqli $mysql,
    ?string $hostname = null,
    ?string $username = null,
    ?string $password = null,
    ?string $database = null,
    ?int $port = null,
    ?string $socket = null,
    int $flags = 0
): bool {}

function mysqli_real_escape_string(mysqli $mysql, string $string): string {}

/** @alias mysqli_real_escape_string */
function mysqli_escape_string(mysqli $mysql, string $string): string {}

function mysqli_real_query(mysqli $mysql, string $query): bool {}

#if defined(MYSQLI_USE_MYSQLND)
function mysqli_reap_async_query(mysqli $mysql): mysqli_result|bool {}
#endif

function mysqli_release_savepoint(mysqli $mysql, string $name): bool {}

function mysqli_rollback(mysqli $mysql, int $flags = 0, ?string $name = null): bool {}

function mysqli_savepoint(mysqli $mysql, string $name): bool {}

function mysqli_select_db(mysqli $mysql, string $database): bool {}

function mysqli_set_charset(mysqli $mysql, string $charset): bool {}

function mysqli_stmt_affected_rows(mysqli_stmt $statement): int|string {}

function mysqli_stmt_attr_get(mysqli_stmt $statement, int $attribute): int {}

function mysqli_stmt_attr_set(mysqli_stmt $statement, int $attribute, int $value): bool {}

function mysqli_stmt_bind_param(mysqli_stmt $statement, string $types, mixed &...$vars): bool {}

function mysqli_stmt_bind_result(mysqli_stmt $statement, mixed &...$vars): bool {}

function mysqli_stmt_close(mysqli_stmt $statement): bool {}

function mysqli_stmt_data_seek(mysqli_stmt $statement, int $offset): void {}

function mysqli_stmt_errno(mysqli_stmt $statement): int {}

function mysqli_stmt_error(mysqli_stmt $statement): string {}

function mysqli_stmt_error_list(mysqli_stmt $statement): array {}

function mysqli_stmt_fetch(mysqli_stmt $statement): ?bool {}

function mysqli_stmt_field_count(mysqli_stmt $statement): int {}

function mysqli_stmt_free_result(mysqli_stmt $statement): void {}

#if defined(MYSQLI_USE_MYSQLND)
function mysqli_stmt_get_result(mysqli_stmt $statement): mysqli_result|false {}
#endif

function mysqli_stmt_get_warnings(mysqli_stmt $statement): mysqli_warning|false {}

function mysqli_stmt_init(mysqli $mysql): mysqli_stmt|false {}

function mysqli_stmt_insert_id(mysqli_stmt $statement): int|string {}

#if defined(MYSQLI_USE_MYSQLND)
function mysqli_stmt_more_results(mysqli_stmt $statement): bool {}

function mysqli_stmt_next_result(mysqli_stmt $statement): bool {}
#endif

function mysqli_stmt_num_rows(mysqli_stmt $statement): int|string {}

function mysqli_stmt_param_count(mysqli_stmt $statement): int {}

function mysqli_stmt_prepare(mysqli_stmt $statement, string $query): bool {}

function mysqli_stmt_reset(mysqli_stmt $statement): bool {}

function mysqli_stmt_result_metadata(mysqli_stmt $statement): mysqli_result|false {}

function mysqli_stmt_send_long_data(mysqli_stmt $statement, int $param_num, string $data): bool {}

function mysqli_stmt_store_result(mysqli_stmt $statement): bool {}

function mysqli_stmt_sqlstate(mysqli_stmt $statement): string {}

function mysqli_sqlstate(mysqli $mysql): string {}

function mysqli_ssl_set(
    mysqli $mysql,
    ?string $key,
    ?string $certificate,
    ?string $ca_certificate,
    ?string $ca_path,
    ?string $cipher_algos
): bool {}

function mysqli_stat(mysqli $mysql): string|false {}

function mysqli_store_result(mysqli $mysql, int $mode = 0): mysqli_result|false {}

function mysqli_thread_id(mysqli $mysql): int {}

function mysqli_thread_safe(): bool {}

function mysqli_use_result(mysqli $mysql): mysqli_result|false {}

function mysqli_warning_count(mysqli $mysql): int {}

function mysqli_refresh(mysqli $mysql, int $flags): bool {}
