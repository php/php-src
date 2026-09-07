--TEST--
session.cookie_lifetime rejects invalid values
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
var_dump(ini_get("session.cookie_lifetime"));

// Non-numeric strings are rejected
ini_set("session.cookie_lifetime", "abc");
var_dump(ini_get("session.cookie_lifetime"));

// Negative values are rejected
ini_set("session.cookie_lifetime", -1);
var_dump(ini_get("session.cookie_lifetime"));

// Negative overflow strings are rejected
ini_set("session.cookie_lifetime", "-99999999999999999999");
var_dump(ini_get("session.cookie_lifetime"));

// Overflow values are rejected
ini_set("session.cookie_lifetime", PHP_INT_MAX);
var_dump(ini_get("session.cookie_lifetime"));

// Valid values still work after rejection
ini_set("session.cookie_lifetime", 200);
var_dump(ini_get("session.cookie_lifetime"));

ob_end_flush();
?>
--EXPECTF--
Warning: ini_set(): session.cookie_lifetime must be of type int in %s on line %d
string(3) "100"

Warning: ini_set(): session.cookie_lifetime must be of type int in %s on line %d
string(3) "100"

Warning: ini_set(): session.cookie_lifetime must be between 0 and %d in %s on line %d
string(3) "100"

Warning: ini_set(): session.cookie_lifetime must be between 0 and %d in %s on line %d
string(3) "100"

Warning: ini_set(): session.cookie_lifetime must be between 0 and %d in %s on line %d
string(3) "100"
string(3) "200"
