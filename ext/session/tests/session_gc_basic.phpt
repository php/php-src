--TEST--
Test session_gc() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_gc() : basic functionality ***\n";

var_dump(session_gc());

var_dump(session_start());
var_dump(session_gc(), session_gc() >= -1);
var_dump(session_destroy());
var_dump(session_id());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_gc() : basic functionality ***

Warning: session_gc(): Session cannot be garbage collected when there is no active session in %s on line %d
bool(false)
bool(true)
int(%d)
bool(true)
bool(true)
string(0) ""
Done
