--TEST--
Test ksort() function : error conditions
--FILE--
<?php
/* Prototype  : bool ksort(array &array_arg [, int sort_flags])
 * Description: Sort an array by key, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

/*
* Testing ksort() function with all possible error conditions
*/

echo "*** Testing ksort() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ksort() function with Zero arguments --\n";
var_dump( ksort() );

//Test ksort with more than the expected number of arguments
echo "\n-- Testing ksort() function with more than expected no. of arguments --\n";
$array_arg = array(1 => 1, 2 => 2);
$flag_value = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING, "SORT_NUMERIC" => SORT_NUMERIC);
$extra_arg = 10;

// loop through $flag_value array and call krsort with all possible sort flag values
foreach($flag_value as $key => $flag){
  echo "\n- Sort flag = $key -\n";
  $temp_array = $array_arg;
  var_dump( ksort($temp_array,$flag, $extra_arg) );
  var_dump( $temp_array);
}

echo "Done";
?>
--EXPECTF--
*** Testing ksort() : error conditions ***

-- Testing ksort() function with Zero arguments --

Warning: ksort() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing ksort() function with more than expected no. of arguments --

- Sort flag = SORT_REGULAR -

Warning: ksort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}

- Sort flag = SORT_STRING -

Warning: ksort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}

- Sort flag = SORT_NUMERIC -

Warning: ksort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}
Done
