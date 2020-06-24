--TEST--
Test session_start() function : variation
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

Notice: session_start(): A session had already been started - ignoring in %s on line %d
bool(true)

Notice: session_start(): A session had already been started - ignoring in %s on line %d
bool(true)

Notice: session_start(): A session had already been started - ignoring in %s on line %d
bool(true)

Notice: session_start(): A session had already been started - ignoring in %s on line %d
bool(true)
Done
