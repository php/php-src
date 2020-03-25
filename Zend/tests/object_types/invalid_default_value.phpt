--TEST--
Object type can only default to null
--FILE--
<?php

function test(object $obj = 42) { }

?>
--EXPECTF--
Fatal error: Cannot use int as default value for parameter $obj of type object in %s on line %d
