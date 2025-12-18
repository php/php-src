--TEST--
Test asort() function : basic functionality
--FILE--
<?php
/*
 * Testing asort() by providing integer/string arrays to check the basic functionality
 * with following flag values.
 *  flag value as default
 *  SORT_REGULAR - compare items normally
 *  SORT_NUMERIC - compare items numerically
 *  SORT_STRING - compare items as strings
 *  SORT_STRICT - compare items by type first, then by value
*/

echo "*** Testing asort() : basic functionality ***\n";

// an array containing unsorted string values with indices
$unsorted_strings = array(
    "l" => "lemon", "o" => "orange",
    "O" => "Orange", "O1" => "Orange1", "o2" => "orange2", "O3" => "Orange3", "o20" => "orange20",
    "b" => "banana",
);
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

echo "\n-- Testing asort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array, SORT_STRING|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying string array (natural), 'flag' = SORT_NATURAL --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array, SORT_NATURAL) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array, SORT_NATURAL|SORT_FLAG_CASE) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( asort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying mixed type array, 'flag' = SORT_STRICT --\n";
$mixed_types = array( "a" => 1, "b" => "1", "c" => 2, "d" => "2", "e" => true, "f" => false, "g" => null );
var_dump( asort($mixed_types, SORT_STRICT) ); // expecting : bool(true)
var_dump( $mixed_types);

echo "\n-- Testing asort() by supplying string array, 'flag' = SORT_STRICT --\n";
$temp_array = $unsorted_strings;
var_dump( asort($temp_array, SORT_STRICT) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing asort() by supplying numeric array, 'flag' = SORT_STRICT --\n";
$temp_array = $unsorted_numerics;
var_dump( asort($temp_array, SORT_STRICT) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing asort() : basic functionality ***

-- Testing asort() by supplying string array, 'flag' value is default --
bool(true)
array(8) {
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["O3"]=>
  string(7) "Orange3"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["o2"]=>
  string(7) "orange2"
  ["o20"]=>
  string(8) "orange20"
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
array(8) {
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["O3"]=>
  string(7) "Orange3"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["o2"]=>
  string(7) "orange2"
  ["o20"]=>
  string(8) "orange20"
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
array(8) {
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["O3"]=>
  string(7) "Orange3"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["o2"]=>
  string(7) "orange2"
  ["o20"]=>
  string(8) "orange20"
}

-- Testing asort() by supplying string array (case insensitive), 'flag' = SORT_STRING|SORT_FLAG_CASE --
bool(true)
array(8) {
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["o2"]=>
  string(7) "orange2"
  ["o20"]=>
  string(8) "orange20"
  ["O3"]=>
  string(7) "Orange3"
}

-- Testing asort() by supplying string array (natural), 'flag' = SORT_NATURAL --
bool(true)
array(8) {
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["O3"]=>
  string(7) "Orange3"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["o2"]=>
  string(7) "orange2"
  ["o20"]=>
  string(8) "orange20"
}

-- Testing asort() by supplying string array (natural, case insensitive), 'flag' = SORT_NATURAL|SORT_FLAG_CASE --
bool(true)
array(8) {
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["o2"]=>
  string(7) "orange2"
  ["O3"]=>
  string(7) "Orange3"
  ["o20"]=>
  string(8) "orange20"
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

-- Testing asort() by supplying mixed type array, 'flag' = SORT_STRICT --
bool(true)
array(7) {
  ["g"]=>
  NULL
  ["f"]=>
  bool(false)
  ["e"]=>
  bool(true)
  ["a"]=>
  int(1)
  ["c"]=>
  int(2)
  ["b"]=>
  string(1) "1"
  ["d"]=>
  string(1) "2"
}

-- Testing asort() by supplying string array, 'flag' = SORT_STRICT --
bool(true)
array(8) {
  ["O"]=>
  string(6) "Orange"
  ["O1"]=>
  string(7) "Orange1"
  ["O3"]=>
  string(7) "Orange3"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemon"
  ["o"]=>
  string(6) "orange"
  ["o2"]=>
  string(7) "orange2"
  ["o20"]=>
  string(8) "orange20"
}

-- Testing asort() by supplying numeric array, 'flag' = SORT_STRICT --
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
