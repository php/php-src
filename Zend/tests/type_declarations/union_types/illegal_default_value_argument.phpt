--TEST--
Argument default value not legal for any type in the union
--FILE--
<?php

function test(int|float $arg = "0") {
}

?>
--EXPECTF--
Fatal error: Cannot use string as default value for parameter $arg of type int|float in %s on line %d
