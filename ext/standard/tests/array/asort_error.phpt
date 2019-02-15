--TEST--
Test asort() function : error conditions
--FILE--
<?php
/* Prototype  : bool asort(array &array_arg [, int sort_flags])
 * Description: Sort an array
 * Source code: ext/standard/array.c
*/

/*
* Testing asort() function with all possible error conditions
*/

echo "*** Testing asort() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing asort() function with Zero arguments --\n";
var_dump( asort() );

//Test asort with more than the expected number of arguments
echo "\n-- Testing asort() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING, "SORT_NUMERIC" => SORT_NUMERIC);
$extra_arg = 10;

// loop through $flag_value array and setting all possible flag values
foreach($flags as $key => $flag){
  echo "\nSort flag = $key\n";
  var_dump( asort($array_arg,$flag, $extra_arg) );

  // dump the input array to ensure that it wasn't changed
  var_dump($array_arg);
}

echo "Done";
?>
--EXPECTF--
*** Testing asort() : error conditions ***

-- Testing asort() function with Zero arguments --

Warning: asort() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing asort() function with more than expected no. of arguments --

Sort flag = SORT_REGULAR

Warning: asort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

Sort flag = SORT_STRING

Warning: asort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

Sort flag = SORT_NUMERIC

Warning: asort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Done
