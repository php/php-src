--TEST--
is_a() with $allow_string false and a string
--FILE--
<?php

class Demo {}

var_dump(is_a(Demo::class, Demo::class, true));
var_dump(is_a(Demo::class, Demo::class, false));

?>
--EXPECTF--
bool(true)

Deprecated: Calling is_a() with a string when $allow_string is false in %s on line %d
bool(false)
