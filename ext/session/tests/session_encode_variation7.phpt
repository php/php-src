--TEST--
Test session_encode() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=php_binary
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
var_dump($encoded);
var_dump(base64_encode($encoded));
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_encode() : variation ***
bool(true)
string(37) "__SESS_N_VARS__i:1;fooi:1234567890;"
string(52) "D19fU0VTU19OX1ZBUlNfX2k6MTsDZm9vaToxMjM0NTY3ODkwOw=="
bool(true)
Done
