--TEST--
array_push() keeps the internal pointer and the next index
--FILE--
<?php
// The internal pointer does not move
$a = [1, 2, 3];
next($a);
array_push($a, 4, 5);
var_dump(key($a));

// The next index left by a removed last element is used
$a = [0, 1, 2];
unset($a[2]);
array_push($a, 3);
var_dump(array_keys($a));

// Pushing nothing keeps the next index of an empty array
$a = [];
array_push($a);
$a[-5] = 1;
$a[] = 2;
var_dump(array_keys($a));

// A negative next index of an empty copy is used
$a = [-3 => 0];
unset($a[-3]);
$copy = $a;
array_push($a, 1, 2);
var_dump(array_keys($a));
?>
--EXPECT--
int(1)
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(3)
}
array(2) {
  [0]=>
  int(-5)
  [1]=>
  int(-4)
}
array(2) {
  [0]=>
  int(-2)
  [1]=>
  int(-1)
}
