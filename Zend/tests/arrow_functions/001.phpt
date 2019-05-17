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

// Closing over $var
$var = 4;
$foo = fn() => $var;
var_dump($foo());

// Not closing over $var, it's a parameter
$foo = fn($var) => $var;
var_dump($foo(5));

// Close over $var by-value, not by-reference
$var = 5;
$foo = fn() => ++$var;
var_dump($foo());
var_dump($var);

// Nested arrow functions closing over variable
$var = 6;
var_dump((fn() => fn() => $var)()());
var_dump((fn() => function() use($var) { return $var; })()());

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(5)
int(6)
int(6)
