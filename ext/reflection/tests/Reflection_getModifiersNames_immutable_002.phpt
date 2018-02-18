--TEST--
Reflection::getModifierNames does not include the immutable modifier on nonimmutable classes.
--FILE--
<?php
abstract class A {}
class B {}
final class C {}
foreach (['A', 'B', 'C'] as $name) {
	$reflector = new ReflectionClass($name);
	var_dump(in_array('immutable', Reflection::getModifierNames($reflector->getModifiers())));
}
?>
--EXPECT--

bool(false)
bool(false)
bool(false)
