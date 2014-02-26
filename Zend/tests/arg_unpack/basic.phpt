--TEST--
Basic argument unpacking
--FILE--
<?php

function test(...$args) {
    var_dump($args);
}

function test2($arg1, $arg2, $arg3 = null) {
    var_dump($arg1, $arg2, $arg3);
}

function getArray($array) {
    return $array;
}

function arrayGen($array) {
    foreach ($array as $element) {
        yield $element;
    }
}

$array = [1, 2, 3];

test(...[]);
test(...[1, 2, 3]);
test(...$array);
test(...getArray([1, 2, 3]));
test(...arrayGen([]));
test(...arrayGen([1, 2, 3]));

test(1, ...[2, 3], ...[4, 5]);
test(1, ...getArray([2, 3]), ...arrayGen([4, 5]));

test2(...[1, 2]);
test2(...[1, 2, 3]);
test2(...[1], ...[], ...[], ...[2, 3], ...[4, 5]);

?>
--EXPECT--
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
int(1)
int(2)
NULL
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
