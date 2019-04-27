--TEST--
Test session_destroy() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_destroy(void)
 * Description : Destroys all data registered to a session
 * Source code : ext/session/session.c
 */

echo "*** Testing session_destroy() : variation ***\n";

var_dump(session_start());
var_dump(session_destroy());
var_dump(session_start());
var_dump(session_destroy());
var_dump(session_start());
var_dump(session_destroy());
var_dump(session_start());
var_dump(session_destroy());
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_destroy() : variation ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Done
