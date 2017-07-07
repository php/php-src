--TEST--
Test session_set_cookie_params() function : variation
--INI--
session.cookie_samesite=test
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : void session_set_cookie_params(int $lifetime [, string $path [, string $domain [, bool $secure [, bool $samesite[, string $samesite]]]]])
 * Description : Set the session cookie parameters
 * Source code : ext/session/session.c
 */

echo "*** Testing session_set_cookie_params() : variation ***\n";

var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, FALSE, "nothing"));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_start());
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, TRUE, "test"));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_destroy());
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, FALSE, "other"));
var_dump(ini_get("session.cookie_samesite"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : variation ***
string(4) "test"
bool(true)
string(7) "nothing"
bool(true)
string(7) "nothing"

Warning: session_set_cookie_params(): Cannot change session cookie parameters when session is active in %s on line 18
bool(false)
string(7) "nothing"
bool(true)
string(7) "nothing"
bool(true)
string(5) "other"
Done
