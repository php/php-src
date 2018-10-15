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
 * Prototype : bool session_write_close(void)
 * Description : Write session data and end session
 * Source code : ext/session/session.c
 */

echo "*** Testing session_write_close() : variation ***\n";

var_dump($_SESSION);
var_dump(session_write_close());
var_dump($_SESSION);
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_write_close() : variation ***
array(0) {
}
bool(true)
array(0) {
}
bool(true)
bool(true)
Done
