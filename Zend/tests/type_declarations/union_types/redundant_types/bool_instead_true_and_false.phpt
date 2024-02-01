--TEST--
Using both true and false in a union instead of bool
--FILE--
<?php

function test(): true|false {
}

?>
--EXPECTF--
Fatal error: Type contains both true and false, bool should be used instead in %s on line %d
