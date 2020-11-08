--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_path=/path
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_cookie_params() : variation ***\n";

var_dump(ini_get("session.cookie_path"));
var_dump(session_set_cookie_params(3600, "/foo"));
var_dump(ini_get("session.cookie_path"));
var_dump(session_start());
var_dump(ini_get("session.cookie_path"));
var_dump(session_set_cookie_params(3600, "/blah"));
var_dump(ini_get("session.cookie_path"));
var_dump(session_destroy());
var_dump(ini_get("session.cookie_path"));
var_dump(session_set_cookie_params(3600, "/guff"));
var_dump(ini_get("session.cookie_path"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : variation ***
string(5) "/path"
bool(true)
string(4) "/foo"
bool(true)
string(4) "/foo"

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
string(4) "/foo"
bool(true)
string(4) "/foo"
bool(true)
string(5) "/guff"
Done
