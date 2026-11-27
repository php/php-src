--TEST--
array_merge() and array_merge_recursive() with leading, middle, and all-empty inputs
--FILE--
<?php
var_dump(array_merge([], [], []));
var_dump(array_merge([], [1, 2], [], [3, 4]));
var_dump(array_merge([], [5 => 'a', 10 => 'b']));
$holes = [0 => 'a', 1 => 'b'];
unset($holes[0]);
var_dump(array_merge([], $holes, [2 => 'c']));
var_dump(array_merge(['k' => 1], [10, 20]));
var_dump(array_merge_recursive([], ['a' => 1], [], ['a' => 2]));
function trailing_unset() {
    $a = [1, 2, 3];
    unset($a[2]);
    return $a;
}
$r = array_merge([], [], trailing_unset(), []);
$r[] = 4;
var_dump($r);
?>
--EXPECT--
array(0) {
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "c"
}
array(3) {
  ["k"]=>
  int(1)
  [0]=>
  int(10)
  [1]=>
  int(20)
}
array(1) {
  ["a"]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
}
