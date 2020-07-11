--TEST--
Test array_change_key_case() function : usage variations - referenced variables
--FILE--
<?php
/*
 * Test array_change_key_case() when:
 * 1. Passed a referenced variable
 * 2. Passed an argument by reference
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

$input = array('one' => 1, 'two' => 2, 'ABC' => 'xyz');

echo "\n-- \$input argument is a reference to array --\n";
$new_input = &$input;
echo "Result:\n";
var_dump(array_change_key_case($new_input, CASE_UPPER));
echo "Original:\n";
var_dump($input);
echo "Referenced:\n";
var_dump($new_input);

echo "Done";
?>
--EXPECT--
*** Testing array_change_key_case() : usage variations ***

-- $input argument is a reference to array --
Result:
array(3) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["ABC"]=>
  string(3) "xyz"
}
Original:
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["ABC"]=>
  string(3) "xyz"
}
Referenced:
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["ABC"]=>
  string(3) "xyz"
}
Done
