--TEST--
Test session_encode() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.auto_start=1
--FILE--
<?php

ob_start();

echo "*** Testing session_encode() : variation ***\n";

var_dump(session_encode());
var_dump(session_destroy());
var_dump(session_encode());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_encode() : variation ***
bool(false)
bool(true)

Warning: session_encode(): Cannot encode non-existent session in %s on line %d
bool(false)
Done
