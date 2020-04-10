<?php

class mysqli_driver
{
}

class mysqli
{
    public function __construct(
        ?string $host = null,
        ?string $user = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null
    );

    /** @return bool */
    public function autocommit(bool $mode);

    /** @return bool */
    public function begin_transaction(int $flags = 0, string $name = UNKNOWN);

    /** @return bool */
    public function change_user(string $user, string $password, ?string $database);

    /** @return string|null */
    public function character_set_name();

    /** @return bool */
    public function close();

    /** @return bool */
    public function commit(int $flags = -1, string $name = UNKNOWN);

    /** @return mysqli|null|false */
    public function connect(
        ?string $host = null,
        ?string $user = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null
    );

    /** @return bool */
    public function dump_debug_info();

    /** @return bool */
    public function debug(string $debug_options);

    /** @return object|null */
    public function get_charset();

    /** @return string|null */
    public function get_client_info();

    /** @return array */
    public function get_connection_stats();

    /** @return string|null */
    public function get_server_info();

    /** @return mysqli_warning|false */
    public function get_warnings();

    /** @return mysqli|false */
    public function init();

    /** @return bool */
    public function kill(int $connection_id);

    /** @return bool */
    public function multi_query(string $query);

    /** @return bool */
    public function more_results();

    /** @return bool */
    public function next_result();

    /** @return bool */
    public function ping();

    /** @return int|false */
    public static function poll(?array &$read, ?array &$write, array &$error, int $sec, int $usec = 0);

    /** @return mysqli_stmt|false */
    public function prepare(string $query);

    /** @return mysqli_result|bool */
    public function query(string $query, int $resultmode = MYSQLI_STORE_RESULT);

    /** @return bool */
    public function real_connect(
        ?string $host = null,
        ?string $user = null,
        ?string $password = null,
        ?string $database = null,
        ?int $port = null,
        ?string $socket = null,
        int $flags = 0
    );

    /** @return string */
    public function real_escape_string(string $string_to_escape);

    /** @return mysqli_result|bool */
    public function reap_async_query();

    /** @return string */
    public function escape_string(string $string_to_escape);

    /** @return bool */
    public function real_query(string $query);

    /** @return bool */
    public function release_savepoint(string $name);

    /** @return bool */
    public function rollback(int $flags = 0, string $name = '');

    /** @return bool */
    public function savepoint(string $name);

    /** @return bool */
    public function select_db(string $database);

    /** @return bool */
    public function set_charset(string $charset);

    /**
     * @param mixed $value
     *
     * @return bool
     */
    public function options(int $option, $value);

    /**
     * @param mixed $value
     *
     * @return bool
     */
    public function set_opt(int $option, $value);

    /** @return bool */
    public function ssl_set(
        string $key,
        string $cert,
        string $certificate_authority,
        string $certificate_authority_path,
        string $cipher
    );

    /** @return string|false */
    public function stat();

    /** @return mysqli_stmt|false */
    public function stmt_init();

    /** @return mysqli_result|false */
    public function store_result(int $flags = 0);

    /** @return bool */
    public function thread_safe();

    /** @return mysqli_result|false */
    public function use_result();

    /** @return bool */
    public function refresh(int $options);
}

class mysqli_result
{
     public function __construct(object $mysqli_link, int $resmode = MYSQLI_STORE_RESULT);

    /** @return void */
    public function close();

    /** @return void */
    public function free();

    /** @return bool */
    public function data_seek(int $offset);

    /** @return object|false */
    public function fetch_field();

    /** @return array */
    public function fetch_fields();

    /** @return object|false */
    public function fetch_field_direct(int $field_nr);

    /** @return array|false */
    public function fetch_all(int $result_type = MYSQLI_NUM);

    /** @return array|null|false */
    public function fetch_array(int $result_type = MYSQLI_BOTH);

    /** @return array|null */
    public function fetch_assoc();

    /** @return object|null */
    public function fetch_object(string $class_name = 'stdClass', array $params = []);

    /** @return array|null */
    public function fetch_row();

    /** @return bool */
    public function field_seek(int $field_nr);

    /** @return void */
    public function free_result();
}

class mysqli_stmt
{
    public function __construct(mysqli $mysqli_link, string $statement = UNKNOWN);

    /** @return int|false */
    public function attr_get(int $attr);

    /** @return bool */
    public function attr_set(int $attr, int $mode_in);

    /** @return bool */
    public function bind_param(string $types, &...$vars);

    /** @return bool */
    public function bind_result(&...$vars);

    /** @return bool */
    public function close();

    /** @return null|false */
    public function data_seek(int $offset);

    /** @return bool */
    public function execute();

    /** @return bool|null */
    public function fetch();

    /** @return mysqli_warning|false */
    public function get_warnings();

    /** @return mysqli_result|false */
    public function result_metadata();

    /** @return bool */
    public function more_results();

    /** @return bool */
    public function next_result();

