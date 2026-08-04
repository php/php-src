--TEST--
Test session_set_cookie_params() samesite validation
--INI--
session.cookie_samesite=Lax
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "-- Valid values --\n";
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(["samesite" => "Strict"]));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(["samesite" => "None"]));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(["samesite" => ""]));
var_dump(ini_get("session.cookie_samesite"));

echo "-- Invalid value via session_set_cookie_params --\n";
var_dump(session_set_cookie_params(["samesite" => "Invalid"]));
var_dump(ini_get("session.cookie_samesite"));

echo "-- Invalid value via ini_set --\n";
var_dump(ini_set("session.cookie_samesite", "Invalid"));
var_dump(ini_get("session.cookie_samesite"));

echo "-- Cannot change while session is active --\n";
var_dump(session_set_cookie_params(["samesite" => "Lax"]));
var_dump(session_start());
var_dump(session_set_cookie_params(["samesite" => "Strict"]));
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
-- Valid values --
string(3) "Lax"
bool(true)
string(6) "Strict"
bool(true)
string(4) "None"
bool(true)
string(0) ""
-- Invalid value via session_set_cookie_params --

Warning: session_set_cookie_params(): session.cookie_samesite must be "Strict", "Lax", "None", or "" in %s on line %d
bool(false)
string(0) ""
-- Invalid value via ini_set --

Warning: ini_set(): session.cookie_samesite must be "Strict", "Lax", "None", or "" in %s on line %d
bool(false)
string(0) ""
-- Cannot change while session is active --
bool(true)
bool(true)

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active (started from %s on line %d) in %s on line %d
bool(false)
bool(true)
Done
