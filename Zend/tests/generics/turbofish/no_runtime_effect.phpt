--TEST--
Turbofish: with matching arity has zero runtime effect on values
--FILE--
<?php
function f<T>(int $x): int { return $x * 2; }
var_dump(f(5));
var_dump(f::<int>(5));
var_dump(f::<string>(5));
var_dump(f::<mixed>(5));
?>
--EXPECT--
int(10)
int(10)
int(10)
int(10)
