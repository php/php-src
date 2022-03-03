--TEST--
Class constant inheritance with mutable data
--SKIPIF--
<?php
if (getenv('SKIP_PRELOAD')) die('skip preload produces additional warnings');
?>
--FILE--
<?php

// This would previously leak under opcache.
class A {
    const X = 'X' . self::Y;
    const Y = 'Y';
}
interface I {
    const X2 = 'X2' . self::Y2;
    const Y2 = 'Y2';
}
eval('class B extends A implements I {}');
var_dump(new B);
var_dump(B::X, B::X2);

// This should only produce one warning, not two.
class X {
    const C = 1 % 1.5;
}
class Y extends X {
}
var_dump(X::C, Y::C);
?>
--EXPECTF--
object(B)#1 (0) {
}
string(2) "XY"
string(4) "X2Y2"

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(0)
int(0)
