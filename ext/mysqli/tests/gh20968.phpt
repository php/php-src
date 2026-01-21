--TEST--
GH-20968 mysqli_options() with invalid option should triggers ValueError
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
   
try {
  $value = $mysqli->options(10, 'invalid_option');
  var_dump($value);
} catch (ValueError $exception) {
  echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
mysqli::options(): Argument #1 ($option) must be one of predefined options
