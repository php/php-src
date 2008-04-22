--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.auto_start=1
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_commit(void)
 * Description : Write session data and end session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_commit() : variation ***\n";

var_dump($_SESSION);
var_dump(session_commit());
var_dump($_SESSION);
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_commit() : variation ***
array(0) {
}
NULL
array(0) {
}
bool(true)
bool(true)
Done

