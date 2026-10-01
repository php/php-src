--TEST--
PFA default argument value scope
--ENV--
A=1
--FILE--
<?php

if (getenv('A')) {
    /* Relative class references are never resolved at compile time on traits */
    trait T {
        static function f($a, $b = self::VAL) {
            var_dump($b);
        }
    }
    trait U {
        static function g($a, $b = parent::VAL) {
            var_dump($b);
        }
    }
}

class C {
    const VAL = 'C';
    use T;
}

class D extends C {
    const VAL = 'D';
    use U;
}

C::f(0);
D::f(0);
C::f(?, ...)(0);
D::f(?, ...)(0);

D::g(0);
D::g(?, ...)(0);

?>
--EXPECT--
string(1) "C"
string(1) "C"
string(1) "C"
string(1) "C"
string(1) "C"
string(1) "C"
