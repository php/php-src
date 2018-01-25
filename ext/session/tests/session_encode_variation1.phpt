--TEST--
Test session_encode() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_encode(void)
 * Description : Encodes the current session data as a string
 * Source code : ext/session/session.c
 */

echo "*** Testing session_encode() : variation ***\n";

var_dump(session_encode());
var_dump(session_start());
var_dump(session_encode());
var_dump(session_write_close());
var_dump(session_encode());
var_dump(session_start());
var_dump(session_encode());
var_dump(session_destroy());
var_dump(session_encode());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_encode() : variation ***

Warning: session_encode(): Cannot encode non-existent session in %s on line %d
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)

Warning: session_encode(): Cannot encode non-existent session in %s on line %d
bool(false)
Done
