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
$unsorted_strings = array(
	"l" => "lemon", "o" => "orange",
	"O" => "Orange", "O1" => "Orange1", "o2" => "orange2", "O3" => "Orange3", "o20" => "orange20",
	"b" => "banana",
);

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

echo "\n-- Testing sort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( sort($temp_array, SORT_STRING|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying string array (natural), 'flag' = SORT_NATURAL --\n";
$temp_array = $unsorted_strings;
var_dump( sort($temp_array, SORT_NATURAL) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( sort($temp_array, SORT_NATURAL|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing sort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( sort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing sort() : basic functionality ***

-- Testing sort() by supplying string array, 'flag' value is defualt --
bool(true)
array(8) {
  [0]=>
  string(6) "Orange"
  [1]=>
  string(7) "Orange1"
  [2]=>
  string(7) "Orange3"
  [3]=>
  string(6) "banana"
  [4]=>
  string(5) "lemon"
  [5]=>
  string(6) "orange"
  [6]=>
  string(7) "orange2"
  [7]=>
  string(8) "orange20"
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
array(8) {
  [0]=>
  string(6) "Orange"
  [1]=>
  string(7) "Orange1"
  [2]=>
  string(7) "Orange3"
  [3]=>
  string(6) "banana"
  [4]=>
  string(5) "lemon"
  [5]=>
  string(6) "orange"
  [6]=>
  string(7) "orange2"
  [7]=>
  string(8) "orange20"
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
array(8) {
  [0]=>
  string(6) "Orange"
  [1]=>
  string(7) "Orange1"
  [2]=>
  string(7) "Orange3"
  [3]=>
  string(6) "banana"
  [4]=>
  string(5) "lemon"
  [5]=>
  string(6) "orange"
  [6]=>
  string(7) "orange2"
  [7]=>
  string(8) "orange20"
}

-- Testing sort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --
bool(true)
array(8) {
  [0]=>
  string(6) "banana"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "orange"
  [3]=>
  string(6) "Orange"
  [4]=>
  string(7) "Orange1"
  [5]=>
  string(7) "orange2"
  [6]=>
  string(8) "orange20"
  [7]=>
  string(7) "Orange3"
}

-- Testing sort() by supplying string array (natural), 'flag' = SORT_NATURAL --
bool(true)
array(8) {
  [0]=>
  string(6) "Orange"
  [1]=>
  string(7) "Orange1"
  [2]=>
  string(7) "Orange3"
  [3]=>
  string(6) "banana"
  [4]=>
  string(5) "lemon"
  [5]=>
  string(6) "orange"
  [6]=>
  string(7) "orange2"
  [7]=>
  string(8) "orange20"
}

-- Testing sort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --
bool(true)
array(8) {
  [0]=>
  string(6) "banana"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "orange"
  [3]=>
  string(6) "Orange"
  [4]=>
  string(7) "Orange1"
  [5]=>
  string(7) "orange2"
  [6]=>
  string(7) "Orange3"
  [7]=>
  string(8) "orange20"
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
