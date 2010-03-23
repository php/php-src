--TEST--
Test array_unique() function : usage variations - two dimensional arrays
--FILE--
<?php
/* Prototype  : array array_unique(array $input)
 * Description: Removes duplicate values from array 
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_unique() by passing 
 * two dimensional arrays for $input argument.
*/

echo "*** Testing array_unique() : two dimensional array for \$input argument ***\n";

// initialize the 2-d array
$input = array( 
  array(1, 2, 3, 1),
  array("hello", "world", "str1" => "hello", "str2" => 'world'),
  array(1 => "one", 2 => "two", "one", 'two'),
  array(1, 2, 3, 1)
);

var_dump( array_unique($input, SORT_STRING) );

echo "Done";
?>
--EXPECTF--
*** Testing array_unique() : two dimensional array for $input argument ***
array(1) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(1)
  }
}
Done
