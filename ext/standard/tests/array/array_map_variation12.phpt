--TEST--
Test array_map() function : usage variations - built-in function as callback
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing buit-in function as callback function
 */

echo "*** Testing array_map() : built-in function ***\n";

$array1 = array(1, 2, 3);
$array2 = array(3, 4, 5);

echo "-- with built-in function 'pow' and two parameters --\n";
var_dump( array_map('pow', $array1, $array2));

echo "-- with built-in function 'pow' and one parameter --\n";
var_dump( array_map('pow', $array1));

echo "-- with language construct --\n";
var_dump( array_map('echo', $array1));

echo "Done";
?>
--EXPECTF--
*** Testing array_map() : built-in function ***
-- with built-in function 'pow' and two parameters --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(16)
  [2]=>
  int(243)
}
-- with built-in function 'pow' and one parameter --

Warning: pow() expects exactly 2 parameters, 1 given in %s on line %d

Warning: pow() expects exactly 2 parameters, 1 given in %s on line %d

Warning: pow() expects exactly 2 parameters, 1 given in %s on line %d
array(3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
-- with language construct --

Warning: array_map(): The first argument, 'echo', should be either NULL or a valid callback in %s on line %d
NULL
Done
