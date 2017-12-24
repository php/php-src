--TEST--
nullable scalar static method parameter type constraint
--DESCRIPTION--
Verifies that a static method can have a nullable scalar parameter type
constraint and that it is possible to call it with all compatible types.
--FILE--
<?php

class C {
    static function f(?scalar $scalar) {
        var_dump($scalar);
    }
}

C::f(true);
C::f(false);
C::f(4.2);
C::f(42);
C::f('str');
C::f(null);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
NULL
