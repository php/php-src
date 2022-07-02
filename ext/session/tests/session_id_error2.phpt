--TEST--
Test session_id() function : error functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_id() : error functionality ***\n";

var_dump(session_id("test"));
var_dump(session_id());
var_dump(session_id("1234567890"));
var_dump(session_id());
var_dump(session_start());
var_dump(session_id("1234567890"));
var_dump(session_destroy());
var_dump(session_id());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_id() : error functionality ***
string(0) ""
string(4) "test"
string(4) "test"
string(10) "1234567890"
bool(true)

Warning: session_id(): Session ID cannot be changed when a session is active in %s on line %d
bool(false)
bool(true)
string(0) ""
Done
