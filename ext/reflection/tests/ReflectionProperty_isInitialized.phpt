--TEST--
Test ReflectionProperty::isInitialized()
--FILE--
<?php

class A {
	public static ?string $ssv = null;
	public static ?string $ss;
	public static $s;
	public ?int $iv = null;
	public ?int $i;
	public $n;
}

var_dump((new ReflectionProperty(A::class, 'ssv'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 'ss'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 's'))->isInitialized());
$a = new A;
$a->d = null;
var_dump((new ReflectionProperty($a, 'iv'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'i'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'n'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'd'))->isInitialized($a));

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)

