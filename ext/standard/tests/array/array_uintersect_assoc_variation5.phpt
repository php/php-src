--TEST--
Test array_uintersect_assoc() function : usage variation - differing comparison functions
--FILE--
<?php
/* Prototype  : array array_uintersect_assoc(array arr1, array arr2 [, array ...], callback data_compare_func)
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments. Data is compared by using an user-supplied callback.
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_uintersect_assoc() : usage variation - differing comparison functions***\n";

$arr1 = array(1);
$arr2 = array(1,2);

echo "\n-- comparison function with an incorrect return value --\n";
function incorrect_return_value ($val1, $val2) {
  return array(1);
}
var_dump(array_uintersect_assoc($arr1, $arr2, 'incorrect_return_value'));

echo "\n-- comparison function taking too many parameters --\n";
function too_many_parameters ($val1, $val2, $val3) {
  return 1;
}
try {
	var_dump(array_uintersect_assoc($arr1, $arr2, 'too_many_parameters'));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

echo "\n-- comparison function taking too few parameters --\n";
function too_few_parameters ($val1) {
  return 1;
}
var_dump(array_uintersect_assoc($arr1, $arr2, 'too_few_parameters'));

?>

===DONE===
--EXPECT--
*** Testing array_uintersect_assoc() : usage variation - differing comparison functions***

-- comparison function with an incorrect return value --
array(0) {
}

-- comparison function taking too many parameters --
Exception: Too few arguments to function too_many_parameters(), 2 passed and exactly 3 expected

-- comparison function taking too few parameters --
array(0) {
}

===DONE===
