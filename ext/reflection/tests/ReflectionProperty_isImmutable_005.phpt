--TEST--
ReflectionProperty::isImmutable reports properties of immutable class as immutable.
--FILE--
<?php
immutable class A {
	public $a;
	protected $b;
	private $c;
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
