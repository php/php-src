--TEST--
Argument unpacking with by-ref arguments
--FILE--
<?php

error_reporting(E_ALL);

function test1(&...$args) {
    foreach ($args as &$arg) {
        $arg++;
    }
}

test1(...[1, 2, 3]);

$array = [1, 2, 3];
test1(...$array);
var_dump($array);

$array1 = [1, 2]; $val2 = 3; $array2 = [4, 5];
test1(...$array1, $val2, ...$array2);
var_dump($array1, $val2, $array2);

function test2($val1, &$ref1, $val2, &$ref2) {
    $ref1++;
    $ref2++;
}

$array = [1, 2, 3, 4];
test2(...$array);
var_dump($array);

$a = $b = $c = $d = 0;

$array = [];
test2(...$array, $a, $b, $c, $d);
var_dump($array, $a, $b, $c, $d);

$array = [1];
test2(...$array, $a, $b, $c, $d);
var_dump($array, $a, $b, $c, $d);

$array = [1, 2];
test2(...$array, $a, $b, $c, $d);
var_dump($array, $a, $b, $c, $d);

$array = [1, 2, 3];
test2(...$array, $a, $b, $c, $d);
var_dump($array, $a, $b, $c, $d);

$vars = [];
$array = [];
test2(...$array, $vars['a'], $vars['b'], $vars['c'], $vars['d']);
var_dump($vars);

$vars = [];
$array = [1];
test2(...$array, $vars['a'], $vars['b'], $vars['c'], $vars['d']);
var_dump($vars);

?>
--EXPECTF--
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
int(4)
array(2) {
  [0]=>
  int(5)
  [1]=>
  int(6)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(3)
  [2]=>
  int(3)
  [3]=>
  int(5)
}
array(0) {
}
int(0)
int(1)
int(0)
int(1)
array(1) {
  [0]=>
  int(1)
}
int(1)
int(1)
int(1)
int(1)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(3)
}
int(1)
int(2)
int(1)
int(1)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(3)
  [2]=>
  int(3)
}
int(2)
int(2)
int(1)
int(1)

Notice: Undefined index: a in %s on line %d

Notice: Undefined index: c in %s on line %d
array(2) {
  ["b"]=>
  int(1)
  ["d"]=>
  int(1)
}

Notice: Undefined index: b in %s on line %d

Notice: Undefined index: d in %s on line %d
array(2) {
  ["a"]=>
  int(1)
  ["c"]=>
  int(1)
}
