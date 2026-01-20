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

$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket);

$value = $mysqli->options(10, 'invalid_option');

var_dump($value);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Argument #1 ($option) is not a valid mysqli option in %s:%d
Stack trace:
#0 %s(%d): mysqli->options(%d, 'invalid_option')
#1 {main}
  thrown in %s on line %d
