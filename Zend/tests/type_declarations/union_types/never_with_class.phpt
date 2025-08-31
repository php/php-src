--TEST--
Combining never with class type
--FILE--
<?php

function test(): T|never {}

?>
--EXPECTF--
Fatal error: never can only be used as a standalone type in %s on line %d
