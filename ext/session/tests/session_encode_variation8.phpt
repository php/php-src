--TEST--
Test session_encode() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=blah
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_encode(void)
 * Description : Encodes the current session data as a string
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_encode() : variation ***\n";

var_dump(session_start());
$_SESSION["foo"] = 1234567890;
$encoded = session_encode();
var_dump(base64_encode($encoded));
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_encode() : variation ***

Warning: session_start(): Cannot find serialization handler 'blah' - session startup failed in %s on line %d
bool(false)

Warning: session_encode(): Cannot encode non-existent session in %s on line %d
string(0) ""

Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d
bool(false)
Done
