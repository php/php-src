--TEST--
Test session_decode() function : error functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler = wrong_handler
--FILE--
<?php

ob_start();
session_start();
ini_set('session.serialize_handler', 'wrong_handler');
var_dump(session_decode(''));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
Warning: session_start(): Cannot find session serialization handler "wrong_handler" - session startup failed in %s on line 4

Warning: ini_set(): Serialization handler "wrong_handler" cannot be found in %s on line 5

Warning: session_decode(): Session data cannot be decoded when there is no active session in %s on line 6
bool(false)
Done
