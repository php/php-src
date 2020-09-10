--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_lifetime=3600
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_cookie_params() : variation ***\n";

var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(3600));
var_dump(ini_get("session.cookie_lifetime"));

var_dump(session_start());
var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(1800));
var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_destroy());

var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(1234567890));
var_dump(ini_get("session.cookie_lifetime"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : variation ***
string(4) "3600"
bool(true)
string(4) "3600"
bool(true)
string(4) "3600"

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
string(4) "3600"
bool(true)
string(4) "3600"
bool(true)
string(10) "1234567890"
Done
