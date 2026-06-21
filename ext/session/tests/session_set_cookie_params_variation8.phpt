--TEST--
Test session_set_cookie_params() function : negative and 0 lifetime
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(["lifetime" => 0]));
var_dump(ini_get("session.cookie_lifetime"));

var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(["lifetime" => -10]));
var_dump(ini_get("session.cookie_lifetime"));
echo "Done";
ob_end_flush();
?>
--EXPECTF--
string(1) "0"
bool(true)
string(1) "0"
string(1) "0"

Warning: session_set_cookie_params(): session.cookie_lifetime must be between 0 and %d in %s on line %d
bool(false)
string(1) "0"
Done
