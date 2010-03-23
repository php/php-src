--TEST--
Test ksort() function : basic functionality   
--FILE--
<?php
/* Prototype  : bool ksort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key, maintaining key to data correlation 			
 * Source code: ext/standard/array.c
*/

/*
 * Testing ksort() by providing  array of integer/string values to check the basic functionality with following flag values :
 *  1.flag value as defualt
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
 *  4.SORT_STRING - compare items as strings
*/

echo "*** Testing ksort() : basic functionality ***\n";

// an array containing unsorted string values with indices  
$unsorted_strings =   array( "lemon" => "l", "orange" => "o", "banana" => "b" ); 
// an array containing unsorted numeric values with indices 
$unsorted_numerics =  array( 100 => 4, 33 => 3, 555 => 2, 22 => 1 );

echo "\n-- Testing ksort() by supplying string array, 'flag' value is defualt --\n";
$temp_array = $unsorted_strings;
var_dump( ksort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing ksort() by supplying numeric array, 'flag' value is defualt --\n";
$temp_array = $unsorted_numerics;
var_dump( ksort($temp_array) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing ksort() by supplying string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_strings;
var_dump( ksort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing ksort() by supplying numeric array, 'flag' = SORT_REGULAR --\n";
$temp_array = $unsorted_numerics;
var_dump( ksort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing ksort() by supplying string array, 'flag' = SORT_STRING --\n";
$temp_array = $unsorted_strings;
var_dump( ksort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump( $temp_array);

echo "\n-- Testing ksort() by supplying numeric array, 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( ksort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump( $temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing ksort() : basic functionality ***

-- Testing ksort() by supplying string array, 'flag' value is defualt --
bool(true)
array(3) {
  ["banana"]=>
  string(1) "b"
  ["lemon"]=>
  string(1) "l"
  ["orange"]=>
  string(1) "o"
}

-- Testing ksort() by supplying numeric array, 'flag' value is defualt --
bool(true)
array(4) {
  [22]=>
  int(1)
  [33]=>
  int(3)
  [100]=>
  int(4)
  [555]=>
  int(2)
}

-- Testing ksort() by supplying string array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  ["banana"]=>
  string(1) "b"
  ["lemon"]=>
  string(1) "l"
  ["orange"]=>
  string(1) "o"
}

-- Testing ksort() by supplying numeric array, 'flag' = SORT_REGULAR --
bool(true)
array(4) {
  [22]=>
  int(1)
  [33]=>
  int(3)
  [100]=>
  int(4)
  [555]=>
  int(2)
}

-- Testing ksort() by supplying string array, 'flag' = SORT_STRING --
bool(true)
array(3) {
  ["banana"]=>
  string(1) "b"
  ["lemon"]=>
  string(1) "l"
  ["orange"]=>
  string(1) "o"
}

-- Testing ksort() by supplying numeric array, 'flag' = SORT_NUMERIC --
bool(true)
array(4) {
  [22]=>
  int(1)
  [33]=>
  int(3)
  [100]=>
  int(4)
  [555]=>
  int(2)
}
Done
