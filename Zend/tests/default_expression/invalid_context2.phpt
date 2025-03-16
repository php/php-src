--TEST--
Tests specifying the default keyword in an invalid context throws a compile error (2)
--FILE--
<?php

function F($V = 2) { return $V; }
F(fn () => default)();
?>
--EXPECTF--
Fatal error: Cannot use default in non-argument context. in %s on line %d
