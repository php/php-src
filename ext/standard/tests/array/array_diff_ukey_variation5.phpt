--TEST--
Test array_diff_ukey() function : usage variation - Passing multi-dimensional array
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments. User supplied function is used for comparing the keys. This function is like array_udiff() but works on the keys instead of the values. The associativity is preserved.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array1 = array(

	  'first' => array('blue' => 1, 'red' => 2),

      'second' => array('yellow' => 7),

      'third' => array(0 => 'zero'),
);

$array2 = array (

	  'first' => array('blue' => 1, 'red' => 2,),

      'second' => array('cyan' => 8),

      'fourth' => array(2 => 'two'),
);

echo "\n-- Testing array_diff_ukey() function with multi dimensional array --\n";
var_dump( array_diff_ukey($array1, $array2, 'strcasecmp') );
var_dump( array_diff_ukey($array2, $array1, 'strcasecmp') );
?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : usage variation ***

-- Testing array_diff_ukey() function with multi dimensional array --
array(1) {
  ["third"]=>
  array(1) {
    [0]=>
    string(4) "zero"
  }
}
array(1) {
  ["fourth"]=>
  array(1) {
    [2]=>
    string(3) "two"
  }
}
===DONE===
