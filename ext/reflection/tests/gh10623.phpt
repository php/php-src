--TEST--
GH-10623 (ReflectionFunction::getClosureUsedVariables() returns empty array in presence of variadic arguments)
--FILE--
<?php

$data1 = 1;
$data2 = 2;
$data3 = 3;

$closure = function($var) use($data1) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

$closure = function($var, ...$variadic) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

$closure = function($var, ...$variadic) use($data1) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

$closure = function($var, ...$variadic) use($data1, $data2, $data3) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

$closure = function(...$variadic) use($data1) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

$closure = function($var, $var2, ...$variadic) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

$closure = function($var, $var2, $var3, ...$variadic) use($data1, $data2, $data3) {};
var_dump((new ReflectionFunction($closure))->getClosureUsedVariables());

?>
--EXPECT--
array(1) {
  ["data1"]=>
  int(1)
}
array(0) {
}
array(1) {
  ["data1"]=>
  int(1)
}
array(3) {
  ["data1"]=>
  int(1)
  ["data2"]=>
  int(2)
  ["data3"]=>
  int(3)
}
array(1) {
  ["data1"]=>
  int(1)
}
array(0) {
}
array(3) {
  ["data1"]=>
  int(1)
  ["data2"]=>
  int(2)
  ["data3"]=>
  int(3)
}
