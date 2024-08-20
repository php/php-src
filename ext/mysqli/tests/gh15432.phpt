--TEST--
Bug GH-15432 (Heap corruption when querying a vector/PHP crashes when processing a MySQL DB query with a new Vector format introduced in MySQL 9.0)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require 'connect.inc';
$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if ($link === false) {
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
}
if ($link->server_version < 90000 || $link->server_version >= 10_00_00) {
    die("skip MySQL 9.0.0+ needed");
}
?>
--FILE--
<?php
require 'connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$expected = '00000040000040400000a040';

mysqli_query($link, "DROP TABLE IF EXISTS test");
mysqli_query($link, "CREATE TABLE test(vectorfield VECTOR)");
mysqli_query($link, 'INSERT INTO test VALUES (TO_VECTOR("[2, 3, 5]"))');

// Textual protocol
$result = mysqli_query($link, "SELECT vectorfield FROM test")->fetch_column();
$value = bin2hex($result);
if($value !== $expected) {
    printf("[001] Expecting %s/%s, got %s/%s\n",
        gettype($expected), $expected,
        gettype($value), $value);
}

// Binary protocol
$result = $link->execute_query("SELECT vectorfield FROM test")->fetch_column();
$value = bin2hex($result);
if($value !== $expected) {
    printf("[002] Expecting %s/%s, got %s/%s\n",
        gettype($expected), $expected,
        gettype($value), $value);
}

// Testing inverse to make sure the value hasn't been changed
$expected = '[2.00000e+00,3.00000e+00,5.00000e+00]';
$result = $link->execute_query("SELECT VECTOR_TO_STRING(0x". $value .")")->fetch_column();
if($result !== $expected) {
    printf("[002] Expecting %s/%s, got %s/%s\n",
        gettype($expected), $expected,
        gettype($result), $result);
}

echo "OK";
?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECT--
OK
