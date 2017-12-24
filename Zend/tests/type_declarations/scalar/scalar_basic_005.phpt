--TEST--
scalar instance method parameter type constraint
--DESCRIPTION--
Verifies that an instance method can have a scalar parameter type constraint
and that it is possible to call it with all compatible types.
--FILE--
<?php

class C {
    function f(scalar $scalar) {
        var_dump($scalar);
    }
}

$c = new C;

$c->f(true);
$c->f(false);
$c->f(4.2);
$c->f(42);
$c->f('str');

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
