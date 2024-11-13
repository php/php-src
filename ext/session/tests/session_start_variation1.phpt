--TEST--
Test session_start() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_start() : variation ***\n";

var_dump(session_start());
var_dump(session_start());
var_dump(session_start());
var_dump(session_start());
var_dump(session_start());

session_destroy();
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_start() : variation ***
bool(true)

Notice: session_start(): Ignoring session_start() because a session is already active (started from %s on line %d) in %s on line %d
bool(true)

Notice: session_start(): Ignoring session_start() because a session is already active (started from %s on line %d) in %s on line %d
bool(true)

Notice: session_start(): Ignoring session_start() because a session is already active (started from %s on line %d) in %s on line %d
bool(true)

Notice: session_start(): Ignoring session_start() because a session is already active (started from %s on line %d) in %s on line %d
bool(true)
Done
