--TEST--
Erasure: turbofish (with matching arity) has zero runtime effect
--FILE--
<?php
function f<T>($x) { return $x * 2; }
var_dump(f(5));
var_dump(f::<int>(5));
var_dump(f::<string>(5));
?>
--EXPECT--
int(10)
int(10)
int(10)
