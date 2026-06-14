--TEST--
Test krsort() function : usage variations - sort hexadecimal values
--FILE--
<?php
/*
 * testing krsort() by providing array of hexa-decimal values for $array argument
 * with following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing krsort() : usage variations ***\n";

// an array containing unsorted hexadecimal values with keys
$unsorted_hex_array = array (
  0x1AB => 0x1AB, 0xFFF => 0xFFF, 0xF => 0xF, 0xFF => 0xFF, 0x2AA => 0x2AA, 0xBB => 0xBB,
  0x1ab => 0x1ab, 0xff => 0xff, -0xff => -0xFF, 0 => 0, -0x2aa => -0x2aa
);

echo "\n-- Testing krsort() by supplying hexadecimal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_hex_array;
var_dump(krsort( $temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_hex_array;
var_dump(krsort( $temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_hex_array;
var_dump(krsort( $temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying hexadecimal value array, 'flag' value is default  --
bool(true)
array(9) {
  [4095]=>
  int(4095)
  [682]=>
  int(682)
  [427]=>
  int(427)
  [255]=>
  int(255)
  [187]=>
  int(187)
  [15]=>
  int(15)
  [0]=>
  int(0)
  [-255]=>
  int(-255)
  [-682]=>
  int(-682)
}

-- Testing krsort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(9) {
  [4095]=>
  int(4095)
  [682]=>
  int(682)
  [427]=>
  int(427)
  [255]=>
  int(255)
  [187]=>
  int(187)
  [15]=>
  int(15)
  [0]=>
  int(0)
  [-255]=>
  int(-255)
  [-682]=>
  int(-682)
}

-- Testing krsort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(9) {
  [4095]=>
  int(4095)
  [682]=>
  int(682)
  [427]=>
  int(427)
  [255]=>
  int(255)
  [187]=>
  int(187)
  [15]=>
  int(15)
  [0]=>
  int(0)
  [-255]=>
  int(-255)
  [-682]=>
  int(-682)
}
Done
