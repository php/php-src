--TEST--
mysqli_float_handling - ensure 4 byte float is handled correctly
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
}

if ($link->server_version < 50709) {
    die("skip MySQL 5.7.9+ needed. Found [".
        intval(substr($link->server_version."", -5, 1)).
        ".".
        intval(substr($link->server_version."", -4, 2)).
        ".".
        intval(substr($link->server_version."", -2, 2)).
        "]");
}
?>
--FILE--
<?php
    require 'connect.inc';
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        die();
    }

    if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        die();
    }

    if (!mysqli_query($link, "CREATE TABLE test(jsfield JSON) ENGINE = InnoDB")) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        die();
    }
    $jsfield_data = '{"aaa": 123}';
    // Insert via string to make sure the real floating number gets to the DB
    if (!mysqli_query($link, "INSERT INTO test VALUES ('".$jsfield_data."')")) {
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        die();
    }

    if (!($res = mysqli_query($link, "SELECT *  FROM test"))) {
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        die();
    }
    $rows = $res->fetch_all();
    if (json_encode($rows[0][0]) != json_encode($jsfield_data)) {
        printf("[006] Data differs");
        var_dump(json_encode($rows[0][0]) != json_encode($jsfield_data));
        die();
    }
    mysqli_close($link);
    echo "OK";
?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECT--
OK
