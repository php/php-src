--TEST--
Basic arrow function functionality check
--FILE--
<?php

$foo = fn() => 1;
var_dump($foo());

$foo = fn($x) => $x;
var_dump($foo(2));

$foo = fn($x, $y) => $x + $y;
var_dump($foo(1, 2));

?>
--EXPECT--
int(1)
int(2)
int(3)
