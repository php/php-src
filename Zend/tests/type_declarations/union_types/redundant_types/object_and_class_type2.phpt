--TEST--
Using both object and a class type 2
--FILE--
<?php

function test(): Test|object {
}

?>
--EXPECTF--
Fatal error: Type Test|object contains both object and a class type, which is redundant in %s on line %d
