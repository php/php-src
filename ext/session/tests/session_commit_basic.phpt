--TEST--
Test session_commit() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_commit(void)
 * Description : Write session data and end session
 * Source code : ext/session/session.c
 */

echo "*** Testing session_commit() : basic functionality ***\n";

var_dump(session_start());
var_dump($_SESSION);
var_dump(session_commit());
var_dump($_SESSION);
var_dump(session_start());
var_dump($_SESSION);
var_dump(session_destroy());
var_dump($_SESSION);

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_commit() : basic functionality ***
bool(true)
array(0) {
}
bool(true)
array(0) {
}
bool(true)
array(0) {
}
bool(true)
array(0) {
}
Done
