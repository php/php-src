--TEST--
Test session_unset() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_unset() : variation ***\n";

var_dump(session_unset());
var_dump(session_start());
var_dump(session_unset());
$_SESSION["foo"] = "Hello World!";
var_dump($_SESSION);
var_dump(session_destroy());
var_dump(session_unset());
var_dump($_SESSION);
var_dump(session_unset());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_unset() : variation ***
bool(false)
bool(true)
bool(true)
array(1) {
  ["foo"]=>
  string(12) "Hello World!"
}
bool(true)
bool(false)
array(1) {
  ["foo"]=>
  string(12) "Hello World!"
}
bool(false)
Done
