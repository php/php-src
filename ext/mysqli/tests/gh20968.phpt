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
mysqli::options(): Argument #%d ($option) must be MYSQLI_INIT_COMMAND, MYSQLI_SET_CHARSET_NAME,  MYSQLI_SERVER_PUBLIC_KEY, or one of the MYSQLI_OPT_* constants