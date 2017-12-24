--TEST--
nullable scalar instance method return type constraint
--DESCRIPTION--
Verifies that an instance method can have a nullable scalar return type
constraint and that it is possible to return all compatible types.
--FILE--
<?php

class C {
    function f($p): ?scalar {
        return $p;
    }
}

$c = new C;

var_dump(
    $c->f(true),
    $c->f(false),
    $c->f(4.2),
    $c->f(42),
    $c->f('str'),
    $c->f(null)
);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
NULL
