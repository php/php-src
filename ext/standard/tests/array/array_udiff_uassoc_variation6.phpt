--TEST--
Test array_udiff_uassoc() function : usage variation 
--FILE--
<?php
/* Prototype  : array array_udiff_uassoc(array arr1, array arr2 [, array ...], callback data_comp_func, callback key_comp_func)
 * Description: Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys and elements are compared by user supplied functions. 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_udiff_uassoc() : usage variation - differing comparison functions***\n";

$arr1 = array(1);
$arr2 = array(1);

echo "\n-- comparison function with an incorrect return value --\n";
function incorrect_return_value ($val1, $val2) {
  return array(1);
}
var_dump(array_udiff_uassoc($arr1, $arr2, 'incorrect_return_value', 'incorrect_return_value'));

echo "\n-- comparison function taking too many parameters --\n";
function too_many_parameters ($val1, $val2, $val3) {
  return 1;
}
var_dump(array_udiff_uassoc($arr1, $arr2, 'too_many_parameters', 'too_many_parameters'));

echo "\n-- comparison function taking too few parameters --\n";
function too_few_parameters ($val1) {
  return 1;
}
var_dump(array_udiff_uassoc($arr1, $arr2, 'too_few_parameters', 'too_few_parameters'));

?>
===DONE===
--EXPECTF--
*** Testing array_udiff_uassoc() : usage variation - differing comparison functions***

-- comparison function with an incorrect return value --
array(1) {
  [0]=>
  int(1)
}

-- comparison function taking too many parameters --

Warning: Missing argument 3 for too_many_parameters() in %sarray_udiff_uassoc_variation6.php on line %d
array(1) {
  [0]=>
  int(1)
}

-- comparison function taking too few parameters --
array(1) {
  [0]=>
  int(1)
}
===DONE===
