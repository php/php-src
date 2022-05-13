--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_domain=foo
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_cookie_params() : variation ***\n";

var_dump(ini_get("session.cookie_domain"));
var_dump(session_set_cookie_params(3600, "/path", "blah"));
var_dump(ini_get("session.cookie_domain"));
var_dump(session_start());
var_dump(ini_get("session.cookie_domain"));
var_dump(session_set_cookie_params(3600, "/path", "guff"));
var_dump(ini_get("session.cookie_domain"));
var_dump(session_destroy());
var_dump(ini_get("session.cookie_domain"));
var_dump(session_set_cookie_params(3600, "/path", "foo"));
var_dump(ini_get("session.cookie_domain"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : variation ***
string(3) "foo"
bool(true)
string(4) "blah"
bool(true)
string(4) "blah"

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
string(4) "blah"
bool(true)
string(4) "blah"
bool(true)
string(3) "foo"
Done
