--TEST--
Test sort() function : basic functionality   
--FILE--
<?php
/* Prototype  : bool sort ( array &$array [, int $sort_flags] )
 * Description: This function sorts an array. 
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * Testing sort() by providing arrays with default keys and assoc arrays 
 * to check the basic functionality with following flag values.
 *  flag value as defualt
 *  SORT_REGULAR - compare items normally
 *  SORT_NUMERIC - compare items numerically
 *  SORT_STRING - compare items as strings
*/

echo "*** Testing sort() : basic functionality ***\n";

// associative array containing unsorted string values  
$unsorted_strings =   array( "l" => "lemon", "o" => "orange", "b" => "banana" );
 
// array with default keys containing unsorted numeric values
$unsorted_numerics =  array( 100, 33, 555, 22 );

echo "\n-- Testing sort() by supplying string array, 'flag' value is defualt --\n";
$temp_array = $unsorted_strings;
var_dump( sort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying numeric array, 'flag' value is defualt --\n";
$temp_array = $unsorted_numerics;
var_dump( sort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_strings;
var_dump( sort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying numeric array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_numerics;
var_dump( sort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying string array, 'flag' = SORT_STRING --\n";
$temp_array = $unsorted_strings;
var_dump( sort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( sort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing sort() : basic functionality ***

-- Testing sort() by supplying string array, 'flag' value is defualt --
bool(true)
array(3) {
  [0]=>
  string(6) "banana"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "orange"
}

-- Testing sort() by supplying numeric array, 'flag' value is defualt --
bool(true)
array(4) {
  [0]=>
  int(22)
  [1]=>
  int(33)
  [2]=>
  int(100)
  [3]=>
  int(555)
}

-- Testing sort() by supplying string array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  [0]=>
  string(6) "banana"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "orange"
}

-- Testing sort() by supplying numeric array, 'flag' = SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  int(22)
  [1]=>
  int(33)
  [2]=>
  int(100)
  [3]=>
  int(555)
}

-- Testing sort() by supplying string array, 'flag' = SORT_STRING --
bool(true)
array(3) {
  [0]=>
  string(6) "banana"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "orange"
}

-- Testing sort() by supplying numeric array, 'flag' = SORT_NUMERIC --
bool(true)
array(4) {
  [0]=>
  int(22)
  [1]=>
  int(33)
  [2]=>
  int(100)
  [3]=>
  int(555)
}
Done
