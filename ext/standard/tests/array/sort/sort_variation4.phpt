--TEST--
Test sort() function : usage variations - sort reference values
--FILE--
<?php
/*
 * Testing sort() by providing reference variable array with following flag values
 *  flag value as default
 *  SORT_REGULAR - compare items normally
 *  SORT_NUMERIC - compare items numerically
*/

echo "*** Testing sort() :usage variations  ***\n";

$value1 = 100;
$value2 = 33;
$value3 = 555;

// an array containing integer references
$unsorted_numerics =  array( &$value1 , &$value2, &$value3);

echo "\n-- Testing sort() by supplying reference variable array, 'flag' value is default --\n";
$temp_array = $unsorted_numerics;
var_dump( sort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying reference variable array, 'flag' = SORT_REGULAR --\n";
$temp_array = &$unsorted_numerics;
var_dump( sort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying reference variable array, 'flag' = SORT_NUMERIC --\n";
$temp_array = &$unsorted_numerics;
var_dump( sort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing sort() :usage variations  ***

-- Testing sort() by supplying reference variable array, 'flag' value is default --
bool(true)
array(3) {
  [0]=>
  &int(33)
  [1]=>
  &int(100)
  [2]=>
  &int(555)
}

-- Testing sort() by supplying reference variable array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  [0]=>
  &int(33)
  [1]=>
  &int(100)
  [2]=>
  &int(555)
}

-- Testing sort() by supplying reference variable array, 'flag' = SORT_NUMERIC --
bool(true)
array(3) {
  [0]=>
  &int(33)
  [1]=>
  &int(100)
  [2]=>
  &int(555)
}
Done
