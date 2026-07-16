--TEST--
Literal types: duplicate literal value is redundant
--FILE--
<?php
function f(1|1 $x): void {}
?>
--EXPECTF--
Fatal error: Literal type 1 is redundant as it is already present in the union in %s on line %d
