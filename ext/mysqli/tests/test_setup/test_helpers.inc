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
): \mysqli|false {

    $flags = $enable_env_flags? get_environment_connection_flags():0;
    if ($flags !== 0) {
        $link = mysqli_init();
        if (!mysqli_real_connect($link, $host, $user, $password, $db, $port, $socket, $flags)) {
            $link = false;
        }
    } else {
        $link = mysqli_connect($host, $user, $password, $db, $port, $socket);
    }

    return $link;
}

/**
 * Whenever possible, please use this wrapper to make testing of MYSQLI_CLIENT_COMPRESS (and potentially SSL) possible
 *
 * @param bool $enable_env_flags Enable setting of connection flags through env(MYSQL_TEST_CONNECT_FLAGS)
 */
function my_mysqli_real_connect(
    mysqli $link,
    string $host,
    string $user,
    string $password,
    string $db,
    int $port,
    ?string $socket = null,
    int $flags = 0,
    bool $enable_env_flags = true
): \mysqli|false {
    if ($enable_env_flags) {
        $flags = $flags | get_environment_connection_flags();
    }

    return mysqli_real_connect($link, $host, $user, $password, $db, $port, $socket, $flags);
}

function default_mysqli_connect_ex(): \mysqli|false {
    return my_mysqli_connect(
        get_default_host(),
        get_default_user(),
        get_default_password(),
        get_default_database(),
        get_default_port(),
        null,
    );
}
function default_mysqli_connect(): \mysqli{
    $link = default_mysqli_connect_ex();
    if (!$link) {
        throw new Error("Cannot connect to default connection");
    }
    return $link;
}
function default_mysqli_real_connect(mysqli $link, int $flags = 0): bool {
    return my_mysqli_real_connect(
        $link,
        get_default_host(),
        get_default_user(),
        get_default_password(),
        get_default_database(),
        get_default_port(),
        null,
        $flags,
    );
}

function mysqli_check_skip_test(): void {
    $link = @default_mysqli_connect_ex();
    if (!is_object($link)) {
        die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
    }
    mysqli_close($link);
}

function tear_down_table_on_default_connection(string $table) {
    $link = default_mysqli_connect();
    if (!mysqli_query($link, 'DROP TABLE IF EXISTS ' . $table)) {
        printf("[clean] Failed to drop \"$table\" table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
}

/* Development setting: test experimental features and/or feature requests that never worked before? */
//$TEST_EXPERIMENTAL = 1 == getenv("MYSQL_TEST_EXPERIMENTAL");
//$engine    = getenv("MYSQL_TEST_ENGINE")   ?: "InnoDB";
