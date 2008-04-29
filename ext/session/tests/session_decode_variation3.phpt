--TEST--
Test session_decode() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=blah
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_decode(void)
 * Description : Decodes session data from a string
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_decode() : variation ***\n";

var_dump(session_start());
var_dump($_SESSION);
$_SESSION["foo"] = 1234567890;
$_SESSION["bar"] = "Blah!";
$_SESSION["guff"] = 123.456;
var_dump($_SESSION);
$encoded = "A2Zvb2k6MTIzNDU2Nzg5MDs=";
var_dump(session_decode(base64_decode($encoded)));
var_dump($_SESSION);
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_decode() : variation ***

Warning: session_start(): Unknown session.serialize_handler. Failed to decode session object. in %s on line %d
bool(true)
array(0) {
}
array(3) {
  ["foo"]=>
  int(1234567890)
  ["bar"]=>
  string(5) "Blah!"
  ["guff"]=>
  float(123.456)
}

Warning: session_decode(): Unknown session.serialize_handler. Failed to decode session object. in %s on line %d
bool(true)
array(3) {
  ["foo"]=>
  int(1234567890)
  ["bar"]=>
  string(5) "Blah!"
  ["guff"]=>
  float(123.456)
}
bool(true)
Done

