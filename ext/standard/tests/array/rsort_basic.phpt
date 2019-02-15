--TEST--
Test rsort() function : basic functionality
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of rsort()
 */

echo "*** Testing rsort() : basic functionality ***\n";

// associative array containing unsorted string values
$unsorted_strings = array(
	"l" => "lemon", "o" => "orange",
	"O" => "Orange", "O1" => "Orange1", "o2" => "orange2", "O3" => "Orange3", "o20" => "orange20",
	"b" => "banana",
);

// array with default keys containing unsorted numeric values
$unsorted_numerics =  array( 100, 33, 555, 22 );

echo "\n-- Testing rsort() by supplying string array, 'flag' value is defualt --\n";
$temp_array = $unsorted_strings;
var_dump( rsort($temp_array) );
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying numeric array, 'flag' value is defualt --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort($temp_array) );
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_strings;
var_dump( rsort($temp_array, SORT_REGULAR) );
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying numeric array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort($temp_array, SORT_REGULAR) );
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying string array, 'flag' = SORT_STRING --\n";
$temp_array = $unsorted_strings;
var_dump( rsort($temp_array, SORT_STRING) );
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( rsort($temp_array, SORT_STRING|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying string array (natural), 'flag' = SORT_NATURAL --\n";
$temp_array = $unsorted_strings;
var_dump( rsort($temp_array, SORT_NATURAL) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( rsort($temp_array, SORT_NATURAL|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing rsort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort($temp_array, SORT_NUMERIC) );
var_dump( $temp_array);

echo "Done";
?>
--EXPECT--
*** Testing rsort() : basic functionality ***

-- Testing rsort() by supplying string array, 'flag' value is defualt --
bool(true)
array(8) {
  [0]=>
  string(8) "orange20"
  [1]=>
  string(7) "orange2"
  [2]=>
  string(6) "orange"
  [3]=>
  string(5) "lemon"
  [4]=>
  string(6) "banana"
  [5]=>
  string(7) "Orange3"
  [6]=>
  string(7) "Orange1"
  [7]=>
  string(6) "Orange"
}

-- Testing rsort() by supplying numeric array, 'flag' value is defualt --
bool(true)
array(4) {
  [0]=>
  int(555)
  [1]=>
  int(100)
  [2]=>
  int(33)
  [3]=>
  int(22)
}

-- Testing rsort() by supplying string array, 'flag' = SORT_REGULAR --
bool(true)
array(8) {
  [0]=>
  string(8) "orange20"
  [1]=>
  string(7) "orange2"
  [2]=>
  string(6) "orange"
  [3]=>
  string(5) "lemon"
  [4]=>
  string(6) "banana"
  [5]=>
  string(7) "Orange3"
  [6]=>
  string(7) "Orange1"
  [7]=>
  string(6) "Orange"
}

-- Testing rsort() by supplying numeric array, 'flag' = SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  int(555)
  [1]=>
  int(100)
  [2]=>
  int(33)
  [3]=>
  int(22)
}

-- Testing rsort() by supplying string array, 'flag' = SORT_STRING --
bool(true)
array(8) {
  [0]=>
  string(8) "orange20"
  [1]=>
  string(7) "orange2"
  [2]=>
  string(6) "orange"
  [3]=>
  string(5) "lemon"
  [4]=>
  string(6) "banana"
  [5]=>
  string(7) "Orange3"
  [6]=>
  string(7) "Orange1"
  [7]=>
  string(6) "Orange"
}

-- Testing rsort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --
bool(true)
array(8) {
  [0]=>
  string(7) "Orange3"
  [1]=>
  string(8) "orange20"
  [2]=>
  string(7) "orange2"
  [3]=>
  string(7) "Orange1"
  [4]=>
  string(6) "orange"
  [5]=>
  string(6) "Orange"
  [6]=>
  string(5) "lemon"
  [7]=>
  string(6) "banana"
}

-- Testing rsort() by supplying string array (natural), 'flag' = SORT_NATURAL --
bool(true)
array(8) {
  [0]=>
  string(8) "orange20"
  [1]=>
  string(7) "orange2"
  [2]=>
  string(6) "orange"
  [3]=>
  string(5) "lemon"
  [4]=>
  string(6) "banana"
  [5]=>
  string(7) "Orange3"
  [6]=>
  string(7) "Orange1"
  [7]=>
  string(6) "Orange"
}

-- Testing rsort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --
bool(true)
array(8) {
  [0]=>
  string(8) "orange20"
  [1]=>
  string(7) "Orange3"
  [2]=>
  string(7) "orange2"
  [3]=>
  string(7) "Orange1"
  [4]=>
  string(6) "orange"
  [5]=>
  string(6) "Orange"
  [6]=>
  string(5) "lemon"
  [7]=>
  string(6) "banana"
}

-- Testing rsort() by supplying numeric array, 'flag' = SORT_NUMERIC --
bool(true)
array(4) {
  [0]=>
  int(555)
  [1]=>
  int(100)
  [2]=>
  int(33)
  [3]=>
  int(22)
}
Done
