--TEST--
Errors: bound `T : T` (top-level self-reference) is rejected
--FILE--
<?php
function f<T : T>(): void {}
?>
--EXPECTF--
Fatal error: Type parameter T cannot reference itself in its own bound or default outside of a generic type argument in %s on line %d
