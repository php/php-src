--TEST--
scalar type function reflection
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
