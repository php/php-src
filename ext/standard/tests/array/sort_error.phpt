--TEST--
Test sort() function : error conditions 
--FILE--
<?php
/* Prototype  : bool sort(array &array_arg [, int sort_flags])
 * Description: Sort an array 
 * Source code: ext/standard/array.c
*/

/*
* Testing sort() function with all possible error conditions 
*/

echo "*** Testing sort() : error conditions ***\n";

// zero arguments
echo "\n-- Testing sort() function with Zero arguments --\n";
var_dump( sort() );

//Test sort with more than the expected number of arguments
echo "\n-- Testing sort() function with more than expected no. of arguments --\n";
$array_arg = array(1, 2);
$flag_value = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING, "SORT_NUMERIC" => SORT_NUMERIC);
$extra_arg = 10;

// loop through $flag_value array and setting all possible flag values
foreach($flag_value as $key => $flag){
  echo "\nSort flag = $key\n";
  var_dump( sort($array_arg,$flag, $extra_arg) );
   
  // dump the input array to ensure that it wasn't changed
  var_dump($array_arg);
}

echo "Done";
?>
--EXPECTF--
*** Testing sort() : error conditions ***

-- Testing sort() function with Zero arguments --

Warning: sort() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing sort() function with more than expected no. of arguments --

Sort flag = SORT_REGULAR

Warning: sort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

Sort flag = SORT_STRING

Warning: sort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

Sort flag = SORT_NUMERIC

Warning: sort() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Done
