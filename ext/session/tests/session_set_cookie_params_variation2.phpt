--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_path=/path
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : void session_set_cookie_params(int $lifetime [, string $path [, string $domain [, bool $secure [, bool $httponly]]]])
 * Description : Set the session cookie parameters
 * Source code : ext/session/session.c
 */

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

Warning: session_set_cookie_params(): Cannot change session cookie parameters when session is active in %s on line 18
bool(false)
string(4) "/foo"
bool(true)
string(4) "/foo"
bool(true)
string(5) "/guff"
Done
