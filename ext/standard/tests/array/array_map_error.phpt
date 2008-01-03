--TEST--
Test array_map() function : error conditions
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_map() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_map() function with Zero arguments --\n";
var_dump( array_map() );

// Testing array_map with one less than the expected number of arguments
echo "\n-- Testing array_map() function with one less than expected no. of arguments --\n";
function callback1() {
  return 1;
}
var_dump( array_map('callback1') );

echo "\n-- Testing array_map() function with less no. of arrays than callback function arguments --\n";
$arr1 = array(1, 2);
function callback2($p, $q) {
  return $p * $q;
}
var_dump( array_map('callback2', $arr1) );

echo "\n-- Testing array_map() function with more no. of arrays than callback function arguments --\n";
$arr2 = array(3, 4);
$arr3 = array(5, 6);
var_dump( array_map('callback2', $arr1, $arr2, $arr3) );

echo "Done";
?>
--EXPECTF--
*** Testing array_map() : error conditions ***

-- Testing array_map() function with Zero arguments --

Warning: Wrong parameter count for array_map() in %s on line %d
NULL

-- Testing array_map() function with one less than expected no. of arguments --

Warning: Wrong parameter count for array_map() in %s on line %d
NULL

-- Testing array_map() function with less no. of arrays than callback function arguments --

Warning: Missing argument 2 for callback2() in %s on line %d

Notice: Undefined variable: q in %s on line %d

Warning: Missing argument 2 for callback2() in %s on line %d

Notice: Undefined variable: q in %s on line %d
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}

-- Testing array_map() function with more no. of arrays than callback function arguments --
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(8)
}
Done
