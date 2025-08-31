--TEST--
Test session_destroy() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

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
