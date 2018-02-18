--TEST--
ReflectionClass::isImmutable reports immutable classes as such.
--FILE--
<?php
immutable abstract class A {}
immutable class B {}
immutable final class C {}
foreach (['A', 'B', 'C'] as $name) {
	$reflector = new ReflectionClass($name);
	var_dump($reflector->isImmutable());
}
?>
--EXPECT--

bool(true)
bool(true)
bool(true)
