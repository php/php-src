--TEST--
PFA optional placeholder: ? always generates required params; ... inherits optionality
--FILE--
<?php

function foo(int $a = 1, string $b = 'default', float $c = 3.14) {
    return [$a, $b, $c];
}

echo "# ? makes targeted params required regardless of original optionality\n";
$f = foo(?);
var_dump((new ReflectionFunction($f))->getParameters()[0]->isOptional());

$f = foo(?, ?);
$params = (new ReflectionFunction($f))->getParameters();
var_dump($params[0]->isOptional(), $params[1]->isOptional());

$f = foo(?, ?, ?);
$params = (new ReflectionFunction($f))->getParameters();
var_dump($params[0]->isOptional(), $params[1]->isOptional(), $params[2]->isOptional());

echo "# ... inherits optionality from the original function\n";
$f = foo(?, ...);
$params = (new ReflectionFunction($f))->getParameters();
var_dump($params[0]->isOptional(), $params[1]->isOptional(), $params[2]->isOptional());

$f = foo(?, ?, ...);
$params = (new ReflectionFunction($f))->getParameters();
var_dump($params[0]->isOptional(), $params[1]->isOptional(), $params[2]->isOptional());

echo "# Calling works: ? params are required, unspecified optional params use their defaults\n";
$f = foo(?, ?);
var_dump($f(10, 'hello'));

echo "# Named ? placeholders are also required\n";

function bar(int $x = 0, int $y = 0) {
    return [$x, $y];
}

$f = bar(y: ?);
var_dump((new ReflectionFunction($f))->getParameters()[0]->isOptional());
var_dump($f(42));

?>
--EXPECT--
# ? makes targeted params required regardless of original optionality
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
# ... inherits optionality from the original function
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
# Calling works: ? params are required, unspecified optional params use their defaults
array(3) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  float(3.14)
}
# Named ? placeholders are also required
bool(false)
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(42)
}
