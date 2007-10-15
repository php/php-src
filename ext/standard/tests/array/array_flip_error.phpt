--TEST--
Test array_flip() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_flip(array $input)
 * Description: Return array with key <-> value flipped 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_flip() : error conditions ***\n";

// Zero arguments
echo "-- Testing array_flip() function with Zero arguments --\n";
var_dump( array_flip() );

//one more than the expected number of arguments
echo "-- Testing array_flip() function with more than expected no. of arguments --\n";
$input = array(1 => 'one', 2 => 'two');
$extra_arg = 10;
var_dump( array_flip($input, $extra_arg) );

echo "Done"
?>
--EXPECTF--
*** Testing array_flip() : error conditions ***
-- Testing array_flip() function with Zero arguments --

Warning: Wrong parameter count for array_flip() in %s on line %d
NULL
-- Testing array_flip() function with more than expected no. of arguments --

Warning: Wrong parameter count for array_flip() in %s on line %d
NULL
Done
