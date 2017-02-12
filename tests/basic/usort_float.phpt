--TEST--
usort tests
--FILE--
<?php

$a = [5, 8, 3, 6, 7, 32, 0];
$cb = function ($a, $b) { return $a - $b; };

usort($a, $cb);

var_dump($a);

$a = [.1, .5, .9, 1.0, 1, -1, -0.5];

usort($a, $cb);

var_dump($a);

?>
--EXPECT--
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(3)
  [2]=>
  int(5)
  [3]=>
  int(6)
  [4]=>
  int(7)
  [5]=>
  int(8)
  [6]=>
  int(32)
}
array(7) {
  [0]=>
  int(-1)
  [1]=>
  float(-0.5)
  [2]=>
  float(0.1)
  [3]=>
  float(0.5)
  [4]=>
  float(0.9)
  [5]=>
  float(1)
  [6]=>
  int(1)
}
