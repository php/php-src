--TEST--
Test ksort() function : usage variations - sort hexadecimal values
--FILE--
<?php
/*
 * testing ksort() by providing array of hexa-decimal values for $array argument with following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing ksort() : usage variations ***\n";

// an array containng unsorted hexadecimal values with keys
// There are multiple keys which are duplicate and the later should be picked
$unsorted_hex_array = array (
  0x1AB => 0x1AB, 0xFFF => 0xFFF, 0xF => 0xF, 0xFF => 0xFF, 0x2AA => 0x2AA, 0xBB => 0xBB,
  0x1ab => 0x1ab, 0xff => 0xff, -0xff => -0xFF, 0 => 0, -0x2aa => -0x2aa
);

echo "\n-- Testing ksort() by supplying hexadecimal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_hex_array;
var_dump(ksort( $temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_hex_array;
var_dump(ksort( $temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_hex_array;
var_dump(ksort( $temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying hexadecimal value array, 'flag' value is default  --
bool(true)
array(9) {
  [-682]=>
  int(-682)
  [-255]=>
  int(-255)
  [0]=>
  int(0)
  [15]=>
  int(15)
  [187]=>
  int(187)
  [255]=>
  int(255)
  [427]=>
  int(427)
  [682]=>
  int(682)
  [4095]=>
  int(4095)
}

-- Testing ksort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(9) {
  [-682]=>
  int(-682)
  [-255]=>
  int(-255)
  [0]=>
  int(0)
  [15]=>
  int(15)
  [187]=>
  int(187)
  [255]=>
  int(255)
  [427]=>
  int(427)
  [682]=>
  int(682)
  [4095]=>
  int(4095)
}

-- Testing ksort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(9) {
  [-682]=>
  int(-682)
  [-255]=>
  int(-255)
  [0]=>
  int(0)
  [15]=>
  int(15)
  [187]=>
  int(187)
  [255]=>
  int(255)
  [427]=>
  int(427)
  [682]=>
  int(682)
  [4095]=>
  int(4095)
}
Done
