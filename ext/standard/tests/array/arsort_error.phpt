--TEST--
Test arsort() function : error conditions 
--FILE--
<?php
/* Prototype  : bool arsort(array &array_arg [, int sort_flags])
 * Description: Sort an array 
 * Source code: ext/standard/array.c
*/

/*
* Testing arsort() function with all possible error conditions 
*/

echo "*** Testing arsort() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing arsort() function with Zero arguments --\n";
var_dump( arsort() );

//Test arsort with more than the expected number of arguments
echo "\n-- Testing arsort() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING, "SORT_NUMERIC" => SORT_NUMERIC);
$extra_arg = 10;

// loop through $flag_value array and setting all possible flag values
foreach($flags as $key => $flag){
  echo "\nSort flag = $key\n";
  var_dump( arsort($array_arg,$flag, $extra_arg) );

  // dump the input array to ensure that it wasn't changed
  var_dump($array_arg);
}

echo "Done";
?>
--EXPECTF--
*** Testing arsort() : error conditions ***

-- Testing arsort() function with Zero arguments --

Warning: arsort() expects at least 1 parameter, 0 given in %sarsort_error.php on line %d
bool(false)

-- Testing arsort() function with more than expected no. of arguments --

Sort flag = SORT_REGULAR

Warning: arsort() expects at most 2 parameters, 3 given in %sarsort_error.php on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

Sort flag = SORT_STRING

Warning: arsort() expects at most 2 parameters, 3 given in %sarsort_error.php on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

Sort flag = SORT_NUMERIC

Warning: arsort() expects at most 2 parameters, 3 given in %sarsort_error.php on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Done