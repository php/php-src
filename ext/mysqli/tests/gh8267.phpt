--TEST--
Bug GH-8267 (MySQLi uses unsupported format specifier on Windows)
--SKIPIF--
<?php
require_once("skipif.inc");
require_once("skipifconnectfailure.inc");
?>
--FILE--
<?php
require_once("connect.inc");

$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$mysqli->query("DROP TABLE IF EXISTS foo");
$mysqli->query("CREATE TABLE foo (id BIGINT UNSIGNED AUTO_INCREMENT, PRIMARY KEY (id))");
$mysqli->query("INSERT INTO foo VALUES (9223372036854775807)");
var_dump($mysqli->insert_id);
$mysqli->query("INSERT INTO foo VALUES (0)");
var_dump($mysqli->insert_id);
?>
--EXPECT--
string(19) "9223372036854775807"
string(19) "9223372036854775808"
