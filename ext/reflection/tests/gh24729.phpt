--TEST--
GH-24729 (Use-after-free in ReflectionClass::setStaticPropertyValue() when the old value's destructor unbinds the property)
--FILE--
<?php
class A { public static $x; }
class Old {
    public function __destruct() {
        unset($GLOBALS['y']);
        $z = 1;
        A::$x = &$z;
    }
}
$y = new Old;
A::$x = &$y;
(new ReflectionClass('A'))->setStaticPropertyValue('x', str_repeat('A', 64));
var_dump(A::$x);

class B { public static string $x = ''; }
class S {
    public function __toString(): string {
        unset($GLOBALS['w']);
        $z = 'z';
        B::$x = &$z;
        return str_repeat('B', 8);
    }
}
$w = 'old';
B::$x = &$w;
(new ReflectionClass('B'))->setStaticPropertyValue('x', new S);
var_dump(B::$x);
?>
--EXPECT--
int(1)
string(8) "BBBBBBBB"
