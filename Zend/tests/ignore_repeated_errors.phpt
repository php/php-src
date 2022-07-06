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
Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u in %s on line %d

Warning: Undefined variable $u in %s on line %d
