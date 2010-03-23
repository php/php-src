--TEST--
Test session_unregister() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_unregister(string $name)
 * Description : Unregister a global variable from the current session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_unregister() : variation ***\n";

var_dump(session_start());
var_dump($_SESSION);
$blah = "Hello World!";
$_SESSION["foo"] = &$blah;
var_dump($_SESSION);
var_dump(session_unregister("foo"));
var_dump($_SESSION);
var_dump(session_destroy());
var_dump($_SESSION);

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_unregister() : variation ***
bool(true)
array(0) {
}
array(1) {
  ["foo"]=>
  &string(12) "Hello World!"
}

Deprecated: Function session_unregister() is deprecated in %s on line %d
bool(true)
array(0) {
}
bool(true)
array(0) {
}
Done
