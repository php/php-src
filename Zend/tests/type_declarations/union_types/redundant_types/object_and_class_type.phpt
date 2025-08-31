--TEST--
Using both object and a class type
--FILE--
<?php

function test(): object|Test {
}

?>
--EXPECTF--
Fatal error: Type Test|object contains both object and a class type, which is redundant in %s on line %d
