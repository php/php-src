--TEST--
New with anonymous class is not supported in constant expressions
--FILE--
<?php

static $x = new class {};

?>
--EXPECTF--
Fatal error: Cannot use anonymous class in constant expression in %s on line %d
