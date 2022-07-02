--TEST--
Test array_change_key_case() function : usage variations - position of internal pointer
--FILE--
<?php
/*
 * Check the position of the internal array pointer after calling the function
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

$input = array ('one' => 'un', 'two' => 'deux', 'three' => 'trois');

echo "\n-- Call array_change_key_case() --\n";
var_dump($result = array_change_key_case($input, CASE_UPPER));

echo "-- Position of Internal Pointer in Result: --\n";
echo key($result) . " => " . current($result) . "\n";
echo "\n-- Position of Internal Pointer in Original Array: --\n";
echo key($input) . " => " . current ($input) . "\n";

echo "Done";
?>
--EXPECT--
*** Testing array_change_key_case() : usage variations ***

-- Call array_change_key_case() --
array(3) {
  ["ONE"]=>
  string(2) "un"
  ["TWO"]=>
  string(4) "deux"
  ["THREE"]=>
  string(5) "trois"
}
-- Position of Internal Pointer in Result: --
ONE => un

-- Position of Internal Pointer in Original Array: --
one => un
Done
