--TEST--
Test arsort() function : usage variations - sort integer/float values
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array and maintain index association 
                Elements will be arranged from highest to lowest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * Testing arsort() by providing different integer/float value arrays for $array argument with following values
 * 1. flag value as defualt
 * 2. SORT_REGULAR - compare items normally
 * 3. SORT_NUMERIC - compare items numerically
*/

echo "*** Testing arsort() : usage variations ***\n";

// group of various arrays with indices
$various_arrays = array(
  // negative/posative integer array
  array(1 => 11, 2 => -11, 3 => 21, 4 => -21, 5 => 31, 6 => -31, 7 => 0, 8 => 41, 10 =>-41),

  // float value array
  array(1 => 10.5, 2 => -10.5, 3 => 10.5e2, 4 => 10.6E-2, 5 => .5, 6 => .0001, 7 => -.1),

  // mixed value array
  array(1 => .0001, 2 => .0021, 3 => -.01, 4 => -1, 5 => 0, 6 => .09, 7 => 2, 8 => -.9, 9 => 10.6E-2, 10 => -10.6E-2, 11 => 33),
 
  // array values contains minimum and maximum ranges
  array(1 => 2147483647, 2 => 2147483648, 3 => -2147483647, 4 => -2147483648, 5 => -0, 6 => 0, 7 => -2147483649)
);

// set of possible flag values
$flag_value = array("SORT_REGULAR" => SORT_REGULAR, "SORT_NUMERIC" => SORT_NUMERIC);

$count = 1;
echo "\n-- Testing arsort() by supplying various integer/float arrays --\n";

// loop through to test arsort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort_flag -\n"; 
  $temp_array = $array; 
  var_dump(arsort($temp_array) );
  var_dump($temp_array);

  // loop through $flag_value array and setting all possible flag values
  foreach($flag_value as $key => $flag){
    echo "- Sort_flag = $key -\n";
    $temp_array = $array; 
    var_dump(arsort($temp_array, $flag) );
    var_dump($temp_array);
  }  
  $count++;
} 

echo "Done\n";
?>
