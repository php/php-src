--TEST--
Combining void with class type
--FILE--
<?php

function test(): T|void {}

?>
--EXPECTF--
Fatal error: Void can only be used as a standalone type in %s on line %d
