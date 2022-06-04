<?php

/** @generate-class-entries */

final class mysqli_driver
{
    /** @readonly */
    public string $client_info;

    /** @readonly */
    public int $client_version;

    /** @readonly */
    public int $driver_version;

    public int $report_mode = 0;
}

class mysqli
{
    /**
     * @readonly
     * @link mysqli.affected-rows
     */
    public int|string $affected_rows;

    /**
     * @readonly
     * @link mysqli.get-client-info
     */
    public string $client_info;

    /**
     * @readonly
     * @link mysqli.get-client-version
     */
    public int $client_version;

    /**
     * @readonly
     * @link mysqli.connect-errno
     */
    public int $connect_errno;

    /**
     * @readonly
     * @link mysqli.connect-error
     */
    public ?string $connect_error;

    /**
     * @readonly
     * @link mysqli.errno
     */
    public int $errno;

    /**
     * @readonly
     * @link mysqli.error
     */
    public string $error;

    /**
     * @readonly
     * @link mysqli.error-list
     */
    public array $error_list;

    /**
     * @readonly
     * @link mysqli.field-count
     */
    public int $field_count;

    /**
     * @readonly
     * @link mysqli.get-host-info
     */
    public string $host_info;

    /**
     * @readonly
     * @link mysqli.info
     */
    public ?string $info;

    /**
     * @readonly
     * @link mysqli.insert-id
     */
    public int|string $insert_id;

    /**
     * @readonly
     * @link mysqli.get-server-info
     */
    public string $server_info;

    /**
     * @readonly
     * @link mysqli.get-server-version
     */
    public int $server_version;

    /**
     * @readonly
     * @link mysqli.sqlstate
     */
    public string $sqlstate;

    /**
     * @readonly
     * @link mysqli.get-proto-info
     */
    public int $protocol_version;

    /**
     * @readonly
     * @link mysqli.thread-id
     */
    public int $thread_id;

    /**
     * @readonly
     * @link mysqli.warning-count
     */
    public int $warning_count;

    /** @sensitive-param $password */
    public function __construct(
        ?string $hostname = null,
        ?string $username = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null
    ) {}

    /**
     * @tentative-return-type
     * @alias mysqli_autocommit
     */
    public function autocommit(bool $enable): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_begin_transaction
     */
    public function begin_transaction(int $flags = 0, ?string $name = null): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_change_user
     */
    public function change_user(string $username, string $password, ?string $database): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_character_set_name
     */
    public function character_set_name(): string {}

