--TEST--
Test session_commit() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.auto_start=1
--FILE--
<?php

ob_start();

echo "*** Testing session_commit() : variation ***\n";

var_dump($_SESSION);
var_dump(session_commit());
var_dump($_SESSION);
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_commit() : variation ***
array(0) {
}
bool(true)
array(0) {
}
bool(true)
bool(true)
Done
