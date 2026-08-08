--TEST--
GH-10497: Guardrail - constant rebinding still fails
--FILE--
<?php
const OBJ = new stdClass;
OBJ = new stdClass;
?>
--EXPECTF--
Parse error: syntax error, unexpected token "=" in %s on line %d
