--TEST--
Test array_slice() function : usage variations - multidimensional arrays
--FILE--
<?php
/*
 * Test array_slice when passed
 * 1. a two-dimensional array as $input argument
 * 2. a sub-array as $input argument
 */

echo "*** Testing array_slice() : usage variations ***\n";

$input = array ('zero', 'one', array('zero', 'un', 'deux'), 9 => 'nine');

echo "\n-- Slice a two-dimensional array --\n";
var_dump(array_slice($input, 1, 3));

echo "\n-- \$input is a sub-array --\n";
var_dump(array_slice($input[2], 1, 2));

echo "Done";
?>
--EXPECT--
*** Testing array_slice() : usage variations ***

-- Slice a two-dimensional array --
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  array(3) {
    [0]=>
    string(4) "zero"
    [1]=>
    string(2) "un"
    [2]=>
    string(4) "deux"
  }
  [2]=>
  string(4) "nine"
}

-- $input is a sub-array --
array(2) {
  [0]=>
  string(2) "un"
  [1]=>
  string(4) "deux"
}
Done
