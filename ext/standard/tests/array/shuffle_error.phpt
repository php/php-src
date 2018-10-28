--TEST--
Test shuffle() function : error conditions
--FILE--
<?php
/* Prototype  : bool shuffle(array $array_arg)
 * Description: Randomly shuffle the contents of an array
 * Source code: ext/standard/array.c
*/

/* Test shuffle() to see that warning messages are emitted
 * when invalid number of arguments are passed to the function
*/

echo "*** Testing shuffle() : error conditions ***\n";

// zero arguments
echo "\n-- Testing shuffle() function with Zero arguments --\n";
var_dump( shuffle() );

// more than the expected number of arguments
echo "\n-- Testing shuffle() function with more than expected no. of arguments --\n";
$array_arg = array(1, "two" => 2);
$extra_arg = 10;
var_dump( shuffle($array_arg, $extra_arg) );

// printing the input array to check that it is not affected
// by above shuffle() function calls
echo "\n-- original input array --\n";
var_dump( $array_arg );

echo "Done";
?>
--EXPECTF--
*** Testing shuffle() : error conditions ***

-- Testing shuffle() function with Zero arguments --

Warning: shuffle() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing shuffle() function with more than expected no. of arguments --

Warning: shuffle() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- original input array --
array(2) {
  [0]=>
  int(1)
  ["two"]=>
  int(2)
}
Done
