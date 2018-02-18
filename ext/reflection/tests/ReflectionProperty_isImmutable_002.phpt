--TEST--
ReflectionProperty::isImmutable does not report nonimmutable properties as immutable.
--FILE--
<?php
class A {
	public static $a;
	public $b;
	protected static $c;
	protected $d;
	private static $e;
	private $f;
	var $g;
	static $h;
}
for ($name = 'a'; $name < 'i'; ++$name) {
	$reflector = new ReflectionProperty(A::class, $name);
	var_dump($reflector->isImmutable());
}
?>
--EXPECT--

bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
