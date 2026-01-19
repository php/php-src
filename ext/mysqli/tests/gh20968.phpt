--TEST--
Bug #20968 mysqli_options() with invalid option triggers ValueError
--EXTENSIONS--
mysqli
--CONFLICTS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$driver = new mysqli_driver();

$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket);

$driver->report_mode = MYSQLI_REPORT_OFF;
$value = $mysqli->options(10, 'invalid_option');

var_dump($value);

$driver->report_mode = MYSQLI_REPORT_ALL;
$value = $mysqli->options(10, 'invalid_option');
var_dump($value);

?>
--EXPECTF--
bool(false)

Fatal error: Uncaught ValueError: mysqli_options(): Invalid option %d in %s:%d
Stack trace:
#0 %s(%d): mysqli->options(%d, 'invalid_option')
#1 {main}
  thrown in %s on line %d