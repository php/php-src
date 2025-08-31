--TEST--
Test array_shift() function : usage variations - multi-dimensional arrays
--FILE--
<?php
/*
 * Test popping elements from a sub-array and popping an array from an array
 */

echo "*** Testing array_shift() : usage variations ***\n";

$stack_first = array(array(1, 2, 3), 'one', 'two');
$stack_last = array ('zero', 'one', array (1, 2, 3));
echo "\n-- Before shift: --\n";
echo "---- \$stack_first:\n";
var_dump($stack_first);
echo "---- \$stack_last:\n";
var_dump($stack_last);

echo "\n-- After shift: --\n";
echo "---- Pop array from array:\n";
echo "Returned value:\t";
var_dump(array_shift($stack_first));
echo "New array:\n";
var_dump($stack_first);

echo "---- Pop element from array within array:\n";
echo "Returned value:\t";
var_dump(array_shift($stack_last[2]));
echo "New array:\n";
var_dump($stack_last);

echo "Done";
?>
--EXPECT--
*** Testing array_shift() : usage variations ***

-- Before shift: --
---- $stack_first:
array(3) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
---- $stack_last:
array(3) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- After shift: --
---- Pop array from array:
Returned value:	array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
New array:
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}
---- Pop element from array within array:
Returned value:	int(1)
New array:
array(3) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
}
Done
