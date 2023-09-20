<?php

function get_default_host(): string {
    static $host = null;
    if ($host === null) {
        $host = getenv('MYSQL_TEST_HOST') ?: '127.0.0.1';
    }
    return $host;
}
function get_default_port(): int {
    static $port = null;
    if ($port === null) {
        $port = getenv('MYSQL_TEST_PORT') ?: 3306;
    }
    return $port;
}
function get_default_user(): string {
    static $user = null;
    if ($user === null) {
        $user = getenv('MYSQL_TEST_USER') ?: 'root';
    }
    return $user;
}
function get_default_password(): string {
    static $password = null;
    if ($password === null) {
        $password = getenv('MYSQL_TEST_PASSWD') ?: '';
    }
    return $password;
}
function get_default_database(): string {
    static $db = null;
    if ($db === null) {
        $db = getenv('MYSQL_TEST_DB') ?: 'test';
    }
    return $db;
}
function get_default_db_engine(): string {
    static $engine = null;
    if ($engine === null) {
        $engine = getenv('MYSQL_TEST_ENGINE') ?: 'InnoDB';
    }
    return $engine;
}
function get_default_socket(): ?string {
    /*
    static $socket = null;
    if ($socket === null) {
        $socket = getenv('MYSQL_TEST_ENGINE') ?: null;
        if ($socket) {
            ini_set('mysqli.default_socket', $socket);
        }
    }
    return $socket;
    */
    return null;
}
function get_environment_connection_flags(): int {
    static $connect_flags = null;
    if ($connect_flags === null) {
        $connect_flags = (int)getenv("MYSQL_TEST_CONNECT_FLAGS") ?: 0;
    }
    return $connect_flags;
}

/**
 * Whenever possible, please use this wrapper to make testing of MYSQLI_CLIENT_COMPRESS (and potentially SSL) possible
 *
 * @param bool $enable_env_flags Enable setting of connection flags through 	env(MYSQL_TEST_CONNECT_FLAGS)?
 */
function my_mysqli_connect(
    string $host,
    string $user,
    string $password,
    string $db,
    int $port,
    ?string $socket = null,
    bool $enable_env_flags = true
): \mysqli {
    // Because the tests are meant to test both error modes, they can set the report_mode to a different value,
    // which we do not want to override. However, we want to make sure that if a connection cannot be made,
    // the constuctor will throw an exception. We store current report_mode in variable and restore it later.
    $driver = new mysqli_driver;
    $report_mode = $driver->report_mode;
    $driver->report_mode = MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT;
    $flags = $enable_env_flags ? get_environment_connection_flags() : 0;
    if ($flags !== 0) {
        $link = mysqli_init();
        mysqli_real_connect($link, $host, $user, $password, $db, $port, $socket, $flags);
    } else {
        $link = mysqli_connect($host, $user, $password, $db, $port, $socket);
    }
    // Restore error mode
    $driver->report_mode = $report_mode;
    return $link;
}
function default_mysqli_connect(): \mysqli{
    return my_mysqli_connect(
        get_default_host(),
        get_default_user(),
        get_default_password(),
        get_default_database(),
        get_default_port(),
        null,
    );
}
function mysqli_check_skip_test(): void {
    try {
        $link = default_mysqli_connect();
    } catch (\mysqli_sql_exception) {
        die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
    }
}
function have_innodb(mysqli $link): bool {
    $res = $link->query("SELECT SUPPORT FROM INFORMATION_SCHEMA.ENGINES WHERE ENGINE = 'InnoDB'");
    $supported = $res->fetch_column();
    return $supported === 'YES' || $supported === 'DEFAULT';
}
function mysqli_check_innodb_support_skip_test(): void {
    try {
        $link = default_mysqli_connect();
    } catch (\mysqli_sql_exception) {
        die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
    }
    if (! have_innodb($link)) {
        die(sprintf("skip Needs InnoDB support"));
    }
}
function tear_down_table_on_default_connection(string $table) {
    $link = default_mysqli_connect();
    mysqli_query($link, 'DROP TABLE IF EXISTS ' . $table);
}

function setup_table_with_data_on_default_connection(string $table): mysqli {
    $link = default_mysqli_connect();
    mysqli_query($link, 'SET SESSION sql_mode=\'\'');
    mysqli_query($link, 'CREATE TABLE '. $table .'(id INT DEFAULT 0, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . get_default_db_engine());
    mysqli_query($link, 'INSERT INTO '. $table .'(id, label) VALUES (1, "a"), (2, "b"), (3, "c"), (4, "d"), (5, "e"), (6, "f")');
    return $link;
}

/* Development setting: test experimental features and/or feature requests that never worked before? */
//$TEST_EXPERIMENTAL = 1 == getenv("MYSQL_TEST_EXPERIMENTAL");
//$engine    = getenv("MYSQL_TEST_ENGINE")   ?: "InnoDB";
