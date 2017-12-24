--TEST--
scalar function return type constraint
--DESCRIPTION--
Verifies that a function can have a scalar return type constraint and that
it is possible to return all compatible types.
--FILE--
<?php

function f($p): scalar {
    return $p;
}

var_dump(
    f(true),
    f(false),
    f(4.2),
    f(42),
    f('str')
);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
