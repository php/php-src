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

Warning: session_start(): Unknown session.serialize_handler. Failed to decode session object. in %s on line %d
bool(true)

Warning: session_encode(): Unknown session.serialize_handler. Failed to encode session object. in %s on line %d
string(0) ""
bool(true)
Done

