--TEST--
Basic arrow function functionality check
--FILE--
<?php

// No return value
$foo = fn() {};
var_dump($foo());

// Return value
$foo = fn() => 1;
var_dump($foo());

$foo = fn() { return 2; };
var_dump($foo());

$foo = fn($x) => $x;
var_dump($foo(3));

$foo = fn($x) { return $x; };
var_dump($foo(4));

$foo = fn($x, $y) => $x + $y;
var_dump($foo(4, 1));

$foo = fn($x, $y) { return $x + $y; };
var_dump($foo(5, 1));

// Closing over $var
$var = 7;
$foo = fn() => $var;
var_dump($foo());

$var = 8;
$foo = fn() { return $var; };
var_dump($foo());

// Not closing over $var, it's a parameter
$foo = fn($var) => $var;
var_dump($foo(9));

$foo = fn($var) { return $var; };
var_dump($foo(10));

// Close over $var by-value, not by-reference
$var = 11;
$foo = fn() => ++$var;
var_dump($var);
var_dump($foo());
var_dump($var);

$var = 13;
$foo = fn() { return ++$var; };
var_dump($var);
var_dump($foo());
var_dump($var);

// Nested arrow functions
$var = 14;
var_dump((fn() => fn() => $var)()());

$var = 15;
var_dump((fn() => function() use($var) { return $var; })()());

$var = 16;
var_dump((fn() { return fn() { return $var; }; })()());

// Nested arrow functions with null return value
var_dump((fn() { return fn() {}; })()());

?>
--EXPECT--
NULL
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(11)
int(13)
int(14)
int(13)
int(14)
int(15)
int(16)
NULL
