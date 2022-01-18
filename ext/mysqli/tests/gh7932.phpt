--TEST--
GH-7932 (MariaDB version prefix not always stripped)
--SKIPIF--
<?php
require_once 'skipif.inc';
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once "connect.inc";

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$result = $mysqli->query("select version()");
$version = $result->fetch_array()[0];
var_dump($mysqli->server_info === $version);
?>
--EXPECT--
bool(true)
