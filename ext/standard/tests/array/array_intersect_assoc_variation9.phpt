--TEST--
Test array_intersect_assoc() function : usage variations - two dimensional arrays for $arr1 and $arr2 arguments
--FILE--
<?php
/*
* Testing the behavior of array_intersect_assoc() by passing 2-D arrays
* to both $arr1 and $arr2 argument.
* Optional argument takes the same value as that of $arr1
*/

echo "*** Testing array_intersect_assoc() : passing two dimensional array to both \$arr1 and \$arr2 arguments ***\n";

// two dimensional arrays for $arr1 and $arr2 argument
$arr1 = array (

  // arrays with default keys
  array(1, 2, "hello", 'world'),
  array(1, 2, 3, 4),

  // arrays with explicit keys
  array(1 => "one", 2 => "two", 3 => "three"),
  array("ten" => 10, "twenty" => 20.00, "thirty" => 30)
);

$arr2 = array (
  array(1, 2, 3, 4),
  array(1 => "one", 2 => "two", 3 => "three")
);

/* Passing the entire array as argument to $arr1 and $arr2 */
// Calling array_intersect_assoc() with default arguments
echo "-- Passing the entire 2-D array to \$arr1 and \$arr2 --\n";
echo "- With default arguments -\n";
var_dump( array_intersect_assoc($arr1, $arr2) );

// Calling array_intersect_assoc() with more arguments
// additional argument passed is the same as $arr1
echo "- With more arguments -\n";
var_dump( array_intersect_assoc($arr1, $arr2, $arr1) );

/* Passing the sub-array as argument to $arr1 and $arr2 */
// Calling array_intersect_assoc() with default arguments
echo "-- Passing the sub-array to \$arr1 and \$arr2 --\n";
echo "- With default arguments -\n";
var_dump( array_intersect_assoc($arr1[0], $arr2[0]) );

// Calling array_intersect_assoc() with more arguments
// additional argument passed is the same as $arr1
echo "- With more arguments -\n";
var_dump( array_intersect_assoc($arr1[0], $arr2[0], $arr1[0]) );

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect_assoc() : passing two dimensional array to both $arr1 and $arr2 arguments ***
-- Passing the entire 2-D array to $arr1 and $arr2 --
- With default arguments -

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    string(5) "hello"
    [3]=>
    string(5) "world"
  }
  [1]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
}
- With more arguments -

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    string(5) "hello"
    [3]=>
    string(5) "world"
  }
  [1]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
}
-- Passing the sub-array to $arr1 and $arr2 --
- With default arguments -
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
- With more arguments -
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Done
