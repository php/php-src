--TEST--
Object type can only default to null
--FILE--
<?php

function test(object $obj = 42) { }

?>
--EXPECTF--
Fatal error: test(): Parameter #1 ($obj) of type object cannot have a default value of type int in %s on line %d
