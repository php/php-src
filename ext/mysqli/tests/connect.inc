<?php
    /*
    Default values are "localhost", "root",
    database "test" and empty password.
    Change the MYSQL_TEST environment values
    if you want to use another configuration
    */

    $driver    = new mysqli_driver;
    $driver->report_mode = MYSQLI_REPORT_OFF;

    $host      = getenv("MYSQL_TEST_HOST")     ?: "127.0.0.1";
    $port      = getenv("MYSQL_TEST_PORT")     ?: 3306;
    $user      = getenv("MYSQL_TEST_USER")     ?: "root";
    $passwd    = getenv("MYSQL_TEST_PASSWD")   ?: "";
    $db        = getenv("MYSQL_TEST_DB")       ?: "test";
    $engine    = getenv("MYSQL_TEST_ENGINE")   ?: "InnoDB";
    $socket    = getenv("MYSQL_TEST_SOCKET")   ?: null;
    $skip_on_connect_failure  = getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") ?: true;
    if ($socket) {
        ini_set('mysqli.default_socket', $socket);
    }

    /* Development setting: test experimental features and/or feature requests that never worked before? */
    $TEST_EXPERIMENTAL = 1 == getenv("MYSQL_TEST_EXPERIMENTAL");

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
    function my_mysqli_connect($host, $user, $passwd, $db, $port, $socket, $enable_env_flags = true) {
        $flags = $enable_env_flags? get_environment_connection_flags():0;
        if ($flags !== 0) {
            $link = mysqli_init();
            if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags))
                $link = false;
        } else {
            $link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);
        }

        return $link;
    }

    /**
    * Whenever possible, please use this wrapper to make testing of MYSQLI_CLIENT_COMPRESS (and potentially SSL) possible
    *
    * @param bool $enable_env_flags Enable setting of connection flags through env(MYSQL_TEST_CONNECT_FLAGS)
    */
    function my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags = 0, $enable_env_flags = true) {
        if ($enable_env_flags) {
            $flags = $flags | get_environment_connection_flags();
        }

        return mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags);
    }

    class my_mysqli extends mysqli {
        public function __construct($host, $user, $passwd, $db, $port, $socket, $enable_env_flags = true) {
            $flags = ($enable_env_flags) ? get_environment_connection_flags() : 0;

            // Because the tests are meant to test both error modes, they can set the report_mode to a different value,
            // which we do not want to override. However, we want to make sure that if a connection cannot be made,
            // the constuctor will throw an exception. We store current report_mode in variable and restore it later.
            $driver = new mysqli_driver;
            $report_mode = $driver->report_mode;
            $driver->report_mode = MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT;

            if ($flags !== 0) {
                parent::__construct();
                $this->real_connect($host, $user, $passwd, $db, $port, $socket, $flags);
            } else {
                parent::__construct($host, $user, $passwd, $db, $port, $socket);
            }

            // Restore error mode
            $driver->report_mode = $report_mode;
        }
    }

    function have_innodb($link) {
        if (($res = $link->query("SHOW VARIABLES LIKE 'have_innodb'"))
            && ($row = $res->fetch_row())
            && !empty($row)
        ) {
            return !($row[1] == 'DISABLED' || $row[1] == 'NO');
        }
        // MySQL 5.6.1+
        if ($res = $link->query('SHOW ENGINES')) {
            while ($row = $res->fetch_assoc()) {
                if (!isset($row['Engine']) || !isset($row['Support'])) {
                    return false;
                }

                if (($row['Engine'] == 'InnoDB')
                    && (($row['Support'] == 'YES') || ($row['Support'] == 'DEFAULT'))
                ) {
                    return true;
                }
            }
        }
        return false;
    }
