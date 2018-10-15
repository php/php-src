--TEST--
Test array_values() function : usage variations - internal array pointer
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array
 * Source code: ext/standard/array.c
 */

/*
 * Test the position of the internal array pointer after a call to array_values
 */

echo "*** Testing array_values() : usage variations ***\n";

$input = array ('one' => 'un', 'two' => 'deux', 'three' => 'trois');

echo "\n-- Call array_values() --\n";
var_dump($result = array_values($input));

echo "-- Position of Internal Pointer in Result: --\n";
echo key($result) . " => " . current($result) . "\n";
echo "\n-- Position of Internal Pointer in Original Array: --\n";
echo key($input) . " => " . current ($input) . "\n";

echo "Done";
?>
--EXPECTF--
*** Testing array_values() : usage variations ***

-- Call array_values() --
array(3) {
  [0]=>
  string(2) "un"
  [1]=>
  string(4) "deux"
  [2]=>
  string(5) "trois"
}
-- Position of Internal Pointer in Result: --
0 => un

-- Position of Internal Pointer in Original Array: --
one => un
Done
