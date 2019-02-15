--TEST--
Test array_rand() function : error conditions
--FILE--
<?php
/* Prototype  : mixed array_rand(array input [, int num_req])
 * Description: Return key/keys for random entry/entries in the array
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_rand() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_rand() function with Zero arguments --\n";
var_dump( array_rand() );

//Test array_rand with one more than the expected number of arguments
echo "\n-- Testing array_rand() function with more than expected no. of arguments --\n";
$input = array(1, 2);
$num_req = 10;
$extra_arg = 10;
var_dump( array_rand($input,$num_req, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing array_rand() : error conditions ***

-- Testing array_rand() function with Zero arguments --

Warning: array_rand() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing array_rand() function with more than expected no. of arguments --

Warning: array_rand() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
