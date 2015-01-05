--TEST--
Test array_uintersect_uassoc() function : usage variation - incorrect callbacks 
--FILE--
<?php
/* Prototype  : array array_uintersect_uassoc(array arr1, array arr2 [, array ...], callback data_compare_func, callback key_compare_func)
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check. Both data and keys are compared by using user-supplied callbacks. 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_uintersect_uassoc() : usage variation - incorrect callbacks ***\n";

$arr1 = array(1);
$arr2 = array(1,2);

echo "\n-- comparison function with an incorrect return value --\n";
function incorrect_return_value ($val1, $val2) {
  return array(1);
}
var_dump(array_uintersect_uassoc($arr1, $arr2, 'incorrect_return_value', 'incorrect_return_value'));

echo "\n-- comparison function taking too many parameters --\n";
function too_many_parameters ($val1, $val2, $val3) {
  return 1;
}
var_dump(array_uintersect_uassoc($arr1, $arr2, 'too_many_parameters', 'too_many_parameters'));

echo "\n-- comparison function taking too few parameters --\n";
function too_few_parameters ($val1) {
  return 1;
}
var_dump(array_uintersect_uassoc($arr1, $arr2, 'too_few_parameters', 'too_few_parameters'));

?>
===DONE===
--EXPECTF--
*** Testing array_uintersect_uassoc() : usage variation - incorrect callbacks ***

-- comparison function with an incorrect return value --
array(0) {
}

-- comparison function taking too many parameters --

Warning: Missing argument 3 for too_many_parameters() in %sarray_uintersect_uassoc_variation6.php on line %d

Warning: Missing argument 3 for too_many_parameters() in %sarray_uintersect_uassoc_variation6.php on line %d

Warning: Missing argument 3 for too_many_parameters() in %sarray_uintersect_uassoc_variation6.php on line %d
array(0) {
}

-- comparison function taking too few parameters --
array(0) {
}
===DONE===
