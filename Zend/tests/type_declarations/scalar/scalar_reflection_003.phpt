--TEST--
scalar type instance method reflection
--FILE--
<?php

class C {
    function f(scalar $p): scalar {}
}

$r = new ReflectionMethod(C::class, 'f');
var_dump(
    $r->getParameters()[0]->getType()->getName(),
    $r->getReturnType()->getName()
);

?>
--EXPECT--
string(6) "scalar"
string(6) "scalar"
