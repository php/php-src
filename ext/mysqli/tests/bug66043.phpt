--TEST--
Bug #66043 (Segfault calling bind_param() on mysqli) - Calling mysql_stmt::bind_result() without storing it's result value in a variable is causing a segfault.
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require 'connect.inc';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$db = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$stmt = $db->prepare("SELECT 'Test'");
$stmt->execute();
$stmt->bind_result($testArg);
echo "Okey";
?>
--EXPECT--
Okey
