--TEST--
Alternative offset syntax should emit E_COMPILE_ERROR in const expression
--FILE--
<?php
const FOO_COMPILE_ERROR = "BAR"{0};
var_dump(FOO_COMPILE_ERROR);
?>
--EXPECTF--
Fatal error: Array and string offset access syntax with curly braces is no longer supported in %s on line 2
