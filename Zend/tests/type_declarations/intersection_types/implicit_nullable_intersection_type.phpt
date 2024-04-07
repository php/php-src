--TEST--
Intersection types can be implicitly nullable
--FILE--
<?php

function foo(X&Y $foo = null) {
    var_dump($foo);
}

foo(null);

?>
--EXPECTF--
Deprecated: foo(): Implicitly marking parameter $foo as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
NULL
