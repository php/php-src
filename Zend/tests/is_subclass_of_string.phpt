--TEST--
is_subclass_of() with $allow_string false and a string
--FILE--
<?php

class Demo {}

class Child extends Demo {}

var_dump(is_subclass_of(Child::class, Demo::class, true));
var_dump(is_subclass_of(Child::class, Demo::class, false));

?>
--EXPECTF--
bool(true)

Deprecated: Calling is_subclass_of() with a string when $allow_string is false in %s on line %d
bool(false)