    /** @return int|string */
    public function num_rows();

    /** @return bool */
    public function send_long_data(int $param_nr, string $data);

    /** @return void */
    public function free_result();

    /** @return bool */
    public function reset();

    /** @return bool */
    public function prepare(string $query);

    /** @return bool */
    public function store_result();

    /** @return mysqli_result|false */
    public function get_result();
}

class mysqli_sql_exception extends RuntimeException
{}

function mysqli_affected_rows(mysqli $mysql_link): int|string {}

function mysqli_autocommit(mysqli $mysql_link, bool $mode): bool {}

function mysqli_begin_transaction(mysqli $mysql_link, int $flags = 0, string $name = UNKNOWN): bool {}

function mysqli_change_user(mysqli $mysql_link, string $user, string $password, ?string $database): bool {}

function mysqli_character_set_name(mysqli $mysql_link): ?string {}

function mysqli_close(mysqli $mysql_link): bool {}

function mysqli_commit(mysqli $mysql_link, int $flags = -1, string $name = UNKNOWN): bool {}

function mysqli_connect(
    ?string $host = null,
    ?string $user = null,
    ?string $password = null,
    ?string $database = null,
    ?int $port = null,
    ?string $socket = null
): mysqli|null|false {}

function mysqli_connect_errno(): int {}

function mysqli_connect_error(): ?string {}

function mysqli_data_seek(mysqli_result $mysql_result, int $offset): bool {}

function mysqli_dump_debug_info(mysqli $mysql_link): bool {}

function mysqli_debug(string $debug): bool {}

function mysqli_errno(mysqli $mysql_link): int {}

function mysqli_error(mysqli $mysql_link): ?string {}

function mysqli_error_list(mysqli $mysql_link): array {}

function mysqli_stmt_execute(mysqli_stmt $mysql_stmt): bool {}

/** @alias mysqli_stmt_execute */
function mysqli_execute(mysqli_stmt $mysql_stmt): bool {}

function mysqli_fetch_field(mysqli_result $mysql_result): object|false {}

function mysqli_fetch_fields(mysqli_result $mysql_result): array {}

function mysqli_fetch_field_direct(mysqli_result $mysql_result, int $offset): object|false {}

function mysqli_fetch_lengths(mysqli_result $mysql_result): array|false {}

function mysqli_fetch_all(mysqli_result $mysql_result, int $mode = MYSQLI_NUM): array|false {}

function mysqli_fetch_array(mysqli_result $mysql_result, int $fetchtype = MYSQLI_BOTH): array|null|false {}

function mysqli_fetch_assoc(mysqli_result $mysql_result): ?array {}

function mysqli_fetch_object(
    mysqli_result $mysqli_result,
    string $class_name = 'stdClass',
    array $params = []
): ?object {}

function mysqli_fetch_row(mysqli_result $mysqli_result): ?array {}

function mysqli_field_count(mysqli $mysqli_link): int {}

function mysqli_field_seek(mysqli_result $mysqli_result, int $field_nr): bool {}

function mysqli_field_tell(mysqli_result $mysqli_result): int {}

function mysqli_free_result(mysqli_result $mysqli_result): void {}

function mysqli_get_connection_stats(mysqli $mysqli_link): array {}

function mysqli_get_client_stats(): array {}

function mysqli_get_charset(mysqli $mysqli_link): ?object {}

function mysqli_get_client_info(mysqli $mysqli_link = UNKNOWN): ?string {}

function mysqli_get_client_version(): int {}

function mysqli_get_links_stats(): array {}

function mysqli_get_host_info(mysqli $mysqli_link): string {}

function mysqli_get_proto_info(mysqli $mysqli_link): int {}

function mysqli_get_server_info(mysqli $mysqli_link): ?string {}

function mysqli_get_server_version(mysqli $mysqli_link): int {}

function mysqli_get_warnings(mysqli $mysqli_link): mysqli_warning|false {}

function mysqli_init(): mysqli|false {}

function mysqli_info(mysqli $mysqli_link): ?string {}

function mysqli_insert_id(mysqli $mysqli_link): int|string {}

function mysqli_kill(mysqli $mysqli_link, int $connection_id): bool {}

function mysqli_more_results(mysqli $mysqli_link): bool {}

function mysqli_multi_query(mysqli $mysqli_link, string $query): bool {}

function mysqli_next_result(mysqli $mysqli_link): bool {}

function mysqli_num_fields(mysqli_result $mysql_result): int {}

function mysqli_num_rows(mysqli_result $mysqli_result): int|string {}

/** @param mixed $value */
function mysqli_options(mysqli $mysqli_link, int $option, $value): bool {}

function mysqli_ping(mysqli $mysqli_link): bool {}

function mysqli_poll(?array &$read, ?array &$write, array &$error, int $sec, int $usec = 0): int|false {}

function mysqli_prepare(mysqli $mysqli_link, string $query): mysqli_stmt|false {}

function mysqli_report(int $flags): bool {}

