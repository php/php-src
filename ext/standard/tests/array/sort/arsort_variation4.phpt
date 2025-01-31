--TEST--
Test arsort() function : usage variations - sort reference variables
--FILE--
<?php
/*
 * Testing arsort() by providing reference variable array with following flag values
 *  flag value as default
 *  SORT_REGULAR - compare items normally
 *  SORT_NUMERIC - compare items numerically
*/

echo "*** Testing arsort() :usage variations  ***\n";

$value1 = 100;
$value2 = 33;
$value3 = 555;

// an array containing integer references
$unsorted_numerics =  array( 1 => &$value1 , 2 => &$value2, 3 => &$value3);

echo "\n-- Testing arsort() by supplying reference variable array, 'flag' value is default --\n";
$temp_array = $unsorted_numerics;
var_dump( arsort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying reference variable array, 'flag' = SORT_REGULAR --\n";
$temp_array = &$unsorted_numerics;
var_dump( arsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying reference variable array, 'flag' = SORT_NUMERIC --\n";
$temp_array = &$unsorted_numerics;
var_dump( arsort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing arsort() :usage variations  ***

-- Testing arsort() by supplying reference variable array, 'flag' value is default --
bool(true)
array(3) {
  [3]=>
  &int(555)
  [1]=>
  &int(100)
  [2]=>
  &int(33)
}

-- Testing arsort() by supplying reference variable array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  [3]=>
  &int(555)
  [1]=>
  &int(100)
  [2]=>
  &int(33)
}

-- Testing arsort() by supplying reference variable array, 'flag' = SORT_NUMERIC --
bool(true)
array(3) {
  [3]=>
  &int(555)
  [1]=>
  &int(100)
  [2]=>
  &int(33)
}
Done
