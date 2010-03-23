--TEST--
Test asort() function : basic functionality   
--FILE--
<?php
/* Prototype  : bool asort ( array &$array [, int $sort_flags] )
 * Description: Sort an array and maintain index association
                Elements will be arranged from lowest to highest when this function has completed. 
 * Source code: ext/standard/array.c
*/

/*
 * Testing asort() by providing integer/string arrays to check the basic functionality 
 * with following flag values.
 *  flag value as default
 *  SORT_REGULAR - compare items normally
 *  SORT_NUMERIC - compare items numerically
 *  SORT_STRING - compare items as strings
*/

echo "*** Testing asort() : basic functionality ***\n";

// an array containing unsorted string values with indices  
$unsorted_strings =   array( "l" => "lemon", "o" => "orange", "b" => "banana" ); 
// an array containing unsorted numeric values with indices 
$unsorted_numerics =  array( 1 => 100, 2 => 33, 3 => 555, 4 => 22 );

echo "\n-- Testing asort() by supplying string array, 'flag' value is default --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying numeric array, 'flag' value is default --\n";
$temp_array = $unsorted_numerics;
var_dump( asort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying numeric array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_numerics;
var_dump( asort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying string array, 'flag' = SORT_STRING --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( asort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing asort() : basic functionality ***

-- Testing asort() by supplying string array, 'flag' value is default --
bool(true)
array(3) {
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
}

-- Testing asort() by supplying numeric array, 'flag' value is default --
bool(true)
array(4) {
  [4]=>
  int(22)
  [2]=>
  int(33)
  [1]=>
  int(100)
  [3]=>
  int(555)
}

-- Testing asort() by supplying string array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
}

-- Testing asort() by supplying numeric array, 'flag' = SORT_REGULAR --
bool(true)
array(4) {
  [4]=>
  int(22)
  [2]=>
  int(33)
  [1]=>
  int(100)
  [3]=>
  int(555)
}

-- Testing asort() by supplying string array, 'flag' = SORT_STRING --
bool(true)
array(3) {
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
}

-- Testing asort() by supplying numeric array, 'flag' = SORT_NUMERIC --
bool(true)
array(4) {
  [4]=>
  int(22)
  [2]=>
  int(33)
  [1]=>
  int(100)
  [3]=>
  int(555)
}
Done
