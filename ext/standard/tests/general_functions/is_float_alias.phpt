--TEST--
Test deprecated is_double aliases for is_float()

--FILE--
<?php

var_dump(is_double("hello") === is_float("hello"));

?>
--EXPECTF--
Deprecated: Function is_double() is deprecated since 8.6, use is_float() instead in %s on line %d
bool(true)
