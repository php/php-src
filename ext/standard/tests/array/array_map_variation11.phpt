--TEST--
Test array_map() function : usage variations - with recursive callback
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing subarrays and recursive callback function
 */

echo "*** Testing array_map() : recursive callback function ***\n";

// callback function
function square_recur_single_array($var) {
   if (is_array($var))
     return array_map('square_recur_single_array', $var);
   return $var * $var;
}

$array1 = array(1, array(2, 3, array(5)), array(4));

var_dump( array_map('square_recur_single_array', $array1));

echo "Done";
?>
--EXPECT--
*** Testing array_map() : recursive callback function ***
array(3) {
  [0]=>
  int(1)
  [1]=>
  array(3) {
    [0]=>
    int(4)
    [1]=>
    int(9)
    [2]=>
    array(1) {
      [0]=>
      int(25)
    }
  }
  [2]=>
  array(1) {
    [0]=>
    int(16)
  }
}
Done
