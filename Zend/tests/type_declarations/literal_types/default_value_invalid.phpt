--TEST--
Literal types: a default value not equal to any literal is rejected
--FILE--
<?php
function f(1|2 $x = 3): void {}
?>
--EXPECTF--
Fatal error: Cannot use int as default value for parameter $x of type 1|2 in %s on line %d
