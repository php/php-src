--TEST--
Reflection and property_exists()
--FILE--
<?php

#[AllowDynamicProperties]
class A
{
    public    $a = 1;
    protected $b = 2;
    private   $c = 3;

    public    $empty;
    public    $init = 1;

    function __toString()
    {
        return 'obj(' . get_class($this) . ')';
    }

    static function test($oc, $props)
    {
        echo '===' . __CLASS__ . "===\n";
        foreach($props as $p2) {
            echo $oc, '::$' , $p2, "\n";
            var_dump(property_exists($oc, $p2));
        }
    }
}

class B extends A
{
    private   $c = 4;

    static function test($oc, $props)
    {
        echo '===' . __CLASS__ . "===\n";
        foreach($props as $p2) {
            echo $oc, '::$' , $p2, "\n";
            var_dump(property_exists($oc, $p2));
        }
    }
}

class C extends B
{
    private   $d = 5;

    static function test($oc, $props)
    {
        echo '===' . __CLASS__ . "===\n";
        foreach($props as $p2) {
            echo $oc, '::$' , $p2, "\n";
            var_dump(property_exists($oc, $p2));
        }
    }
}

$oA = new A;
$oA->e = 6;

$oC = new C;

$pc = array($oA, 'A', 'B', 'C', $oC);
$pr = array('a', 'b', 'c', 'd', 'e');

foreach($pc as $p1) {
    if (is_object($p1)) {
        $p1->test($p1, $pr);
    } else {
        $r = new ReflectionMethod($p1, 'test');
        $r->invoke(NULL, $p1, $pr);
    }
    echo "===GLOBAL===\n";
    foreach($pr as $p2) {
        echo $p1, '::$' , $p2, "\n";
        var_dump(property_exists($p1, $p2));
    }
}

echo "===PROBLEMS===\n";
try {
    var_dump(property_exists(NULL, 'empty'));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(property_exists(25,'empty'));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(property_exists('',''));
var_dump(property_exists('A',''));
var_dump(property_exists('A','123'));
var_dump(property_exists('A','init'));
var_dump(property_exists('A','empty'));
var_dump(property_exists(new A, ''));
var_dump(property_exists(new A, '123'));
var_dump(property_exists(new A, 'init'));
var_dump(property_exists(new A, 'empty'));
?>
--EXPECT--
===A===
obj(A)::$a
bool(true)
obj(A)::$b
bool(true)
obj(A)::$c
bool(true)
obj(A)::$d
bool(false)
obj(A)::$e
bool(true)
===GLOBAL===
obj(A)::$a
bool(true)
obj(A)::$b
bool(true)
obj(A)::$c
bool(true)
obj(A)::$d
bool(false)
obj(A)::$e
bool(true)
===A===
A::$a
bool(true)
A::$b
bool(true)
A::$c
bool(true)
A::$d
bool(false)
A::$e
bool(false)
===GLOBAL===
A::$a
bool(true)
A::$b
bool(true)
A::$c
bool(true)
A::$d
bool(false)
A::$e
bool(false)
===B===
B::$a
bool(true)
B::$b
bool(true)
B::$c
bool(true)
B::$d
bool(false)
B::$e
bool(false)
===GLOBAL===
B::$a
bool(true)
B::$b
bool(true)
B::$c
bool(true)
B::$d
bool(false)
B::$e
bool(false)
===C===
C::$a
bool(true)
C::$b
bool(true)
C::$c
bool(false)
C::$d
bool(true)
C::$e
bool(false)
===GLOBAL===
C::$a
bool(true)
C::$b
bool(true)
C::$c
bool(false)
C::$d
bool(true)
C::$e
bool(false)
===C===
obj(C)::$a
bool(true)
obj(C)::$b
bool(true)
obj(C)::$c
bool(false)
obj(C)::$d
bool(true)
obj(C)::$e
bool(false)
===GLOBAL===
obj(C)::$a
bool(true)
obj(C)::$b
bool(true)
obj(C)::$c
bool(false)
obj(C)::$d
bool(true)
obj(C)::$e
bool(false)
===PROBLEMS===
property_exists(): Argument #1 ($object_or_class) must be of type object|string, null given
property_exists(): Argument #1 ($object_or_class) must be of type object|string, int given
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
