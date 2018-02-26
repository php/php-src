--TEST--
ReflectionClass::getModifiers includes T_IMMUTABLE on immutable classes.
--FILE--
<?php
immutable abstract class A {}
immutable class B {}
immutable final class C {}
for ($name = 'A'; $name < 'D'; ++$name) {
	$reflector = new ReflectionClass($name);
	$modifiers = $reflector->getModifiers();
	var_dump(($modifiers & ReflectionClass::IS_IMMUTABLE) === ReflectionClass::IS_IMMUTABLE);
}
?>
--EXPECT--

bool(true)
bool(true)
bool(true)
