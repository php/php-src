--TEST--
nullable scalar static method return type constraint
--DESCRIPTION--
Verifies that a static method can have a nullable scalar return type
constraint and that it is possible to return all compatible types.
--FILE--
<?php

class C {
    static function f($p): ?scalar {
        return $p;
    }
}

var_dump(
    C::f(true),
    C::f(false),
    C::f(4.2),
    C::f(42),
    C::f('str'),
    C::f(null)
);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
NULL
