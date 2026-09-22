--TEST--
Closure 029: Testing lambda with instanceof operator
--FILE--
<?php

var_dump(function() { } instanceof closure);
var_dump(function(&$x) { } instanceof closure);
var_dump(@function(&$x) use ($y, $z) { } instanceof closure);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
