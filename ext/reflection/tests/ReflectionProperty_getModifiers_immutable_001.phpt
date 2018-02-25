--TEST--
ReflectionProperty::getModifiers does not include T_IMMUTABLE on nonimmutable properties.
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
	$modifiers = $reflector->getModifiers();
	var_dump(($modifiers & ReflectionProperty::IS_IMMUTABLE) === ReflectionProperty::IS_IMMUTABLE);
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
