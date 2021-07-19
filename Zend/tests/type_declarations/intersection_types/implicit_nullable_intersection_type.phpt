--TEST--
Intersection types can be implicitly nullable as the others
--FILE--
<?php

function foo(X&Y $foo = null) { return $foo; }

var_dump(foo());

?>
--EXPECT--
NULL
