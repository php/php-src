--TEST--
ReflectionProperty::getModifiers includes T_IMMUTABLE on immutable properties.
--FILE--
<?php
class A {
	public immutable $a;
	protected immutable $b;
	private immutable $c;
}
for ($name = 'a'; $name < 'd'; ++$name) {
	$reflector = new ReflectionProperty(A::class, $name);
	$modifiers = $reflector->getModifiers();
	var_dump(($modifiers & ReflectionProperty::IS_IMMUTABLE) === ReflectionProperty::IS_IMMUTABLE);
}
?>
--EXPECT--

bool(true)
bool(true)
bool(true)
