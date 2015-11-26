--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_domain=foo
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
NULL
string(4) "blah"
bool(true)
string(4) "blah"
NULL
string(4) "guff"
bool(true)
string(4) "guff"
NULL
string(3) "foo"
Done

