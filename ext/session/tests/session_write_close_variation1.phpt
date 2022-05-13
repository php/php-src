--TEST--
Test session_write_close() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

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
--EXPECT--
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
