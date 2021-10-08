--TEST--
Bug #63398 (Segfault when calling fetch_object on a use_result and DB pointer has closed)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once("connect.inc");
if (!$IS_MYSQLND) {
    die("skip mysqlnd only test");
}
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require 'connect.inc';
$db = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

mysqli_report(MYSQLI_REPORT_ERROR);
$result = $db->query('SELECT 1', MYSQLI_USE_RESULT);
$db->close();
var_dump($result->fetch_object());
?>
--EXPECTF--
Warning: mysqli_result::fetch_object(): (HY000/2014): Commands out of sync; you can't run this command now in %s on line %d
bool(false)
