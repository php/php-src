--TEST--
GH-10014: Incorrect short-circuiting in constant expressions
--FILE--
<?php

#[Attribute(+[[][2]?->y]->y)]
class y {
}

(new ReflectionClass(y::class))->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Warning: Undefined array key 2 in %s on line %d

Warning: Attempt to read property "y" on array in %s on line %d
