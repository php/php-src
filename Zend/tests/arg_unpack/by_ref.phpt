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

$array1 = [1, 2]; $array2 = [3, 4];
test1(...$array1, ...$array2);
var_dump($array1, $array2);

function test2($val1, &$ref1, $val2, &$ref2) {
    $ref1++;
    $ref2++;
}

$array = [0, 0, 0, 0];
test2(...$array);
var_dump($array);

$array1 = [1, 2]; $array2 = [4, 5];
test1(...$array1, ...$array2);
var_dump($array1, $array2);

$a = $b = $c = $d = 0;
$array = [0, 0, 0, 0];

test2($a, ...$array);
var_dump($a, $array);

test2($a, $b, ...$array);
var_dump($a, $b, $array);

test2($a, $b, $c, ...$array);
var_dump($a, $b, $c, $array);

test2($a, $b, $c, $d, ...$array);
var_dump($a, $b, $c, $d, $array);

?>
--EXPECT--
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
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(0)
  [3]=>
  int(1)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(2) {
  [0]=>
  int(5)
  [1]=>
  int(6)
}
int(0)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  int(0)
}
int(0)
int(1)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(0)
}
int(0)
int(2)
int(0)
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(0)
}
int(0)
int(3)
int(0)
int(1)
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(0)
}
