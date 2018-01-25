--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_start(void)
 * Description : Initialize session data
 * Source code : ext/session/session.c
 */

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

Notice: session_start(): A session had already been started - ignoring in %s on line 14
bool(true)

Notice: session_start(): A session had already been started - ignoring in %s on line 15
bool(true)

Notice: session_start(): A session had already been started - ignoring in %s on line 16
bool(true)

Notice: session_start(): A session had already been started - ignoring in %s on line 17
bool(true)
Done
