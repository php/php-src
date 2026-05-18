--TEST--
Reflection: ReflectionClass::isGeneric() returns false for non-generic class
--FILE--
<?php
class P {}
var_dump((new ReflectionClass('P'))->isGeneric());
?>
--EXPECT--
bool(false)
