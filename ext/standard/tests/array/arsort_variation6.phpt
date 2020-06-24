--TEST--
Test arsort() function : usage variations - sort hexadecimal values
--FILE--
<?php
/*
 * testing arsort() by providing different hexa-decimal array for $array argument with following flag values
 * flag value as default
 * SORT_REGULAR - compare items normally
 * SORT_NUMERIC - compare items numerically
*/

echo "*** Testing arsort() : usage variations ***\n";

// an array contains unsorted hexadecimal values
// There are multiple keys which are duplicate and the later should be picked
$unsorted_hex_array = array ( 0x1AB => 0x1AB, 0xFFF => 0xFFF, 0xF => 0xF, 0xFF => 0xFF, 0x2AA => 0x2AA, 0xBB => 0xBB,
                              0x1ab => 0x1ab, 0xff => 0xff, -0xff => -0xFF, 0 => 0, -0x2aa => -0x2aa
                            );

echo "\n-- Testing arsort() by supplying hexadecimal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_hex_array;
var_dump(arsort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_hex_array;
var_dump(arsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_hex_array;
var_dump(arsort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing arsort() : usage variations ***

-- Testing arsort() by supplying hexadecimal value array, 'flag' value is default  --
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

-- Testing arsort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --
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

-- Testing arsort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --
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
