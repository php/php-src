--TEST--
Arrow functions syntax variations
--FILE--
<?php

// By-reference argument and return
$var = 1;
$id = fn&(&$x) => $x;
$ref =& $id($var);
$ref++;
var_dump($var);

// int argument and return type
$var = 10;
$int_fn = fn(int $x): int => $x;
var_dump($int_fn($var));
try {
    $int_fn("foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$varargs = fn(?int... $args): array => $args;
var_dump($varargs(20, null, 30));
try {
    $varargs(40, "foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
int(2)
int(10)
Argument 1 passed to {closure}() must be of the type int, string given, called in %s on line %d
array(3) {
  [0]=>
  int(20)
  [1]=>
  NULL
  [2]=>
  int(30)
}
Argument 2 passed to {closure}() must be of the type int or null, string given, called in %s on line %d
