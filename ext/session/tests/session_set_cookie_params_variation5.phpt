--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_httponly=TRUE
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_cookie_params() : variation ***\n";

var_dump(ini_get("session.cookie_httponly"));
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, FALSE));
var_dump(ini_get("session.cookie_httponly"));
var_dump(session_start());
var_dump(ini_get("session.cookie_httponly"));
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, TRUE));
var_dump(ini_get("session.cookie_httponly"));
var_dump(session_destroy());
var_dump(ini_get("session.cookie_httponly"));
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, FALSE));
var_dump(ini_get("session.cookie_httponly"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : variation ***
string(1) "1"
bool(true)
string(1) "0"
bool(true)
string(1) "0"

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
string(1) "0"
bool(true)
string(1) "0"
bool(true)
string(1) "0"
Done
