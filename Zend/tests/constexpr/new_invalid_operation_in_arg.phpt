--TEST--
Invalid operation in new arg in const expr
--FILE--
<?php

static $x = new stdClass($foo);

?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s on line %d
