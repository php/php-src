--TEST--
Test session_decode() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
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
$_SESSION["bar"] = "Hello World!";
$_SESSION["guff"] = 123.456;
var_dump($_SESSION);
var_dump(session_decode("foo|a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}guff|R:1;blah|R:1;"));
var_dump($_SESSION);
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_decode() : variation ***
bool(true)
array(0) {
}
array(3) {
  ["foo"]=>
  int(1234567890)
  ["bar"]=>
  string(12) "Hello World!"
  ["guff"]=>
  float(123.456)
}
bool(true)
array(4) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["bar"]=>
  string(12) "Hello World!"
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["blah"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
bool(true)
Done

