--TEST--
Errors: top-level self-reference inside a union bound is rejected
--FILE--
<?php
function f<T : T|int>(): void {}
?>
--EXPECTF--
Fatal error: Type parameter T cannot reference itself in its own bound or default outside of a generic type argument in %s on line %d
