--TEST--
Test ignore_repeated_errors ini setting
--INI--
ignore_repeated_errors=1
--FILE--
<?php

// Not a repeated error due to different variables names.
$u1 + $u2;

// Repeated error.
$u + $u;

// Not a repeated error, because the line is different.
$u + 1;

?>
--EXPECTF--
Warning: Undefined variable $u1 (This will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $u2 (This will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $u (This will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $u (This will become an error in PHP 9.0) in %s on line %d