function mysqli_query(mysqli $mysqli_link, string $query, int $resultmode = MYSQLI_STORE_RESULT): mysqli_result|bool {}

function mysqli_real_connect(
    mysqli $mysqli_link,
    ?string $host = null,
    ?string $user = null,
    ?string $password = null,
    ?string $database = null,
    ?int $port = null,
    ?string $socket = null,
    int $flags = 0
): bool {}

function mysqli_real_escape_string(mysqli $mysqli_link, string $string_to_escape): string {}

function mysqli_real_query(mysqli $mysqli_link, string $query): bool {}

function mysqli_reap_async_query(mysqli $mysqli_link): mysqli_result|bool {}

function mysqli_release_savepoint(mysqli $mysqli_link, string $name): bool {}

function mysqli_rollback(mysqli $mysqli_link, int $flags = 0, string $name = ''): bool {}

function mysqli_savepoint(mysqli $mysqli_link, string $name): bool {}

function mysqli_select_db(mysqli $mysqli_link, string $dbname): bool {}

function mysqli_set_charset(mysqli $mysqli_link, string $charset): bool {}

function mysqli_stmt_affected_rows(mysqli_stmt $mysql_stmt): int|string {}

function mysqli_stmt_attr_get(mysqli_stmt $mysql_stmt, int $attr): int|false {}

function mysqli_stmt_attr_set(mysqli_stmt $mysql_stmt, int $attr, int $mode_in): bool {}

/** @param mixed &...$vars */
function mysqli_stmt_bind_param(mysqli_stmt $mysql_stmt, string $types, &...$vars): bool {}

/** @param mixed &...$vars */
function mysqli_stmt_bind_result(mysqli_stmt $mysql_stmt, &...$vars): bool {}

function mysqli_stmt_close(mysqli_stmt $mysql_stmt): bool {}

function mysqli_stmt_data_seek(mysqli_stmt $mysql_stmt, int $offset): ?bool {}

function mysqli_stmt_errno(mysqli_stmt $mysql_stmt): int {}

function mysqli_stmt_error(mysqli_stmt $mysql_stmt): ?string {}

function mysqli_stmt_error_list(mysqli_stmt $mysql_stmt): array {}

function mysqli_stmt_fetch(mysqli_stmt $mysql_stmt): ?bool {}

function mysqli_stmt_field_count(mysqli_stmt $mysql_stmt): int {}

function mysqli_stmt_free_result(mysqli_stmt $mysql_stmt): void {}

function mysqli_stmt_get_result(mysqli_stmt $mysql_stmt): mysqli_result|false {}

function mysqli_stmt_get_warnings(mysqli_stmt $mysql_stmt): mysqli_warning|false {}

function mysqli_stmt_init(mysqli $mysql_link): mysqli_stmt|false {}

function mysqli_stmt_insert_id(mysqli_stmt $mysql_stmt): int|string {}

function mysqli_stmt_more_results(mysqli_stmt $mysql_stmt): bool {}

function mysqli_stmt_next_result(mysqli_stmt $mysql_stmt): bool {}

function mysqli_stmt_num_rows(mysqli_stmt $mysql_stmt): int|string {}

function mysqli_stmt_param_count(mysqli_stmt $mysql_stmt): int {}

function mysqli_stmt_prepare(mysqli_stmt $mysql_stmt, string $query): bool {}

function mysqli_stmt_reset(mysqli_stmt $mysql_stmt): bool {}

function mysqli_stmt_result_metadata(mysqli_stmt $mysql_stmt): mysqli_result|false {}

function mysqli_stmt_send_long_data(mysqli_stmt $mysql_stmt, int $param_nr, string $data): bool {}

function mysqli_stmt_store_result(mysqli_stmt $mysql_stmt): bool {}

function mysqli_stmt_sqlstate(mysqli_stmt $mysql_stmt): ?string {}

function mysqli_sqlstate(mysqli $mysqli_link): ?string {}

function mysqli_ssl_set(
    mysqli $mysql_link,
    string $key,
    string $cert,
    string $certificate_authority,
    string $certificate_authority_path,
    string $cipher
): bool {}

function mysqli_stat(mysqli $mysql_link): string|false {}

function mysqli_store_result(mysqli $mysql_link, int $flags = 0): mysqli_result|false {}

function mysqli_thread_id(mysqli $mysql_link): int {}

function mysqli_thread_safe(): bool {}

function mysqli_use_result(mysqli $mysql_link): mysqli_result|false {}

function mysqli_warning_count(mysqli $mysql_link): int {}

function mysqli_refresh(mysqli $mysqli_link, int $options): bool {}

/** @alias mysqli_real_escape_string */
function mysqli_escape_string(mysqli $mysqli_link, string $string_to_escape): string {}

/**
 * @param mixed $value
 * @alias mysqli_options
 */
function mysqli_set_opt(mysqli $mysqli_link, int $option, $value): bool {}
