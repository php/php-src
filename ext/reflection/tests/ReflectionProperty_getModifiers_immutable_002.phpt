--TEST--
ReflectionProperty::getModifiers includes T_IMMUTABLE on immutable properties.
--FILE--
<?php
class A {
	public static immutable $a;
	public immutable $b;
	protected static immutable $c;
	protected immutable $d;
	private static immutable $e;
	private immutable $f;
	var immutable $g;
	static immutable $h;
}
for ($name = 'a'; $name < 'i'; ++$name) {
	$reflector = new ReflectionProperty(A::class, $name);
	$modifiers = $reflector->getModifiers();
	var_dump(($modifiers & T_IMMUTABLE) === T_IMMUTABLE);
}
?>
--EXPECT--

bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
