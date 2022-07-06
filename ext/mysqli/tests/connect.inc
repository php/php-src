<?php
    /*
    Default values are "localhost", "root",
    database "test" and empty password.
    Change the MYSQL_TEST environment values
    if you want to use another configuration
    */

    $driver    = new mysqli_driver;

    $host      = getenv("MYSQL_TEST_HOST")     ?: "127.0.0.1";
    $port      = getenv("MYSQL_TEST_PORT")     ?: 3306;
    $user      = getenv("MYSQL_TEST_USER")     ?: "root";
    $passwd    = getenv("MYSQL_TEST_PASSWD")   ?: "";
    $db        = getenv("MYSQL_TEST_DB")       ?: "test";
    $engine    = getenv("MYSQL_TEST_ENGINE")   ?: "InnoDB";
    $socket    = getenv("MYSQL_TEST_SOCKET")   ?: null;
    $skip_on_connect_failure  = getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") ?: true;
    $connect_flags = (int)getenv("MYSQL_TEST_CONNECT_FLAGS") ?: 0;
    if ($socket) {
        ini_set('mysqli.default_socket', $socket);
    }

    /* Development setting: test experimental features and/or feature requests that never worked before? */
    $TEST_EXPERIMENTAL = (in_array(getenv("MYSQL_TEST_EXPERIMENTAL"), array(0, 1))) ?
                ((1 == getenv("MYSQL_TEST_EXPERIMENTAL")) ? true : false) :
                false;

    $IS_MYSQLND = stristr(mysqli_get_client_info(), "mysqlnd");

    if (!function_exists('sys_get_temp_dir')) {
        function sys_get_temp_dir() {

            if (!empty($_ENV['TMP']))
                return realpath( $_ENV['TMP'] );
            if (!empty($_ENV['TMPDIR']))
                return realpath( $_ENV['TMPDIR'] );
            if (!empty($_ENV['TEMP']))
                return realpath( $_ENV['TEMP'] );

            $temp_file = tempnam(md5(uniqid(rand(), TRUE)), '');
            if ($temp_file) {
                $temp_dir = realpath(dirname($temp_file));
                unlink($temp_file);
                return $temp_dir;
            }
            return FALSE;
        }
    }

    if (!function_exists('my_mysqli_connect')) {

        /**
        * Whenever possible, please use this wrapper to make testing ot MYSQLI_CLIENT_COMPRESS (and potentially SSL) possible
        *
        * @param enable_env_flags Enable setting of connection flags through 	env(MYSQL_TEST_CONNECT_FLAGS)?
        */
        function my_mysqli_connect($host, $user, $passwd, $db, $port, $socket, $enable_env_flags = true) {
            global $connect_flags;

            $flags = $enable_env_flags? $connect_flags:0;
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
        * Whenever possible, please use this wrapper to make testing ot MYSQLI_CLIENT_COMPRESS (and potentially SSL) possible
        *
        * @param enable_env_flags Enable setting of connection flags through env(MYSQL_TEST_CONNECT_FLAGS)
        */
        function my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags = 0, $enable_env_flags = true) {
            global $connect_flags;

            if ($enable_env_flags)
                $flags = $flags | $connect_flags;

            return mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags);
        }

        class my_mysqli extends mysqli {
            public function __construct($host, $user, $passwd, $db, $port, $socket, $enable_env_flags = true) {
                global $connect_flags;

                $flags = ($enable_env_flags) ? $connect_flags : 0;

                if ($flags !== false) {
                    parent::init();
                    $this->real_connect($host, $user, $passwd, $db, $port, $socket, $flags);
                } else {
                    parent::__construct($host, $user, $passwd, $db, $port, $socket);
                }
            }
        }

        function have_innodb($link) {
          if (($res = $link->query("SHOW VARIABLES LIKE 'have_innodb'")) &&
                ($row = $res->fetch_row()) &&
                !empty($row)) {
                if ($row[1] == "DISABLED" || $row[1] == "NO") {
                    return false;
                }
                return true;
          } else {
                /* MySQL 5.6.1+ */
                if ($res = $link->query("SHOW ENGINES")) {
                    while ($row = $res->fetch_assoc()) {
                        if (!isset($row['Engine']) || !isset($row['Support']))
                            return false;

                        if (('InnoDB' == $row['Engine']) &&
                            (('YES' == $row['Support']) || ('DEFAULT' == $row['Support']))
                            ) {
                            return true;
                        }
                    }
                    return false;
                } else {
                    return false;
                }
          }
          return false;
        }

    } else {
        printf("skip Eeeek/BUG/FIXME - connect.inc included twice! skipif bug?\n");
    }

    function handle_catchable_fatal($errno, $error, $file, $line) {
        static $errcodes = array();
        if (empty($errcodes)) {
            $constants = get_defined_constants();
            foreach ($constants as $name => $value) {
                if (substr($name, 0, 2) == "E_")
                    $errcodes[$value] = $name;
            }
        }
        printf("[%s] %s in %s on line %s\n",
            (isset($errcodes[$errno])) ? $errcodes[$errno] : $errno,
             $error, $file, $line);

        return true;
    }
?>
