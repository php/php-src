--TEST--
Literal types: literal is redundant when the base scalar type is also present (literal first)
--FILE--
<?php
function f(1|int $x): void {}
?>
--EXPECTF--
Fatal error: Literal type 1 is redundant as the union already allows its base type in %s on line %d
