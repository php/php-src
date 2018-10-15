--TEST--
Test session_id() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_id([string $id])
 * Description : Get and/or set the current session id
 * Source code : ext/session/session.c
 */

echo "*** Testing session_id() : error functionality ***\n";

var_dump(session_id());
var_dump(session_start());
var_dump(session_id("test"));
var_dump(session_id());
var_dump(session_id("1234567890"));
var_dump(session_id());
var_dump(session_destroy());
var_dump(session_id());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_id() : error functionality ***
string(0) ""
bool(true)
string(%d) "%s"
string(4) "test"
string(4) "test"
string(10) "1234567890"
bool(true)
string(0) ""
Done
