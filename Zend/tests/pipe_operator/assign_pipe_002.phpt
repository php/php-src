--TEST--
Pipe assign operator with closures and arrow functions
--FILE--
<?php

$x = 5;
$x |>= (fn($v) => $v * 2);
var_dump($x);

$y = "hello world";
$y |>= (function($s) { return strtoupper($s); });
var_dump($y);

$arr = [5, 2, 8, 1, 9];
$arr |>= (fn($a) => array_filter($a, fn($n) => $n > 3));
var_dump($arr);

?>
--EXPECT--
int(10)
string(11) "HELLO WORLD"
array(3) {
  [0]=>
  int(5)
  [2]=>
  int(8)
  [4]=>
  int(9)
}
