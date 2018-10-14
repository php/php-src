--TEST--
Test session_write_close() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_write_close(void)
 * Description : Write session data and end session
 * Source code : ext/session/session.c
 */

echo "*** Testing session_write_close() : variation ***\n";

var_dump(session_start());
var_dump(session_write_close());
var_dump(session_write_close());
var_dump(session_write_close());
var_dump(session_write_close());
var_dump(session_write_close());
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_write_close() : variation ***
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
Done
