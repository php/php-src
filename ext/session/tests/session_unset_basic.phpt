--TEST--
Test session_unset() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_unset() : basic functionality ***\n";

var_dump(session_start());
$_SESSION["foo"] = "Hello World!";
var_dump($_SESSION);
var_dump(session_unset());
var_dump($_SESSION);
var_dump(session_destroy());
var_dump($_SESSION);

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_unset() : basic functionality ***
bool(true)
array(1) {
  ["foo"]=>
  string(12) "Hello World!"
}
bool(true)
array(0) {
}
bool(true)
array(0) {
}
Done
