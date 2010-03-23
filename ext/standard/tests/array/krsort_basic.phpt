--TEST--
Test krsort() function : basic functionality   
--FILE--
<?php
/* Prototype  : bool krsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key in reverse order, maintaining key to data correlation 			
 * Source code: ext/standard/array.c
*/

/*
 * Testing krsort() by providing array of integer/string values to check the basic functionality 
 * with following flag values :
 *  1.flag value as defualt
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
 *  4.SORT_STRING - compare items as strings
*/

echo "*** Testing krsort() : basic functionality ***\n";

// an array containing unsorted string values with indices  
$unsorted_strings =   array( "lemon" => "l", "orange" => "o", "banana" => "b" ); 
// an array containing unsorted numeric values with indices 
$unsorted_numerics =  array( 100 => 4, 33 => 3, 555 => 2, 22 => 1 );

echo "\n-- Testing krsort() by supplying string array, 'flag' value is defualt --\n";
$temp_array = $unsorted_strings;
var_dump( krsort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing krsort() by supplying numeric array, 'flag' value is defualt --\n";
$temp_array = $unsorted_numerics;
var_dump( krsort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing krsort() by supplying string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_strings;
var_dump( krsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing krsort() by supplying numeric array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_numerics;
var_dump( krsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing krsort() by supplying string array, 'flag' = SORT_STRING --\n";
$temp_array = $unsorted_strings;
var_dump( krsort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing krsort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( krsort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing krsort() : basic functionality ***

-- Testing krsort() by supplying string array, 'flag' value is defualt --
bool(true)
array(3) {
  ["orange"]=>
  string(1) "o"
  ["lemon"]=>
  string(1) "l"
  ["banana"]=>
  string(1) "b"
}

-- Testing krsort() by supplying numeric array, 'flag' value is defualt --
bool(true)
array(4) {
  [555]=>
  int(2)
  [100]=>
  int(4)
  [33]=>
  int(3)
  [22]=>
  int(1)
}

-- Testing krsort() by supplying string array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  ["orange"]=>
  string(1) "o"
  ["lemon"]=>
  string(1) "l"
  ["banana"]=>
  string(1) "b"
}

-- Testing krsort() by supplying numeric array, 'flag' = SORT_REGULAR --
bool(true)
array(4) {
  [555]=>
  int(2)
  [100]=>
  int(4)
  [33]=>
  int(3)
  [22]=>
  int(1)
}

-- Testing krsort() by supplying string array, 'flag' = SORT_STRING --
bool(true)
array(3) {
  ["orange"]=>
  string(1) "o"
  ["lemon"]=>
  string(1) "l"
  ["banana"]=>
  string(1) "b"
}

-- Testing krsort() by supplying numeric array, 'flag' = SORT_NUMERIC --
bool(true)
array(4) {
  [555]=>
  int(2)
  [100]=>
  int(4)
  [33]=>
  int(3)
  [22]=>
  int(1)
}
Done
