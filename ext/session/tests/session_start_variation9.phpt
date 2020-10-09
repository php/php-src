--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.auto_start=1
--FILE--
<?php

ob_start();

echo "*** Testing session_start() : variation ***\n";

var_dump(session_id());
var_dump(session_start());
var_dump(session_id());
var_dump(session_destroy());
var_dump(session_id());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_start() : variation ***
string(%d) "%s"

Notice: session_start(): Ignoring session_start() because a session is already active in %s on line %d
bool(true)
string(%d) "%s"
bool(true)
string(0) ""
Done
