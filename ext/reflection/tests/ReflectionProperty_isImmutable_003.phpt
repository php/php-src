--TEST--
ReflectionProperty::isImmutable reports immutable properties as such.
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
	var_dump($reflector->isImmutable());
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
