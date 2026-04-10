--TEST--
session.cookie_lifetime rejects non-integer values
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

ini_set("session.cookie_lifetime", 100);

// Float strings are rejected
ini_set("session.cookie_lifetime", "1.5");
var_dump(ini_get("session.cookie_lifetime")); // unchanged

// Non-numeric strings are rejected
ini_set("session.cookie_lifetime", "abc");
var_dump(ini_get("session.cookie_lifetime")); // unchanged

// Negative values are rejected
ini_set("session.cookie_lifetime", -1);
var_dump(ini_get("session.cookie_lifetime")); // unchanged

// Negative overflow strings are rejected
ini_set("session.cookie_lifetime", "-99999999999999999999");
var_dump(ini_get("session.cookie_lifetime")); // unchanged

ob_end_flush();
?>
--EXPECTF--
Warning: ini_set(): CookieLifetime must be an integer in %s on line %d
string(3) "100"

Warning: ini_set(): Invalid value for CookieLifetime in %s on line %d
string(3) "100"

Warning: ini_set(): CookieLifetime cannot be negative in %s on line %d
string(3) "100"

Warning: ini_set(): CookieLifetime cannot be negative in %s on line %d
string(3) "100"
