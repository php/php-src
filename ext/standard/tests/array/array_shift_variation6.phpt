--TEST--
Test array_shift() function : usage variations - Referenced arrays
--FILE--
<?php
/* Prototype  : mixed array_shift(array &$stack)
 * Description: Pops an element off the beginning of the array
 * Source code: ext/standard/array.c
 */

/*
 * Test how array_shift when passed:
 * 1. a variable that is referenced to an array
 * 2. an array that contains a referenced array
 */

echo "*** Testing array_shift() : usage variations ***\n";

echo "\n-- Variable is referenced array --\n";
$original_array = array('zero', 'one', 'two');
$copied_array = &$original_array;

echo "Result: ";
var_dump(array_shift($copied_array));
echo "\n\$original_array:\n";
var_dump($original_array);
echo "\n\$copied_array:\n";
var_dump($copied_array);

echo "\n-- Element is referenced array --\n";
$new_array = array (&$copied_array, 1, 'two');
echo "Result: ";
var_dump(array_shift($new_array[0]));
echo "\n\$new_array:\n";
var_dump($new_array);
echo "\n\$copied_array\n";
var_dump($copied_array);

echo "Done";
?>
--EXPECTF--
*** Testing array_shift() : usage variations ***

-- Variable is referenced array --
Result: string(4) "zero"

$original_array:
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}

$copied_array:
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}

-- Element is referenced array --
Result: string(3) "one"

$new_array:
array(3) {
  [0]=>
  &array(1) {
    [0]=>
    string(3) "two"
  }
  [1]=>
  int(1)
  [2]=>
  string(3) "two"
}

$copied_array
array(1) {
  [0]=>
  string(3) "two"
}
Done