    /**
     * @return true
     * @alias mysqli_close
     * @no-verify
     */
    public function close() {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias mysqli_commit
     */
    public function commit(int $flags = 0, ?string $name = null): bool {}

    /**
     * @tentative-return-type
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
    ): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_dump_debug_info
     */
    public function dump_debug_info(): bool {}

    /**
     * @return true
     * @alias mysqli_debug
     * @no-verify Should really be a static method
     */
    public function debug(string $options) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias mysqli_get_charset
     */
    public function get_charset(): ?object {}

    /**
     * @tentative-return-type
     * @alias mysqli_get_client_info
     * @deprecated 8.1.0
     */
    public function get_client_info(): string {}

    /**
     * @return array<string, mixed>
     * @tentative-return-type
     * @alias mysqli_get_connection_stats
     */
    public function get_connection_stats(): array {}

    /**
     * @tentative-return-type
     * @alias mysqli_get_server_info
     */
    public function get_server_info(): string {}

    /**
     * @tentative-return-type
     * @alias mysqli_get_warnings
     */
    public function get_warnings(): mysqli_warning|false {}

    /**
     * @deprecated
     * @return bool|null
     * */
    public function init() {}

    /**
     * @tentative-return-type
     * @alias mysqli_kill
     */
    public function kill(int $process_id): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_multi_query
     */
    public function multi_query(string $query): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_more_results
     */
    public function more_results(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_next_result
     */
    public function next_result(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_ping
     */
    public function ping(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_poll
     */
    public static function poll(?array &$read, ?array &$error, array &$reject, int $seconds, int $microseconds = 0): int|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_prepare
     */
    public function prepare(string $query): mysqli_stmt|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_query
     */
    public function query(string $query, int $result_mode = MYSQLI_STORE_RESULT): mysqli_result|bool {}

    /**
     * @tentative-return-type
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
    ): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_real_escape_string
     */
    public function real_escape_string(string $string): string {}

    /**
     * @tentative-return-type
     * @alias mysqli_reap_async_query
     */
    public function reap_async_query(): mysqli_result|bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_real_escape_string
     */
    public function escape_string(string $string): string {}

    /**
     * @tentative-return-type
     * @alias mysqli_real_query
     */
    public function real_query(string $query): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_release_savepoint
     */
    public function release_savepoint(string $name): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_rollback
     */
    public function rollback(int $flags = 0, ?string $name = null): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_savepoint
     */
    public function savepoint(string $name): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_select_db
     */
    public function select_db(string $database): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_set_charset
     */
    public function set_charset(string $charset): bool {}

    /**
     * @param string|int $value
     * @tentative-return-type
     * @alias mysqli_options
     */
    public function options(int $option, $value): bool {}

    /**
     * @param string|int $value
     * @tentative-return-type
     * @alias mysqli_options
     */
    public function set_opt(int $option, $value): bool {}

    /**
     * @return true
     * @alias mysqli_ssl_set
     * @no-verify
     */
    public function ssl_set(
        ?string $key,
        ?string $certificate,
        ?string $ca_certificate,
        ?string $ca_path,
        ?string $cipher_algos
    ) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias mysqli_stat
     */
    public function stat(): string|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_init
     */
    public function stmt_init(): mysqli_stmt|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_store_result
     */
    public function store_result(int $mode = 0): mysqli_result|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_thread_safe
     */
    public function thread_safe(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_use_result
     */
    public function use_result(): mysqli_result|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_refresh
     */
    public function refresh(int $flags): bool {}
}

class mysqli_result implements IteratorAggregate
{
    /**
     * @readonly
     * @link mysqli-result.current-field
     */
    public int $current_field;

    /**
     * @readonly
     * @link mysqli-result.field-count
     */
    public int $field_count;

    /**
     * @readonly
     * @link mysqli-result.lengths
     */
    public ?array $lengths;

    /**
     * @readonly
     * @link mysqli-result.num-rows
     */
    public int|string $num_rows;

    public int $type;

    public function __construct(mysqli $mysql, int $result_mode = MYSQLI_STORE_RESULT) {}

    /**
     * @tentative-return-type
     * @alias mysqli_free_result
     */
    public function close(): void {}

    /**
     * @tentative-return-type
     * @alias mysqli_free_result
     */
    public function free(): void {}

    /**
     * @tentative-return-type
     * @alias mysqli_data_seek
     */
    public function data_seek(int $offset): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_fetch_field
     */
    public function fetch_field(): object|false {}

    /**
     * @return array<int, object>
     * @tentative-return-type
     * @alias mysqli_fetch_fields
     */
    public function fetch_fields(): array {}

    /**
     * @tentative-return-type
     * @alias mysqli_fetch_field_direct
     */
    public function fetch_field_direct(int $index): object|false {}

    /**
     * @return array<int|string, mixed>
     * @tentative-return-type
     * @alias mysqli_fetch_all
     */
    public function fetch_all(int $mode = MYSQLI_NUM): array {}

    /**
     * @return array<int|string, mixed>|null|false
     * @tentative-return-type
     * @alias mysqli_fetch_array
     */
    public function fetch_array(int $mode = MYSQLI_BOTH): array|null|false {}

    /**
     * @return array<int|string, mixed>|null|false
     * @tentative-return-type
     * @alias mysqli_fetch_assoc
     */
    public function fetch_assoc(): array|null|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_fetch_object
     */
    public function fetch_object(string $class = "stdClass", array $constructor_args = []): object|null|false {}

    /**
     * @return array<int, mixed>|null|false
     * @tentative-return-type
     * @alias mysqli_fetch_row
     */
    public function fetch_row(): array|null|false {}

    /** @alias mysqli_fetch_column */
    public function fetch_column(int $column = 0): null|int|float|string|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_field_seek
     */
    public function field_seek(int $index): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_free_result
     */
    public function free_result(): void {}

    public function getIterator(): Iterator {}
}

class mysqli_stmt
{
    /**
     * @readonly
     * @link mysqli-stmt.affected-rows
     */
    public int|string $affected_rows;

    /**
     * @readonly
     * @link mysqli-stmt.insert-id
     */
    public int|string $insert_id;

    /**
     * @readonly
     * @link mysqli-stmt.num-rows
     */
    public int|string $num_rows;

    /**
     * @readonly
     * @link mysqli-stmt.param-count
     */
    public int $param_count;

    /**
     * @readonly
     * @link mysqli-stmt.field-count
     */
    public int $field_count;

    /**
     * @readonly
     * @link mysqli-stmt.errno
     */
    public int $errno;

    /**
     * @readonly
     * @link mysqli-stmt.error
     */
    public string $error;

    /**
     * @readonly
     * @link mysqli-stmt.error-list
     */
    public array $error_list;

    /**
     * @readonly
     * @link mysqli-stmt.sqlstate
     */
    public string $sqlstate;

    public int $id;

    public function __construct(mysqli $mysql, ?string $query = null) {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_attr_get
     */
    public function attr_get(int $attribute): int {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_attr_set
     */
    public function attr_set(int $attribute, int $value): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_bind_param
     */
    public function bind_param(string $types, mixed &...$vars): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_bind_result
     */
    public function bind_result(mixed &...$vars): bool {}

    /**
     * @return true
     * @alias mysqli_stmt_close
     * @no-verify
     */
    public function close() {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_data_seek
     */
    public function data_seek(int $offset): void {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_execute
     */
    public function execute(?array $params = null): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_fetch
     */
    public function fetch(): ?bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_get_warnings
     */
    public function get_warnings(): mysqli_warning|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_result_metadata
     */
    public function result_metadata(): mysqli_result|false {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_more_results
     */
    public function more_results(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_next_result
     */
    public function next_result(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_num_rows
     */
    public function num_rows(): int|string {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_send_long_data
     */
    public function send_long_data(int $param_num, string $data): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_free_result
     */
    public function free_result(): void {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_reset
     */
    public function reset(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_prepare
     */
    public function prepare(string $query): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_store_result
     */
    public function store_result(): bool {}

    /**
     * @tentative-return-type
     * @alias mysqli_stmt_get_result
     */
    public function get_result(): mysqli_result|false {}
}

final class mysqli_warning
{
    public string $message;

    public string $sqlstate;

    public int $errno;

    private function __construct() {}

    public function next(): bool {}
}

final class mysqli_sql_exception extends RuntimeException
{
    protected string $sqlstate = "00000";

    public function getSqlState(): string {}
}

/** @refcount 1 */
function mysqli_affected_rows(mysqli $mysql): int|string {}

function mysqli_autocommit(mysqli $mysql, bool $enable): bool {}

function mysqli_begin_transaction(mysqli $mysql, int $flags = 0, ?string $name = null): bool {}

function mysqli_change_user(mysqli $mysql, string $username, string $password, ?string $database): bool {}

/** @refcount 1 */
function mysqli_character_set_name(mysqli $mysql): string {}

/** @return true */
function mysqli_close(mysqli $mysql): bool {} // TODO make return type void

function mysqli_commit(mysqli $mysql, int $flags = 0, ?string $name = null): bool {}

/** @refcount 1 */
function mysqli_connect(
    ?string $hostname = null,
    ?string $username = null,
    ?string $password = null,
    ?string $database = null,
    ?int $port = null,
    ?string $socket = null
): mysqli|false {}

function mysqli_connect_errno(): int {}

/** @refcount 1 */
function mysqli_connect_error(): ?string {}

function mysqli_data_seek(mysqli_result $result, int $offset): bool {}

function mysqli_dump_debug_info(mysqli $mysql): bool {}

/** @return true */
function mysqli_debug(string $options): bool {} // TODO make return type void

function mysqli_errno(mysqli $mysql): int {}

/** @refcount 1 */
function mysqli_error(mysqli $mysql): string {}

/**
 * @return array<int, array>
 * @refcount 1
 */
function mysqli_error_list(mysqli $mysql): array {}

function mysqli_stmt_execute(mysqli_stmt $statement, ?array $params = null): bool {}

/** @alias mysqli_stmt_execute */
function mysqli_execute(mysqli_stmt $statement, ?array $params = null): bool {}

/** @refcount 1 */
function mysqli_fetch_field(mysqli_result $result): object|false {}

/**
 * @return array<int, object>
 * @refcount 1
 */
function mysqli_fetch_fields(mysqli_result $result): array {}

/** @refcount 1 */
function mysqli_fetch_field_direct(mysqli_result $result, int $index): object|false {}

/**
 * @return array<int, int>|false
 * @refcount 1
 */
function mysqli_fetch_lengths(mysqli_result $result): array|false {}

/**
 * @return array<int|string, mixed>
 * @refcount 1
 */
function mysqli_fetch_all(mysqli_result $result, int $mode = MYSQLI_NUM): array {}

/**
 * @return array<int|string, mixed>|null|false
 * @refcount 1
 */
function mysqli_fetch_array(mysqli_result $result, int $mode = MYSQLI_BOTH): array|null|false {}

/**
 * @return array<int|string, mixed>|null|false
 * @refcount 1
 */
function mysqli_fetch_assoc(mysqli_result $result): array|null|false {}

/** @refcount 1 */
function mysqli_fetch_object(mysqli_result $result, string $class = "stdClass", array $constructor_args = []): object|null|false {}

/**
 * @return array<int, mixed>|null|false
 * @refcount 1
 */
function mysqli_fetch_row(mysqli_result $result): array|null|false {}

function mysqli_fetch_column(mysqli_result $result, int $column = 0): null|int|float|string|false {}

function mysqli_field_count(mysqli $mysql): int {}

function mysqli_field_seek(mysqli_result $result, int $index): bool {}

function mysqli_field_tell(mysqli_result $result): int {}

function mysqli_free_result(mysqli_result $result): void {}

/**
 * @return array<string, mixed>
 * @refcount 1
 */
function mysqli_get_connection_stats(mysqli $mysql): array {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function mysqli_get_client_stats(): array {}

/** @refcount 1 */
function mysqli_get_charset(mysqli $mysql): ?object {}

/** @refcount 1 */
function mysqli_get_client_info(?mysqli $mysql = null): string {}

function mysqli_get_client_version(): int {}

/**
 * @return array<string, int>
 * @refcount 1
 */
function mysqli_get_links_stats(): array {}

/** @refcount 1 */
function mysqli_get_host_info(mysqli $mysql): string {}

function mysqli_get_proto_info(mysqli $mysql): int {}

/** @refcount 1 */
function mysqli_get_server_info(mysqli $mysql): string {}

function mysqli_get_server_version(mysqli $mysql): int {}

/** @refcount 1 */
function mysqli_get_warnings(mysqli $mysql): mysqli_warning|false {}

/** @refcount 1 */
function mysqli_init(): mysqli|false {}

/** @refcount 1 */
function mysqli_info(mysqli $mysql): ?string {}

/** @refcount 1 */
function mysqli_insert_id(mysqli $mysql): int|string {}

function mysqli_kill(mysqli $mysql, int $process_id): bool {}

function mysqli_more_results(mysqli $mysql): bool {}

function mysqli_multi_query(mysqli $mysql, string $query): bool {}

function mysqli_next_result(mysqli $mysql): bool {}

function mysqli_num_fields(mysqli_result $result): int {}

/** @refcount 1 */
function mysqli_num_rows(mysqli_result $result): int|string {}

/** @param string|int $value */
function mysqli_options(mysqli $mysql, int $option, $value): bool {}

/**
 * @param string|int $value
 * @alias mysqli_options
 */
function mysqli_set_opt(mysqli $mysql, int $option, $value): bool {}

function mysqli_ping(mysqli $mysql): bool {}

function mysqli_poll(?array &$read, ?array &$error, array &$reject, int $seconds, int $microseconds = 0): int|false {}

/** @refcount 1 */
function mysqli_prepare(mysqli $mysql, string $query): mysqli_stmt|false {}

function mysqli_report(int $flags): bool {}

/** @refcount 1 */
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

/** @refcount 1 */
function mysqli_real_escape_string(mysqli $mysql, string $string): string {}

/** @alias mysqli_real_escape_string */
function mysqli_escape_string(mysqli $mysql, string $string): string {}

function mysqli_real_query(mysqli $mysql, string $query): bool {}

/** @refcount 1 */
function mysqli_reap_async_query(mysqli $mysql): mysqli_result|bool {}

function mysqli_release_savepoint(mysqli $mysql, string $name): bool {}

function mysqli_rollback(mysqli $mysql, int $flags = 0, ?string $name = null): bool {}

function mysqli_savepoint(mysqli $mysql, string $name): bool {}

function mysqli_select_db(mysqli $mysql, string $database): bool {}

function mysqli_set_charset(mysqli $mysql, string $charset): bool {}

/** @refcount 1 */
function mysqli_stmt_affected_rows(mysqli_stmt $statement): int|string {}

function mysqli_stmt_attr_get(mysqli_stmt $statement, int $attribute): int {}

function mysqli_stmt_attr_set(mysqli_stmt $statement, int $attribute, int $value): bool {}

function mysqli_stmt_bind_param(mysqli_stmt $statement, string $types, mixed &...$vars): bool {}

function mysqli_stmt_bind_result(mysqli_stmt $statement, mixed &...$vars): bool {}

/** @return true */
function mysqli_stmt_close(mysqli_stmt $statement): bool {}

function mysqli_stmt_data_seek(mysqli_stmt $statement, int $offset): void {}

function mysqli_stmt_errno(mysqli_stmt $statement): int {}

/** @refcount 1 */
function mysqli_stmt_error(mysqli_stmt $statement): string {}

/**
 * @return array<int, array>
 * @refcount 1
 */
function mysqli_stmt_error_list(mysqli_stmt $statement): array {}

function mysqli_stmt_fetch(mysqli_stmt $statement): ?bool {}

function mysqli_stmt_field_count(mysqli_stmt $statement): int {}

function mysqli_stmt_free_result(mysqli_stmt $statement): void {}

/** @refcount 1 */
function mysqli_stmt_get_result(mysqli_stmt $statement): mysqli_result|false {}

/** @refcount 1 */
function mysqli_stmt_get_warnings(mysqli_stmt $statement): mysqli_warning|false {}

/** @refcount 1 */
function mysqli_stmt_init(mysqli $mysql): mysqli_stmt|false {}

/** @refcount 1 */
function mysqli_stmt_insert_id(mysqli_stmt $statement): int|string {}

function mysqli_stmt_more_results(mysqli_stmt $statement): bool {}

function mysqli_stmt_next_result(mysqli_stmt $statement): bool {}

/** @refcount 1 */
function mysqli_stmt_num_rows(mysqli_stmt $statement): int|string {}

function mysqli_stmt_param_count(mysqli_stmt $statement): int {}

function mysqli_stmt_prepare(mysqli_stmt $statement, string $query): bool {}

function mysqli_stmt_reset(mysqli_stmt $statement): bool {}

/** @refcount 1 */
function mysqli_stmt_result_metadata(mysqli_stmt $statement): mysqli_result|false {}

function mysqli_stmt_send_long_data(mysqli_stmt $statement, int $param_num, string $data): bool {}

function mysqli_stmt_store_result(mysqli_stmt $statement): bool {}

/** @refcount 1 */
function mysqli_stmt_sqlstate(mysqli_stmt $statement): string {}

/** @refcount 1 */
function mysqli_sqlstate(mysqli $mysql): string {}

/** @return true */
function mysqli_ssl_set(
    mysqli $mysql,
    ?string $key,
    ?string $certificate,
    ?string $ca_certificate,
    ?string $ca_path,
    ?string $cipher_algos
): bool {} // TODO make return type void

/** @refcount 1 */
function mysqli_stat(mysqli $mysql): string|false {}

/** @refcount 1 */
function mysqli_store_result(mysqli $mysql, int $mode = 0): mysqli_result|false {}

function mysqli_thread_id(mysqli $mysql): int {}

function mysqli_thread_safe(): bool {}

/** @refcount 1 */
function mysqli_use_result(mysqli $mysql): mysqli_result|false {}

function mysqli_warning_count(mysqli $mysql): int {}

function mysqli_refresh(mysqli $mysql, int $flags): bool {}
