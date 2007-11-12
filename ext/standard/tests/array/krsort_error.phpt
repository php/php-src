--TEST--
Test krsort() function : error conditions 
--FILE--
<?php
/* Prototype  : bool krsort(array &array_arg [, int asort_flags])
 * Description: Sort an array 
 * Source code: ext/standard/array.c
*/

/*
* Testing krsort() function with all possible error conditions 
*/

echo "*** Testing krsort() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing krsort() function with zero arguments --\n";
var_dump( krsort() );

//Test krsort with more than the expected number of arguments
echo "\n-- Testing krsort() function with more than expected no. of arguments --\n";
$array_arg = array(1 => 1, 2 => 2);
$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING, "SORT_NUMERIC" => SORT_NUMERIC);
$extra_arg = 10;

// loop through $flag_value array and call krsort with all possible sort flag values
foreach($flags as $key => $flag){
  echo "\n- Sort flag = $key -\n";
  $temp_array = $array_arg;
  var_dump( krsort($temp_array,$flag, $extra_arg) );
  var_dump($temp_array); 
}

echo "Done";
?>
--EXPECTF--
*** Testing krsort() : error conditions ***

-- Testing krsort() function with zero arguments --

Warning: krsort() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing krsort() function with more than expected no. of arguments --

- Sort flag = SORT_REGULAR -

Warning: krsort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}

- Sort flag = SORT_STRING -

Warning: krsort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}

- Sort flag = SORT_NUMERIC -

Warning: krsort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}
Done
