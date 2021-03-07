--TEST--
Combining noreturn with class type
--FILE--
<?php

function test(): T|noreturn {}

?>
--EXPECTF--
Fatal error: noreturn can only be used as a standalone type in %s on line %d
