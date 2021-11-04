--TEST--
Test ignore_repeated_source ini setting
--INI--
ignore_repeated_errors=1
ignore_repeated_source=1
--FILE--
<?php

// Not a repeated error due to different variables names.
$u1 + $u2;

// Repeated error.
$u + $u;

// Also a repeated error, because we're ignoring the different source.
$u + 1;

?>
--EXPECTF--
Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u in %s on line %d
