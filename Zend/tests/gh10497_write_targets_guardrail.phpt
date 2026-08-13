--TEST--
GH-10497: Guardrail - destructuring into a constant that is not reached via a property
--FILE--
<?php
const ARR = [1, 2, 3];
[ARR[0]] = [9];
?>
--EXPECTF--
Fatal error: Assignments can only happen to writable values in %s on line %d
