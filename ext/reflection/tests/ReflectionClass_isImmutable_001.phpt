--TEST--
ReflectionClass::isImmutable does not report nonimmutable classes as immutable.
--FILE--
<?php
abstract class A {}
class B {}
final class C {}
foreach (['A', 'B', 'C'] as $name) {
	$reflector = new ReflectionClass($name);
	var_dump($reflector->isImmutable());
}
?>
--EXPECT--

bool(false)
bool(false)
bool(false)
