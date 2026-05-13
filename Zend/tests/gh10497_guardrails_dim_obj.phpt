--TEST--
GH-10497: Guardrail - dim write on constant object still fails
--FILE--
<?php
const OBJ = new stdClass;
OBJ["x"] = 1;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
