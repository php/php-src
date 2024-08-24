--TEST--
Tests passing default as a named argument to a closure that doesn't declare any argument by that name
--FILE--
<?php

$F = fn ($V = 1) => $V;
var_dump($F(default: default + 1));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: Cannot pass default to non-existent named parameter $default of {closure:%s:%d}() in %s:%d
Stack trace:
%a
