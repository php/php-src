--TEST--
Test deprecated is_long/is_integer aliases for is_int()

--FILE--
<?php

var_dump(is_long("hello") === is_int("hello"));
var_dump(is_integer("hello") === is_int("hello"));

?>
--EXPECTF--
Deprecated: Function is_long() is deprecated since 8.6, use is_int() instead in %s on line %d
bool(true)

Deprecated: Function is_integer() is deprecated since 8.6, use is_int() instead in %s on line %d
bool(true)
