--TEST--
Bug #81335: Packets out of order after connection timeout
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");

if (!defined('MYSQLI_STORE_RESULT_COPY_DATA')) die('skip requires mysqlnd');

require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) die("skip cannot connect");
if (mysqli_get_server_version($link) < 80024 || str_contains(mysqli_get_server_info($link), 'MariaDB')) {
    $link->close();
    die("skip: Due to many MySQL Server differences, the test requires >= 8.0.24");
}
$link->close();
?>
--FILE--
<?php

require_once 'connect.inc';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$mysqli->query('SET WAIT_TIMEOUT=1');
usleep(1000000 * 1.1);
try {
    $mysqli->query('SELECT 1 + 1');
} catch(mysqli_sql_exception $e) {
    echo $e->getMessage();
    echo "\n";
    echo $e->getCode();
}
?>
--EXPECTF--
The client was disconnected by the server because of inactivity. See wait_timeout and interactive_timeout for configuring this behavior.
4031
