--TEST--
Alternative offset syntax should emit E_COMPILE_ERROR in const expression
--FILE--
<?php
const FOO_COMPILE_ERROR = "BAR"{0};
var_dump(FOO_COMPILE_ERROR);
?>
--EXPECTF--
Parse error: syntax error, unexpected token "{", expecting "," or ";" in %s on line %d
