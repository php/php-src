--TEST--
Test session_register() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_register(mixed $name [,mixed $...])
 * Description : Register one or more global variables with the current session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_register() : variation ***\n";

var_dump(session_start());
var_dump($_SESSION);
var_dump(session_register());
var_dump($_SESSION);
var_dump(session_destroy());
var_dump($_SESSION);

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_register() : variation ***
bool(true)
array(0) {
}
bool(false)
array(0) {
}
bool(true)
array(0) {
}
Done

