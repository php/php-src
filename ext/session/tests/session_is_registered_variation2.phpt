--TEST--
Test session_unregister() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_is_registered(string $name)
 * Description : Find out whether a global variable is registered in a session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_is_registered() : variation ***\n";

var_dump(session_is_registered("foo"));
var_dump(session_start());
var_dump(session_is_registered("foo"));
var_dump($_SESSION);
$_SESSION["foo"] = "Hello World!";
var_dump(session_is_registered("foo"));
var_dump($_SESSION);
var_dump(session_is_registered("foo"));
var_dump(session_unregister("foo"));
var_dump(session_is_registered("foo"));
var_dump($_SESSION);
var_dump(session_destroy());
var_dump(session_is_registered("foo"));
var_dump($_SESSION);
var_dump(session_is_registered("foo"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_is_registered() : variation ***
bool(false)
bool(true)
bool(false)
array(0) {
}
bool(true)
array(1) {
  ["foo"]=>
  string(12) "Hello World!"
}
bool(true)
bool(true)
bool(false)
array(0) {
}
bool(true)
bool(false)
array(0) {
}
bool(false)
Done

