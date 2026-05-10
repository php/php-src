--TEST--
Reflection: ReflectionClass::isGeneric() returns true for generic class
--FILE--
<?php
class G<T> {}
var_dump((new ReflectionClass('G'))->isGeneric());
?>
--EXPECT--
bool(true)
