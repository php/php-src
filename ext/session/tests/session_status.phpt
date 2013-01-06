--TEST--
Test session_status() function : active, none
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_status() : active, none\n";

var_dump(PHP_SESSION_NONE != PHP_SESSION_ACTIVE);
var_dump(session_status() == PHP_SESSION_NONE);

session_start();

var_dump(session_status() == PHP_SESSION_ACTIVE);

?>
--EXPECTF--
*** Testing session_status() : active, none
bool(true)
bool(true)
bool(true)
