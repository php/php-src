--TEST--
session.cookie_lifetime overflow value is clamped
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

// Set a valid value first
ini_set("session.cookie_lifetime", 100);
var_dump(ini_get("session.cookie_lifetime"));

// Set an overflow value - should succeed with warning, value clamped
ini_set("session.cookie_lifetime", PHP_INT_MAX);
$val = (int) ini_get("session.cookie_lifetime");
var_dump($val < PHP_INT_MAX); // clamped, not PHP_INT_MAX
var_dump($val > 0); // positive

// Valid values still work after clamping
ini_set("session.cookie_lifetime", 200);
var_dump(ini_get("session.cookie_lifetime"));

ob_end_flush();
?>
--EXPECTF--
string(3) "100"

Warning: ini_set(): CookieLifetime value too large, value was set to the maximum of %d in %s on line %d
bool(true)
bool(true)
string(3) "200"
