--TEST--
scalar type function reflection
--DESCRIPTION--
Tests that it is possible to reflect the scalar type constraints of a function.
--FILE--
<?php

function f(scalar $p): scalar {}

$r = new ReflectionFunction('f');
var_dump(
    $r->getParameters()[0]->getType()->getName(),
    $r->getReturnType()->getName()
);

?>
--EXPECT--
string(6) "scalar"
string(6) "scalar"
