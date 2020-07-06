--TEST--
Argument default value not legal for any type in the union
--FILE--
<?php

function test(int|float $arg = "0") {
}

?>
--EXPECTF--
Fatal error: test(): Parameter #1 ($arg) of type int|float cannot have a default value of type string in %s on line %d
