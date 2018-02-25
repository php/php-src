--TEST--
ReflectionProperty::isImmutable reports immutable properties as such.
--FILE--
<?php
class A {
	public immutable $a;
	protected immutable $b;
	private immutable $c;
}
for ($name = 'a'; $name < 'd'; ++$name) {
	$reflector = new ReflectionProperty(A::class, $name);
	var_dump($reflector->isImmutable());
}
?>
--EXPECT--

bool(true)
bool(true)
bool(true)
