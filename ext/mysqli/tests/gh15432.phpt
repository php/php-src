--TEST--
Bug GH-15432 (Heap corruption when querying a vector)
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
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
var_dump($link->query('SELECT STRING_TO_VECTOR("[1.05, -17.8, 32]")'));
?>
--EXPECTF--
Warning: mysqli::query(): Unknown type 242 sent by the server. Please send a report to the developers in %s on line %d
bool(false)
