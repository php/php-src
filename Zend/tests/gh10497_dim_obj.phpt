--TEST--
GH-10497: Dim write on constant object fails
--FILE--
<?php
const OBJ = new stdClass;
OBJ["x"] = 1;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
