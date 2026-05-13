--TEST--
GH-10497: Guardrail - array dim write on constant still fails
--FILE--
<?php
const ARR = [1, 2, 3];
ARR[0] = 9;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
