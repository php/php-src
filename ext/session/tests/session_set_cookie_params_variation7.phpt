--TEST--
Test session_set_cookie_params() function : array parameter variation
--INI--
session.cookie_lifetime=0
session.cookie_path="/"
session.cookie_domain=""
session.cookie_secure=0
session.cookie_httponly=0
session.cookie_samesite=""
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : void session_set_cookie_params(array $options)
 * Description : Set the session cookie parameters
 * Source code : ext/session/session.c
 */

echo "*** Testing session_set_cookie_params() : array parameter variation ***\n";

// Invalid cases
var_dump(session_set_cookie_params([]));
var_dump(session_set_cookie_params(["unknown_key" => true, "secure_invalid" => true]));

var_dump(ini_get("session.cookie_secure"));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(["secure" => true, "samesite" => "please"]));
var_dump(ini_get("session.cookie_secure"));
var_dump(ini_get("session.cookie_samesite"));

var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(["lifetime" => 42]));
var_dump(ini_get("session.cookie_lifetime"));

var_dump(ini_get("session.cookie_path"));
var_dump(session_set_cookie_params(["path" => "newpath/"], "arg after options array"));
var_dump(ini_get("session.cookie_path"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : array parameter variation ***

Warning: session_set_cookie_params(): No valid keys were found in the options array in %s
bool(false)

Warning: session_set_cookie_params(): Unrecognized key 'unknown_key' found in the options array in %s

Warning: session_set_cookie_params(): Unrecognized key 'secure_invalid' found in the options array in %s

Warning: session_set_cookie_params(): No valid keys were found in the options array in %s
bool(false)
string(1) "0"
string(0) ""
bool(true)
string(1) "1"
string(6) "please"
string(1) "0"
bool(true)
string(2) "42"
string(1) "/"

Warning: session_set_cookie_params(): Cannot pass arguments after the options array in %s
bool(false)
string(1) "/"
Done
