--TEST--
Test session_decode() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=blah
--FILE--
<?php

ob_start();

echo "*** Testing session_decode() : variation ***\n";

var_dump(session_start());
var_dump($_SESSION);
$_SESSION["foo"] = 1234567890;
$_SESSION["bar"] = "Blah!";
$_SESSION["guff"] = 123.456;
var_dump($_SESSION);
$encoded = "foo|i:1234567890;";
var_dump(session_decode($encoded));
var_dump($_SESSION);
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_decode() : variation ***

Warning: session_start(): Cannot find session serialization handler "blah" - session startup failed in %s on line %d
bool(false)

Warning: Undefined global variable $_SESSION in %s on line %d
NULL
array(3) {
  ["foo"]=>
  int(1234567890)
  ["bar"]=>
  string(5) "Blah!"
  ["guff"]=>
  float(123.456)
}

Warning: session_decode(): Session data cannot be decoded when there is no active session in %s on line %d
bool(false)
array(3) {
  ["foo"]=>
  int(1234567890)
  ["bar"]=>
  string(5) "Blah!"
  ["guff"]=>
  float(123.456)
}

Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d
bool(false)
Done
