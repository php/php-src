--TEST--
array_shift() on packed arrays: hole-free fast path and holed fallback
--FILE--
<?php
// Hole-free tail move (tail longer than one element).
$a = [1, "two", 3, 4, 5];
var_dump(array_shift($a), $a);
$a[] = 6; // The next appended key must follow the compacted tail.
var_dump($a);

// Hole-free two-element array (single-element tail).
$a = [10, 20];
var_dump(array_shift($a), $a);

// Hole-free single element: shifts to empty, the next appended key is 0.
$a = [42];
var_dump(array_shift($a), $a);
$a[] = 7;
var_dump($a);

// Holes at the head and in the middle: the compaction loop must skip them.
$a = [1, 2, 3, 4, 5];
unset($a[0], $a[2]);
var_dump(array_shift($a), $a);

// Shift a holed array down to empty: the next appended key must be 0.
$a = [10, 20];
unset($a[0]);
var_dump(array_shift($a), $a);
$a[] = 30;
var_dump($a);
?>
--EXPECT--
int(1)
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
array(5) {
  [0]=>
  string(3) "two"
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
}
int(10)
array(1) {
  [0]=>
  int(20)
}
int(42)
array(0) {
}
array(1) {
  [0]=>
  int(7)
}
int(2)
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
int(20)
array(0) {
}
array(1) {
  [0]=>
  int(30)
}
