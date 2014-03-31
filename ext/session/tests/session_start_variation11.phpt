--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.auto_start=0
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_start(void)
 * Description : Initialize session data
 * Source code : ext/session/session.c
 */

echo "*** Testing session_start() : variation ***\n";

session_id('test');
var_dump(session_start(['read_and_close'=>FALSE]));
var_dump(session_status());
$_SESSION['abc'] = 123;
var_dump(session_commit());

var_dump(session_start(['read_and_close'=>TRUE]));
var_dump(session_status());
var_dump($_SESSION);
$_SESSION['abc'] = 456;
var_dump(session_commit());

var_dump(session_start(['read_and_close'=>FALSE]));
var_dump(session_status());
var_dump($_SESSION);
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_start() : variation ***
bool(true)
int(2)
NULL
bool(true)
int(1)
array(1) {
  ["abc"]=>
  int(123)
}
NULL
bool(true)
int(2)
array(1) {
  ["abc"]=>
  int(123)
}
bool(true)
Done
