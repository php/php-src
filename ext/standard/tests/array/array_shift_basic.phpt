--TEST--
Test array_shift() function : basic functionality
--FILE--
<?php
/* Prototype  : mixed array_shift(array &$stack)
 * Description: Pops an element off the beginning of the array
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of array_shift()
 */

echo "*** Testing array_shift() : basic functionality ***\n";

$array = array('zero', 'one', '3' => 'three', 'four' => 4);
echo "\n-- Before shift: --\n";
var_dump($array);

echo "\n-- After shift: --\n";
echo "Returned value:\t";
var_dump(array_shift($array));
echo "New array:\n";
var_dump($array);

echo "Done";
?>
--EXPECT--
*** Testing array_shift() : basic functionality ***

-- Before shift: --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [3]=>
  string(5) "three"
  ["four"]=>
  int(4)
}

-- After shift: --
Returned value:	string(4) "zero"
New array:
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  string(5) "three"
  ["four"]=>
  int(4)
}
Done
