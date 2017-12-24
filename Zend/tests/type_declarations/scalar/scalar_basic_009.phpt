--TEST--
nullable scalar function parameter type constraint
--DESCRIPTION--
Verifies that a function can have a nullable scalar parameter type constraint
and that it is possible to call it with all compatible types.
--FILE--
<?php

function f(?scalar $scalar) {
    var_dump($scalar);
}

f(true);
f(false);
f(4.2);
f(42);
f('str');
f(null);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
NULL
