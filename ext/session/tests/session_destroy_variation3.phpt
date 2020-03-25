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
var_dump(session_id());
var_dump(session_destroy());
var_dump(session_id());
var_dump(session_start());
var_dump(session_id());
var_dump(session_destroy());
var_dump(session_id());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_destroy() : variation ***
bool(true)
string(%d) "%s"
bool(true)
string(0) ""
bool(true)
string(%d) "%s"
bool(true)
string(0) ""
Done
