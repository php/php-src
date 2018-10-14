--TEST--
Test array_udiff() function : usage variation
--FILE--
<?php
/* Prototype  : array array_udiff(array arr1, array arr2 [, array ...], callback data_comp_func)
 * Description: Returns the entries of arr1 that have values which are not present in any of the others arguments. Elements are compared by user supplied function.
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_udiff() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$arr1 = array(1);
$arr2 = array(1);

echo "\n-- comparison function with an incorrect return value --\n";
function incorrect_return_value ($val1, $val2) {
  return array(1);
}
var_dump(array_udiff($arr1, $arr2, 'incorrect_return_value'));

echo "\n-- comparison function taking too many parameters --\n";
function too_many_parameters ($val1, $val2, $val3) {
  return 0;
}
try {
	var_dump(array_udiff($arr1, $arr2, 'too_many_parameters'));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

echo "\n-- comparison function taking too few parameters --\n";
function too_few_parameters ($val1) {
  return 0;
}
var_dump(array_udiff($arr1, $arr2, 'too_few_parameters'));

?>
===DONE===
--EXPECTF--
*** Testing array_udiff() : usage variation ***

-- comparison function with an incorrect return value --
array(1) {
  [0]=>
  int(1)
}

-- comparison function taking too many parameters --
Exception: Too few arguments to function too_many_parameters(), 2 passed and exactly 3 expected

-- comparison function taking too few parameters --
array(0) {
}
===DONE===
