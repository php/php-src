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

echo "Static properties:\n";
var_dump((new ReflectionProperty(A::class, 'ssv'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 'ss'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 's'))->isInitialized());

echo "Declared properties:\n";
$a = new A;
var_dump((new ReflectionProperty($a, 'iv'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'i'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'n'))->isInitialized($a));

echo "Declared properties after unset:\n";
unset($a->iv);
unset($a->i);
unset($a->n);
var_dump((new ReflectionProperty($a, 'i'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'iv'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'n'))->isInitialized($a));

echo "Dynamic properties:\n";
$a->d = null;
$rp = new ReflectionProperty($a, 'd');
var_dump($rp->isInitialized($a));
unset($a->d);
var_dump($rp->isInitialized($a));

?>
--EXPECT--
Static properties:
bool(true)
bool(false)
bool(true)
Declared properties:
bool(true)
bool(false)
bool(true)
Declared properties after unset:
bool(false)
bool(false)
bool(false)
Dynamic properties:
bool(true)
bool(false)
