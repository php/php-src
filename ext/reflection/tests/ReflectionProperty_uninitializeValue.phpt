--TEST--
Test ReflectionProperty::uninitializeValue()
--FILE--
<?php

class A {
    public static ?string $ssv = null;
    public static string $ssv2 = 'foo';
    public static ?string $ss;
    public static $s;

    public ?int $iv = null;
    public int $iv2 = 5;
    public ?int $i;
    public $n;
}

class B extends A {
    public int $ivb = 6;

    public function __get(string $n): void
    {
        var_dump('__get: ' . $n);
    }
}

function dumpProperty($obj, string $name) {
    try {
        var_dump($obj->{$name});
    } catch (\Error $e) {
        echo $e->getMessage() . "\n";
    }
}

echo "Static properties:\n";
(new ReflectionProperty(A::class, 'ssv'))->uninitializeValue();
(new ReflectionProperty(A::class, 'ssv2'))->uninitializeValue();
(new ReflectionProperty(A::class, 'ss'))->uninitializeValue();
(new ReflectionProperty(A::class, 's'))->uninitializeValue();
var_dump((new ReflectionProperty(A::class, 'ssv'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 'ssv2'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 'ss'))->isInitialized());
var_dump((new ReflectionProperty(A::class, 's'))->isInitialized());

(new ReflectionProperty(B::class, 'ssv'))->uninitializeValue();
(new ReflectionProperty(B::class, 'ssv2'))->uninitializeValue();
(new ReflectionProperty(B::class, 'ss'))->uninitializeValue();
(new ReflectionProperty(B::class, 's'))->uninitializeValue();
var_dump((new ReflectionProperty(B::class, 'ssv'))->isInitialized());
var_dump((new ReflectionProperty(B::class, 'ssv2'))->isInitialized());
var_dump((new ReflectionProperty(B::class, 'ss'))->isInitialized());
var_dump((new ReflectionProperty(B::class, 's'))->isInitialized());

echo "Declared properties:\n";
$a = new A();
dumpProperty($a, 'iv');
dumpProperty($a, 'iv2');
dumpProperty($a, 'i');
dumpProperty($a, 'n');
(new ReflectionProperty($a, 'iv'))->uninitializeValue($a);
(new ReflectionProperty($a, 'iv2'))->uninitializeValue($a);
(new ReflectionProperty($a, 'i'))->uninitializeValue($a);
(new ReflectionProperty($a, 'n'))->uninitializeValue($a);
var_dump((new ReflectionProperty($a, 'iv'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'iv2'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'i'))->isInitialized($a));
var_dump((new ReflectionProperty($a, 'n'))->isInitialized($a));
dumpProperty($a, 'iv');
dumpProperty($a, 'iv2');
dumpProperty($a, 'i');
dumpProperty($a, 'n');

$b = new B();
dumpProperty($b, 'iv');
dumpProperty($b, 'iv2');
dumpProperty($b, 'i');
dumpProperty($b, 'n');
dumpProperty($b, 'ivb');
(new ReflectionProperty($b, 'iv'))->uninitializeValue($b);
(new ReflectionProperty($b, 'iv2'))->uninitializeValue($b);
(new ReflectionProperty($b, 'i'))->uninitializeValue($b);
(new ReflectionProperty($b, 'n'))->uninitializeValue($b);
(new ReflectionProperty($b, 'ivb'))->uninitializeValue($b);
var_dump((new ReflectionProperty($b, 'iv'))->isInitialized($b));
var_dump((new ReflectionProperty($b, 'iv2'))->isInitialized($b));
var_dump((new ReflectionProperty($b, 'i'))->isInitialized($b));
var_dump((new ReflectionProperty($b, 'n'))->isInitialized($b));
var_dump((new ReflectionProperty($b, 'iv2'))->isInitialized($b));
dumpProperty($b, 'iv');
dumpProperty($b, 'iv2');
dumpProperty($b, 'i');
dumpProperty($b, 'n');
dumpProperty($b, 'ivb');

echo "Dynamic properties:\n";
$a->d = 'da';
dumpProperty($a, 'd');
$rp = new ReflectionProperty($a, 'd');
var_dump($rp->isInitialized($a));
(new ReflectionProperty($a, 'd'))->uninitializeValue($a);
var_dump($rp->isInitialized($a));
dumpProperty($a, 'd');

$b->d = 'db';
dumpProperty($b, 'd');
$rp = new ReflectionProperty($b, 'd');
var_dump($rp->isInitialized($b));
(new ReflectionProperty($b, 'd'))->uninitializeValue($b);
var_dump($rp->isInitialized($b));
dumpProperty($b, 'd');

?>
--EXPECT--
Static properties:
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
Declared properties:
NULL
int(5)
Typed property A::$i must not be accessed before initialization
NULL
bool(false)
bool(false)
bool(false)
bool(true)
Typed property A::$iv must not be accessed before initialization
Typed property A::$iv2 must not be accessed before initialization
Typed property A::$i must not be accessed before initialization
NULL
NULL
int(5)
Typed property A::$i must not be accessed before initialization
NULL
int(6)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
Typed property A::$iv must not be accessed before initialization
Typed property A::$iv2 must not be accessed before initialization
Typed property A::$i must not be accessed before initialization
NULL
Typed property B::$ivb must not be accessed before initialization
Dynamic properties:
string(2) "da"
bool(true)
bool(true)
NULL
string(2) "db"
bool(true)
bool(true)
NULL
