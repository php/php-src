--TEST--
Typed local variables: using a typed local as a foreach by-reference value is a compile error
--FILE--
<?php
int $x = 1;
foreach ([1, 2, 3] as &$x) {
}
?>
--EXPECTF--
Fatal error: Cannot use typed local variable $x as foreach by-reference value in %s on line %d
