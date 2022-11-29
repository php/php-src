--TEST--
Intersection types can be implicitly nullable
--FILE--
<?php

function foo(X&Y $foo = null) {
    var_dump($foo);
}

foo(null);

?>
--EXPECT--
NULL
