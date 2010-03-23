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
$unsorted_strings =   array( "l" => "lemon", "o" => "orange", "b" => "banana" );
 
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

echo "\n-- Testing rsort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort($temp_array, SORT_NUMERIC) );
var_dump( $temp_array);

echo "Done";
?>

--EXPECTF--
*** Testing rsort() : basic functionality ***

-- Testing rsort() by supplying string array, 'flag' value is defualt --
bool(true)
array(3) {
  [0]=>
  string(6) "orange"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "banana"
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
array(3) {
  [0]=>
  string(6) "orange"
  [1]=>
  string(5) "lemon"
  [2]=>
  string(6) "banana"
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
array(3) {
  [0]=>
  string(6) "orange"
  [1]=>
  string(5) "lemon"
  [2]=>
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