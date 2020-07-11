--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_start() : variation ***\n";

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
*** Testing session_start() : variation ***
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
