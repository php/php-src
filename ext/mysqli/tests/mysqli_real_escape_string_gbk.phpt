--TEST--
mysqli_real_escape_string() - gbk
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';

if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Cannot connect to MySQL, [%d] %s\n",
        mysqli_connect_errno(), mysqli_connect_error()));
}
if (!mysqli_set_charset($link, 'gbk'))
    die(sprintf("skip Cannot set charset 'gbk'"));

mysqli_close($link);
?>
--FILE--
<?php

    require_once 'connect.inc';
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
{
        printf("[001] Cannot connect to the server using host=%s, user=%s,
passwd=***, dbname=%s, port=%s, socket=%s - [%d] %s\n", $host, $user, $db,
$port, $socket, mysqli_connect_errno(), mysqli_connect_error());
    }

    if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
        printf("Failed to drop old test table: [%d] %s\n", mysqli_errno($link),
mysqli_error($link));
    }

    if (!mysqli_query($link, 'CREATE TABLE test(id INT, label CHAR(3), PRIMARY
KEY(id)) ENGINE=' . $engine . " DEFAULT CHARSET=gbk")) {
        printf("Failed to create test table: [%d] %s\n", mysqli_errno($link),
mysqli_error($link));
    }

    var_dump(mysqli_set_charset($link, "gbk"));

    if ('�İ汾\\\\�İ汾' !== ($tmp = mysqli_real_escape_string($link, '�İ汾\\�İ汾')))
        printf("[004] Expecting \\\\, got %s\n", $tmp);

    if ('�İ汾\"�İ汾' !== ($tmp = mysqli_real_escape_string($link, '�İ汾"�İ汾')))
        printf("[005] Expecting \", got %s\n", $tmp);

    if ("�İ汾\'�İ汾" !== ($tmp = mysqli_real_escape_string($link, "�İ汾'�İ汾")))
        printf("[006] Expecting ', got %s\n", $tmp);

    if ("�İ汾\\n�İ汾" !== ($tmp = mysqli_real_escape_string($link, "�İ汾\n�İ汾")))
        printf("[007] Expecting \\n, got %s\n", $tmp);

    if ("�İ汾\\r�İ汾" !== ($tmp = mysqli_real_escape_string($link, "�İ汾\r�İ汾")))
        printf("[008] Expecting \\r, got %s\n", $tmp);

    if ("�İ汾\\0�İ汾" !== ($tmp = mysqli_real_escape_string($link, "�İ汾" . chr(0) . "�İ汾")))
        printf("[009] Expecting %s, got %s\n", "�İ汾\\0�İ汾", $tmp);

    var_dump(mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, '��')"));

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
bool(true)
bool(true)
done!
