--TEST--
Literal types: literal is redundant when the base scalar type is also present (scalar first)
--FILE--
<?php
function f(string|'foo' $x): void {}
?>
--EXPECTF--
Fatal error: Literal type 'foo' is redundant as the union already allows its base type in %s on line %d
