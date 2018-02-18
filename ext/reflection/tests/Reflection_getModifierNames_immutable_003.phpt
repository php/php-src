--TEST--
Reflection::getModifierNames includes the immutable modifier on immutable classes.
--FILE--
<?php
immutable abstract class A {}
immutable class B {}
immutable final class C {}
foreach (['A', 'B', 'C'] as $name) {
	$reflector = new ReflectionClass($name);
	var_dump(in_array('immutable', Reflection::getModifierNames($reflector->getModifiers())));
}
?>
--EXPECT--

bool(true)
bool(true)
bool(true)
