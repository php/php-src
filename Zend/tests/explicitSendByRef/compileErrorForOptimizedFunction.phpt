--TEST--
A compilation error is thrown when passing result of optimized function by explicit ref
--FILE--
<?php

function argByRef(&$a) {}

argByRef(&strlen("foo"));

?>
--EXPECTF--
Fatal error: Cannot pass result of by-value function by reference in %s on line %d
