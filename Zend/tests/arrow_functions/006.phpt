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

$id = fn&(&$x) => { return $x; };
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

$var = 11;
$int_fn = fn(int $x): int => { return $x; };
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

$varargs = fn(?int... $args): array => { return $args; };
var_dump($varargs(40, null, 50));
try {
    $varargs(60, "foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
int(2)
int(3)
int(10)
{closure}(): Argument #1 ($x) must be of type int, string given, called in %s on line %d
int(11)
{closure}(): Argument #1 ($x) must be of type int, string given, called in %s on line %d
array(3) {
  [0]=>
  int(20)
  [1]=>
  NULL
  [2]=>
  int(30)
}
{closure}(): Argument #2 must be of type ?int, string given, called in %s on line %d
array(3) {
  [0]=>
  int(40)
  [1]=>
  NULL
  [2]=>
  int(50)
}
{closure}(): Argument #2 must be of type ?int, string given, called in %s on line %d
