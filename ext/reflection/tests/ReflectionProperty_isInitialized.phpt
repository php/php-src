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

    private int $p;
}

class B extends A { }

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

echo "Visibility handling:\n";
$rp = new ReflectionProperty('A', 'p');
try {
    var_dump($rp->isInitialized($a));
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
$rp->setAccessible(true);
var_dump($rp->isInitialized($a));

echo "Object type:\n";
$rp = new ReflectionProperty('B', 'i');
var_dump($rp->isInitialized($a));

try {
    var_dump($rp->isInitialized(new stdClass));
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump($rp->isInitialized());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

class WithMagic {
    public $prop;
    public int $intProp;

    public function __isset($name) {
        echo "__isset($name)\n";
        return true;
    }

    public function __get($name) {
        echo "__get($name)\n";
        return "foobar";
    }
}

echo "Class with __isset:\n";
$obj = new WithMagic;
unset($obj->prop);
$rp = new ReflectionProperty('WithMagic', 'prop');
var_dump($rp->isInitialized($obj));
$rp = new ReflectionProperty('WithMagic', 'intProp');
var_dump($rp->isInitialized($obj));

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
Visibility handling:
Cannot access non-public property A::$p
bool(false)
Object type:
bool(false)
Given object is not an instance of the class this property was declared in
ReflectionProperty::isInitialized(): Argument #1 ($object) must be provided for instance properties
Class with __isset:
bool(false)
bool(false)
