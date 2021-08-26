--TEST--
foreach by reference with destructuring
--FILE--
<?php

function test1(array $array) {
    foreach ($array as [$x, &$y, &$z]) {
    }
    $y = 'y';
    $z = 'z';
    var_dump($array);
}

function test2(array $array) {
    foreach ($array as [, [&$y], &$z]) {
    }
    $y = 'y';
    $z = 'z';
    var_dump($array);
}

test1([[0, 1, 2]]);
test2([[0, [1], 2]]);

?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(0)
    [1]=>
    array(1) {
      [0]=>
      int(1)
    }
    [2]=>
    int(2)
  }
}
