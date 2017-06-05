--TEST--
Test session_start() failure still populates $_SESSION
--SKIPIF--
<?php if (!function_exists("session_start")) die("skip session support is not available"); ?>
--FILE--
<?php

echo "start\n";

session_start();
$_SESSION['blah'] = 'foo';
var_dump($_SESSION);
session_write_close();

session_start();
var_dump($_SESSION);

?>
--EXPECTF--
start

Warning: session_start(): Cannot send session cookie - headers already sent by (output started at %s:%d) in %s on line %d

Warning: session_start(): Cannot send session cache limiter - headers already sent (output started at %s:%d) in %s on line %d
array(1) {
  ["blah"]=>
  string(3) "foo"
}

Warning: session_start(): Cannot send session cookie - headers already sent by (output started at %s:%d) in %s on line %d

Warning: session_start(): Cannot send session cache limiter - headers already sent (output started at %s:%d) in %s on line %d
array(1) {
  ["blah"]=>
  string(3) "foo"
}
