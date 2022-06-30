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

// All literal variables are closed over
$a = 0;
$foo = fn() => $a = $var;
var_dump((new ReflectionFunction($foo))->getStaticVariables());

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
array(2) {
  ["a"]=>
  int(0)
  ["var"]=>
  int(6)
}
