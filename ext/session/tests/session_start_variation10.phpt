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

var_dump(session_start(['lazy_write'=>TRUE]));
$id = session_id();
$_SESSION['abc'] = 123;
var_dump(session_id());
var_dump(session_commit());

// start again.
var_dump(session_id($id));
var_dump(session_start(['lazy_write'=>TRUE]));
var_dump($_SESSION);
var_dump(session_destroy());

// onec again
var_dump(session_id($id));
var_dump(session_start(['lazy_write'=>TRUE]));
var_dump($_SESSION);
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_start() : variation ***
bool(true)
string(26) "%s"
NULL
string(26) "%s"
bool(true)
array(1) {
  ["abc"]=>
  int(123)
}
bool(true)
string(0) ""
bool(true)
array(0) {
}
bool(true)
Done

