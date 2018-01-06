--TEST--
Object type can only default to null
--FILE--
<?php

function test(object $obj = 42) { }

?>
--EXPECTF--
Fatal error: Default value for parameters with an object type can only be NULL in %s on line %d
