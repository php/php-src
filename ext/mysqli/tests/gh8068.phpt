--TEST--
GH-8068 (mysqli_fetch_object creates inaccessible properties)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$res = $mysqli->query('SELECT 42');
$obj = $res->fetch_object();
var_dump(
    $obj,
    $obj->{42}
);
?>
--EXPECT--
object(stdClass)#4 (1) {
  ["42"]=>
  string(2) "42"
}
string(2) "42"
