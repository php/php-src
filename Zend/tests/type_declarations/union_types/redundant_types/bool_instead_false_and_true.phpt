--TEST--
Using both false and true in a union instead of bool
--FILE--
<?php

function test(): false|true {
}

?>
--EXPECTF--
Fatal error: Type contains both true and false, bool should be used instead in %s on line %d
