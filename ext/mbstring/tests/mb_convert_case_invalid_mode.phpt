--TEST--
Calling mb_convert_case() with an invalid casing mode
--FILE--
<?php

var_dump(mb_convert_case("foobar", 100));

?>
--EXPECTF--
Warning: mb_convert_case(): Invalid case mode in %s on line %d
bool(false)
