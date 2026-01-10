--TEST--
Bug #69676: Resolution of self::FOO in class constants not correct (variation)
--FILE--
<?php

class P {
    const N = 'P';
}
class A extends P {
    const selfN = self::N;
    const parentN = parent::N;
    const N = 'A';
}
class B extends A {
    const N = 'B';
}

var_dump(B::selfN); // A
var_dump(B::parentN); // P

class A2 {
    const selfN = self::N;
    const N = 'A2';
}
class B2 extends A2 {
    const indSelfN = self::selfN;
    const N = 'B2';
}
class C2 extends B2 {
    const N = 'C2';
}

var_dump(C2::indSelfN); // A2

class A3 {
    const selfN = self::N;
    const N = 'A3';
}
class B3 extends A3 {
    const exprSelfN = "expr" . self::selfN;
    const N = 'B3';
}
class C3 extends B3 {
    const N = 'C3';
}

var_dump(C3::exprSelfN); // exprA3

class A4 {
    const selfN = self::N;
    const N = 'A4';
}
class B4 extends A4 {
    const N = 'B4';
    public $prop = self::selfN;
}
class C4 extends B4 {
    const N = 'C4';
}

var_dump((new C4)->prop); // A4

?>
--EXPECT--
string(1) "A"
string(1) "P"
string(2) "A2"
string(6) "exprA3"
string(2) "A4"
