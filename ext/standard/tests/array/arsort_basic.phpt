--TEST--
Test arsort() function : basic functionality   
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array and maintain index association
                Elements will be arranged from highest to lowest when this function has completed. 
 * Source code: ext/standard/array.c
*/

/*
 * Testing arsort() by providing integer/string arrays to check the basic functionality 
 * with following flag values.
 *  flag value as default
 *  SORT_REGULAR - compare items normally
 *  SORT_NUMERIC - compare items numerically
 *  SORT_STRING - compare items as strings
*/

echo "*** Testing arsort() : basic functionality ***\n";

// an array containing unsorted string values with indices  
$unsorted_strings = array(
	"l" => "lemon", "o" => "orange",
	"O" => "Orange", "O1" => "Orange1", "o2" => "orange2", "O3" => "Orange3", "o20" => "orange20",
	"b" => "banana",
);
// an array containing unsorted numeric values with indices 
$unsorted_numerics =  array( 1 => 100, 2 => 33, 3 => 555, 4 => 22 );

echo "\n-- Testing arsort() by supplying string array, 'flag' value is default --\n";
$temp_array = $unsorted_strings;
var_dump( arsort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying numeric array, 'flag' value is default --\n";
$temp_array = $unsorted_numerics;
var_dump( arsort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_strings;
var_dump( arsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying numeric array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_numerics;
var_dump( arsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying string array, 'flag' = SORT_STRING --\n";
$temp_array = $unsorted_strings;
var_dump( arsort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( arsort($temp_array, SORT_STRING|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying string array (natural), 'flag' = SORT_NATURAL --\n";
$temp_array = $unsorted_strings;
var_dump( arsort($temp_array, SORT_NATURAL) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( arsort($temp_array, SORT_NATURAL|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing arsort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( arsort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing arsort() : basic functionality ***

-- Testing arsort() by supplying string array, 'flag' value is default --
bool(true)
array(8) {
  ["o20"]=>
  string(8) "orange20"
  ["o2"]=>
  string(7) "orange2"
  ["o"]=>
  string(6) "orange"
  ["l"]=>
  string(5) "lemon"
  ["b"]=>
  string(6) "banana"
  ["O3"]=>
  string(7) "Orange3"
  ["O1"]=>
  string(7) "Orange1"
  ["O"]=>
  string(6) "Orange"
}

-- Testing arsort() by supplying numeric array, 'flag' value is default --
bool(true)
array(4) {
  [3]=>
  int(555)
  [1]=>
  int(100)
  [2]=>
  int(33)
  [4]=>
  int(22)
}

-- Testing arsort() by supplying string array, 'flag' = SORT_REGULAR --
bool(true)
array(8) {
  ["o20"]=>
  string(8) "orange20"
  ["o2"]=>
  string(7) "orange2"
  ["o"]=>
  string(6) "orange"
  ["l"]=>
  string(5) "lemon"
  ["b"]=>
  string(6) "banana"
  ["O3"]=>
  string(7) "Orange3"
  ["O1"]=>
  string(7) "Orange1"
  ["O"]=>
  string(6) "Orange"
}

-- Testing arsort() by supplying numeric array, 'flag' = SORT_REGULAR --
bool(true)
array(4) {
  [3]=>
  int(555)
  [1]=>
  int(100)
  [2]=>
  int(33)
  [4]=>
  int(22)
}

-- Testing arsort() by supplying string array, 'flag' = SORT_STRING --
bool(true)
array(8) {
  ["o20"]=>
  string(8) "orange20"
  ["o2"]=>
  string(7) "orange2"
  ["o"]=>
  string(6) "orange"
  ["l"]=>
  string(5) "lemon"
  ["b"]=>
  string(6) "banana"
  ["O3"]=>
  string(7) "Orange3"
  ["O1"]=>
  string(7) "Orange1"
  ["O"]=>
  string(6) "Orange"
}

-- Testing arsort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --
bool(true)
array(8) {
  ["O3"]=>
  string(7) "Orange3"
  ["o20"]=>
  string(8) "orange20"
  ["o2"]=>
  string(7) "orange2"
  ["O1"]=>
  string(7) "Orange1"
  ["o"]=>
  string(6) "orange"
  ["O"]=>
  string(6) "Orange"
  ["l"]=>
  string(5) "lemon"
  ["b"]=>
  string(6) "banana"
}

-- Testing arsort() by supplying string array (natural), 'flag' = SORT_NATURAL --
bool(true)
array(8) {
  ["o20"]=>
  string(8) "orange20"
  ["o2"]=>
  string(7) "orange2"
  ["o"]=>
  string(6) "orange"
  ["l"]=>
  string(5) "lemon"
  ["b"]=>
  string(6) "banana"
  ["O3"]=>
  string(7) "Orange3"
  ["O1"]=>
  string(7) "Orange1"
  ["O"]=>
  string(6) "Orange"
}

-- Testing arsort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --
bool(true)
array(8) {
  ["o20"]=>
  string(8) "orange20"
  ["O3"]=>
  string(7) "Orange3"
  ["o2"]=>
  string(7) "orange2"
  ["O1"]=>
  string(7) "Orange1"
  ["o"]=>
  string(6) "orange"
  ["O"]=>
  string(6) "Orange"
  ["l"]=>
  string(5) "lemon"
  ["b"]=>
  string(6) "banana"
}

-- Testing arsort() by supplying numeric array, 'flag' = SORT_NUMERIC --
bool(true)
array(4) {
  [3]=>
  int(555)
  [1]=>
  int(100)
  [2]=>
  int(33)
  [4]=>
  int(22)
}
Done