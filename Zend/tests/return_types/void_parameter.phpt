--TEST--
void return type: not valid as a parameter type
--FILE--
<?php

function foobar(void $a) {}

?>
--EXPECTF--
Fatal error: foobar(): Parameter #1 ($a) cannot be of type void in %s on line %d
