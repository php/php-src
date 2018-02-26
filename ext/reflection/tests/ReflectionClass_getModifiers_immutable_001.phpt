--TEST--
ReflectionClass::getModifiers does not include T_IMMUTABLE on nonimmutable classes.
--FILE--
<?php
abstract class A {}
class B {}
final class C {}
for ($name = 'A'; $name < 'D'; ++$name) {
	$reflector = new ReflectionClass($name);
	$modifiers = $reflector->getModifiers();
	var_dump(($modifiers & ReflectionClass::IS_IMMUTABLE) === ReflectionClass::IS_IMMUTABLE);
}
?>
--EXPECT--

bool(false)
bool(false)
bool(false)
